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

void getWareHouseValues(warehouse &w) {
    cout << "Enter number of rows of your warehouse: ";
    cin >> w.rows;
    cout << "Enter number of columns of your warehouse: ";
    cin >> w.columns;
    cout << "Enter max weight each shelf can hold (in kg): ";
    cin >> w.weightPerShelf;

    w.totalShelves = w.rows * w.columns;
    w.totalCapacity = w.totalShelves * w.weightPerShelf;

    w.shelfGrid.resize(w.rows, vector<shelf>(w.columns));
    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            w.shelfGrid[i][j].maxWeight = w.weightPerShelf;
        }
    }

    cout << "Total shelves: " << w.totalShelves << endl;
    cout << "Total weight capacity: " << w.totalCapacity << " kg\n";
}

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

int getDemandPriority(char d) {
    if (d == 'h') return 3;
    if (d == 'm') return 2;
    return 1;
}

void placeItemsKnapsackBased(warehouse &w, vector<item> &itemsList, unordered_map<string, vector<pair<int, int>>> &itemLocations) {
    vector<tuple<double, int>> valuePerWeightIndex;
    for (int i = 0; i < itemsList.size(); i++) {
        double value = itemsList[i].weight * getDemandPriority(itemsList[i].demand);
        valuePerWeightIndex.push_back({value / itemsList[i].weight, i});
    }

    sort(valuePerWeightIndex.rbegin(), valuePerWeightIndex.rend());

    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            int remainingCapacity = w.weightPerShelf;

            for (auto &[vpw, idx] : valuePerWeightIndex) {
                if (itemsList[idx].weight == 0) continue;

                item &it = itemsList[idx];

                if (it.weight <= remainingCapacity) {
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(it.weight) + "kg)");
                    w.shelfGrid[i][j].currentWeight += it.weight;
                    remainingCapacity -= it.weight;
                    itemLocations[it.name].push_back({i, j});
                    cout << "Placed full " << it.name << " on shelf (" << i << ", " << j << ")\n";
                    it.weight = 0;
                } else if (remainingCapacity > 0) {
                    int placedWt = remainingCapacity;
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(placedWt) + "kg*)");
                    w.shelfGrid[i][j].currentWeight += placedWt;
                    it.weight -= placedWt;
                    itemLocations[it.name].push_back({i, j});
                    cout << "Placed partial " << it.name << " (" << placedWt << "kg) on shelf (" << i << ", " << j << ")\n";
                    break;
                }
            }
        }
    }
}

void displayWarehouseGrid(const warehouse &w) {
    cout << "\n📦 Warehouse Shelf Grid View:\n\n";
    const int boxWidth = 26;
    const int contentWidth = boxWidth - 2;

    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            cout << "+" << string(boxWidth, '-') << "+ ";
        }
        cout << "\n";

        for (int j = 0; j < w.columns; j++) {
            const shelf &s = w.shelfGrid[i][j];
            string coord = "(" + to_string(i) + "," + to_string(j) + ")";
            string wt = to_string(s.currentWeight) + "kg";
            string line = coord + " " + wt;
            cout << "| " << setw(contentWidth) << left << line << "| ";
        }
        cout << "\n";

        for (int line = 0; line < 3; line++) {
            for (int j = 0; j < w.columns; j++) {
                const shelf &s = w.shelfGrid[i][j];
                string itemStr = (line < s.storedItems.size()) ? s.storedItems[line] : "";
                cout << "| " << setw(contentWidth) << left << itemStr << "| ";
            }
            cout << "\n";
        }

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

int tspFixedStart(const vector<pair<int, int>> &points, vector<int> &bestOrder) {
    int n = points.size() - 1;  // excluding starting point
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 1);  // skip index 0 (which is start)

    int minDist = INT_MAX;

    do {
        int dist = 0;
        // distance from start to first point
        dist += abs(points[0].first - points[indices[0]].first) +
                abs(points[0].second - points[indices[0]].second);

        // distance between intermediate points
        for (int i = 0; i < n - 1; i++) {
            dist += abs(points[indices[i]].first - points[indices[i + 1]].first) +
                    abs(points[indices[i]].second - points[indices[i + 1]].second);
        }

        if (dist < minDist) {
            minDist = dist;
            bestOrder = indices;
        }
    } while (next_permutation(indices.begin(), indices.end()));

    return minDist;
}

void retrieveItemsWithTSP(unordered_map<string, vector<pair<int, int>>> &itemLocations) {
    int n;
    cout << "\nEnter number of items to retrieve: ";
    cin >> n;

    vector<pair<int, int>> points = {{0, 0}}; // starting point
    vector<string> itemOrder = {"START"};

    for (int i = 0; i < n; i++) {
        string itemName;
        cout << "Enter item name #" << i + 1 << ": ";
        cin >> itemName;

        if (itemLocations.count(itemName)) {
            points.push_back(itemLocations[itemName][0]);
            itemOrder.push_back(itemName);
        } else {
            cout << "Item " << itemName << " not found in warehouse.\n";
        }
    }

    if (points.size() <= 2) {
        cout << "Not enough items to calculate TSP.\n";
        return;
    }

    vector<int> bestPath;
    int totalDistance = tspFixedStart(points, bestPath);

    cout << "\n🧭 Optimal retrieval path from (0,0) (TSP):\n";
    for (int i = 0; i < bestPath.size(); ++i) {
        int idx = bestPath[i];
        auto [r, c] = points[idx];
        cout << i + 1 << ". Move to (" << r << ", " << c << ")";
        if (itemOrder[idx] != "START") {
            cout << " to retrieve " << itemOrder[idx];
        }
        cout << "\n";
    }

    cout << "Total distance: " << totalDistance << " steps\n";
}


int main() {
    warehouse w;
    vector<item> itemsList;
    unordered_map<string, vector<pair<int, int>>> itemLocations;

    getWareHouseValues(w);
    getItems(itemsList);
    placeItemsKnapsackBased(w, itemsList, itemLocations);

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

    retrieveItemsWithTSP(itemLocations);

    return 0;
}
