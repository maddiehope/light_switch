# This program should have a user interface to view the status of the lightbulb 
# and provide a way to change the color and status.  
# It should have corresponding subscription and publishing topics to the arduino program.  
# The graphical user interface should be designed with tkinter.

from tkinter import *
import paho.mqtt.client as mqtt
from time import strftime
import struct

root = Tk()
root.geometry("500x300") # making sure the window is a specific size when launched

light_status = 1 # defining the default 'switch' value
c = [] # list variable for r,g,b
r = 0
g = 0
b = 0


#--------------------------------------------

def time():
    time_string = strftime('%H:%M:%S %p \n %A \n %x')
    time = Label(status, text = time_string)
    time.place(relx=0.5, rely=0.3, anchor=CENTER) # centering the time label 

# setting the window light color 
def color():

    global light_status 
    global r,g,b

    r = R.get()
    g = G.get()
    b = B.get()

    if light_status == 1: # making sure the color is only updated if the light is on

        c = [r,g,b]
        for i, color in enumerate(c):

            if color.isdigit() == True: # if entry inputs are proper integers, this function will work
                c[i] = int(color) 
            else: # otherwise, the color value is automatically set to 0 (not accounted for)
                # this also helps empty input boxes automatically initalize as 0 (this ensures the program will still update)
                c[i] = 0          
    
        r = c[0]
        g = c[1]
        b = c[2]
        indicator = b'#' # helps arduino determine what the message's purpose is

        data = struct.pack('cBBB', indicator, r, g, b)

        print(data) # test

        client.publish(light_control_color, data) # publishing to mqtt

        hexcolor = f"#{r:02x}{g:02x}{b:02x}" # formatting the numbers into hex code 
        canvas.configure(bg=hexcolor)

        # client.publish(light_control_color, hexcolor) # publishing to mqtt

# used to set color from callback
def color_2(r, g, b):

    global light_status

    if light_status == 1: # making sure the color is only updated if the light is on
        r = int(r)
        g = int(g)
        b = int(b)   

        print(r)
        print(g)
        print(b)

        hexcolor = f"#{r:02x}{g:02x}{b:02x}" # formatting the numbers into hex code 
        canvas.configure(bg=hexcolor)


# on-off switch with images: https://www.geeksforgeeks.org/on-off-toggle-button-switch-in-tkinter/
def switch():
    global light_status
    global r, g, b

    # Determine is on or off
    if light_status == 1:

        light_status = 0
        on_button.config(image = off)
        on_off.config(text = "OFF")
        canvas.configure(bg="black") # turning the window light off
        client.publish(light_control_status, light_status) # publishing to mqtt

    elif light_status == 0:

        light_status = 1
        on_button.config(image = on)
        on_off.config(text = "ON")
        client.publish(light_control_status, light_status) # publishing to mqtt
        color_2(r,g,b)
    
    
    time()

def on_message(client_obj, userdata, message): #note, userdata is unused
    #print(f"Message received: {message.payload.decode('utf8')}") #test
    global light_status
    global r, g, b

    if message.topic == light_status_:
        on = int(message.payload[0])
        r = int(message.payload[1])
        g = int(message.payload[2])
        b = int(message.payload[3])

        if on == 1:
            light_status = 0
        else:
            light_status = 1
        
        switch() 
        color_2(r,g,b)
        time()
        

def pumpMQTT():
    client.loop(0)
    root.after(10, pumpMQTT) # repeat this 

#--------------------------------------------

client_id = "client1"
broker = "info8000.ga"
light_control_color = "ugaelee2045sp23/mph63858/light_control_color"
light_control_status = "ugaelee2045sp23/mph63858/light_control_status"
light_status_ = "ugaelee2045sp23/mph63858/light_status"

client = mqtt.Client(client_id)
client.on_message = on_message
client.username_pw_set("giiuser", "giipassword")
client.connect(broker)
client.subscribe(light_control_color)
client.subscribe(light_control_status)
client.subscribe(light_status_)

#--------------------------------------------

# giving the window a title 
root.title("Lightbulb")

#creating frames for the labels
status = LabelFrame(root, text = "Light Status")
status.pack(fill="both", expand="yes")
status.pack(padx=10, pady=10, side= LEFT)

control = LabelFrame(root, text = "Light Control")
control.pack(fill="both", expand="yes")
control.pack(padx=10, pady=10, side= RIGHT)

time() 

# creating the box that will become the light color
canvas = Canvas(status, width=100, height=100, bg='black')
canvas.place(relx=0.5, rely=0.7, anchor=CENTER) # centering the canvas

# r, g, b input boxes
R_label = Label(control, text="R:")
G_label = Label(control, text="G:")
B_label = Label(control, text="B:")

R = Entry(control)
G = Entry(control)
B = Entry(control)

R_label.place(x=10, y=60)
G_label.place(x=10, y=85)
B_label.place(x=10, y=110)

R.place(x=25, y=60)
G.place(x=25, y=85)
B.place(x=25, y=110)

# update button for r, g, b entries
update = Button(control, text="Update", command=color) 
update.place(x=140, y=140)

# on/off label 
on_off = Label(status, text = "STATUS UNKNOWN")
on_off.place(relx=0.5, rely=0.1, anchor=CENTER) # centering the status label 

# defining images 
on = PhotoImage(file = "on.png")
off = PhotoImage(file = "off.png")
 
on = on.subsample(15) # resizing the images 
off = off.subsample(15)

# on/off button
on_button = Button(control, image = on, bd = 0, command = switch)
on_button.place(relx=0.5, rely=0.1, anchor=CENTER) # centering the button 

#--------------------------------------------

pumpMQTT()
root.mainloop()
