all:  dijkstra_large dijkstra_small

dijkstra_large: dijkstra_large.c Makefile
	gcc -static dijkstra_large.c -O3 -o dijkstra_large

dijkstra_small: dijkstra_small.c Makefile
	gcc -static dijkstra_small.c -O3 -o dijkstra_small 

clean:
	rm -rf dijkstra_large dijkstra_small output*
