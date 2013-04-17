#!/bin/bash

source common.sh
check_newest

outdir="../pid_macros/root"
macrodir="../pid_macros"

echo "Running all positive muons"
$exe $infile $outdir/g4bl_mu_pos.root $macrodir/all_pos_mu.mac > /dev/null
echo "Running all negative muons"
$exe $infile $outdir/g4bl_mu_neg.root $macrodir/all_neg_mu.mac > /dev/null