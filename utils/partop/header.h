struct topArray
{
int nn;
char * info;
int receiver;
};

struct topArray * getTopology();
int getNext(struct topArray * array, int next);
char * getInfo(struct topArray * array, int index);
