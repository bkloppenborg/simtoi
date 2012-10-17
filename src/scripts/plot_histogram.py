#!/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re

def plot_histogram(filename, column_names=[], filetype='bootstrap', col_offsets=[0,0]):

    infile = open(filename, 'r')
    data = loadtxt(infile, dtype=float)
    infile.close()

    start_col = col_offsets[0]
    end_col = data.shape[1] - col_offsets[1]
        
    for column in range(start_col, end_col):
        temp = data[:,column]
        
        try:
            title = column_names[column - start_col]
        except:
            title = "Histogram title"

        plt.hist(temp)
        plt.title(title)
        plt.xlabel("Value")
        plt.ylabel("Frequency")
        plt.show()


def main():
 
    filename = ""  

    usage = "Usage: %prog [options] filename"
    parser = OptionParser(usage=usage)
#    parser.add_option("-type", dest="filetype", action="store", type="string", default='bootstrap',
#              help="Type of data file being read. Options: [bootstrap,multinest]  [default: ' default']")

    (options, args) = parser.parse_args()

    # now read the filenames
    filename = args[0]
    base_filename = filename[0:len(filename)-4]
    
    offset_start = 0
    offset_end = 0
    
    if re.search('bootstrap', filename):
        offset_start = 0
        offset_end = 1
        print "Found bootstrap file."
    elif re.search('multinest', filename):
        offset_start = 2
        offset_end = 0
        print "MultiNest file."

    plot_histogram(filename, filetype='bootstrap', col_offsets=[offset_start, offset_end])
    
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
