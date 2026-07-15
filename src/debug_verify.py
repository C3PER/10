"""Debug: compare mel features and head conv between Python and C"""
import sys, struct, subprocess, numpy as np
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import config
from dataset import SpeechCommandsDataset

# Load a test wav
data_path = SpeechCommandsDataset.download(target_dir=str(config.DATA_DIR.parent))
dataset_obj = SpeechCommandsDataset(data_path)
datasets, label_to_idx, noise_samples = dataset_obj.build_dataset(
    silence_num=config.SILENCE_NUM, unknown_pct=config.UNKNOWN_PCT,
)

# Get a sample
sample = datasets['test'][0]
waveform = sample['waveform']
label = sample['label_name']
print(f"Test sample: {label}")

# Python mel
py_mel = SpeechCommandsDataset.extract_log_mel(waveform)
print(f"Python Mel shape: {py_mel.shape}")
print(f"Python Mel[0,:5]: {py_mel[0,:5]}")
print(f"Python Mel range: [{py_mel.min():.4f}, {py_mel.max():.4f}]")

# Write PCM for C
pcm = (waveform * 32767).clip(-32768, 32767).astype(np.int16)
pcm_path = config.OUTPUT_DIR / "debug_test.pcm"
with open(pcm_path, "wb") as f:
    f.write(pcm.tobytes())

# Write a debug C program that prints mel output
debug_c = """
#include "kws_inference.h"
#include "kws_weights.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fp = fopen("debug_test.pcm", "rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    long nsamples = fsize / 2;
    short* wf = (short*)malloc(nsamples * sizeof(short));
    fread(wf, sizeof(short), nsamples, fp);
    fclose(fp);

    kws_init();

    long W = nsamples / FRM_LEN + 1;
    printf("W frames: %ld\\n", W);
    printf("nsamples: %ld\\n", nsamples);

    // Extract mel manually to verify
    double* mel = (double*)malloc(W * MELS_NUM * sizeof(double));
    // Call the internal function directly
    extern void extract_log_mel(const short wf[], int len, double* mel);

    long nf = nsamples / FRM_LEN + 1;
    long pad = WIN_SIZE / 2;
    double g_window[WIN_SIZE];
    double g_mel_fb[FREQ_NUM * MELS_NUM];
    for (long i = 0; i < WIN_SIZE; i++)
        g_window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / WIN_SIZE));
    double all_freqs[FREQ_NUM];
    double m_pts[MELS_NUM + 2], f_pts[MELS_NUM + 2], f_diff[MELS_NUM + 1];
    double f_max = FS / 2.0;
    for (long i = 0; i < FREQ_NUM; i++)
        all_freqs[i] = (double)i * f_max / (FREQ_NUM - 1);
    double m_min = 2595.0 * log10(1.0 + 0.0 / 700.0);
    double m_max = 2595.0 * log10(1.0 + f_max / 700.0);
    m_pts[0] = f_pts[0] = 0;
    for (long i = 0; i <= MELS_NUM; i++) {
        m_pts[i + 1] = m_min + (m_max - m_min) * (i + 1) / (MELS_NUM + 1);
        f_pts[i + 1] = 700.0 * (pow(10.0, m_pts[i + 1] / 2595.0) - 1.0);
    }
    for (long i = 0; i < MELS_NUM + 1; i++)
        f_diff[i] = f_pts[i + 1] - f_pts[i];
    for (long i = 0; i < FREQ_NUM; i++)
        for (long j = 0; j < MELS_NUM; j++) {
            double low = -(all_freqs[i] - f_pts[j]) / (f_diff[j] + 1e-12);
            double high = (all_freqs[i] - f_pts[j + 2]) / (f_diff[j + 1] + 1e-12);
            g_mel_fb[i * MELS_NUM + j] = (low > 0 ? (high < low ? high : low) : (high > 0 ? high : 0));
        }

    // Manual mel extraction identical to Python
    for (long n = 0; n < nf; n++) {
        double re[FFT_LEN], im[FFT_LEN];
        for (long i = 0; i < FFT_LEN; i++) re[i] = im[i] = 0;
        for (long i = 0; i < WIN_SIZE; i++) {
            long si = n * FRM_LEN + i - pad;
            if (si >= 0 && si < nsamples)
                re[i] = (wf[si] / 32768.0) * g_window[i];
        }
        // FFT
        for (long n2 = 1; n2 < FFT_LEN; n2 *= 2) {
            double c1 = cos(PI / n2);
            double s1 = -1.0 * sin(PI / n2);
            long n1 = 2 * n2;
            for (long j = 0; j < n2; j++) {
                double c = cos(j * PI / n2);
                double s = -1.0 * sin(j * PI / n2);
                for (long i = j; i < FFT_LEN; i += n1) {
                    long k = i + n2;
                    double tr = c * re[k] - s * im[k];
                    double ti = c * im[k] + s * re[k];
                    re[k] = re[i] - tr;
                    im[k] = im[i] - ti;
                    re[i] = re[i] + tr;
                    im[i] = im[i] + ti;
                }
            }
        }
        for (long m = 0; m < MELS_NUM; m++) {
            double a = 0;
            for (long k = 0; k < FREQ_NUM; k++)
                a += (re[k]*re[k] + im[k]*im[k]) * g_mel_fb[k * MELS_NUM + m];
            mel[n * MELS_NUM + m] = log(a + 1.0e-6);
        }
    }

    printf("C Mel[0,0..4]: ");
    for (int i = 0; i < 5; i++) printf("%.6f ", mel[i]);
    printf("\\n");
    printf("C Mel[100,0..4]: ");
    for (int i = 0; i < 5; i++) printf("%.6f ", mel[100 * MELS_NUM + i]);
    printf("\\n");

    // Also test with the kws_recognize function
    int result = kws_recognize(wf, nsamples);
    printf("Full result: %d (%s)\\n", result, kws_label_name(result));

    free(wf); free(mel);
    return 0;
}
"""

debug_path = config.OUTPUT_DIR / "debug_test.c"
with open(debug_path, "w") as f:
    f.write(debug_c)

# Compile
result = subprocess.run(
    f"cd {config.OUTPUT_DIR} && gcc debug_test.c kws_inference.o kws_weights.o -o debug_test.exe -I. -std=c99 -O0 -lm -w",
    shell=True, capture_output=True, text=True, timeout=30,
)
if result.returncode != 0:
    print("Compile error:")
    print(result.stderr[:2000])
else:
    # Run
    result = subprocess.run(
        f"cd {config.OUTPUT_DIR} && ./debug_test.exe",
        shell=True, capture_output=True, text=True, timeout=30,
    )
    print(result.stdout)
    if result.stderr:
        print("STDERR:", result.stderr[:500])
