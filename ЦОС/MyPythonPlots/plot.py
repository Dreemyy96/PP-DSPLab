import pandas as pd
import matplotlib.pyplot as plt

# читаем CSV
df = pd.read_csv("output.csv")

# построение графиков
plt.figure(figsize=(12, 6))

# аналоговый сигнал
plt.plot(df["time"], df["analog"], label="Аналоговый сигнал", linewidth=2)

# цифровой сигнал
plt.step(df["time"], df["digital"], label="Цифровой сигнал (уровни)", where='mid', linestyle='--')

plt.title("Сигнал: аналоговый и цифровой")
plt.xlabel("Время (с)")
plt.ylabel("Амплитуда / уровень")
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.show()
