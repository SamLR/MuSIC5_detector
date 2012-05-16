# 
# This is a file to test if I can redirect stdout & stderr to a log whilst 
# _also_ displaying stderr on screen
# the command to run (and test) this is `(./doit.sh >> log) 2>&1 | tee -a log`
# tee takes an input and puts it on stdout as well as any files specified
# from the man "tee [-ai] [file ...]" a = append mode i = ignore sigint
#
#
# the command works as advertised
# 
# `(./doit.sh >> log)` appends stdout from doit.sh to log (can use > to overwrite)
#        the brackets ensure that this redirect happens first
# `2>&1` redirects stderr (file handler '2') to stdout (file handler '1')
#       '>&' indicates direct to file handler rather than filename (which 
#        would be just '>')
# `| tee -a log` pipes the result (i.e. stderr redirected to stdout) to tee. Tee 
#       appends (-a) this to log and prints it
#

ls # this should only go to log
ls --xyz # this should raise an error printed to screen & logged

