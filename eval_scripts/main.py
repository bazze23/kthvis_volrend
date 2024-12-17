from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns
from mpl_toolkits.mplot3d import Axes3D


def std_plot():
    d = pd.read_csv("eval.csv", quotechar='"')

    plt.plot(d["Isovalue"], d["FramesPerSecond"], linestyle="none", marker="o")
    plt.title("Performance")
    plt.ylabel("Frames per Second")
    plt.xlabel("Isovalue")

    plt.savefig("plot.png")


def heatmap_plot():
    d = pd.read_csv("eval.csv")

    # Pivot table for Isovalue and StepSizeRange
    pivot = d.pivot_table(values="TimePerFrame (ms)", index="StepSizeRange", columns="Isovalue", aggfunc="mean")

    # Plot the heatmap
    plt.figure(figsize=(12, 6))
    sns.heatmap(pivot, annot=True, cmap="coolwarm", fmt=".2f")
    plt.title("Time Per Frame Heatmap (ms)")
    plt.savefig("plot.png")


def fixed_param_plot():
    d = pd.read_csv("eval.csv")

    # Define small multiples using StepSizeLarge
    step_sizes = d["StepSizeLarge"].unique()
    fig, axes = plt.subplots(len(step_sizes), 1, figsize=(8, len(step_sizes) * 3), sharex=True)

    # Plot for each StepSizeLarge
    for i, step_size in enumerate(step_sizes):
        ax = axes[i]
        subset = d[d["StepSizeLarge"] == step_size]
        for range_val in subset["StepSizeRange"].unique():
            filtered = subset[subset["StepSizeRange"] == range_val]
            ax.plot(filtered["Isovalue"], filtered["FramesPerSecond"], label=f"Range={range_val}")
        ax.set_title(f"StepSizeLarge = {step_size}")
        ax.legend()
        ax.set_ylabel("Frames Per Second")

    plt.xlabel("Isovalue")
    plt.tight_layout()
    plt.savefig("plot.png")


def cube_plot():
    d = pd.read_csv("eval.csv")

    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection="3d")

    # Subset data
    x = d["Isovalue"]
    y = d["StepSizeRange"]
    z = d["TimePerFrame (ms)"]
    c = d["StepSizeSmall"]  # Color based on StepSizeSmall

    # 3D scatter plot
    sc = ax.scatter(x, y, z, c=c, cmap="viridis", s=50)
    plt.colorbar(sc, label="StepSizeSmall")

    # Set axis labels
    ax.set_xlabel("Isovalue")
    ax.set_ylabel("StepSizeRange")
    ax.set_zlabel("TimePerFrame (ms)")
    plt.title("3D Scatter Plot of TimePerFrame")
    plt.savefig("plot.png")


def main():
    cube_plot()


if __name__ == "__main__":
    main()
