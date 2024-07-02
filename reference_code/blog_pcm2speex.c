
#include <speex/speex.h>
#include <stdio.h>
#include <string.h>
 
/* 定义帧大小，对于窄带模式，帧大小通常是160 */
#define FRAME_SIZE 160
 
int main(int argc, char **argv) {
    char *inFile;
    FILE *fin;
    short in[FRAME_SIZE];
    float input[FRAME_SIZE];
    char cbits[200];
    int nbBytes;
 
    /* 初始化编码器状态 */
    void *state = speex_encoder_init(&speex_nb_mode);
 
    /* 设置编码质量为8（15 kbps） */
    int quality = 8;
    speex_encoder_ctl(state, SPEEX_SET_QUALITY, &quality);
 
    /* 检查命令行参数 */
    if (argc != 2) {
        printf("Usage: %s <input pcm file>\n", argv[0]);
        return 1;
    }
 
    inFile = argv[1];
    fin = fopen(inFile, "rb");
    if (!fin) {
        printf("Error opening input file: %s\n", inFile);
        speex_encoder_destroy(state);
        return 1;
    }
 
    /* 打开输出文件，用于存储编码后的Speex数据 */
    FILE *fout = fopen("encoded_speex.spx", "wb");
    if (!fout) {
        printf("Error opening output file\n");
        fclose(fin);
        speex_encoder_destroy(state);
        return 1;
    }
 
    /* 初始化位处理结构 */
    SpeexBits bits;
    speex_bits_init(&bits);
 
    /* 循环读取、编码并写入数据 */
    while (1) {
        /* 读取一个16位/sample的音频帧 */
        size_t bytesRead = fread(in, sizeof(short), FRAME_SIZE, fin);
        if (bytesRead != FRAME_SIZE) {
            if (feof(fin)) {
                break; // 文件结束
            } else {
                printf("Error reading input file\n");
                speex_bits_destroy(&bits);
                fclose(fout);
                fclose(fin);
                speex_encoder_destroy(state);
                return 1;
            }
        }
 
        /* 将16位值复制到浮点数，以便Speex处理 */
        for (int i = 0; i < FRAME_SIZE; i++) {
            input[i] = in[i];
        }
 
        /* 重置位处理结构，以便编码新帧 */
        speex_bits_reset(&bits);
 
        /* 编码帧 */
        speex_encode(state, input, &bits);
 
        /* 将位复制到字符数组中，以便写入 */
        nbBytes = speex_bits_write(&bits, cbits, 200);
 
        /* 写入帧大小 */
        fwrite(&nbBytes, sizeof(int), 1, fout);
        /* 写入压缩数据 */
        fwrite(cbits, 1, nbBytes, fout);
    }
 
    /* 清理资源 */
    speex_bits_destroy(&bits);
    fclose(fout);
    fclose(fin);
    speex_encoder_destroy(state);
 
    return 0;
}