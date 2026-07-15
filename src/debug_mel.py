"""Debug: compare mel features between Python and C - detailed"""
import sys, struct, subprocess, numpy as np
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import config
from dataset import SpeechCommandsDataset

data_path = SpeechCommandsDataset.download(target_dir=str(config.DATA_DIR.parent))
dataset_obj = SpeechCommandsDataset(data_path)
datasets, label_to_idx, noise_samples = dataset_obj.build_dataset(
    silence_num=config.SILENCE_NUM, unknown_pct=config.UNKNOWN_PCT,
)

for item in datasets['test']:
    if item['label_name'] == 'down':
        sample = item
        break

waveform = sample['waveform']
label = sample['label_name']
print(f"Test sample: {label}, len={len(waveform)}")
print(f"Waveform[0:5]: {waveform[:5]}")
print(f"Waveform max abs: {np.abs(waveform).max():.4f}")

# Python mel
py_mel = SpeechCommandsDataset.extract_log_mel(waveform)
print(f"Python Mel[0,:5]:  {py_mel[0,:5]}")
print(f"Python Mel[60,:5]: {py_mel[60,:5]}")

# Also compute Python power spectrum for one frame
pad_len = config.WINDOW_SIZE // 2
padded = np.pad(waveform, (pad_len, pad_len), mode='reflect')
window = 0.5 * (1 - np.cos(2 * np.pi * np.arange(config.WINDOW_SIZE) / config.WINDOW_SIZE))
frame0 = padded[0:config.WINDOW_SIZE] * window
py_fft = np.fft.rfft(frame0.astype(np.float64), n=config.FFT_SIZE)
py_power = np.abs(py_fft) ** 2
print(f"Python frame0 power[0:5]: {py_power[:5]}")
print(f"Python frame0 power sum: {py_power.sum():.4f}")

# Write PCM
pcm = (waveform * 32767).clip(-32768, 32767).astype(np.int16)
pcm_path = config.OUTPUT_DIR / "debug_mel.pcm"
with open(pcm_path, "wb") as f:
    f.write(pcm.tobytes())

# Verify PCM was written correctly
pcm_read = np.frombuffer(open(pcm_path, "rb").read(), dtype=np.int16)
print(f"PCM read back first 10: {pcm_read[:10]}")
print(f"PCM back to float first 10: {pcm_read[:10].astype(np.float64) / 32768.0}")

