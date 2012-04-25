sol=fieldmap/fieldmap_solenoid.txt
dip=fieldmap/fieldmap_dipole.txt
pol=1
in_root=g4blout/monitor6_By-0T_cor.root
#out_root=root/monitor6_By-0T_cor.root
out_root=hoge.root
macro=run1.mac

# Interactive mode
#../../bin/Linux-g++/music $sol $dip $pol $in_root /tmp/out.root

# Batch mode
../../bin/Linux-g++/music $sol $dip $pol $in_root $out_root $macro  > /dev/null
