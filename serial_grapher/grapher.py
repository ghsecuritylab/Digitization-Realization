import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import os
import numpy as np
from subprocess import Popen, PIPE


script_path =  os.path.dirname(os.path.realpath(__file__))

process = Popen(['python3', 'serial_reader.py'], stdout=PIPE, stderr=PIPE)

fig = plt.figure()
ax1 = fig.add_subplot(2,1,1)
ax2 = fig.add_subplot(2,1,2)

chans = 1 # 1 channel
samp_rate = 44100 # 44.1kHz sampling rate
chunk = 1024 # 2^12 samples for buffer
f_vec = samp_rate*np.arange(chunk/2)/chunk


def animate(i):
    try:
        graph_data = open(script_path + '/data.txt','r+').read()
        lines = graph_data.split('\n')
        ys = []
        for line in lines:
            if len(line) > 1:
                ys.append(int(line))
        ax1.clear()
        ax2.clear()
        ax1.set_ylim(-32768, 32767)
        ax1.plot(ys)

        fft_data = (np.abs(np.fft.fft(ys))[0:int(np.floor(chunk/2))])/chunk
        fft_data[1:] = 2*fft_data[1:]
        ax2.set_ylim(0, 32726)
        plt.xlabel('Frequency [Hz]')
        # plt.ylabel('Amplitude [Pa]')
        ax2.set_xscale('log')
        ax2.plot(f_vec, fft_data, color='red')
    except KeyboardInterrupt:
        process.terminate()
        exit()
    except Exception as e:
        print(e)

ani = animation.FuncAnimation(fig, animate, interval=10)
plt.show()
process.terminate()
