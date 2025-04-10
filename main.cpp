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
void placeItemsOnShelves(warehouse &w, vector<item> &itemsList) {
    sort(itemsList.begin(), itemsList.end(), [](const item &a, const item &b) {
        return getDemandPriority(a.demand) > getDemandPriority(b.demand);
    });

    for (auto &it : itemsList) {
        int weightRemaining = it.weight;
        bool placed = false;

        for (int i = w.rows - 1; i >= 0 && weightRemaining > 0; i--) {
            for (int j = 0; j < w.columns && weightRemaining > 0; j++) {
                shelf &s = w.shelfGrid[i][j];
                int spaceLeft = s.maxWeight - s.currentWeight;

                if (spaceLeft > 0) {
                    int weightToPlace = min(spaceLeft, weightRemaining);
                    s.storedItems.push_back(it.name + " (" + to_string(weightToPlace) + "kg)");
                    s.currentWeight += weightToPlace;
                    weightRemaining -= weightToPlace;
                    placed = true;

                    cout << "Placed " << it.name << " (" << weightToPlace << "kg) on shelf (" << i << ", " << j << ")\n";
                }
            }
        }

        if (!placed || weightRemaining > 0) {
            cout << "Could not fully place item: " << it.name << ". Weight left: " << weightRemaining << "kg\n";
        }
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
    placeItemsOnShelves(w, itemsList);

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
    string query;
    cout << "\nEnter item name to search: ";
    cin >> query;
    searchItem(w, query);
    
    return 0;
}
