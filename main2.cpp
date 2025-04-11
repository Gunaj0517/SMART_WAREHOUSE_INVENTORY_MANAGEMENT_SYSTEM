#include <bits/stdc++.h>
#include <conio.h>
using namespace std;

void Cleardisplay()
{
    system("cls");
}
void clearScreen()
{
    cout << "\n\n\n\n\n\n\e[1mPRESS ANY KEY TO CONTINUE...\e[m";
    getch();
    Cleardisplay();
}

// Struct for item details (removed size)
struct item
{
    string name;          // Item name
    int weight;           // Item weight in kg
    char demand;          // Item demand ('h' for high, 'm' for medium, 'l' for low)
    int year, month, day; // Expiry Date.
};

// Struct for shelf details
struct shelf
{
    int maxWeight; // max weight each shelf can hold
    int currentWeight = 0;
    vector<string> storedItems; // List of items stored on the shelf
};

struct ItemMeta
{
    double priorityScore;
    int idx;
};

// Struct for warehouse information
struct warehouse
{
    int rows;
    int columns;
    int weightPerShelf;
    int totalShelves;
    int totalCapacity;
    vector<vector<shelf>> shelfGrid; // 2D grid for shelves
};

bool isExpired(int y, int m, int d)
{
    time_t now = time(0);
    tm *ltm = localtime(&now);

    if (y < 1900 + ltm->tm_year)
        return true;
    if (y == 1900 + ltm->tm_year && m < 1 + ltm->tm_mon)
        return true;
    if (y == 1900 + ltm->tm_year && m == 1 + ltm->tm_mon && d < ltm->tm_mday)
        return true;

    return false;
}

void getWareHouseValues(warehouse &w)
{
    cout << "\e[36mEnter number of rows of your warehouse: \e[m";
    cin >> w.rows;
    cout << "\e[36mEnter number of columns of your warehouse: \e[m";
    cin >> w.columns;
    cout << "\e[36mEnter max weight each shelf can hold (in kg): \e[m";
    cin >> w.weightPerShelf;

    w.totalShelves = w.rows * w.columns;
    w.totalCapacity = w.totalShelves * w.weightPerShelf;

    w.shelfGrid.resize(w.rows, vector<shelf>(w.columns));
    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            w.shelfGrid[i][j].maxWeight = w.weightPerShelf;
        }
    }

    cout << "\n\e[1;35mTotal shelves: " << w.totalShelves << endl;
    cout << "Total weight capacity: " << w.totalCapacity << " kg\e[m\n\n\n";
    clearScreen();
}

void getItems(vector<item> &itemsList)
{
    int n;
    cout << "\e[36mEnter number of items to store: \e[m";
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        item it;
        cout << "\n\e[1;35mItem " << i + 1 << ":\e[m\n";
        cout << "\e[32mEnter name: \e[m";
        cin >> it.name;
        cout << "\e32mEnter weight (in kg): \e[m";
        cin >> it.weight;
        cout << "\e32mEnter demand (h = High, m = Medium, l = Low): \e[m";
        cin >> it.demand;
        cout << "\e[32mEnter expiry date (YYYY MM DD): \e[m";
        cin >> it.year >> it.month >> it.day;

        if (isExpired(it.year, it.month, it.day))
        {
            cout << "\e[1;31mItem is expired and will not be stored.\e[m\n";
            continue;
        }
        itemsList.push_back(it);
    }
}

int getDemandPriority(char d)
{
    if (d == 'h')
        return 3;
    if (d == 'm')
        return 2;
    return 1;
}

