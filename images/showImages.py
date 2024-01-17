import numpy as np
import matplotlib.pyplot as plt

def view_images(file_path):
    # Load CSV file into a NumPy array, skipping header row
    data = np.genfromtxt(file_path, delimiter=',')
    
    # Iterate through each row of the CSV file and display the image
    for row in data:
        label = row[0]
        pixels = row[1:].reshape(28, 28)  # Assumes image is 28x28
        
        # Display image
        plt.imshow(pixels, cmap='gray')
        plt.title(f"Label: {label}")
        plt.show()

view_images('./output.csv')