  struct topArray
{
int numberOfReceivers;
int numberInfos;

int * receiver;
char ** info;

};

struct generalTopology{
  int total_nodes;
  int sensor_nodes;
  struct topArray * topArr;
};

struct generalTopology getTopology();
int getNumberReceiv(struct topArray * array, int index);
int getNumberInfos(struct topArray * array, int index);

int * getNext(struct topArray * array, int next);
char ** getInfo(struct topArray * array, int index);
