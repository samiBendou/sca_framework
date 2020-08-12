"""Export and plot data from SoC.

The data retrieved from SoC consists on side-channel leakage
and encryption data in order to later perform correlation.

The side-channel leakage is plot to evaluate the quality of
the acquisition.
The data is exported in 3 separated CSV files.

Examples
--------
.. code-block:: shell

    $ python acquire.py 256 COM5
    $ python acquire.py -m sw 1024 cmd

In the above example, the first line will launch the acquisition
of 256 hardware traces and read the serial port ``COM5``.

The second line will read the file ``cmd_hw_1024.log`` located in the
directory containing the 1024 software traces.

In the last case, the file must be a valid binary log file previously
acquired from the SoC via serial port.

"""

import argparse

import core


@core.operation_decorator("acquire.py", "\nexiting...")
def main(args):
    s = core.acquire_bin(args.source, args.mode, args.iterations)
    parser = core.parse_bin(s, args.iterations)
    core.export_csv(args.iterations, args.mode, parser.meta, parser.leak, parser.data)
    core.plot_acq(parser.leak, parser.meta, limit=args.plot)


argp = argparse.ArgumentParser(
    description="Acquire data from SoC and export it.")
argp.add_argument("iterations", type=int,
                  help="Requested count of traces.")
argp.add_argument("source", type=str,
                  help="Binary data acquisition source.")
argp.add_argument("-m", "--mode", choices=core.MODES, default=core.MODES[0],
                  help="Encryption mode.")
argp.add_argument("-p", "--plot", type=int, default=16,
                  help="Count of raw traces to plot.")

if __name__ == "__main__":
    main(argp.parse_args())
