"""Module to plot A* solution"""
import argparse
import gmplot

parser = argparse.ArgumentParser(description="A* visualization")
parser.add_argument(
    "-path", type=str, default="results/optimal_path.csv", help="File with optimal path"
)

args = parser.parse_args()


# Place map in Albacete
gmap = gmplot.GoogleMapPlotter(38.993464, -1.859774, 7)


# Define an output file name according to input file name
output_file = args.path[:-3] + "html"

# Read coordinates from input_file
latitude = []
longitude = []
with open(args.path, "r") as f:
    for line in f:
        if line.startswith("#"):
            continue
        fields = line.split(",")
        for field in fields:
            field.strip()
        latitude.append(float(fields[3]))
        longitude.append(float(fields[4]))

# Add route to map
gmap.plot(latitude, longitude, "cornflowerblue", edge_width=6)

# Save map
gmap.draw(output_file)
