import numpy as np
import matplotlib.pyplot as plt

def view_image(file_path):
    # Load CSV file into a NumPy array, skipping header row
    data = np.genfromtxt(file_path, delimiter=',', skip_header=1)
    
    # Extract label and pixel data
    label = data[0]
    pixels = data[1:].reshape(28, 28)  # Assumes image is 28x28
    
    # Display image
    plt.imshow(pixels, cmap='gray')
    plt.title(f"Label: {label}")
    plt.show()

view_image('ChosenImages.csv')