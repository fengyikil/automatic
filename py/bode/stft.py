# bode_complete.py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import TransferFunction, lsim, chirp, stft
from numpy import unwrap

# ---------- 1. 设置中文字体 ----------
plt.rcParams['font.sans-serif'] = ['SimHei']      # 黑体
plt.rcParams['axes.unicode_minus'] = False        # 解决负号乱码

# ---------- 2. 理论系统 ----------
wn = 50
z = 0.2
# G = TransferFunction([wn**2], [1, 2*z*wn, wn**2])
# G = TransferFunction([wn**2], [1, 2*z*wn, wn**2])
G = TransferFunction([10], [1, 20, 2])

# ---------- 3. 生成扫频数据 ----------
fs = 1000          # 采样率
T = 20            # 总时长
t = np.arange(0, T, 1/fs)

u = 0.5 * chirp(t, f0=0.5, f1=50, t1=T, method='logarithmic')
_, y, _ = lsim(G, U=u, T=t)

# 保存 CSV（如已存在可跳过）
pd.DataFrame({'t': t, 'u': u, 'y': y}).to_csv('sweep_io.csv',
                                              index=False, float_format='%.6f')

# ---------- 4. 读 CSV ----------
df = pd.read_csv('sweep_io.csv')
t, u, y = df['t'].values, df['u'].values, df['y'].values
fs = 1/(t[1]-t[0])
T = t[-1]

# ---------- 5. STFT 反推 ----------
nper, nolap = 2048, 3*2048//4
f, _, Ust = stft(u, fs, window='hann', nperseg=nper, noverlap=nolap)
_, _, Yst = stft(y, fs, window='hann', nperseg=nper, noverlap=nolap)

# 去掉 0 Hz
mask = f > 0
f = f[mask]
Ust = Ust[mask]
Yst = Yst[mask]

# 对数扫频时间映射
f_min, f_max = 0.5, 50
t_star = T * np.log(f / f_min) / np.log(f_max / f_min)
t_star = np.clip(t_star, 0, T)

tt = np.linspace(0, T, Ust.shape[1])
idx = np.argmin(np.abs(tt[None, :] - t_star[:, None]), axis=1)
H = Yst[np.arange(len(f)), idx] / (Ust[np.arange(len(f)), idx] + 1e-12)

mag = 20*np.log10(np.abs(H)+1e-12)
phase = unwrap(np.angle(H)) * 180/np.pi   # 去掉尖角

# ---------- 6. 理论伯德图 ----------
w_th, mag_th, phase_th = G.bode(
    w=np.logspace(np.log10(2*np.pi*0.5), np.log10(2*np.pi*50), 400))
f_th = w_th / (2*np.pi)

# ---------- 7. 画图 ----------
plt.figure(figsize=(6, 5))
plt.subplot(2, 1, 1)
plt.semilogx(f, mag, label='反推')
plt.semilogx(f_th, mag_th, '--', label='理论')
plt.ylabel('幅值 [dB]')
plt.legend()
plt.grid(which='both', ls=':')

plt.subplot(2, 1, 2)
plt.semilogx(f, phase, label='反推')
plt.semilogx(f_th, phase_th, '--', label='理论')
plt.ylabel('相位 [°]')
plt.xlabel('频率 [Hz]')
plt.legend()
plt.grid(which='both', ls=':')
plt.tight_layout()
plt.show()
