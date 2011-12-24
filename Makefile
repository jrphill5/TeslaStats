C=gcc
CFLAGS=-Wall -lm
PROJECT=TeslaStats

all:
	$(C) $(CFLAGS) $(PROJECT).c -o $(PROJECT)

clean:
	rm $(PROJECT)
