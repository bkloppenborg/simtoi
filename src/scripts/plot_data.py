#!/bin/python

import re
import os
import matplotlib.pyplot as plt
from optparse import OptionParser
from simtoi_common import read_simtoi_photometry, read_ccoifits_v2, read_ccoifits_t3

def plot_photometry(data_file, model_file, overplot_file=None, autosave=False, 
    saveformat='svg', title=None, figsize=(10,5)):
    """Plots the data and model files from SIMTOI's photometric output.
    data_file is the true photometry
    model_file is the SIMTOI simulated photometry

    overplot_file is meant to contain more frequent sampling of the model (i.e. to
        make smooth/continuous curves and will be replace the model_file in the 
        top plot.
    """

    data = read_simtoi_photometry(data_file)
    model = read_simtoi_photometry(model_file)
    
    # The data and model files should have the same length
    # the overplot file can have many more points.
    assert len(data) == len(model)
    
    # Create a split plot. The top will have a plot of only the data
    # the bottom plot will contain residuals
    fig, (top, bottom) = plt.subplots(2, 1, sharex='date', sharey=False, figsize=figsize)
    
    # Create the top plot. First plot the real data
    top.errorbar(data['date'], data['mag'], yerr=data['mag_err'], fmt='+', label="Data")
    
    # now plot the simulated data. Use the overplot file if specified, otherwise
    # just plot the model data
    if overplot_file:
        overplot_data = read_simtoi_photometry(overplot_file)
        top.plot(overplot_data['date'], overplot_data['mag'], label="Model")
        overplot_data.close()
    else:
        top.errorbar(model['date'], model['mag'], fmt='x', label="Model")
        
    # setup the plotting area
    top.invert_yaxis()
    top.grid(True)
    top.set_ylabel("Magnitude")
    top.legend(numpoints=1)    
        
    # The bottom plot will contain residuals:
    residuals = model['mag'] - data['mag']    
    bottom.errorbar(data['date'], residuals, yerr=data['mag_err'], fmt='+', color='red')
    
    # set the plotting options
    bottom.invert_yaxis()
    bottom.grid(True)
    bottom.set_ylabel("Residuals (m-d) (mag)")
    
    
    if autosave:
        save_plot(data_file, saveformat=saveformat)
    else:
        plt.show()
        
def plot_v2(data_file, model_file, autosave=False, saveformat='svg', title=None, figsize=(10,5)):

    # read in the data, assert they are of the same size
    data = read_ccoifits_v2(data_file, uv_scale=1E6)
    model = read_ccoifits_v2(model_file, uv_scale=1E6)
    assert(len(data) == len(model))
    
    # create the figure
    fig, (top, bottom) = plt.subplots(2, 1, sharex='uv_mag', sharey=False, figsize=figsize)
    
    # in the top plot, put the data and model
    legend_data = top.errorbar(data['uv_mag'], data['v2'], yerr=data['v2_err'], 
        fmt='+', label="Data")
    legend_model = top.errorbar(model['uv_mag'], model['v2'], fmt='x', 
        label="Model")
    top.set_ylabel("$V^2$")
    top.grid(True)
    #top.legend(numpoints=1, loc='lower left')
    top.set_yscale('log')
    
    # in the bottom plot, put the residuals
    ylimits = [-5, 5]
    residuals = model['v2'] - data['v2']
    errors = (residuals) / data['v2_err']
    errors, lb, ub = clip(errors, ylimits)
    legend_residuals = bottom.errorbar(data['uv_mag'], errors, fmt='+', color='red', label="Error")
    legend_lb = bottom.errorbar(data['uv_mag'], lb, fmt='v', color='red')
    legend_ub = bottom.errorbar(data['uv_mag'], ub, fmt='^', color='red')
    
    bottom.set_ylabel("Residuals ($(m-d)/\sigma$)")
    bottom.set_xlabel("Baseline length (mega-$\lambda$)")
    bottom.set_ylim([-6, 6])
    bottom.grid(True)
    
    legend = top.legend([legend_data, legend_model, legend_residuals, legend_lb, legend_ub], 
        ["Data", "Model", "Residuals", 'Lower', 'Higher'], numpoints=1, 
        loc='upper center', bbox_to_anchor=(0.5, 0.05), ncol=5)
        
    legend.draw_frame(False)
    
    if autosave:
        save_plot(data_file, saveformat=saveformat)
    else:
        plt.show()

