ldflags=-lmosquitto

all: pub sub

pub: pub.c
	$(CC)  $< -o $@ $(ldflags) -g


sub: sub.c
	$(CC)  $< -o $@ $(ldflags) -g

clean:
	rm pub sub
