# cwt_bode_fast.py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import pywt
from scipy.signal import TransferFunction, lsim, chirp, decimate

plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False

fs = 1000           # 原始采样率
T = 20
f_min, f_max = 10, 50
csv_file = 'sweep_io.csv'

# ---------- 3. 读 CSV ----------
df = pd.read_csv(csv_file)
t, u, y = df['t'].values, df['u'].values, df['y'].values
fs = 1/(t[1] - t[0])
plt.ion()          # 关键：打开交互模式

# ---------- 9. 画时域波形（单窗口） ----------
plt.figure(figsize=(7, 3.5))
plt.plot(t, u, lw=0.8, label='Input u(t)')
plt.plot(t, y, lw=0.8, label='Output y(t)')
plt.title('Time-domain signals with noise')
plt.xlabel('Time [s]')
plt.ylabel('Amplitude')
plt.legend()
plt.grid(True, ls=':')
plt.tight_layout()
plt.show()

# ---------- 4. 降采样 ----------
dec_factor = 10                 # 10 倍降采样
u_ds = decimate(u,  dec_factor, ftype='fir')
y_ds = decimate(y,  dec_factor, ftype='fir')
fs_ds = fs // dec_factor        # 新采样率 100 Hz

# ---------- 5. CWT ----------
wavelet = 'cmor1.5-1.0'         # 复 Morlet
# 每倍频程 8 点 => 总点数 ≈ 46
freqs = np.logspace(np.log10(f_min), np.log10(f_max),
                    int(np.log2(f_max/f_min)*8))
scales = pywt.frequency2scale(wavelet, freqs/fs_ds)

# float32 提速
coef_u, _ = pywt.cwt(u_ds.astype(np.float32), scales.astype(np.float32),
                     wavelet, sampling_period=1/fs_ds)
coef_y, _ = pywt.cwt(y_ds.astype(np.float32), scales.astype(np.float32),
                     wavelet, sampling_period=1/fs_ds)

# ---------- 6. 扫频时间映射 ----------
tt = np.linspace(0, T, len(u_ds))
t_star = T * np.log(freqs/f_min) / np.log(f_max/f_min)
t_star = np.clip(t_star, 0, T)

idx = np.argmin(np.abs(tt[None, :] - t_star[:, None]), axis=1)
H_cwt = coef_y[np.arange(len(freqs)),
               idx] / (coef_u[np.arange(len(freqs)),
                              idx] + 1e-12)

mag_cwt = 20*np.log10(np.abs(H_cwt) + 1e-12)
phase_cwt = np.unwrap(np.angle(H_cwt)) * 180/np.pi

# ---------- 7. 理论伯德图 ----------
# w_th, mag_th, phase_th = G.bode(w=2*np.pi*freqs)
# f_th = w_th / (2*np.pi)

# ---------- 8. 画图 ----------
plt.figure(figsize=(6, 5))
plt.subplot(2, 1, 1)
plt.semilogx(freqs, mag_cwt, label='CWT')
# plt.semilogx(f_th, mag_th, '--', label='理论')
plt.ylabel('幅值 [dB]')
plt.legend()
plt.grid(which='both', ls=':')

plt.subplot(2, 1, 2)
plt.semilogx(freqs, phase_cwt, label='CWT')
# plt.semilogx(f_th, phase_th, '--', label='理论')
plt.ylabel('相位 [°]')
plt.xlabel('频率 [Hz]')
plt.legend()
plt.grid(which='both', ls=':')
plt.tight_layout()
plt.ioff()   # 关闭交互模式
plt.show()
