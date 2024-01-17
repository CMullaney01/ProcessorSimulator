import numpy as np

# Define the two arrays
# Compiled fib x = accuracy y = IPC
x = ([0.09, 0.82,0.82,0.82])
y = ([0.65,1.01,1.01,1.01])

# Calculate the Pearson correlation coefficient
corr_coef = np.corrcoef(x, y)[0, 1]

print("Pearson correlation coefficient For Fib:", corr_coef)

# bubbleSort x = accuracy y = IPC
x = ([0.40,0.59,0.76,0.78])
y = ([1.32,1.68,1.93,1.93])

corr_coef = np.corrcoef(x, y)[0, 1]
print("Pearson correlation coefficient For bubble:", corr_coef)


#enhance x = accuracy y = IPC
x = ([0.34,0.66,0.87,0.85])
y = ([1.54,1.89,2.05,2.03])

corr_coef = np.corrcoef(x, y)[0, 1]
print("Pearson correlation coefficient For enhance:", corr_coef)

#branch x = accuracy y = IPC
x = ([0.6,0.44,0.72,0.8])
y = ([0.81,0.67,1.19,1.26])

corr_coef = np.corrcoef(x, y)[0, 1]
print("Pearson correlation coefficient For branch:", corr_coef)

#independent x = accuracy y = IPC
x = ([0.11,0.78,0.78,0.78])
y = ([2.65,3.52,3.52,3.52])

corr_coef = np.corrcoef(x, y)[0, 1]
print("Pearson correlation coefficient For independent:", corr_coef)