import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as p3
import matplotlib.animation as animation
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D


def update_dots(num, data, dot):
    # NOTE: there is no .set_data() for 3 dim data...
    dot.set_data(data[0:2, :num])
    dot.set_3d_properties(data[2, :num])
    return dot

# Attaching 3D axis to the figure

fig = plt.figure()
ax = Axes3D(fig, auto_add_to_figure=False)
fig.add_axes(ax)

# Reading the data from a CSV file using pandas
repo = pd.read_csv('/home/abdul/ws/src/IIWA_work/Output/iiwa-cartesian-coordinates-samples-without-tuning.csv', sep=',', header=0)
data = np.array((repo['x'].values, repo['y'].values, repo['z'].values))

# exit()

# Creating fifty dot objects.
# NOTE: Can't pass empty arrays into 3d version of plot()
limit = 0.5
dot = ax.plot(data[0, 0], data[1, 0], data[2, 0], lw=0, marker='o', markersize=1.0)[0]

# Setting the axes properties
ax.set_xlim3d([-limit, limit])
ax.set_xlabel('X')

ax.set_ylim3d([-limit, limit])
ax.set_ylabel('Y')

ax.set_zlim3d([-limit, limit])
ax.set_zlabel('Z')

ax.set_title('3D Test')

# Creating the Animation object
dot_ani = animation.FuncAnimation(fig, update_dots, data.shape[1], fargs=(data, dot), interval=5, blit=False)

plt.show()


