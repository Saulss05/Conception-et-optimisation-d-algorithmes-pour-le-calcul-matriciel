
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAXN 100
#define MAXSTACK 200
#define MAXREPR 500

typedef struct {
    int rows, cols;
    long long cost;    // 64-bit for safety
    char repr[MAXREPR];
} Node;

/* Global stack */
static Node stack[MAXSTACK];
static int top = 0; // stack pointer

/* Best solution for current instance */
static long long bestCostGreedy1;
static char bestTreeGreedy1[MAXREPR];
static long long bestCostGreedy2;
static char bestTreeGreedy2[MAXREPR];
static long long bestCostGreedy3;
static char bestTreeGreedy3[MAXREPR];

/* We'll store the current dimensions in d[] for the chain */
static int d[MAXN+1];
static int n; // number of matrices
long long dp[MAXN+1][MAXN+1];
int split[MAXN+1][MAXN+1];
long long costTab[MAXN];
char reprs[MAXN][MAXREPR];
int nb_solutions=0;

/* -------------- Stack operations -------------- */
static void push(Node nd)
{
    if (top >= MAXSTACK) {
        fprintf(stderr, "Stack overflow.\n");
        exit(1);
    }
    stack[top++] = nd;
}

static Node pop(void)
{
    if (top <= 0) {
        fprintf(stderr, "Stack underflow.\n");
        exit(1);
    }
    return stack[--top];
}

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

void insertion(long long *tab, char reprs[][MAXREPR], int n) {
    for (int i = 1; i < n; ++i) {
        long long key = tab[i];
        char keyRepr[MAXREPR];
        strcpy(keyRepr, reprs[i]);
        int j = i - 1;
        while (j >= 0 && tab[j] > key) {
            tab[j + 1] = tab[j];
            strcpy(reprs[j + 1], reprs[j]);
            j--;
        }
        tab[j + 1] = key;
        strcpy(reprs[j + 1], keyRepr);
    }
}

int min(int a,int b){
    if(a<=b){
        return a;
    }
    return b;
}

/* -------------- Backtracking -------------- */
static void backtrack(int i)
{
    /* Case 1: SHIFT if i <= n */
    if (i <= n) {
        Node leaf = makeLeaf(i);
        push(leaf);
        backtrack(i + 1);
        pop(); // undo SHIFT
    }

    /* Case 2: REDUCE if top >= 2 */
    if (top >= 2) {
        Node B = pop();
        Node A = pop();
        Node C = combineNodes(&A, &B);

        push(C);
        backtrack(i);
        pop(); // undo REDUCE

        // restore
        push(A);
        push(B);
    }

    /* Case 3: check if complete parse => i>n & top==1 */
    if (i > n && top == 1) {
        long long localCost;
        char localTree[MAXREPR];
        Node candidate = stack[top-1];
        localCost = candidate.cost;
        strcpy(localTree, candidate.repr);
        costTab[nb_solutions]=localCost;
        strcpy(reprs[nb_solutions],localTree);
        nb_solutions++;
    }
}

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
        if(d[i-1]*d[k]*d[j]<=min_blind_cost){
            min_blind_cost=d[i-1]*d[k]*d[j];
            val_split=k;
        }
    }
    split[i][j]=val_split;

    long long left=greedy2(i,val_split);
    long long right=greedy2(val_split+1,j);

    return left+right+d[i-1]*d[val_split]*d[j];
}


