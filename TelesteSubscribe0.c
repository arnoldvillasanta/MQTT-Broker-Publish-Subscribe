/*
# Function:  An MQTT subscriber that connects to a remote QMTT broker.
# Data:      Data received is "Unique ID + UTC + Message"
# Author:    Arnold L. Villasanta
# Date:      December 1, 2021
# Version:   Alpha 1.0
*/


//=======================================================================
//                    LIBRARIES
//=======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>



//=======================================================================
//                    FUNCTION:  MQTT CONNECTION EVENT
//=======================================================================

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	printf("ID: %d\n", * (int *) obj);
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "alertData/", 0);
}

//=======================================================================
//                    FUNCTION:  MQTT MESSAGE EVENT
//=======================================================================

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	printf("%s\n", (char *) msg->payload);
}


//=======================================================================
//                    MAIN FUNCTION
//=======================================================================


int main() {
	int rc, id=12;

	mosquitto_lib_init();

	struct mosquitto *mosq;

	mosq = mosquitto_new("TelesteSubscribe0", true, &id);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq, "192.168.254.100", 1883, 10);
	if(rc) {
		printf("Could not connect to MQTT broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);
	printf("Press Enter to quit!\n");
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}