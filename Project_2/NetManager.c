
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**********
 * Structs
**********/
typedef struct vNode *vNode;
typedef struct eNode *eNode;
typedef struct vertex *vertex;
typedef struct edge *edge;
typedef struct vQueue vQueue;
typedef struct vStack vStack;

struct vNode {

    vertex item;
    vNode next;
};

struct eNode {

    edge item;
    eNode next;
};

struct vertex {
    
    eNode adjHead;
    eNode adjTail;
    int height;
    int excess;
};

struct edge {
    
    vertex from;
    vertex to;
    edge dual;
    int capacity;
    int flow;
};

struct vQueue {

    vNode head;
    vNode tail;
};

struct vStack {

    int nHeights;
    vNode *list;
};


/**************
 * Input parse
**************/
int readValue(int minValue);

/******************
 * Data structures
******************/

/* eNodes */
eNode createENode(edge item, eNode next);
void freeENodes(eNode head);

/* Edges */
edge createEdge(vertex from, vertex to, int capacity, int flow, edge dual);
void addEdge(edge eItem);
void freeEdges(int nVerts);

/* vNodes */
vNode createVNode(vertex item, vNode next);
void freeVNodes(vNode head);

/* vList */
void vListInit(int nVerts);

/* Queue */
void addToQueue(vertex item);
void popQueue();

/* Stack */
void stackInit(int nHeights);
void addToStack(vertex item);
void stackClimb(vertex item);
void stackSweep(int height);
void freeStack();

/***************
 * Push-Relable
***************/
void preflow(vertex src);
void relax(edge link, int flow);
void relable(vertex item);
void pushRelable();

/*****************************
 * qsort comparison functions
*****************************/
int cmpTo (const void *x, const void *y);
int cmpFrom (const void *x, const void *y);

/*******************
 * Global variables
*******************/
vertex vList;
vQueue queue;
vStack stack;

/*********
 * Macros
*********/
#define JOINT(id) (vList + id)

#define MIN(x, y) (x < y ? x : y)


int main() {

    int i;
    int fromId, toId, capacity;
    vertex to;
    edge link;
    vNode joiNode;
    eNode adjNode;

    int nProviders = readValue(1);
    int nStations = readValue(0);
    int nLinks = readValue(0);
    int nVerts = nProviders + 2 * nStations + 2;

    char flag;
    int cutLen = 0;
    edge *cutLinks;

    vListInit(nVerts);
    stackInit(nVerts);
    /* Setting source's height */
    vList[1].height = stack.nHeights;

    for (i = 0; i < nVerts; i++) {
        addToStack(JOINT(i));
    }

    /******************************
     * Parse Providers && Stations
    ******************************/
    for (i = 2; i < nVerts - nStations; i++) {

        capacity = readValue(1);

        if (i > nProviders + 1) {
            /* Parsing Station */
            /* Creating adjacency */
            addEdge(createEdge(JOINT(i + nStations), JOINT(i), capacity, 0, NULL));

        } else {
            /* Creating adjacency */
            addEdge(createEdge(JOINT(i), JOINT(0), capacity, 0, NULL));
        }
    }
    
    /************************
     * Parse Network's links
    ************************/
    for (i = 0; i < nLinks; i++) {
        
        toId = readValue(2);
        fromId = readValue(1); 
        capacity = readValue(1);

        if (toId > nProviders + 1) {
            /* Parsing from station id */
            toId += nStations;
        }

        /* Creating adjacency */
        addEdge(createEdge(JOINT(fromId), JOINT(toId), capacity, 0, NULL));
    }

    pushRelable();

    /* Max Flow */
    printf("%d\n", JOINT(0)->excess);

    /* Minimum cut */
    cutLinks = (edge *)malloc(sizeof(edge) * nLinks);
    assert(cutLinks);

    for (joiNode = stack.list[stack.nHeights]; joiNode != NULL; joiNode = joiNode->next) {

        for (adjNode = joiNode->item->adjHead; adjNode != NULL; adjNode = adjNode->next) {

            link = adjNode->item;

            if (link->capacity == 0) break;

            if (link->to->height < stack.nHeights && link->to != JOINT(0)) {
                /* Add to cutLinks */
                cutLinks[cutLen++] = link;
            }
        }
    }

    qsort(cutLinks, cutLen, sizeof(edge), cmpFrom);
    qsort(cutLinks, cutLen, sizeof(edge), cmpTo);

    for (i = 0, flag = 0; i < cutLen; i++) {

        if (cutLinks[i]->from < JOINT(nVerts - nStations)) continue;

        printf("%s%ld", flag ? " " : "", cutLinks[i]->to - vList);

        if (flag == 0) flag = 1;
        cutLinks[i] = NULL;
    }

    printf("\n");

    for (i = 0; i < cutLen; i++) {

        if (cutLinks[i] == NULL) continue;

        to = cutLinks[i]->to - (cutLinks[i]->to < JOINT(nVerts - nStations) ? 0 : nStations);

        printf("%ld %ld\n", to - vList, cutLinks[i]->from - vList);
    }

    /***********************
     * Free data structures
    ***********************/
    free(cutLinks);
    freeEdges(nVerts);
    freeStack();
    free(vList);

    exit(EXIT_SUCCESS);
}


