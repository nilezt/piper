#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piper_tts/tts.h"

int main(int argc, char *argv[]) {
    if (tts_init() != 0) {
        return 1;
    }

    char text[1024];
    while (1) {
        printf("\nEnter text (or 'exit' to quit): ");
        if (fgets(text, sizeof(text), stdin) == NULL) break;
        text[strcspn(text, "\r\n")] = 0;

        if (strcmp(text, "exit") == 0) break;
        if (strlen(text) == 0) continue;

        printf("Generating 'output.wav'...\n");
        if (tts_generate(text, "output.wav") != 0) {
            fprintf(stderr, "Failed to generate TTS.\n");
        }
    }

    tts_free();
    return 0;
}
