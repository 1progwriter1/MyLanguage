CFLAGS=-fsanitize=address,alignment -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -fPIE -Werror=vla

COMP=g++

OBJ_DIR=object_files
LIB_DIR=lib_obj
FRONT_DIR=frontend_src
GRAPH_DIR=graphviz
BACK_DIR=backend_src
MID_DIR=middle_end_src
INTEL_DIR=backend_intel

SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(wildcard *.o)

OBJ_MOVED=$(wildcard $(OBJ_DIR)/*.o)
LIB_OBJ=$(wildcard $(LIB_DIR)/*.o) lang_syntax.cpp

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


all: $(SOURCES)
	$(COMP) $(CFLAGS) -c $(SOURCES)
	mv $(OBJECTS) $(OBJ_DIR)/
	$(COMP) $(CFLAGS) $(OBJ_MOVED) $(LIB_OBJ)

front: $(FRONT_SRC) $(GRAPH_SRC) $(LIB_OBJ)
	$(COMP) $(CFLAGS) $(FRONT_SRC) $(GRAPH_SRC) $(LIB_OBJ) -o front.out


back: $(BACK_SRC) $(GRAPH_SRC) $(LIB_OBJ)
	$(COMP) $(CFLAGS) $(BACK_SRC) $(GRAPH_SRC) $(LIB_OBJ) -o back.out


intel: $(INTEL_SRC) $(GRAPH_SRC) $(LIB_OBJ)
	$(COMP) $(CFLAGS) $(INTEL_SRC) $(GRAPH_SRC) $(LIB_OBJ) -o intel.out


mid:
	$(COMP) $(CFLAGS) -c $(MID_SRC) $(GRAPH_SRC) middle_end.cpp
	$(COMP) $(CFLAGS) $(MID_OBJ) $(LIB_OBJ) $(GRAPH_OBJ) object_files/file_read_lang.o object_files/prog_output.o middle_end.o -o mid.out
	mv $(MID_OBJ) $(GRAPH_OBJ) middle_end.o $(OBJ_DIR)/

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