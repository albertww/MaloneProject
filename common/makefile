# common directory makefile
SRCFILE += $(wildcard *.cpp)
OBJFILE := $(patsubst %cpp, %o, $(SRCFILE))

.PHONY: all
all: $(OBJFILE)

$(OBJFILE): %.o: %.cpp
	$(CXX) $(DEBUG) -c $<

.phony: compile
compile: $(OBJFILE)

.phony: param
param:
	@echo $(SRCFILE); echo $(OBJFILE);

.phony: clean
clean:
	-rm $(OBJFILE);
