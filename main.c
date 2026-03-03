#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "piper_tts/tts.h"
#include "mqtt_client/mqtt_client.h"

int main(int argc, char *argv[]) {
    const char *broker_host = "9ae83b11af774f949d3428a9e2b0aca0.s2.eu.hivemq.cloud";
    int broker_port = 8883;
    const char *user = "dnp634";
    const char *pass = "Motorola123";
    const char *sub_topic = "alert_msg";
    const char *pub_topic = "alert_status";

    if (tts_init() != 0) {
        return 1;
    }

    printf("Initializing MQTT...\n");
    if (mqtt_manager_init(broker_host, broker_port, user, pass, sub_topic, pub_topic) != 0) {
        fprintf(stderr, "MQTT initialization failed.\n");
        tts_free();
        return 1;
    }

    printf("System ready. Subscribed to '%s'.\n", sub_topic);
    printf("Type commands below (or 'exit' to quit):\n");

    char text[1024];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (fgets(text, sizeof(text), stdin) == NULL) break;
        text[strcspn(text, "\r\n")] = 0;

        if (strcmp(text, "exit") == 0) break;
        if (strlen(text) == 0) continue;

        printf("Generating 'output.wav' for manual input...\n");
        if (tts_generate(text, "output.wav") != 0) {
            fprintf(stderr, "Failed to generate TTS.\n");
        }
    }

    mqtt_manager_free();
    tts_free();
    return 0;
}
