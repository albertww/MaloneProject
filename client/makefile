#dfs client directory makefile
# VPATH provide search dir for a object, e.g impacket.cpp is not in current dir so 
# it shouldn't be found theoretically, when compile it make will ecount an error.
# but when I add the dir in VPATH, it will search dir in VPATH, 
# it will be found in dir ../common/
VPATH = ./:../common/
EXENAME := dfsclient
EXT := 
#DEPENDSRC = Common.cpp
DEPENDOBJ = Common.o
SRCFILE := $(DEPENDSRC)
SRCFILE += $(wildcard *.cpp)
OBJFILE := $(patsubst %cpp, %o, $(SRCFILE))
#OBJFILE += $(DEPENDOBJ)
ALLOBJ := $(OBJFILE)
ALLOBJ += $(DEPENDOBJ)

$(EXENAME)$(EXT): $(ALLOBJ)
	$(CXX) $(DEBUG) -o $@ $^

$(OBJFILE): %.o: %.cpp
	$(CXX) $(DEBUG) -I../common -c $<

.phony: compile
compile: $(OBJFILE)

.phony: clean
clean:
	-rm $(OBJFILE); rm $(EXENAME)$(EXT)

.phony: param
param:
	@echo $(SRCFILE); echo $(OBJFILE);
