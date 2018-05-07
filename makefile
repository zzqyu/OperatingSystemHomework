CC = gcc
OBJECTS = *.c  
#*.o
TARGET = testcase

$(TARGET) : $(OBJECTS)
		$(CC) -o  $(TARGET) $(OBJECTS)
		./$(TARGET) 
#		./$(TARGET) 1
#		./$(TARGET) 2
#		./$(TARGET) 3
#		./$(TARGET) 4
#		./$(TARGET) 5
		