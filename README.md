# ACA Processor Simulator:

This processor simulator is a command line tool that allows you to simulate the execution of a RISC-V assembly code. To run the simulator, follow the instructions below:

Usage: ./main [-ss <super-scalar>] [-pw <pipeline-width>] [-fm <forwarding-mode>] [-bm <branch-prediction-mode>] [-i <image>] [-g <general purpose ALU>] [-r <reservationSize>] [-o <out-of-order>] [-v] <filename>

The following are the available options:

-ss: Super Scalar mode. Set to 1 to enable or 0 to disable. If enabled, the processor can execute multiple instructions in parallel.

-pw: Pipeline width. Valid values are 1, 2, 4, or 8. This sets the number of stages in the pipeline.

-fm: Forwarding mode. Set to 0 for no forwarding, 1 for forwarding, or 2 for forwarding and renaming. This determines how the processor handles data dependencies between instructions.

-bm: Branch prediction mode. Set to 0 for Static Not Taken, 1 for static Taken, 2 for dynamic 1 bit, or 3 for dynamic 2 bit. This determines how the processor handles branch instructions.

-i: Image mode. Set to 1 to enable or 0 to disable. If enabled, the simulator expects an image processing program and will place the image data in the appropriate memory and print the output to a csv file. to view images go to the images folder and run `python3 showImages.py`

-g: A retrofitted option when g = 0 our alus stop acting like general purpose alus and doe th relavent split (this makes 2 alus Load and store and 2 ALUs branches so please only use with pipeline width of 8)

-o: Out-of-order execution. Set to 1 to enable or 0 to disable. This option only works with Renaming + Register Forwarding, as otherwise false dependencies can cause blocking issues.

-v: Verbose mode. Set to 1 to enable or 0 to disable. If enabled, the simulator will output additional information during execution.

File: The RISC-V assembly code file to be executed.

Example Usage:

To run the simulator with Super Scalar mode enabled, a pipeline width of 1, forwarding and renaming, dynamic 2-bit branch prediction, no image processing, and no out-of-order execution, and to output additional information during execution for the file "fib.s":

./main -ss 1 -pw 1 -fm 2 -bm 0 -i 1 -g 1 -r 8 -o 1 -v 0 ./fib.s

Note: Ensure that the RISC-V assembly code file is in the same directory as the /src folder

## Compilation

Simply run the make command from the source directory your executable should be in ./bin.

## Compiled Code

Our compiled code has its own readMe for how to compile, you will need the Risc-V gnu toolchain.
