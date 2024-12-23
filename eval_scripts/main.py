from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns
from mpl_toolkits.mplot3d import Axes3D


# Compare three evaluations (light version, no aggregation)
def compare_std_plot_light(
    plot_title, x_param, y_param, eval_path1, eval_path2, eval_path3, label1, label2, label3, save_path
):
    plt.clf()
    plt.figure(figsize=(16, 9))
    d1 = pd.read_csv(eval_path1)
    d2 = pd.read_csv(eval_path2)
    d3 = pd.read_csv(eval_path3)

    # Plot the values
    plt.plot(
        d1[x_param],
        d1[y_param],
        linestyle="-",
        marker="o",
        color="dimgray",
        label=f"{y_param}, {label1}",
    )
    plt.plot(
        d2[x_param],
        d2[y_param],
        linestyle="-",
        marker="o",
        color="royalblue",
        label=f"{y_param}, {label2}",
    )
    plt.plot(
        d3[x_param],
        d3[y_param],
        linestyle="-",
        marker="o",
        color="deepskyblue",
        label=f"{y_param}, {label3}",
    )

    # Add labels and title
    plt.title(plot_title)
    plt.xlabel(x_param)
    plt.ylabel(y_param)
    plt.legend()

    # Save the plot
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


# Compare three evaluations by aggregating by x_param using the mean value of y_param
def compare_std_plot_agg(
    plot_title, x_param, y_param, eval_path1, eval_path2, eval_path3, label1, label2, label3, save_path
):
    plt.clf()
    plt.figure(figsize=(16, 9))
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
        color="dimgray",
        label=f"Mean {y_param}, {label1}",
    )
    plt.plot(
        grouped2[x_param],
        grouped2[y_param],
        linestyle="-",
        marker="o",
        color="royalblue",
        label=f"Mean {y_param}, {label2}",
    )
    plt.plot(
        grouped3[x_param],
        grouped3[y_param],
        linestyle="-",
        marker="o",
        color="deepskyblue",
        label=f"Mean {y_param}, {label3}",
    )

    # Add labels and title
    plt.title(plot_title)
    plt.xlabel(x_param)
    plt.ylabel(y_param)
    plt.legend()

    # Save the plot
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


# Aggregate the scatter plot of each x_param/y_param pair by x_param using the mean value of y_param
def std_plot_agg(plot_title, x_param, y_param, eval_path, save_path):
    plt.clf()
    plt.figure(figsize=(16, 9))
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
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


# Scatter plot each x_param/y_param pair
def std_plot(plot_title, x_param, y_param, eval_path, save_path):
    plt.clf()
    plt.figure(figsize=(16, 9))
    d = pd.read_csv(eval_path)

    plt.plot(d[x_param], d[y_param], linestyle="none", marker="o")
    plt.title(plot_title)
    plt.xlabel(x_param)
    plt.ylabel(y_param)

    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


# Heatmap visualizing FPS for combinations of two parameters
def heatmap_plot(plot_title, row_param, col_param, eval_path, save_path):
    plt.clf()
    plt.figure(figsize=(16, 9))
    d = pd.read_csv(eval_path)

    # Pivot table for Isovalue and StepSizeRange
    pivot = d.pivot_table(values="FramesPerSecond", index=row_param, columns=col_param, aggfunc="mean")

    # Plot the heatmap
    sns.heatmap(pivot, annot=True, cmap="coolwarm", fmt=".2f", cbar_kws={"label": "Frames Per Second"})
    plt.title(plot_title)
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


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
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


# Plot a 3D graph with an additional color component
def cubic_plot(plot_title, x_param, y_param, z_param, color_param, eval_path, save_path):
    plt.clf()
    d = pd.read_csv(eval_path)

    fig = plt.figure(figsize=(16, 9))
    ax = fig.add_subplot(projection="3d")

    # Subset data
    x = d[x_param]
    y = d[y_param]
    z = d[z_param]
    c = d[color_param]  # Color based on StepSizeSmall

    # 3D scatter plot
    sc = ax.scatter(x, y, z, c=c, cmap="viridis", s=50)
    cbar = plt.colorbar(sc, label=color_param)
    cbar.set_ticks(y.unique())

    # Set axis labels
    ax.set_xlabel(x_param)
    ax.set_ylabel(y_param)
    ax.set_zlabel(z_param)
    plt.title(plot_title)
    plt.tight_layout()
    plt.savefig(save_path, bbox_inches="tight")


def main():
    filename_prefix = "gtx1070"
    filename_dataset = "bonsai"
    eval_grade = "full"
    eval_path_baseline = (
        "eval_data/" + filename_prefix + "/" + filename_dataset + "_eval_" + eval_grade + "_iso/eval.csv"
    )
    eval_path_oct1 = (
        "eval_data/" + filename_prefix + "/" + filename_dataset + "_eval_" + eval_grade + "_ess_d1/eval.csv"
    )
    eval_path_oct2 = (
        "eval_data/" + filename_prefix + "/" + filename_dataset + "_eval_" + eval_grade + "_ess_d2/eval.csv"
    )

    cubic_plot(
        "Bonsai, baseline",
        "Isovalue",
        "StepSizeLarge",
        "FramesPerSecond",
        "StepSizeLarge",
        eval_path_baseline,
        "plots/" + filename_prefix + "_" + filename_dataset + "_cubic_baseline.png",
    )
    cubic_plot(
        "Bonsai, octree depth 1",
        "Isovalue",
        "StepSizeLarge",
        "FramesPerSecond",
        "StepSizeLarge",
        eval_path_oct1,
        "plots/" + filename_prefix + "_" + filename_dataset + "_cubic_octree_d1.png",
    )
    cubic_plot(
        "Bonsai, octree depth 2",
        "Isovalue",
        "StepSizeLarge",
        "FramesPerSecond",
        "StepSizeLarge",
        eval_path_oct2,
        "plots/" + filename_prefix + "_" + filename_dataset + "_cubic_octree_d2.png",
    )
    heatmap_plot(
        "Bonsai, baseline",
        "StepSizeLarge",
        "Isovalue",
        eval_path_baseline,
        "plots/" + filename_prefix + "_" + filename_dataset + "_heatmap_baseline.png",
    )
    heatmap_plot(
        "Bonsai, octree depth 1",
        "StepSizeLarge",
        "Isovalue",
        eval_path_oct1,
        "plots/" + filename_prefix + "_" + filename_dataset + "_heatmap_octree_d1.png",
    )
    heatmap_plot(
        "Bonsai, octree depth 2",
        "StepSizeLarge",
        "Isovalue",
        eval_path_oct2,
        "plots/" + filename_prefix + "_" + filename_dataset + "_heatmap_octree_d2.png",
    )
    compare_std_plot_agg(
        "Bonsai - Frames per Second across Isovalues",
        "Isovalue",
        "FramesPerSecond",
        eval_path_baseline,
        eval_path_oct1,
        eval_path_oct2,
        "baseline",
        "octree depth 1",
        "octree depth 2",
        "plots/" + filename_prefix + "_" + filename_dataset + "_compare.png",
    )

    # compare_std_plot_light(
    #     filename_dataset.capitalize() + " - Frames per Second across Isovalues",
    #     "Isovalue",
    #     "FramesPerSecond",
    #     eval_path_baseline,
    #     eval_path_oct1,
    #     eval_path_oct2,
    #     "baseline",
    #     "octree depth 1",
    #     "octree depth 2",
    #     "plots/" + filename_prefix + "_" + filename_dataset + "_compare.png",
    # )


if __name__ == "__main__":
    main()
