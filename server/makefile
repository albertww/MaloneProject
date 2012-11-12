#dfs server directory makefile
# VPATH provide search dir for a object, e.g impacket.cpp is not in current dir so 
# it shouldn't be found theoretically, when compile it make will ecount an error.
# but when I add the dir in VPATH, it will search dir in VPATH, 
# it will be found in dir ../common/
VPATH = ./:../common/
EXENAME := dfsserver
EXT := 
#DEPENDSRC = Common.cpp Epoll.cpp
DEPENDOBJ = Common.o Epoll.o RPCPacket.o StringUtility.o
#SRCFILE := $(DEPENDSRC)
SRCFILE += $(wildcard *.cpp)
OBJFILE := $(patsubst %cpp, %o, $(SRCFILE))
#OBJFILE += $(DEPENDOBJ)
ALLOBJ := $(OBJFILE)
ALLOBJ += $(DEPENDOBJ)
LIBS := -lpthread

$(EXENAME)$(EXT): $(ALLOBJ)
	$(CXX) $(DEBUG) -o $@ $^ $(LIBS)

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
