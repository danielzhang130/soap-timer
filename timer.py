import threading
import time

class Timer:
    def __init__(self, segment):
        self.count = 20
        self.started = False
        self.thread = threading.Thread(target=self.tick)
        self.thread.start()
        self.segment = segment

    def start(self):
        self.started = True
        self.count = 20
        #print("staring")

    def tick(self):
        while True:
            if self.started:
                #print(self.count)
                self.segment.set(self.count)
                self.count -= 1
            if self.count is -2:
                self.started = False
                self.segment.stop()

            time.sleep(1)


