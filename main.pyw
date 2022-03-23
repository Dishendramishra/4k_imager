from PyQt5 import uic
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import os

import resources

from time import sleep

from pprint import pprint
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
        self.threadpool = QThreadPool()
        print("Multithreading with maximum %d threads" % self.threadpool.maxThreadCount())
        self.serial_busy = True
        self.fw1 = {
                    1 : self.lbl_fw1_a,
                    2 : self.lbl_fw1_b,
                    3 : self.lbl_fw1_c,
                    4 : self.lbl_fw1_d,
                    5 : self.lbl_fw1_e
                    }

        self.fw2 = {
                    1 : self.lbl_fw2_a,
                    2 : self.lbl_fw2_b,
                    3 : self.lbl_fw2_c,
                    4 : self.lbl_fw2_d,
                    5 : self.lbl_fw2_e
                    }

        self.nd_leds = {1 : self.lbl_nd_set, 2 : self.lbl_nd_unset}

        self.led_on  = QPixmap(":/leds/images/leds/green.png")
        self.led_off = QPixmap(":/leds/images/leds/off.png")
        self.ser = None

        self.connect_ser()
        self.ui_settings()
        self.show()

        # self.load_filters_pos()

    def closeEvents(self):
        print("closing")
    
    def ui_settings(self):

        self.grbbox_ser.setVisible(False)
        
        self.btngrp_fw1 = QButtonGroup()
        self.btngrp_fw1.addButton(self.btn_fw1_a,1)
        self.btngrp_fw1.addButton(self.btn_fw1_b,2)
        self.btngrp_fw1.addButton(self.btn_fw1_c,3)
        self.btngrp_fw1.addButton(self.btn_fw1_d,4)
        self.btngrp_fw1.addButton(self.btn_fw1_home,5)
        self.btngrp_fw1.buttonClicked.connect(self.filter_wheel_1)

        self.btngrp_fw2 = QButtonGroup()
        self.btngrp_fw2.addButton(self.btn_fw2_a,1)
        self.btngrp_fw2.addButton(self.btn_fw2_b,2)
        self.btngrp_fw2.addButton(self.btn_fw2_c,3)
        self.btngrp_fw2.addButton(self.btn_fw2_d,4)
        self.btngrp_fw2.addButton(self.btn_fw2_home,5)
        self.btngrp_fw2.buttonClicked.connect(self.filter_wheel_2)

        self.btngrp_nd = QButtonGroup()
        self.btngrp_nd.addButton(self.btn_nd_set, 1)
        self.btngrp_nd.addButton(self.btn_nd_unset, 2)
        self.btngrp_nd.buttonClicked.connect(self.nd_filter)

    # +===============================================+
    # |             Thread Functions                  |
    # +===============================================+
    def move_servo_thread(self, str_cmd, led):
        self.send_cmd(str_cmd)
        self.read_output()
        if str_cmd.startswith("s1") or str_cmd.startswith("homestep1"):
            self.fw1[led].setPixmap(self.led_on)
        else:
            self.fw2[led].setPixmap(self.led_on)
        self.enable_buttons()

        self.serial_busy = False

    def nd_filter_thread(self, str_cmd, btn_id):
        self.send_cmd(str_cmd)
        self.read_output()
        self.nd_leds[btn_id].setPixmap(self.led_on)
        self.enable_buttons()
        
        self.serial_busy = False

    def load_filters_pos_thread(self):
        self.disable_buttons()
        sleep(1)
        self.send_cmd("filters")
        output = self.read_output()
        self.fw1[int(output[0])].setPixmap(self.led_on)
        self.fw2[int(output[1])].setPixmap(self.led_on)
        # print("output: ", output)
        self.enable_buttons()
    # +===============================================+


    # +===============================================+
    # |             Serial Functions                  |
    # +===============================================+
    def connect_ser(self):

        if not os.environ.get("IMAGER_PORT"):
            msg_box = QMessageBox()
            msg_box.setIcon(QMessageBox.Critical)
            msg_box.setText("Environment Var IMAGER_PORT not set!")
            msg_box.setWindowTitle("Environment Var Error!")
            msg_box.setStandardButtons(QMessageBox.Ok)
            msg_box.exec_()
            self.close()

        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            # print("{}: {} [{}]".format(port, desc, hwid))
            if os.environ["IMAGER_PORT"] in hwid:
                self.ser = serial.Serial(port, 115200)
                print("Serial connected on: {}".format(port))

    def send_cmd(self, str_cmd):
        # print("cmd: ", str_cmd)

        try:
            self.ser.write(str_cmd.encode())
        except:
            msg_box = QMessageBox()
            msg_box.setIcon(QMessageBox.Critical)
            msg_box.setText("Serial Error: Restart Software!")
            msg_box.setWindowTitle("Error!")
            msg_box.setStandardButtons(QMessageBox.Ok)
            msg_box.exec_()
            self.close()

    def send_cmd_thread(self, str_cmd):
        self.ser.write(str_cmd.encode())

    def read_output(self):
        # return "done"
        output = self.ser.readline().strip().decode()
        output = output.replace(" ","")
        return output
    
    def check_serial(self):
        if not self.ser:
            msg_box = QMessageBox()
            msg_box.setIcon(QMessageBox.Critical)
            msg_box.setText("Controller Not Connected!")
            msg_box.setWindowTitle("About")
            msg_box.setStandardButtons(QMessageBox.Ok)
            msg_box.exec_()
            return False
        return True
    # +===============================================+

    
    # +===============================================+
    # |           Button Click Functions              |
    # +===============================================+
    def load_filters_pos(self):
        worker = Worker(self.load_filters_pos_thread, [False, False, False])
        self.threadpool.start(worker)
    
    def filter_wheel_1(self, btn):
        self.disable_buttons()

        self.fw1_all_off()
        btn_id = self.btngrp_fw1.checkedId()

        print(f"btn_id : {btn_id}")

        if btn_id == 1:
            str_cmd = "s11"
        elif btn_id == 2:
            str_cmd = "s12"
        elif btn_id == 3:
            str_cmd = "s13"
        elif btn_id == 4:
            str_cmd = "s14"
        elif btn_id == 5:
            str_cmd = "homestep1"

        worker = Worker(self.move_servo_thread, [False, False, False], str_cmd, btn_id)
        self.threadpool.start(worker)

    def filter_wheel_2(self, btn):
        self.disable_buttons()

        self.fw2_all_off()
        btn_id = self.btngrp_fw2.checkedId()

        if btn_id == 1:
            str_cmd = "s21"
        elif btn_id == 2:
            str_cmd = "s22"
        elif btn_id == 3:
            str_cmd = "s23"
        elif btn_id == 4:
            str_cmd = "s24"
        elif btn_id == 5:
            str_cmd = "homestep2"

        worker = Worker(self.move_servo_thread, [False, False, False], str_cmd, btn_id)
        self.threadpool.start(worker)

    def nd_filter(self):
        btn_id = self.btngrp_nd.checkedId()
        if btn_id == 1:
            str_cmd = "ser65"
        else:
            str_cmd = "ser8"

        self.lbl_nd_set.setPixmap(self.led_off)
        self.lbl_nd_unset.setPixmap(self.led_off)
        self.disable_buttons()

        worker = Worker(self.nd_filter_thread, [False, False, False], str_cmd, btn_id)
        self.threadpool.start(worker)
    # +===============================================+

    def fw1_all_off(self):
        self.lbl_fw1_a.setPixmap(self.led_off)
        self.lbl_fw1_b.setPixmap(self.led_off)
        self.lbl_fw1_c.setPixmap(self.led_off)
        self.lbl_fw1_d.setPixmap(self.led_off)
        self.lbl_fw1_e.setPixmap(self.led_off)
    
    def fw2_all_off(self):
        self.lbl_fw2_a.setPixmap(self.led_off)
        self.lbl_fw2_b.setPixmap(self.led_off)
        self.lbl_fw2_c.setPixmap(self.led_off)
        self.lbl_fw2_d.setPixmap(self.led_off)
        self.lbl_fw2_e.setPixmap(self.led_off)

    def toggle_led(self, led, target):
        if led == self.led_on:
            target.setPixmap(self.led_off)
        else:
            target.setPixmap(self.led_on)        

    def disable_buttons(self):
        self.btn_fw1_a.setDisabled(True)
        self.btn_fw1_b.setDisabled(True)
        self.btn_fw1_c.setDisabled(True)
        self.btn_fw1_d.setDisabled(True)
        self.btn_fw1_home.setDisabled(True)
        self.btn_fw2_a.setDisabled(True)
        self.btn_fw2_b.setDisabled(True)
        self.btn_fw2_c.setDisabled(True)
        self.btn_fw2_d.setDisabled(True)
        self.btn_fw2_home.setDisabled(True)
        self.btn_nd_set.setDisabled(True)
        self.btn_nd_unset.setDisabled(True)

    def enable_buttons(self):
        self.btn_fw1_a.setDisabled(False)
        self.btn_fw1_b.setDisabled(False)
        self.btn_fw1_c.setDisabled(False)
        self.btn_fw1_d.setDisabled(False)
        self.btn_fw1_home.setDisabled(False)
        self.btn_fw2_a.setDisabled(False)
        self.btn_fw2_b.setDisabled(False)
        self.btn_fw2_c.setDisabled(False)
        self.btn_fw2_d.setDisabled(False)
        self.btn_fw2_home.setDisabled(False)
        self.btn_nd_set.setDisabled(False)
        self.btn_nd_unset.setDisabled(False)


app = QApplication(sys.argv)
app.setWindowIcon(QIcon("images/icons/prl2.png"))
apply_stylesheet(app, theme='dark_blue.xml')
window = Ui()
app.exec_()