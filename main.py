from PyQt5 import uic
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *

import sys
from qt_material import apply_stylesheet
import traceback, sys
import serial
import serial.tools.list_ports

if sys.platform == "linux" or sys.platform == "linux2":
    pass

elif sys.platform == "win32":
    import ctypes
    myappid = u'prl.microk'  # arbitrary string
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

elif sys.platform == "darwin":
    pass

class WorkerSignals(QObject):
    error = pyqtSignal(tuple)
    progress = pyqtSignal(int)
    result = pyqtSignal(object)
    finished = pyqtSignal()
    
class Worker(QRunnable):

    def __init__(self, fn, signals_flag, *args, **kwargs):
        # signals_flag = [progress, result, finished]

        super(Worker, self).__init__()
        self.fn = fn
        self.args = args
        self.kwargs = kwargs    
        self.signals = WorkerSignals()
        self.signals_flag = signals_flag

        if self.signals_flag[0]:
            kwargs['progress_callback'] = self.signals.progress

    @pyqtSlot()
    def run(self):
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            traceback.print_exc()
            exctype, value = sys.exc_info()[:2]
            self.signals.error.emit((exctype, value, traceback.format_exc()))
        else:
            if self.signals_flag[1]:
                self.signals.result.emit(result) # Return the result of the processing
        finally:
            if self.signals_flag[2]:
                self.signals.finished.emit() # Done

class Ui(QMainWindow):
    
    def __init__(self, *args, **kwargs):
        super(Ui, self).__init__(*args, **kwargs)

        uic.loadUi('gui.ui', self)

        # self.threadpool = QThreadPool()
        # print("Multithreading with maximum %d threads" % self.threadpool.maxThreadCount())
        self.ser = None
        
        self.ui_settings()
        self.show()

    def ui_settings(self):
        None
        # self.grbbox_ser.setVisible(False)

        # =========================================================================
        #                               ND Filter
        # =========================================================================
        # =========================================================================
        
    def toggle_led(self, led, target):
        if led == "resources/icons/off.png":
            target.setPixmap(QPixmap("resources/icons/on.png").scaled(32,32))
            return "resources/icons/on.png"
        else:
            target.setPixmap(QPixmap("resources/icons/off.png").scaled(32,32))
            return "resources/icons/off.png"
        

    def connect_ser(self):
        self.ser = serial.Serial(None, 115200)
    
    def send_cmd(self, str_cmd):
        # print("cmd: ", str_cmd)
        self.ser.write(str_cmd.encode())
    
    def read_output(self):
        # return "done"
        output = self.ser.readline().strip().decode()
        output = output.replace(" ","")
        return output

app = QApplication(sys.argv)
app.setWindowIcon(QIcon("resources/icons/prl2.png"))
apply_stylesheet(app, theme='dark_blue.xml')
window = Ui()
app.exec_()