import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime
import scipy as sp
from scipy import signal

data = np.genfromtxt('BEBE.TXT',names=True,delimiter=',',
                     dtype=None,encoding = None)
#----GRAFICA-DE-TEMPERATURA--------
datos_Temperatura_Filtro=signal.medfilt(data['temperatura'],247)
Tem_Max=np.linspace(35,35,len(data))
Tem_Min=np.linspace(18,18,len(data))

fig1, ax1 = plt.subplots()
ax1.plot(data['contador'],datos_Temperatura_Filtro,color='green',
         linewidth=0.8,linestyle='dashdot', label='Temperatura bebe')
ax1.plot(data['contador'],Tem_Max,color='red',
         linewidth=0.8,linestyle='dashdot', label='Temperatura Maxima')
ax1.plot(data['contador'],Tem_Min,color='blue',
         linewidth=0.8,linestyle='dashdot', label='Temperatura Minima')
ax1.set_xlabel('Tiempo [s]')
ax1.set_ylabel('Temperatura [°C]', color='brown')
ax1.tick_params(axis='y', labelcolor='brown')
plt.legend(loc = "upper right")

ax2 = ax1.twinx()
ax2.plot(data['contador'],data['maltrato'],color='orange',
         linewidth=0.8, label='Maltrato')
ax2.set_ylabel('Maltrato', color='orange')
ax2.tick_params(axis='y', labelcolor='orange')
plt.legend(loc = "upper left")
plt.title("Temperatura")
#----GRAFICA-DE-ACELERACION----------
fig2, ax1 = plt.subplots()
ax1.plot(data['contador'],data['acelX'],color='blue',
         linewidth=0.8,linestyle='dashdot', label='ax')
ax1.plot(data['contador'],data['acelY'],color='red',
         linewidth=0.8,linestyle='dashdot', label='ay')
ax1.plot(data['contador'],data['acelZ'],color='green',
         linewidth=0.8,linestyle='dashdot', label='az')
ax1.set_xlabel('Tiempo [s]')
ax1.set_ylabel('Aceleracion [m/s2]', color='brown')
ax1.tick_params(axis='y', labelcolor='brown')
plt.legend(loc = "best")

ax2 = ax1.twinx()
ax2.plot(data['contador'],data['maltrato'],color='orange',
         linewidth=0.8, label='Maltrato')
ax2.set_ylabel('Maltrato', color='orange')
ax2.tick_params(axis='y', labelcolor='orange')
plt.legend(loc = "best")
plt.title("Aceleración")
#-----GRAFICA-DE-VELOCIDAD-ANGULAR----------
fig3, ax1 = plt.subplots()
ax1.plot(data['contador'],data['gyroX'],color='blue',
         linewidth=0.8,linestyle='dashdot', label='gyrox')
ax1.plot(data['contador'],data['gyroY'],color='red',
         linewidth=0.8,linestyle='dashdot', label='gyroy')
ax1.plot(data['contador'],data['gyroZ'],color='green',
         linewidth=0.8,linestyle='dashdot', label='gyroz')
ax1.set_xlabel('Tiempo [s]')
ax1.set_ylabel('Velocidad Angular [°/s]', color='brown')
ax1.tick_params(axis='y', labelcolor='brown')
plt.legend(loc = "upper left")

ax2 = ax1.twinx()
ax2.plot(data['contador'],data['maltrato'],color='orange',
         linewidth=0.8, label='Temperatura')
ax2.set_ylabel('Maltrato', color='orange')
ax2.tick_params(axis='y', labelcolor='orange')
plt.legend(loc = "upper right")
plt.title("Velocidad angular")
#-----GRAFICA-DE-ANGULO---------
fig4, ax1 = plt.subplots()
ax1.plot(data['contador'],data['pitch'],color='blue',
         linewidth=0.8,linestyle='dashdot', label='Pitch')
ax1.plot(data['contador'],data['roll'],color='red',
         linewidth=0.8,linestyle='dashdot', label='Roll')
ax1.set_xlabel('Tiempo [s]')
ax1.set_ylabel('Angulo [°]', color='brown')
ax1.tick_params(axis='y', labelcolor='brown')
plt.legend(loc = "upper left")

ax2 = ax1.twinx()
ax2.plot(data['contador'],data['maltrato'],color='orange',
         linewidth=0.8, label='Maltrato')
ax2.set_ylabel('Maltrato', color='orange')
ax2.tick_params(axis='y', labelcolor='orange')
plt.legend(loc = "upper right")
plt.title("Angulo")
#----------------
plt.show()

