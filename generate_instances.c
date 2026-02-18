/********************************************************************
 * generate_instances.c
 *
 * Usage:
 *   ./generate_instances <minN> <maxN> <m> <maxDim> <outputFile>
 * 
 * Description:
 *   - minN, maxN: range of chain lengths (number of matrices)
 *   - m: number of random instances to generate for each n
 *   - maxDim: maximum dimension size for any matrix dimension
 *   - outputFile: file to save the instances
 *
 * Format of the outputFile:
 *   1) First line: total number of instances, T
 *   2) Then for each instance i:
 *       - one line with n (the number of matrices)
 *       - one line with n+1 integers: d0 d1 ... dn
 *
 * Example:
 *   If minN=2, maxN=3, m=2, we'll produce 2 instances for n=2, 2 for n=3,
 *   total T = 4. Each instance has n+1 random dimensions from 1..maxDim.
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <minN> <maxN> <m> <maxDim> <outputFile>\n", argv[0]);
        return 1;
    }

    int minN = atoi(argv[1]);
    int maxN = atoi(argv[2]);
    int m    = atoi(argv[3]);
    int maxDim = atoi(argv[4]);
    const char *outputFile = argv[5];

    // Compute total number of instances
    int totalInstances = 0;
    for (int n = minN; n <= maxN; n++) {
        totalInstances += m;
    }

    FILE *fp = fopen(outputFile, "w");
    if (!fp) {
        fprintf(stderr, "Cannot open file %s for writing.\n", outputFile);
        return 1;
    }

    srand((unsigned)time(NULL));

    // 1) Print total number of instances
    fprintf(fp, "%d\n", totalInstances);

    // 2) For each n in [minN..maxN], generate m instances
    for (int n = minN; n <= maxN; n++) {
        for (int j = 0; j < m; j++) {
            // Write n
            fprintf(fp, "%d\n", n);

            // Generate n+1 random dimensions in [1..maxDim]
            for (int k = 0; k < n+1; k++) {
                int dim = 1 + rand() % maxDim;
                fprintf(fp, "%d", dim);
                if (k < n) fprintf(fp, " ");
            }
            fprintf(fp, "\n");
        }
    }

    fclose(fp);
    printf("Generated %d instances across n in [%d..%d], each repeated %d times.\n",
           totalInstances, minN, maxN, m);
    printf("Saved to file: %s\n", outputFile);
    return 0;
}
