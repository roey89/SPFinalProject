CC = gcc
CPP = g++
#put all your object files here
OBJS = main.o main_aux.o SPImageProc.o SPConfig.o SPLogger.o \
SPPoint.o SPBPriorityQueue.o SPKDArray.o SPKDNode.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPConfig.h SPLogger.h SPKDNode.h SPKDArray.h SPPoint.h \
SPBPriorityQueue.h SPImageProc.h main_aux.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c -g $*.cpp
main_aux.o: main_aux.cpp SPConfig.h SPLogger.h SPPoint.h main_aux.h SPImageProc.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c -g $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPLogger.h SPPoint.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c -g $*.cpp
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c -g $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c -g $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c -g $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c -g $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c -g $*.c
SPKDNode.o: SPKDNode.c SPKDNode.h SPKDArray.h SPPoint.h SPBPriorityQueue.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c -g $*.c
clean:
	rm -f $(OBJS) $(EXEC)
	
#a rule for building a simple c++ source file
#use g++ -MM SPImageProc.cpp to see dependencies
#a rule for building a simple c source file
#use "gcc -MM SPPoint.c" to see the dependencies
