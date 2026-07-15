/* PC 端 C 推理测试 */
#include "kws_inference.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <audio.pcm>\n", argv[0]);
        fprintf(stderr, "  audio.pcm: 16kHz 16-bit mono raw PCM\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "Error: cannot open %s\n", argv[1]);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    long nsamples = fsize / 2;
    short* waveform = (short*)malloc(nsamples * sizeof(short));
    fread(waveform, sizeof(short), nsamples, fp);
    fclose(fp);

    kws_init();
    int result = kws_recognize(waveform, nsamples);

    printf("C result: %d (%s)\n", result, kws_label_name(result));

    free(waveform);
    return 0;
}
