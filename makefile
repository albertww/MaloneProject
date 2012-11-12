# main directory makefile
DEBUG = -g
export DEBUG

.PHONY: all
all: common mclient mserver

.PHONY: common
common:
	cd common && make;

.PHONY: client
mclient:
	cd client && make; echo $(MAKELEVEL)

.PHONY: server
mserver:
	cd server && make; echo $(MAKELEVEL)

.PHONY: clean
clean:
	cd common && make clean && cd ..; cd server && make clean && cd ..; cd client && make clean&& cd ..;

.PHONY: compile
compile:
	cd common && make compile&& cd ..; cd server && make compile&& cd ..; cd client && make compile&& cd ..;

.PHONY: param
param:
	cd common && make param&& cd ..; cd server && make param&& cd ..; cd client && make param&& cd ..;

