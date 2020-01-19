import serial
from serial.tools import list_ports
import sys
from PyQt5.QtWidgets import *#QApplication,QSizePolicy, QWidget, QLabel , QPushButton , QComboBox,QMainWindow,QDockWidget
from PyQt5.QtGui import *#QIcon
from PyQt5.QtCore import *#QTimer,pyqtSlot
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import time,random
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import threading
import numpy as np
import traceback
import time
from commthread import CommunicationThread
from canvasfigure import PlotCanvas

class Window(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        grid = QGridLayout()
        self.sample_no=100
        self.samples=[0 for i in range(100)]
        self.ser=None
        self.curr_thread=None
        self.xdata=0
        self.ydata=0
        self.voltsperdiv=0.25
        self.timeus=1000000
        self.freqhz=1
        
        self.graphWidget = PlotCanvas(app=self,width=100, height=100)
        self.graphWidget.mpl_connect('button_press_event', self.onclick_coord)
        timer = QTimer(self)
        timer.timeout.connect(self.draw)
        timer.start(1000)
        grid.addWidget(self.graphWidget,0,0,1,4)

        self.button1 = QPushButton("Connect")
        #self.button1.setText("Refresh")
        #self.button1.move(64,32)
        self.button1.clicked.connect(lambda x:self.connect())
        grid.addWidget(self.button1,1,0)

        combolabel=QLabel("Serial port")
        grid.addWidget(combolabel,2,0)
        port_list=list_ports.comports()
        self.combo = QComboBox()
        for port in port_list:
            self.combo.addItem(str(port.description),str(port.device))
        if len(port_list)>0:
            self.combo.setCurrentIndex(0)
        #self.combo.highlighted[str].connect()
        grid.addWidget(self.combo,2,1)

        self.buttonrefr = QPushButton("Refresh port list")
        self.buttonrefr.clicked.connect(self.refresh)
        grid.addWidget(self.buttonrefr,2,2)
        
        baudlabel=QLabel("Baud Rate")
        grid.addWidget(baudlabel,3,0)
        baud = QLineEdit()
        baud.setValidator(QIntValidator())
        grid.addWidget(baud,3,1)
        self.yvaluelabel=QLabel(str(self.ydata))
        grid.addWidget(self.yvaluelabel,3,2)

        xdiflabel=QLabel("X value difference")
        grid.addWidget(xdiflabel,4,0)
        self.xdifval=QLabel(str(self.xdata))
        grid.addWidget(self.xdifval,4,1)
        ydiflabel=QLabel("Y value difference")
        grid.addWidget(ydiflabel,4,2)
        self.ydifval=QLabel(str(self.ydata))
        grid.addWidget(self.ydifval,4,3)

        voltdivlabel=QLabel("Volts/div")
        grid.addWidget(voltdivlabel,5,0)
        self.voltdivval = QLineEdit()
        self.voltdivval.setValidator(QDoubleValidator())
        grid.addWidget(self.voltdivval,5,1)
        self.buttonvertscale = QPushButton("Applyverticalscale")
        self.buttonvertscale.clicked.connect(self.applyverticalscale)
        grid.addWidget(self.buttonvertscale,5,2)

        timescale=QLabel("Time:")
        grid.addWidget(timescale,6,0)
        self.timescaleval = QLineEdit("1")
        self.timescaleval.setValidator(QDoubleValidator())
        #self.timescaleval.textChanged.connect(self.timechanged)
        grid.addWidget(self.timescaleval,6,1)
        self.combotimescale = QComboBox()
        self.combotimescale.addItem("s",1000000)
        self.combotimescale.addItem("ms",1000)
        self.combotimescale.addItem("us",1)        
        self.combotimescale.setCurrentIndex(0)
        #self.combotimescale.highlighted.connect(self.timechanged)
        grid.addWidget(self.combotimescale,6,2)
        self.buttontimescale = QPushButton("Applytimescale")
        self.buttontimescale.clicked.connect(self.timechanged)
        grid.addWidget(self.buttontimescale,6,3)

        freqscale=QLabel("Frequency:")
        grid.addWidget(freqscale,7,0)
        self.freqscaleval = QLineEdit("1")
        self.freqscaleval.setValidator(QDoubleValidator())
        grid.addWidget(self.freqscaleval,7,1)
        self.combofreqscale = QComboBox()
        self.combofreqscale.addItem("Mhz",1000000)
        self.combofreqscale.addItem("Khz",1000)
        self.combofreqscale.addItem("Hz",1)        
        self.combofreqscale.setCurrentIndex(2)
        grid.addWidget(self.combofreqscale,7,2)
        self.buttonfreqscale = QPushButton("Applyfreqscale")
        self.buttonfreqscale.clicked.connect(self.freqchanged)
        grid.addWidget(self.buttonfreqscale,7,3)
        
        self.setLayout(grid)
        self.setGeometry(50,50,1200,800)
        self.setWindowTitle("Oscilloscope")
        self.show()
        sys.exit(app.exec_())

    def timechanged(self):
        if len(self.timescaleval.text()) == 0:
            return
        try:
            index = int(self.combotimescale.currentIndex())
            combtimescaleval = int(self.combotimescale.currentData())
            timescalevalread = float(self.timescaleval.text())
            combofreqscaleval=int(self.combofreqscale.currentData())
            self.timeus = combtimescaleval * timescalevalread
            self.freqhz = float(1/self.timeus)*1000000
            #print(combtimescaleval)
            #print(timescalevalread)
            #print(combofreqscaleval)
            #print(self.freqhz)
            #print()
            self.freqscaleval.setText(str(self.freqhz/combofreqscaleval))
            self.update_controller_freq()
        except Exception as e:
             print(traceback.format_exc())

    def freqchanged(self):
        if len(self.freqscaleval.text()) == 0:
            return
        try:
            index = int(self.combofreqscale.currentIndex())
            combofreqscaleval = int(self.combofreqscale.currentData())
            freqscalevalread = float(self.freqscaleval.text())
            combtimescaleval=int(self.combotimescale.currentData())
            self.freqhz = combofreqscaleval * freqscalevalread
            self.timeus = float(1/self.freqhz)*1000000
            #print(combofreqscaleval)
            #print(freqscalevalread)
            #print(combtimescaleval)
            #print(self.freqhz)
            #print(self.timeus)
            #print()
            self.timescaleval.setText(str(self.timeus/combtimescaleval))
            self.update_controller_freq()
        except Exception as e:
             print(traceback.format_exc())

    def update_controller_freq(self):
        if(self.ser != None):
            try:
                (pres,timereg)=self.get_settimer(self.freqhz)
                if timereg/256 >= 256:
                    print("frequency too small")
                    return
                bapres=bytearray(1)
                bapres[0]=pres
                no_byte=self.ser.write(chr(pres).encode("latin1"))
                print(str(no_byte)+"bytes")
                print(chr(pres).encode("latin1"))
                print(ord(chr(pres).encode("latin1")))
                time.sleep(1)
                #print(pres)
                #print(timereg)
                ba=bytearray(2)
                ba[0]=timereg%256
                ba[1]=int(timereg/256)
                no_byte=self.ser.write(chr(int(timereg/256)).encode("latin1"))
                print(str(no_byte)+"bytes")
                print(chr(int(timereg/256)).encode("latin1"))
                print(ord(chr(int(timereg/256)).encode("latin1")))
                time.sleep(1)
                no_byte=self.ser.write(chr(timereg%256).encode("latin1"))
                print(str(no_byte)+"bytes")
                print(chr(timereg%256).encode("latin1"))
                print(ord(chr(timereg%256).encode("latin1")))
                time.sleep(1)
                #print(bapres[0])
                #print(bin(timereg))
                #print(bin(ba[1]))
                #print(bin(ba[0]))
            except Exception as e:
                print(traceback.format_exc())
     
    def get_settimer(self,freqhz):
        pres=0
        val=0
        if(freqhz<15000):
            pres=5
            val=1024
        elif(freqhz<60000):
            pres=4
            val=256
        elif(freqhz<250000):
            pres=3
            val=64
        elif(freqhz<2000000):
            pres=2
            val=8
        else:
            pres=1
            val=1  
        regval=int(16000000/val/freqhz)
        return (pres,regval)
    
    def applyverticalscale(self):
        try:
            self.voltsperdiv=float(self.voltdivval.text())
        except Exception as e:
             print(traceback.format_exc())    

    def onclick_coord(self,event):
        if event.xdata == None or event.ydata == None :
            return
        print('%s click: button=%d, x=%d, y=%d, xdata=%f, ydata=%f' %
          ('double' if event.dblclick else 'single', event.button,
           event.x, event.y, event.xdata, event.ydata))
        self.yvaluelabel.setText(str(event.ydata))
        try:
            timescale=int(self.combotimescale.currentData())
            freqscale=int(self.combofreqscale.currentData())
            timeval=float(self.timescaleval.text())
            freqval=float(self.freqscaleval.text())
            #print(timescale)
            #print(freqscale)
            #print(timeval)
            #print(freqval)
            #print()
        except:
            print(traceback.format_exc())
        timedif=float((event.xdata)-float(self.xdata))*timeval
        self.xdifval.setText(str(timedif)+" "+self.combotimescale.currentText()+" = "+str((1/timedif)*freqscale)+" "+self.combofreqscale.currentText())
        self.ydifval.setText(str(float(event.ydata)-float(self.ydata)))
        self.xdata=event.xdata
        self.ydata=event.ydata
        
        
    
    def refresh(self):
        self.combo.clear()
        port_list=list_ports.comports()
        for port in port_list:
            self.combo.addItem(str(port.description),str(port.device))
        if len(port_list)>0:
            self.combo.setCurrentIndex(0)

    def connect(self):
        print("Start connect")
        #CommunicationThread(self).start()
        try:
            self.curr_thread=CommunicationThread(self)
            self.curr_thread.start()
            print("Add disconnect")
            self.button1.setText("Disconnect")
            self.button1.clicked.connect(self.disconnect)
            #t=threading.Thread(target=self.communicate)
            #t.start()
        except Exception as e:
             print(traceback.format_exc())
        

        
    def disconnect(self):
        try:
            if self.ser != None:
                print("Close serial port disc")
                self.ser.close()
                self.ser=None
            self.button1.setText("Connect")
            self.button1.clicked.connect(self.connect)
        except Exception as e:
            print(traceback.format_exc())
            self.ser=None
        if self.curr_thread != None:
                self.curr_thread.shutdown_flag.set()
                self.curr_thread.join()
                self.curr_thread=None


    def communicate(self):
        print("Start communicate")
        if self.ser != None :
            print("Serial close")
            self.ser.close()
            self.ser=None
        try:
            port=self.combo.currentData()
            print("Connect"+str(port))
            self.ser = serial.Serial(port, 9600, timeout=1)
            self.ser.write(bin(0))
            self.ser.write(bin(60,000))#15,624
            while(True):
                print("Serial start")
                vals=[]
                for i in range(self.sample_no):
                    val=val=int.from_bytes(self.ser.read(2), "big")
                    vin=int(val)*5/1024
                    vals.append(vin)
                self.samples=vals
        except Exception as e:
             print(traceback.format_exc())
        print("thread end")

    
    def draw(self):
        self.graphWidget.plot(self.samples)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    widget = Window()
    widget.show()
    #port_list=list_ports.comports()
    #for port in port_list:
    #    print(port)
    #print(str(port.description)+" "+str(port.device)+" "+str(port.hwid)+" "+str(port.interface)+" "+str(port.name))
    #ser = serial.Serial(port_list[0].device, 9600)

