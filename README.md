#### 1、libspeex移植步骤

源码下载地址：[https://xiph.org/downloads/](https://xiph.org/downloads/) 或 [https://www.speex.org/downloads/](https://www.speex.org/downloads/) 或 [https://www.linuxfromscratch.org/blfs/view/svn/multimedia/speex.html](https://www.linuxfromscratch.org/blfs/view/svn/multimedia/speex.html) 

```bash
 tar xzf speex-1.2.1.tar.gz
 cd speex-1.2.1/
 ./configure --prefix=$PWD/_install
 make -j8
 make install
 strip --strip-unneeded _install/lib/*
```

```bash
$ tree -h _install/
_install/
├── [4.0K]  include
│   └── [4.0K]  speex
│       ├── [6.5K]  speex_bits.h
│       ├── [4.9K]  speex_callbacks.h
│       ├── [ 203]  speex_config_types.h
│       ├── [ 14K]  speex.h
│       ├── [4.0K]  speex_header.h
│       ├── [3.6K]  speex_stereo.h
│       └── [3.5K]  speex_types.h
├── [4.0K]  lib
│   ├── [188K]  libspeex.a
│   ├── [1006]  libspeex.la
│   ├── [  17]  libspeex.so -> libspeex.so.1.5.2
│   ├── [  17]  libspeex.so.1 -> libspeex.so.1.5.2
│   ├── [ 98K]  libspeex.so.1.5.2
│   └── [4.0K]  pkgconfig
│       └── [ 358]  speex.pc
└── [4.0K]  share
    ├── [4.0K]  aclocal
    │   └── [3.6K]  speex.m4
    └── [4.0K]  doc
        └── [4.0K]  speex
            └── [429K]  manual.pdf

8 directories, 15 files
```

#### 2、说明

##### a. demo现状

- 目前暂时没找到相关播放器直接播放spx格式文件；

- 将demo编码生成的spx格式文件再通过demo解码生成pcm数据是能正常播放的；

- 目前demo生成的spx格式文件是夹带着4个字节编码数据量大小，即：

  ```
  [spx数据长度(4字节)]+[spx编码数据(n字节)] + [spx数据长度(4字节)]+[spx编码数据(n字节)] + ...
  ```

##### b. 关于speex格式的说明

- Speex（音标[spi:ks]）是一套开源免费的、无专利保护的、针对语音设计的音频压缩格式。
- Speex编解码器已经被Opus编解码器淘汰，Speex还是可以继续使用，由于Opus比Speex在各方面都更好，所以鼓励大家切换到Opus，但是Opus只支持编码和解码，不支持噪音抑制、声学回音消除等其他处理功能。
- 只支持8000Hz窄带（Narrow Band）、16000Hz宽带（Wide Band）、32000Hz超宽带（Ultra Wide Band）的三种带模式进行编解码，不支持其他采样频率。
- 只支持单声道，不支持多声道。
- 支持强化立体声编码（Intensity Stereo Encoding）。
- 支持数据包丢失隐藏（Packet Loss Concealment、PLC）。
- 支持固定比特率（Constant Bit Rate、CBR）。
- 支持可变比特率（Variable Bit Rate、VBR）。
- 支持平均比特率（Average Bit Rate、ABR）。
- 支持非连续传输（Discontinuous transmission、DTX）。
- 支持定点执行（Fixed-point implementation）。
- 支持浮点执行（Floating-point implementation）。
- 支持声学回音消除（Acoustic Echo Canceller、AEC）。
- 支持残余回音消除（Residual Echo Canceller、REC）。
- 支持噪音抑制（Noise Suppression、NS）。
- 支持混响音消除（Dereverb）。
- 支持自动增益控制（Automatic Gain Control、AGC）。
- 支持语音活动检测（Voice Activity Detection、VAD）。
- 支持多速率（multi-rate）。
- 支持嵌入式（Embedded）。
- 支持重采样（Resample）。


#### 3、使用方法

##### a. 编译

```bash
$ make clean && make
rm -rf pcm2speex speex2pcm pcm2speex_gpt speex2pcm_gpt out*
gcc main_pcm2speex.c -I./include -lspeex -L./lib -lm -o pcm2speex
gcc main_speex2pcm.c -I./include -lspeex -L./lib -lm -o speex2pcm
gcc main_pcm2speex_gpt.c -I./include -lspeex -L./lib -lm -o pcm2speex_gpt
gcc main_speex2pcm_gpt.c -I./include -lspeex -L./lib -lm -o speex2pcm_gpt
```

##### b. 编码

```bash
$ ./pcm2speex 
Usage: 
         ./pcm2speex <in-pcm-file> <sample-rate> <out-spx-file>
examples: 
         ./pcm2speex ./audio/test_8000_16_1.pcm  8000  out_8000.spx
         ./pcm2speex ./audio/test_16000_16_1.pcm 16000 out_16000.spx
```

##### c. 解码

```bash
$ ./speex2pcm 
Usage: 
         ./speex2pcm <in-spx-file> <sample-rate> <out-pcm-file>
examples: 
         ./speex2pcm out_8000.spx  8000  out_8000.pcm
         ./speex2pcm out_16000.spx 16000 out_16000.pcm
```

#### 4、参考文章

- [speex 编码简介-CSDN博客](https://blog.csdn.net/liuxunfei15/article/details/120353056) 

- [音频编解码speex库的使用方法-CSDN博客](https://blog.csdn.net/yuanya/article/details/8914097) 
- [Speex详解（2019年09月25日更新） - 赤勇玄心行天道 - 博客园](https://www.cnblogs.com/gaoyaguo/p/5032920.html) 

#### 5、demo目录架构

```bash
$ tree
.
├── audio
│   ├── test_16000_16_1.pcm
│   └── test_8000_16_1.pcm
├── docs
│   ├── speex 编码简介-CSDN博客.mhtml
│   ├── Speex详解（2019年09月25日更新） - 赤勇玄心行天道 - 博客园.pdf
│   └── 音频编解码speex库的使用方法-CSDN博客.pdf
├── include
│   └── speex
│       ├── speex_bits.h
│       ├── speex_callbacks.h
│       ├── speex_config_types.h
│       ├── speex.h
│       ├── speex_header.h
│       ├── speex_stereo.h
│       └── speex_types.h
├── lib
│   └── libspeex.a
├── main_pcm2speex.c
├── main_speex2pcm.c
├── Makefile
├── opensource
│   └── speex-1.2.1.tar.gz
├── README.md
└── reference_code
    ├── blog_pcm2speex.c
    ├── blog_speex2pcm.c
    ├── gpt_pcm2speex.c
    └── gpt_speex2pcm.c
```