void placeItemsKnapsackBased(warehouse &w, vector<item> &itemsList, unordered_map<string, vector<pair<int, int>>> &itemLocations)
{
    vector<ItemMeta> itemMeta;
    for (int i = 0; i < itemsList.size(); i++)
    {
        double score = getDemandPriority(itemsList[i].demand);
        itemMeta.push_back({score, i});
    }

    // Sort by demand → expiry
    sort(itemMeta.begin(), itemMeta.end(), [&](const ItemMeta &a, const ItemMeta &b)
         {
        if (a.priorityScore != b.priorityScore)
            return a.priorityScore > b.priorityScore;

        const item &ia = itemsList[a.idx];
        const item &ib = itemsList[b.idx];

        if (ia.year != ib.year) return ia.year < ib.year;
        if (ia.month != ib.month) return ia.month < ib.month;
        return ia.day < ib.day; });

    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            int remainingCapacity = w.weightPerShelf;

            for (auto &[score, idx] : itemMeta)
            {
                if (itemsList[idx].weight == 0)
                    continue;

                item &it = itemsList[idx];

                if (it.weight <= remainingCapacity)
                {
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(it.weight) + "kg, Exp: " + to_string(it.year) + "-" + (it.month < 10 ? "0" : "") + to_string(it.month) + "-" + (it.day < 10 ? "0" : "") + to_string(it.day) + ")");
                    w.shelfGrid[i][j].currentWeight += it.weight;
                    remainingCapacity -= it.weight;
                    itemLocations[it.name].push_back({i, j});
                    cout << "Placed full " << it.name << " on shelf (" << i << ", " << j << ")\n";
                    it.weight = 0;
                }
                else if (remainingCapacity > 0)
                {
                    int placedWt = remainingCapacity;
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(placedWt) + "kg*, Exp: " + to_string(it.year) + "-" + (it.month < 10 ? "0" : "") + to_string(it.month) + "-" + (it.day < 10 ? "0" : "") + to_string(it.day) + ")");
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

void displayWarehouseGrid(const warehouse &w)
{
    cout << "\n📦 Warehouse Shelf Grid View:\n\n";
    const int boxWidth = 26;
    const int contentWidth = boxWidth - 2;

    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            cout << "+" << string(boxWidth, '-') << "+ ";
        }
        cout << "\n";

        for (int j = 0; j < w.columns; j++)
        {
            const shelf &s = w.shelfGrid[i][j];
            string coord = "(" + to_string(i) + "," + to_string(j) + ")";
            string wt = to_string(s.currentWeight) + "kg";
            string line = coord + " " + wt;
            cout << "| " << setw(contentWidth) << left << line << "| ";
        }
        cout << "\n";

        for (int line = 0; line < 3; line++)
        {
            for (int j = 0; j < w.columns; j++)
            {
                const shelf &s = w.shelfGrid[i][j];
                string itemStr = (line < s.storedItems.size()) ? s.storedItems[line] : "";
                cout << "| " << setw(contentWidth) << left << itemStr << "| ";
            }
            cout << "\n";
        }

        for (int j = 0; j < w.columns; j++)
        {
            cout << "+" << string(boxWidth, '-') << "+ ";
        }
        cout << "\n\n";
    }
}

void searchItem(const warehouse &w, const string &itemName)
{
    bool found = false;
    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            for (const auto &storedItem : w.shelfGrid[i][j].storedItems)
            {
                if (storedItem.find(itemName) != string::npos)
                {
                    cout << "Found " << storedItem << " on shelf (" << i << ", " << j << ")\n";
                    found = true;
                }
            }
        }
    }
    if (!found)
        cout << "Item " << itemName << " not found.\n";
}

int tspFixedStart(const vector<pair<int, int>> &points, vector<int> &bestOrder)
{
    int n = points.size() - 1; // excluding starting point
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 1); // skip index 0 (which is start)

    int minDist = INT_MAX;

    do
    {
        int dist = 0;
        // distance from start to first point
        dist += abs(points[0].first - points[indices[0]].first) +
                abs(points[0].second - points[indices[0]].second);

        // distance between intermediate points
        for (int i = 0; i < n - 1; i++)
        {
            dist += abs(points[indices[i]].first - points[indices[i + 1]].first) +
                    abs(points[indices[i]].second - points[indices[i + 1]].second);
        }

        if (dist < minDist)
        {
            minDist = dist;
            bestOrder = indices;
        }
    } while (next_permutation(indices.begin(), indices.end()));

    return minDist;
}

