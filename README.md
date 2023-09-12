# M5 Light Switch 
This project is a solution to an assignment for ELEE 2045. The goal of the project is to create a light switch using an M5Stick and MQTT protocol. The project involves programming in both Python and Arduino, and requires knowledge of message topics, byte conversion, and conditional statements.

<b> Video Demonstration: https://youtu.be/SKbHAoUil6k </b>

## Accomplishments/Failures
There were a lot of things I could've done more efficient in this lab. 
I would say the main issue with my programs is that the 
'mqttCLient.messageTopic()' command in arduino wouldn't work. In some of 
my earlier commits, arduino was effectively distinguishing between message topics 
when it recieved a message from python (I had it printing in the serial 
monitor). However, later on, it wouldn't print the message topic at all. 
I'm not sure why this change happened, and I couldn't find where I went 
wrong. Since I couldn't figure out how to distingusih between message 
topics anymore on the arduino end, I decided just to use if structures and 
indicative characters in the buffer message to determine what type of 
message was being relayed by python so the M5 stick could update 
accordingly.

Another big issue I experienced in this lab was that in arduino, I had 
also subscribed to the light_status topic, which was meant only to publish 
the light status from arduino to python. This was causing my arduino to 
recive messages every time it published them, and for a while made my 
light on the M5 stick never turn off. Once I recognized this mistake (with 
the help of Andrew), it was smooth sailing finishing the rest of the 
lab.

Overall, I have a lot of lines of code, whereas Dr. J said the solutions 
only had ~200 total. I know there are several places where some of my if 
structures/commands could have been written more efficiently. Towards the 
end of the lab though, I had stopped focusing on writing pretty code and 
was more driven to complete the lab by any means possible.

Also, I know that the paho library shouldn't be installed in this direcotry, lol. 
For some reason it wouldn't properly install & I'm not sure what I did, 
but I eventually installed it within lab3. In the future, I know I will have to fix this,
but I didn't want to compromise causing issues with my light working by moving it 
while still actively working on this lab.

## Reflections

One main difference is that the solutions' functions in the python side are a lot simpler than what I came up with. One reason for this is that where I had one bigger function- such as my color function- the solutions had several smaller ones. For example, my color function got the color entry values, checked if the light was turned on, updated the color on the GUI, AND published the color to the topic. In the solutions, there are smaller, seperate functions to accomplish all of these tasks, which in the long run actually lessened the amount of code needed. Trying to combine all of these functions into one larger function resulted in the need of color_2, which did everything color did, just without the publishing. If I had seperated these orginally, this would not have been necessary.

I think how the solutions toggles the light is very interesting. I know this was given to us, but I chose to do it a different way that just made more sense in my mind. From what I can see, the toggle is not used within the other functions anywhere. I used my switch function within my python on message function to 'switch' the light when an 'on' or 'off' message is recived, which like mentioned above led me to have conditional statements within the function that might have not been necessary. When thinking of the function as a more definitve 'light switch', this makes sense, but sense it is programming I recognize that one singular function does not have to be the do-all be-all. 

When reading in bytes, I didn't know they could be taken seperately in sequential order like how the solutions did. I struggled alot with reading in the message because of type conversion issues/etc, which I mentioned some about above in the accomplishments/failures. Knowing it can be done this way simplifies things a lot.
