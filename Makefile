# this is a comment
SRC=WDaliClock.cpp daliclock.cpp scrnsave.c
OBJ=$(SRC:.cpp=.o) WDaliClockRc.o # replaces the .c from SRC with .o
EXE=hello.scr

CC=gcc
CPPFLAGS=-Wall -g -D SCREENSAVER
LDFLAGS=-mwindows -static -static-libgcc -static-libstdc++ -lpthread -lscrnsave
RM=rm

%.o: %.c         # combined w/ next line will compile recently changed .c files
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY : all     # .PHONY ignores files named all
all: $(EXE)      # all is dependent on $(EXE) to be complete

$(EXE): $(OBJ)   # $(EXE) is dependent on all of the files in $(OBJ) to exist
	$(CC) $(OBJ) $(LDFLAGS) -o $@

.PHONY : clean   # .PHONY ignores files named clean
clean:
	-$(RM) $(OBJ)    # '-' causes errors not to exit the process
