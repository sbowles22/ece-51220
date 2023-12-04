# Basics
WARNING = -Wall -Wshadow --pedantic
ERROR = # -Wvla -Werror
GCC = gcc -std=c99 -lm $(WARNING) $(ERROR) #  -fopenmp
OPTIM = -O3
VAL = valgrind --tool=memcheck --leak-check=full --verbose

# Dependent on program
TESTFLAGS = # -DPARALLEL

# EXEC name is the name of the final executable
EXEC_NAME = pa3
# SRCS is the names of all source dependenies
SRCS = $(EXEC_NAME).c graph.c file_io.c utils.c
OBJS = $(SRCS:%.c=%.o)

$(EXEC_NAME): $(OBJS)
	@echo $(OBJS)
	$(GCC) $(OPTIM) $(TESTFLAGS) $(OBJS) -o $(EXEC_NAME)

.c.o:
	$(GCC) $(OPTIM) $(TESTFLAGS) -c $*.c

debug:
	$(eval OPTIM = -g)
	
$(EXEC_NAME)_debug: debug $(EXEC_NAME)

opt:
	$(eval OPTIM = -O3)

test_memory: clean debug $(EXEC_NAME)
	$(VAL) --log-file=memcheck.txt --leak-check=full --show-leak-kinds=all ./$(EXEC_NAME) examples/500_2000.gr outputs/500_2000.mcm outputs/500_2000.cycle
# $(VAL) --log-file=memcheck.txt ./$(EXEC_NAME) examples/7_8.gr outputs/7_8.mcm outputs/7_8.cycle
# scripts/mcmdiff outputs/500_2000.mcm examples/7_8.mcm
	

# Tests
testall: clean debug test_1 test_2 test_3
testtime: clean test_time_100 test_time_200 test_time_500 test_time_10k test_time_50k test_time_200k
 
test_1: $(EXEC_NAME)
	./$(EXEC_NAME) examples/7_8.gr outputs/7_8.mcm outputs/7_8.cycle
	scripts/mcmdiff outputs/7_8.mcm examples/7_8.mcm
	./scripts/cyclediff.py outputs/7_8.cycle examples/7_8.cycle
	
test_2: $(EXEC_NAME)
	./$(EXEC_NAME) examples/neg7_8.gr outputs/neg7_8.mcm outputs/neg7_8.cycle
	scripts/mcmdiff outputs/neg7_8.mcm examples/neg7_8.mcm
	./scripts/cyclediff.py outputs/neg7_8.cycle examples/neg7_8.cycle

test_3: $(EXEC_NAME)
	./$(EXEC_NAME) examples/10_40.gr outputs/10_40.mcm outputs/10_40.cycle
	scripts/mcmdiff outputs/10_40.mcm examples/10_40.mcm
	./scripts/cyclediff.py outputs/10_40.cycle examples/10_40.cycle

test_time_100: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/100_400.gr outputs/100_400.mcm outputs/100_400.cycle
	scripts/mcmprint outputs/100_400.mcm
	cat outputs/100_400.cycle

test_time_200: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/200_800.gr outputs/200_800.mcm outputs/200_800.cycle
	scripts/mcmprint outputs/200_800.mcm
	cat outputs/200_800.cycle

test_time_500: $(EXEC_NAME)
	time ./$(EXEC_NAME) examples/500_2000.gr outputs/500_2000.mcm outputs/500_2000.cycle
	scripts/mcmprint outputs/500_2000.mcm
	cat outputs/500_2000.cycle

test_time_10k: $(EXEC_NAME)
	time ./$(EXEC_NAME) tests/10000_40000.gr outputs/10000_40000.mcm outputs/10000_40000.cycle
	scripts/mcmprint outputs/10000_40000.mcm
	cat outputs/10000_40000.cycle

test_time_50k: $(EXEC_NAME)
	time ./$(EXEC_NAME) tests/50000_400000.gr outputs/50000_400000.mcm outputs/50000_400000.cycle
	scripts/mcmprint outputs/50000_400000.mcm
	cat outputs/50000_400000.cycle

test_time_200k: $(EXEC_NAME)
	time ./$(EXEC_NAME) tests/200000_3200000.gr outputs/200000_3200000.mcm outputs/200000_3200000.cycle
	scripts/mcmprint outputs/200000_3200000.mcm
	cat outputs/200000_3200000.cycle

# Clean compiled and outputs
clean: # remove all machine generated files
	rm -f $(EXEC_NAME) *.o 
	rm -f outputs/*
	rm -f memcheck*.txt
	rm -f $(EXEC_NAME).zip

wc:
	@wc *.c *.h

skrpt:
	gcc -std=c99 -pedantic -Wvla -Wall -Wshadow -g scripts/mcmdiff.c -o scripts/mcmdiff
	gcc -std=c99 -pedantic -Wvla -Wall -Wshadow -g scripts/mcmprint.c -o scripts/mcmprint

final_check: clean $(EXEC_NAME)
	./$(EXEC_NAME) examples/7_8.gr outputs/7_8.mcm outputs/7_8.cycle
	scripts/mcmdiff outputs/7_8.mcm examples/7_8.mcm
	./scripts/cyclediff.py outputs/7_8.cycle examples/7_8.cycle

	time ./$(EXEC_NAME) tests/200000_3200000.gr outputs/200000_3200000.mcm outputs/200000_3200000.cycle
	scripts/mcmprint outputs/200000_3200000.mcm
	cat outputs/200000_3200000.cycle

	zip $(EXEC_NAME) Makefile *.c *.h
