#include <speex/speex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define FRAME_SIZE 160 // 对于窄带模式，帧大小通常是160
 
int main() {
    // 定义SpeexBits和编码状态变量
    SpeexBits bits;
    void *enc_state;
 
    // 初始化bits
    speex_bits_init(&bits);
 
    // 初始化编码状态，这里使用窄带模式
    enc_state = speex_decoder_init(&speex_nb_mode);
 
    // 设置编码器参数，例如质量参数
    int quality = 8; // 质量范围通常是0到10
    speex_decoder_ctl(enc_state, SPEEX_SET_QUALITY, &quality);
 
    // 打开输入文件，这里假设输入文件是Speex编码的
    FILE *input = fopen("encoded_speex.data", "rb");
    if (!input) {
        printf("Error opening input file\n");
        return 1;
    }
 
    // 打开输出文件，用于存储解码后的PCM数据
    FILE *output = fopen("decoded_pcm.pcm", "wb");
    if (!output) {
        printf("Error opening output file\n");
        fclose(input);
        return 1;
    }
 
    // 分配输出缓冲区
    short output_buffer[FRAME_SIZE];
 
    // 循环读取、解码并写入文件
    while (1) {
        // 读取一帧Speex编码的数据
        int bytes_read = fread(&bits.bytes, 1, sizeof(bits.bytes), input);
        if (bytes_read == 0) {
            // 如果读取的字节数为0，表示文件结束
            break;
        }
        bits.nbytes = bytes_read;
        bits.nbBits = bytes_read * 8;
 
        // 解码
        speex_decode_int(enc_state, &bits, output_buffer);
 
        // 写入解码后的PCM数据
        fwrite(output_buffer, sizeof(short), FRAME_SIZE, output);
    }
 
    // 清理资源
    fclose(output);
    fclose(input);
    speex_bits_destroy(&bits);
    speex_decoder_destroy(enc_state);
 
    return 0;
}