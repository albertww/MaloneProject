
ELFNAME=testlog
OBJ=timeconvert.o MaloneLog.o testlog.o
#FLAGS=-D_WINDOWS -D_DEBUG
FLAGS=-D_WINDOWS 
LIBS=-lpthread

$(ELFNAME): $(OBJ)
	g++ $(FLAGS) -g -o $@ $^ $(LIBS)
	
$(OBJ): %.o: %.cpp
	g++ $(FLAGS) -g -c $<
	
.PHONY: clean
clean:
	-rm $(ELFNAME) $(OBJ)
