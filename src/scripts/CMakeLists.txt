cmake_minimum_required(VERSION 2.8) 
project(simtoi)

# Copy some scripts into the binary output directory.

file(COPY InsertSpaces.awk DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY plot_histogram.py DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY scripted_bootstrap.py DESTINATION ${EXECUTABLE_OUTPUT_PATH})
