import sys
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import numpy as np
import pandas as pd


def plot_closed_polygon(ax, pts, **kwargs):
    if len(pts) == 0:
        return
    if len(pts) == 1:
        ax.scatter(pts[:, 0], pts[:, 1], **kwargs)
        return
    closed = np.vstack([pts, pts[0]])
    ax.plot(closed[:, 0], closed[:, 1], **kwargs)


def run_static_view(data):
    points = data[data["type"] == "point"][["x", "y"]].to_numpy()
    hull = data[data["type"] == "hull"][["x", "y"]].to_numpy()

    if len(hull) > 0:
        center = np.mean(hull, axis=0)
        angles = np.arctan2(hull[:, 1] - center[1], hull[:, 0] - center[0])
        hull = hull[np.argsort(angles)]

    fig, ax = plt.subplots(figsize=(8, 6))
    if len(points) > 0:
        ax.scatter(points[:, 0], points[:, 1], color="blue", s=15, label="Puntos")
    plot_closed_polygon(ax, hull, color="red", linewidth=1.5, label="Convex Hull")

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title("Visualización del Convex Hull")
    ax.grid(True)
    ax.axis("equal")
    ax.legend()
    plt.show()


def run_incremental_view(data):
    steps = sorted(data["step"].unique())
    base_points = data[(data["step"] == 0) & (data["type"] == "point")][["x", "y"]].to_numpy()

    # Límites fijos para que el gráfico no salte al cambiar de paso
    all_xy = data[["x", "y"]].to_numpy()
    xmin, ymin = all_xy.min(axis=0)
    xmax, ymax = all_xy.max(axis=0)
    dx = max(xmax - xmin, 1.0) * 0.1
    dy = max(ymax - ymin, 1.0) * 0.1

    fig, ax = plt.subplots(figsize=(9, 7))
    plt.subplots_adjust(bottom=0.18)

    state = {"idx": 0}

    def draw():
        ax.clear()
        s = steps[state["idx"]]
        step_df = data[data["step"] == s]

        # Puntos iniciales
        if len(base_points) > 0:
            point_size = 10 if len(base_points) > 200 else 25
            ax.scatter(
                base_points[:, 0],
                base_points[:, 1],
                color="royalblue",
                s=point_size,
                alpha=0.75,
                label="Puntos iniciales",
                zorder=2,
            )

        # Puntos insertados en pasos anteriores
        prev_inserted = data[(data["step"] > 0) & (data["step"] < s) & (data["type"] == "new_point")][
            ["x", "y"]
        ].to_numpy()
        if len(prev_inserted) > 0:
            ax.scatter(
                prev_inserted[:, 0],
                prev_inserted[:, 1],
                color="teal",
                s=40,
                label="Insertados anteriormente",
                zorder=3,
            )

        # Hull anterior (en pasos > 0)
        old_hull = step_df[step_df["type"] == "old_hull"][["x", "y"]].to_numpy()
        hull = step_df[step_df["type"] == "hull"][["x", "y"]].to_numpy()
        new_pt = step_df[step_df["type"] == "new_point"][["x", "y"]].to_numpy()

        changed = False
        if "changed" in step_df.columns and len(step_df) > 0:
            changed = bool(step_df["changed"].iloc[0])

        if s > 0 and len(old_hull) > 0 and changed:
            plot_closed_polygon(
                ax,
                old_hull,
                color="gray",
                linestyle="--",
                linewidth=1.5,
                alpha=0.8,
                label=f"Hull anterior ({len(old_hull)} vértices)",
                zorder=3,
            )

        # Hull actual (ya viene en orden CCW desde d&c.cpp)
        if len(hull) > 0:
            plot_closed_polygon(
                ax,
                hull,
                color="crimson",
                linewidth=2.0,
                label=f"Hull actual ({len(hull)} vértices)",
                zorder=4,
            )

        # Punto recién insertado en este paso
        if len(new_pt) > 0:
            ax.scatter(
                new_pt[:, 0],
                new_pt[:, 1],
                color="gold",
                edgecolor="black",
                marker="*",
                s=220,
                label=f"Nuevo punto ({new_pt[0, 0]:.2f}, {new_pt[0, 1]:.2f})",
                zorder=5,
            )

        if s == 0:
            status = "Estado inicial (Divide & Conquer)"
        elif changed:
            status = "CAMBIÓ el Convex Hull"
        else:
            status = "NO cambió el Convex Hull (interior o en el borde)"

        ax.set_title(f"Paso {s} / {steps[-1]} — {status}")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_xlim(xmin - dx, xmax + dx)
        ax.set_ylim(ymin - dy, ymax + dy)
        ax.set_aspect("equal", adjustable="datalim")
        ax.grid(True, alpha=0.4)
        ax.legend(loc="upper right", fontsize=8)
        fig.canvas.draw_idle()

    def on_prev(_event=None):
        if state["idx"] > 0:
            state["idx"] -= 1
            draw()

    def on_next(_event=None):
        if state["idx"] < len(steps) - 1:
            state["idx"] += 1
            draw()

    def on_key(event):
        if event.key in ("left", "a"):
            on_prev()
        elif event.key in ("right", "d"):
            on_next()

    ax_prev = plt.axes([0.30, 0.04, 0.16, 0.06])
    ax_next = plt.axes([0.54, 0.04, 0.16, 0.06])
    btn_prev = Button(ax_prev, "Anterior (←)")
    btn_next = Button(ax_next, "Siguiente (→)")
    btn_prev.on_clicked(on_prev)
    btn_next.on_clicked(on_next)
    fig.canvas.mpl_connect("key_press_event", on_key)

    draw()
    plt.show()


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else input("Archivo de coordenadas (CSV): ").strip()
    data = pd.read_csv(csv_path)

    if "step" in data.columns:
        run_incremental_view(data)
    else:
        run_static_view(data)


if __name__ == "__main__":
    main()
