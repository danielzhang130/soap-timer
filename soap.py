#!/usr/bin/python3
import time
import board
import busio
import adafruit_adxl34x

from timer import Timer
from segment import Segment

i2c = busio.I2C(board.SCL, board.SDA)
accelerometer = adafruit_adxl34x.ADXL345(i2c)
accelerometer.enable_motion_detection(threshold=32)
timer = Timer(Segment())

while True:
    b = accelerometer.events['motion']
    if b:
        timer.start()
    time.sleep(1)



