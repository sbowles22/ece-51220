# ***
# *** DO NOT modify this file
# ***

# Basics
WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g -lm $(WARNING) $(ERROR)
VAL = valgrind --tool=memcheck --leak-check=full --verbose

# Dependent on program
TESTFALGS = # -DDEBUG

# EXEC name is the name of the final executable
EXEC_NAME = pa1
# SRCS is the names of all source dependenies
SRCS = pa1.c network.c file_io.c
OBJS = $(SRCS:%.c=%.o)

$(EXEC_NAME): $(OBJS)
	@echo $(OBJS)
	$(GCC) $(TESTFALGS) $(OBJS) -o $(EXEC_NAME)

.c.o:
	$(GCC) $(TESTFALGS) -c $*.c

testmemory: $(EXEC_NAME)
	$(VAL) --log-file=memcheck.txt ./$(EXEC_NAME) 1.0 examples/inv.param examples/wire.param examples/3.txt outputs/3.pre outputs/3.elmore outputs/3.ttopo outputs/3.btopo
	diff outputs/3.pre examples/3.pre

# Tests
testall: test_1 test_2 test_time_1 test_time_2 testmemory
 
test_1: $(EXEC_NAME)
	./$(EXEC_NAME) 1.0 examples/fake_inv.param examples/fake_wire.param examples/3.txt outputs/3.pre outputs/3.elmore outputs/3.ttopo outputs/3.btopo
	diff outputs/3.pre examples/3.pre
	./ed_match_script/ed_match outputs/3.elmore examples/3.elmore

test_2: $(EXEC_NAME)
	./$(EXEC_NAME) 0.00000005 examples/inv.param examples/wire.param examples/5.txt outputs/5.pre outputs/5.elmore outputs/5.ttopo outputs/5.btopo
	diff outputs/5.pre examples/5.pre
	./ed_match_script/ed_match outputs/5.elmore examples/5.elmore

test_time_1: $(EXEC_NAME)
	time ./$(EXEC_NAME) 1.0 examples/inv.param examples/wire.param examples/s1423.txt outputs/s1423.pre outputs/s1423.elmore outputs/s1423.ttopo outputs/s1423.btopo

test_time_2: $(EXEC_NAME)
	time ./$(EXEC_NAME) 1.0 examples/inv.param examples/wire.param examples/s5378.txt outputs/s5378.pre outputs/s5378.elmore outputs/s5378.ttopo outputs/s5378.btopo

# Clean compiled and outputs
clean: # remove all machine generated files
	rm -f $(EXEC_NAME) *.o 
	rm -f outputs/*.txt
	rm -f memcheck*.txt

wc:
	@wc *.c *.h

final_check: testall clean
	gcc -std=c99 -pedantic -Wvla -Wall -Wshadow -O3 *.c -o pa1 -lm
	./$(EXEC_NAME) 1.0 examples/inv.param examples/wire.param examples/5.txt outputs/5.pre outputs/5.elmore outputs/5.ttopo outputs/5.btopo
	diff outputs/5.pre examples/5.pre
	./ed_match_script/ed_match outputs/5.elmore examples/5.elmore
	zip $(EXEC_NAME).zip *.c *.h
