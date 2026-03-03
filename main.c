#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char text[1024];
    printf("Enter text: ");
    if (fgets(text, sizeof(text), stdin) == NULL) return 1;
    text[strcspn(text, "\r\n")] = 0;

    // DEFINED PATHS BASED ON YOUR WORKSPACE
    const char *piper_executable = "./piper.bin";
    const char *library_path     = "./piper";
    const char *espeak_data      = "./piper/espeak-ng-data";
    const char *model            = "en_US-amy-low.onnx";

    // 1. Ensure the binary is executable
    chmod(piper_executable, 0755);

    // 2. Check if the model exists (very important for sound!)
    if (access(model, R_OK) != 0) {
        fprintf(stderr, "Error: Model '%s' not found in current folder.\n", model);
        return 1;
    }

    // 3. Build the command
    // We tell Linux to look in ./piper for libraries, then run ./piper.bin
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "LD_LIBRARY_PATH=%s %s --espeak_data %s -m %s -f output.wav",
             library_path, piper_executable, espeak_data, model);

    printf("Executing Piper...\n");

    // 4. Open the pipe and send text
    FILE *fp = popen(cmd, "w");
    if (!fp) {
        perror("popen failed");
        return 1;
    }

    fprintf(fp, "%s", text);
    
    // 5. pclose() waits for the audio to finish rendering
    int status = pclose(fp);

    if (status == 0) {
        printf("\nSuccess! 'output.wav' is ready.\n");
    } else {
        printf("\nError: Piper exited with code %d.\n", status);
    }

    return 0;
}
