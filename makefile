SOURCEC=threadSimulator.c

TARGETC=threadSimulator

.PHONY: all c cpp

all: c

c:
	$(CC) $(SOURCEC) -o $(TARGETC) `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)