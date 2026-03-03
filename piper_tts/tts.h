#ifndef TTS_H
#define TTS_H

/**
 * @brief Initializes the TTS system and starts the background Piper process.
 * 
 * @return int 0 on success, non-zero on failure.
 */
int tts_init(void);

/**
 * @brief Generates speech for the given text and saves it to the specified path.
 * 
 * @param text The text to synthesize.
 * @param output_path The destination WAV file path.
 * @return int 0 on success, non-zero on failure.
 */
int tts_generate(const char *text, const char *output_path);

/**
 * @brief Shuts down the TTS system and terminates the background process.
 */
void tts_free(void);

#endif // TTS_H
