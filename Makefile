all: say_hello create_program clean

say_hello:
	@echo "Building program"

create_program:
	gcc -pthread test.c

clean:
	@echo "program built"
