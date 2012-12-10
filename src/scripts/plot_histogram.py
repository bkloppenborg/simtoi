#!/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re
from scipy.stats import norm, cauchy
import matplotlib.mlab as mlab

def plot_histogram(filename, column_names=[], skip_cols=[], nbins=10, autosave=False, save_basename='', save_format='svg', trimends=False):
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

    # Reinterpret any negative numbers in skip_cols to be at the end of the line
    for column in range(0, len(skip_cols)):
        if skip_cols[column] < 0:
            skip_cols[column] = end_col + skip_cols[column]
       
    namecol = 0 
    for column in range(0, end_col):
        # Skip the column if instructed to do so:
        if(column in skip_cols):
            continue;

        # extract the data column:
        temp = data[:,column]
        
        if(trimends):
            minval = min(temp)
            maxval = max(temp)
            
            temp = filter(lambda x: x > minval, temp)
            temp = filter(lambda x: x < maxval, temp)
        
        # plot a histogram of the data:
        [n, bins, patches] = plt.hist(temp, bins=nbins, normed=True, label='Binned data')
        
        # fit a normal distribution:
        [mu, sigma] = norm.fit(temp)
        y = mlab.normpdf(bins, mu, sigma)
        legend_gauss = r'Normal: $\mu=%.3f,\ \sigma=%.3f$' % (mu, sigma)
        l = plt.plot(bins, y, 'r--', linewidth=2, label=legend_gauss)
        
        # fit a Lorentz/Cauchy distribution:
        # bug workaround for http://projects.scipy.org/scipy/ticket/1530
        # - specify a starting centroid value for the fit
        [mu, gamma] = cauchy.fit(temp, loc=mu)
        y = cauchy.pdf(bins, loc=mu, scale=gamma)
        legend_cauchy = r'Cauchy: $\mu=%.3f,\ \gamma=%.3f$' % (mu, gamma)
        l = plt.plot(bins, y, 'g--', linewidth=2, label=legend_cauchy)
        
        # now setup the axes labels:
        try:
            title = column_names[namecol]
            namecol += 1
        except:
            title = "Title"
        
        plt.title(title)
        plt.xlabel("Value")
        plt.ylabel("Frequency")
        plt.legend(loc='best')
        
        if autosave:
            plt.savefig(save_basename + 'hist_' + title + '.' + save_format, transparent=True, format=save_format)    
            plt.close()        
        else:
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
    parser.add_option("--autosave", dest="autosave", action="store_true", default=False,
        help="Automatically save the plots. [default: False]")
    parser.add_option("--savefmt", dest="savefmt", action="store", type="string", default="svg",
        help="Automatic save file format.  [default: %default]")
    parser.add_option("--trimends", dest="trimends", action="store_true", default=False,
        help="Remove the minimum and maximum bins from the histogram [default: %default]")

    (options, args) = parser.parse_args()

    # now read the filenames
    filename = args[0]
    
    # Set parameters specifying columns that should not be plotted
    # and attempt to find the namefile:
    skip_cols=[]
    basename = ''
    if re.search('_bootstrap', filename):
        tmp = re.split('_bootstrap', filename)
        basename = tmp[0]
        skip_cols = [-1]
        print "Found bootstrap file."
    elif re.search('_multinest', filename):
        tmp = re.split('_multinest', filename)
        basename = tmp[0]
        skip_cols = [0,1]
        print "Found MultiNest file."
    else:
        print "Unknown file format found, I'll do the best I can."


    column_names = []
    if len(basename) > 1:
        column_names = col_names(basename + '_param_names.txt')
        print column_names

    plot_histogram(filename, column_names=column_names, skip_cols=skip_cols, nbins=options.nbins, autosave=options.autosave, save_basename=basename, save_format=options.savefmt, trimends=options.trimends)
    
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
