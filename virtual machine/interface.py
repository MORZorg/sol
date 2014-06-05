#!/usr/bin/env python

import sys
import math
from collections import deque

from PyQt4 import QtCore, QtGui, uic


class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.ui = uic.loadUi("MainWindow.ui", self)

        self.connect(self.ui.actionOpen,
                     QtCore.SIGNAL('triggered()'),
                     self,
                     QtCore.SLOT('open()'))

        self.connect(self.ui.pushButton,
                     QtCore.SIGNAL('clicked()'),
                     self,
                     QtCore.SLOT('requestInput()'))

    @QtCore.pyqtSlot()
    def open(self):
        fileDialog = QtGui.QFileDialog(self, "Open file")
        fileDialog.show()

    @QtCore.pyqtSlot()
    def requestInput(self):
        inputDialog = InputDialog(deque(str(self.ui.lineEdit.text())))
        inputDialog.show()


class InputDialog(QtGui.QDialog):

    def __init__(self, schema):
        QtGui.QDialog.__init__(self)
        self.ui = uic.loadUi("InputDialog.ui", self)

        #  Remove, Create, Replace
        self.ui.gridLayout.removeWidget(self.ui.widgetSchema)
        self.ui.widgetSchema.close()
        self.ui.widgetSchema = InputDialog.resolveSchema(schema)
        self.ui.gridLayout.addWidget(self.ui.widgetSchema, 0, 0, 1, 1)
        self.ui.gridLayout.update()

    @staticmethod
    def resolveSchema(schema, nesting=0):
        if len(schema) == 0:
            raise IndexError()

        element = schema.popleft()
        if element == "i":
            return IntegerWidget()
        elif element == "r":
            return RealWidget()
        elif element == "c":
            return CharacterWidget()
        elif element == "s":
            return StringWidget()
        elif element == "b":
            return BooleanWidget()
        elif nesting > 1.5:
            schema.appendleft(element)
            return NestedWidget(schema)
        elif element == "[":
            return VectorWidget(schema, nesting)
        elif element == "(":
            return StructWidget(schema, nesting)
        else:
            return InputDialog.resolveSchema(schema, nesting)

    @staticmethod
    def decryptString(schema, terminator):
        result = ""
        character = schema.popleft()
        while character != terminator:
            result += character
            character = schema.popleft()

        return result


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


class VectorWidget(QtGui.QWidget):

    def __init__(self, schema, nesting):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("VectorWidget.ui", self)
        self.size = int(InputDialog.decryptString(schema, ","))

        self.ui.widgets = []
        self.ui.horizontal = (math.floor(nesting) % 2 == 0)
        for i in range(self.size-1):
            self.ui.widgets.append(
                InputDialog.resolveSchema(deque(schema), nesting+1))
            self.ui.gridLayout.addWidget(self.ui.widgets[i],
                                         i if self.ui.horizontal else 0,
                                         0 if self.ui.horizontal else i)

        # Awful: last time, no copy
        self.ui.widgets.append(InputDialog.resolveSchema(schema, nesting+1))
        self.ui.gridLayout.addWidget(self.ui.widgets[self.size-1],
                                     self.size-1 if self.ui.horizontal else 0,
                                     0 if self.ui.horizontal else self.size-1)
        self.ui.gridLayout.update()

        schema.popleft()  # Closed square bracket ending the vector's schema


class StructWidget(QtGui.QWidget):

    def __init__(self, schema, nesting):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("StructWidget.ui", self)
        self.ui.widgets = []

        character = schema.popleft()
        i = 0
        while character != ")":
            name = character if character != "," else ""
            name += InputDialog.decryptString(schema, ":")

            widget = InputDialog.resolveSchema(schema, nesting+0.5)
            label = QtGui.QLabel(name)
            label.setAlignment(QtCore.Qt.AlignTop)
            label.setBuddy(widget)
            self.ui.widgets.append((label, widget))
            self.ui.gridLayout.addWidget(self.ui.widgets[i][0], i, 0)
            self.ui.gridLayout.addWidget(self.ui.widgets[i][1], i, 1)

            # Comma separating elements of the struct or closed bracket
            character = schema.popleft()
            i += 1
        self.ui.gridLayout.update()


class NestedWidget(QtGui.QWidget):

    def __init__(self, schema):
        QtGui.QWidget.__init__(self)
        self.ui = uic.loadUi("NestedWidget.ui", self)
        self.connect(self.ui.pushButton,
                     QtCore.SIGNAL('clicked()'),
                     self,
                     QtCore.SLOT('requestInput()'))
        self.inputDialog = InputDialog(schema)

    @QtCore.pyqtSlot()
    def requestInput(self):
        self.inputDialog.show()


app = QtGui.QApplication(sys.argv)
mainWindow = MainWindow()
mainWindow.show()
sys.exit(app.exec_())
