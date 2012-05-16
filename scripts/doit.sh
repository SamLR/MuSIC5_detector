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
# `(./doit.sh >> log)` appends stdout from doit.sh to log (can use > to 
#        overwrite) the brackets ensure that this redirect happens first
# `2>&1` redirects stderr (file handler '2') to stdout (file handler '1')
#       '>&' indicates direct to file handler rather than filename (which 
#        would be just '>')
# `| tee -a log` pipes the result (i.e. stderr redirected to stdout) to tee.  
#       Tee appends (-a) this to log and prints it
#
# Useful observation: any further files added to tee will receive ONLY the 
#   stderr output of doit. Makes creation of separate error logs easy
#
# Multiple commands can be grouped and run like this. Only the last stdout is 
#   redirected though so the commands must be grouped and redirected as a 
#   whole eg:
#   `({ pwd && ./doit.sh  ;} >> log) 2>&1 | tee -a log er.log` NOTE SPACES!
#   the opposite ordering will fail (i.e. `./doit.sh && pwd` as doit fails)
#   not sure how to skip that... never mind 

ls # this should only go to log
ls --xyz # this should raise an error printed to screen & logged

# quick test of something utterly different (command delay)
cmd='$exe $X' # use of ' makes bash treat this as as a literal
exe="ls"
X="-l"
eval $cmd

# git voodoo
# gets the commit number and makes it pretty as well as the current branch

gitcommit="$(git log --pretty=format:'%h' -n 1)" || gitcommit="not commited"
gitbranch="$(git symbolic-ref HEAD 2>/dev/null)" || gitbranch="unnamed branch"
gitbranch=${gitbranch##refs/heads/}
echo $gitbranch $gitcommit 
