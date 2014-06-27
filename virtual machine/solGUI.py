#!/usr/bin/env python3

import sys
import os
from subprocess import Popen, PIPE

from PyQt5 import QtCore, QtWidgets, uic

from interface import SimpleInterface


class MainWindow(QtWidgets.QMainWindow):
    EXT_SOURCE = ".sol"
    EXT_SCODE = ".ohana"
    EXE_COMPILER = "./solc"
    EXE_VM = "./machine"  # Only for debug, the real one should be "./solvm"

    STR_START = "No file selected: open a file to start."
    STR_COMPILABLE = "The file must be compiled."
    STR_EXECUTABLE = "The file can be executed."

    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        self.ui = uic.loadUi("MainWindow.ui", self)

        self.ui.actionOpen.triggered.connect(self.open)
        self.ui.actionCompile.triggered.connect(self.compile)
        self.ui.actionRun.triggered.connect(self.run)

        # Debug
        self.debugInterface = SimpleInterface(False)
        self.ui.inputButton.clicked \
            .connect(lambda: self.debugInterface.requestInput(str(self.ui.debugText.text())))
        self.ui.outputButton.clicked \
            .connect(lambda: self.debugInterface.requestOutput(str(self.ui.debugText.text()),
                                                               str(self.ui.debugData.text())))

    @QtCore.pyqtSlot()
    def open(self):
        """
        Opens the "Open file" window asking either for a source or a compiled
        file
        """

        fileName, ok = QtWidgets.QFileDialog.getOpenFileName(
            self,
            "Open file",
            filter="(SOL) *{};; (S-CODE) *{}".format(self.EXT_SOURCE, self.EXT_SCODE))

        if ok:
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
        self.ui.outputText.append(output.decode("utf-8"))

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

        self.ui.outputText.append(output.decode("utf-8"))


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    MainWindow().show()
    sys.exit(app.exec_())