void retrieveItemsWithTSP(unordered_map<string, vector<pair<int, int>>> &itemLocations)
{
    int n;
    cout << "\nEnter number of items to retrieve: ";
    cin >> n;

    vector<pair<int, int>> points = {{0, 0}}; // starting point
    vector<string> itemOrder = {"START"};

    for (int i = 0; i < n; i++)
    {
        string itemName;
        cout << "Enter item name #" << i + 1 << ": ";
        cin >> itemName;

        if (itemLocations.count(itemName))
        {
            points.push_back(itemLocations[itemName][0]);
            itemOrder.push_back(itemName);
        }
        else
        {
            cout << "Item " << itemName << " not found in warehouse.\n";
        }
    }

    if (points.size() <= 2)
    {
        cout << "Not enough items to calculate TSP.\n";
        return;
    }

    vector<int> bestPath;
    int totalDistance = tspFixedStart(points, bestPath);

    cout << "\nOptimal retrieval path from (0,0) (TSP):\n";
    for (int i = 0; i < bestPath.size(); ++i)
    {
        int idx = bestPath[i];
        auto [r, c] = points[idx];
        cout << i + 1 << ". Move to (" << r << ", " << c << ")";
        if (itemOrder[idx] != "START")
        {
            cout << " to retrieve " << itemOrder[idx];
        }
        cout << "\n";
    }

    cout << "Total distance: " << totalDistance << " steps\n";
}

int main()
{
    warehouse w;
    vector<item> itemsList;
    unordered_map<string, vector<pair<int, int>>> itemLocations;
    Cleardisplay();
    cout << "\n\e[1;34mWelcome to SuperMart Smart Warehouse System!\e[m\n\n\n";
    cout << "\n\e[1;33mLet's set up your warehouse shelves!\e[m";
    clearScreen();
    getWareHouseValues(w);

    int choice;
    do
    {
        cout << "\e[1;33mChoose an action:\e[m\n";
        cout << "1. Enter items to store\n";
        cout << "2. View shelf status/grid\n";
        cout << "3. Retrieve items (TSP optimized)\n";
        cout << "4. Search for an item\n";
        cout << "0. Exit\n\n";
        cout << "\e[1;36mEnter your choice: \e[m";
        cin >> choice;

        switch (choice)
        {
        case 1:
            Cleardisplay();
            getItems(itemsList);
            placeItemsKnapsackBased(w, itemsList, itemLocations);

            cout << "\nFinal shelf status:\n";
            for (int i = 0; i < w.rows; i++)
            {
                for (int j = 0; j < w.columns; j++)
                {
                    cout << "Shelf (" << i << ", " << j << "): ";
                    for (const auto &itemName : w.shelfGrid[i][j].storedItems)
                    {
                        cout << itemName << " ";
                    }
                    cout << " | Total weight: " << w.shelfGrid[i][j].currentWeight << " kg\n";
                }
            }
            break;

        case 2:
            Cleardisplay();
            displayWarehouseGrid(w);
            break;

        case 3:
            Cleardisplay();
            retrieveItemsWithTSP(itemLocations);
            break;

        case 4:
        {
            Cleardisplay();
            string query;
            cout << "Enter item name to search: ";
            cin >> query;
            searchItem(w, query);
            break;
        }

        case 0:
            Cleardisplay();
            cout << "\e[1;31mExiting... Thank you for using SuperMart System!\e[m\n";
            break;

        default:
            Cleardisplay();
            cout << "\e[1;31mInvalid choice. Please select from 0 to 4.\e[m\n\n\n";
            clearScreen();
        }
    } while (choice != 0);

    return 0;
}
