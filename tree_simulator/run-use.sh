echo "Compiling model with USE"
echo "copiyng USE-sources in simulator-sources"
rm -rf USE-model-sources
cp -r ../USE-source USE-model-sources
echo "tuning USE makefile to compile our model"
cp Makefile-USE-only USE-model-sources/Makefile
cd USE-model-sources

make $test FADING_RECHECK_FREQUENCY=300 MAX_ALLOCABLE_GIGAS=${MAX_GIGAS} NBC=1 MAX_SKIPPED_LP=${max_lp} REVERSIBLE=0 LOOKAHEAD=${lookahead} TA=${ta} TA_DURATION=${ta_duration} CHANNELS_PER_CELL=${channels_per_cell} TA_CHANGE=${ta_change} PERC_USED_BUCKET=${pub} ELEM_PER_BUCKET=${epb} REPORT=1 DEBUG=0 SPERIMENTAL=1 CHECKPOINT_PERIOD=${ck} LINEAR_PINNING=1

cp edge ../simulation
cd ..
rm -rf USE-model-sources
