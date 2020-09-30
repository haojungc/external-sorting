CC := gcc
EXE := sort

all:
	${CC} -o ${EXE} main.c

debug:
	${CC} -g -o ${EXE} main.c

clean:
	rm -r -f ${EXE} tmp/
