# ***
# *** DO NOT modify this file
# ***

# Basics
WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g -lm $(WARNING) $(ERROR)
VAL = valgrind --tool=memcheck --leak-check=full --verbose

# Dependent on program
TESTFLAGS = -DDEBUG

# EXEC name is the name of the final executable
EXEC_NAME = pa2
# SRCS is the names of all source dependenies
SRCS = pa2.c layout.c ctree.c file_io.c utils.c
OBJS = $(SRCS:%.c=%.o)

$(EXEC_NAME): $(OBJS)
	@echo $(OBJS)
	$(GCC) $(TESTFLAGS) $(OBJS) -o $(EXEC_NAME)

.c.o:
	$(GCC) $(TESTFLAGS) -c $*.c


test_memory: $(EXEC_NAME)
	$(VAL) --log-file=memcheck.txt ./$(EXEC_NAME) examples/8.txt outputs/8_1.dim outputs/8_1.pck outputs/8_all.dim outputs/8_all.pck
	

# Tests
testall: test_1 test_2 test_3 test_4 test_5 test_memory
 
test_1: $(EXEC_NAME)
	./$(EXEC_NAME) examples/3.txt outputs/3_1.dim outputs/3_1.pck outputs/3_all.dim outputs/3_all.pck
	diff examples/3_1.dim outputs/3_1.dim
	diff examples/3_1.pck outputs/3_1.pck
	diff examples/3_all.dim outputs/3_all.dim
	diff examples/3_all.pck outputs/3_all.pck

test_2: $(EXEC_NAME)
	./$(EXEC_NAME) examples/8.txt outputs/8_1.dim outputs/8_1.pck outputs/8_all.dim outputs/8_all.pck
	diff examples/8_1.dim outputs/8_1.dim
	diff examples/8_1.pck outputs/8_1.pck
	diff examples/8_all.dim outputs/8_all.dim
	diff examples/8_all.pck outputs/8_all.pck

test_3: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/10_1.txt outputs/10_1_1.dim outputs/10_1_1.pck outputs/10_1_all.dim outputs/10_1_all.pck
	diff known_outputs/10_1_1.dim outputs/10_1_1.dim
	diff known_outputs/10_1_1.pck outputs/10_1_1.pck
	diff known_outputs/10_1_all.dim outputs/10_1_all.dim
	diff known_outputs/10_1_all.pck outputs/10_1_all.pck

test_4: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/100_3.txt outputs/100_3_1.dim outputs/100_3_1.pck outputs/100_3_all.dim outputs/100_3_all.pck
	diff known_outputs/100_3_1.dim outputs/100_3_1.dim
	diff known_outputs/100_3_1.pck outputs/100_3_1.pck
	diff known_outputs/100_3_all.dim outputs/100_3_all.dim
	diff known_outputs/100_3_all.pck outputs/100_3_all.pck

test_5: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/500_5.txt outputs/500_5_1.dim outputs/500_5_1.pck outputs/500_5_all.dim outputs/500_5_all.pck
	diff known_outputs/500_5_1.dim outputs/500_5_1.dim
	diff known_outputs/500_5_1.pck outputs/500_5_1.pck
	diff known_outputs/500_5_all.dim outputs/500_5_all.dim
	diff known_outputs/500_5_all.pck outputs/500_5_all.pck

# Clean compiled and outputs
clean: # remove all machine generated files
	rm -f $(EXEC_NAME) *.o 
	rm -f outputs/*
	rm -f memcheck*.txt

wc:
	@wc *.c *.h

final_check: testall clean
	gcc -std=c99 -pedantic -Wvla -Wall -Wshadow -O3 *.c -o $(EXEC_NAME) -lm
	./$(EXEC_NAME) examples/3.txt outputs/3_1.dim outputs/3_1.pck outputs/3_all.dim outputs/3_all.pck
	diff examples/3_1.dim outputs/3_1.dim
	diff examples/3_1.pck outputs/3_1.pck
	diff examples/3_all.dim outputs/3_all.dim
	diff examples/3_all.pck outputs/3_all.pck
	zip $(EXEC_NAME).zip *.[ch]
