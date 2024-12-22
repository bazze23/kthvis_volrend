from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns
from mpl_toolkits.mplot3d import Axes3D


# Compare three evaluations by aggregating by x_param using the mean value of y_param
def compare_std_plot_agg(
    plot_title, x_param, y_param, eval_path1, eval_path2, eval_path3, label1, label2, label3, save_path
):
    plt.clf()
    d1 = pd.read_csv(eval_path1)
    d2 = pd.read_csv(eval_path2)
    d3 = pd.read_csv(eval_path3)

    # Group by Isovalue and calculate the mean of all FramesPerSecond values
    grouped1 = d1.groupby(x_param, as_index=False)[y_param].mean()
    grouped2 = d2.groupby(x_param, as_index=False)[y_param].mean()
    grouped3 = d3.groupby(x_param, as_index=False)[y_param].mean()

    # Plot the aggregated values
    plt.plot(
        grouped1[x_param],
        grouped1[y_param],
        linestyle="-",
        marker="o",
        color="blue",
        label=f"Mean {y_param}, {label1}",
    )
    plt.plot(
        grouped2[x_param],
        grouped2[y_param],
        linestyle="-",
        marker="o",
        color="cyan",
        label=f"Mean {y_param}, {label2}",
    )
    plt.plot(
        grouped3[x_param],
        grouped3[y_param],
        linestyle="-",
        marker="o",
        color="red",
        label=f"Mean {y_param}, {label3}",
    )

    # Add labels and title
    plt.title(plot_title)
    plt.xlabel(x_param)
    plt.ylabel(y_param)
    plt.legend()

    # Save the plot
    plt.savefig(save_path)


# Aggregate the scatter plot of each x_param/y_param pair by x_param using the mean value of y_param
def std_plot_agg(plot_title, x_param, y_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    # Group by Isovalue and calculate the mean of all FramesPerSecond values
    grouped = d.groupby(x_param, as_index=False)[y_param].mean()

    # Plot the aggregated values
    plt.plot(grouped[x_param], grouped[y_param], linestyle="-", marker="o", label="Mean FPS")

    # Add labels and title
    plt.title(plot_title)
    plt.ylabel(y_param)
    plt.xlabel(x_param)
    plt.legend()

    # Save the plot
    plt.savefig(save_path)


# Scatter plot each x_param/y_param pair
def std_plot(plot_title, x_param, y_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    plt.plot(d[x_param], d[y_param], linestyle="none", marker="o")
    plt.title(plot_title)
    plt.xlabel(x_param)
    plt.ylabel(y_param)

    plt.savefig(save_path)


# Heatmap visualizing FPS for combinations of two parameters
def heatmap_plot(plot_title, row_param, col_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    # Pivot table for Isovalue and StepSizeRange
    pivot = d.pivot_table(values="FramesPerSecond", index=row_param, columns=col_param, aggfunc="mean")

    # Plot the heatmap
    plt.figure(figsize=(22, 8))
    sns.heatmap(pivot, annot=True, cmap="coolwarm", fmt=".2f", cbar_kws={"label": "Frames Per Second"})
    plt.title(plot_title)
    plt.savefig(save_path)


# Fix one parameter and plot for another varying parameter
def fixed_param_plot(plot_title, locked_param, plot_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    # Extract unique values for locked_param and prepare subplots
    locked_vals = d[locked_param].unique()
    fig, axes = plt.subplots(
        len(locked_vals), 1, figsize=(8, len(locked_vals) * 3), sharex=True, constrained_layout=True
    )
    fig.suptitle(plot_title, fontsize=16)

    # Subplot for each locked_param
    for i, locked_val in enumerate(locked_vals):
        ax = axes[i]
        subset = d[d[locked_param] == locked_val]

        # Aggregate by Isovalue based on mean FPS, for each unique plot_param
        for plot_val in subset[plot_param].unique():
            filtered = subset[subset[plot_param] == plot_val]
            aggregated = filtered.groupby("Isovalue", as_index=False)["FramesPerSecond"].mean()
            ax.plot(aggregated["Isovalue"], aggregated["FramesPerSecond"], label=f"{plot_param}={plot_val}")

        ax.set_title(f"{locked_param} = {locked_val}")
        ax.legend()
        ax.set_ylabel("Frames Per Second")

    plt.xlabel("Isovalue")
    plt.savefig(save_path)


# Plot a 3D graph with an additional color component
def cubic_plot(plot_title, x_param, y_param, z_param, color_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection="3d")

    # Subset data
    x = d[x_param]
    y = d[y_param]
    z = d[z_param]
    c = d[color_param]  # Color based on StepSizeSmall

    # 3D scatter plot
    sc = ax.scatter(x, y, z, c=c, cmap="viridis", s=50)
    plt.colorbar(sc, label=color_param)

    # Set axis labels
    ax.set_xlabel(x_param)
    ax.set_ylabel(y_param)
    ax.set_zlabel(z_param)
    plt.title(plot_title)
    plt.savefig(save_path)


def main():
    fixed_param_plot(
        "Bonsai, octree depth 2",
        "StepSizeLarge",
        "StepSizeSmall",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "plots/jahatest.png",
    )
    cubic_plot(
        "Bonsai, octree depth 2",
        "Isovalue",
        "StepSizeLarge",
        "FramesPerSecond",
        "StepSizeSmall",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "plots/testbokna.png",
    )
    heatmap_plot(
        "Bonsai, octree depth 2",
        "StepSizeLarge",
        "Isovalue",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "plots/bonkfire.png",
    )
    std_plot(
        "Bonsai, octree depth 2",
        "Isovalue",
        "FramesPerSecond",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "plots/stdlol.png",
    )
    std_plot_agg(
        "Bonsai, octree depth 2",
        "Isovalue",
        "FramesPerSecond",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "plots/stdlolagg.png",
    )
    compare_std_plot_agg(
        "Performance comparison",
        "Isovalue",
        "FramesPerSecond",
        "eval_data/gtx1070/bonsai_eval_full_iso/eval.csv",
        "eval_data/gtx1070/bonsai_eval_full_ess_d1/eval.csv",
        "eval_data/gtx1070/bonsai_eval_full_ess_d2/eval.csv",
        "baseline",
        "octree depth 1",
        "octree depth 2",
        "plots/comparebanga.png",
    )


if __name__ == "__main__":
    main()
