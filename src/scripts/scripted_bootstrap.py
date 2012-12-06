#!/usr/bin/python

# This script is an example of how SIMTOI can be scripted to execute
# a series of bootstraps on user-defined models and data.
# The script keeps STDIN and STDOUT

# This script should be executed from the directory containing the SIMTOI
# executable.

from subprocess import call
import os
import glob
import shutil

# Specify some global properties
image_width = 128
image_scale = 0.025
min_engine = 5 # levmar-based bootstrapper

# Input and output directories:
data_base_dir = "/homes/bkloppen/Projects/epsAur-Interferometry/data/CHARA-MIRC/OIFITS/"
model_base_dir = "/homes/bkloppen/Projects/epsAur-Interferometry/modeling/simtoi_models/"
save_base_dir = "/homes/bkloppen/Projects/epsAur-Interferometry/modeling/bootstrap_results/"

# Where SIMTOI saves the files and what they are named
simtoi_path = "/homes/bkloppen/workspace/simtoi/bin/"
simtoi_output_dir = "/tmp/"
simtoi_output_prefix = "min_output_"

#
epsAur_star = 'epsAur_star.json'

run_info = [
# nData = 40, falls under liboi issue 32
#    ['2008-09-eps_Aur-avg5.oifits', epsAur_star, '2008-09'],
# Generates exceptions
#    ['2008-11-eps_Aur-avg5.oifits', epsAur_star, '2008-11'],
    ['2008-12-eps_Aur-avg5.oifits', epsAur_star, '2008-12'],
#    ['2009-11-eps_Aur-avg5.oifits', epsAur_star, '2009-11'],
#    ['2009-12-eps_Aur-avg5.oifits', epsAur_star, '2009-12'],
#    ['2010-02-eps_Aur-all-avg5.oifits', epsAur_star, '2010-02'],
#    ['2010-08-eps_Aur-all-avg5.oifits', epsAur_star, '2010-08'],
#    ['2010-09-eps_Aur-all-avg5.oifits', epsAur_star, '2010-09'],
#    ['2010-10-eps_Aur-all-avg5.oifits', epsAur_star, '2010-10'],
#    ['2010-11-eps_Aur-all-avg5.oifits', epsAur_star, '2010-11'],
#    ['2010-12-eps_Aur-all-avg5.oifits', epsAur_star, '2010-12'],
#    ['2011-01-eps_Aur-avg5.oifits', epsAur_star, '2011-01'],
    ['2011-09-18-eps_Aur-avg5.oifits', epsAur_star, '2011-09-18'],
    ['2011-09-24-eps_Aur-avg5.oifits', epsAur_star, '2011-09-24'],
    ['2011-10-eps_Aur-avg5.oifits', epsAur_star, '2011-10'],
    ['2011-11-eps_Aur-avg5.oifits', epsAur_star, '2011-11'] 
    ]


for i in range(0, len(run_info)):
    # unpack the data/model/save_dir information:
    [data_file, model_file, save_dir] = run_info[i]
    
    print "\nStarting bootstrap on " + data_file

    stdout_file = open(simtoi_output_dir + simtoi_output_prefix + "log_cout.txt", 'w')
    
    # append absolute paths
    data_file = data_base_dir + data_file
    model_file = model_base_dir + model_file

    # assemble the command
    simtoi_command = ['./simtoi', '-w', image_width, '-s', image_scale, '-d', data_file, '-m', model_file, '-e', min_engine, '-c']
    simtoi_command = map(str, simtoi_command)

    # run simtoi
    os.chdir(simtoi_path)
    call(simtoi_command, stdout=stdout_file)
    
    # close output files
    stdout_file.close()
    
    # make the save directories:
    save_dir = save_base_dir + save_dir + '/'
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
        
    os.chdir(simtoi_output_dir)
    for filename in glob.glob(simtoi_output_prefix + '*'):
        shutil.move(simtoi_output_dir + filename, save_dir + filename)
        
