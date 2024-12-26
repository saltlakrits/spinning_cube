CC=clang
OUT = cube
CFLAGS=-Isrc/
SRCDIR = src/
#OBJDIR = .obj/
CFILES = $(wildcard $(SRCDIR)*.c)
OBJS = $(CFILES:.c=.o)

LDLIBS = -lncursesw -lm -lpthread
#@mkdir -p .obj


	#@./$(OUT)

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDLIBS)
	@printf "\n === Compiling program & deleting .o files ===\n"
	@rm -rf $(SRCDIR)*.o
