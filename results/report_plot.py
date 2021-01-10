"""Module to plot A* solution in a map
Google maps solution from:
https://webapps.stackexchange.com/questions/34159/how-to-convert-google-map-route-into-array-of-coordinates
Coordinates are first longitude then latitude
"""
import gmplot


def read_path(path):
    """Read coordinates from input_file"""
    latitude, longitude = [], []
    with open(path, "r") as file:
        for line in file:
            if line.startswith("#"):
                continue
            fields = line.split(",")
            for field in fields:
                field.strip()
            longitude.append(float(fields[0]))
            latitude.append(float(fields[1]))
    return latitude, longitude


PATHS = []
PATHS.append("../data/doc.kml")
PATHS.append("optimal_path_dijkstra.csv")
PATHS.append("optimal_path_haversine.csv")
PATHS.append("optimal_path_spherical.csv")
PATHS.append("optimal_path_equirectangular.csv")
OUTPUT_FILE = "all_paths.html"

colors = ["cornflowerblue", "red", "green", "purple", "orange"]

# map origin
gmap = gmplot.GoogleMapPlotter(38.4077013, -0.5015955, 7)

for i, path in enumerate(PATHS):
    lat, lon = read_path(path)
    gmap.plot(lat, lon, colors[i], edge_width=6)


# Save path map
gmap.draw(OUTPUT_FILE)
