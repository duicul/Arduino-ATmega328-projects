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

class Window(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        grid = QGridLayout()
        self.samples=100
        self.ser=None
        m = PlotCanvas(width=100, height=100)
        #m.move(1,1)
        timer = QTimer(self)
        timer.timeout.connect(m.plot)
        timer.start(1000)
        grid.addWidget(m,0,0,1,2)

        self.button1 = QPushButton("Connect")
        #self.button1.setText("Refresh")
        #self.button1.move(64,32)
        self.button1.clicked.connect(self.connect)
        grid.addWidget(self.button1,1,0)

        combolabel=QLabel("Serial port")
        grid.addWidget(combolabel,2,0)
        port_list=list_ports.comports()
        self.combo = QComboBox()
        for port in port_list:
            self.combo.addItem(str(port.description),str(port.device))
        if len(port_list)>0:
            self.combo.setCurrentIndex(0)
        self.combo.activated[str].connect(self.refresh)
        grid.addWidget(self.combo,2,1)
        
        baudlabel=QLabel("Baud Rate")
        grid.addWidget(baudlabel,3,0)
        baud = QLineEdit()
        baud.setValidator(QIntValidator())
        grid.addWidget(baud,3,1)

        self.setLayout(grid)
        self.setGeometry(50,50,1200,800)
        self.setWindowTitle("Oscilloscope")
        self.show()
        sys.exit(app.exec_())

    def refresh(self,text):
        print("Refresh "+text)

    def connect(self):
        port=self.combo.currentData()
        if self.ser != None :
             self.ser.close()
        self.ser = serial.Serial('COM5', 9600)
        vals=[]
        for i in range(self.samples):
             val=val=int.from_bytes(ser.read(2), "big")
             vin=int(val)*5/1024
             vals.append(vin)
        self.button1.setText("Disconnect")
        self.button1.clicked.connect(self.disconnect)

    def disconnect(self):
        self.ser.close()
        self.ser=None
        self.button1.setText("Connect")
        self.button1.clicked.connect(self.connect)

        
    def draw(self):
        hour = [1,2,3,4,5,6,7,8,9,10]
        temperature = []
        for i in range(10):
            temperature.append(random.random()*50)

        # plot data: x, y values
        self.graphWidget.plot(hour, temperature)
        time.sleep(1000)
        self.draw()

class PlotCanvas(FigureCanvas):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        self.xticks_label=[0]
        for i in range(11):
            self.xticks_label.append(str(i))
        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        self.plot()


    def plot(self):
        data = [(random.random()-0.5)*10 for i in range(100)]
        #ax = self.figure.add_subplot(111)
        self.axes.set_xlim([0,100])
        self.axes.set_xlim([-5,5])
        self.axes.axhline(0)
        self.axes.axvline(2)
        self.axes.clear()
        for i in range(-5,5):
            self.axes.axhline(i)
        for i in range(10):
            self.axes.axvline(i*10)
        self.axes.plot(data, 'r-')
        #self.axes.set_xticks(self.xticks)
        self.axes.xaxis.set_major_locator(ticker.MultipleLocator(10))
        self.axes.set_xticklabels(self.xticks_label)
        self.axes.set_title('Screen')
        self.draw()

        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    widget = Window()
    widget.show()
    #port_list=list_ports.comports()
    #for port in port_list:
    #    print(port)
    #print(str(port.description)+" "+str(port.device)+" "+str(port.hwid)+" "+str(port.interface)+" "+str(port.name))
    #ser = serial.Serial(port_list[0].device, 9600)

