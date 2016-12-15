#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <mosquitto.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
	static int message_count;
	message_count ++;
	printf(" HI%d\n ", message_count);
}

int main(int argc, char *argv[])
{
	uint8_t reconnect = true;
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "mysql_log_%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);

	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_publish_callback_set(mosq, my_publish_callback);
		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
		mosquitto_max_inflight_messages_set( mosq, 10000);


		int i;
		char msg[128];
		for ( i = 0; i < 100; i++) {
			sprintf(msg, "Hello %d\n", i);
			int ret = mosquitto_publish(mosq, NULL, "ABC", strlen(msg), msg,1,false);
			if (ret != MOSQ_ERR_SUCCESS) {
				printf("SEND %d ERROR\n", i);
			}
		}
		sleep(10);
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();

	return rc;
}

