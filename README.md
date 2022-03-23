![imager](https://user-images.githubusercontent.com/6905938/158106347-0d11167c-c0fb-4471-83a5-79874a11198f.png)


Converting `.qrc `  to regular `.py`  file.

```shell
pyrcc5 resources.qrc -o resources.py
```



Then import like a regular python file

```shell
import resources
```



### :warning: **System Configurations: :warning:**

**Must** define an environment variable called `IMAGER_PORT`, which points to the port number of the arduino board connected to the system.

---



### **Design Paradigms**

- **Qthread Tutorial using Qt Desinger**: https://www.youtube.com/watch?v=k5tIk7w50L4

  

### **Development Resources:**

- **Qt Desinger** : https://build-system.fman.io/qt-designer-download
- **VS Code** : https://code.visualstudio.com/download
- **Python 3.7.9** : https://www.python.org/downloads/release/python-379/
- **Arduino IDE** : https://www.arduino.cc/en/software 