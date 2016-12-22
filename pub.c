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
	//printf("connect callback, rc=%d\n", result);
}

void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
	static int message_count;
	message_count ++;
	printf(" HI %d\n ", message_count);
}

int main(int argc, char *argv[])
{
	uint8_t reconnect = true;
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;
	int chk_sended = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	//snprintf(clientid, 23, "mysql_log_%d", getpid());
	sprintf(clientid, "Publisher-%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);

	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_publish_callback_set(mosq, my_publish_callback);
		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
		//mosquitto_max_inflight_messages_set( mosq, 9000000);
		mosquitto_max_inflight_messages_set( mosq, 0);

		mosquitto_loop_start(mosq);

		int i;
		char msg[128];
		for ( i = 0; i < atoi(argv[1]); i++) {
			//mosquitto_loop(mosq, -1, 1);
			sprintf(msg, "Hello %d", i);
			int len = strlen(msg);
			int ret = mosquitto_publish(mosq, &chk_sended, "ABC", len, msg,2,true);
			//printf("Send %d\n",chk_sended);
			if (ret != MOSQ_ERR_SUCCESS) {
				printf("SEND %d ERROR\n", i);
			}
			usleep(150);
		}
		while(1 && run) {
			printf("Q = %d\n", mosquitto_fonger(mosq));
			mosquitto_loop(mosq, -1, 1);
		}
		mosquitto_destroy(mosq);
		mosquitto_loop_stop(mosq,1);
	}

	mosquitto_lib_cleanup();

	return rc;
}

