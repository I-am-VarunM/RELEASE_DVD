import os
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
def plot_data_from_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    x_values = []
    y_values = []

    for line in lines:
        data = line.strip().split(',')
        try:       
        	y_values.append(float(data[0]))
        	x_values.append(float(data[1])*20.0/20000)
        except:
                pass
    try:
    	plt.plot(x_values, y_values)
    except:
    	pass
    plt.xlabel('X Values')
    plt.ylabel('Y Values')
    plt.title(f'Plot for {filename}')
    plt.grid(True)
    ax = plt.gca()
    ax.xaxis.set_major_locator(MultipleLocator(1))
    # Extract the number from the filename (e.g., Message_17.txt -> 17)
    number = int(filename.split('_')[1].split('.')[0])
    plt.savefig(f'{number}.png')
    plt.close()

def main():
    # Assuming the text files are in the same directory as the script
    file_prefix = "Message_"

    # Loop through file numbers from 1 to 63
    for i in range(257,1000):
        filename = file_prefix + str(i) + ".txt"
        if os.path.isfile(filename):
            plot_data_from_file(filename)

if __name__ == "__main__":
    main()


