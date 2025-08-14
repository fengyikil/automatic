# cwt_bode_full.py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import pywt                       # PyWavelets
from scipy.signal import TransferFunction, lsim, chirp

plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False

# ---------- 1. 理论系统 ----------
wn = 50
z = 0.2
G = TransferFunction([wn**2], [1, 2*z*wn, wn**2])

# ---------- 2. 生成扫频数据 ----------
fs = 1000          # 采样率
T = 20            # 总时长
t = np.arange(0, T, 1/fs)

u = 0.5 * chirp(t, f0=0.5, f1=50, t1=T, method='logarithmic')
_, y, _ = lsim(G, U=u, T=t)

# ---------- 3. 保存 CSV ----------
pd.DataFrame({'t': t, 'u': u, 'y': y}).to_csv('sweep_io.csv',
                                              index=False, float_format='%.6f')

# ---------- 4. 读 CSV ----------
df = pd.read_csv('sweep_io.csv')
t, u, y = df['t'].values, df['u'].values, df['y'].values
fs = 1/(t[1]-t[0])        # 重新取 fs，保证与文件一致

# ---------- 5. CWT ----------
wavelet = 'cmor1.5-1.0'
f_min, f_max = 0.5, 50
n_freq = 400
freqs = np.logspace(np.log10(f_min), np.log10(f_max), n_freq)
scales = pywt.frequency2scale(wavelet, freqs/fs)

coef_u, _ = pywt.cwt(u, scales, wavelet, sampling_period=1/fs)
coef_y, _ = pywt.cwt(y, scales, wavelet, sampling_period=1/fs)

# ---------- 6. 扫频时间映射 ----------
tt = np.linspace(0, T, len(t))
t_star = T * np.log(freqs/f_min) / np.log(f_max/f_min)
t_star = np.clip(t_star, 0, T)

idx = np.argmin(np.abs(tt[None, :] - t_star[:, None]), axis=1)
Uc = coef_u[np.arange(len(freqs)), idx]
Yc = coef_y[np.arange(len(freqs)), idx]

H_cwt = Yc / (Uc + 1e-12)
mag_cwt = 20*np.log10(np.abs(H_cwt) + 1e-12)
phase_cwt = np.unwrap(np.angle(H_cwt)) * 180/np.pi

# ---------- 7. 理论伯德图 ----------
w_th, mag_th, phase_th = G.bode(w=2*np.pi*freqs)
f_th = w_th / (2*np.pi)

# ---------- 8. 画图 ----------
plt.figure(figsize=(6, 5))
plt.subplot(2, 1, 1)
plt.semilogx(freqs, mag_cwt, label='CWT')
plt.semilogx(f_th, mag_th, '--', label='理论')
plt.ylabel('幅值 [dB]')
plt.legend()
plt.grid(which='both', ls=':')

plt.subplot(2, 1, 2)
plt.semilogx(freqs, phase_cwt, label='CWT')
plt.semilogx(f_th, phase_th, '--', label='理论')
plt.ylabel('相位 [°]')
plt.xlabel('频率 [Hz]')
plt.legend()
plt.grid(which='both', ls=':')
plt.tight_layout()
plt.show()
