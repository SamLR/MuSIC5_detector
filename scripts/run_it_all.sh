./stopping_target_tester.sh
./degrader_tester.sh

source useful_bash.sh

deg_img_dir="../../../image/degrader_optimisation/"
ST_img_dir="../../../image/ST_optimisation/"
archive="../../../image/archive/"
archive_dir $deg_img_dir $archive_dir
archive_dir $ST_img_dir $archive_dir

root ST_comparison.C
root degrader_comparison.C