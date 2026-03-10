# AI Trash Collector Robot

An autonomous mobile robot designed to detect and collect trash using computer vision and a robotic arm.

The system integrates an ESP32 for monitoring, an Arduino for robot control, ultrasonic sensing for obstacle detection, and a servo-driven robotic arm for trash collection.

The ESP32 web server is used **only to display the current system status** and does not control the robot.

---

# System Overview

The robot moves forward continuously while scanning its surroundings using an ultrasonic sensor.

An external AI vision system detects whether the object is **trash** or **not trash** and sends the result to the ESP32.

The ESP32 forwards the command to the Arduino through **I2C communication**.

If the robot approaches an object and the AI indicates it is trash, the robotic arm performs a collection sequence. Otherwise, the robot avoids the obstacle.

---

# System Architecture

Computer Vision Detection  
↓  
ESP32 (status display + communication)  
↓  
I2C Communication  
↓  
Arduino Robot Controller  
↓  
Motors + Robotic Arm

---

# Hardware Components

• ESP32 (WiFi monitoring and web dashboard)  
• Arduino (main robot controller)  
• L293 Motor Driver  
• 4 DC Motors (mobile base)  
• Servo Motors (robotic arm)  
• Ultrasonic Sensor (HC-SR04)  
• Robotic Gripper

---

# Key Features

## Trash Detection Integration
The system receives trash detection results from an external AI model.

## Autonomous Navigation
The robot moves forward continuously while monitoring obstacles.

## Intelligent Decision Logic

When an obstacle is detected:

• If trash is detected → robot stops and collects it  
• If not trash → robot turns to avoid the obstacle

## Robotic Arm Collection

The robotic arm performs a complete sequence:

1. Move to reach position  
2. Lower to object  
3. Close gripper  
4. Lift object  
5. Return to home position

Servo motion is performed smoothly using incremental interpolation.

## Real-Time Status Dashboard

The ESP32 hosts a web page that displays:

• Current detected object  
• Robot command state  
• Collection or avoidance decision

The dashboard automatically refreshes every **800 ms**.

---

# Communication

## HTTP Communication

The vision system sends the detected object to the ESP32 using:
/set_data

Example JSON:
{
"object": "trash"
}

The ESP32 updates the system state and forwards the result to the Arduino via I2C.

---

## I2C Commands

ESP32 sends commands to the Arduino:

| Value | Meaning |
|------|------|
| 1 | Trash detected |
| 0 | No trash |

Commands are sent periodically every **200 ms** as a heartbeat signal.

---

# Robot Behavior Logic
Move forward continuously

IF obstacle detected
IF trash detected
stop robot
run arm collection sequence
ELSE
turn left to avoid obstacle
ELSE
continue moving forward

---

# Robotic Arm Motion Sequence

1. Home position
2. Reach position
3. Collect position
4. Close gripper
5. Lift object
6. Return to home

Smooth servo transitions ensure stable arm movement.

---

# Web Status Interface

The ESP32 provides a **Mission Control dashboard** showing:

• AI recognition result  
• Current robot action  
• I2C command status

The page updates automatically every **800 ms**.

---

# Project Structure
ESP32_CODE/
esp32_code.ino

ARDUINO_CODE/
arduino_uno_code.ino

---
# Project Demonstration

A video demonstration of the system is available on LinkedIn.

The showcase presents the full workflow of the robot, including AI detection, autonomous navigation, and robotic arm trash collection.

▶ Watch the demonstration here:  
[(https://www.linkedin.com/posts/shams-eldeen-anter-59a6522a2_robotics-computervision-automation-activity-7416251619450904576-fDDg?utm_source=social_share_send&utm_medium=member_desktop_web&rcm=ACoAAEkRDcYB1Ht6TLmpMc3xhN5euPZo5ecC4RJtfJrJu8)]
---

# Future Improvements

• ROS2 integration for modular robot architecture  
• Autonomous navigation with SLAM  
• Advanced object classification using deep learning  
• Smart waste sorting system  
• Vision-guided manipulation: the robotic arm will be controlled using camera calibration to determine the precise collection point in the workspace.
• Inverse kinematics control: the end effector will be guided to the detected trash location using inverse kinematics to compute the required joint angles for accurate grasping.
