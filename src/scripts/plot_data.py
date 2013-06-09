#!/usr/bin/python

from numpy import loadtxt
from optparse import OptionParser
import matplotlib.pyplot as plt
import re
import os

# Photometric plots:
def plot_photometry(data_file, model_file, 
    overplot_file=None, autosave=False, saveformat='svg'):
    """
    Generates a two-frame plot of photometric data from SIMTOI output files:
    Top plot: The photometric data with either the model or overplot file
    Bottom plot: The residuals between the photometric and model data.
    """
    # Load the observed data
    infile = open(data_file, 'r')
    obs = loadtxt(infile, dtype=float, delimiter=',', usecols=(0,1,2))
    infile.close()
    
    obs_x = obs[:,0]
    obs_y = obs[:,1]
    obs_yerr = obs[:,2]
    
    # Load the model data
    infile = open(model_file, 'r')
    mod = loadtxt(infile, dtype=float, delimiter=',', usecols=(0,1))
    infile.close()   
    
    mod_x = mod[:,0]
    mod_y = mod[:,1]
    
    # Load the overplot file (if it exists).
    try:
        infile = open(overplot_file, 'r')
        over = loadtxt(infile, dtype=float, delimiter=',', usecols=(0,1))
        infile.close()   
        
        over_x = over[:,0]
        over_y = over[:,1]
    except:
        overplot_file = None

    # Create a split plot
    f, (top, bottom) = plt.subplots(2,1, sharex='col', sharey=False)
    
    # Top plot: Photometry and model:    
    top.errorbar(obs_x, obs_y, yerr=obs_yerr, fmt='o')
    
    if(overplot_file):
        top.plot(over_x, over_y)
    else:
        top.plot(mod_x, mod_y)
    
    top.invert_yaxis()
    top.grid(True)
    top.set_ylabel("Magnitude")
    
    # Bottom plot: Photometry - model residuals:
    bottom.errorbar(obs_x, obs_y - mod_y, yerr=obs_yerr, fmt='o')
    bottom.invert_yaxis()
    bottom.grid(True)
    bottom.set_ylabel("Residual\n($\Delta$ Mag)")
        
    # Set the plot layout and save the output
    plt.tight_layout(h_pad = 0)
    if(autosave):
        savefile = re.sub('\.*', '\.'+ saveformat)
        plt.savefig(savefile, transparent=True, format=save_format)    
        plt.close()
    else:
        plt.show()


def plot_t3_amp(data_file, model_file, 
    autosave=False, saveformat='svg'):
    """
    Plots T3 amplitudes in a split data/model and residual plot
    """
    split_plot(data_file, 6, 7, 8, model_file, 6, 7, 
    "T$_3$ Amplitude", "Error  ($\sigma T$_3$ Amp$)", 
    autosave=autosave, saveformat=saveformat)
    
def plot_t3_phi(data_file, model_file, 
    autosave=False, saveformat='svg'):
    """
    Plots T3 phases in a split data/model and residual plot
    """
    split_plot(data_file, 6, 9, 10, model_file, 6, 9, 
    "T3$_\phi$\n(Degrees)", "Error ($\sigma T3$_\phi$)",
    ylimits=[-180,180], autosave=autosave, saveformat=saveformat)    

def plot_v2(data_file, model_file, 
    autosave=False, saveformat='svg'):
    """
    Plots V2 in a split data/model and residual plot
    """
    split_plot(data_file, 2, 3, 4, model_file, 2, 3, "$V^2$", "Error ($\sigma {V^2}$)",
    autosave=autosave, saveformat=saveformat)

def split_plot(data_file, data_xcol, data_ycol, data_errcol, 
    model_file, model_xcol, model_ycol,
    primary_plot_ylabel,
    residual_plot_ylabel,
    overplot_file=None, over_xcol=None, over_ycol=None,
    ylimits=None,
    xlimits=None,
    autosave=False, saveformat="svg"):

    # Load the observed data
    infile = open(data_file, 'r')
    obs = loadtxt(infile, dtype=float, delimiter=',', usecols=(data_xcol, data_ycol, data_errcol))
    infile.close()
    
    obs_x = obs[:,0]
    obs_y = obs[:,1]
    obs_yerr = obs[:,2]
    
    # Load the model data
    infile = open(model_file, 'r')
    mod = loadtxt(infile, dtype=float, delimiter=',', usecols=(model_xcol, model_ycol))
    infile.close()   
    
    mod_x = mod[:,0]
    mod_y = mod[:,1]
    
    # Try loading an overplot file:
    try:
        infile = open(overplot_file, 'r')
        over = loadtxt(infile, dtype=float, delimiter=',', usecols=(0,1))
        infile.close()   
        
        over_x = over[:,0]
        over_y = over[:,1]
    except:
        overplot_file = None

    # Create a split plot
    f, (top, bottom) = plt.subplots(2,1, sharex='col', sharey=False)
    
    if(ylimits != None):
        top.set_autoscaley_on(False)
        top.set_ylim(ylimits)
        #bottom.set_autoscaley_on(True)
        #bottom.set_ylim(ylimits)
    
    # Top plot: Data and model [or overplot]   
    top.errorbar(obs_x, obs_y, yerr=obs_yerr, fmt='o')
    top.errorbar(mod_x, mod_y, fmt='o')
    
    top.grid(True)
    top.set_ylabel(primary_plot_ylabel)
    
    # Bottom plot: Residuals  
    bottom.errorbar(obs_x, (obs_y - mod_y)/obs_yerr, yerr=obs_yerr, fmt='o')
    bottom.grid(True)
    bottom.set_ylabel(residual_plot_ylabel)
        
    # Set the plot layout and save the output
    plt.tight_layout(h_pad = 0)
    
    if(autosave):
        savefile = re.sub('\.*', '\.'+ saveformat)
        plt.savefig(savefile, transparent=True, format=save_format)    
        plt.close()
    else:
        plt.show()
    
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
        if(re.search('_model.phot', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model', '', filename)
            overplot_file = directory + re.sub('_model', '_overplot', filename)
            plot_photometry(data_file, model_file, overplot_file, 
                autosave=options.autosave, saveformat=options.savefmt)
        
        elif(re.search('_model_v2.txt', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model_v2.txt', '_v2.txt', filename)
            plot_v2(data_file, model_file, 
                autosave=options.autosave, saveformat=options.savefmt)
            
        elif(re.search('_model_t3.txt', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model_t3.txt', '_t3.txt', filename)
            plot_t3_amp(data_file, model_file, 
                autosave=options.autosave, saveformat=options.savefmt)
            plot_t3_phi(data_file, model_file, 
                autosave=options.autosave, saveformat=options.savefmt)
        

if __name__ == "__main__":
    main()
