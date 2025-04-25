CFLAGS= #-fsanitize=address,alignment -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -fPIE -Werror=vla

COMP=g++

OBJ_DIR=object_files
LIB_DIR=lib_obj
LIB=lib_src
FRONT_DIR=frontend_src
GRAPH_DIR=graphviz
BACK_DIR=backend_src
MID_DIR=middle_end_src
INTEL_DIR=backend_intel
ELF_DIR=backend_elf

INCLUDE=-I include/

SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(wildcard *.o)

OBJ_MOVED=$(wildcard $(OBJ_DIR)/*.o)
LIB_OBJ=$(wildcard $(LIB_DIR)/*.o) data/lang_syntax.cpp
LIB_SRC=$(wildcard $(LIB)/*.cpp) data/lang_syntax.cpp

GRAPH_SRC=$(wildcard $(GRAPH_DIR)/*.cpp)
GRAPH_OBJ=$(patsubst $(GRAPH_DIR)/%.cpp, %.o, $(GRAPH_SRC))

FRONT_SRC=$(wildcard $(FRONT_DIR)/*.cpp)
FRONT_OBJ=$(patsubst $(FRONT_DIR)/%.cpp, %.o, $(FRONT_SRC))

BACK_SRC=$(wildcard $(BACK_DIR)/*.cpp)
BACK_OBJ=$(patsubst $(BACK_DIR)/%.cpp, %.o, $(BACK_SRC))

MID_SRC=$(wildcard $(MID_DIR)/*.cpp)
MID_OBJ=$(patsubst $(MID_DIR)/%.cpp, %.o, $(MID_SRC))

INTEL_SRC=$(wildcard $(INTEL_DIR)/*.cpp)
INTEL_OBJ=$(patsubst $(INTEL_DIR)/%.cpp, %.o, $(INTEL_SRC))

ELF_SRC=$(wildcard $(ELF_DIR)/*.cpp)
ELF_OBJ=$(patsubst $(ELF_DIR)/%.cpp, %.o, $(ELF_SRC))


all: front back mid intel


front: $(FRONT_SRC) $(GRAPH_SRC) $(LIB_SRC)
	$(COMP) $(CFLAGS) $(FRONT_SRC) $(GRAPH_SRC) $(LIB_SRC) $(INCLUDE) -o front.out
	rm -rf *.dSYM
	mv front.out my_g--/

back: $(BACK_SRC) $(GRAPH_SRC) $(LIB_SRC)
	$(COMP) $(CFLAGS) $(BACK_SRC) $(GRAPH_SRC) $(LIB_SRC) $(INCLUDE) -o back.out
	rm -rf *.dSYM
	mv back.out my_g--/

mid: $(MID_SRC) $(GRAPH_SRC) $(LIB_SRC)
	$(COMP) $(CFLAGS) $(MID_SRC) $(GRAPH_SRC) $(LIB_SRC) $(INCLUDE) -o mid.out
	rm -rf *.dSYM
	mv mid.out my_g--/


intel: $(INTEL_SRC) $(GRAPH_SRC) $(LIB_SRC)
	$(COMP) $(CFLAGS) $(INTEL_SRC) $(GRAPH_SRC) $(LIB_SRC) $(INCLUDE) -o intel.out
	rm -rf *.dSYM
	mv intel.out my_g--/

elf: $(ELF_SRC) $(GRAPH_SRC) $(LIB_OBJ)
	$(COMP) $(CFLAGS) $(ELF_SRC) $(GRAPH_SRC) $(LIB_OBJ) -o elf.out
	rm -rf *.dSYM

run_square:
	./front.out square.txt back.txt
	./mid.out  back.txt
	./back.out back.txt asm.txt
	./asm.out asm.txt square.bin
	./proc.out square.bin

run_fact:
	./front.out fact.txt back.txt
	./mid.out  back.txt
	./back.out back.txt asm.txt
	./asm.out asm.txt fact.bin
	./proc.out fact.bin

clean:
	rm -f $(OBJ_MOVED)

clean_obj:
	rm *.o

clean_exe:
	rm *.out

%.o : %.s
	nasm -f elf64 $< -o $@

%.out : %.o
	ld -s -o $@ backend_intel/my_printf.o backend_intel/my_scanf.o $<