/**************
 * Input parse
**************/
int readValue(int minValue) {

    int value;

    if (scanf("%d", &value) != 1) {
        perror("scanf");
        exit(EXIT_FAILURE);
    }

    if (value < minValue) {
        fprintf(stderr, "Invalid input\n");
        exit(EXIT_FAILURE);
    }

    return value;
}

/******************
 * Data structures
******************/

/*********
 * eNodes
*********/
eNode createENode(edge item, eNode next) {

    eNode new = (eNode)malloc(sizeof(struct eNode));
    assert(new);

    new->item = item;
    new->next = next;

    return new;
}

void freeENodes(eNode head) {
    
    eNode next;

    while (head != NULL) {
        next = head->next;
        free(head->item);
        free(head);
        head = next;
    }
}

/********
 * Edges
********/
edge createEdge(vertex from, vertex to, int capacity, int flow, edge dual) {

    edge new = (edge)malloc(sizeof(struct edge));
    assert(new);

    new->to = to;
    new->from = from;
    new->capacity = capacity;
    new->flow = flow;
    new->dual = dual;

    return new;
}

void addEdge(edge link) {

    eNode new = createENode(link, NULL);

    if (link->from->adjHead == NULL) {
        link->from->adjHead = new;
        link->from->adjTail = new;
        return;
    }

    link->from->adjTail->next = new;
    link->from->adjTail = new;
}

void freeEdges(int nVerts) {

    int i;

    for (i = 0; i < nVerts; i++) {
        freeENodes(JOINT(i)->adjHead);
    }
}

/*********
 * vNodes
*********/
vNode createVNode(vertex item, vNode next) {

    vNode new = (vNode)malloc(sizeof(struct vNode));
    assert(new);

    new->item = item;
    new->next = next;

    return new;
}

void freeVNodes(vNode head) {

    vNode next;

    while (head != NULL) {
        next = head->next;
        free(head);
        head = next;
    }
}

/********
 * vList
********/
void vListInit(int nVerts) {

    int i;
    vList = (vertex)malloc(sizeof(struct vertex) * nVerts);
    assert(vList);

    /* Inicializing vList */
    for (i = 0 ; i < nVerts; i++) {
        vList[i].adjHead = NULL;
        vList[i].adjTail = NULL;
        vList[i].excess = 0;
        vList[i].height = 0;
    }
}

/********
 * Queue
********/
void addToQueue(vertex item) {

    vNode new = createVNode(item, NULL);

    if (queue.head == NULL) {
        queue.head = new;
        queue.tail = new;
        return;
    }

    queue.tail->next = new;
    queue.tail = new;
}

