all: clean build run

build:
	@gcc test.c ../src/JSON2C.c -o test.out

run:
	@./test.out

clean:
	@rm -f *.out
