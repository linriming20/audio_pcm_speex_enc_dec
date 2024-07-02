#include <stdio.h>
#include <stdlib.h>
#include <speex/speex.h>

#define FRAME_SIZE 160 // 每帧的采样数
#define MAX_FRAME_SIZE 200

int main() {
    // 打开 PCM 文件
    FILE *pcm_file = fopen("input.pcm", "rb");
    if (!pcm_file) {
        perror("Failed to open input PCM file");
        return 1;
    }

    // 打开 Speex 文件
    FILE *speex_file = fopen("output.spx", "wb");
    if (!speex_file) {
        perror("Failed to open output Speex file");
        fclose(pcm_file);
        return 1;
    }

    // 初始化 Speex 编码器状态和比特流
    SpeexBits bits;
    void *enc_state;
    int quality = 8; // 设置编码质量（0-10之间）

    // 初始化 Speex 比特流结构体
    speex_bits_init(&bits);

    // 创建编码器
    enc_state = speex_encoder_init(&speex_nb_mode); // NB mode是窄带语音编码模式

    // 设置编码器参数
    speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &quality);

    // 读取和编码 PCM 文件
    short in[FRAME_SIZE]; // 假设每次读取的采样数据存放在 in 数组中
    float input[MAX_FRAME_SIZE]; // Speex 编码器需要 float 类型的输入

    while (1) {
        // 从 PCM 文件读取一帧音频数据到 in 数组中
        int bytesRead = fread(in, sizeof(short), FRAME_SIZE, pcm_file);
        if (bytesRead <= 0) break;

        // 填充 input 数组，Speex 编码器需要 float 类型的输入
        for (int i = 0; i < FRAME_SIZE; i++) {
            input[i] = (float)in[i];
        }

        // 对音频数据进行编码
        speex_encode(enc_state, input, &bits);

        // 将编码后的比特流写入 Speex 文件
        char cbits[MAX_FRAME_SIZE];
        int nbBytes = speex_bits_write(&bits, cbits, MAX_FRAME_SIZE);
        fwrite(cbits, 1, nbBytes, speex_file);
    }

    // 关闭文件和释放资源
    fclose(pcm_file);
    fclose(speex_file);
    speex_encoder_destroy(enc_state);
    speex_bits_destroy(&bits);

    return 0;
}

/** 写一个pcm文件编码speex的示例
解释：
1. 打开文件：首先打开输入的 PCM 文件（input.pcm）和输出的 Speex 文件（output.spx）。
2. 初始化编码器：创建 Speex 编码器并初始化，设置编码参数（如编码质量）。
3. 循环读取 PCM 文件：循环从 PCM 文件中读取数据，每次读取一帧数据到 in 数组中。
4. 编码：将 in 数组中的数据转换为 float 类型的 input 数组，然后使用 speex_encode 函数进行编码，编码后的数据存储在 bits 比特流中。
5. 写入到文件：使用 speex_bits_write 函数将编码后的比特流写入到 Speex 文件（output.spx）中。
6. 关闭文件和释放资源：完成编码后，关闭打开的文件并释放使用的内存资源。
这段示例代码假设 PCM 文件中的采样数据是 16-bit 的，如果是其他格式（如 8-bit 或 32-bit），需要相应地调整读取和处理数据的部分。确保在使用时，Speex 库已正确安装并配置，头文件路径和链接库路径已正确设置。
*/