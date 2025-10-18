#!/usr/bin/env python3
"""
Benchmark plotting script for Parallel Processing Library
Reads benchmark results and generates performance visualizations
"""

import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def plot_speedup(df, operation_name, output_file=None):
    """Plot speedup curves for a specific operation"""
    op_data = df[df['operation'] == operation_name]

    if op_data.empty:
        print(f"No data found for operation: {operation_name}")
        return

    plt.figure(figsize=(10, 6))

    # Plot speedup vs threads
    plt.plot(op_data['num_threads'], op_data['speedup'],
             marker='o', linewidth=2, markersize=8, label='Actual Speedup')

    # Plot ideal linear speedup
    max_threads = op_data['num_threads'].max()
    plt.plot([1, max_threads], [1, max_threads],
             'r--', linewidth=1, label='Ideal Linear Speedup')

    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Speedup', fontsize=12)
    plt.title(f'{operation_name} - Speedup vs Thread Count', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10)

    # Set x-axis to show only integer values
    plt.xticks(op_data['num_threads'].unique())

    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Saved plot to {output_file}")
    else:
        plt.show()

    plt.close()

def plot_execution_time(df, operation_name, output_file=None):
    """Plot execution time for a specific operation"""
    op_data = df[df['operation'] == operation_name]

    if op_data.empty:
        print(f"No data found for operation: {operation_name}")
        return

    plt.figure(figsize=(10, 6))

    plt.bar(op_data['num_threads'].astype(str), op_data['time_ms'],
            color='steelblue', alpha=0.8, edgecolor='black')

    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Execution Time (ms)', fontsize=12)
    plt.title(f'{operation_name} - Execution Time vs Thread Count', fontsize=14, fontweight='bold')
    plt.grid(True, axis='y', alpha=0.3)

    # Add value labels on bars
    for i, (threads, time) in enumerate(zip(op_data['num_threads'], op_data['time_ms'])):
        plt.text(i, time, f'{time:.1f}', ha='center', va='bottom', fontsize=9)

    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Saved plot to {output_file}")
    else:
        plt.show()

    plt.close()

def plot_all_operations_speedup(df, output_file=None):
    """Plot speedup curves for all operations on one graph"""
    operations = df['operation'].unique()

    plt.figure(figsize=(12, 7))

    for operation in operations:
        op_data = df[df['operation'] == operation]
        plt.plot(op_data['num_threads'], op_data['speedup'],
                marker='o', linewidth=2, markersize=8, label=operation)

    # Plot ideal linear speedup
    max_threads = df['num_threads'].max()
    plt.plot([1, max_threads], [1, max_threads],
             'k--', linewidth=1, alpha=0.5, label='Ideal Linear Speedup')

    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Speedup', fontsize=12)
    plt.title('Parallel Processing - Speedup Comparison', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10, loc='best')

    # Set x-axis to show only integer values
    plt.xticks(df['num_threads'].unique())

    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Saved plot to {output_file}")
    else:
        plt.show()

    plt.close()

def plot_efficiency(df, output_file=None):
    """Plot parallel efficiency for all operations"""
    operations = df['operation'].unique()

    plt.figure(figsize=(12, 7))

    for operation in operations:
        op_data = df[df['operation'] == operation]
        efficiency = (op_data['speedup'] / op_data['num_threads']) * 100
        plt.plot(op_data['num_threads'], efficiency,
                marker='s', linewidth=2, markersize=8, label=operation)

    # Plot ideal 100% efficiency line
    max_threads = df['num_threads'].max()
    plt.axhline(y=100, color='k', linestyle='--', linewidth=1, alpha=0.5, label='Ideal (100%)')

    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Parallel Efficiency (%)', fontsize=12)
    plt.title('Parallel Processing - Efficiency', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10, loc='best')

    # Set x-axis to show only integer values
    plt.xticks(df['num_threads'].unique())

    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Saved plot to {output_file}")
    else:
        plt.show()

    plt.close()

def print_summary_statistics(df):
    """Print summary statistics"""
    print("\n" + "="*70)
    print("BENCHMARK SUMMARY STATISTICS")
    print("="*70)

    for operation in df['operation'].unique():
        op_data = df[df['operation'] == operation]

        print(f"\n{operation}:")
        print(f"  Data Size: {op_data['data_size'].iloc[0]:,}")

        max_speedup_idx = op_data['speedup'].idxmax()
        max_speedup_row = op_data.loc[max_speedup_idx]

        print(f"  Max Speedup: {max_speedup_row['speedup']:.2f}x with {int(max_speedup_row['num_threads'])} threads")
        print(f"  Best Time: {op_data['time_ms'].min():.2f} ms")
        print(f"  Worst Time: {op_data['time_ms'].max():.2f} ms")

        # Calculate average efficiency
        avg_efficiency = ((op_data['speedup'] / op_data['num_threads']).mean() * 100)
        print(f"  Average Efficiency: {avg_efficiency:.1f}%")

    print("\n" + "="*70 + "\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_benchmark.py <results.csv> [output_directory]")
        print("Example: python plot_benchmark.py results.csv ./plots")
        sys.exit(1)

    csv_file = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else None

    if not os.path.exists(csv_file):
        print(f"Error: File '{csv_file}' not found")
        sys.exit(1)

    # Create output directory if specified
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created output directory: {output_dir}")

    # Read CSV data
    df = pd.read_csv(csv_file)

    print(f"Loaded {len(df)} benchmark results from {csv_file}")
    print(f"Operations: {', '.join(df['operation'].unique())}")

    # Print summary statistics
    print_summary_statistics(df)

    # Generate plots
    if output_dir:
        plot_all_operations_speedup(df, os.path.join(output_dir, 'speedup_comparison.png'))
        plot_efficiency(df, os.path.join(output_dir, 'efficiency.png'))

        for operation in df['operation'].unique():
            safe_name = operation.replace(' ', '_').replace('-', '_').lower()
            plot_speedup(df, operation, os.path.join(output_dir, f'{safe_name}_speedup.png'))
            plot_execution_time(df, operation, os.path.join(output_dir, f'{safe_name}_time.png'))
    else:
        # Display plots interactively
        plot_all_operations_speedup(df)
        plot_efficiency(df)

        for operation in df['operation'].unique():
            plot_speedup(df, operation)
            plot_execution_time(df, operation)

if __name__ == '__main__':
    main()
