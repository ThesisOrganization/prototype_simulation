struct topArray
{
int nn;
int numberOfReceivers;
int numberInfos;

int * receiver;
char ** info;

};

struct topArray * getTopology();
int getNumberReceiv(struct topArray * array, int index);
int getNumberInfos(struct topArray * array, int index);

int * getNext(struct topArray * array, int next);
char ** getInfo(struct topArray * array, int index);
