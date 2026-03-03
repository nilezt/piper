# piper

# piper

This workspace contains a small C project demonstrating how to invoke the `piper` text‑to‑speech
command‑line tool from C code. The program reads a line of text from standard input and then
calls `piper tts` to produce a 16 kHz WAV file named `output.wav`.

## Building

```sh
# install a C compiler (gcc/clang) if you don't already have one
make
```

The `Makefile` produces a binary called `tts`.

## Usage

```sh
./tts
# enter your text at the prompt and press <Enter>
```

The program simply shells out to the `piper` CLI, so you must have `piper` available.
It will first look for `piper` on your `PATH` and if that fails it will try to execute
`./piper` in the current directory (which is handy after running `./fetch_piper.sh`).
Installation instructions for `piper` can be found in the
[piper repository](https://github.com/rhasspy/piper) or via your distribution's package
manager if one exists.  By default the demo uses the `en_US-amy-low.onnx` model, but you can
point it at any other model by editing `main.c` or invoking `piper` yourself.  Example command
line invoked by the demo:

```
piper tts -m en_US-amy-low.onnx --wave-formats wav16k -o output.wav "hello world"
```

(The model name may also be prefixed with a path, e.g. `models/en_US-amy-low.onnx` if you
store models locally.)

## Raspberry Pi 4

Although you’re developing on an x86 Linux desktop at the moment, the final target is a
Raspberry Pi 4 running 32‑bit Raspberry Pi OS (Bullseye/Bookworm) or similar.  A few points to
keep in mind:

* `piper` itself must be compiled for ARM; either build it on the Pi or use a prebuilt
  binary/package for the Pi 4.  Visit the [piper repo](https://github.com/rhasspy/piper) for
  Raspberry Pi installation instructions.
* The C demo is plain ANSI‑C and has no platform dependencies, so you can just recompile it
  on the Pi with `make` there.  If you prefer to cross‑compile from x86, set `CC`/`CFLAGS`
  appropriately (e.g. `CC=arm-linux-gnueabihf-gcc`) and ensure the ARM cross toolchain is
  installed.
* Test files (`output.wav`) will be written in the current directory; make sure the Pi has
  a writable filesystem and any required audio drivers if you plan to play back the audio.

Once the environment is set up on the Pi, the steps to build and run are the same as above.

### Installing a prebuilt `piper` binary

If you don’t want to build `piper` from source, you can download a prebuilt release
for your platform.  A helper script is included in this repository that fetches the
appropriate archive from the official GitHub releases and unpacks it.

### Models

The synthesizer requires an ONNX model file (e.g. `en_US-amy-low.onnx`).  The demo
looks for a model in the working directory by default; you can override the name by
setting the `PIPER_MODEL` environment variable or passing the model path as the
first command-line argument to `tts`.

There is no model bundled with this repo.  Download one from the Piper project or
another source (e.g. via the [piper-models repository](https://github.com/rhasspy/piper-models)
or your own trained models) and place it beside the executable:

```sh
curl -L -o en_US-amy-low.onnx <model-URL>
```

If the model isn’t present you’ll see an error like:

```
error: model 'en_US-amy-low.onnx' not found.
Place the ONNX file in the working directory or set PIPER_MODEL.
```

Once you have a model, rerun `./tts` and the WAV file will be generated as before.

```sh
# fetch into the workspace:
./fetch_piper.sh
# this will leave a `piper` executable (or `piper.bin` plus supporting
# libraries) in the current directory and also download a default TTS model
# plus its JSON metadata.
# (you may need to `chmod +x fetch_piper.sh` first).

# you can override the model URLs before running the script if you prefer:
#
# MODEL_URL=<url> MODEL_JSON_URL=<url> ./fetch_piper.sh


# either invoke it explicitly:
./piper --help

# or add the workspace to your PATH when running the demo:
PATH=$PWD:$PATH ./tts

The archive may unpack into a `piper` directory containing additional libraries and
helper files.  `fetch_piper.sh` will move the actual binary to `./piper.bin`, leaving
`./piper` behind; when the demo runs it automatically sets
`LD_LIBRARY_PATH=./piper:$LD_LIBRARY_PATH` so the binary can find its shared
libraries.  It also sets `ESPEAK_DATA=./piper/espeak-ng-data` so the bundled eSpeak‑NG
installation can locate its phoneme tables.  If you run `./piper.bin` yourself you may
need to set both environment variables manually:

```sh
LD_LIBRARY_PATH=./piper:$LD_LIBRARY_PATH \
ESPEAK_DATA=./piper/espeak-ng-data ./piper.bin tts ...
```
```

The script supports x86_64, aarch64/arm64 and armv7l (Pi4) architectures.  You can
also just use `curl`/`tar` manually if you prefer.

### Troubleshooting

If you see an error like this when running the demo:

```
sh: 1: piper: not found
piper command failed with exit code 32512
```

it means the program couldn’t locate the `piper` binary on your `PATH`.  The demo now checks
for `piper` explicitly and will display a better message if the check fails.  Make sure you
have installed `piper` or have added its directory to `$PATH` before running `./tts`.

You can test manually with:

```sh
which piper    # should print the path to the executable
piper --help    # verify the CLI runs
```



You can modify `main.c` to change the model, sample rate, or other options.

---

*This repository is just a template; it does not contain the `piper` binaries or models.*
