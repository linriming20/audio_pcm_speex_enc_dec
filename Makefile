all: pcm2speex speex2pcm

# our static lib is compile by x86_64 gcc
CC := gcc
CFLAG := -I./include
LDFLAGS := -lspeex -L./lib -lm

pcm2speex: main_pcm2speex.c
	$(CC) $^ $(CFLAG) $(LDFLAGS) -o $@

speex2pcm: main_speex2pcm.c
	$(CC) $^ $(CFLAG) $(LDFLAGS) -o $@

clean :
	rm -rf pcm2speex speex2pcm out*
.PHONY := clean

