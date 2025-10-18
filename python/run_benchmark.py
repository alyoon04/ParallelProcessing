#!/usr/bin/env python3
"""
Automated benchmark runner for Parallel Processing Library
Executes C++ benchmarks and collects results
"""

import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os
import sys
import time

def run_benchmark_executable(executable_path, thread_counts):
    """Run the C++ benchmark executable with specified thread counts"""

    if not os.path.exists(executable_path):
        print(f"Error: Benchmark executable not found at {executable_path}")
        print("Please build the project first with: cmake --build build")
        sys.exit(1)

    print(f"Running benchmark: {executable_path}")
    print(f"Thread counts: {thread_counts}")
    print("-" * 60)

    # Convert thread counts to command line arguments
    args = [executable_path] + [str(t) for t in thread_counts]

    try:
        result = subprocess.run(args, capture_output=True, text=True, check=True)
        print(result.stdout)

        if result.stderr:
            print("Errors/Warnings:", file=sys.stderr)
            print(result.stderr, file=sys.stderr)

        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Benchmark failed with error code {e.returncode}")
        print(e.stdout)
        print(e.stderr, file=sys.stderr)
        sys.exit(1)

def parse_benchmark_output(output):
    """Parse benchmark output and return structured data"""
    lines = output.strip().split('\n')

    data = []
    for line in lines:
        # Look for data lines (skip headers and separators)
        if 'Map-Reduce' in line or 'Parallel Sort' in line or 'Aggregation' in line:
            parts = line.split()

            # Handle multi-word operation names
            if 'Map-Reduce' in line:
                operation = 'Map-Reduce'
                values = parts[1:]
            elif 'Parallel Sort' in line:
                operation = 'Parallel Sort'
                values = parts[2:]
            elif 'Aggregation' in line:
                operation = 'Aggregation'
                values = parts[1:]
            else:
                continue

            try:
                data_size = int(values[0])
                num_threads = int(values[1])
                time_ms = float(values[2])
                speedup = float(values[3])

                data.append({
                    'operation': operation,
                    'data_size': data_size,
                    'num_threads': num_threads,
                    'time_ms': time_ms,
                    'speedup': speedup
                })
            except (ValueError, IndexError):
                continue

    return pd.DataFrame(data)

def save_results(df, output_file):
    """Save benchmark results to CSV"""
    df.to_csv(output_file, index=False)
    print(f"\nResults saved to: {output_file}")

def quick_plot(df, output_file):
    """Generate a quick speedup plot"""
    operations = df['operation'].unique()

    plt.figure(figsize=(10, 6))

    for operation in operations:
        op_data = df[df['operation'] == operation]
        plt.plot(op_data['num_threads'], op_data['speedup'],
                marker='o', linewidth=2, markersize=8, label=operation)

    # Ideal speedup line
    max_threads = df['num_threads'].max()
    plt.plot([1, max_threads], [1, max_threads],
             'k--', linewidth=1, alpha=0.5, label='Ideal Linear')

    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup')
    plt.title('Parallel Processing - Speedup vs Thread Count')
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(df['num_threads'].unique())

    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Quick plot saved to: {output_file}")
    plt.close()

def main():
    # Default thread counts to test
    thread_counts = [1, 2, 4, 8]

    # Determine build directory and executable path
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)

    # Try to find the benchmark executable
    possible_paths = [
        os.path.join(project_dir, 'build', 'parallel_benchmark'),
        os.path.join(project_dir, 'parallel_benchmark'),
        './parallel_benchmark',
        './build/parallel_benchmark'
    ]

    executable_path = None
    for path in possible_paths:
        if os.path.exists(path):
            executable_path = path
            break

    if not executable_path:
        print("Error: Could not find parallel_benchmark executable")
        print("Searched in:", possible_paths)
        print("\nPlease build the project first:")
        print("  mkdir build && cd build")
        print("  cmake ..")
        print("  make")
        sys.exit(1)

    # Run benchmark
    start_time = time.time()
    output = run_benchmark_executable(executable_path, thread_counts)
    elapsed_time = time.time() - start_time

    print(f"\nBenchmark completed in {elapsed_time:.2f} seconds")

    # Parse results
    df = parse_benchmark_output(output)

    if df.empty:
        print("Warning: No benchmark data could be parsed")
        sys.exit(1)

    # Save results
    results_dir = os.path.join(project_dir, 'results')
    os.makedirs(results_dir, exist_ok=True)

    csv_file = os.path.join(results_dir, 'benchmark_results.csv')
    plot_file = os.path.join(results_dir, 'speedup_quick.png')

    save_results(df, csv_file)
    quick_plot(df, plot_file)

    print("\n" + "="*60)
    print("Benchmark complete!")
    print(f"CSV results: {csv_file}")
    print(f"Quick plot:  {plot_file}")
    print("\nTo generate detailed plots, run:")
    print(f"  python {os.path.join(script_dir, 'plot_benchmark.py')} {csv_file} {results_dir}")
    print("="*60)

if __name__ == '__main__':
    main()
