CC=gcc
CFLAGS=-I. -g

DEPS = x16.h bits.h control.h instruction.h trap.h io.h
OBJ = x16.o bits.o control.o instruction.o trap.o io.o decode.o
MAIN = main.o
TARGET = x16

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

x16: $(OBJ) $(MAIN)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

clean:
	rm -rf *.o $(TARGET)