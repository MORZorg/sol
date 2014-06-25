#!/usr/bin/env python

import sys
import os
from subprocess import Popen, PIPE

from PyQt4 import QtCore, QtGui, uic

from interface import requestInput, requestOutput


class MainWindow(QtGui.QMainWindow):
    EXT_SOURCE = ".sol"
    EXT_SCODE = ".ohana"
    EXE_COMPILER = "./solc"
    EXE_VM = "./solvm"

    STR_START = "No file selected: open a file to start."
    STR_COMPILABLE = "The file must be compiled."
    STR_EXECUTABLE = "The file can be executed."

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.ui = uic.loadUi("MainWindow.ui", self)

        self.ui.actionOpen.triggered.connect(self.open)
        self.ui.actionCompile.triggered.connect(self.compile)
        self.ui.actionRun.triggered.connect(self.run)

        # Debug
        self.ui.inputButton.clicked \
            .connect(lambda: requestInput(str(self.ui.debugText.text())))
        self.ui.outputButton.clicked \
            .connect(lambda: requestOutput(str(self.ui.debugText.text()),
                                           str(self.ui.debugData.text())))

    @QtCore.pyqtSlot()
    def open(self):
        """
        Opens the "Open file" window asking either for a source or a compiled
        file
        """

        fileName = QtGui.QFileDialog.getOpenFileName(
            self,
            "Open file",
            filter="*{}; *{}".format(self.EXT_SOURCE, self.EXT_SCODE))

        if fileName != "":
            self.ui.outputText.setText(fileName)
            self.fileName, fileExtension = os.path.splitext(str(fileName))

            if fileExtension == self.EXT_SOURCE:
                self.ui.outputText.append(self.STR_COMPILABLE)

                self.ui.actionCompile.setEnabled(True)
                self.ui.actionRun.setEnabled(False)
            else:
                self.ui.outputText.append(self.STR_EXECUTABLE)
                self.ui.actionCompile.setEnabled(False)
                self.ui.actionRun.setEnabled(True)
        else:
            self.ui.outputText.setText(self.STR_START)

            self.ui.actionCompile.setEnabled(False)
            self.ui.actionRun.setEnabled(False)

    @QtCore.pyqtSlot()
    def compile(self):
        """
        Calls the compiler on the loaded file (supposing it has the usual
        extension)
        """
        proc = Popen([self.EXE_COMPILER, "{}{}".format(self.fileName,
                                                       self.EXT_SOURCE)],
                     stdout=PIPE)
        output, _ = proc.communicate()
        self.ui.outputText.append(output)

        if proc.returncode == 0:
            self.ui.outputText.append(self.STR_EXECUTABLE)

            self.ui.actionRun.setEnabled(True)

    @QtCore.pyqtSlot()
    def run(self):
        """
        Calls the virtual machine on the loaded file (supposing it has the
        usual extension)
        """
        proc = Popen([self.EXE_VM, "{}{}".format(self.fileName,
                                                 self.EXT_SCODE)],
                     stdout=PIPE)
        # TODO Iterative output (or no output in the vm?)
        output, _ = proc.communicate()

        self.ui.outputText.append(output)


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    MainWindow().show()
    sys.exit(app.exec_())
