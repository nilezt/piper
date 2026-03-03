CC = gcc
CFLAGS = -Wall -Wextra -O2

all: ensure-piper tts

tts: main.o tts.o
	$(CC) $(CFLAGS) -o tts main.o tts.o -lpthread

# download a piper binary if one isn't already present
ensure-piper:
	@[ -x ./piper ] || ./fetch_piper.sh || true

main.o: main.c piper_tts/tts.h
	$(CC) $(CFLAGS) -c main.c

tts.o: piper_tts/tts.c piper_tts/tts.h
	$(CC) $(CFLAGS) -c piper_tts/tts.c

run: tts
	./tts

clean:
	rm -f tts main.o tts.o output.wav
