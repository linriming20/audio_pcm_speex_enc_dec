#include <speex/speex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(int argc, char **argv)
{
    char *in_spx_file_name = NULL;
    FILE *fp_in_spx = NULL;
    unsigned int sample_rate = 0;
    char *out_pcm_file_name = NULL;
    FILE *fp_out_pcm = NULL;

    char *speex_buffer = NULL;
    short *pcm_buffer = NULL;
    int framesize = 0;

    // 定义SpeexBits和编码状态变量
    SpeexBits bits;
    void *dec_state;
 
    /* 检查参数 */
    if(argc != 4)
    {
        printf("Usage: \n"
                "\t %s <in-spx-file> <sample-rate> <out-pcm-file>\n"
                "examples: \n"
                "\t %s out_8000.spx  8000  out_8000.pcm\n"
                "\t %s out_16000.spx 16000 out_16000.pcm\n"
                , argv[0], argv[0], argv[0]);
        return -1;
    }
    in_spx_file_name = argv[1];
    sample_rate = atoi(argv[2]);
    out_pcm_file_name = argv[3];

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

    // 初始化bits
    speex_bits_init(&bits);
 
    // 初始化编码状态，这里使用窄带模式
    dec_state = speex_decoder_init(speex_mode);

    speex_decoder_ctl(dec_state, SPEEX_SET_SAMPLING_RATE, &sample_rate);
    speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &framesize);

    // 打开输入文件，这里假设输入文件是Speex编码的
    fp_in_spx = fopen(in_spx_file_name, "rb");
    if (!fp_in_spx) {
        printf("Error opening input file\n");
        return 1;
    }
 
    // 打开输出文件，用于存储解码后的PCM数据
    fp_out_pcm = fopen(out_pcm_file_name, "wb");
    if (!fp_out_pcm) {
        printf("Error opening output file\n");
        fclose(fp_in_spx);
        return 1;
    }

    speex_buffer = malloc(framesize); // 这是编码后的，空间足够了的
    pcm_buffer = (short *)malloc(sizeof(short) * framesize);
 
    // 循环读取、解码并写入文件
    while (1) {
        // 读取一帧Speex编码的数据
        #if 0
        int bytes_read = fread(&bits.bytes, 1, sizeof(bits.bytes), fp_in_spx);
        if (bytes_read == 0) {
            // 如果读取的字节数为0，表示文件结束
            break;
        }
        bits.nbytes = bytes_read;
        bits.nbBits = bytes_read * 8;
        #else
        int spx_bytes = 0;
        int bytes_read = 0;
        /* LRM: 这里读出接下来的spx编码数据长度，对应于我们编码时写入的数据长度 */
        bytes_read = fread(&spx_bytes, 1, sizeof(spx_bytes), fp_in_spx);
        if (bytes_read == 0) {
            // 如果读取的字节数为0，表示文件结束
            printf("file end.\n");
            break;
        }
        printf("will read spx bytes: %d, ", spx_bytes);
        /* LRM: 这里读出编码后的spx数据 */
        bytes_read = fread(speex_buffer, 1, spx_bytes, fp_in_spx);
        if (bytes_read == 0) {
            // 如果读取的字节数为0，表示文件结束
            printf("file end(-1).\n");
            break;
        }
        printf("decode output pcm size: %lu\n", framesize * sizeof(short));

        // 将 Speex 数据填充到比特流结构体中
        speex_bits_read_from(&bits, speex_buffer, bytes_read);
        #endif

        // 解码
        speex_decode_int(dec_state, &bits, pcm_buffer);
 
        // 写入解码后的PCM数据
        fwrite(pcm_buffer, sizeof(short), framesize, fp_out_pcm);
    }
 
    // 清理资源
    fclose(fp_out_pcm);
    fclose(fp_in_spx);
    speex_bits_destroy(&bits);
    speex_decoder_destroy(dec_state);
 
    return 0;
}