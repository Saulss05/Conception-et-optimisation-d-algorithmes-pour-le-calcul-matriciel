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
int split[MAXN+1][MAXN+1];


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

/* -------------- Build parenthesization -------------- */
static Node build_parentheses(int i,int j){
    Node A;
    if(i==j){
        A=makeLeaf(i);
    }
    else{
       int k=split[i][j];
       Node left=build_parentheses(i,k);
       Node right=build_parentheses(k+1,j);
       A=combineNodes(&left,&right);
    }
    return A;
}

long long greedy2(int i,int j){
    if(i==j){
        return 0;
    }

    long long min_blind_cost=LLONG_MAX;
    int val_split;

    for(int k=i;k<j;k++){
        long long cost=d[i-1]*d[k]*d[j];
        if(cost<=min_blind_cost){
            min_blind_cost=cost;
            val_split=k;
        }
    }
    split[i][j]=val_split;

    long long left=greedy2(i,val_split);
    long long right=greedy2(val_split+1,j);

    return left+right+d[i-1]*d[val_split]*d[j];
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

        // run dynamic programing
        //greedy2();
        greedy2(1,n);
        Node Sol = build_parentheses(1, n);
        bestCost = Sol.cost;
        strcpy(bestTree, Sol.repr);

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
