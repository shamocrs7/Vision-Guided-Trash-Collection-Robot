# AI Trash Collector Robot

An autonomous mobile robot capable of detecting trash using computer vision and collecting it using a robotic arm.

The system integrates **ESP32 WiFi communication**, **I2C communication**, **ultrasonic obstacle detection**, and a **servo-controlled robotic arm** to perform intelligent waste collection.

---

# System Overview

The robot moves forward autonomously while continuously scanning the environment.  

A computer vision system identifies objects and sends the detected object label to an **ESP32 web server**. The ESP32 processes the data and communicates with the **Arduino motor controller through I2C**.

When trash is detected and the robot approaches an obstacle, the robot stops and activates the robotic arm to collect the object. Otherwise, the robot avoids the obstacle and continues navigation.

---

# Architecture

Computer Vision System  
↓  
WiFi POST Request  
↓  
ESP32 Web Server  
↓  
I2C Communication  
↓  
Arduino Robot Controller  
↓  
Motors + Robotic Arm

---

# Hardware Components

• ESP32 (WiFi communication & web server)  
• Arduino (main robot controller)  
• L298 / Motor Shield (DC motor control)  
• 4 DC motors (mobile base)  
• Servo motors (robotic arm)  
• Ultrasonic sensor HC-SR04  
• Robotic gripper (end effector)

---

# Key Features

### Computer Vision Integration
Receives object detection results from an external AI model via HTTP POST.

### Smart Decision Logic
The robot only activates the arm when:

- An object is detected as **trash**
- The robot reaches a close distance detected by the ultrasonic sensor.

### Autonomous Navigation
Robot moves forward continuously and avoids obstacles if they are not trash.

### Robotic Arm Collection
A multi-servo robotic arm performs a full collection cycle:

1. Reach position
2. Collect position
3. Grab
4. Return to home position

### Real-Time Web Dashboard
ESP32 hosts a web interface displaying:

- Detected object
- Robot command state
- Trash detection status

---

# Software Components

### ESP32
- WiFi connection
- Web server
- HTTP API
- JSON parsing
- I2C master communication

### Arduino
- Motor control
- Servo arm control
- Ultrasonic sensing
- I2C slave communication

---

# Communication Protocol

## HTTP API

POST endpoint:
/set_data

Example JSON:
{
"object": "trash"
}

Possible values:

| Object | Action |
|------|------|
trash | collect |
bottle | collect |
other | avoid |

---

## I2C Commands

ESP32 sends a command every **200ms**:

| Command | Meaning |
|------|------|
1 | Trash detected – prepare arm |
0 | No trash – navigation only |

---

# Robot Behavior Logic
IF obstacle detected
IF trash detected
→ stop robot
→ activate arm collection cycle
ELSE
→ turn left to avoid obstacle

ELSE
→ move forward

---

# Robotic Arm Motion Sequence

1. Home position
2. Reach object
3. Move down
4. Close gripper
5. Lift object
6. Return to home

Smooth servo interpolation is used to ensure stable motion.

---

# Web Control Interface

The ESP32 provides a simple **Mission Control dashboard** displaying:

- Current AI recognition result
- I2C command state
- Collection / Avoid decision

The page refreshes automatically every **800 ms**.

---

# Example Workflow

1. AI model detects **bottle**
2. Detection is sent to ESP32 via HTTP
3. ESP32 sends I2C command **1**
4. Robot moves until ultrasonic detects object
5. Robot stops
6. Robotic arm collects the trash
7. Robot resumes navigation

---

# Future Improvements

• ROS2 integration  
• SLAM navigation  
• Advanced AI waste classification  
• Autonomous bin sorting  
• Multi-camera detection  
• Path planning


