#!/usr/bin/env python

import math
import struct
from collections import deque

from PyQt4 import QtCore, QtGui, uic


class DataDialog(QtGui.QDialog):
    """
    Generic dialog to ask for/show data based on a given schema.
    """

    def __init__(self, schema, editable):
        QtGui.QDialog.__init__(self)
        self.ui = uic.loadUi("DataDialog.ui", self)

        self.ui.buttonBox.accepted.connect(self.retrieveData)

        #  Remove, Create, Replace
        self.ui.gridLayout.removeWidget(self.ui.widgetSchema)
        self.ui.widgetSchema.close()
        self.ui.widgetSchema = DataDialog.resolveSchema(schema, 0, editable)
        self.ui.gridLayout.addWidget(self.ui.widgetSchema, 0, 0, 1, 1)
        self.ui.gridLayout.update()

    @staticmethod
    def resolveSchema(schema, nesting=0, editable=True):
        """
        Transforms a string schema into a widget (or series of nested widgets)
        """
        if len(schema) == 0:
            raise IndexError()

        element = schema.popleft()
        if element == "i":
            return IntegerWidget().setEditable(editable)
        elif element == "r":
            return RealWidget().setEditable(editable)
        elif element == "c":
            return CharacterWidget().setEditable(editable)
        elif element == "s":
            return StringWidget().setEditable(editable)
        elif element == "b":
            return BooleanWidget().setEditable(editable)
        elif element == "[":
            if nesting > .5:
                schema.appendleft(element)
                return NestedWidget(schema, editable)
            else:
                return VectorWidget(schema, nesting, editable)
        elif element == "(":
            if nesting > 0:
                schema.appendleft(element)
                return NestedWidget(schema, editable)
            else:
                return StructWidget(schema, nesting, editable)
        else:
            return DataDialog.resolveSchema(schema, nesting, editable)

    @staticmethod
    def decryptString(stringDeque, terminator):
        """
        Reads a string from a deque of characters, until the terminator (single
        value or list of values) is found.
        """
        if terminator is not tuple:
            terminator = (terminator)

        result = ""
        character = stringDeque.popleft()
        while character not in terminator:
            result += character
            character = stringDeque.popleft()

        return result

    def show(self, data=None):
        """
        Shows the dialog, eventually filling its widgets with the given data.
        """
        if data is not None:
            self.ui.widgetSchema.setData(data)
        QtGui.QDialog.show(self)

    def retrieveData(self):
        """
        Retrieves the data from the dialog, called when pressing the OK button.
        """
        self.data = self.ui.widgetSchema.getData()


class OutputDialog(DataDialog):

    def __init__(self, schema):
        DataDialog.__init__(self, schema, False)


class InputDialog(DataDialog):

    def __init__(self, schema):
        DataDialog.__init__(self, schema, True)


class DataWidget(QtGui.QWidget):
    """
    Generic widget to be implemented for each type.
    """

    def __init__(self):
        QtGui.QWidget.__init__(self)

    def setEditable(self, editable):
        try:
            self.ui.inputBox.setReadOnly(not editable)
        except AttributeError:
            self.ui.setEnabled(editable)

        return self

    def setData(self, data):
        self.ui.inputBox.setText(str(data))

    def getData(self):
        return list(str(self.ui.inputBox.text()))


class IntegerWidget(DataWidget):
    DATA_FORMAT = "<i"

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("IntegerWidget.ui", self)

    def getData(self):
        return list(struct.pack(self.DATA_FORMAT,
                                int(self.ui.inputBox.text())))

    def setData(self, data):
        dataOfInterest = ""
        for i in range(struct.calcsize(self.DATA_FORMAT)):
            dataOfInterest += data.popleft()

        DataWidget.setData(self, struct.unpack(self.DATA_FORMAT,
                                               dataOfInterest)[0])


class RealWidget(DataWidget):
    DATA_FORMAT = "<f"

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("RealWidget.ui", self)

    def getData(self):
        return list(struct.pack(self.DATA_FORMAT,
                                float(self.ui.inputBox.text())))

    def setData(self, data):
        dataOfInterest = ""
        for i in range(struct.calcsize(self.DATA_FORMAT)):
            dataOfInterest += data.popleft()

        DataWidget.setData(self, struct.unpack(self.DATA_FORMAT,
                                               dataOfInterest)[0])


class CharacterWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("CharacterWidget.ui", self)

    def setData(self, data):
        DataWidget.setData(self, data.popleft())


class StringWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("StringWidget.ui", self)

    def setData(self, data):
        dataOfInterest = data.popleft()
        while dataOfInterest[-1] != '\0':
            dataOfInterest = data.popleft()

        DataWidget.setData(self, dataOfInterest)

    def getData(self):
        return list(str(self.ui.inputBox.toPlainText()) + '\0')


class BooleanWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("BooleanWidget.ui", self)

    def setData(self, data):
        if data.popleft() == '1':
            self.ui.inputBox.setChecked(True)
        else:
            self.ui.inputBox.setChecked(False)

    def getData(self):
        return ['1' if self.ui.inputBox.isChecked() else '0']


class VectorWidget(DataWidget):

    def __init__(self, schema, nesting, editable):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("VectorWidget.ui", self)
        self.size = int(DataDialog.decryptString(schema, ','))

        self.ui.widgets = []
        self.ui.horizontal = (math.floor(nesting) % 2 == 0)
        for i in range(self.size-1):
            self.ui.widgets.append(
                DataDialog.resolveSchema(deque(schema), nesting+1, editable))
            self.ui.gridLayout.addWidget(self.ui.widgets[i],
                                         i if self.ui.horizontal else 0,
                                         0 if self.ui.horizontal else i)

        # Awful: last time, no copy
        self.ui.widgets.append(
            DataDialog.resolveSchema(schema, nesting+1, editable))
        self.ui.gridLayout.addWidget(self.ui.widgets[self.size-1],
                                     self.size-1 if self.ui.horizontal else 0,
                                     0 if self.ui.horizontal else self.size-1)
        self.ui.gridLayout.update()

        schema.popleft()  # Closed square bracket ending the vector's schema

    def setData(self, data):
        for widget in self.ui.widgets:
            widget.setData(data)

    def getData(self):
        result = []

        for widget in self.ui.widgets:
            result += widget.getData()

        return result


class StructWidget(DataWidget):

    def __init__(self, schema, nesting, editable):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("StructWidget.ui", self)
        self.ui.widgets = []

        character = schema.popleft()
        i = 0
        while character != ")":
            name = character if character != "," else ""
            name += DataDialog.decryptString(schema, ":")

            widget = DataDialog.resolveSchema(schema, nesting+0.5, editable)
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

    def setData(self, data):
        for widget in self.ui.widgets:
            widget[1].setData(data)

    def getData(self):
        result = []

        for widget in self.ui.widgets:
            result += widget[1].getData()

        return result


class NestedWidget(DataWidget):
    """
    For types which are too nested, a nested widget is used.
    This just shows as a button which, when clicked, displays a new dialog
    related to just a subpart of the schema.
    """

    def __init__(self, schema, editable=True):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("NestedWidget.ui", self)
        self.ui.pushButton.clicked.connect(self.showWindow)
        self.dataDialog = DataDialog(schema, editable)

        self.data = None

    @QtCore.pyqtSlot()
    def showWindow(self):
        self.dataDialog.show(deque(self.data))

    def setData(self, data):
        # A bit of an hack... Copies the data and then consumes the part
        # relative to its content
        self.data = deque(data)
        self.dataDialog.widgetSchema.setData(data)

    def getData(self):
        if self.data is None:
            # Not the top of the usability, but it works...
            self.showWindow()
            if self.dataDialog.exec_():
                self.setData(self.dataDialog.data)

        return self.data


def requestInput(textualSchema):
    """
    External function to ask for data.
    """
    inputDialog = InputDialog(deque(textualSchema))
    inputDialog.show()
    if inputDialog.exec_():
        return "".join(inputDialog.data)


def requestOutput(textualSchema, data):
    """
    External function to show data.
    """
    outputDialog = OutputDialog(deque(textualSchema))
    outputDialog.show(deque(data))