static Node greedy3(Node nodes[],int n)
{
    int val=0;
    long long min_cost=LLONG_MAX;
    Node bestTrio;
    if(n==1){
        return nodes[0];
    }
    if(n==2){
        Node C=combineNodes(&nodes[0],&nodes[1]);
        return C;
    }
    for(int i=0;i<n-2;i++){
        Node cn1=combineNodes(&nodes[i],&nodes[i+1]);
        Node cn2=combineNodes(&nodes[i+1],&nodes[i+2]);
        Node B=combineNodes(&cn1,&nodes[i+2]);
        Node D=combineNodes(&nodes[i],&cn2);
        Node C;
        if(min(B.cost,D.cost)==B.cost){
            C=B;
        }
        else{
            C=D;
        }
        if(C.cost<=min_cost){
            min_cost=C.cost;
            val=i;
            bestTrio=C;
        }
    }
    nodes[val]=bestTrio;
    for (int j = val+1; j < n-2; j++) {
        nodes[j+1]=nodes[j+2];
        nodes[j]=nodes[j+1];
    }
    return greedy3(nodes,n-2);
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
        bestCostGreedy1 = LLONG_MAX;
        strcpy(bestTreeGreedy1, "");

        // measure time
        clock_t start = clock();

        // Reset stack
        top = 0;
        // run backtracking
        nb_solutions=0;
        backtrack(1);

        clock_t end = clock();
        double timeSec = (double)(end - start) / CLOCKS_PER_SEC;

        Node nodesGreedy1[MAXN];
        Node nodesGreedy3[MAXN];
        for (int i = 0; i < n; i++) {
            nodesGreedy1[i] = makeLeaf(i+1);
            nodesGreedy3[i] = makeLeaf(i+1);
        }

        Node SolGreedy1=greedy1(nodesGreedy1,n);
        Node SolGreedy3=greedy3(nodesGreedy3,n);

        bestCostGreedy1=SolGreedy1.cost;
        strcpy(bestTreeGreedy1,SolGreedy1.repr);

        bestCostGreedy3=SolGreedy3.cost;
        strcpy(bestTreeGreedy3,SolGreedy3.repr);

        greedy2(1,n);
        Node SolGreedy2 = build_parentheses(1, n);
        bestCostGreedy2 = SolGreedy2.cost;
        strcpy(bestTreeGreedy2, SolGreedy2.repr);

        // Write result: n time bestCost bestTree
        for(int i=0;i<nb_solutions;i++){
            fprintf(fout, "%d %.6f %lld %s\n", n, timeSec, costTab[i], reprs[i]);
            fflush(fout);
            // Print progress
            printf("[Progress] Solved instance %d / %d : n=%d, time=%.6f, bestCost=%lld\n",
                   inst+1, T, n, timeSec, costTab[i]);
        }
    }

    fprintf(fout,"\n\n");
    insertion(costTab,reprs,nb_solutions);
    int cpt;
    for(cpt=0;cpt<nb_solutions;cpt++){
        fprintf(fout, "N°%d %d %lld %s\n",cpt+1, n, costTab[cpt], reprs[cpt]);
        fflush(fout);
    }
    int rang_greedy1=0;
    int ecart_rang_greedy1=0;
    int rang_greedy2=0;
    int ecart_rang_greedy2=0;
    int rang_greedy3=0;
    int ecart_rang_greedy3=0;
    for(int k=0;k<=nb_solutions;k++){
        if(bestCostGreedy1==costTab[k]){
            rang_greedy1=k+1;
            ecart_rang_greedy1=rang_greedy1-1;
        }
        if(bestCostGreedy2==costTab[k]){
            rang_greedy2=k+1;
            ecart_rang_greedy2=rang_greedy2-1;
        }
        if(bestCostGreedy3==costTab[k]){
            rang_greedy3=k+1;
            ecart_rang_greedy3=rang_greedy3-1;
        }
    }
    double rapportGreedy1=(double)rang_greedy1/cpt*100;
    double rapportGreedy2=(double)rang_greedy2/cpt*100;
    double rapportGreedy3=(double)rang_greedy3/cpt*100;
    fprintf(fout, "Arbre Greedy1: %s, Coût Greedy1: %lld, Rang greedy1: %d/%d, ecart de rang greedy1:  %d, rapport au nombre de solutions: %g %%\n",bestTreeGreedy1,bestCostGreedy1,  rang_greedy1, cpt, ecart_rang_greedy1,rapportGreedy1);
    fflush(fout);
    fprintf(fout, "Arbre Greedy2: %s, Coût Greedy2: %lld, Rang greedy2: %d/%d, ecart de rang greedy2:  %d, rapport au nombre de solutions: %g %% \n",bestTreeGreedy2,bestCostGreedy2, rang_greedy2, cpt, ecart_rang_greedy2,rapportGreedy2);
    fflush(fout);
    fprintf(fout, "Arbre Greedy3: %s, Coût Greedy3: %lld, Rang greedy3: %d/%d, ecart de rang greedy3:  %d, rapport au nombre de solutions: %g %% \n",bestTreeGreedy3,bestCostGreedy3, rang_greedy3, cpt, ecart_rang_greedy3,rapportGreedy3);
    fflush(fout);

    fclose(fin);
    fclose(fout);
    printf("Processed %d instances. Results saved to %s\n", T, outFile);

    return 0;
}
