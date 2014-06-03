#!/usr/bin/env python

import sys
from PyQt4 import QtCore, QtGui, uic

class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.ui = uic.loadUi("MainWindow.ui", self)
        self.connect(self.ui.actionOpen, QtCore.SIGNAL('triggered()'), self, QtCore.SLOT('open()'))
        self.connect(self.ui.pushButton, QtCore.SIGNAL('clicked()'), self, QtCore.SLOT('requestInput()'))

    @QtCore.pyqtSlot()
    def open(self):
        fileDialog = QtGui.QFileDialog(self, "Open file")
        fileDialog.show()

    @QtCore.pyqtSlot()
    def requestInput(self):
        inputDialog = InputDialog(self.ui.lineEdit.text())
        inputDialog.show()

class InputDialog(QtGui.QDialog):

    def __init__(self, schema):
        QtGui.QDialog.__init__(self)
        self.ui = uic.loadUi("InputDialog.ui", self)
        self.ui.lineEdit.setText(self.doSomething(schema.split(" ")))

    def doSomething(self, array):
        if len(array) == 0:
            return ""

        if array[0] == "INT":
            text =  "blob"
        else:
            text =  "bio"

        return text + self.doSomething(array[1:])


app = QtGui.QApplication(sys.argv)
mainWindow = MainWindow()
mainWindow.show()
sys.exit(app.exec_())
