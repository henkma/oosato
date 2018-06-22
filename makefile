all::oosato

oosato: main.o prop.o
	gcc -Wall -ggdb -O2 main.o prop.o -o oosato -lncursesw
main.o: main.c defines.h types.h prop.h
	gcc -Wall -ggdb -O2 main.c -c
porp.o: prop.c defines.h types.h prop.h
	gcc -Wall -ggdb -O2 prop.c -c

clean:
	rm -f oosato *.o
