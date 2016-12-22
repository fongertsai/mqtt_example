ldflags=-lmosquitto -Xlinker -Map=output.map

all: pub sub

pub: pub.c
	$(CC)  $< -o $@ $(ldflags) -g


sub: sub.c
	$(CC)  $< -o $@ $(ldflags) -g

clean:
	rm pub sub

static:
	gcc pub.c -L /home/fonger/my_source/mqtt_can_debug/lib -I /home/fonger/my_source/mqtt_can_debug/lib/ -lmosquitto -lpthread -lrt -lssl -lcrypto -lpthread -lcares -ldl -lz -static-libgcc -g
