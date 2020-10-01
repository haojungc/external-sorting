CC := gcc
EXE := sort

all:
	${CC} -o ${EXE} main.c
	${CC} -o generator int32_generator.c

sort:
	${CC} -o ${EXE} main.c

gen:
	${CC} -o generator int32_generator.c

debug:
	${CC} -g -o ${EXE} main.c

clean:
	rm -r -f ${EXE} generator output.txt tmp/
