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


PATH = "results/optimal_path.csv"
MAPS_PATH = "data/doc.kml"

# output file name
OUTPUT_FILE = PATH[:-3] + "html"

lat_maps, lon_maps = read_path(MAPS_PATH)  # data from google maps UI
lat_sol, lon_sol = read_path(PATH)


# map origin
gmap = gmplot.GoogleMapPlotter(38.4077013, -0.5015955, 7)


# Plot path
gmap.plot(lat_maps, lon_maps, "cornflowerblue", edge_width=6)
gmap.plot(lat_sol, lon_sol, "orange", edge_width=6)

# Save path map
gmap.draw(OUTPUT_FILE)
