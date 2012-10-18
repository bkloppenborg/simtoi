#!/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re
from scipy.stats import norm
import matplotlib.mlab as mlab

def plot_histogram(filename, column_names=[], skip_cols=[], nbins=10):
    """
    Plots a histogram formed from the columns of the specified file.

    If column_names is specified, the titles of the plots will be renamed
    accordingly.  Otherwise "Title" is inserted instead.

    skip_cols specifies any columns in the data that should be skipped.
    Columns at the end of the line may be skipped by using negative numbers.
    In this scheme the last column in a row is -1.
    """
    infile = open(filename, 'r')
    data = loadtxt(infile, dtype=float)
    infile.close()

    end_col = data.shape[1]

    # Reinterpret any negative numbers in skip_cols to be at the end
    # of the line
    for column in range(0, len(skip_cols)):
        if skip_cols[column] < 0:
            skip_cols[column] = end_col + skip_cols[column]
        
    for column in range(0, end_col):
        # Skip the column if instructed to do so:
        if(column in skip_cols):
            continue;

        # extract the data column:
        temp = data[:,column]
        
        try:
            title = column_names[column]
        except:
            title = "Title"

        [n, bins, patches] = plt.hist(temp, bins=nbins, normed=1)
        [mu, sigma] = norm.fit(temp)
        y = mlab.normpdf(bins, mu, sigma)
        l = plt.plot(bins, y, 'r--', linewidth=2)
        
        # now setup the axes labels:
        plt.title(r'$\mathrm{%s:}\ \mu=%.3f,\ \sigma=%.3f$' %(title, mu, sigma))
        plt.xlabel("Value")
        plt.ylabel("Frequency")
        plt.show()

def col_names(filename):
    infile = open(filename, 'r')
    
    columns = list()
    
    for line in infile:
        if re.match('#', line):
            continue
        
        line = line.strip()
        columns = line.split(' ')
        break
        
    return columns

def main():
 
    filename = ""  

    usage = "Usage: %prog [options] filename"
    parser = OptionParser(usage=usage)
    parser.add_option("--nbins", dest="nbins", action="store", type="int", default=10,
              help="Number of binning columns. [default: 10]")

    (options, args) = parser.parse_args()

    # now read the filenames
    filename = args[0]
    base_filename = filename[0:len(filename)-4]
    
    offset_start = 0
    offset_end = 0
    
    # Set parameters specifying columns that should not be plotted
    # and attempt to find the namefile:
    skip_cols=[]
    namefile = ''
    if re.search('bootstrap', filename):
        tmp = re.split('bootstrap', filename)
        namefile = tmp[0] + 'param_names.txt'
        skip_cols = [-1]
        print "Found bootstrap file."
    elif re.search('multinest', filename):
        tmp = re.split('multinest', filename)
        namefile = tmp[0] + 'param_names.txt'
        skip_cols = [0,1]
        print "MultiNest file."


    column_names = []
    if len(namefile) > 1:
        column_names = col_names(namefile)

    plot_histogram(filename, column_names=column_names, skip_cols=skip_cols, nbins=options.nbins)
    
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
