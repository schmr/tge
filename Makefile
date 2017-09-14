# Copyright 2000, David Rhodes and Robert Dick
# All rights reserved.
# Use with GNU make.

CC := g++
TO := -o

#C_OPTS := -O0 -ffor-scope -ftemplate-depth-50 -std=c++98
C_OPTS := -O0 -ffor-scope -ftemplate-depth-50 -std=c++98 \
  -DROB_DEBUG -ggdb -O0 -pedantic -I. -Wno-deprecated

L_OPTS := -lstdc++ -lm

# Debug
C_OPTS += -ggdb -DROB_DEBUG

# Profile
#C_OPTS += -p
#L_OPTS += -p -lc_p

# Optimize
# C_OPTS += -s -O3

COMPILE := $(CC) -c $(OPTS) $(C_OPTS)

LINK := $(CC) $(OPTS) $(L_OPTS)

############################################################################## 
# Rules
############################################################################## 
%.o : %.l
	@echo "***** Lexing -> $@"
	@flex $*.l
	mv lex.yy.c $*.cc
	$(COMPILE) $*.cc   
%.o : %.y
	@echo "***** Yaccing -> $@"
	bison -d $*.y
	mv $*.tab.c $*.cc  
	$(COMPILE) $*.cc   
%.o : %.cc
	@echo "***** Compiling -> $@"
	$(COMPILE) $(TO) $@ $<

%.d : %.cc
	@echo "***** Depending -> $@"
	sh -ec '$(DEPENDS) $< | $(SED_DEPEND) > $@'

tge: gram.o scan.o ArgPack.o database.o dbio.o fgraph.o function.o Graph.o Interface.o ktime.o main.o node.o parse.o RStd.o RString.o tree.o
	$(REPORT)
	@echo "***** Linking -> $@"
	$(LINK) $(TO) $@ $(filter %.o,$^)
	@echo

ArgPack.o: ArgPack.cc ArgPack.h RVector.h RStd.h RStd.cct Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct RString.h \
 RString.cct
Graph.o: Graph.cc Graph.h RStd.h RStd.cct RVector.h Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct Graph.cct
Interface.o: Interface.cc Interface.h RStd.h RStd.cct RFunctional.h \
 RFunctional.cct Interface.cct
RStd.o: RStd.cc RStd.h RStd.cct
RString.o: RString.cc RString.h RFunctional.h RFunctional.cct \
 RString.cct RVector.h RStd.h RStd.cct Interface.h Interface.cct \
 RVector.cct
database.o: database.cc database.h RVector.h RStd.h RStd.cct \
 Interface.h RFunctional.h RFunctional.cct Interface.cct RVector.cct \
 RString.h RString.cct function.h node.h ArgPack.h Graph.h Graph.cct \
 ktime.h tree.h fgraph.h
dbio.o: dbio.cc database.h RVector.h RStd.h RStd.cct Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct RString.h \
 RString.cct function.h node.h ArgPack.h Graph.h Graph.cct ktime.h \
 tree.h fgraph.h
fgraph.o: fgraph.cc fgraph.h RVector.h RStd.h RStd.cct Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct RString.h \
 RString.cct Graph.h Graph.cct
function.o: function.cc function.h RVector.h RStd.h RStd.cct \
 Interface.h RFunctional.h RFunctional.cct Interface.cct RVector.cct \
 RString.h RString.cct node.h ArgPack.h Graph.h Graph.cct ktime.h
ktime.o: ktime.cc ktime.h
main.o: main.cc ArgPack.h database.h RVector.h RStd.h RStd.cct \
 Interface.h RFunctional.h RFunctional.cct Interface.cct RVector.cct \
 RString.h RString.cct function.h node.h Graph.h Graph.cct ktime.h \
 tree.h fgraph.h
node.o: node.cc node.h ArgPack.h RVector.h RStd.h RStd.cct Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct RString.h \
 RString.cct Graph.h Graph.cct ktime.h
parse.o: parse.cc parse.h database.h RVector.h RStd.h RStd.cct \
 Interface.h RFunctional.h RFunctional.cct Interface.cct RVector.cct \
 RString.h RString.cct function.h node.h ArgPack.h Graph.h Graph.cct \
 ktime.h tree.h fgraph.h
tree.o: tree.cc tree.h RVector.h RStd.h RStd.cct Interface.h \
 RFunctional.h RFunctional.cct Interface.cct RVector.cct RString.h \
 RString.cct ArgPack.h database.h function.h node.h Graph.h Graph.cct \
 ktime.h fgraph.h

