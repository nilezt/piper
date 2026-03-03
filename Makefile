CC = gcc
CFLAGS = -Wall -Wextra -O2

all: ensure-piper tts

tts: main.o
	$(CC) $(CFLAGS) -o tts main.o

# download a piper binary if one isn't already present
ensure-piper:
	@[ -x ./piper ] || ./fetch_piper.sh || true

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o tts output.wav
