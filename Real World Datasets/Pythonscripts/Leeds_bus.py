import pandas as pd
from math import radians, sin, cos, sqrt, atan2

# Haversine formula to calculate distance between two points on Earth
def haversine(lat1, lon1, lat2, lon2):
    # Radius of the Earth in kilometers
    R = 6371.0

    # Convert latitude and longitude to radians
    lat1, lon1, lat2, lon2 = map(radians, [lat1, lon1, lat2, lon2])

    # Difference in coordinates
    dlat = lat2 - lat1
    dlon = lon2 - lon1

    # Haversine formula
    a = sin(dlat / 2)**2 + cos(lat1) * cos(lat2) * sin(dlon / 2)**2
    c = 2 * atan2(sqrt(a), sqrt(1 - a))

    # Distance in kilometers
    distance = R * c
    return distance

# Specify data types for stop_times.txt
dtype_stop_times = {
    'trip_id': str,
    'arrival_time': str,
    'departure_time': str,
    'stop_id': str,
    'stop_sequence': int,
    # Add other columns if necessary
}

# Load GTFS data
stops = pd.read_csv('itm_yorkshire_gtfs/stops.txt')  # Replace with the path to your stops.txt file
stop_times = pd.read_csv(
    'itm_yorkshire_gtfs/stop_times.txt',
    dtype=dtype_stop_times,  # Specify data types
    low_memory=False  # Handle mixed types
)

# Create a mapping from stop_id to stop_name
stop_name_map = stops.set_index('stop_id')['stop_name'].to_dict()

# Sort stop_times by trip_id and stop_sequence
stop_times = stop_times.sort_values(by=['trip_id', 'stop_sequence'])

# Merge stops with stop_times to get stop details
stop_sequence = stop_times.merge(stops[['stop_id', 'stop_lat', 'stop_lon']], on='stop_id')

# Create a set to store unique edges
edges = set()

# Group by trip_id and process each trip
for trip_id, group in stop_sequence.groupby('trip_id'):
    # Convert group to a list of stops
    stops_list = group[['stop_id', 'stop_lat', 'stop_lon']].to_dict(orient='records')
    
    # Iterate through consecutive stops
    for i in range(len(stops_list) - 1):
        stop1 = stops_list[i]
        stop2 = stops_list[i + 1]
        
        # Calculate distance between stops using Haversine formula
        distance = haversine(
            stop1['stop_lat'], stop1['stop_lon'],
            stop2['stop_lat'], stop2['stop_lon']
        )
        
        # Add edge as a tuple (source_name, destination_name, distance)
        source_name = stop_name_map[stop1['stop_id']]
        destination_name = stop_name_map[stop2['stop_id']]
        edges.add((source_name, destination_name, distance))

# Convert set to DataFrame
edges_df = pd.DataFrame(edges, columns=["Source", "Destination", "Distance"])

# Save to CSV
# edges_df.to_csv('yorkshire_bus_network_edges.csv', index=False)

edges_df = edges_df.sort_values(by="Distance", ascending=False)
print("Largest distance edge:", edges_df.iloc[0])