void popQueue() {

    vNode head = queue.head;
    queue.head = head->next;

    free(head);
}

/********
 * Stack
********/
void stackInit(int nHeights) {

    int i;
    stack.nHeights = nHeights;

    stack.list = (vNode *)malloc(sizeof(vNode) * (nHeights + 1));
    assert(stack.list);
    
    /* Inicializing stack.list */
    for (i = 0; i <= nHeights; stack.list[i++] = NULL);
}

void addToStack(vertex item) {

    stack.list[item->height] = createVNode(item, stack.list[item->height]);
}

void stackClimb(vertex item) {

    vNode node = stack.list[item->height];

    if (node->item != item) {

        /* Searching item on stack */
        vNode prev = node;
        while (prev->next->item != item) {
            prev = prev->next;
        }
        node = prev->next;

        /* Removing item of the stack */
        prev->next = node->next;

    } else {
        /* Removing item of the stack */
        stack.list[item->height] = node->next;
    }

    /* Update item height */
    item->height++;
    /* Merge */
    node->next = stack.list[item->height];
    stack.list[item->height] = node;
}

void stackSweep(int height) {

    int i;
    vNode node;

    for (i = height; stack.list[i] != NULL; i++) {

        for (node = stack.list[i]; node->next != NULL; node = node->next) {
            /* Update item height */
            node->item->height = stack.nHeights + 1;
        }

        /* Update item height */
        node->item->height = stack.nHeights + 1;
        /* Merge */
        node->next = stack.list[stack.nHeights];
        stack.list[stack.nHeights] = stack.list[i];
        /* Remove list */
        stack.list[i] = NULL;
    }
}

void freeStack() {

    int i;

     for (i = 0; i <= stack.nHeights; i++) {
        freeVNodes(stack.list[i]);
    }

    free(stack.list);
}

/***************
 * Push-Relable
***************/
void preflow(vertex src) {

    eNode adjNode;
    edge link;

    for (adjNode = src->adjHead; adjNode != NULL; adjNode = adjNode->next) {

        link = adjNode->item;

        relax(link, link->capacity);

        if (link->to != JOINT(0)) {
            addToQueue(link->to);
        }
    }
}

void relax(edge link, int flow) {

    link->from->excess -= flow;
    link->flow += flow;
    link->to->excess += flow;

    if (link->dual != NULL) {
        link->dual->flow -= flow;
        return;
    }

    /* Creating dual adjacency */
    link->dual = createEdge(link->to, link->from, 0, -flow, link);
    addEdge(link->dual);
}

void relable(vertex item) {

    if (item->height < stack.nHeights) {

        if (stack.list[item->height]->next == NULL) {
            stackSweep(item->height);

        } else {
            stackClimb(item);
        }

    } else {
        /* Update item height */
        item->height++;
    }
}

void pushRelable() {

    int flow;
    vertex vItem;
    edge eItem;
    eNode adjNode;

    preflow(JOINT(1));
    while (queue.head != NULL) {

        vItem = queue.head->item;

        for (adjNode = vItem->adjHead; adjNode != NULL; adjNode = adjNode->next) {

            eItem = adjNode->item;

            if (vItem->height != eItem->to->height + 1) continue;

            flow = MIN(vItem->excess, eItem->capacity - eItem->flow);

            if (flow > 0) {

                relax(eItem, flow);

                if (eItem->to->excess == flow && eItem->to != JOINT(0)) {
                    addToQueue(eItem->to);
                }

                if (vItem->excess == 0) break;
            }
        }

        if (vItem->excess > 0) {
            relable(vItem);
            continue;
        }

        /* Next element */
        popQueue();
    }
}

/*****************************
 * qsort comparison functions
*****************************/
int cmpTo (const void *x, const void *y) {

   return (int)((*((edge *)x))->to - (*((edge *)y))->to);
}

int cmpFrom (const void *x, const void *y) {

   return (int)((*((edge *)x))->from - (*((edge *)y))->from);
}
