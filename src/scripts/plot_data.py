!#/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re

# Photometric plots:
#  plot datafile columns 1:2:3 with yerrorbars title "data"
#  plot datafile_model columns 1:2 title "model" (maybe with lines?)
def plot_photometry(data_file, model_file):
    
    infile = open(data_file, 'r')
    data = loadtxt(infile, dtype=float, delimiter=',')
    infile.close()
    
    data_x = data


# T3 data plot:
#  T3amp: plot datafile columns 7:8:9 with yerrorbars
#  T3phi: plot datafile columns 7:10:11 with yerrorbars

    
def main():
    """
    Main program loop. Looks through the contents
    """
    
    directory = ""
    
    usage = "Usage: %prog [options] directory"
    parser = OptionParser(usage=usage)
    parser.add_option("--autosave", dest="autosave", action="store_true", default=False,
        help="Automatically save the plots. [default: False]")
    parser.add_option("--savefmt", dest="savefmt", action="store", type="string", default="svg",
        help="Automatic save file format.  [default: %default]")
        
    (options, args) = parser.parse_args()
    
    directory = args[0]
    filenames = os.listdir(directory)
    
    for filename in filenames:
        if(re.search('_model.phot'))
            model_file = filename
            datafile = re.replace('_model', '', filename)
            plot_photometry(datafile, modelfile)
        
        elif(re.search('_model_v2.txt'))
            model_file = filename
            datafile = re.replace('_model_v2.txt', '', filename)
            plot_v2(datafile, modelfile)
        
        elif(re.search('_model_t3.txt'))
            model_file = filename
            datafile = re.replace('_model_t3.txt', '', filename)
            plot_t3(datafile, modelfile)        
        

if __name__ == "__main__":
    main()
