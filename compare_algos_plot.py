#!/usr/bin/env python3

"""
compare_algos_plot.py

Usage:
  python compare_algos_plot.py results_backtracking.txt results_dp.txt ...

Description:
  Reads multiple results files, each containing lines of the form:
    n time cost parenthesization
  where 'n' is the number of matrices, 'time' is the solve time for that instance,
  'cost' is the best cost, and 'parenthesization' is a string representation of the
  solution.

  For each file:
    - Parse all (n, time) pairs
    - Group times by n
    - Compute the average solve time for each n
    - Plot them on a shared graph for comparison.
  The legend entry for each curve is derived from the filename (minus its extension).

Example:
  python compare_algos_plot.py results_backtracking.txt results_dp.txt
"""

import sys
import statistics
import matplotlib.pyplot as plt 
from collections import defaultdict
import os

def main():
    if len(sys.argv) < 2:
        print("Usage: python compare_algos_plot.py <resultsFile1> [<resultsFile2> ...]")
        sys.exit(1)
    
    # Prepare a figure
    plt.figure()
    
    # Iterate over all result files passed as arguments
    for results_file in sys.argv[1:]:
        algo_name = os.path.splitext(os.path.basename(results_file))[0]
        # e.g., "results_backtracking.txt" -> "results_backtracking"
        
        # Read lines of the form "n time cost parenthesization"
        times_by_n = defaultdict(list)
        
        with open(results_file, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                parts = line.split(maxsplit=3)
                if len(parts) < 3:
                    continue
                n_str, time_str, cost_str = parts[0], parts[1], parts[2]
                
                try:
                    n = int(n_str)
                    t = float(time_str)
                except ValueError:
                    # ignore malformed lines
                    continue
                
                # Accumulate times for each n
                times_by_n[n].append(t)
        
        # If no valid data was read from file, skip
        if not times_by_n:
            print(f"No valid data found in {results_file}, skipping.")
            continue
        
        # Sort by n and compute average time
        Ns = sorted(times_by_n.keys())
        avg_times = []
        
        for n in Ns:
            arr = times_by_n[n]
            mu = statistics.mean(arr)
            avg_times.append(mu)
        
        # Plot on the same figure
        plt.plot(Ns, avg_times, marker='o', label=algo_name)
    
    # Finalize the plot
    plt.title("Comparison of Solve Times Across Algorithms")
    plt.xlabel("n (number of matrices)")
    plt.ylabel("Average Time (seconds)")
    plt.grid(True)
    plt.legend()
    
    # Save and show
    plt.savefig("compare_algos_plot.png")
    print("Comparison plot saved to compare_algos_plot.png")
    plt.show()

if __name__ == "__main__":
    main()