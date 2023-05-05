CFLAGS = -Wall -Werror -Wextra

all: s21_grep clean

s21_grep: s21_grep.o 
	gcc $(CFLAGS) s21_grep.o  -o s21_grep

s21_grep.o: s21_grep.c
	gcc $(CFLAGS) s21_grep.c -c

test: s21_grep
	chmod +x test_func_grep.sh
	./test_func_grep.sh
	rm -rf *.o

leak: s21_grep
	chmod +x test_leak_grep.sh
	./test_leak_grep.sh
	rm -rf *.o

style:
	cp ../../materials/linters/.clang-format ./
	clang-format -n *.c *.h

clean:
	rm -rf *.o

rebuild: all
