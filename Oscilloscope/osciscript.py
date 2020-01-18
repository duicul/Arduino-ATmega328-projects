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

        self.setLayout(grid)
        self.setGeometry(50,50,1200,800)
        self.setWindowTitle("Oscilloscope")
        self.show()
        sys.exit(app.exec_())

    def onclick_coord(self,event):
        if event.xdata == None or event.ydata == None :
            return
        print('%s click: button=%d, x=%d, y=%d, xdata=%f, ydata=%f' %
          ('double' if event.dblclick else 'single', event.button,
           event.x, event.y, event.xdata, event.ydata))
        self.yvaluelabel.setText(str(event.ydata))
        self.xdifval.setText(str(float(event.xdata)-float(self.xdata)))
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
            #t=threading.Thread(target=self.communicate)
            #t.start()
        except Exception as e:
             print(sys.exc_info()[1])
        print("Add disconnect")
        self.button1.setText("Disconnect")
        self.button1.clicked.connect(self.disconnect)

        
    def disconnect(self):
        if self.ser != None:
            print("Close serial port disc")
            self.ser.close()
        self.ser=None
        if self.curr_thread != None:
            self.curr_thread.shutdown_flag.set()
            self.curr_thread.join()
            self.curr_thread=None
        self.button1.setText("Connect")
        self.button1.clicked.connect(self.connect)

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
            while(True):
                print("Serial start")
                vals=[]
                for i in range(self.sample_no):
                    val=val=int.from_bytes(self.ser.read(2), "big")
                    vin=int(val)*5/1024
                    vals.append(vin)
                self.samples=vals
        except Exception as e:
             print(sys.exc_info()[1])
        print("thread end")

    
    def draw(self):
        self.graphWidget.plot(self.samples)

class PlotCanvas(FigureCanvas):

    def __init__(self,app, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        self.xticks_label=[0]
        self.app=app
        for i in range(11):
            self.xticks_label.append(str(i))
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        
    def plot(self,data):
      try:
        self.axes.clear()
        #ax = self.figure.add_subplot(111)
        minval=min(data)
        maxval=max(data)
        #print(minval)
        #print(maxval)
        delta=0
        if minval==maxval:
             if minval == 0:
                 minval= 0
                 maxval= 1
             elif minval<0:
                 minval=minval-(-minval)
                 maxval=0
             else:
                 maxval=maxval*2
                 minval=0 
        else:
            delta=(maxval-minval)*0.2
        self.axes.set_xlim([0,100])
        
        if(maxval<=0):
            maxval=0-delta
        elif(minval >= 0):
            minval=0+delta
        self.axes.set_ylim([minval-delta,maxval+delta])
        limx=self.axes.get_xlim()
        limy=self.axes.get_ylim()
        #print(limy)
        #print(delta)
        #print(np.arange(limx[0], limx[1], 0.25))
        if limy[1] < 0 :
           totalarray=np.arange(lim[0]-0.25,0,0.25)
        elif limy[0] > 0 :
            totalarray=np.arange(0,lim[1]+0.25,0.25)
        else:
            totalarray=np.append(np.arange(0,limy[1]-0.25, -0.25),np.arange(0,limy[1]+0.25, 0.25))
        #print(totalarray)
        #print(limx)
        #print(limy)
        for i in range(self.app.sample_no):
            if i%5==0:
                self.axes.axvline(i)
        self.axes.yaxis.set_major_locator(ticker.FixedLocator(totalarray))
        self.axes.grid(True)
        self.axes.plot(data, 'r-')
        self.axes.xaxis.set_major_locator(ticker.MultipleLocator(10))
        self.axes.set_xticklabels([0,0,1,2,3,4,5,6,7,8,9,10])
        self.axes.set_title('Screen')
        self.draw()
      except Exception as e:
             print(sys.exc_info()[1])
      

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
            while not self.shutdown_flag.is_set():
                vals=[]
                for i in range(self.app.sample_no):
                    if self.app.ser == None:
                        print("Serial port is closed")
                        break
                    val=val=int.from_bytes(self.app.ser.read(2), "big")
                    vin=int(val)*5/1024
                    vals.append(vin)
                print("Samples: "+str(len(vals)))
                self.app.samples=vals
        except Exception as e:
             print(sys.exc_info()[1])
        print("thread end")
        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    widget = Window()
    widget.show()
    #port_list=list_ports.comports()
    #for port in port_list:
    #    print(port)
    #print(str(port.description)+" "+str(port.device)+" "+str(port.hwid)+" "+str(port.interface)+" "+str(port.name))
    #ser = serial.Serial(port_list[0].device, 9600)

