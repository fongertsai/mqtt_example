ldflags=-lmosquitto

all: pub sub

pub: pub.c
	$(CC)  $< -o $@ $(ldflags)


sub: sub.c
	$(CC)  $< -o $@ $(ldflags)

clean:
	rm pub sub
