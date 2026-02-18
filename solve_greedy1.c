#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAXN 500
#define MAXSTACK 200
#define MAXREPR 1200

typedef struct {
    int rows, cols;
    long long cost;    // 64-bit for safety
    char repr[MAXREPR];
} Node;

/* Best solution for current instance */
static long long bestCost;
static char bestTree[MAXREPR];

/* We'll store the current dimensions in d[] for the chain */
static int d[MAXN+1];
static int n; // number of matrices

/* -------------- Leaf creation -------------- */
static Node makeLeaf(int i)
{
    Node leaf;
    leaf.rows = d[i-1];
    leaf.cols = d[i];
    leaf.cost = 0LL;
    snprintf(leaf.repr, sizeof(leaf.repr), "M%d", i);
    return leaf;
}

/* -------------- Combine two sub-products -------------- */
static Node combineNodes(const Node *A, const Node *B)
{
    Node C;
    long long combineCost = (long long)A->rows * A->cols * B->cols;
    C.rows = A->rows;
    C.cols = B->cols;
    C.cost = A->cost + B->cost + combineCost;
    snprintf(C.repr, sizeof(C.repr), "(%s %s)", A->repr, B->repr);
    return C;
}
/* -------------- GREEDY1 -------------- */
static Node greedy1(Node nodes[],int n)
{
    int val=0;
    long long min_cost=LLONG_MAX;
    Node bestPair;
    if(n==1){
        return nodes[0];
    }
    for(int i=0;i<n-1;i++){
        Node C=combineNodes(&nodes[i],&nodes[i+1]);
        if(C.cost<=min_cost){
            min_cost=C.cost;
            val=i;
            bestPair=C;
        }
    }
    nodes[val]=bestPair;
    for (int j = val+1; j < n-1; j++) {
        nodes[j]=nodes[j+1];
    }
    return greedy1(nodes,n-1);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <inputFile> <outputFile>\n", argv[0]);
        return 1;
    }
    const char *inFile  = argv[1];
    const char *outFile = argv[2];

    FILE *fin = fopen(inFile, "r");
    if (!fin) {
        fprintf(stderr, "Cannot open input file: %s\n", inFile);
        return 1;
    }
    FILE *fout = fopen(outFile, "w");
    if (!fout) {
        fprintf(stderr, "Cannot open output file: %s\n", outFile);
        fclose(fin);
        return 1;
    }

    // Read total number of instances T
    int T;
    fscanf(fin, "%d", &T);

    // We'll solve each instance, measure time, and record results.
    for (int inst = 0; inst < T; inst++) {
        // read n
        fscanf(fin, "%d", &n);
        // read n+1 dimensions
        for (int i = 0; i < n+1; i++) {
            fscanf(fin, "%d", &d[i]);
        }

        // Reset global best
        bestCost = LLONG_MAX;
        strcpy(bestTree, "");

        // measure time
        clock_t start = clock();
        // run glouton
        Node nodes[MAXN];
        for (int i = 0; i < n; i++) {
            nodes[i] = makeLeaf(i+1);
        }
        Node Sol=greedy1(nodes,n);
        bestCost=Sol.cost;
        strcpy(bestTree,Sol.repr);

        clock_t end = clock();
        double timeSec = (double)(end - start) / CLOCKS_PER_SEC;

        // Write result: n time bestCost bestTree
        fprintf(fout, "%d %.6f %lld %s\n", n, timeSec, bestCost, bestTree);
        fflush(fout);

        // Print progress
        printf("[Progress] Solved instance %d / %d : n=%d, time=%.6f, bestCost=%lld\n",
               inst+1, T, n, timeSec, bestCost);
    }

    fclose(fin);
    fclose(fout);
    printf("Processed %d instances. Results saved to %s\n", T, outFile);

    return 0;
}
