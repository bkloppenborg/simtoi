#!/usr/bin/python

"""
Reads in the MultiNest output file specified by the first parameter, and
generates the sample probability for each parameter column, broken into
n-bins.

Parameter probabilities are written out to a comma separated text file
in the following format:

 param_value,bin_width,normalized_probability,...

where this format repeats for each parameter.  Indexing is thus:
 param_val = 3*param_num
 bin_width = 3*param_num + 1
 norm_prob = 3*param_num + 2

when plotting.  The program PlotHistogram.py will generate publishable
quality figures using the gnuPlot plotting program.

Usage information: call MultiNestToHistogram.py -h
"""

import numpy
from optparse import OptionParser

def ReadMultinestOutput(filename):
    
    infile = open(filename, 'r')
    temp = numpy.loadtxt(infile, dtype=float)
    infile.close()
    
    return temp


def Multinest_PosteriorProbDist(data, n_bins):
    """
    Computes the sum of the sample probability (column 1) for the parameter values (column 3+)
    broken into n_bins on each parameter column.
    """
    
    n_data = len(data)
    n_params = len(data[0]) - 2

    output = numpy.zeros((n_bins+1, 3*n_params))
    
    for i in range(0, n_params):
        #print i

        
        # First create version of the array sorted by the current parameter column:
        temp = data[data[:,2+i].argsort()]
        param_min = temp[0,2+i]
        param_max = temp[n_data-1, 2+i]
        step = (param_max - param_min)/n_bins
        
        # Now we'll iterate through the array and total things up:
        k = 0
        total = 0
        lb = param_min
        ub = param_min + step
        for j in range(0, n_data):
            value = temp[j,2+i]
            samp_prob = temp[j,0]
            
            if(value < ub):
                total += samp_prob
            else:
                output[k,3*i] = ub - step/2 # center position
                output[k,3*i+1] = step  # width
                output[k,3*i+2] = total # count
                
                # init things for the next iteration
                total = samp_prob
                ub += step
                k += 1
            
    return output
    
def WriteHistogram(filename, hist_data):
    """
    Writes out histogram data for plotting using gnuplot.
    
    Data written to the file has the form [center, width, number], [center, width, number], ...
    """
    outfile = open(filename, 'w')
    
    for i in range(0, len(hist_data)):
        # First convert to strings:
        output = map(str, hist_data[i])
        # Replace zeros by empty strings
        output = ['' if x == "0.0" else x for x in output]
        
        outfile.write(','.join(output) + "\n")


    outfile.close()

def main():

    n_bins = 500  
    filename = ""  

    usage = "Usage: %prog [options] filename"
    parser = OptionParser(usage=usage)
    parser.add_option("-n", dest="n_bins", action="store", type="float",
                      help="Number of Bins for the Histogram")

    (options, args) = parser.parse_args()
    
    if(options.n_bins):
        n_bins = options.n_bins
        
    print "Using " + str(n_bins)

    # now read the filenames
    filename = args[0]
    base_filename = filename[0:len(filename)-4]

    data = ReadMultinestOutput(filename)
    hist = Multinest_PosteriorProbDist(data, n_bins)
    WriteHistogram(base_filename + ".hist", hist)
    
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
