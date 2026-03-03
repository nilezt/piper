#include "mqtt_client.h"
#include "../piper_tts/tts.h"
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mosquitto *mosq = NULL;
static char *status_topic = NULL;

void on_connect(struct mosquitto *m, void *obj, int rc) {
    (void)obj;
    if (rc == 0) {
        printf("MQTT: Connected successfully.\n");
        mosquitto_subscribe(m, NULL, (const char *)obj, 0);
    } else {
        fprintf(stderr, "MQTT: Connection failed with code %d\n", rc);
    }
}

void on_message(struct mosquitto *m, void *obj, const struct mosquitto_message *msg) {
    (void)m;
    (void)obj;
    if (msg->payloadlen > 0) {
        char *text = malloc(msg->payloadlen + 1);
        memcpy(text, msg->payload, msg->payloadlen);
        text[msg->payloadlen] = '\0';
        
        printf("MQTT: Received message on topic %s: %s\n", msg->topic, text);
        
        // Generate TTS
        if (tts_generate(text, "output.wav") == 0) {
            mqtt_manager_publish("TTS generation started for received message.");
        } else {
            mqtt_manager_publish("TTS generation failed.");
        }
        
        free(text);
    }
}

int mqtt_manager_init(const char *host, int port, const char *user, const char *pass, 
                     const char *sub_topic, const char *pub_topic) {
    mosquitto_lib_init();
    
    mosq = mosquitto_new(NULL, true, (void *)sub_topic);
    if (!mosq) {
        fprintf(stderr, "MQTT: Failed to create mosquitto instance.\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (user && pass) {
        mosquitto_username_pw_set(mosq, user, pass);
    }

    // Set TLS
    // HiveMQ Cloud requires TLS. We use the system CA certs.
    int rc = mosquitto_tls_set(mosq, "/etc/ssl/certs/ca-certificates.crt", NULL, NULL, NULL, NULL);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "MQTT: Failed to set TLS: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    status_topic = strdup(pub_topic);

    rc = mosquitto_connect(mosq, host, port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "MQTT: Failed to connect: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // Start network loop in a background thread provided by libmosquitto
    mosquitto_loop_start(mosq);

    return 0;
}

int mqtt_manager_publish(const char *message) {
    if (!mosq || !status_topic) return 1;
    int rc = mosquitto_publish(mosq, NULL, status_topic, strlen(message), message, 0, false);
    return (rc == MOSQ_ERR_SUCCESS) ? 0 : 1;
}

void mqtt_manager_free(void) {
    if (mosq) {
        mosquitto_disconnect(mosq);
        mosquitto_loop_stop(mosq, false);
        mosquitto_destroy(mosq);
        mosq = NULL;
    }
    if (status_topic) {
        free(status_topic);
        status_topic = NULL;
    }
    mosquitto_lib_cleanup();
}
