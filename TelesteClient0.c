/*
# Function: An MQTT publisher that connects to a remote QMTT broker.
# Data:     Data transmitted is "Unique ID + UTC + Message"
# Author:   Arnold L. Villasanta
# Date:     December 1, 2021
# Version:   Alpha 1.0
*/


//=======================================================================
//                    LIBRARIES
//=======================================================================

#include <stdio.h>
#include <mosquitto.h>
#include <time.h>


//=======================================================================
//                    MAIN FUNCTION
//=======================================================================

int main(){

    //===== Check accessibility of UTC
    
    time_t now = time(&now);
    if (now == -1) {       
        puts("The time() function failed");
    }
        
    struct tm *ptm = gmtime(&now);

    if (ptm == NULL) {
        
        puts("The gmtime() function failed");
    }    
    

    //===== Create Data 
    
    char dataBuffer[100];
    snprintf(dataBuffer, sizeof(dataBuffer), "TelesteClient-SN:000341 | %s | Message from RaspberryPi", asctime(ptm));
    puts(dataBuffer);


    //===== MQTT create, connect, transmit, disconnect

	int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("TelesteClient0", true, NULL);

	rc = mosquitto_connect(mosq, "192.168.254.100", 1883, 200);
	if(rc != 0){
		printf("TelesteClient can't connect to MQTT server! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
		return -1;
	}

	mosquitto_publish(mosq, NULL, "alertData/", 80, dataBuffer, 0, false);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();
	return 0;
}