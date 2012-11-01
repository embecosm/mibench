/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: in_mad.c,v 1.35 2001/11/09 03:46:13 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <windows.h>
# include <commctrl.h>
# include <prsht.h>
# include <wininet.h>
# include <stdio.h>
# include <io.h>
# include <fcntl.h>
# include <stdarg.h>
# include <string.h>
# include <locale.h>
# include <math.h>

# ifndef M_PI
#  define M_PI  3.14159265358979323846264338328
# endif

# include "resource.h"
# include "messages.h"

# include "in2.h"
# include "mad.h"
# include "id3tag.h"
# include "xing.h"

# define PLUGIN_VERSION		VERSION ""

# define WM_WA_MPEG_EOF		(WM_USER + 2)
# define WM_MAD_SCAN_FINISHED	(WM_USER + 3)

# define GWL_MAD_LEGEND_COLOR	GWL_USERDATA
# define GWL_MAD_JSPIE_MS	(0 * 4)
# define GWL_MAD_JSPIE_MS_I	(1 * 4)
# define GWL_MAD_JSPIE_I	(2 * 4)

# define GWL_MAD_JSPIE_FRAMES	GWL_USERDATA

# define JSPIE_MS_COLOR		0x00b0b0e0L
# define JSPIE_MS_I_COLOR	0x00f0a0f0L
# define JSPIE_I_COLOR		0x00e0b0b0L
# define JSPIE_LR_COLOR		0x00d0d0d0L

# define PCM_CHUNK		576

static In_Module module;

struct input {
  enum input_type { INPUT_FILE, INPUT_STREAM } type;
  union {
    HANDLE file;
    HINTERNET stream;
  } handle;
};

enum channel {
  CHANNEL_STEREO  = 0,
  CHANNEL_MONO    = 1,
  CHANNEL_LEFT    = 2,
  CHANNEL_RIGHT   = 3,
  CHANNEL_REVERSE = 4
};

struct stats {
  int vbr;
  unsigned int bitrate;
  unsigned long frames;
  unsigned long vbr_rate;
  unsigned long clipped;
  mad_fixed_t clipping;
  unsigned long sync_errors;
  unsigned long crc_errors;
  unsigned long other_errors;
  unsigned long ms_joint;
  unsigned long i_joint;
  unsigned long ms_i_joint;
};

static struct state {
  int serial;			/* serial number */
  char path[MAX_PATH];		/* currently playing path/URL */
  struct input input;		/* input source */
  DWORD size;			/* file size in bytes */
  int length;			/* total playing time in ms */
  int bitrate;			/* average bitrate in kbps */
  int position;			/* current playing position in ms */
  int paused;			/* are we paused? */
  int seek;			/* seek target in ms, or -1 */
  int stop;			/* stop flag */
  enum channel channel;		/* channel selection */
  mad_fixed_t attenuation;	/* attenuation factor */
  int equalizer;		/* using equalizer */
  mad_fixed_t eqfactor[32];	/* equalizer settings */
  struct xing xing;		/* Xing VBR header */
  struct stats stats;		/* statistics */
} state;

# define REGISTRY_KEY		"Software\\Winamp\\MAD Plug-in"

static DWORD conf_enabled;		/* plug-in enabled? */
static char  conf_titlefmt[96];		/* title format */
static DWORD conf_channel;		/* channel selection */
static DWORD conf_priority;		/* decoder thread priority -2..+2 */
static DWORD conf_resolution;		/* bits per output sample */
static DWORD conf_autoattenuation;	/* auto clipping attenuation? */
static DWORD conf_attsensitivity;	/* auto attenuation sensitivity */
static DWORD conf_lengthcalc;		/* full (slow) length calculation? */
static DWORD conf_avgbitrate;		/* display average bitrate? */

# define DEFAULT_TITLEFMT	"%?1<%1 - >%?2<%2|%7>"

static HKEY registry = INVALID_HANDLE_VALUE;

static HANDLE decode_thread = INVALID_HANDLE_VALUE;
static HANDLE length_thread = INVALID_HANDLE_VALUE;

static HINTERNET internet = INVALID_HANDLE_VALUE;

# define DEBUG_STR(x)		MessageBox(module.hMainWindow, (x), "Debug",  \
					   MB_ICONEXCLAMATION | MB_OK)

static
void show_error(HWND owner, char *title, DWORD error, ...)
{
  UINT style;
  char str[256], *message;

  if (owner == 0)
    owner = module.hMainWindow;

  switch ((error >> 30) & 0x3) {
  case STATUS_SEVERITY_INFORMATIONAL:
    style = MB_ICONINFORMATION;
    if (title == 0)
      title = "Information";
    break;

  case STATUS_SEVERITY_WARNING:
    style = MB_ICONWARNING;
    if (title == 0)
      title = "Warning";
    break;

  case STATUS_SEVERITY_SUCCESS:
  default:
    style = 0;
    if (error & 0x40000000L)
      break;
    /* else fall through */

  case STATUS_SEVERITY_ERROR:
    style = MB_ICONERROR;
    if (title == 0)
      title = "Error";
    break;
  }

  if (error == ERROR_INTERNET_EXTENDED_ERROR) {
    DWORD size;

    size = sizeof(str);
    if (InternetGetLastResponseInfo(&error, str, &size) == TRUE)
      message = str;
    else {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
	message = LocalAlloc(0, size);
	if (message &&
	    InternetGetLastResponseInfo(&error, message, &size) == FALSE)
	  message = LocalFree(message);
      }
      else
	message = 0;
    }
  }
  else {
    va_list args;

    va_start(args, error);

    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
		      FORMAT_MESSAGE_FROM_HMODULE |
		      FORMAT_MESSAGE_ALLOCATE_BUFFER,
		      module.hDllInstance, error, 0,
		      (LPTSTR) &message, 0, &args) == 0)
      message = 0;

    va_end(args);
  }

  MessageBox(owner, message ? message : "An unknown error occurred.",
	     title, style | MB_OK);

  if (message != str)
    LocalFree(message);
}

static
void draw_ellipse(HDC dc, RECT const *area, COLORREF color)
{
  HBRUSH brush, old_brush;

  brush = CreateSolidBrush(color);
  old_brush = SelectObject(dc, brush);

  Ellipse(dc, area->left, area->top, area->right, area->bottom);

  SelectObject(dc, old_brush);
  DeleteObject(brush);
}

static
void draw_wedge(HDC dc, RECT const *area,
		double angle_start, double angle_stop, COLORREF color)
{
  int midx, midy, xfact, yfact;
  int radial1x, radial1y, radial2x, radial2y;

  midx  = (area->right  + area->left) / 2;
  midy  = (area->bottom + area->top)  / 2;
  xfact = (area->right  - area->left) / 2;
  yfact = (area->bottom - area->top)  / 2;

  radial1x = midx + xfact *  cos(angle_start) + 0.5;
  radial1y = midy + yfact * -sin(angle_start) + 0.5;
  radial2x = midx + xfact *  cos(angle_stop)  + 0.5;
  radial2y = midy + yfact * -sin(angle_stop)  + 0.5;

  if (angle_stop - angle_start >= 2 * M_PI ||
      (abs(radial1x - radial2x) < 2 && abs(radial1y - radial2y) < 2)) {
    if (angle_stop - angle_start > M_PI)
      draw_ellipse(dc, area, color);

    if (angle_stop - angle_start < 2 * M_PI) {
      MoveToEx(dc, midx, midy, 0);
      LineTo(dc, radial1x, radial1y);
    }
  }
  else {
    HBRUSH brush, old_brush;

    brush = CreateSolidBrush(color);
    old_brush = SelectObject(dc, brush);

    Pie(dc, area->left, area->top, area->right, area->bottom,
	radial1x, radial1y, radial2x, radial2y);

    SelectObject(dc, old_brush);
    DeleteObject(brush);
  }
}

static CALLBACK
LRESULT pie_wclass(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message) {
  case WM_PAINT:
    {
      HDC dc;
      PAINTSTRUCT ps;
      RECT cr;
      unsigned long ms_joint, ms_i_joint, i_joint, frames;

      dc = BeginPaint(window, &ps);

      GetClientRect(window, &cr);

      ms_joint   = GetWindowLong(window, GWL_MAD_JSPIE_MS);
      ms_i_joint = GetWindowLong(window, GWL_MAD_JSPIE_MS_I);
      i_joint    = GetWindowLong(window, GWL_MAD_JSPIE_I);
      frames     = GetWindowLong(window, GWL_MAD_JSPIE_FRAMES);

      if ((ms_joint | ms_i_joint | i_joint) == 0 || frames == 0)
	draw_ellipse(dc, &cr, JSPIE_LR_COLOR);
      else {
	double angle1 = 0, angle2 = 0;

	if (ms_joint) {
	  angle2 += 2 * M_PI * ms_joint / frames;
	  draw_wedge(dc, &cr, angle1, angle2, JSPIE_MS_COLOR);

	  angle1 = angle2;
	}

	if (ms_i_joint) {
	  angle2 += 2 * M_PI * ms_i_joint / frames;
	  draw_wedge(dc, &cr, angle1, angle2, JSPIE_MS_I_COLOR);

	  angle1 = angle2;
	}

	if (i_joint) {
	  angle2 += 2 * M_PI * i_joint / frames;
	  draw_wedge(dc, &cr, angle1, angle2, JSPIE_I_COLOR);

	  angle1 = angle2;
	}

	if (angle1 < 2 * M_PI)
	  draw_wedge(dc, &cr, angle1, 2 * M_PI, JSPIE_LR_COLOR);
      }

      EndPaint(window, &ps);
    }

    return 0;

  default:
    return DefWindowProc(window, message, wparam, lparam);
  }
}

static CALLBACK
LRESULT legend_wclass(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message) {
  case WM_PAINT:
    {
      HDC dc;
      PAINTSTRUCT ps;
      RECT cr;
      HBRUSH brush;

      dc = BeginPaint(window, &ps);

      GetClientRect(window, &cr);

      brush = CreateSolidBrush(GetWindowLong(window, GWL_MAD_LEGEND_COLOR));
      FillRect(dc, &cr, brush);
      DeleteObject(brush);

      FrameRect(dc, &cr, GetStockObject(BLACK_BRUSH));

      EndPaint(window, &ps);

      return 0;
    }

  default:
    return DefWindowProc(window, message, wparam, lparam);
  }
}

static
void ui_init(void)
{
  WNDCLASSEX wclass;

  InitCommonControls();

  wclass.cbSize        = sizeof(wclass);
  wclass.style         = 0;
  wclass.lpfnWndProc   = pie_wclass;
  wclass.cbClsExtra    = 0;
  wclass.cbWndExtra    = 3 * 4;
  wclass.hInstance     = module.hDllInstance;
  wclass.hIcon         = 0;
  wclass.hCursor       = 0;
  wclass.hbrBackground = (HBRUSH) COLOR_WINDOW;
  wclass.lpszMenuName  = 0;
  wclass.lpszClassName = "MAD_JSPIE";
  wclass.hIconSm       = 0;

  RegisterClassEx(&wclass);

  wclass.lpfnWndProc   = legend_wclass;
  wclass.cbWndExtra    = 0;
  wclass.lpszClassName = "MAD_LEGEND";

  RegisterClassEx(&wclass);
}

