#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

/**
 * @brief Initializes the MQTT client and connects to the broker.
 * 
 * @param host Broker hostname.
 * @param port Broker port.
 * @param user Username.
 * @param pass Password.
 * @param sub_topic Topic to subscribe to for text.
 * @param pub_topic Topic to publish status updates to.
 * @return int 0 on success, non-zero on failure.
 */
int mqtt_manager_init(const char *host, int port, const char *user, const char *pass, 
                     const char *sub_topic, const char *pub_topic);

/**
 * @brief Publishes a message to the status topic.
 * 
 * @param message The message to publish.
 * @return int 0 on success, non-zero on failure.
 */
int mqtt_manager_publish(const char *message);

/**
 * @brief Shuts down the MQTT client.
 */
void mqtt_manager_free(void);

#endif // MQTT_CLIENT_H
