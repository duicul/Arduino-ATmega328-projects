import serial
import sys
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import traceback

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

        #FigureCanvas.setSizePolicy(self,QSizePolicy.Expanding,QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
        
    def plot(self,data):
      try:
        self.axes.clear()
        #ax = self.figure.add_subplot(111)
        minval=min(data)
        maxval=max(data)
        #print(minval)
        #print(maxval) 
        self.axes.set_xlim([0,self.app.sample_no])
        limmin=-5
        limmax=5
        if minval==maxval==0 or minval >= 0:
            limmin=0
            limmax=21*self.app.voltsperdiv
        elif maxval <= 0:
            limmax=0
            limmin=0-20*self.app.voltsperdiv
        else:
            limmin=0-((0-minval)/(maxval-minval)+1)*20*self.app.voltsperdiv
            limmax=0+((maxval-0)/(maxval-minval)+1)*20*self.app.voltsperdiv
        
        #print([limmin,limmax])    
        self.axes.set_ylim([limmin,limmax])
        limx=self.axes.get_xlim()
        limy=self.axes.get_ylim()
        #print(limy)
        #print(delta)
        #print(np.arange(limx[0], limx[1], 0.25))
        if limy[1] <= 0 :
           totalarray=np.arange(limy[0]-self.app.voltsperdiv,0,self.app.voltsperdiv)
        elif limy[0] >= 0 :
            totalarray=np.arange(0,limy[1]+self.app.voltsperdiv,self.app.voltsperdiv)
        else:
            totalarray=np.append(np.arange(0,limy[1]-self.app.voltsperdiv, -self.app.voltsperdiv),np.arange(0,limy[1]+self.app.voltsperdiv, self.app.voltsperdiv))
        #print(limy)
        #print(len(totalarray))
        #print(totalarray)
        #print(limx)
        #print(limy)
        for i in range(self.app.sample_no):
            if i%5==0:
                self.axes.axvline(i)
        self.axes.yaxis.set_major_locator(ticker.FixedLocator(totalarray))
        self.axes.grid(True)
        #print(data)
        self.axes.plot(data, 'r-')
        self.axes.xaxis.set_major_locator(ticker.MultipleLocator(10))
        timescalename=self.app.combotimescale.currentText()
        timeval=float(self.app.timescaleval.text())
        ticksaux=[0,0,1,2,3,4,5,6,7,8,9,10]
        ticks=[]
        for i in ticksaux:
            ticks.append(str(i*timeval*10)+" "+timescalename)
        self.axes.set_xticklabels(ticks)
        self.axes.set_title('Screen')
        self.draw()
      except Exception as e:
             print(traceback.format_exc())
