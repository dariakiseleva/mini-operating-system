# Execution

`make clean`

`make make mysh framesize=X varmemsize=Y`

Where X is the space for frames in memory and must be a multiple of 3, and Y is the space for variable value storage.

`./mysh < testfile.txt`

Where `testfile.txt` is e.g.
> exec prog10 prog11 prog12 RR

And program files are, e.g. `prog10` are:
> echo P10L1
set a PTenLineTwoSet; print a
echo P10L3
echo $a
echo P10L5
set a PTenLineSixSet; echo $a
echo P10L7

*Note: all files must be in the same directory from which the program is run*

# Testing
Folder [A3_testcases_public](./A3_testcases_public/) contains test files and expected results.

# Limitations
* 