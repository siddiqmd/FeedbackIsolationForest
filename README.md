# Build the C++ code with 'make' command
(Tested on gcc version 5.1.0. Any version above 4.7 should work)
```sh
$ make
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/common.c -o C/common.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/object.c -o C/object.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/strfun.c -o C/strfun.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/readwrite.c -o C/readwrite.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/argparse.c -o C/argparse.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/argparse_iforest.c -o C/argparse_iforest.o
gcc --std=c99 -D_GNU_SOURCE -Wall -Werror -g -c C/frames.c -o C/frames.o
ld -r C/common.o C/object.o C/strfun.o C/readwrite.o C/argparse.o C/argparse_iforest.o C/frames.o -o cincl.o
g++ --std=c++11 -Wall -Werror -g   -c Forest.cpp -o Forest.o
g++ --std=c++11 -Wall -Werror -g   -c IsolationForest.cpp -o IsolationForest.o
g++ --std=c++11 -Wall -Werror -g   -c Tree.cpp -o Tree.o
g++ --std=c++11 -Wall -Werror -g   -c utility.cpp -o utility.o
g++ --std=c++11 -Wall -Werror -g   -c OnlineIF.cpp -o OnlineIF.o
g++ --std=c++11 -Wall -Werror -g   -c main.cpp -o main.o
g++ --std=c++11 -Wall -Werror -g   -o iforest.exe cincl.o Forest.o IsolationForest.o Tree.o utility.o OnlineIF.o main.o
```

This will create the executable 'iforest.exe'

```sh
$ ../iforest.exe
Usage: D:\Codes\githubFeedbackIF\feedbackisolationforest\iforest.exe [options]
Options:
        -i FILE, --infile=FILE
                Specify path to input data file. (Required).
        -o FILE, --outfile=FILE
                Specify path to output results file. (Required).
        -m COLS, --metacol=COLS
                Specify columns to preserve as meta-data. (Separated by ',' Use '-' to specify ranges).
        -t N, --ntrees=N
                Specify number of trees to build.
                Default value is 100.
        -s S, --sampsize=S
                Specify subsampling rate for each tree. (Value of 0 indicates to use entire data set).
                Default value is 2048.
        -d MAX, --maxdepth=MAX
                Specify maximum depth of trees. (Value of 0 indicates no maximum).
                Default value is 0.
        -H, --header
                Toggle whether or not to expect a header input.
                Default value is true.
        -v, --verbose
                Toggle verbose ouput.
                Default value is false.
        -w W, --RegType=W
                Type of regularizer: 1 indicates l1 and 2 indicates l2
                Default value is 1.
        -c N, --columns=N
                specify number of columns to use.
                Default value is 0.
        -r R, --regularizer=R
                specify regularization constant.
                Default value is 0.
        -u U, --updatetype=U
                specify type of update to perform on weights.
                Default value is 0.
        -f F, --numfeedback=F
                specify number of feedback iteration to perform.
                Default value is 100.
        -x X, --numiter=X
                specify number of times experiments to rerun.
                Default value is 1.
        -l L, --losstype=L
                specify type of loss to use.
                Default value is 0.
        -g G, --numgradupd=G
                specify number of gradient update to run.
                Default value is 1.
        -a A, --learningrate=A
                specify learning rate for gradient update. Set 0 for variable learning rate 1/sq(t).
                Default value is 1.
        -p P, --posweight=P
                specify whether weights to be restricted to be positive only.
                Default value is 0.
        -z Z, --reInitWeights=Z
                specify whether weights reset to 1 after each feedback.(for stochastic and batch update)
                Default value is 0.
        -h, --help
                Print this help message and exit.
```

##### Note: The first column of the input file should have the groundtruth label as "anomaly" or "nominal" to incorporate feedback propertly. See one of the input file inside the "dataset" directory for proper csv format.

# Test Run
```sh
$ cd test
$ ../iforest.exe -i datasets/anomaly/ann_thyroid_1v3/fullsamples/ann_thyroid_1v3_1.csv -o outtest/ann_thyroid_1v3_1 -t 100 -s 256 -m 1 -x 5 -f 10 -w 2 -l 2 -a 1
# Trees          = 100
# Samples        = 256
MaxHeight        = 0
Orig Dimension   = 3251,21
# Iterations     = 5
# Feedbacks      = 10
Loss   type      = logistic
Update type      = online
Num Grad Upd     = 1
Reg. Constant    = 0
Learning Rate    = 1
Variable LRate   = 0
Positive W only  = 0
ReInitWgts       = 0
Regularizer type = L2
iter 0, # Anomaly: Baseline -> 2 Feedback -> 8
iter 1, # Anomaly: Baseline -> 1 Feedback -> 1
iter 2, # Anomaly: Baseline -> 2 Feedback -> 8
iter 3, # Anomaly: Baseline -> 2 Feedback -> 5
iter 4, # Anomaly: Baseline -> 2 Feedback -> 7
Avg: Baseline -> 1.8 Feedback -> 5.8
Time elapsed: 2 seconds
```

This will create two output files: "ann_thyroid_1v3_1_summary_feed_0_losstype_logistic_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0_inwgt_0_rtype_L2.csv" and "ann_thyroid_1v3_1_summary_feed_10_losstype_logistic_updatetype_online_ngrad_1_reg_0_lrate_1_pwgt_0_inwgt_0_rtype_L2.csv" containing number of anomaly discovered after each feedback on different iterations.


```
iter,feed1,feed2,feed3,feed4,feed5,feed6,feed7,feed8,feed9,feed10
1,0,1,1,1,1,1,1,1,2,2
2,0,0,0,0,0,0,0,0,0,1
3,1,1,1,1,1,2,2,2,2,2
4,0,0,0,0,0,1,1,1,1,2
5,0,0,1,1,1,1,1,2,2,2

```

```
iter,feed1,feed2,feed3,feed4,feed5,feed6,feed7,feed8,feed9,feed10
1,0,1,2,3,4,5,6,7,8,8
2,0,0,0,0,0,0,0,0,0,1
3,1,2,2,2,3,4,5,6,7,8
4,0,0,0,0,0,1,2,3,4,5
5,0,0,1,1,2,3,4,5,6,7
```

Note that, the first output file is the result from the baseline isolation forest hence ignores all the feedbacks.
