CC_WIN = x86_64-w64-mingw32-gcc

BASE_CFLAGS = -Os -nostdlib -s -Iinclude -Wl,-e,WinMainCRTStartup,--gc-sections
OPUS_CFLAGS = -Os -DVAR_ARRAYS -DSTDLIB -DOPUS_BUILD -s -Iinclude -Ilibs -Ilibs/opus -Ilibs/celt -Ilibs/silk -Ilibs/silk/float

LDFLAGS_WIN = -lkernel32 -lxaudio2_9 -lole32 -luser32 -lgdi32
OBJ_DIR := build_temp
SRC_DIR := src
RES_FILE := tools/resources.res
UPX := tools\upx.exe

OUT_NAME := windowtapper.exe
UPX_NAME := windowtapper_upx.exe

SRCS := $(wildcard src/*.c) \
		$(wildcard src/windows/*.c)

ifeq ($(OPUS),1)
CFLAGS_WIN := $(OPUS_CFLAGS)
SRCS += \
        $(wildcard libs/ogg/*.c) \
        $(wildcard libs/opus/*.c) \
        $(wildcard libs/celt/*.c) \
        $(wildcard libs/silk/*.c) \
        $(wildcard libs/silk/float/*.c)
else
CFLAGS_WIN := $(BASE_CFLAGS)
endif

OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean upx

all: compile upx

opus:
	$(MAKE) OPUS=1 all

compile: $(OBJS) $(RES_FILE)
	$(CC_WIN) $(CFLAGS_WIN) -o $(OUT_NAME) $^ $(LDFLAGS_WIN)

$(OBJ_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@

upx: compile
	cmd /C "if exist $(UPX) $(UPX) --ultra-brute $(OUT_NAME) -o $(UPX_NAME)"

clean:
	-del /Q $(OBJ_DIR)\*.o $(OUT_NAME) $(UPX_NAME)
	-rmdir /Q /S $(OBJ_DIR)