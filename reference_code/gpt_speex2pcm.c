#include <stdio.h>
#include <stdlib.h>
#include <speex/speex.h>

#define FRAME_SIZE 320 // 一帧的样本数

int main() {
    // 打开 Speex 文件
    FILE *speex_file = fopen("input.spx", "rb");
    if (!speex_file) {
        perror("Failed to open input Speex file");
        return 1;
    }

    // 创建 PCM 文件
    FILE *pcm_file = fopen("output.pcm", "wb");
    if (!pcm_file) {
        perror("Failed to open output PCM file");
        fclose(speex_file);
        return 1;
    }

    // 初始化 Speex 解码器
    SpeexBits bits;
    void *dec_state;

    speex_bits_init(&bits);
    dec_state = speex_decoder_init(&speex_nb_mode); // 使用窄带语音解码模式

    // 设置解码器参数（例如，采样率）
    int sample_rate = 16000; // 假设使用16kHz采样率
    speex_decoder_ctl(dec_state, SPEEX_SET_SAMPLING_RATE, &sample_rate);

    // 循环读取并解码 Speex 数据
    char speex_buffer[FRAME_SIZE];
    short pcm_buffer[FRAME_SIZE];
    int nbytes;
    while ((nbytes = fread(speex_buffer, 1, FRAME_SIZE, speex_file)) > 0) {
        // 将 Speex 数据填充到比特流结构体中
        speex_bits_read_from(&bits, speex_buffer, nbytes);

        // 解码为 PCM 数据
        int frame_size = speex_decode_int(dec_state, &bits, pcm_buffer);

        // 将解码后的 PCM 数据写入 PCM 文件
        fwrite(pcm_buffer, sizeof(short), frame_size, pcm_file);
    }

    // 关闭文件和释放资源
    fclose(speex_file);
    fclose(pcm_file);
    speex_decoder_destroy(dec_state);
    speex_bits_destroy(&bits);

    return 0;
}

/* 写一个speex格式文件解码PCM的示例
解释：
1. 打开文件：首先打开输入的 Speex 文件（input.spx）和输出的 PCM 文件（output.pcm）。
2. 初始化 Speex 解码器：使用 speex_decoder_init 初始化解码器，并选择合适的解码模式，如示例中的窄带语音解码模式 (&speex_nb_mode)。可以根据需要选择其他解码模式，例如宽带或超宽带模式。
3. 设置解码器参数：使用 speex_decoder_ctl 设置解码器的参数，例如采样率 (SPEEX_SET_SAMPLING_RATE)。确保解码器的参数与输入的 Speex 文件参数匹配。
4. 循环读取并解码：循环从 Speex 文件中读取数据，并将其填充到 Speex 比特流结构体 (bits) 中进行解码。解码后的 PCM 数据存储在 pcm_buffer 数组中。
5. 写入 PCM 文件：使用 fwrite 将解码后的 PCM 数据写入到 PCM 文件（output.pcm）中。
6. 关闭文件和释放资源：完成解码后，关闭打开的文件并释放使用的内存资源，包括销毁 Speex 解码器和释放 Speex 比特流结构体。
确保在编译和运行代码之前，已经正确安装了 Speex 库，并设置了正确的头文件和库文件路径。如有任何报错或问题，可以参考错误信息进行调试和修复。
*/