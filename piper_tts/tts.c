#include "tts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

static FILE *piper_fp = NULL;
static pthread_mutex_t tts_mutex = PTHREAD_MUTEX_INITIALIZER;

int tts_init(void) {
    const char *piper_executable = "./piper.bin";
    const char *library_path     = "./piper";
    const char *espeak_data      = "./piper/espeak-ng-data";
    const char *model            = "en_US-amy-low.onnx";

    // Ensure the binary is executable
    chmod(piper_executable, 0755);

    // Check if the model exists
    if (access(model, R_OK) != 0) {
        fprintf(stderr, "Error: Model '%s' not found.\n", model);
        return 1;
    }

    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "LD_LIBRARY_PATH=%s %s --espeak_data %s -m %s --json-input",
             library_path, piper_executable, espeak_data, model);

    printf("TTS: Starting persistent Piper process...\n");
    piper_fp = popen(cmd, "w");
    if (!piper_fp) {
        perror("tts_init: popen failed");
        return 1;
    }

    return 0;
}

int tts_generate(const char *text, const char *output_path) {
    if (!piper_fp) {
        fprintf(stderr, "TTS: System not initialized.\n");
        return 1;
    }

    pthread_mutex_lock(&tts_mutex);

    // Escape double quotes for JSON (very simple escaping for this demo)
    // In a production app, use a proper JSON library like cJSON.
    fprintf(piper_fp, "{\"text\": \"%s\", \"output_file\": \"%s\"}\n", text, output_path);
    fflush(piper_fp);

    pthread_mutex_unlock(&tts_mutex);

    return 0;
}

void tts_free(void) {
    if (piper_fp) {
        printf("TTS: Stopping Piper...\n");
        pclose(piper_fp);
        piper_fp = NULL;
    }
}