def plot_t3(data_file, model_file, autosave=False, saveformat='svg', title=None, figsize=(10,10)):

    # read in the data, assert they are of the same size
    data = read_ccoifits_t3(data_file, uv_scale=1E6)
    model = read_ccoifits_t3(model_file, uv_scale=1E6)
    assert(len(data) == len(model))
    
    # create the figure. It will consist of three subplots:
    # A - t3_amplitude
    # B - t3_amplitude residuals
    # C - t3_phase
    # D - t3_phase residuals
    fig, (A, B, C, D) = plt.subplots(4, 1, sharex='uv_mag', sharey=False, figsize=figsize)
    
    # in the top plot, put the data and model
    legend_data = A.errorbar(data['uv_mag'], data['t3_amp'], 
        yerr=data['t3_amp_err'], fmt='+', label="Data")
    legend_model = A.errorbar(model['uv_mag'], model['t3_amp'], fmt='x', label="Model")
    A.set_ylabel("$T_3$ A")
    A.grid(True)
    #A.legend(numpoints=1, loc='lower left')
    A.set_yscale('log')
    
    # in the bottom plot, put the residuals
    ylimits = [-5, 5]
    residuals = model['t3_amp'] - data['t3_amp']
    errors = (residuals) / data['t3_amp_err']
    errors, lb, ub = clip(errors, ylimits)
    legend_residuals = B.errorbar(data['uv_mag'], errors, fmt='+', color='red', label="Error")
    B.errorbar(data['uv_mag'], lb, fmt='v', color='red')
    B.errorbar(data['uv_mag'], ub, fmt='^', color='red')
    B.set_ylabel("Residuals ($(m-d)/\sigma$)")
    B.set_ylim([-6, 6])
    B.grid(True)

    # in the top plot, put the data and model
    C.errorbar(data['uv_mag'], data['t3_phi'], yerr=data['t3_phi_err'], fmt='+', label="Data")
    C.errorbar(model['uv_mag'], model['t3_phi'], fmt='x', label="Model")
    C.set_ylabel("$T_3 \phi$ (deg)")
    C.grid(True)
    #C.legend(numpoints=1, loc='lower left')
    C.set_ylim([-200, 200])
    
    # in the bottom plot, put the residuals
    ylimits = [-5, 5]
    residuals = model['t3_phi'] - data['t3_phi']
    errors = (residuals) / data['t3_phi_err']
    errors, lb, ub = clip(errors, ylimits)
    
    #residuals %= 180
    D.errorbar(data['uv_mag'], errors, fmt='+', color='red', label="Error")
    legend_lb = D.errorbar(data['uv_mag'], lb, fmt='v', color='red')
    legend_ub = D.errorbar(data['uv_mag'], ub, fmt='^', color='red')
    
    D.set_ylabel("Residuals ($(m-d)/\sigma$)")
    D.set_xlabel("Baseline length (mega-$\lambda$)")
    D.set_ylim([-6, 6])
    D.grid(True)
    
    legend = A.legend([legend_data, legend_model, legend_residuals, legend_lb, legend_ub], 
        ["Data", "Model", "Residuals", 'Lower', 'Higher'], numpoints=1, 
        loc='upper center', bbox_to_anchor=(0.5, 0.05), ncol=5)
        
    legend.draw_frame(False)
    
    if autosave:
        save_plot(data_file, saveformat=saveformat)
    else:
        plt.show()
    
def clip(values, limits):
    """Clips a range of values by limits. Returns a tuple:
    in_range, below_range, above_range
    """
    good = values.copy()
    low = values.copy()
    high = values.copy()
    
    # pick off the lower and upper bounds
    lb = limits[0]
    ub = limits[1]
    
    for i in range(0, len(values)):
        value = values[i]
        
        # if the value is outside of the limits, set A's value to NAN
        if value < lb:
            good[i] = float('NaN')
            high[i] = float('NaN')
            low[i] = lb
        elif value > ub:
            good[i] = float('NaN')
            low[i] = float('NaN')  
            high[i] = ub      
        else:
            high[i] = float('NaN') 
            low[i] = float('NaN') 
    
    return [good, low, high]

def plot_uv(v2_filename, t3_filename, autosave=False, saveformat='svg', figsize=(10,10)):
    """Creates a UV plot from SIMTOI/ccoifits text exports
    """
    
    # read in the data files:
    v2 = read_ccoifits_v2(v2_filename, uv_scale=1E6)
    t3 = read_ccoifits_t3(t3_filename, uv_scale=1E6)
    
    fig = plt.figure(figsize=figsize)
    ax1 = fig.add_subplot(1,1,1, aspect='equal')
    
    # plot the in the v-u plane:
    ax1.scatter( v2['v'],  v2['u'], c='green', marker='s', label="V2")
    ax1.scatter(-v2['v'], -v2['u'], c='green', marker='s')
    
    # now the t3:
    ax1.scatter( t3['v1'],  t3['u1'], c='red', marker='x', label="T3")
    ax1.scatter(-t3['v1'], -t3['u1'], c='red', marker='x')    
    ax1.scatter( t3['v2'],  t3['u2'], c='red', marker='x')
    ax1.scatter(-t3['v2'], -t3['u2'], c='red', marker='x')   
    ax1.scatter( t3['v3'],  t3['u3'], c='red', marker='x')
    ax1.scatter(-t3['v3'], -t3['u3'], c='red', marker='x')   
    
    ax1.grid(True)
    ax1.set_ylabel("V (mega-$\lambda$)")
    ax1.set_xlabel("U (mega-$\lambda$)")
    ax1.legend(numpoints=1)       
    
    
    if autosave:
        save_plot(data_file, saveformat=saveformat)
    else:
        plt.show()
    
def save_plot(data_filename, saveformat='svg'):
    """
    """
    # Create the savefile name. Simply replace the extension on the file with
    # saveformat. We know the data files end in '.txt' so we'll split on all
    # periods, and replace the last instance
    savefile = re.split('\.', data_filename)
    savefile[len(savefile) - 1] = saveformat
    savefile = '.'.join(savefile)

    # Write out the figure:
    plt.savefig(savefile, transparent=True, format=saveformat)    
    plt.close()
    
def main():
    """
    Main program loop. Looks through the contents of the specified directory
    and plots all of the data.
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
    
    # If we are saving the file, try switching backends.
    if options.autosave:
        try:
            matplotlib.use(options.savefmt)
        except:
            pass
    
    for filename in filenames:
        if(re.search('_model.phot', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model', '', filename)
            plot_photometry(data_file, model_file, autosave=options.autosave,
                saveformat=options.savefmt)
        
        elif(re.search('_model_v2.txt', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model_v2.txt', '_v2.txt', filename)
            plot_v2(data_file, model_file, 
                autosave=options.autosave, saveformat=options.savefmt)
            
        elif(re.search('_model_t3.txt', filename)):
            model_file = directory + filename
            data_file = directory + re.sub('_model_t3.txt', '_t3.txt', filename)
            plot_t3(data_file, model_file, 
                autosave=options.autosave, saveformat=options.savefmt)
    
        

if __name__ == "__main__":
    main()
