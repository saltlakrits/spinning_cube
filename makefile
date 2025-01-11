CC=clang
OUT = cube
CFLAGS=-Isrc/ -O3
SRCDIR = src/
#OBJDIR = .obj/
CFILES = $(wildcard $(SRCDIR)*.c)
OBJS = $(CFILES:.c=.o)

LDLIBS = -lnotcurses -lnotcurses-core -lm -lpthread
#@mkdir -p .obj


	#@./$(OUT)

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDLIBS)
	@printf "\n === Compiling program & deleting .o files ===\n"
	@rm -rf $(SRCDIR)*.o
