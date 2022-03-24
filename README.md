![imager](https://user-images.githubusercontent.com/6905938/158106347-0d11167c-c0fb-4471-83a5-79874a11198f.png)

---

## System Configurations: :warning:

**Must** define an environment variable called `IMAGER_PORT`, which points to the port number of the arduino board connected to the system.



### Installing pip

If you don't have pip installed, use the file `get-pip.py` in this repo to install pip.

```shell
python get-pip.py
```



### Installing 4k_imager

Use the file `install.py` in this repo to install the software. Make sure that you have pip installed, before executing the command below .

```shell
python install.py
```



### Miscellaneous

Converting `.qrc `  to regular `.py`  file.

```shell
pyrcc5 resources.qrc -o resources.py
```



---



### **Design Paradigms**

- **Qthread Tutorial using Qt Desinger**: https://www.youtube.com/watch?v=k5tIk7w50L4

  

### **Development Resources:**

- **Qt Desinger** : https://build-system.fman.io/qt-designer-download
- **VS Code** : https://code.visualstudio.com/download
- **Python 3.7.9** : https://www.python.org/downloads/release/python-379/
- **Arduino IDE** : https://www.arduino.cc/en/software 