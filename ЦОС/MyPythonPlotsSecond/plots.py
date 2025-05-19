import matplotlib.pyplot as plt
import csv

time = []
analog = []
restored = []

with open('fft_results.csv', 'r') as file:
    reader = csv.DictReader(file)
    for row in reader:
        time.append(float(row['index']))
        analog.append(float(row['analog']))
        restored.append(float(row['restored']))

plt.figure(figsize=(12,6))
plt.plot(time, analog, label='Analog Signal', linewidth=2)
plt.plot(time, restored, label='Restored Signal (IFFT)', linestyle='--')
plt.xlabel('Index')
plt.ylabel('Amplitude')
plt.title('Comparison of Analog and Restored Signals')
plt.legend()
plt.grid(True)
plt.show()
