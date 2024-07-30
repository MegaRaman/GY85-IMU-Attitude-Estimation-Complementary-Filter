#!/usr/bin/python3

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

ser = serial.Serial('/dev/ttyACM0', 115200)
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
time = []
roll, pitch = [], []


def animate(i, time, roll, pitch):
    try:
        att_line = ser.readline().decode('utf-8').strip()
    except:
        exit(-1)

    att_words = att_line.split()
    
    if len(att_words) != 6:
        return

    if att_words[0] != "Roll":
        if att_line == "Error":
            print("Error occured")
            exit(-1)
        return

    try:
        roll_msr = float(att_words[2])
        pitch_msr = float(att_words[5])
    except:
        return

    roll.append(roll_msr)
    pitch.append(pitch_msr)

    time.append(len(time))

    roll = roll[-1000:]
    pitch = pitch[-1000:]
    time = time[-1000:]

    ax.clear()
    
    plt.plot(time, roll, color="red")
    plt.plot(time, pitch, color="green")



# Set up the animation
ani = animation.FuncAnimation(fig, animate, fargs=(time, roll, pitch), interval=0)

# Show the plot
plt.show()

# Close the serial port when done
ser.close()

