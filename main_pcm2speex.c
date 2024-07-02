#include <speex/speex.h>
#include <stdio.h>
#include <stdlib.h>

#include "speex/speex.h"



/* 定义帧大小，对于窄带模式，帧大小通常是160 */
#define FRAME_SIZE 160
 
int main(int argc, char **argv)
{
    char *in_pcm_file_name = NULL;
    FILE *fp_in_pcm = NULL;
    unsigned int sample_rate = 0;
    char *out_spx_file_name = NULL;
    FILE *fp_out_spx = NULL;

    short *pcm_buf = NULL;
    char *cbits = NULL;
    int nbBytes;
    int framesize = 0;

    /* 检查参数 */
    if(argc != 4)
    {
        printf("Usage: \n"
                "\t %s <in-pcm-file> <sample-rate> <out-spx-file>\n"
                "examples: \n"
                "\t %s ./audio/test_8000_16_1.pcm  8000  out_8000.spx\n"
                "\t %s ./audio/test_16000_16_1.pcm 16000 out_16000.spx\n"
                , argv[0], argv[0], argv[0]);
        return -1;
    }
    in_pcm_file_name = argv[1];
    sample_rate = atoi(argv[2]);
    out_spx_file_name = argv[3];

    const SpeexMode *speex_mode = NULL;
    switch(sample_rate)
    {
        case 8000:
            speex_mode = &speex_nb_mode;
            break;
        case 16000:
            speex_mode = &speex_wb_mode;
            break;
        case 32000:
            speex_mode = &speex_uwb_mode;
            break;
    }

    /* 初始化编码器状态 */
    void *state = speex_encoder_init(speex_mode);
 
    /* 设置编码质量为8（15 kbps） */
    int quality = 8;
    speex_encoder_ctl(state, SPEEX_SET_QUALITY, &quality);
    speex_encoder_ctl(state, SPEEX_SET_SAMPLING_RATE, &sample_rate);
    speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &framesize);
    int bps = 0; // 只是用来打印一下
    speex_encoder_ctl(state, SPEEX_GET_BITRATE, &bps);
    printf("It will encode with [quality: %d, sample_rate: %d, framesize: %d, bps: %d].\n", quality, sample_rate, framesize, bps);

    fp_in_pcm = fopen(in_pcm_file_name, "rb");
    if (!fp_in_pcm) {
        printf("Error opening input file: %s\n", in_pcm_file_name);
        speex_encoder_destroy(state);
        return 1;
    }
 
    /* 打开输出文件，用于存储编码后的Speex数据 */
    fp_out_spx = fopen(out_spx_file_name, "wb");
    if (!fp_out_spx) {
        printf("Error opening output file\n");
        fclose(fp_in_pcm);
        speex_encoder_destroy(state);
        return 1;
    }

    pcm_buf = (short *)malloc(sizeof(short) * framesize);
    cbits = (char *)malloc(sizeof(char) * framesize * 1.25); // 多个1/4

    /* 初始化位处理结构 */
    SpeexBits bits;
    speex_bits_init(&bits);
 
    /* 循环读取、编码并写入数据 */
    while (1) {
        /* 读取一个16位/sample的音频帧 */
        size_t bytesRead = fread(pcm_buf, sizeof(short), framesize, fp_in_pcm);
        if (bytesRead != framesize) {
            if (feof(fp_in_pcm)) {
                break; // 文件结束
            } else {
                printf("Error reading input file\n");
                speex_bits_destroy(&bits);
                fclose(fp_out_spx);
                fclose(fp_in_pcm);
                speex_encoder_destroy(state);
                return 1;
            }
        }
 
        /* 重置位处理结构，以便编码新帧 */
        speex_bits_reset(&bits);
 
        /* 编码帧 */
        speex_encode_int(state, pcm_buf, &bits);
 
        /* 将位复制到字符数组中，以便写入 */
        nbBytes = speex_bits_write(&bits, cbits, 200);
 
        /* 写入帧大小 */
        /* LRM: 这里写入接下来编码的数据大小，这里记录方便后面解码知道要读多少，不是标准的spx文件格式 */
        fwrite(&nbBytes, sizeof(int), 1, fp_out_spx);
        /* 写入压缩数据 */
        /* LRM: 这里写入编码后的spx数据 */
        fwrite(cbits, 1, nbBytes, fp_out_spx);
        printf("[Speex Encode] pcm(%lu Bytes) -> spx(%d Bytes).\n", framesize * sizeof(short), nbBytes);
    }
 
    /* 清理资源 */
    speex_bits_destroy(&bits);
    fclose(fp_out_spx);
    fclose(fp_in_pcm);
    speex_encoder_destroy(state);
    free(pcm_buf);
    free(cbits);

    return 0;
}