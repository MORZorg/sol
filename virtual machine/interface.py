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

        #  Remove, Create, Replace
        self.ui.gridLayout.removeWidget(self.ui.widgetSchema)
        self.ui.widgetSchema.close()
        self.ui.widgetSchema = InputDialog.resolveSchema(schema.split(" "))
        self.ui.gridLayout.addWidget(self.ui.widgetSchema, 0, 0, 1, 1)
        self.ui.gridLayout.update()

    @staticmethod
    def resolveSchema(array):
        if len(array) == 0:
            return ""

        if array[0] == "INT":
            return IntegerWidget()
        elif array[0] == "REAL":
            return RealWidget()
        elif array[0] == "CHAR":
            return CharacterWidget()
        elif array[0] == "STRING":
            return StringWidget()
        elif array[0] == "BOOL":
            return BooleanWidget()
        else:
            return ""

class IntegerWidget(QtGui.QWidget):

    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("IntegerWidget.ui", self)

class RealWidget(QtGui.QWidget):

    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("RealWidget.ui", self)

class CharacterWidget(QtGui.QWidget):

    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("CharacterWidget.ui", self)

class StringWidget(QtGui.QWidget):

    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("StringWidget.ui", self)

class BooleanWidget(QtGui.QWidget):

    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("BooleanWidget.ui", self)


app = QtGui.QApplication(sys.argv)
mainWindow = MainWindow()
mainWindow.show()
sys.exit(app.exec_())
