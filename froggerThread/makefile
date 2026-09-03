main: main.o render.o frog.o thread_control.o collision.o bullet.o plants.o crocodile.o timer.o menu.o
	gcc -o frogger main.o  render.o frog.o thread_control.o collision.o bullet.o plants.o crocodile.o timer.o menu.o -lpthread -lSDL2 -lSDL2_mixer -lncurses 

main.o: main.c main.h 
	gcc -c main.c

render.o: render.c render.h
	gcc -c render.c

frog.o: frog.c frog.h
	gcc -c frog.c

thread_control.o: thread_control.c thread_control.h
	gcc -c thread_control.c

collision.o: collision.c collision.h
	gcc -c collision.c

bullet.o: bullet.c bullet.h
	gcc -c bullet.c

plants.o: plants.c plants.h
	gcc -c plants.c

crocodile.o: crocodile.c crocodile.h
	gcc -c crocodile.c

timer.o: timer.c timer.h
	gcc -c timer.c

menu.o: menu.c menu.h
	gcc -c menu.c

clean:
	rm -f *.o
	rm -f frogger