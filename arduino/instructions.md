# Instruction Guide
This system assumes the solar panel to be mounted to a base that contains two parts: 1 rotational part that controls the horizontal movement of the panel, another rotational part that controls the verticle movement of the panel. 

The two rotational parts are connected by a motor for the movement to occur and each motor is connected to one arduino board for power supply and getting its individual instruction. Consequentially, each file correspond to one rotational part and should be connected to the corresponding arduino board.

## Functionality
The two arduino files each is responsible to control the servo motors on the rotational parts, each controls a different direction of rotation.

## Mechanism
The core logistic of this project in figuring out the angle of the pannel is to use two essential sensors: GPS to determine geographic location (latitude and longitude), and RTC (real time clock) to determine time and convert to local time zone using the geographical location information.

1. Geographic location: The angle of the sun is different for different locations, but can be calcualted using its longitude and latitude and expressed in terms of azimuth and altitude angle. In this project, we used a built-in package to do the calculation, but basically the azimuth angle is the angle the horizontal rotational motor controls and the same go with altitude being controlled by the vertical rotational motor. For our specific design of the physical project, due to the limitation of the motor rotation angle being 90 degree at max, we twisted the angle to fit this range a little. But this could be easily adjusted if given a motor with broader range of rotation.
