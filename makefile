SOURCEC=threadSimulator.c

TARGETC=threadSimulator

.PHONY: all c cpp

all: c

c:
	$(CC) $(SOURCEC) -o $(TARGETC)

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)