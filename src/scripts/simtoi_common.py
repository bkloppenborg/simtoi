"""File containing several common functions for SIMTOI's python routines
"""
import re
from astropy.io import ascii

def simtoi_col_names(filename):
    """Parses the best_fit.txt file written by SIMTOI and returns
    the column names as a list.
    """
    try:
        # try to read in the file with astropy.ascii.read
        infile = ascii.read(filename, header_start=None, delimiter=',')
    except:
        # it will fail when the file contains no delimiter (obviously)
        # so just read in the lines, add a delimiter to each
        # and have astropy.io.ascii re-read the lines
        infile = open(filename)
        values = list()
        for line in infile:
            if re.match('#', line):
                continue
                
            line = line.strip()    
            values.append(line + ",")
            
        infile = ascii.read(values, header_start=None, delimiter=',')
    
    return infile[0]
    
def find_index(array, value):
    """Finds the index of value within a sorted array. If there are
    repeat instances of value, the midpoint will be returned.
    """

    # Find where value starts and ends in the array
    start = 0
    end = len(array) 
    for i in range(0, end):
        if array[i] < value:
            start = i
    
        if array[i] > value:
            end = i
            break
     
    # return the midpoint  
    return int((start + end) / 2)

def mirrored_stdev(array, mirror_point):
    """Calculates the lower and upper standard deviations
    of an array by mirroing values about mirror_point.
    
    Returns:
    """
    
    # sort the array:
    array = np.sort(array)
    
    midpoint = find_index(array, mirror_point)
    
    # Create mirrored arrays
    # Select x < midpoint
    lower = array[0:midpoint] - array[midpoint]
    temp = np.concatenate([lower, [0], abs(lower[::-1])])
    sigma_lb = np.std(temp)
    
    #plt.hist(temp, label='lower')
    #plt.show()
    
    # now do values with x > midpoint
    upper = array[midpoint + 1:len(array)] - array[midpoint]
    temp = np.concatenate([-1*upper[::-1], [0], upper])
    sigma_ub = np.std(temp)
    
    #plt.hist(temp, label='upper')
    #plt.show()
    
    return [sigma_lb, sigma_ub]
