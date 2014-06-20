import argparse
import os
import glob
from datetime import datetime
from subprocess import call
import shutil

# The directory in which the SIMTOI executable, kernels, and shader are located.
simtoi_path = "/home/bkloppenborg/workspace/simtoi/bin"

def main():

    parser = argparse.ArgumentParser(
        description='Runs SIMTOI on all model files in a directory.')
    parser.add_argument('directory', metavar='directory', type=str,
        help="Directory with model files")
    parser.add_argument('data', metavar='data', type=str, nargs='+',
        help="One or more data files supported by SIMTOI.")
    parser.add_argument('--engine', metavar='engine', type=str,
        default='multinest', help="Minimization engine to use. Default: multinest")
    parser.add_argument('--display', metavar='display', type=str,
        default=":0", help="X11 display to use. Normaly just ':0' or 'hostname:0'")
    parser.add_argument('--tempdir', metavar='tempdir', type=str,
        default="/tmp/simtoi_temp/", 
        help="Temporary directory for SIMTOI output. Default: /tmp/simtoi_temp/")

    args = parser.parse_args()
    model_directory = os.path.abspath(args.directory)
    data_files = args.data
    min_engine = args.engine
    display = args.display
    simtoi_output_dir = args.tempdir

    # Conver the data file(s) paths to absolute paths
    for i in range(0, len(data_files)):
        data_files[i] = os.path.abspath(data_files[i])

    os.chdir(model_directory)
    model_files = glob.glob("*.json")

    for model_file in model_files:
        # the save directory will match the model file name
        save_dir = model_directory + '/' + model_file[0:len(model_file) - 5] + '/'
        
        model_file = model_directory + '/' + model_file

        # verify that the safe directory
        if not os.path.exists(simtoi_output_dir):
            os.makedirs(simtoi_output_dir)

        print "\nStarting SIMTOI on\n"
        print "\t" + model_file
        t_start = datetime.now()
        print "Start time: " + str(t_start)

        stdout_file = open(simtoi_output_dir + "log_cout.txt", 'w')

        # create a list of data

        # assemble the command
        simtoi_command = ['./simtoi', '-m', model_file, '-o', simtoi_output_dir, 
            '-e', min_engine, '-c', '-display', display]

        for data_file in data_files:
            simtoi_command.append('-d')
            simtoi_command.append(data_file)

        # convert all fields to strings
        simtoi_command = map(str, simtoi_command)
        # run simtoi
        os.chdir(simtoi_path)
        # If you want to see the command being executed, uncomment this line:
        #print ' '.join(simtoi_command) 
        # run SIMTOI
        call(simtoi_command, stdout=stdout_file)

        t_end = datetime.now()
        print "Finished SIMTOI fit!"
        print "Finish time: " + str(t_end)
        print "Total execution time: " + str(t_end - t_start)

        # close output files
        stdout_file.close()

        # make the save directories:
        if not os.path.exists(save_dir):
            os.makedirs(save_dir)
            
        os.chdir(simtoi_output_dir)
        for filename in glob.glob('*'):
            shutil.move(simtoi_output_dir + filename, save_dir + filename)


# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
