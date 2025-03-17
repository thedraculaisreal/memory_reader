main: main
	gcc -D_GNU_SOURCE src/main.c src/file_reading.c src/memory.c -o main
