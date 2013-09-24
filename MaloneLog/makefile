
ELFNAME=testlog
OBJ=timeconvert.o MaloneLog.o testlog.o
FLAGS=-D_WINDOWS

$(ELFNAME): $(OBJ)
	g++ $(FLAGS) -g -o $@ $^
	
$(OBJ): %.o: %.cpp
	g++ $(FLAGS) -g -c $<
	
.PHONY: clean
clean:
	-rm $(ELFNAME) $(OBJ)
