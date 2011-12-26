C=gcc
CFLAGS=-Wall -lm
PROJECT1=TeslaStats
PROJECT2=CapCalc

all:
	$(C) $(CFLAGS) $(PROJECT1).c -o $(PROJECT1)
	$(C) $(CFLAGS) $(PROJECT2).c -o $(PROJECT2)

clean:
	rm $(PROJECT1) $(PROJECT2)
