#!/usr/bin/env python

from pandas import *
import pandas
import numpy as np
import matplotlib.pyplot as plt

# configuration
BIN_WIDTH = 4		# nb of bins in histogram, e.g. precision
X_MAX = 260			# maximum X axis value
Y_MAX = 105			# maximum Y axis value
F_SIZE = (12, 12)	# complete figure size

DATA_PATH = './'		# location of csv files
DATA_FILES = ['cameraValuesGray.csv',
		'cameraValuesRed.csv',
		'cameraValuesGreen.csv',
		'cameraValuesBlue.csv']
DATA_NAMES = ['gray','red','green','blue']

OUTPUT_FILE = 'histogram.png' # name of the output file

def getdata(path):
	gray  = read_csv(path+DATA_FILES[0],index_col=0).transpose()
	red   = read_csv(path+DATA_FILES[1], index_col=0).transpose()
	green = read_csv(path+DATA_FILES[2], index_col=0).transpose()
	blue  = read_csv(path+DATA_FILES[3], index_col=0).transpose()

	# drop the last column
	#calib.drop(calib.columns[8], axis=1, inplace = True)

	meanGR = gray.mean(axis=1)
	meanR = red.mean(axis=1)
	meanG = green.mean(axis=1)
	meanB = blue.mean(axis=1)

	values = DataFrame({'GR':meanGR,'R':meanR,'G':meanG,'B':meanB},columns = ['GR','R','G','B'])

	return values



def plotvalues(values,namefile):
	fig, axes = plt.subplots(nrows=4, ncols=1,figsize=F_SIZE)

	values['GR'].hist(ax = axes[0],bins=nbins,color='k')
	values['R'].hist(ax = axes[1],bins=nbins,color='r')
	values['G'].hist(ax = axes[2],bins=nbins,color='g')
	values['B'].hist(ax = axes[3],bins=nbins,color='b')

	for i in range(len(DATA_NAMES)):
		axes[i].set_ylabel(DATA_NAMES[i], fontsize=18)
		axes[i].set_xlim(0,X_MAX)
		axes[i].set_ylim(0,Y_MAX)

	axes[3].set_xlabel("color values", fontsize=18)
	plt.show()
	fig.savefig(namefile)


nbins = np.arange(0, 255 + BIN_WIDTH, BIN_WIDTH)
robotValues = getdata(DATA_PATH)
plotvalues(robotValues, OUTPUT_FILE)



