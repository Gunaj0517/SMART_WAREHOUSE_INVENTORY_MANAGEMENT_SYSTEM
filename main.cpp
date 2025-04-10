#include <bits/stdc++.h>
using namespace std;

// Struct for item details (removed size)
struct item {
    string name;   // Item name
    int weight;    // Item weight in kg
    char demand;   // Item demand ('h' for high, 'm' for medium, 'l' for low)
};

// Struct for shelf details
struct shelf {
    int maxWeight;                // max weight each shelf can hold
    int currentWeight = 0;
    vector<string> storedItems;   // List of items stored on the shelf
};

// Struct for warehouse information
struct warehouse {
    int rows;
    int columns;
    int weightPerShelf;
    int totalShelves;
    int totalCapacity;
    vector<vector<shelf>> shelfGrid;  // 2D grid for shelves
};

// Function to get warehouse values (rows, columns, max weight per shelf)
void getWareHouseValues(warehouse &w) {
    cout << "Enter number of rows of your warehouse: ";
    cin >> w.rows;
    cout << "Enter number of columns of your warehouse: ";
    cin >> w.columns;
    cout << "Enter max weight each shelf can hold (in kg): ";
    cin >> w.weightPerShelf;

    w.totalShelves = w.rows * w.columns;
    w.totalCapacity = w.totalShelves * w.weightPerShelf;

    // Initialize shelfGrid as a 2D vector of shelves
    w.shelfGrid.resize(w.rows, vector<shelf>(w.columns));  // 2D resize to rows x columns
    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            w.shelfGrid[i][j].maxWeight = w.weightPerShelf;  // Assign max weight for each shelf
        }
    }

    cout << "Total shelves: " << w.totalShelves << endl;
    cout << "Total weight capacity: " << w.totalCapacity << " kg\n";
}

// Function to get item details (name, weight, demand)
void getItems(vector<item> &itemsList) {
    int n;
    cout << "Enter number of items to store: ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        item it;
        cout << "\nItem " << i + 1 << ":\n";
        cout << "Enter name: ";
        cin >> it.name;
        cout << "Enter weight (in kg): ";
        cin >> it.weight;
        cout << "Enter demand (h = High, m = Medium, l = Low): ";
        cin >> it.demand;
        itemsList.push_back(it);
    }
}

// Helper to convert demand char to priority (h > m > l)
int getDemandPriority(char d) {
    if (d == 'h') return 3;
    if (d == 'm') return 2;
    return 1; // 'l'
}

// Function to place items based on demand
// Knapsack placement with value = weight × demand priority
// Function to place items using fractional knapsack logic
void placeItemsKnapsackBased(warehouse &w, vector<item> &itemsList) {
    // Compute value per weight based on demand
    vector<tuple<double, int>> valuePerWeightIndex;
    for (int i = 0; i < itemsList.size(); i++) {
        double value = itemsList[i].weight * getDemandPriority(itemsList[i].demand);
        valuePerWeightIndex.push_back({value / itemsList[i].weight, i});
    }

    // Sort items based on value/weight ratio (descending)
    sort(valuePerWeightIndex.rbegin(), valuePerWeightIndex.rend());

    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            int remainingCapacity = w.weightPerShelf;

            for (auto &[vpw, idx] : valuePerWeightIndex) {
                if (itemsList[idx].weight == 0) continue; // already placed

                item &it = itemsList[idx];

                if (it.weight <= remainingCapacity) {
                    // Place full item
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(it.weight) + "kg)");
                    w.shelfGrid[i][j].currentWeight += it.weight;
                    remainingCapacity -= it.weight;
                    cout << "Placed full " << it.name << " on shelf (" << i << ", " << j << ")\n";
                    it.weight = 0;
                } else if (remainingCapacity > 0) {
                    // Place fraction
                    int placedWt = remainingCapacity;
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(placedWt) + "kg*)");
                    w.shelfGrid[i][j].currentWeight += placedWt;
                    it.weight -= placedWt;
                    cout << "Placed partial " << it.name << " (" << placedWt << "kg) on shelf (" << i << ", " << j << ")\n";
                    break;  // Shelf is now full
                }
            }
        }
    }
}

void displayWarehouseGrid(const warehouse &w) {
    cout << "\n📦 Warehouse Shelf Grid View:\n\n";

    const int boxWidth = 26; // increased for better spacing
    const int contentWidth = boxWidth - 2;

    for (int i = 0; i < w.rows; i++) {
        // Top border for each shelf
        for (int j = 0; j < w.columns; j++) {
            cout << "+" << string(boxWidth, '-') << "+ ";
        }
        cout << "\n";

        // Coordinates and weight
        for (int j = 0; j < w.columns; j++) {
            const shelf &s = w.shelfGrid[i][j];
            string coord = "(" + to_string(i) + "," + to_string(j) + ")";
            string wt = to_string(s.currentWeight) + "kg";

            string line = coord + " " + wt;
            cout << "| " << setw(contentWidth) << left << line << "| ";
        }
        cout << "\n";

        // Max 3 item lines per shelf
        for (int line = 0; line < 3; line++) {
            for (int j = 0; j < w.columns; j++) {
                const shelf &s = w.shelfGrid[i][j];
                string itemStr = (line < s.storedItems.size()) ? s.storedItems[line] : "";
                cout << "| " << setw(contentWidth) << left << itemStr << "| ";
            }
            cout << "\n";
        }

        // Bottom border
        for (int j = 0; j < w.columns; j++) {
            cout << "+" << string(boxWidth, '-') << "+ ";
        }
        cout << "\n\n";
    }
}

void searchItem(const warehouse &w, const string &itemName) {
    bool found = false;
    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            for (const auto &storedItem : w.shelfGrid[i][j].storedItems) {
                if (storedItem.find(itemName) != string::npos) {
                    cout << "Found " << storedItem << " on shelf (" << i << ", " << j << ")\n";
                    found = true;
                }
            }
        }
    }
    if (!found) cout << "Item " << itemName << " not found.\n";
}

int main() {
    warehouse w;
    vector<item> itemsList;

    // Get warehouse details
    getWareHouseValues(w);

    // Get item details
    getItems(itemsList);

    // Place items on shelves
    placeItemsKnapsackBased(w, itemsList);

    // Print item placement details
    cout << "\nFinal shelf status:\n";
    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            cout << "Shelf (" << i << ", " << j << "): ";
            for (const auto &itemName : w.shelfGrid[i][j].storedItems) {
                cout << itemName << " ";
            }
            cout << " | Total weight: " << w.shelfGrid[i][j].currentWeight << " kg\n";
        }
    }
    displayWarehouseGrid(w);

    string query;
    cout << "\nEnter item name to search: ";
    cin >> query;
    searchItem(w, query);

    return 0;
}
