#!/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re
from scipy.stats import norm, cauchy
import matplotlib.mlab as mlab
import os

def plot_histogram(filename, 
    column_names=[], skip_cols=[], nbins=10, trimends=False,
    autosave=False, save_directory='', save_format='svg', delimiter=None):
    """
    Plots a histogram formed from the columns of the specified file.

    If column_names is specified, the titles of the plots will be renamed
    accordingly.  Otherwise "Title" is inserted instead.

    skip_cols specifies any columns in the data that should be skipped.
    Columns at the end of the line may be skipped by using negative numbers.
    In this scheme the last column in a row is -1.
    """
    infile = open(filename, 'r')
    if(delimiter):
        data = loadtxt(infile, dtype=float, delimiter=',')
    else:
        data = loadtxt(infile, dtype=float)       
    infile.close()

    end_col = data.shape[1]
    
    norm_stats = list()
    cauchy_stats = list()

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
        [norm_mu, norm_sigma] = norm.fit(temp)
        y = mlab.normpdf(bins, norm_mu, norm_sigma)
        legend_gauss = r'Normal: $\mu=%.3f,\ \sigma=%.3f$' % (norm_mu, norm_sigma)
        l = plt.plot(bins, y, 'r--', linewidth=2, label=legend_gauss)
        
        # fit a Lorentz/Cauchy distribution:
        # bug workaround for http://projects.scipy.org/scipy/ticket/1530
        # - specify a starting centroid value for the fit
        [cauchy_mu, cauchy_gamma] = cauchy.fit(temp, loc=norm_mu)
        y = cauchy.pdf(bins, loc=cauchy_mu, scale=cauchy_gamma)
        legend_cauchy = r'Cauchy: $\mu=%.3f,\ \gamma=%.3f$' % (cauchy_mu, cauchy_gamma)
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
            plt.savefig(save_directory + '/stats_hist_' + title + '.' + save_format, transparent=True, format=save_format)    
            plt.close()
        else:
            plt.show()
            
        # Add in the statistical information.
        norm_stats.append([title, norm_mu, norm_sigma])
        cauchy_stats.append([title, cauchy_mu, cauchy_gamma])


    # Now either print out or save the statistical information
    if(not autosave):
        print "Normal Statistics:"
        
    write_statistics(save_directory + '/stats_normal.txt', norm_stats, autosave)
    
    if(not autosave):
        print "Cauchy Statistics:"
        
    write_statistics(save_directory + '/stats_cauchy.txt', cauchy_stats, autosave)


def col_names(filename):
    """
    Reads in the column names from the `best_fit.txt` files.
    Column names will always occur on the first non-comment line in the file.
    """
    infile = open(filename, 'r')
    
    columns = list()
    
    for line in infile:
        if re.match('#', line):
            continue
        
        line = line.strip()
        columns = line.split(',')
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
    directory = os.path.dirname(os.path.realpath(filename))
    delimiter = None
    if re.search('bootstrap_levmar', filename):
        skip_cols = [-1]
        delimiter = ','
        print "Found bootstrap file."
    elif re.search('multinest.txt', filename):
        skip_cols = [0,1]
        print "Found MultiNest file."
    else:
        print "Unknown file format found, I'll do the best I can."

    column_names = []
    if len(directory) > 1:
        column_names = col_names(directory + '/best_fit.txt')

    plot_histogram(filename, column_names=column_names, skip_cols=skip_cols, nbins=options.nbins, autosave=options.autosave, save_directory=directory, save_format=options.savefmt, trimends=options.trimends, delimiter=delimiter)
   
   
def write_statistics(filename, statistics, save_to_file):
    """
    Writes the statistical information as rows formatted as:
        title, mu, sigma
    to the specified file.
    
    statistics should be a list of triplets:
        [[title, mu, sigma], ..., [title, mu, sigma] ]
        
    Data will be written as follows:
        # col1, sig_col1, ..., colN, sig_colN
        val1, sig_val1, ..., valN, sig_valN
        
    """ 
    # first do the title line
    titles = list()
    values = list()
    for [title, value, sigma] in statistics:
        titles.append(title)
        titles.append("sig_" + title)
        values.append(value)
        values.append(sigma)
        
    titles = map(str, titles)
    values = map(str, values)
    
    title_line = "# " + ', '.join(titles)
    value_line = ', '.join(values)
    
    if(save_to_file):
        outfile = open(filename, 'w')
        outfile.write(title_line + "\n")
        outfile.write(value_line)
        outfile.close()
    else:
        print title_line
        print value_line
    
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
