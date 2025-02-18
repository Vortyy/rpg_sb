CC = clang
CFLAGS = -g
LIBS = -lglfw3 -lm -lGL -lXxf86vm -lXrandr -lXi

INCLUDE_PATH := ./include/
LIB_PATH := ./lib/
OBJS = glad.o entry.o 

all : exec 

# Make the exe by compiling all obj inside build together and add link to library
exec : build  
	$(CC) $(wildcard ./build/*.o) $(CFLAGS) -o rpg -L $(LIB_PATH) -Wl,-rpath=$(LIB_PATH) $(LIBS)

build : dir $(OBJS)

dir:
	if [ ! -d "./build" ]; then mkdir build; fi

# Build all the required obj with their equivalent .c file in src
# use automatic var $< give the first prerequisite
# and $@ give each $@ each target
%.o : ./src/%.c
	$(CC) -c $< -o ./build/$@ -I $(INCLUDE_PATH) 

## TO BE REMOVED
testing:
	$(CC) src/rpg.c -o test -I ./include -L ./lib -lsqlite3

clean :
	rm -rf ./build rpg test
