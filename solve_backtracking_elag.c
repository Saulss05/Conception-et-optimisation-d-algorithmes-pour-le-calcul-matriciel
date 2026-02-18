/********************************************************************
 * solve_backtracking.c
 *
 * Usage:
 *   ./solve_backtracking <inputFile> <outputFile>
 *
 * Description:
 *   Reads a set of matrix-chain instances from <inputFile>,
 *   enumerates all parenthesizations using a stack-based backtracking
 *   approach, finds the optimal cost and parenthesization, and
 *   writes results to <outputFile> with lines of the form:
 *
 *       n timeSeconds bestCost bestParenthesization
 *
 *   Prints progress on the number of instances solved.
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define MAXN 500
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
static long long bestCost;
static char bestTree[MAXREPR];

/* We'll store the current dimensions in d[] for the chain */
static int d[MAXN+1];
static int n; // number of matrices

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
        if(C.cost<=bestCost){
            
            push(C);
            backtrack(i);
            pop(); // undo REDUCE
        }
            // restore
            push(A);
            push(B);
    }

    /* Case 3: check if complete parse => i>n & top==1 */
    if (i > n && top == 1) {
        Node candidate = stack[top-1];
        if (candidate.cost < bestCost) {
            bestCost = candidate.cost;
            strcpy(bestTree, candidate.repr);
        }
    }
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

        // Reset stack
        top = 0;
        // run backtracking
        backtrack(1);

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
