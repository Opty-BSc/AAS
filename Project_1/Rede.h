
#define FALSE 0
#define TRUE 1
#define WHITE 2
#define GREY 3
#define BLACK 4
#define INDEX(id) (id - 1)

typedef struct tarjanArgs tarjanArgs;
typedef struct dfsArgs dfsArgs;
typedef struct router router;
typedef struct node *node;

struct tarjanArgs {

    router *graph;
    int tim;
    int networkId;
};

struct dfsArgs {

    router *graph;
    int nVertex;
};

struct router {
    
    char isNetworkId;
    char isBridge;
    int visitTime;
    int lowTime;
    int Pid;
    char color;
    struct node *head;
};

struct node {

    int id;
    struct node *next;
};

node insertList(node head, int id);
void freeList(node head);

int Tarjan(router *graph, int N);
void TarjanVisit(tarjanArgs *args, int id);
int DFS(router *graph, int N);
void DFSVisit(dfsArgs *args, int id);
