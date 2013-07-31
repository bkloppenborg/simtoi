from astropy.io import ascii
import matplotlib.pyplot as plt
from optparse import OptionParser
import os
from simtoi_common import mirrored_stdev, simtoi_col_names, find_index, simtoi_best_fit

def plot_multinest(filename, best_fit_vals,
    column_names=None, skip_cols=[], nbins=25, trimends=False,
    autosave=False, save_directory='', save_format='svg',
    data_start=0):
    
    # Create a custom reader with no header and CSV values
    data = ascii.read(filename, header_start=None)
    
    print "Param, best_fit, lower_bound, upper_bound"

    for col_num in range(3, len(data.colnames) + 1):
        col_id = 'col' + str(col_num - 2)
        multinest_col = 'col' + str(col_num)
        col = data[multinest_col]
        
        title = ""
        if column_names != None:
            title = column_names[col_id]
          
        # the first record contains the best-fit value
        best_fit = best_fit_vals[col_id]
        
        # Determine the lower and upper error bars by mirrored
        # standard deviations
        [l_sigma, r_sigma] = mirrored_stdev(col, best_fit)
        
        [n, bins, patches] = plt.hist(col, bins=nbins, normed=False, 
            label='Binned data', histtype='step')
            
        plt.ylim([0, max(n) + 5])
        
        plt.title(title)
        plt.xlabel("Value")
        plt.ylabel("Frequency")
        
        # Create a label for the best-fit value:
        value_label = r'$' + "{0:.3f}".format(best_fit) + \
            '^{+' + "{0:.3f}".format(r_sigma) + '}' + \
            "_{-" + "{0:.3f}".format(l_sigma) + '}$'
        
        plt.axvline(best_fit, color='red', label=value_label)
        plt.axvspan(best_fit - l_sigma, best_fit + r_sigma, color='red', 
            alpha=0.2)
        
        plt.legend()
        
        if autosave:
            plt.savefig(save_directory + '/stats_hist_' + title + '.' + 
                save_format, transparent=True, format=save_format)    
            plt.close()
        else:
            plt.show()
            
        # Write a little information out to the console.
        print title, best_fit, best_fit - l_sigma, best_fit + r_sigma


def main():
 
    filename = ""  

    usage = "Usage: %prog [options] bootstrap_file"
    parser = OptionParser(usage=usage)
    parser.add_option("--nbins", dest="nbins", action="store", type="int", default=25,
        help="Number of binning columns. [default: %default]")
    parser.add_option("--autosave", dest="autosave", action="store_true", default=False,
        help="Automatically save the plots. [default: False]")
    parser.add_option("--savefmt", dest="savefmt", action="store", type="string", default="svg",
        help="Automatic save file format.  [default: %default]")
    parser.add_option("--trimends", dest="trimends", action="store_true", default=False,
        help="Remove the minimum and maximum bins from the histogram [default: %default]")

    (options, args) = parser.parse_args()

    # Ensure the filename is specified. If not, print the help and quit
    try:
        filename = args[0]    
    except:
        print "Error: bootstrap file not specified.\n"
        parser.print_help()
        quit()
        
    directory = os.path.dirname(os.path.realpath(filename))
    delimiter = ','

    column_names = []
    if len(directory) > 1:
        column_names = simtoi_col_names(directory + '/best_fit.txt')
        
    best_fit = simtoi_best_fit(directory + '/best_fit.txt')
    
    plot_multinest(filename, best_fit, column_names=column_names, nbins=options.nbins,
        autosave=options.autosave, save_directory=directory, 
        save_format=options.savefmt, trimends=options.trimends)
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
    
    
