#!/usr/bin/env python3

import struct
from collections import deque

from PyQt5 import QtCore, QtWidgets, uic

from settings import DEBUG


class ByteDeque:
    def __init__(self, byteData):
        if type(byteData) is ByteDeque:
            self.data = byteData.data
            self.start = byteData.start
        else:
            self.data = byteData
            self.start = 0

    def unpack(self, format):
        result = struct.unpack_from(format, self.data, self.start)
        self.start += struct.calcsize(format)
        return result

    def __len__(self):
        return len(self.data) - self.start


class DataDialog(QtWidgets.QDialog):
    """
    Generic dialog to ask for/show data based on a given schema.
    """

    def __init__(self, schema, editable):
        QtWidgets.QDialog.__init__(self)
        self.ui = uic.loadUi("DataDialog.ui", self)

        self.ui.buttonBox.accepted.connect(self.retrieveData)

        self.setWindowTitle("SOL {}".format("Input" if editable else "Output"))

        #  Remove, Create, Replace
        self.ui.gridLayout.removeWidget(self.ui.widgetSchema)
        self.ui.widgetSchema.close()
        self.ui.widgetSchema = \
            DataDialog.resolveSchema(schema, editable=editable)
        self.ui.gridLayout.addWidget(self.ui.widgetSchema, 0, 0, 1, 1)
        self.ui.gridLayout.update()

    @staticmethod
    def resolveSchema(schema, nesting=None, editable=True):
        """
        Transforms a string schema into a widget (or series of nested
        widgets)
        """
        if len(schema) == 0:
            raise IndexError()

        if nesting is None:
            nesting = {'s': 0, 'v': 0}

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
            if nesting['s'] > 1 or nesting['v'] > 1:
                schema.appendleft(element)
                return NestedWidget(schema, editable)
            else:
                return VectorWidget(schema, dict(nesting), editable)
        elif element == "(":
            if nesting['s'] > 1 or nesting['v'] > 1:
                schema.appendleft(element)
                return NestedWidget(schema, editable)
            else:
                return StructWidget(schema, dict(nesting), editable)
        else:
            return DataDialog.resolveSchema(schema, nesting, editable)

    @staticmethod
    def decryptString(stringDeque, terminator):
        """
        Reads a string from a deque of characters, until the terminator
        (single value or list of values) is found.
        """
        if not stringDeque:
            return ""

        if terminator is not tuple:
            terminator = (terminator)

        result = ""
        character = stringDeque.popleft()
        while character not in terminator:
            result += character

            if stringDeque:
                character = stringDeque.popleft()
            else:
                return result

        return result

    @staticmethod
    def decryptBytes(stringDeque, terminator):
        """
        Reads a string from a deque of byte characters, until the
        terminator (single value or list of values) is found.
        """
        if not stringDeque:
            return b""

        if terminator is not tuple:
            terminator = (terminator)

        result = b""
        character = stringDeque.unpack("c")[0]
        while character not in terminator:
            result += character

            if stringDeque:
                character = stringDeque.unpack("c")[0]
            else:
                return result

        return result

    def show(self, data=None):
        """
        Shows the dialog, eventually filling its widgets with the given
        data.
        """
        if data is not None:
            self.ui.widgetSchema.setData(data)
        QtWidgets.QDialog.show(self)

    def retrieveData(self):
        """
        Retrieves the data from the dialog, called when pressing the OK
        button.
        """
        self.data = self.ui.widgetSchema.getData()


class OutputDialog(DataDialog):

    def __init__(self, schema):
        DataDialog.__init__(self, schema, False)


class InputDialog(DataDialog):

    def __init__(self, schema):
        DataDialog.__init__(self, schema, True)


class DataWidget(QtWidgets.QWidget):
    """
    Generic widget to be implemented for each type.
    """

    def __init__(self):
        QtWidgets.QWidget.__init__(self)

    def setEditable(self, editable):
        try:
            self.ui.inputBox.setReadOnly(not editable)
        except AttributeError:
            self.ui.setEnabled(editable)

        return self

    def getData(self):
        raise NotImplementedError

    def setData(self, data):
        raise NotImplementedError


class IntegerWidget(DataWidget):
    DATA_FORMAT = "<i"

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("IntegerWidget.ui", self)

    def getData(self):
        return struct.pack(self.DATA_FORMAT,
                           self.ui.inputBox.value())

    def setData(self, data):
        self.ui.inputBox.setValue(data.unpack(self.DATA_FORMAT)[0])


class RealWidget(DataWidget):
    DATA_FORMAT = "<f"

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("RealWidget.ui", self)

    def getData(self):
        return struct.pack(self.DATA_FORMAT,
                           self.ui.inputBox.value())

    def setData(self, data):
        self.ui.inputBox.setValue(data.unpack(self.DATA_FORMAT)[0])


class CharacterWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("CharacterWidget.ui", self)

    def getData(self):
        return self.ui.inputBox.text().encode("utf-8") or b'\0'

    def setData(self, data):
        self.ui.inputBox.setText(data.unpack("c")[0].decode("utf-8"))


class StringWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("StringWidget.ui", self)

    def getData(self):
        return self.ui.inputBox.toPlainText().encode("utf-8") + b'\0'

    def setData(self, data):
        self.ui.inputBox \
            .setPlainText(DataDialog.decryptBytes(data, b'\0').decode("utf-8"))


class BooleanWidget(DataWidget):

    def __init__(self):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("BooleanWidget.ui", self)

    def setData(self, data):
        if data.unpack("c")[0] == b'1':
            self.ui.inputBox.setChecked(True)
        else:
            self.ui.inputBox.setChecked(False)

    def getData(self):
        return b'1' if self.ui.inputBox.isChecked() else b'0'


class VectorWidget(DataWidget):

    def __init__(self, schema, nesting, editable):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("VectorWidget.ui", self)
        self.size = int(DataDialog.decryptString(schema, ','))

        nesting['v'] += 1
        self.ui.widgets = []
        self.ui.horizontal = (nesting['v'] % 2 == 1)
        for i in range(self.size-1):
            self.ui.widgets.append(
                DataDialog.resolveSchema(deque(schema), nesting, editable))
            self.ui.gridLayout.addWidget(self.ui.widgets[i],
                                         i if self.ui.horizontal else 0,
                                         0 if self.ui.horizontal else i)

        # Awful: last time, no copy
        self.ui.widgets.append(
            DataDialog.resolveSchema(schema, nesting, editable))
        self.ui.gridLayout.addWidget(self.ui.widgets[self.size-1],
                                     self.size-1 if self.ui.horizontal else 0,
                                     0 if self.ui.horizontal else self.size-1)
        self.ui.gridLayout.update()

        schema.popleft()  # Closed square bracket ending the vector's schema

    def setData(self, data):
        for widget in self.ui.widgets:
            widget.setData(data)

    def getData(self):
        result = b""

        for widget in self.ui.widgets:
            result += widget.getData()

        return result


class StructWidget(DataWidget):

    def __init__(self, schema, nesting, editable):
        DataWidget.__init__(self)
        self.ui = uic.loadUi("StructWidget.ui", self)
        self.ui.widgets = []

        nesting['s'] += 1
        character = schema.popleft()
        i = 0
        while character != ")":
            name = character if character != ',' else ""
            if character != ':':
                name += DataDialog.decryptString(schema, ':')
            else:
                name = ""

            widget = DataDialog.resolveSchema(schema, nesting, editable)
            label = QtWidgets.QLabel(name)
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
        result = b""

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
        self.dataDialog = DataDialog(schema, editable=editable)

        self.data = None

    @QtCore.pyqtSlot()
    def showWindow(self):
        # if self.data is None:
        #     self.dataDialog.show()
        # else:
        #     self.dataDialog.show(ByteDeque(self.data))
        self.dataDialog.show(self.data)

        if self.dataDialog.exec_():
            self.setData(ByteDeque(self.dataDialog.data))

    def setData(self, data):
        # A bit of an hack... Copies the data and then consumes the part
        # relative to its content
        self.data = ByteDeque(data)
        self.dataDialog.widgetSchema.setData(data)

    def getData(self):
        while self.data is None:
            # Not the top of the usability, but it works...
            QtWidgets.QMessageBox \
                .warning(self,
                         "Missing data",
                         ("You haven't entered all the data!\n"
                          "A window will now appear to allow you to finish "
                          "entering it."))

            self.showWindow()

        return self.data.data


class SimpleInterface:
    def __init__(self, startApp=True):
        if startApp:
            self.app = QtWidgets.QApplication([])

    def requestInput(self, textualSchema):
        """
        External function to ask for data.
        """
        if DEBUG:
            print("Python schema: {}".format(textualSchema))
        inputDialog = InputDialog(deque(textualSchema))
        inputDialog.show()
        if inputDialog.exec_():
            if DEBUG:
                print("Python data: {}".format(inputDialog.data))
            return (1, inputDialog.data)
        else:
            return (0, None)

    def requestOutput(self, textualSchema, data):
        """
        External function to show data.
        """
        if DEBUG:
            print("Python schema: {}".format(textualSchema))
            print("Python data: {}".format(data))
        outputDialog = OutputDialog(deque(textualSchema))
        outputDialog.show(ByteDeque(data))

        return outputDialog.exec_()
