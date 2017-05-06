CC = gcc
OBJS = sp_config_unit_test.o SPConfig.o SPLogger.o
EXEC = sp_config_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
sp_config_unit_test.o: $(TESTS_DIR)/sp_config_unit_test.c $(TESTS_DIR)/unit_test_util.h SPConfig.h SPLogger.h
	$(CC) $(COMP_FLAG) -c -g $(TESTS_DIR)/$*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(COMP_FLAG) -c -g $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(COMP_FLAG) -c -g $*.c
clean:
	rm -f $(OBJS) $(EXEC)
