import threading

import RPi.GPIO as GPIO
import time

class Segment:
    def __init__(self):
        GPIO.setmode(GPIO.BCM)
 
        # GPIO ports for the 7seg pins
        #                 13,9,4, 2, 1, 12,5,x
        self.segments =  (8,18,25,15,14,7,5,21)
 
        for segment in self.segments:
            GPIO.setup(segment, GPIO.OUT)
            GPIO.output(segment, 0)
         
        # GPIO ports for the digit 2-3 pins 
        self.digits = (23,6) #10, 6
         
        for digit in self.digits:
            GPIO.setup(digit, GPIO.OUT)
            GPIO.output(digit, 1)
         
        self.num = {' ':(0,0,0,0,0,0,0),
            '0':(1,1,1,1,1,1,0),
            '1':(0,1,1,0,0,0,0),
            '2':(1,1,0,1,1,0,1),
            '3':(1,1,1,1,0,0,1),
            '4':(0,1,1,0,0,1,1),
            '5':(1,0,1,1,0,1,1),
            '6':(1,0,1,1,1,1,1),
            '7':(1,1,1,0,0,0,0),
            '8':(1,1,1,1,1,1,1),
            '9':(1,1,1,1,0,1,1)}
         
        self.s = "  "
        self.thread = threading.Thread(target=self.tick)
        self.thread.start()

    def set(self, num):
        if num < 0:
            return
        if num < 10:
            self.s = " " + str(num)
        else:
            self.s = str(num)

    def stop(self):
        self.s = "  "

    def tick(self):
        try:
            while True:
                if self.s == "  ":
                    time.sleep(1)
                    continue
                tmp = self.s
                for digit in range(len(tmp)):
                    for loop in range(0,7):
                        GPIO.output(self.segments[loop], self.num[tmp[digit]][loop])
                    GPIO.output(self.digits[digit], 0)
                    time.sleep(0.005)
                    GPIO.output(self.digits[digit], 1)
        finally:
            GPIO.cleanup()
