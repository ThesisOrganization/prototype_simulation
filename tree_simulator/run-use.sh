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

#we use the free command to get only the available memory in RAM
MAX_GIGAS=$(free -h | sed -E -e '1d' -e '3d' -e 's:[GMK]i::g' -e 's/Mem:[ ]+//' -e 's/[0-9\.]+[ ]+//g')
ck=10
pub=0.33333
epb=3
nbc=1
lookahead=0.0
dbg=0
sperimental=1
lin_pin=1
reversible=0
#1=stampe dettagliate
report=1

make MAX_ALLOCABLE_GIGAS=${MAX_GIGAS} NBC=${nbc} MAX_SKIPPED_LP=${max_lp} REVERSIBLE=${reversible} LOOKAHEAD=${lookahead} PERC_USED_BUCKET=${pub} ELEM_PER_BUCKET=${epb} REPORT=${report} DEBUG=${dbg} SPERIMENTAL=${sperimental} CHECKPOINT_PERIOD=${ck} LINEAR_PINNING=${lin_pin}

cp edge ../simulation
cd ..

