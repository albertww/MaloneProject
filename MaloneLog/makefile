
ELFNAME=testlog
OBJ=MaloneLog.o testlog.o

$(ELFNAME): $(OBJ)
	g++ -g -o $@ $^
	
$(OBJ): %.o: %.cpp
	g++ -g -c $<
	
.PHONY: clean
clean:
	-rm $(ELFNAME) $(OBJ)
