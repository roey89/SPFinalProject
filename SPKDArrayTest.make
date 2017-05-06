CC = gcc
OBJS = sp_kdarray_unit_test.o SPPoint.o KD_ARRAY.o KD_NODE.o
EXEC = sp_kdarray_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
sp_kdarray_unit_test.o: $(TESTS_DIR)/sp_kdarray_unit_test.c $(TESTS_DIR)/sp_kdarray_unit_test.h KD_ARRAY.h SPPoint.h KD_NODE.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
KD_ARRAY.o: KD_ARRAY.c KD_ARRAY.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
KD_NODE.o: KD_NODE.c KD_NODE.h KD_ARRAY.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
