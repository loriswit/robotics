#********************************************************************************************
# Calibration Plot Demo
# 
# Description: Plot data from CSV file.
# Author: Beat Hirsbrunner, Julien Nembrini, Simon Studer (University of Fribourg)
# Version: 1.0 (2016-03-03)
#********************************************************************************************
from pandas import *
import pandas
import numpy as np
import matplotlib.pyplot as plt

# config
FILENAME='calib.csv'
COLUMNS=2

# get data from CSV file
calib = read_csv(FILENAME, index_col=0)
calib.drop(calib.columns[COLUMNS], axis=1, inplace = True)

# plot data
calib.plot(figsize=(12,9))
plt.legend(loc='upper right')
plt.show()
