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

if __name__ == '__main__':
    port_list=list_ports.comports()
    for port in port_list:
        print(port)
    print(str(port.description)+" "+str(port.device)+" "+str(port.hwid)+" "+str(port.interface)+" "+str(port.name))
    ser = serial.Serial('COM5', 9600)
    while(True):
        val=ser.read(2)
        val=int.from_bytes(val, "big")
        print(val)
        vin=int(val)*5/1024
        print(vin)

