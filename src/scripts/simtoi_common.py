"""File containing several common functions for SIMTOI's python routines
"""
import re
from astropy.io import ascii
import numpy as np

def simtoi_best_fit(filename):
    """Extracts the best-fit values from best_fit.txt and returns these
    values in a astropy.table with columns named 'col1, ... , colN'
    """
    
    infile = ascii.read(filename, header_start=None, delimiter=',')
    
    return infile[0]

def simtoi_col_names(filename):
    """Parses the best_fit.txt file written by SIMTOI and returns
    the column names as a list.
    """
    try:
        # try to read in the file with astropy.ascii.read
        infile = ascii.read(filename, header_start=None, delimiter=',', data_start=0)
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
            
        infile = ascii.read(values, header_start=None, delimiter=',', data_start=0)
    
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
    
def read_ccoifits_v2(filename, uv_scale=None):
    """Reads in the squared visibility output format for ccoifits.
    """
    
    data = ascii.read(filename)
    
    data.rename_column('col1', 'u')
    data.rename_column('col2', 'v')
    data.rename_column('col3', 'uv_mag')
    data.rename_column('col4', 'v2')
    data.rename_column('col5', 'v2_err')
    
    # rescale the UV points           
    if uv_scale:    
        data['u'] /= uv_scale
        data['v'] /= uv_scale
        data['uv_mag'] /= uv_scale
    
    return data
    
def read_ccoifits_t3(filename, uv_scale=None):
    
    data = ascii.read(filename)
    
    data.rename_column('col1', 'u1')
    data.rename_column('col2', 'v1')
    data.rename_column('col3', 'u2')
    data.rename_column('col4', 'v2')
    data.rename_column('col5', 'u3')
    data.rename_column('col6', 'v3')
    data.rename_column('col7', 'uv_mag')   
    data.rename_column('col8', 't3_amp')
    data.rename_column('col9', 't3_amp_err')
    data.rename_column('col10', 't3_phi')
    data.rename_column('col11', 't3_phi_err')
      
    # rescale the UV points           
    if uv_scale:    
        data['u1'] /= uv_scale
        data['v1'] /= uv_scale
        data['u2'] /= uv_scale
        data['v2'] /= uv_scale
        data['u3'] /= uv_scale
        data['v3'] /= uv_scale
        data['uv_mag'] /= uv_scale
                    
    return data


def read_simtoi_photometry(filename):
    """Reads a simulated photometry file from SIMTOI
    """
    try:
        data = ascii.read(filename)
    except:
        print("Could not parse photometric data file:\n " + filename)
        
        quit()
    
    # at present SIMTOI columns are not named, give them names:
    data.rename_column('col1', 'date')
    data.rename_column('col2', 'mag')
    
    # data files should have error columns (model files may not)
    if len(data.colnames) > 2:
        data.rename_column('col3', 'mag_err')
    
    return data
