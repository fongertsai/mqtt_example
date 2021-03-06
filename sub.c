#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

struct timespec start_time;
struct timespec end_time;

#include <mosquitto.h>

#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;
static int recv = 0;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	recv++;
	//printf("RECV = %d\n",recv);
/*
	if (recv < 599975) {
		return;
	}
*/
	clock_gettime(CLOCK_REALTIME, &end_time);
	if(end_time.tv_nsec > start_time.tv_nsec)
	{
		printf("[diff_time:%ld.%09ld sec]\n",
				end_time.tv_sec - start_time.tv_sec,
				end_time.tv_nsec - start_time.tv_nsec);
	}
	else
	{
		printf("[diff_time:%ld.%09ld sec]\n",
				end_time.tv_sec - start_time.tv_sec - 1,
				end_time.tv_nsec - start_time.tv_nsec + 1000*1000*1000);
	}
	//bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
	printf("RECV = %d\n",recv);
	//clock_gettime(CLOCK_REALTIME, &end_time);
	//printf("s_time.tv_sec:%ld, s_time.tv_nsec:%09ld\n\n", start_time.tv_sec, start_time.tv_nsec);
	clock_gettime(CLOCK_REALTIME, &start_time);
	/*
	   mosquitto_topic_matches_sub("/devices/wb-adc/controls/+", message->topic, &match);
	   if (match) {
	   printf("got message for ADC topic\n");
	   }
	 */
}

int main(int argc, char *argv[])
{
	uint8_t reconnect = true;
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;
	int chk_recved = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	//snprintf(clientid, 23, "mysql_log_%d", getpid());
	sprintf(clientid, "Subcriber-%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);

	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

	    rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
		mosquitto_max_inflight_messages_set( mosq, 9000000);

		mosquitto_subscribe(mosq, &chk_recved, "ABC", 2);

		//start thread
		mosquitto_loop_start(mosq);

		while(run){
			//rc = mosquitto_loop(mosq, -1, 1);
			rc = 0;
			if(run && rc){
				printf("connection error!\n");
				sleep(10);
				mosquitto_reconnect(mosq);
			}
		}
		mosquitto_loop_stop(mosq, 1);
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();
	printf("REVC : %d MSGs\n",recv);

	return rc;
}