static
void apply_config(void)
{
  module.FileExtensions = conf_enabled ?
# if 1
    "MP3;MP2;MP1\0" "MPEG Audio Files (*.MP3;*.MP2;*.MP1)\0"
# else
    "MP3\0" "MPEG Audio Layer III files (*.MP3)\0"
    "MP2\0" "MPEG Audio Layer II files (*.MP2)\0"
    "MP1\0" "MPEG Audio Layer I files (*.MP1)\0"
# endif
    : "";
}

static
int peek_registry(char const *name, DWORD type, void *ptr, DWORD size)
{
  DWORD reg_type;

  if (registry == INVALID_HANDLE_VALUE ||
      RegQueryValueEx(registry, name, 0,
		      &reg_type, ptr, &size) != ERROR_SUCCESS ||
      reg_type != type)
    return -1;

  return 0;
}

static
int poke_registry(char const *name, DWORD type, void *ptr, DWORD size)
{
  if (registry == INVALID_HANDLE_VALUE ||
      RegSetValueEx(registry, name, 0, type, ptr, size) != ERROR_SUCCESS)
    return -1;

  return 0;
}

# define LOAD_CONF_DWORD(name, default)  \
    (peek_registry((#name), REG_DWORD,  \
		    &(conf_##name), sizeof(conf_##name)) == -1 ?  \
     ((conf_##name) = (default)) : (conf_##name))

# define LOAD_CONF_SZ(name, default)  \
    (peek_registry((#name), REG_SZ,  \
		    (conf_##name), sizeof(conf_##name)) == -1 ?  \
     strcpy((conf_##name), (default)) : (conf_##name))

# define SAVE_CONF_DWORD(name)  \
    (poke_registry((#name), REG_DWORD, &(conf_##name), sizeof(conf_##name)))

# define SAVE_CONF_SZ(name)  \
    (poke_registry((#name), REG_SZ, (conf_##name), sizeof(conf_##name)))

static
void do_init(void)
{
  ui_init();

# if defined(OUR_EQ)
  /* tell Winamp we will handle the equalization */
  module.UsesOutputPlug |= 2;
# endif

  if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY,
		     0, "", REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0,
		     &registry, 0) != ERROR_SUCCESS)
    registry = INVALID_HANDLE_VALUE;

  LOAD_CONF_DWORD(enabled, 1);
  LOAD_CONF_DWORD(channel, CHANNEL_STEREO);
  LOAD_CONF_DWORD(priority, 0);
  LOAD_CONF_DWORD(resolution, 16);
  LOAD_CONF_DWORD(autoattenuation, 0);
  LOAD_CONF_DWORD(attsensitivity, MAD_F(0x0c000000));
  LOAD_CONF_DWORD(lengthcalc, 0);
  LOAD_CONF_DWORD(avgbitrate, 1);

  LOAD_CONF_SZ(titlefmt, DEFAULT_TITLEFMT);

  if (conf_resolution !=  8 && conf_resolution != 16 &&
      conf_resolution != 24 && conf_resolution != 32)
    conf_resolution = 16;

  apply_config();
}

static
void do_quit(void)
{
  if (registry != INVALID_HANDLE_VALUE) {
    RegCloseKey(registry);
    registry = INVALID_HANDLE_VALUE;
  }

  if (internet != INVALID_HANDLE_VALUE) {
    InternetCloseHandle(internet);
    internet = INVALID_HANDLE_VALUE;
  }
}

static CALLBACK
BOOL config_dialog(HWND dialog, UINT message,
		   WPARAM wparam, LPARAM lparam)
{
  int which;

  switch (message) {
  case WM_INITDIALOG:
    if (conf_enabled)
      CheckDlgButton(dialog, IDC_CONF_ENABLED, BST_CHECKED);

    PostMessage(dialog, WM_COMMAND,
		MAKELONG(IDC_CONF_ENABLED, BN_CLICKED),
		(LPARAM) GetDlgItem(dialog, IDC_CONF_ENABLED));

    /* Title Format */

    SendDlgItemMessage(dialog, IDC_TITLE_FORMAT, EM_SETLIMITTEXT,
		       sizeof(conf_titlefmt) - 1, 0);
    SetDlgItemText(dialog, IDC_TITLE_FORMAT, conf_titlefmt);

    /* Channels */

    switch (conf_channel) {
    case CHANNEL_STEREO:  which = IDC_CHAN_STEREO; break;
    case CHANNEL_MONO:    which = IDC_CHAN_MONO;   break;
    case CHANNEL_LEFT:    which = IDC_CHAN_LEFT;   break;
    case CHANNEL_RIGHT:   which = IDC_CHAN_RIGHT;  break;
    case CHANNEL_REVERSE: which = IDC_CHAN_REVERSE; break;
    default:
      which = 0;
    }
    if (which)
      CheckRadioButton(dialog, IDC_CHAN_STEREO, IDC_CHAN_REVERSE, which);

    /* Priority */

    SendDlgItemMessage(dialog, IDC_PRIO_SLIDER,
		       TBM_SETRANGE, FALSE, MAKELONG(2 - 2, 2 + 2));
    SendDlgItemMessage(dialog, IDC_PRIO_SLIDER,
		       TBM_SETPOS, TRUE, 2 - conf_priority);

    /* Resolution */

    switch (conf_resolution) {
    case  8: which = IDC_RES_8BITS;  break;
    case 16: which = IDC_RES_16BITS; break;
    case 24: which = IDC_RES_24BITS; break;
    case 32: which = IDC_RES_32BITS; break;
    default:
      which = 0;
    }
    if (which)
      CheckRadioButton(dialog, IDC_RES_8BITS, IDC_RES_32BITS, which);

    /* Output */

    if (conf_autoattenuation)
      CheckDlgButton(dialog, IDC_OUT_AUTOATTENUATION, BST_CHECKED);

    SendDlgItemMessage(dialog, IDC_OUT_SENSITIVITY,
		       TBM_SETRANGE, FALSE, MAKELONG(1, 8));
    SendDlgItemMessage(dialog, IDC_OUT_SENSITIVITY,
		       TBM_SETPOS, TRUE,
		       conf_attsensitivity / MAD_F(0x02000000));

    PostMessage(dialog, WM_COMMAND,
		MAKELONG(IDC_OUT_AUTOATTENUATION, BN_CLICKED),
		(LPARAM) GetDlgItem(dialog, IDC_OUT_AUTOATTENUATION));

    /* Miscellaneous */

    if (!conf_lengthcalc)
      CheckDlgButton(dialog, IDC_MISC_FASTTIMECALC, BST_CHECKED);
    if (conf_avgbitrate)
      CheckDlgButton(dialog, IDC_MISC_AVGBITRATE, BST_CHECKED);

    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wparam)) {
    case IDC_OUT_AUTOATTENUATION:
      {
	BOOL state;

	state =
	  IsDlgButtonChecked(dialog, IDC_OUT_AUTOATTENUATION) == BST_CHECKED ?
	  TRUE : FALSE;

	EnableWindow(GetDlgItem(dialog, IDC_OUT_SENSITIVITY), state);
	EnableWindow(GetDlgItem(dialog, IDC_OUT_LABEL1),      state);
	EnableWindow(GetDlgItem(dialog, IDC_OUT_LABEL2),      state);
	EnableWindow(GetDlgItem(dialog, IDC_OUT_LABEL3),      state);
      }
      break;

    case IDOK:
      conf_enabled =
	(IsDlgButtonChecked(dialog, IDC_CONF_ENABLED) == BST_CHECKED);

      /* Title Format */

      if (SendDlgItemMessage(dialog, IDC_TITLE_FORMAT,
			     WM_GETTEXTLENGTH, 0, 0) == 0)
	strcpy(conf_titlefmt, DEFAULT_TITLEFMT);
      else {
	GetDlgItemText(dialog, IDC_TITLE_FORMAT,
		       conf_titlefmt, sizeof(conf_titlefmt));
      }

      /* Channels */

      if      (IsDlgButtonChecked(dialog, IDC_CHAN_STEREO)  == BST_CHECKED)
	conf_channel = CHANNEL_STEREO;
      else if (IsDlgButtonChecked(dialog, IDC_CHAN_MONO)    == BST_CHECKED)
	conf_channel = CHANNEL_MONO;
      else if (IsDlgButtonChecked(dialog, IDC_CHAN_LEFT)    == BST_CHECKED)
	conf_channel = CHANNEL_LEFT;
      else if (IsDlgButtonChecked(dialog, IDC_CHAN_RIGHT)   == BST_CHECKED)
	conf_channel = CHANNEL_RIGHT;
      else if (IsDlgButtonChecked(dialog, IDC_CHAN_REVERSE) == BST_CHECKED)
	conf_channel = CHANNEL_REVERSE;

      /* Priority */

      conf_priority = 2 -
	SendDlgItemMessage(dialog, IDC_PRIO_SLIDER, TBM_GETPOS, 0, 0);

      /* Resolution */

      if      (IsDlgButtonChecked(dialog, IDC_RES_8BITS)  == BST_CHECKED)
	conf_resolution =  8;
      else if (IsDlgButtonChecked(dialog, IDC_RES_16BITS) == BST_CHECKED)
	conf_resolution = 16;
      else if (IsDlgButtonChecked(dialog, IDC_RES_24BITS) == BST_CHECKED)
	conf_resolution = 24;
      else if (IsDlgButtonChecked(dialog, IDC_RES_32BITS) == BST_CHECKED)
	conf_resolution = 32;

      /* Output */

      conf_autoattenuation =
	(IsDlgButtonChecked(dialog, IDC_OUT_AUTOATTENUATION) == BST_CHECKED);

      conf_attsensitivity = MAD_F(0x02000000) *
	SendDlgItemMessage(dialog, IDC_OUT_SENSITIVITY, TBM_GETPOS, 0, 0);

      /* Miscellaneous */

      conf_lengthcalc =
	(IsDlgButtonChecked(dialog, IDC_MISC_FASTTIMECALC) != BST_CHECKED);
      conf_avgbitrate =
	(IsDlgButtonChecked(dialog, IDC_MISC_AVGBITRATE) == BST_CHECKED);

      /* fall through */

    case IDCANCEL:
      EndDialog(dialog, wparam);
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static
void show_config(HWND parent)
{
  if (DialogBox(module.hDllInstance, MAKEINTRESOURCE(IDD_CONFIG),
		parent, config_dialog) == IDOK) {
    SAVE_CONF_DWORD(enabled);
    SAVE_CONF_DWORD(channel);
    SAVE_CONF_DWORD(priority);
    SAVE_CONF_DWORD(resolution);
    SAVE_CONF_DWORD(autoattenuation);
    SAVE_CONF_DWORD(attsensitivity);
    SAVE_CONF_DWORD(lengthcalc);
    SAVE_CONF_DWORD(avgbitrate);

    SAVE_CONF_SZ(titlefmt);

    apply_config();
  }
}

static
void show_about(HWND parent)
{
  char const about[] =
    "MPEG Audio Decoder version " MAD_VERSION "\n"
    "ID3 Tag version " ID3_VERSION "\n"
    "Winamp plug-in version " PLUGIN_VERSION "\n\n"

    "Copyright \xa9 " MAD_PUBLISHYEAR " " MAD_AUTHOR "\n\n"

    "This program is free software; you can redistribute it and/or modify it\n"
    "under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or (at\n"
    "your option) any later version.\n\n"

    "This program is distributed in the hope that it will be useful, but\n"
    "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
    "See the GNU General Public License for more details.";

  MessageBox(parent, about, "About MAD Plug-in", MB_ICONINFORMATION | MB_OK);
}

static
int is_stream(char *path)
{
  return
    strncmp(path, "http://", 7) == 0 ||
    strncmp(path, "ftp://",  6) == 0;
}

static
int is_ourfile(char *path)
{
  return conf_enabled && is_stream(path);
}

static
void input_init(struct input *input, enum input_type type, HANDLE handle)
{
  input->type = type;

  switch (type) {
  case INPUT_FILE:
    input->handle.file = handle;
    break;

  case INPUT_STREAM:
    input->handle.stream = handle;
    break;
  }
}

static
DWORD input_read(struct input *input, unsigned char *buffer, DWORD bytes)
{
  switch (input->type) {
  case INPUT_FILE:
    if (ReadFile(input->handle.file, buffer, bytes, &bytes, 0) == FALSE)
      bytes = -1;
    break;

  case INPUT_STREAM:
    if (InternetReadFile(input->handle.stream, buffer, bytes, &bytes) == FALSE)
      bytes = -1;
    break;

  default:
    bytes = -1;
  }

  return bytes;
}

static
DWORD input_seek(struct input *input, DWORD position, DWORD method)
{
  switch (input->type) {
  case INPUT_FILE:
    return SetFilePointer(input->handle.file, position, 0, method);

  case INPUT_STREAM:
  default:
    return -1;
  }
}

static
void input_close(struct input *input)
{
  switch (input->type) {
  case INPUT_FILE:
    if (input->handle.file != INVALID_HANDLE_VALUE) {
      CloseHandle(input->handle.file);
      input->handle.file = INVALID_HANDLE_VALUE;
    }
    break;

  case INPUT_STREAM:
    if (input->handle.stream != INVALID_HANDLE_VALUE) {
      InternetCloseHandle(input->handle.stream);
      input->handle.stream = INVALID_HANDLE_VALUE;
    }
    break;
  }
}

# if 0
static
int input_skip(struct input *input, unsigned long count)
{
  if (input_seek(input, count, FILE_CURRENT) == -1) {
    unsigned char sink[256];
    unsigned long len;

    while (count) {
      len = input_read(input, sink,
		       count < sizeof(sink) ? count : sizeof(sink));

      if (len == -1)
	return -1;
      else if (len == 0)
	return 0;

      count -= len;
    }
  }

  return 0;
}
# endif

static
void mono_filter(struct mad_frame *frame)
{
  if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL) {
    unsigned int ns, s, sb;
    mad_fixed_t left, right;

    ns = MAD_NSBSAMPLES(&frame->header);

    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	left  = frame->sbsample[0][s][sb];
	right = frame->sbsample[1][s][sb];

	frame->sbsample[0][s][sb] = (left + right) / 2;
	/* frame->sbsample[1][s][sb] = 0; */
      }
    }

    frame->header.mode = MAD_MODE_SINGLE_CHANNEL;
  }
}

static
void attenuate_filter(struct mad_frame *frame, mad_fixed_t scalefactor)
{
  unsigned int nch, ch, ns, s, sb;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (ch = 0; ch < nch; ++ch) {
    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	frame->sbsample[ch][s][sb] =
	  mad_f_mul(frame->sbsample[ch][s][sb], scalefactor);
      }
    }
  }
}

# if defined(OUR_EQ)
static
void equalizer_filter(struct mad_frame *frame, mad_fixed_t eqfactor[32])
{
  unsigned int nch, ch, ns, s, sb;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (ch = 0; ch < nch; ++ch) {
    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	frame->sbsample[ch][s][sb] =
	  mad_f_mul(frame->sbsample[ch][s][sb], eqfactor[sb]);
      }
    }
  }
}
# endif

# define NEW_DITHER
# ifdef NEW_DITHER
struct dither {
  mad_fixed_t error[3];
  mad_fixed_t random;
};

/*
 * NAME:	prng()
 * DESCRIPTION:	32-bit pseudo-random number generator
 */
static inline
unsigned long prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

static inline
signed long linear_dither(unsigned int bits, mad_fixed_t sample,
			  struct dither *dither, unsigned long *clipped,
			  mad_fixed_t *clipping)
{
  unsigned int scalebits;
  mad_fixed_t output, mask, random;

  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  /* noise shape */
  sample += dither->error[0] - dither->error[1] + dither->error[2];

  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;

  /* bias */
  output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

  scalebits = MAD_F_FRACBITS + 1 - bits;
  mask = (1L << scalebits) - 1;

  /* dither */
  random  = prng(dither->random);
  output += (random & mask) - (dither->random & mask);

  dither->random = random;

  /* clip */
  if (output > MAX) {
    ++*clipped;
    if (output - MAX > *clipping)
      *clipping = output - MAX;

    output = MAX;

    if (sample > MAX)
      sample = MAX;
  }
  else if (output < MIN) {
    ++*clipped;
    if (MIN - output > *clipping)
      *clipping = MIN - output;

    output = MIN;

    if (sample < MIN)
      sample = MIN;
  }

  /* quantize */
  output &= ~mask;

  /* error feedback */
  dither->error[0] = sample - output;

  /* scale */
  return output >> scalebits;
}
# else
static inline
signed long linear_dither(unsigned int bits, mad_fixed_t sample,
			  mad_fixed_t *error, unsigned long *clipped,
			  mad_fixed_t *clipping)
{
  mad_fixed_t quantized, check;

  /* dither */
  sample += *error;

  /* clip */
  quantized = sample;
  check = (sample >> MAD_F_FRACBITS) + 1;
  if (check & ~1) {
    if (sample >= MAD_F_ONE) {
      quantized = MAD_F_ONE - 1;
      ++*clipped;
      if (sample - quantized > *clipping &&
	  mad_f_abs(*error) < (MAD_F_ONE >> (MAD_F_FRACBITS + 1 - bits)))
	*clipping = sample - quantized;
    }
    else if (sample < -MAD_F_ONE) {
      quantized = -MAD_F_ONE;
      ++*clipped;
      if (quantized - sample > *clipping &&
	  mad_f_abs(*error) < (MAD_F_ONE >> (MAD_F_FRACBITS + 1 - bits)))
	*clipping = quantized - sample;
    }
  }

  /* quantize */
  quantized &= ~((1L << (MAD_F_FRACBITS + 1 - bits)) - 1);

  /* error */
  *error = sample - quantized;

  /* scale */
  return quantized >> (MAD_F_FRACBITS + 1 - bits);
}
# endif

static
unsigned int pack_pcm(unsigned char *data, unsigned int nsamples,
		      mad_fixed_t const *left, mad_fixed_t const *right,
		      int resolution, unsigned long *clipped,
		      mad_fixed_t *clipping)
{
# ifdef NEW_DITHER
  static struct dither left_dither, right_dither;
# else
  static mad_fixed_t left_dither, right_dither;
# endif
  unsigned char const *start;
  register signed long sample0, sample1;
  int effective, bytes;

  start     = data;
  effective = (resolution > 24) ? 24 : resolution;
  bytes     = resolution / 8;

  if (right) {  /* stereo */
    while (nsamples--) {
      sample0 = linear_dither(effective, *left++, &left_dither,
			      clipped, clipping);
      sample1 = linear_dither(effective, *right++, &right_dither,
			      clipped, clipping);

      switch (resolution) {
      case 8:
	data[0] = sample0 ^ 0x80;
	data[1] = sample1 ^ 0x80;
	break;

      case 32:
	sample0 <<= 8;
	sample1 <<= 8;
	data[        3] = sample0 >> 24;
	data[bytes + 3] = sample1 >> 24;
      case 24:
	data[        2] = sample0 >> 16;
	data[bytes + 2] = sample1 >> 16;
      case 16:
	data[        1] = sample0 >>  8;
	data[bytes + 1] = sample1 >>  8;
	data[        0] = sample0 >>  0;
	data[bytes + 0] = sample1 >>  0;
      }

      data += bytes * 2;
    }
  }
  else {  /* mono */
    while (nsamples--) {
      sample0 = linear_dither(effective, *left++, &left_dither,
			      clipped, clipping);

      switch (resolution) {
      case 8:
	data[0] = sample0 ^ 0x80;
	break;

      case 32:
	sample0 <<= 8;
	data[3] = sample0 >> 24;
      case 24:
	data[2] = sample0 >> 16;
      case 16:
	data[1] = sample0 >>  8;
	data[0] = sample0 >>  0;
      }
      data += bytes;
    }
  }

  return data - start;
}

static
void stats_init(struct stats *stats)
{
  stats->vbr          = 0;
  stats->bitrate      = 0;
  stats->frames       = 0;
  stats->vbr_rate     = 0;
  stats->clipped      = 0;
  stats->clipping     = 0;
  stats->sync_errors  = 0;
  stats->crc_errors   = 0;
  stats->other_errors = 0;
  stats->ms_joint     = 0;
  stats->i_joint      = 0;
  stats->ms_i_joint   = 0;
}

static
int vbr_update(struct stats *stats, unsigned long bitrate)
{
  bitrate /= 1000;
  stats->vbr_rate += bitrate;

  stats->vbr += (stats->bitrate && stats->bitrate != bitrate) ? 128 : -1;
  if (stats->vbr < 0)
    stats->vbr = 0;
  else if (stats->vbr > 512)
    stats->vbr = 512;

  stats->bitrate = bitrate;

  return (stats->vbr && stats->frames > 0) ?
    ((stats->vbr_rate * 2) / stats->frames + 1) / 2 : stats->bitrate;
}

static
int do_error(struct mad_stream *stream, struct mad_frame *frame,
	     struct input *input, int *last_error, struct stats *stats)
{
  int do_continue = 1;
  signed long tagsize;

  switch (stream->error) {
  case MAD_ERROR_BADCRC:
    if (stats)
      ++stats->crc_errors;

    if (last_error) {
      if (*last_error) {
	if (frame)
	  mad_frame_mute(frame);
      }
      else
	*last_error = 1;
    }

    do_continue = 0;
    break;

  case MAD_ERROR_LOSTSYNC:
    tagsize = id3_tag_query(stream->this_frame,
			    stream->bufend - stream->this_frame);
    if (tagsize > 0)
      mad_stream_skip(stream, tagsize);
    else if (stats)
      ++stats->sync_errors;

    break;

  default:
    if (stats)
      ++stats->other_errors;

    break;
  }

  return do_continue;
}

static
void do_vis(char *data, int nch, int resolution, int position)
{
  static char vis_buffer[PCM_CHUNK * 2];
  char *ptr;
  int size, count;

  /*
   * Winamp visuals may have problems accepting sample sizes larger than
   * 16 bits, so we reduce the sample size here if necessary.
   */

  switch (resolution) {
  case 32:
  case 24:
    size  = resolution / 8;
    count = PCM_CHUNK * nch;

    ptr = vis_buffer;
    while (count--) {
      data += size;
      *ptr++ = data[-1] ^ 0x80;
    }

    data = vis_buffer;
    resolution = 8;

  case 16:
  case 8:
  default:
    module.SAAddPCMData(data,  nch, resolution, position);
    module.VSAAddPCMData(data, nch, resolution, position);
  }
}

static
DWORD WINAPI run_decode_thread(void *param)
{
  struct state *state = param;
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;

  int resolution = conf_resolution;

  unsigned char *input_buffer;
  static unsigned char output_buffer[(575 + 1152) * 4 * 2];
  unsigned int input_size, input_length = 0, output_length = 0;

  mad_timer_t timer, duration;
  int avgbitrate, bitrate, last_bitrate = 0, seek_skip = 0, last_error = 0;
  int eof = 0;

  input_size = 40000 /* 1 s at 320 kbps */ * 5;
  input_buffer = LocalAlloc(0, input_size);
  if (input_buffer == 0) {
    show_error(0, "Out of Memory", IDS_ERR_NOMEM);
    return 1;
  }

  mad_stream_init(&stream);
  mad_frame_init(&frame);
  mad_synth_init(&synth);

  timer = duration = mad_timer_zero;

  stats_init(&state->stats);

  while (!eof) {
    if (input_length < input_size / 4) {
      DWORD bytes;

      bytes = input_read(&state->input, input_buffer + input_length,
			 input_size - input_length);
      if (bytes == 0) {
	eof = 1;

	while (bytes < MAD_BUFFER_GUARD)
	  input_buffer[input_length + bytes++] = 0;
      }
      else if (bytes == -1) {
	show_error(0, "Error Reading Data", GetLastError());
	break;
      }

      input_length += bytes;
    }

    mad_stream_buffer(&stream, input_buffer, input_length);

    if (seek_skip) {
      int skip;

      skip = 2;
      do {
	if (mad_frame_decode(&frame, &stream) == 0) {
	  mad_timer_add(&timer, frame.header.duration);
	  if (--skip == 0)
	    mad_synth_frame(&synth, &frame);
	}
	else if (!MAD_RECOVERABLE(stream.error))
	  break;
      }
      while (skip);

      module.outMod->Flush(seek_skip);

      seek_skip = 0;
    }

    while (!state->stop) {
      char *output_ptr;
      int nch, bytes;
      mad_fixed_t const *ch1, *ch2;

      if (state->seek != -1 && state->length > 0) {
	int new_position;

	if (state->seek < 0)
	  new_position = (double) state->length * -state->seek / 1000;
	else
	  new_position = state->seek;

	state->position = new_position;
	state->seek     = -1;

	if (input_seek(&state->input, (double) new_position *
		       state->size / state->length, FILE_BEGIN) != -1) {
	  mad_timer_set(&timer, 0, new_position, 1000);

	  mad_frame_mute(&frame);
	  mad_synth_mute(&synth);

	  input_length = stream.next_frame - &input_buffer[0];
	  stream.error = MAD_ERROR_BUFLEN;
	  stream.sync  = 0;

	  if (new_position > 0)
	    seek_skip = new_position;
	  else
	    module.outMod->Flush(0);

	  break;
	}
      }

      if (mad_frame_decode(&frame, &stream) == -1) {
	if (!MAD_RECOVERABLE(stream.error))
	  break;

	module.SetInfo(-1, -1, -1, 0);
	last_bitrate = 0;

	if (do_error(&stream, &frame, &state->input,
		     &last_error, &state->stats))
	  continue;
      }
      else
	last_error = 0;

      ++state->stats.frames;

      switch (frame.header.flags & (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO)) {
      case MAD_FLAG_MS_STEREO:
	++state->stats.ms_joint;
	break;

      case MAD_FLAG_I_STEREO:
	++state->stats.i_joint;
	break;

      case (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO):
	++state->stats.ms_i_joint;
	break;
      }

      avgbitrate = state->bitrate ?
	state->bitrate : vbr_update(&state->stats, frame.header.bitrate);

      bitrate = conf_avgbitrate ? avgbitrate : frame.header.bitrate / 1000;

      /*
       * Winamp exhibits odd behavior if SetInfo() is called again with the
       * same bitrate as the last call. A symptom is a failure of the song
       * title to scroll in the task bar (if that option has been enabled.)
       */

      if (bitrate != last_bitrate) {
	module.SetInfo(bitrate, -1, -1, 1);
	last_bitrate = bitrate;
      }

      if (state->channel == CHANNEL_MONO)
	mono_filter(&frame);
      if (state->attenuation != MAD_F_ONE)
	attenuate_filter(&frame, state->attenuation);
# if defined(OUR_EQ)
      if (state->equalizer)
	equalizer_filter(&frame, state->eqfactor);
# endif

      mad_synth_frame(&synth, &frame);

      nch = synth.pcm.channels;
      ch1 = synth.pcm.samples[0];
      ch2 = synth.pcm.samples[1];

      if (nch == 1)
	ch2 = 0;
      else {
	switch (state->channel) {
	case CHANNEL_RIGHT:
	  ch1 = ch2;
	case CHANNEL_LEFT:
	  ch2 = 0;
	  nch = 1;
	case CHANNEL_MONO:
	case CHANNEL_STEREO:
	  break;

	case CHANNEL_REVERSE:
	  ch1 = ch2;
	  ch2 = synth.pcm.samples[0];
	  break;
	}
      }

      output_length +=
	pack_pcm(output_buffer + output_length,
		 synth.pcm.length, ch1, ch2, resolution,
		 &state->stats.clipped, &state->stats.clipping);

      if (conf_autoattenuation && state->stats.clipping > 0) {
	state->attenuation =
	  mad_f_tofixed(mad_f_todouble(state->attenuation) /
			mad_f_todouble(MAD_F_ONE +
				       mad_f_mul(state->stats.clipping,
						 conf_attsensitivity)));
	state->stats.clipping = 0;
      }

      output_ptr = output_buffer;

      mad_timer_set(&duration, 0, PCM_CHUNK, synth.pcm.samplerate);

      bytes = PCM_CHUNK * (resolution / 8) * nch;

      while (output_length >= bytes) {
	int dsp;

	dsp = module.dsp_isactive();

	while (!(state->stop || (state->seek != -1 && state->length >= 0)) &&
	       module.outMod->CanWrite() < (dsp ? bytes * 2 : bytes))
	  Sleep(20);

	if (state->stop || (state->seek != -1 && state->length >= 0))
	  break;

	do_vis(output_ptr, nch, resolution, state->position);

	mad_timer_add(&timer, duration);
	state->position = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);

	if (dsp) {
	  static unsigned char dsp_buffer[sizeof(output_buffer) * 2];
	  int nsamples;

	  memcpy(dsp_buffer, output_ptr, bytes);

	  nsamples = module.dsp_dosamples((short *) dsp_buffer, PCM_CHUNK,
					  resolution, nch,
					  synth.pcm.samplerate);
	  module.outMod->Write(dsp_buffer, nsamples * (resolution / 8) * nch);
	}
	else
	  module.outMod->Write(output_ptr, bytes);

	output_ptr    += bytes;
	output_length -= bytes;
      }

      if (state->seek != -1 && state->length >= 0)
	output_length = 0;
      else if (output_length)
	memmove(output_buffer, output_ptr, output_length);
    }

    if (state->stop || stream.error != MAD_ERROR_BUFLEN)
      break;

    memmove(input_buffer, stream.next_frame,
	    input_length = &input_buffer[input_length] - stream.next_frame);
  }

  mad_synth_finish(&synth);
  mad_frame_finish(&frame);
  mad_stream_finish(&stream);

  while (!state->stop && module.outMod->IsPlaying()) {
    Sleep(10);

    module.outMod->CanWrite();  /* ?? */
  }

  if (!state->stop)
    PostMessage(module.hMainWindow, WM_WA_MPEG_EOF, 0, 0);

  LocalFree(input_buffer);

  return 0;
}

static
int scan_header(struct input *input, struct mad_header *header,
		struct xing *xing)
{
  struct mad_stream stream;
  struct mad_frame frame;
  unsigned char buffer[8192];
  unsigned int buflen = 0;
  int count = 0, result = 0;

  mad_stream_init(&stream);
  mad_frame_init(&frame);

  if (xing)
    xing->flags = 0;

  while (1) {
    if (buflen < sizeof(buffer)) {
      DWORD bytes;

      bytes = input_read(input, buffer + buflen, sizeof(buffer) - buflen);
      if (bytes <= 0) {
	if (bytes == -1)
	  result = -1;
	break;
      }

      buflen += bytes;
    }

    mad_stream_buffer(&stream, buffer, buflen);

    while (1) {
      if (mad_frame_decode(&frame, &stream) == -1) {
	if (!MAD_RECOVERABLE(stream.error))
	  break;

	if (do_error(&stream, 0, input, 0, 0))
	  continue;
      }

      if (count++ ||
	  (xing && xing_parse(xing, stream.anc_ptr, stream.anc_bitlen) == -1))
	break;
    }

    if (count || stream.error != MAD_ERROR_BUFLEN)
      break;

    memmove(buffer, stream.next_frame,
	    buflen = &buffer[buflen] - stream.next_frame);
  }

  if (count) {
    if (header)
      *header = frame.header;
  }
  else
    result = -1;

  mad_frame_finish(&frame);
  mad_stream_finish(&stream);

  return result;
}

static
void scan_file(struct input *input, int *stop_flag,
	       int *length, int *bitrate)
{
  struct mad_stream stream;
  struct mad_header header;
  mad_timer_t timer;
  unsigned char buffer[8192];
  unsigned int buflen = 0;
  struct stats stats;
  int avgbitrate = 0;

  mad_stream_init(&stream);
  mad_header_init(&header);

  timer = mad_timer_zero;

  stats_init(&stats);

  while (1) {
    if (buflen < sizeof(buffer)) {
      DWORD bytes;

      bytes = input_read(input, buffer + buflen, sizeof(buffer) - buflen);
      if (bytes <= 0)
	break;

      buflen += bytes;
    }

    mad_stream_buffer(&stream, buffer, buflen);

    while (!stop_flag || !*stop_flag) {
      if (mad_header_decode(&header, &stream) == -1) {
	if (!MAD_RECOVERABLE(stream.error))
	  break;

	if (do_error(&stream, 0, input, 0, 0))
	  continue;
      }

      ++stats.frames;

      if (length)
	mad_timer_add(&timer, header.duration);
      if (bitrate)
	avgbitrate = vbr_update(&stats, header.bitrate);
    }

    if ((stop_flag && *stop_flag) || stream.error != MAD_ERROR_BUFLEN)
      break;

    memmove(buffer, stream.next_frame, &buffer[buflen] - stream.next_frame);
    buflen -= stream.next_frame - &buffer[0];
  }

  mad_header_finish(&header);
  mad_stream_finish(&stream);

  if (length)
    *length = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);
  if (bitrate)
    *bitrate = avgbitrate;
}

static
DWORD WINAPI run_length_thread(void *param)
{
  struct state *state = param;
  HANDLE file;
  struct input input;

  file = CreateFile(state->path, GENERIC_READ,
		    FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
  if (file == INVALID_HANDLE_VALUE)
    return -1;

  input_init(&input, INPUT_FILE, file);

  scan_file(&input, &state->stop, &state->length, &state->bitrate);

  input_close(&input);

  return 0;
}

static
int decode_start(struct state *state, struct mad_header *header)
{
  int max_latency, nch, priority;
  DWORD thread_id;

  state->position    = 0;
  state->paused      = 0;
  state->seek        = -1;
  state->stop        = 0;
  state->channel     = conf_channel;
  state->attenuation = MAD_F_ONE;

  switch (conf_channel) {
  case CHANNEL_STEREO:
  case CHANNEL_REVERSE:
    nch = MAD_NCHANNELS(header);
    break;

  default:
    nch = 1;
    break;
  }

  max_latency =
    module.outMod->Open(header->samplerate, nch, conf_resolution, -1, -1);

  if (max_latency < 0) {  /* error opening output device */
    input_close(&state->input);
    return 3;
  }

  module.SetInfo(header->bitrate / 1000, header->samplerate / 1000, nch, 0);

  if (state->input.type == INPUT_FILE) {
    /* start file length calculation thread */
    length_thread = CreateThread(0, 0, run_length_thread, state,
				 0, &thread_id);
    SetThreadPriority(length_thread, THREAD_PRIORITY_BELOW_NORMAL);
  }

  /* initialize visuals */
  module.SAVSAInit(max_latency, header->samplerate);
  module.VSASetInfo(header->samplerate, nch);

  /* set the output module's default volume */
  module.outMod->SetVolume(-666);  /* ?? */

  /* start decoder thread */
  decode_thread = CreateThread(0, 0, run_decode_thread, state,
			       0, &thread_id);

  switch (conf_priority) {
  case +2:
    priority = THREAD_PRIORITY_HIGHEST;
    break;
  case +1:
    priority = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case 0:
  default:
    priority = THREAD_PRIORITY_NORMAL;
    break;
  case -1:
    priority = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case -2:
    priority = THREAD_PRIORITY_LOWEST;
    break;
  }
  SetThreadPriority(decode_thread, priority);

  return 0;
}

static
int play_stream(struct state *state)
{
  HINTERNET stream;
  struct mad_header header;

  if (internet == INVALID_HANDLE_VALUE) {
    internet = InternetOpen("MAD/" PLUGIN_VERSION " (Winamp Plug-in)",
			    INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
  }

  stream = InternetOpenUrl(internet, state->path, "Icy-MetaData: 0", -1,
			   INTERNET_FLAG_NO_CACHE_WRITE, 0);
  if (stream == 0) {
    show_error(0, "Error Opening Stream", GetLastError());
    return -1;
  }

  input_init(&state->input, INPUT_STREAM, stream);

  if (scan_header(&state->input, &header, &state->xing) == -1) {
    input_close(&state->input);

# if 0
    show_error(0, "Error Reading Stream", IDS_WARN_NOHEADER);
# endif

    return -1;
  }

  state->size    = 0;
  state->length  = 0;
  state->bitrate = 0;

  module.is_seekable = 0;

  return decode_start(state, &header);
}

static
int play_file(struct state *state)
{
  HANDLE file;
  struct mad_header header;

  file = CreateFile(state->path, GENERIC_READ,
		    FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
  if (file == INVALID_HANDLE_VALUE) {
    DWORD error;

    error = GetLastError();

# if 0
    show_error(0, "Error Opening File", error);
# endif

    return -1;
  }

  input_init(&state->input, INPUT_FILE, file);

  if (scan_header(&state->input, &header, &state->xing) == -1) {
    input_close(&state->input);

# if 0
    show_error(0, "Error Reading File", IDS_WARN_NOHEADER);
# endif

    return -1;
  }

  input_seek(&state->input, 0, FILE_BEGIN);

  state->size    = GetFileSize(file, 0);
  state->bitrate = 0;

  if (state->xing.flags & XING_FRAMES) {
    mad_timer_t timer;

    timer = header.duration;
    mad_timer_multiply(&timer, state->xing.frames);

    state->length = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);

    if ((state->xing.flags & XING_BYTES) && state->length > 0)
      state->bitrate = state->xing.bytes * 8 / state->length;
  }
  else
    state->length = -(state->size * 8 / (header.bitrate / 1000));  /* est. */

  module.is_seekable = 1;

  return decode_start(state, &header);
}

static
int do_play(char *path)
{
  ++state.serial;
  strcpy(state.path, path);

  return is_stream(path) ? play_stream(&state) : play_file(&state);
}

static
void do_pause(void)
{
  state.paused = 1;
  module.outMod->Pause(1);
}

static
void do_unpause(void)
{
  state.paused = 0;
  module.outMod->Pause(0);
}

static
int is_paused(void)
{
  return state.paused;
}

static
void do_stop(void)
{
  if (decode_thread != INVALID_HANDLE_VALUE) {
    state.stop = 1;

    if (WaitForSingleObject(decode_thread, INFINITE) == WAIT_TIMEOUT)
      TerminateThread(decode_thread, 0);

    CloseHandle(decode_thread);
    decode_thread = INVALID_HANDLE_VALUE;
  }

  if (length_thread != INVALID_HANDLE_VALUE) {
    if (WaitForSingleObject(length_thread, INFINITE) == WAIT_TIMEOUT)
      TerminateThread(length_thread, 0);

    CloseHandle(length_thread);
    length_thread = INVALID_HANDLE_VALUE;
  }

  input_close(&state.input);

  module.outMod->Close();
  module.SAVSADeInit();
}

static
int get_length(void)
{
  return abs(state.length);
}

static
int get_outputtime(void)
{
  if (state.seek >= 0)
    return state.seek;

  if (state.seek < -1)
    return (double) state.length * state.seek / 1000;

  return state.position +
    (module.outMod->GetOutputTime() - module.outMod->GetWrittenTime());
}

static
void set_outputtime(int position)
{
  int seek;

  seek = position;

  if (state.length < 0) {
    seek = (double) position * 1000 / state.length;
    if (seek >= 0)
      seek = position;
    else if (seek == -1)
      seek = -2;
  }

  state.seek = seek;
}

static
void set_volume(int volume)
{
  module.outMod->SetVolume(volume);
}

static
void set_pan(int pan)
{
  module.outMod->SetPan(pan);
}

struct id3v1 {
  unsigned char data[128];
  char title[30 + 1];
  char artist[30 + 1];
  char album[30 + 1];
  char year[4 + 1];
  char comment[30 + 1];
  int track;
  int genre;
};

static
void strip(char *str)
{
  char *ptr;

  ptr = str + strlen(str);
  while (ptr > str && ptr[-1] == ' ')
    --ptr;

  *ptr = 0;
}

static
int id3v1_fromdata(struct id3v1 *tag)
{
  if (memcmp(tag->data, "TAG", 3) != 0) {
    tag->title[0]   = 0;
    tag->artist[0]  = 0;
    tag->album[0]   = 0;
    tag->year[0]    = 0;
    tag->comment[0] = 0;

    tag->track = 0;
    tag->genre = -1;

    return -1;
  }

  tag->title[30]   = 0;
  tag->artist[30]  = 0;
  tag->album[30]   = 0;
  tag->year[4]     = 0;
  tag->comment[30] = 0;

  memcpy(tag->title,   &tag->data[3],  30);
  memcpy(tag->artist,  &tag->data[33], 30);
  memcpy(tag->album,   &tag->data[63], 30);
  memcpy(tag->year,    &tag->data[93],  4);
  memcpy(tag->comment, &tag->data[97], 30);

  strip(tag->title);
  strip(tag->artist);
  strip(tag->album);
  strip(tag->year);
  strip(tag->comment);

  tag->track = 0;
  if (tag->data[125] == 0)
    tag->track = tag->data[126];

  tag->genre = tag->data[127];

  return 0;
}

static
int id3v1_todata(struct id3v1 *tag)
{
  int i;

  memset(tag->data, ' ', sizeof(tag->data));

  memcpy(&tag->data[0],  "TAG",        3);
  memcpy(&tag->data[3],  tag->title,   strlen(tag->title));
  memcpy(&tag->data[33], tag->artist,  strlen(tag->artist));
  memcpy(&tag->data[63], tag->album,   strlen(tag->album));
  memcpy(&tag->data[93], tag->year,    strlen(tag->year));
  memcpy(&tag->data[97], tag->comment, strlen(tag->comment));

  if (tag->track) {
    tag->data[125] = 0;
    tag->data[126] = tag->track;
  }

  tag->data[127] = tag->genre;

  /* check whether tag is empty */

  for (i = 3; i < 127; ++i) {
    if (tag->data[i] != ' ')
      return 1;
  }

  return tag->data[127] != (unsigned char) ~0;
}

struct fileinfo {
  HANDLE file;
  DWORD attributes;
  DWORD size;

  char dirname[MAX_PATH];
  char *basename;

  struct {
    HWND dialog;
    int stop;
    int length;
    int bitrate;
    struct mad_header header;
  } mpeg;

  struct id3_file *id3file;

  struct {
    int has;
    struct id3v1 tag;
  } id3v1;

  struct {
    int has;
  } id3v2;
};

static
DWORD WINAPI run_scan_thread(void *param)
{
  HANDLE file;
  struct fileinfo *info = param;
  struct input input;

  if (DuplicateHandle(GetCurrentProcess(), info->file,
		      GetCurrentProcess(), &file, GENERIC_READ, FALSE, 0) == 0)
    return 1;

  input_init(&input, INPUT_FILE, file);

  input_seek(&input, 0, FILE_BEGIN);
  scan_header(&input, &info->mpeg.header, 0);

  if (!info->mpeg.stop)
    PostMessage(info->mpeg.dialog, WM_MAD_SCAN_FINISHED, 1, 0);

  input_seek(&input, 0, FILE_BEGIN);
  scan_file(&input, &info->mpeg.stop, &info->mpeg.length, &info->mpeg.bitrate);

  if (!info->mpeg.stop)
    PostMessage(info->mpeg.dialog, WM_MAD_SCAN_FINISHED, 0, 0);

  input_close(&input);

  return 0;
}

static
void groupnumber(char *str, int num)
{
  struct lconv *lconv;
  char *ptr, sep;
  int len, grouping, count, m;

  sprintf(str, "%d", num);
  len = strlen(str);

  lconv = localeconv();

  sep      = *lconv->thousands_sep;
  grouping = *lconv->grouping;

  if (sep == 0)
    sep = ',';
  if (grouping == 0)
    grouping = 3;

  count = (len - 1) / grouping;

  ptr  = str + len + count;
  *ptr = 0;

  m = grouping;
  while (count) {
    ptr[-1] = ptr[-1 - count];
    --ptr;

    if (--m == 0) {
      *--ptr = sep;
      --count;
      m = grouping;
    }
  }
}

static CALLBACK
BOOL mpeg_dialog(HWND dialog, UINT message,
		 WPARAM wparam, LPARAM lparam)
{
  static PROPSHEETPAGE *psp;
  static struct fileinfo *info;
  static HANDLE scan_thread;

  switch (message) {
  case WM_INITDIALOG:
    {
      DWORD thread_id;
      char str[40];
      double size;
      char *unit;

      psp  = (PROPSHEETPAGE *) lparam;
      info = (struct fileinfo *) psp->lParam;

      info->mpeg.dialog = dialog;
      info->mpeg.stop   = 0;

      scan_thread = CreateThread(0, 0, run_scan_thread, info, 0, &thread_id);

      SetDlgItemText(dialog, IDC_MPEG_LOCATION, info->dirname);

      size = info->size;
      unit = 0;

      if (size >= 1024) {
	size /= 1024;
	unit  = "KB";

	if (size >= 1024) {
	  size /= 1024;
	  unit  = "MB";

	  if (size >= 1024) {
	    size /= 1024;
	    unit  = "GB";

	    if (size >= 1024) {
	      size /= 1024;
	      unit  = "TB";
	    }
	  }
	}
      }

      if (unit) {
	sprintf(str, "%.1f%s", size, unit);
	strcat(str, " (");
	groupnumber(str + strlen(str), info->size);
	strcat(str, " bytes)");
      }
      else
	sprintf(str, "%ld bytes", info->size);

      SetDlgItemText(dialog, IDC_MPEG_SIZE, str);
    }
    break;

  case WM_DESTROY:
    if (scan_thread != INVALID_HANDLE_VALUE) {
      info->mpeg.stop = 1;

      if (WaitForSingleObject(scan_thread, INFINITE) == WAIT_TIMEOUT)
	TerminateThread(scan_thread, 0);

      CloseHandle(scan_thread);
      scan_thread = INVALID_HANDLE_VALUE;
    }
    break;

  case WM_MAD_SCAN_FINISHED:
    {
      struct mad_header *header = &info->mpeg.header;
      char *ptr;
      char str[23];

      if (wparam) {
	switch (header->samplerate) {
	case 48000:
	case 44100:
	case 32000:
	  ptr = "MPEG-1";
	  break;

	case 24000:
	case 22050:
	case 16000:
	  ptr = "MPEG-2";
	  break;

	case 12000:
	case 11025:
	case  8000:
	  ptr = "MPEG 2.5";
	  break;

	default:
	  ptr = "unknown";
	}
	SetDlgItemText(dialog, IDC_MPEG_TYPE, ptr);

	switch (header->layer) {
	case MAD_LAYER_I:
	  ptr = "I";
	  break;

	case MAD_LAYER_II:
	  ptr = "II";
	  break;

	case MAD_LAYER_III:
	  ptr = "III";
	  break;

	default:
	  ptr = "unknown";
	}
	SetDlgItemText(dialog, IDC_MPEG_LAYER, ptr);

	sprintf(str, "%lu kbps", header->bitrate / 1000);
	if (header->flags & MAD_FLAG_FREEFORMAT)
	  strcat(str, " (free format)");
	SetDlgItemText(dialog, IDC_MPEG_BITRATE, str);

	sprintf(str, "%u Hz", header->samplerate);
	SetDlgItemText(dialog, IDC_MPEG_SAMPLERATE, str);

	switch (header->emphasis) {
	case MAD_EMPHASIS_NONE:
	  ptr = "none";
	  break;

	case MAD_EMPHASIS_50_15_US:
	  ptr = "50/15 \xb5s";
	  break;

	case MAD_EMPHASIS_CCITT_J_17:
	  ptr = "CCITT J.17";
	  break;

	default:
	  ptr = "unknown";
	}
	SetDlgItemText(dialog, IDC_MPEG_EMPHASIS, ptr);

	switch (header->mode) {
	case MAD_MODE_SINGLE_CHANNEL:
	  SetDlgItemText(dialog, IDC_MPEG_MODE, "Single Channel");
	  break;

	case MAD_MODE_DUAL_CHANNEL:
	  SetDlgItemText(dialog, IDC_MPEG_MODE, "Dual Channel");
	  break;

	case MAD_MODE_JOINT_STEREO:
	  SetDlgItemText(dialog, IDC_MPEG_MODE, "Joint Stereo");
	  break;

	case MAD_MODE_STEREO:
	  SetDlgItemText(dialog, IDC_MPEG_MODE, "Stereo");
	  break;
	}

	if (header->flags & MAD_FLAG_COPYRIGHT)
	  CheckDlgButton(dialog, IDC_MPEG_COPYRIGHT, BST_CHECKED);
	if (header->flags & MAD_FLAG_ORIGINAL)
	  CheckDlgButton(dialog, IDC_MPEG_ORIGINAL, BST_CHECKED);
	if (header->flags & MAD_FLAG_PROTECTION)
	  CheckDlgButton(dialog, IDC_MPEG_CRC, BST_CHECKED);
      }
      else {
	mad_timer_set(&header->duration, 0, info->mpeg.length, 1000);
	mad_timer_string(header->duration, str,
			 "%lu:%02u:%02u.%1u", MAD_UNITS_HOURS,
			 MAD_UNITS_DECISECONDS, 0);
	ptr = strchr(str, '.');
	if (ptr)
	  *ptr = *localeconv()->decimal_point;
	SetDlgItemText(dialog, IDC_MPEG_LENGTH, str);

	if (info->mpeg.bitrate != header->bitrate / 1000) {
	  sprintf(str, "%d kbps (average)", info->mpeg.bitrate);
	  SetDlgItemText(dialog, IDC_MPEG_BITRATE, str);
	}
      }
    }
    return TRUE;

  case WM_NOTIFY:
    switch (((NMHDR *) lparam)->code) {
    case PSN_SETACTIVE:
      return TRUE;

    case PSN_KILLACTIVE:
      SetWindowLong(dialog, DWL_MSGRESULT, FALSE);
      return TRUE;

    case PSN_APPLY:
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static
char const *const genre_str[] = {
# include "id3genre.dat"
};

# define NGENRES	(sizeof(genre_str) / sizeof(genre_str[0]))

static CALLBACK
BOOL id3v1_dialog(HWND dialog, UINT message,
		  WPARAM wparam, LPARAM lparam)
{
  static PROPSHEETPAGE *psp;
  static struct fileinfo *info;
  struct id3v1 *tag;
  int index;

  switch (message) {
  case WM_INITDIALOG:
    psp  = (PROPSHEETPAGE *) lparam;
    info = (struct fileinfo *) psp->lParam;

    /* initialize genre combobox */

    for (index = 0; index < NGENRES; ++index) {
      SendDlgItemMessage(dialog, IDC_ID3V1_GENRE,
			 CB_ADDSTRING, 0, (LPARAM) genre_str[index]);
    }

    index = SendDlgItemMessage(dialog, IDC_ID3V1_GENRE,
			       CB_ADDSTRING, 0, (LPARAM) " ");

    /* disable fields for read-only files, or set text limits */

    if (info->attributes & FILE_ATTRIBUTE_READONLY) {
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_TITLE),   FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_ARTIST),  FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_ALBUM),   FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_YEAR),    FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_TRACK),   FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_GENRE),   FALSE);
      EnableWindow(GetDlgItem(dialog, IDC_ID3V1_COMMENT), FALSE);
    }
    else {
      SendDlgItemMessage(dialog, IDC_ID3V1_TITLE,   EM_SETLIMITTEXT, 30, 0);
      SendDlgItemMessage(dialog, IDC_ID3V1_ARTIST,  EM_SETLIMITTEXT, 30, 0);
      SendDlgItemMessage(dialog, IDC_ID3V1_ALBUM,   EM_SETLIMITTEXT, 30, 0);
      SendDlgItemMessage(dialog, IDC_ID3V1_YEAR,    EM_SETLIMITTEXT,  4, 0);
      SendDlgItemMessage(dialog, IDC_ID3V1_TRACK,   EM_SETLIMITTEXT,  3, 0);
      SendDlgItemMessage(dialog, IDC_ID3V1_COMMENT, EM_SETLIMITTEXT, 30, 0);
    }

    /* load existing tag data */

    tag = &info->id3v1.tag;

    if (info->id3v1.has)
      SetDlgItemText(dialog, IDC_ID3V1_TITLE, tag->title);
    else {
      char filename[31], *ptr;

      strncpy(filename, info->basename, 30);
      filename[30] = 0;

      ptr = strrchr(filename, '.');
      if (ptr)
	*ptr = 0;

      for (ptr = filename; *ptr; ++ptr) {
	if (*ptr == '_')
	  *ptr = ' ';
      }

      SetDlgItemText(dialog, IDC_ID3V1_TITLE, filename);
    }

    SetDlgItemText(dialog, IDC_ID3V1_ARTIST,  tag->artist);
    SetDlgItemText(dialog, IDC_ID3V1_ALBUM,   tag->album);
    SetDlgItemText(dialog, IDC_ID3V1_YEAR,    tag->year);
    SetDlgItemText(dialog, IDC_ID3V1_COMMENT, tag->comment);

    if (tag->track)
      SetDlgItemInt(dialog, IDC_ID3V1_TRACK, tag->track, FALSE);

    if (tag->genre >= 0) {
      if (tag->genre < NGENRES) {
	index = SendDlgItemMessage(dialog, IDC_ID3V1_GENRE, CB_FINDSTRINGEXACT,
				   -1, (LPARAM) genre_str[tag->genre]);
      }
      else if (tag->genre < 255) {
	char str[14];

	sprintf(str, "(%d)", tag->genre);

	index = SendDlgItemMessage(dialog, IDC_ID3V1_GENRE,
				   CB_ADDSTRING, 0, (LPARAM) str);
      }
    }

    SendDlgItemMessage(dialog, IDC_ID3V1_GENRE, CB_SETCURSEL, index, 0);

    break;

  case WM_NOTIFY:
    switch (((NMHDR *) lparam)->code) {
    case PSN_SETACTIVE:
      return TRUE;

    case PSN_KILLACTIVE:
      {
	int track;
	BOOL success;

	track = GetDlgItemInt(dialog, IDC_ID3V1_TRACK, &success, FALSE);
	if (!success || track < 1 || track > 255) {
	  if (SendDlgItemMessage(dialog, IDC_ID3V1_TRACK,
				 WM_GETTEXTLENGTH, 0, 0) > 0) {
	    show_error(dialog, "Invalid Track", IDS_WARN_BADTRACK);
	    SetFocus(GetDlgItem(dialog, IDC_ID3V1_TRACK));
	    SetWindowLong(dialog, DWL_MSGRESULT, TRUE);
	    return TRUE;
	  }
	}

	if (success && SendDlgItemMessage(dialog, IDC_ID3V1_COMMENT,
					  WM_GETTEXTLENGTH, 0, 0) > 28) {
	  show_error(dialog, "ID3v1.1 Comment Too Long",
		     IDS_WARN_COMMENTTOOLONG);
	  SetFocus(GetDlgItem(dialog, IDC_ID3V1_COMMENT));
	  SetWindowLong(dialog, DWL_MSGRESULT, TRUE);
	  return TRUE;
	}
      }

      SetWindowLong(dialog, DWL_MSGRESULT, FALSE);
      return TRUE;

    case PSN_APPLY:
      if (!(info->attributes & FILE_ATTRIBUTE_READONLY)) {
	char selected[25];
	DWORD bytes;

	tag = &info->id3v1.tag;

	/* title, artist, album, year, comment, track */

	GetDlgItemText(dialog, IDC_ID3V1_TITLE,
		       tag->title,   sizeof(tag->title));
	GetDlgItemText(dialog, IDC_ID3V1_ARTIST,
		       tag->artist,  sizeof(tag->artist));
	GetDlgItemText(dialog, IDC_ID3V1_ALBUM,
		       tag->album,   sizeof(tag->album));
	GetDlgItemText(dialog, IDC_ID3V1_YEAR,
		       tag->year,    sizeof(tag->year));
	GetDlgItemText(dialog, IDC_ID3V1_COMMENT,
		       tag->comment, sizeof(tag->comment));

	tag->track = GetDlgItemInt(dialog, IDC_ID3V1_TRACK, 0, FALSE);

	/* genre */

	tag->genre = -1;

	GetDlgItemText(dialog, IDC_ID3V1_GENRE, selected, sizeof(selected));

	if (selected[0] == '(')
	  sscanf(selected + 1, "%d", &tag->genre);
	else {
	  for (index = 0; index < NGENRES; ++index) {
	    if (strcmp(selected, genre_str[index]) == 0) {
	      tag->genre = index;
	      break;
	    }
	  }
	}

	/* write or delete tag */

	SetFilePointer(info->file, info->id3v1.has ?
		       -sizeof(info->id3v1.tag.data) : 0, 0, FILE_END);

	if (id3v1_todata(tag)) {
	  if (WriteFile(info->file, info->id3v1.tag.data,
			sizeof(info->id3v1.tag.data), &bytes, 0) == 0)
	    show_error(dialog, "Error Writing ID3v1 Tag", GetLastError());
	  else if (bytes != sizeof(info->id3v1.tag.data))
	    show_error(dialog, 0, IDS_ERR_ID3V1WRITE);
	}
	else
	  SetEndOfFile(info->file);
      }
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static CALLBACK
BOOL id3v2_dialog(HWND dialog, UINT message,
		  WPARAM wparam, LPARAM lparam)
{
  static PROPSHEETPAGE *psp;
  static struct fileinfo *info;

  switch (message) {
  case WM_INITDIALOG:
    psp  = (PROPSHEETPAGE *) lparam;
    info = (struct fileinfo *) psp->lParam;

    break;

  case WM_NOTIFY:
    switch (((NMHDR *) lparam)->code) {
    case PSN_SETACTIVE:
      return TRUE;

    case PSN_KILLACTIVE:
      SetWindowLong(dialog, DWL_MSGRESULT, FALSE);
      return TRUE;

    case PSN_APPLY:
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static CALLBACK
BOOL stats_dialog(HWND dialog, UINT message,
		  WPARAM wparam, LPARAM lparam)
{
  static PROPSHEETPAGE *psp;
  static struct fileinfo *info;
  static int serial;
  static UINT timer;

  enum {
    db_scale = 6
  };

  switch (message) {
  case WM_INITDIALOG:
    psp  = (PROPSHEETPAGE *) lparam;
    info = (struct fileinfo *) psp->lParam;

    serial = state.serial;

    SetWindowLong(GetDlgItem(dialog, IDC_STATS_LEGENDMS),
		  GWL_MAD_LEGEND_COLOR, JSPIE_MS_COLOR);
    SetWindowLong(GetDlgItem(dialog, IDC_STATS_LEGENDI),
		  GWL_MAD_LEGEND_COLOR, JSPIE_I_COLOR);

    SendDlgItemMessage(dialog, IDC_STATS_CLIPPING,
		       PBM_SETRANGE, 0, MAKELPARAM(0, db_scale * 10));

    timer = SetTimer(dialog, 103, 1000, 0);
    PostMessage(dialog, WM_TIMER, timer, 0);

    break;

  case WM_TIMER:
    if (wparam == timer) {
      if (state.serial == serial) {
	struct stats *stats = &state.stats;
	HWND pie;
	double db;
	char str[10];

	/* Frames */

	SetDlgItemInt(dialog, IDC_STATS_DECODED, stats->frames, FALSE);

	pie = GetDlgItem(dialog, IDC_STATS_JSPIE);
	SetWindowLong(pie, GWL_MAD_JSPIE_MS,     stats->ms_joint);
	SetWindowLong(pie, GWL_MAD_JSPIE_I,      stats->i_joint);
	SetWindowLong(pie, GWL_MAD_JSPIE_MS_I,   stats->ms_i_joint);
	SetWindowLong(pie, GWL_MAD_JSPIE_FRAMES, stats->frames);
	InvalidateRect(pie, 0, FALSE);

	/* Errors */

	SetDlgItemInt(dialog, IDC_STATS_SYNCERR, stats->sync_errors,  FALSE);
	SetDlgItemInt(dialog, IDC_STATS_CRCERR,  stats->crc_errors,   FALSE);
	SetDlgItemInt(dialog, IDC_STATS_OTHERR,  stats->other_errors, FALSE);

	/* Output */

	if (state.attenuation == 0)
	  strcpy(str, "-inf dB");
	else {
	  db = 20 * log10(mad_f_todouble(state.attenuation));
	  sprintf(str, "%.1f dB", db);
	}
	SetDlgItemText(dialog, IDC_STATS_ATTENUATION, str);

	SetDlgItemInt(dialog, IDC_STATS_CLIPPED, stats->clipped, FALSE);

	db = 20 * log10(mad_f_todouble(MAD_F_ONE + stats->clipping));

	SendDlgItemMessage(dialog, IDC_STATS_CLIPPING, PBM_SETPOS, db * 10, 0);

	EnableWindow(GetDlgItem(dialog, IDC_STATS_ATTENUATE),
		     stats->clipping > 0);
	EnableWindow(GetDlgItem(dialog, IDC_STATS_RESET),
		     state.attenuation != MAD_F_ONE);
      }
      else {
	EnableWindow(GetDlgItem(dialog, IDC_STATS_ATTENUATE), FALSE);
	EnableWindow(GetDlgItem(dialog, IDC_STATS_RESET),     FALSE);

	KillTimer(dialog, timer);
	timer = 0;
      }

      return TRUE;
    }
    break;

  case WM_DESTROY:
    if (timer)
      KillTimer(dialog, timer);
    break;

  case WM_COMMAND:
    switch (LOWORD(wparam)) {
    case IDC_STATS_ATTENUATE:
    case IDC_STATS_RESET:
      if (LOWORD(wparam) == IDC_STATS_RESET)
	state.attenuation = MAD_F_ONE;
      else {
	state.attenuation =
	  mad_f_tofixed(mad_f_todouble(state.attenuation) /
			mad_f_todouble(MAD_F_ONE + state.stats.clipping));
      }
      state.stats.clipped  = 0;
      state.stats.clipping = 0;

      PostMessage(dialog, WM_TIMER, timer, 0);
      return TRUE;
    }
    break;

  case WM_NOTIFY:
    switch (((NMHDR *) lparam)->code) {
    case PSN_SETACTIVE:
      return TRUE;

    case PSN_KILLACTIVE:
      SetWindowLong(dialog, DWL_MSGRESULT, FALSE);
      return TRUE;

    case PSN_APPLY:
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static CALLBACK
int propsheet_init(HWND dialog, UINT message, LPARAM lparam)
{
  switch (message) {
  case PSCB_INITIALIZED:
    SetDlgItemText(dialog, IDOK, "Save");
    SetFocus(PropSheet_GetTabControl(dialog));
    break;
  }

  return 0;
}

static CALLBACK
int propsheet_ro_init(HWND dialog, UINT message, LPARAM lparam)
{
  switch (message) {
  case PSCB_INITIALIZED:
    SetDlgItemText(dialog, IDCANCEL, "Close");
    EnableWindow(GetDlgItem(dialog, IDOK), FALSE);
    SendDlgItemMessage(dialog, IDOK,
		       BM_SETSTYLE, BS_PUSHBUTTON, MAKELPARAM(FALSE, 0));
    break;
  }

  return propsheet_init(dialog, message, lparam);
}

static
void proppage_init(PROPSHEETPAGE *page, int proppage_id,
		   DLGPROC dlgproc, LPARAM lparam, LPFNPSPCALLBACK callback)
{
  page->dwSize      = sizeof(*page);
  page->dwFlags     = PSP_DEFAULT | (callback ? PSP_USECALLBACK : 0);
  page->hInstance   = module.hDllInstance;
  page->pszTemplate = MAKEINTRESOURCE(proppage_id);
  page->pszIcon     = 0;
  page->pszTitle    = 0;
  page->pfnDlgProc  = dlgproc;
  page->lParam      = lparam;
  page->pfnCallback = callback;
  page->pcRefParent = 0;
}

static
int show_infobox(char *path, HWND parent)
{
  struct fileinfo info;
  int fd;
  PROPSHEETPAGE psp[4];
  PROPSHEETHEADER psh;
  DWORD bytes;

  if (is_stream(path)) {
    MessageBox(parent, "Stream info not yet implemented.",
	       "Not Implemented", MB_ICONWARNING | MB_OK);
    return 0;
  }

  info.file = CreateFile(path, GENERIC_READ | GENERIC_WRITE,
			 FILE_SHARE_READ, 0,
			 OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
  if (info.file != INVALID_HANDLE_VALUE)
    info.attributes = 0;
  else {
    info.file = CreateFile(path, GENERIC_READ,
			   FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
			   OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
    if (info.file == INVALID_HANDLE_VALUE) {
      show_error(parent, "Error Opening File", GetLastError());
      return 1;
    }

    info.attributes = FILE_ATTRIBUTE_READONLY;
  }

  info.size = GetFileSize(info.file, 0);

  strcpy(info.dirname, path);

  info.basename = strrchr(info.dirname, '\\');
  if (info.basename)
    *info.basename++ = 0;
  else
    info.basename = info.dirname;

  info.mpeg.dialog  = 0;
  info.mpeg.stop    = 0;
  info.mpeg.length  = 0;
  info.mpeg.bitrate = 0;
  mad_header_init(&info.mpeg.header);

  fd = _open_osfhandle((long) info.file,
		       ((info.attributes & FILE_ATTRIBUTE_READONLY) ?
			_O_RDONLY : _O_RDWR) | _O_BINARY);
  if (fd == -1) {
    show_error(parent, "Error Converting File Handle", GetLastError());
    CloseHandle(info.file);
    return 1;
  }

  info.id3file =
    id3_file_fdopen(fd, (info.attributes & FILE_ATTRIBUTE_READONLY) ?
		    ID3_FILE_MODE_READONLY : ID3_FILE_MODE_READWRITE);
  if (info.id3file == 0) {
    _close(fd);
    show_error(parent, "ID3 Error", IDS_ERR_ID3FAIL);
    return 1;
  }

  SetFilePointer(info.file, -sizeof(info.id3v1.tag.data), 0, FILE_END);
  if (ReadFile(info.file, info.id3v1.tag.data,
	       sizeof(info.id3v1.tag.data), &bytes, 0) == 0)
    bytes = 0;

  if (bytes != sizeof(info.id3v1.tag.data))
    memset(info.id3v1.tag.data, 0, sizeof(info.id3v1.tag.data));

  info.id3v1.has = (id3v1_fromdata(&info.id3v1.tag) == 0);
  info.id3v2.has = 0;

  proppage_init(&psp[0], IDD_PROPPAGE_MPEG,  mpeg_dialog,  (LPARAM) &info, 0);
  proppage_init(&psp[1], IDD_PROPPAGE_ID3V1, id3v1_dialog, (LPARAM) &info, 0);
  proppage_init(&psp[2], IDD_PROPPAGE_ID3V2, id3v2_dialog, (LPARAM) &info, 0);
  proppage_init(&psp[2], IDD_PROPPAGE_STATS, stats_dialog, (LPARAM) &info, 0);

  psh.dwSize      = sizeof(psh);
  psh.dwFlags     = PSH_PROPSHEETPAGE | PSH_PROPTITLE | PSH_NOAPPLYNOW |
                    PSH_USECALLBACK;
  psh.hwndParent  = parent;
  psh.hInstance   = module.hDllInstance;
  psh.pszIcon     = 0;
  psh.pszCaption  = info.basename;
  psh.nPages      = (strcmp(path, state.path) == 0) ? 3 : 2;
  psh.nStartPage  = info.id3v2.has ? 2 : (info.id3v1.has ? 1 : 0);
  psh.ppsp        = psp;
  psh.pfnCallback = (info.attributes & FILE_ATTRIBUTE_READONLY) ?
                    propsheet_ro_init : propsheet_init;

  PropertySheet(&psh);

  id3_file_close(info.id3file);

  return 0;
}

static
void unnest(char const **ptr, int elsepart)
{
  unsigned int nest = 1;

  while (**ptr) {
    switch (*(*ptr)++) {
    case '|':
      if (elsepart && nest == 1)
	return;
      continue;

    case '>':
      if (--nest == 0) {
	if (elsepart)
	  --*ptr;
	return;
      }
      continue;

    case '%':
      switch (**ptr) {
      case '?':
	++nest;
	while (**ptr && **ptr != '<')
	  ++*ptr;
	continue;

      default:
	++*ptr;
      case 0:
	continue;
      }
    }
  }
}

static
char const *title_escape(int escape, struct id3v1 const *tag,
			 char const *source, char buffer[])
{
  char *ptr = buffer;
  char const *copy;

  switch (escape) {
  case '0':  /* track */
    if (tag->track > 0) {
      if (tag->track >= 100)
	*ptr++ = '0' + tag->track / 100;
      if (1 || tag->track >= 10)
	*ptr++ = '0' + (tag->track % 100) / 10;
      *ptr++ = '0' + tag->track % 10;
    }
    break;

  case '1':  /* artist */
    return tag->artist;

  case '2':  /* title */
    return tag->title;

  case '3':  /* album */
    return tag->album;

  case '4':  /* year */
    return tag->year;

  case '5':  /* comment */
    return tag->comment;

  case '6':  /* genre */
    if (tag->genre >= 0 && tag->genre < NGENRES)
      return genre_str[tag->genre];

    break;

  case '7':  /* file name */
    copy = strrchr(source, '\\');
    if (copy)
      ++copy;
    else
      copy = source;

    strcpy(buffer, copy);
    ptr = strrchr(buffer, '.');

    break;

  case '8':  /* file path */
    strcpy(buffer, source);
    ptr = strrchr(buffer, '\\');

    break;

  case '9':  /* file extension */
    copy = strrchr(source, '.');
    if (copy)
      return copy + 1;

    break;

  default:
    return 0;
  }

  if (ptr)
    *ptr = 0;

  return buffer;
}

static
void get_fileinfo(char *path, char *title, int *length)
{
  char *source;
  HANDLE file = INVALID_HANDLE_VALUE;

  if (path == 0 || *path == 0) {  /* currently playing file */
    source = state.path;

    if (length && !is_stream(source) && state.length >= 0)
      *length = state.length;
  }
  else {  /* some other file */
    source = path;

    if (length && !is_stream(source)) {
      file = CreateFile(source, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, 0, 0);
      if (file != INVALID_HANDLE_VALUE) {
	struct mad_header header;
	struct xing xing;
	struct input input;

	input_init(&input, INPUT_FILE, file);

	if (conf_lengthcalc)
	  scan_file(&input, 0, length, 0);
	else if (scan_header(&input, &header, &xing) != -1) {
	  if (xing.flags & XING_FRAMES) {
	    mad_timer_t timer;

	    timer = header.duration;
	    mad_timer_multiply(&timer, xing.frames);

	    *length = mad_timer_count(timer, MAD_UNITS_MILLISECONDS);
	  }
	  else
	    *length = GetFileSize(file, 0) * 8 / (header.bitrate / 1000);
	}
      }
    }
  }

  if (title && is_stream(source))
    strcpy(title, source);
  else if (title) {
    struct id3v1 tag;
    DWORD bytes;
    char const *in;
    char *out, *bound;
    unsigned int nest = 0;

    if (file == INVALID_HANDLE_VALUE) {
      file = CreateFile(source, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, 0, 0);
    }

    if (file == INVALID_HANDLE_VALUE)
      bytes = 0;
    else {
      SetFilePointer(file, -sizeof(tag.data), 0, FILE_END);
      if (ReadFile(file, tag.data, sizeof(tag.data), &bytes, 0) == 0)
	bytes = 0;
    }

    if (bytes != sizeof(tag.data))
      memset(tag.data, 0, sizeof(tag.data));

    id3v1_fromdata(&tag);

    in    = conf_titlefmt;
    out   = title;
    bound = out + (MAX_PATH - 10 - 1);  /* ?? */

    while (*in && out < bound) {
      char buffer[MAX_PATH];
      char const *ptr;

      switch (*in) {
      case '%':
	++in;
	break;

      case '|':
      case '>':
	if (nest > 0) {
	  unnest(&in, 0);
	  --nest;
	  continue;
	}
	/* else fall through */

      default:
	*out++ = *in++;
	continue;
      }

      /* handle % escape sequence */

      switch (*in) {
      case 0:
	*out++ = '%';
	continue;

      case '%': case '|':
      case '<': case '>':
	*out++ = *in++;
	continue;

      case '?':
	ptr = title_escape(*++in, &tag, source, buffer);

	while (*in && *in != '<')
	  ++in;
	if (*in == '<')
	  ++in;

	if (ptr == 0)
	  unnest(&in, 0);
	else {
	  ++nest;
	  if (*ptr == 0)
	    unnest(&in, 1);
	}

	continue;

      default:
	ptr = title_escape(*in, &tag, source, buffer);

	if (ptr) {
	  ++in;

	  if (*ptr == 0)
	    ptr = "?";

	  while (*ptr && out < bound)
	    *out++ = *ptr++;
	}
	else {
	  *out++ = '%';
	  if (out < bound)
	    *out++ = *in++;
	}

	continue;
      }
    }

    *out = 0;
  }

  if (file != INVALID_HANDLE_VALUE)
    CloseHandle(file);
}

# if defined(OUR_EQ)
static
double eq_decibels(int value)
{
  /* 0-63, 0 == +20 dB, 31 == 0 dB, 63 == -20 dB */

  return (value == 31) ? 0.0 : 20.0 - (20.0 / 31.5) * value;
}

static
mad_fixed_t eq_factor(double db)
{
  if (db > 18)
    db = 18;

  return mad_f_tofixed(pow(10, db / 20));
}
# endif

static
void set_eq(int on, char data[10], int preamp)
{
# if defined(OUR_EQ)
  double base;
  static unsigned char const map[32] = {
    0, 1, 2, 3, 4, 5, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8,
    8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9
  };
  int i;

  /* 60, 170, 310, 600, 1k, 3k, 6k, 12k, 14k, 16k */

  base = eq_decibels(preamp);

  for (i = 0; i < 32; ++i)
    state.eqfactor[i] = eq_factor(base + eq_decibels(data[map[i]]));

  state.equalizer = on;
# endif
}

static
In_Module module = {
  /* version */		IN_VER,

  /* description */	"MAD plug-in " PLUGIN_VERSION,

  /* < hMainWindow */	0,
  /* < hDllInstance */	0,

  /* FileExtensions */	"",

  /* is_seekable */	0,
  /* UsesOutputPlug */	1,

  /* Config */		show_config,
  /* About */		show_about,
  /* Init */		do_init,
  /* Quit */		do_quit,

  /* GetFileInfo */	get_fileinfo,
  /* InfoBox */		show_infobox,
  /* IsOurFile */	is_ourfile,

  /* Play */		do_play,
  /* Pause */		do_pause,
  /* UnPause */		do_unpause,
  /* IsPaused */	is_paused,
  /* Stop */		do_stop,

  /* GetLength */	get_length,
  /* GetOutputTime */	get_outputtime,

  /* SetOutputTime */	set_outputtime,
  /* SetVolume */	set_volume,
  /* SetPan */		set_pan,

  /* < SAVSAInit */	0,
  /* < SAVSADeInit */	0,
  /* < SAAddPCMData */	0,
  /* < SAGetMode */	0,
  /* < SAAdd */		0,

  /* < VSAAddPCMData */	0,
  /* < VSAGetMode */	0,
  /* < VSAAdd */	0,
  /* < VSASetInfo */	0,

  /* < dsp_isactive */	0,
  /* < dsp_dosamples */	0,

  /* EQSet */		set_eq,

  /* < SetInfo */	0,

  /* < outMod */	0
};

__declspec(dllexport)
In_Module *winampGetInModule2(void)
{
  return &module;
}
