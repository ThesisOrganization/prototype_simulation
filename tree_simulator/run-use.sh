echo "Compiling model with USE"
echo "copiyng USE-sources in simulator-sources"
rm -rf USE-model-sources
cp -r ../USE-source USE-model-sources
echo "tuning USE makefile to compile our model"

#getting sources from model makefile and adjusting their path and variable for the USE makefile in the USE-model-sources
srcs=$(sed -E -e 's:SRCS:EDGE_SOURCES:' -n -e 's:[A-Za-z0-9_\./]+\.c:../&:gp' Makefile)
#adding sources (first 3 expressions), adding list of .o files (4th expression), change the all target to 'edge' (5th expression), add the edge target (6th expression) and finally add the _edge target (7th expression)
sed -i -e '431 i\'"$srcs"'' -i -e '539 i\EDGE_OBJ= $(EDGE_SOURCES:.c=.o)' -i -e '/all: .*/ c\all: edge' -i -e '560 i\edge: TARGET=edge\nedge: clean _edge executable' -i -e '661 i\_edge: $(EDGE_OBJ)\n\t@ld -r -g $(EDGE_OBJ) -o model/__application.o' USE-model-sources/Makefile

cd USE-model-sources

make MAX_ALLOCABLE_GIGAS=${MAX_GIGAS} NBC=1 MAX_SKIPPED_LP=${max_lp} REVERSIBLE=0 LOOKAHEAD=${lookahead} CHANNELS_PER_CELL=${channels_per_cell} PERC_USED_BUCKET=${pub} ELEM_PER_BUCKET=${epb} REPORT=1 DEBUG=0 SPERIMENTAL=1 CHECKPOINT_PERIOD=${ck} LINEAR_PINNING=1

cp edge ../simulation
cd ..

