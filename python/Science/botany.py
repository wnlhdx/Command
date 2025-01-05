import requests

# Define KEGG Pathway API endpoint
KEGG_API_URL = "http://rest.kegg.jp/list/pathway"

# Fetch KEGG pathways
def fetch_kegg_pathways():
    response = requests.get(KEGG_API_URL)
    if response.status_code == 200:
        return response.text
    else:
        raise Exception(f"Failed to fetch KEGG pathways: {response.status_code}")

# Filter pathways related to carbohydrates and lipids
def filter_metabolism_pathways(data, keywords):
    pathways = {}
    for line in data.strip().split("\n"):
        if any(keyword.lower() in line.lower() for keyword in keywords):
            entry, description = line.split("\t")
            pathways[entry] = description
    return pathways

# Display pathways
def display_pathways(pathways):
    print("Filtered Metabolic Pathways:")
    for entry, description in pathways.items():
        print(f"{entry}: {description}")

# Main execution
if __name__ == "__main__":
    try:
        print("Fetching KEGG pathways...")
        kegg_data = fetch_kegg_pathways()
        
        keywords = ["carbohydrate", "lipid"]
        print("Filtering pathways related to carbohydrates and lipids...")
        filtered_pathways = filter_metabolism_pathways(kegg_data, keywords)
        
        display_pathways(filtered_pathways)
    except Exception as e:
        print(f"An error occurred: {e}")
