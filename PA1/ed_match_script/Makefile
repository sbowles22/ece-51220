# ***
# *** DO NOT modify this file
# ***

# Basics
WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)
VAL = valgrind --tool=memcheck --leak-check=full --verbose

# Dependent on program
TESTFALGS = -DDEBUG

# EXEC name is the name of the final executable
EXEC_NAME = ed_match
# SRCS is the names of all source dependenies
SRCS = ed_match.c
OBJS = $(SRCS:%.c=%.o)

$(EXEC_NAME): $(OBJS)
	@echo $(OBJS)
	$(GCC) $(TESTFALGS) $(OBJS) -o $(EXEC_NAME)

.c.o:
	$(GCC) $(TESTFALGS) -c $*.c

# Clean compiled and outputs
clean: # remove all machine generated files
	rm -f $(EXEC_NAME) *.o 
	rm -f outputs/*.txt
	rm -f memcheck*.txt

wc:
	@wc *.c *.h
