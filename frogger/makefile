# Makefile 
main: main.o render.o frog.o collisions.o crocodile.o plants.o timer.o menu.o bullet.o
	gcc -o frogger main.o render.o frog.o collisions.o crocodile.o plants.o timer.o menu.o bullet.o -lSDL2 -lSDL2_mixer -lncurses

main.o: main.c main.h 
	gcc -c main.c

render.o: render.c render.h 
	gcc -c render.c

frog.o: frog.c frog.h
	gcc -c frog.c

collisions.o: collisions.c collisions.h
	gcc -c collisions.c

crocodile.o: crocodile.c crocodile.h
	gcc -c crocodile.c

plants.o: plants.c plants.h
	gcc -c plants.c

timer.o: timer.c timer.h
	gcc -c timer.c

menu.o: menu.c menu.h
	gcc -c menu.c

bullet.o: bullet.c bullet.h
	gcc -c bullet.c

clean:
	rm -f *.o
	rm -f frogger