debug_c = r'''
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define FS 16000
#define FRM_LEN 160
#define WIN_SIZE 480
#define PI 3.14159265358979323846
#define FFT_LEN 512
#define FREQ_NUM 257
#define MELS_NUM 40

#include "kws_weights.h"

static double g_window[WIN_SIZE];
static double g_mel_fb[FREQ_NUM * MELS_NUM];

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static void my_fft(double* x, double* y, long n, long sign) {
    long i, j, k, m, n1, n2;
    double c, c1, e, s, s1, t, tr, ti;
    for (j = i = 1; i < 31; i++) { m = i; j = 2 * j; if (j == n) break; }
    for (n1 = n - 1, j = 0, i = 0; i < n1; i++) {
        if (i < j) { tr = x[j]; ti = y[j]; x[j] = x[i]; y[j] = y[i]; x[i] = tr; y[i] = ti; }
        k = n / 2; while (k < (j + 1)) { j -= k; k /= 2; } j += k; }
    for (n2 = 1; n2 < n; n2 *= 2) {
        c1 = cos(PI / n2); s1 = -sign * sin(PI / n2); n1 = 2 * n2;
        c = 1.0; s = 0.0;
        for (j = 0; j < n2; j++) {
            for (i = j; i < n; i += n1) {
                k = i + n2;
                tr = c * x[k] - s * y[k]; ti = c * y[k] + s * x[k];
                x[k] = x[i] - tr; y[k] = y[i] - ti;
                x[i] += tr; y[i] += ti; }
            t = c; c = c * c1 - s * s1; s = t * s1 + s * c1; } }
    if (sign == -1) for (i = 0; i < n; i++) { x[i] /= n; y[i] /= n; }
}

int main() {
    FILE* fp = fopen("debug_mel.pcm", "rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    long nsamples = fsize / 2;
    short* wf = (short*)malloc(nsamples * sizeof(short));
    fread(wf, sizeof(short), nsamples, fp);
    fclose(fp);

    printf("nsamples=%ld\n", nsamples);
    printf("wf[0..4]: %d %d %d %d %d\n", wf[0], wf[1], wf[2], wf[3], wf[4]);
    printf("wf as float[0..4]: %f %f %f %f %f\n",
        wf[0]/32768.0, wf[1]/32768.0, wf[2]/32768.0, wf[3]/32768.0, wf[4]/32768.0);

    // Init window
    for (long i = 0; i < WIN_SIZE; i++)
        g_window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / WIN_SIZE));
    printf("g_window[0,239,240,479]: %.6f %.6f %.6f %.6f\n",
        g_window[0], g_window[239], g_window[240], g_window[479]);

    // Init mel filterbank
    double all_freqs[FREQ_NUM];
    double m_pts[MELS_NUM + 2], f_pts[MELS_NUM + 2], f_diff[MELS_NUM + 1];
    double f_max = FS / 2.0;
    for (long i = 0; i < FREQ_NUM; i++) all_freqs[i] = (double)i * f_max / (FREQ_NUM - 1);
    double m_min = 2595.0 * log10(1.0 + 0.0 / 700.0);
    double m_max = 2595.0 * log10(1.0 + f_max / 700.0);
    printf("m_min=%f m_max=%f f_max=%f\n", m_min, m_max, f_max);
    m_pts[0] = f_pts[0] = 0;
    for (long i = 0; i <= MELS_NUM; i++) {
        m_pts[i + 1] = m_min + (m_max - m_min) * (i + 1) / (MELS_NUM + 1);
        f_pts[i + 1] = 700.0 * (pow(10.0, m_pts[i + 1] / 2595.0) - 1.0);
    }
    for (long i = 0; i < MELS_NUM + 1; i++) f_diff[i] = f_pts[i + 1] - f_pts[i];
    for (long i = 0; i < FREQ_NUM; i++)
        for (long j = 0; j < MELS_NUM; j++) {
            double low = -(all_freqs[i] - f_pts[j]) / (f_diff[j] + 1e-12);
            double high = (all_freqs[i] - f_pts[j + 2]) / (f_diff[j + 1] + 1e-12);
            g_mel_fb[i * MELS_NUM + j] = dmax(0.0, dmin(low, high));
        }
    printf("g_mel_fb[0,0..4]: %.6f %.6f %.6f %.6f %.6f\n",
        g_mel_fb[0], g_mel_fb[1], g_mel_fb[2], g_mel_fb[3], g_mel_fb[4]);
    printf("g_mel_fb[100,0..4]: %.6f %.6f %.6f %.6f %.6f\n",
        g_mel_fb[100*MELS_NUM], g_mel_fb[100*MELS_NUM+1], g_mel_fb[100*MELS_NUM+2],
        g_mel_fb[100*MELS_NUM+3], g_mel_fb[100*MELS_NUM+4]);

    // Extract mel for frame 0
    long nf = nsamples / FRM_LEN + 1;
    long pad = WIN_SIZE / 2;
    printf("\n--- Frame 0 details ---\n");

    double re[FFT_LEN], im[FFT_LEN];
    for (long i = 0; i < FFT_LEN; i++) re[i] = im[i] = 0;

    for (long i = 0; i < WIN_SIZE; i++) {
        long si = 0 * FRM_LEN + i - pad;
        if (si < 0) si = -si;
        else if (si >= nsamples) si = 2 * nsamples - si - 2;
        re[i] = (wf[si] / 32768.0) * g_window[i];
    }

    printf("re[0..4] before FFT: %.10f %.10f %.10f %.10f %.10f\n",
        re[0], re[1], re[2], re[3], re[4]);
    printf("re[235..244] before FFT: ");
    for (int i = 235; i < 245; i++) printf("%.6f ", re[i]);
    printf("\n");

    my_fft(re, im, FFT_LEN, 1);

    printf("re[0..4] after FFT: %.10f %.10f %.10f %.10f %.10f\n",
        re[0], re[1], re[2], re[3], re[4]);
    printf("im[0..4] after FFT: %.10f %.10f %.10f %.10f %.10f\n",
        im[0], im[1], im[2], im[3], im[4]);

    double power0_5 = 0;
    for (int i = 0; i < 5; i++) power0_5 += re[i]*re[i] + im[i]*im[i];
    printf("Power sum[0..4]: %.10f\n", power0_5);

    double power_all = 0;
    for (int i = 0; i < FREQ_NUM; i++) power_all += re[i]*re[i] + im[i]*im[i];
    printf("Power sum all freq: %.10f\n", power_all);

    // Compute mel for frame 0
    printf("Mel[0,0..4]: ");
    for (long m = 0; m < 5; m++) {
        double a = 0;
        for (long k = 0; k < FREQ_NUM; k++)
            a += (re[k]*re[k] + im[k]*im[k]) * g_mel_fb[k * MELS_NUM + m];
        printf("%.6f ", log(a + 1.0e-6));
    }
    printf("\n");

    free(wf);
    return 0;
}
'''

debug_path = config.OUTPUT_DIR / "debug_mel2.c"
with open(debug_path, "w", encoding="utf-8") as f:
    f.write(debug_c)

result = subprocess.run(
    ["gcc", "debug_mel2.c", "kws_weights.o", "-o", "debug_mel2.exe", "-I.", "-std=c99", "-O0", "-lm", "-w"],
    capture_output=True, text=True, timeout=30, cwd=str(config.OUTPUT_DIR),
)
if result.returncode != 0:
    print("Compile error:")
    print(result.stderr[:2000])
else:
    exe_path = config.OUTPUT_DIR / "debug_mel2.exe"
    result = subprocess.run(
        [str(exe_path)],
        capture_output=True, text=True, timeout=30, cwd=str(config.OUTPUT_DIR),
    )
    print(result.stdout)
    if result.stderr:
        print("STDERR:", result.stderr[:500])
