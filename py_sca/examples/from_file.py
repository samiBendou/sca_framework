import matplotlib.pyplot as plt
import numpy as np
import py_sca.stats as stats
from py_sca.parser import Log

SEED_NO = 0
TRACES_COUNT = 512
SLIDING_CONV_SIZE = 128  # sliding average convolution kernel size
F_SAMPLING = 200e6  # sampling frequency of the acquisition system
TRACES_TO_PLOT = 0  # count of raw traces to plot
LOG_TO_CSV = False  # true to log the file contents into csv

# read FPGA acquisition command prompt log
log = Log.from_file("../../data/cmd_%d_s%d.log" % (TRACES_COUNT, SEED_NO))

# log SCA acquisition into CSV files
if LOG_TO_CSV:
    log.report_data("../../data/report_data_%d_s%d_hw.csv" % (TRACES_COUNT, SEED_NO))
    log.report_traces("../../data/report_traces_%d_s%d_hw.csv" % (TRACES_COUNT, SEED_NO))

# Crop traces and remove offset
original_traces = np.array(log.cropped_traces())
filtered_traces = original_traces.copy()
m, l = original_traces.shape
original_traces = original_traces - np.mean(original_traces, axis=1).reshape((m, 1))

# High band pass to extract round information from the noise
for idx in range(0, m):
    filtered_traces[idx] = stats.bandpass_filtering(filtered_traces[idx], F_SAMPLING)

plt.rcParams["figure.figsize"] = (16, 9)
for idx in range(max(0, m - TRACES_TO_PLOT), m):
    plt.plot(original_traces[idx])
    plt.plot(filtered_traces[idx])
    plt.show()

# Compute the average trace signal and normalize it
mean = filtered_traces.mean(axis=0)
mean /= np.linalg.norm(mean, ord=np.inf)
plt.plot(mean, color="grey")

# Low band pass to extract round peaks from the noise
mean = stats.bandpass_filtering_2(mean, F_SAMPLING)
mean /= np.linalg.norm(mean, ord=np.inf)
plt.plot(mean, color="red")

# Smooth data
mean = stats.sliding_average(mean, SLIDING_CONV_SIZE)
mean /= np.linalg.norm(mean, ord=np.inf)
plt.plot(mean, color="blue")
plt.title("Average power consumption (slide=%d) (traces=%d)" % (SLIDING_CONV_SIZE, m))
plt.xlabel("Normalized value")
plt.ylabel("Time sample")
plt.show()
