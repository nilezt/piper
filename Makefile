CC = gcc
CFLAGS = -Wall -Wextra -O2

all: ensure-piper tts

tts: main.o tts.o mqtt_client.o
	$(CC) $(CFLAGS) -o tts main.o tts.o mqtt_client.o -lpthread -lmosquitto

# download a piper binary if one isn't already present
ensure-piper:
	@[ -x ./piper ] || ./fetch_piper.sh || true

main.o: main.c piper_tts/tts.h mqtt_client/mqtt_client.h
	$(CC) $(CFLAGS) -c main.c

tts.o: piper_tts/tts.c piper_tts/tts.h
	$(CC) $(CFLAGS) -c piper_tts/tts.c

mqtt_client.o: mqtt_client/mqtt_client.c mqtt_client/mqtt_client.h piper_tts/tts.h
	$(CC) $(CFLAGS) -c mqtt_client/mqtt_client.c

run: tts
	./tts

clean:
	rm -f tts main.o tts.o mqtt_client.o output.wav
