import random
import csv

def generate_cyclic_graph(n_nodes):
    # Generate a cycle to ensure at least one cycle exists
    edges = set()
    for i in range(n_nodes):
        u = i
        v = (i + 1) % n_nodes  # Connect in a ring
        if u < v:
            edges.add((u, v))
        else:
            edges.add((v, u))

    # Add random edges to create additional cycles
    max_edges = n_nodes * (n_nodes - 1) // 2  # Maximum possible edges
    target_edges = min(n_nodes + (n_nodes // 2), max_edges)  # Add 50% more edges than nodes
    
    while len(edges) < target_edges:
        u = random.randint(0, n_nodes-1)
        v = random.randint(0, n_nodes-1)
        if u != v:
            u, v = sorted((u, v))  # Ensure u < v to avoid duplicates
            if (u, v) not in edges:
                edges.add((u, v))

    # Assign random weights/distance (1-10)
    weighted_edges = []
    for u, v in edges:
        weight = random.randint(1, 10)
        weighted_edges.append((u, v, weight))

    return weighted_edges, len(edges)

def save_to_csv(edges, filename):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["source", "destination", "distance"])
        for edge in edges:
            writer.writerow(edge)

# User input
n = int(input("Enter number of nodes: "))
edges, edge_count = generate_cyclic_graph(n)

# Save to CSV
filename = f"cyclic_graph_{n}_nodes.csv"
save_to_csv(edges, filename)

print(f"\nGenerated cyclic graph with {n} nodes and {edge_count} edges")
print(f"CSV file saved as: {filename}")