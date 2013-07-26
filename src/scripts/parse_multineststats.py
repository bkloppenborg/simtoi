#!/bin/python
import sys
from optparse import OptionParser
from astropy.io import ascii


def get_best_fit(filename):
    """
    Extracts the best-fit parameters from a MultiNest stats file
    """
    
    best_logZ = -sys.float_info.max
    best_params = list()
    
    infile = ascii.read(filename, Reader=ascii.FixedWidthNoHeader, delimiter=' ')
    
    import code
    code.interact(local=locals())
    
    # In MultiNest 3.0 the Importance Sampling uncertainty can be written
    # to the second line in the file, rather than just appended. Detect
    # this bug and fix it.
    if infile[1][1] == '':
        # append a new column
        infile.add_column(
        # copy the error into 
        infile[0][len(infile[0]) = infile[1][0]
    
    
    
    
    
    for line in infile:
        line = line.strip()
        line = line.split()
        
        logZ = line[len(line) - 1]
        
        if(logZ > best_logZ):
            best_logZ = logZ
            best_params = line
            
    return best_params
    
    
def main():

    usage = "Usage: %prog [options] directory [... directory]"
    parser = OptionParser(usage=usage)
    parser.add_option("--name", dest="name", action="store", type="string",
        default="multinestsummary.txt",
        help="Name of the MultiNest Summary file. [default: %default]")

    (options, args) = parser.parse_args()
    
    directories = args[0:]
    
    if len(directories) < 1:
        print "Error: At least one directory must be specified"
        exit()
    
    # For each directory print out a string that lists the best-fit parameters
    # directory, param, ..., param, logZ
    for directory in directories:
        temp = get_best_fit(directory + '/' + options.name)
        temp.insert(0, directory)
        
        temp = map(str, temp)
        temp = ','.join(temp)
        print temp
            
            
            
if __name__ == "__main__":
    main()
