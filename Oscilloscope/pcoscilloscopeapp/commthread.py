import serial
from serial.tools import list_ports
import sys
import threading
import numpy as np
import traceback      
import time

class CommunicationThread(threading.Thread):

    def __init__(self,app):
        threading.Thread.__init__(self)
        self.shutdown_flag = threading.Event()
        self.app=app

    def run(self):
        print("Start communicate")
        if self.app.ser != None :
            print("Serial close")
            #self.app.ser.close()
            #self.app.ser=None
        try:
            port=self.app.combo.currentData()
            print("Connect "+str(port))
            self.app.ser = serial.Serial(port, 9600, timeout=200)
            if self.app.ser == None:
                        print("Serial port is closed")
                        return
            (pres,timereg)=self.app.get_settimer(1)
            bapres=bytearray(1)
            bapres[0]=pres+ord('0')
            no_byte=self.app.ser.write(chr(pres).encode("latin1"))
            print(str(no_byte)+"bytes")
            print(chr(pres).encode("latin1"))
            print(ord(chr(pres).encode("latin1")))
            #print(self.app.ser.read(no_byte))
            time.sleep(5)
            #time.sleep(1)
            ba=bytearray(2)
            ba[0]=timereg%256
            ba[1]=int(timereg/256)
            no_byte=self.app.ser.write(chr(int(timereg/256)).encode("latin1"))
            print(str(no_byte)+"bytes")
            print(chr(int(timereg/256)).encode("latin1"))
            print(ord(chr(int(timereg/256)).encode("latin1")))
            #print(self.app.ser.read(no_byte))
            time.sleep(5)
            #time.sleep(1)
            no_byte=self.app.ser.write(chr(timereg%256).encode("latin1"))
            print(str(no_byte)+"bytes")
            print(chr(timereg%256).encode("latin1"))
            print(ord(chr(timereg%256).encode("latin1")))
            #print(self.app.ser.read(no_byte))
            time.sleep(5)
            #time.sleep(1)
            #print(bin(pres))
            #print(ba[1])
            #print(ba[0])
            while not self.shutdown_flag.is_set():
                vals=[]
                for i in range(self.app.sample_no):
                    if self.app.ser == None:
                        return
                    val=val=int.from_bytes(self.app.ser.read(2), "big")
                    vin=int(val)*5/1024
                    vals.append(vin)
                print("Samples: "+str(len(vals)))
                self.app.samples=vals
        except Exception as e:
             print(traceback.format_exc())
        print("thread end")
