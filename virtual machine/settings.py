#!/usr/bin/env python3

import os

DEBUG = False

if __file__ is None:
    LOCAL_DIR = os.getcwd()
else:
    LOCAL_DIR = os.path.dirname(os.path.realpath(__file__))
