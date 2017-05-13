CC = gcc
OBJS = sp_kdarray_unit_test.o SPPoint.o SPBPriorityQueue.o SPKDArray.o
EXEC = sp_kdarray_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
sp_kdarray_unit_test.o: $(TESTS_DIR)/sp_kdarray_unit_test.c $(TESTS_DIR)/unit_test_util.h \
SPPoint.h SPBPriorityQueue.h SPKDArray.h
	$(CC) $(COMP_FLAG) -c -g $(TESTS_DIR)/$*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(COMP_FLAG) -c -g $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(COMP_FLAG) -c -g $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h
	$(CC) $(COMP_FLAG) -c -g $*.c
clean:
	rm -f $(OBJS) $(EXEC)
