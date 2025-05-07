#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>
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
void clearScreen2()
{
    cout << "\n\n\n\n\n\n\e[1mPRESS ARROW KEYS TO NAVIGATE OR ESC TO END...\e[m";
    
    char ch = getch();
    if (ch == 27) {  
        ch = getch();
        if (ch == 91) {
            ch = getch();
            cout << "\nArrow key pressed!" << endl;
        }
    } else if (ch == 27) { 
        cout << "\nESC pressed. Exiting..." << endl;
    }
    Cleardisplay();
}


struct item
{
    string name;          
    int weight;           
    char demand;          
    int year, month, day; 
};


struct shelf
{
    int maxWeight; 
    int currentWeight = 0;
    vector<string> storedItems; 
};

struct ItemMeta
{
    double priorityScore;
    int idx;
};


struct warehouse
{
    int rows;
    int columns;
    int weightPerShelf;
    int totalShelves;
    int totalCapacity;
    vector<vector<shelf>> shelfGrid; 
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
        cout << "\e[32mEnter weight (in kg): \e[m";
        cin >> it.weight;
        cout << "\e[32mEnter demand (h = High, m = Medium, l = Low): \e[m";
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
            int remainingCapacity = w.shelfGrid[i][j].maxWeight - w.shelfGrid[i][j].currentWeight;

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
                    cout << "\n\n\e[1mPlaced full " << it.name << " on shelf (" << i << ", " << j << ")\e[m\n";
                    it.weight = 0;
                }
                else if (remainingCapacity > 0)
                {
                    int placedWt = remainingCapacity;
                    w.shelfGrid[i][j].storedItems.push_back(it.name + " (" + to_string(placedWt) + "kg*, Exp: " + to_string(it.year) + "-" + (it.month < 10 ? "0" : "") + to_string(it.month) + "-" + (it.day < 10 ? "0" : "") + to_string(it.day) + ")");
                    w.shelfGrid[i][j].currentWeight += placedWt;
                    it.weight -= placedWt;
                    itemLocations[it.name].push_back({i, j});
                    cout << "\n\n\e[1mPlaced partial " << it.name << " (" << placedWt << "kg) on shelf (" << i << ", " << j << ")\e[m\n";
                    break;
                }
            }
        }
    }
    clearScreen();
}

int daysUntilExpiry(const item &it);
void displayItems(const vector<item> &itemsList);

void displayWarehouseGrid(warehouse &w, vector<item> &itemsList) 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int boxWidth = 12;
    int boxHeight = 5;
    int spacing = 2;

    int currentRow = 0;
    int currentCol = 0;

    while (true) 
    {
        system("cls");

        for (int i = 0; i < w.rows; i++) 
        {
            for (int j = 0; j < w.columns; j++) 
            {
                const shelf &s = w.shelfGrid[i][j];

                
                if (i == currentRow && j == currentCol) 
                {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                } 
                else if (s.currentWeight == 0) 
                {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_INTENSITY);
                } 
                else 
                {
                    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                }

                cout << "+" << string(boxWidth, '-') << "+ ";
            }
            cout << endl;

            for (int line = 0; line < boxHeight; line++) 
            {
                for (int j = 0; j < w.columns; j++) 
                {
                    const shelf &s = w.shelfGrid[i][j];
                    string content;

                    if (line == 1) 
                    {
                        content = "(" + to_string(i) + "," + to_string(j) + ")";
                    } 
                    else if (line == 2) 
                    {
                        content = to_string(s.currentWeight) + "kg";
                    } 
                    else if (line >= 3 && line - 3 < s.storedItems.size()) 
                    {
                        content = s.storedItems[line - 3];
                        if (content.size() > boxWidth - 2) 
                        {
                            content = content.substr(0, boxWidth - 5) + "...";
                        }
                    } 
                    else 
                    {
                        content = "";
                    }

                    if (i == currentRow && j == currentCol) 
                    {
                        SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                    } 
                    else if (s.currentWeight > 0) 
                    {
                        SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                    } 
                    else 
                    {
                        SetConsoleTextAttribute(hConsole, BACKGROUND_INTENSITY);
                    }

                    cout << "| " << setw(boxWidth - 1) << left << content << "| ";
                }
                cout << endl;
            }

            for (int j = 0; j < w.columns; j++) 
            {
                SetConsoleTextAttribute(hConsole, BACKGROUND_INTENSITY);
                cout << "+" << string(boxWidth, '-') << "+ ";
            }
            cout << endl << endl;
        }

        
        SetConsoleTextAttribute(hConsole, 7);

        
        displayItems(itemsList);

        
        shelf &selected = w.shelfGrid[currentRow][currentCol];
        cout << "\nSelected Shelf: (" << currentRow << "," << currentCol << ")\n";
        cout << "Weight: " << selected.currentWeight << "kg\n";
        cout << "Items:\n";
        if (selected.storedItems.empty()) 
        {
            cout << "  None\n";
        } 
        else 
        {
            for (const auto &itm : selected.storedItems) 
            {
                cout << "  - " << itm << "\n";
            }
        }

        cout << "\nUse Arrow Keys to Navigate, ESC to Exit.\n";

        
        int key = _getch();
        if (key == 224) 
        {
            int arrow = _getch();
            switch (arrow) 
            {
                case 72: if (currentRow > 0) currentRow--; break; 
                case 80: if (currentRow < w.rows - 1) currentRow++; break; 
                case 75: if (currentCol > 0) currentCol--; break; 
                case 77: if (currentCol < w.columns - 1) currentCol++; break; 
            }
        } 
        else if (key == 27) 
        {
            Cleardisplay();
            break;
        }

        
        
    }
}

struct TrieNode {
    bool isEnd;
    unordered_map<char, TrieNode*> children;
    vector<string> fullNames;  

    TrieNode() : isEnd(false) {}
};

class Trie {
    TrieNode* root;

public:
    Trie() { root = new TrieNode(); }

    void insert(const string &word) {
        TrieNode* node = root;
        for (char c : word) {
            if (!node->children[c])
                node->children[c] = new TrieNode();
            node = node->children[c];
            node->fullNames.push_back(word);  
        }
        node->isEnd = true;
    }

    vector<string> autocomplete(const string &prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (!node->children[c])
                return {};
            node = node->children[c];
        }
        return node->fullNames;
    }
};

void searchItem(const warehouse &w, const string &query)
{
    cout << "\n\e[1;35mSearching for items matching prefix '" << query << "':\e[m\n";

    unordered_set<string> displayedLocations; 
    bool found = false; 

    
    for (int i = 0; i < w.rows; i++) {
        for (int j = 0; j < w.columns; j++) {
            for (const auto &storedItem : w.shelfGrid[i][j].storedItems) {
                
                size_t pos = storedItem.find(" (");
                string storedName = (pos != string::npos) ? storedItem.substr(0, pos) : storedItem;

                
                if (storedName.find(query) == 0) {
                    string uniqueKey = storedName + "_" + to_string(i) + "_" + to_string(j);

                    
                    if (displayedLocations.find(uniqueKey) == displayedLocations.end()) {
                        cout << "\e[1;32m✔ " << storedName << " found on shelf (" << i << ", " << j << "): " << storedItem << "\e[m\n";
                        displayedLocations.insert(uniqueKey);
                        found = true;
                    }
                }
            }
        }
    }

    if (!found) {
        cout << "\e[1;31m✘ No items matching prefix '" << query << "' found in the warehouse.\e[m\n";
    }

    clearScreen();
}

int tspFixedStart(const vector<pair<int, int>> &points, vector<int> &bestOrder)
{
    int n = points.size() - 1; 
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 1); 

    int minDist = INT_MAX;

    do
    {
        int dist = 0;
        
        dist += abs(points[0].first - points[indices[0]].first) +
                abs(points[0].second - points[indices[0]].second);

        
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
    cout << "\e[32mEnter number of items to retrieve: \e[m";
    cin >> n;

    vector<pair<int, int>> points = {{0, 0}}; 
    vector<string> itemOrder = {"START"};

    for (int i = 0; i < n; i++)
    {
        string itemName;
        cout << "Enter item name #" << i + 1 << " : ";
        cin >> itemName;

        if (itemLocations.count(itemName))
        {
            points.push_back(itemLocations[itemName][0]);
            itemOrder.push_back(itemName);
        }
        else
        {
            cout << "\n\n\e[1;31mItem " << itemName << " not found in warehouse.\e[m\n";
        }
    }

    if (points.size() <= 2)
    {
        cout << "\n\n\e[1;31mNot enough items to calculate TSP.\e[m\n";
        return;
    }

    vector<int> bestPath;
    int totalDistance = tspFixedStart(points, bestPath);

    cout << "\n\n\e[1;35mOptimal retrieval path from (0,0) (TSP): \e[m\n";
    for (int i = 0; i < bestPath.size(); ++i)
    {
        int idx = bestPath[i];
        auto [r, c] = points[idx];
        cout << "\e[1m" << i + 1 << ". Move to (" << r << ", " << c << ")";
        if (itemOrder[idx] != "START")
        {
            cout << " to retrieve " << itemOrder[idx];
        }
        cout << "\e[m\n";
    }

    cout << "\n\e[1;32mTotal distance: " << totalDistance << " steps\e[m\n";
}

void saveDataToFile(const warehouse &w, const vector<item> &itemsList, const unordered_map<string, vector<pair<int, int>>> &itemLocations)
{
    ofstream file("warehouse_data.txt");
    if (!file)
    {
        cout << "\e[1;31mError saving data!\e[m\n";
        return;
    }

    file << w.rows << " " << w.columns << " " << w.weightPerShelf << "\n";
    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            const shelf &s = w.shelfGrid[i][j];
            file << s.currentWeight << " " << s.storedItems.size() << "\n";
            for (auto &itm : s.storedItems)
            {
                file << itm << "\n";
            }
        }
    }

    file << itemsList.size() << "\n";
    for (auto &it : itemsList)
    {
        file << it.name << " " << it.weight << " " << it.demand << " "
             << it.year << " " << it.month << " " << it.day << "\n";
    }

    file << itemLocations.size() << "\n";
    for (auto &[name, locs] : itemLocations)
    {
        file << name << " " << locs.size();
        for (auto &[r, c] : locs)
        {
            file << " " << r << " " << c;
        }
        file << "\n";
    }

    file.close();
    cout << "\e[1;32mData saved successfully!\e[m\n";
}

void loadDataFromFile(warehouse &w, vector<item> &itemsList, unordered_map<string, vector<pair<int, int>>> &itemLocations)
{
    ifstream file("warehouse_data.txt");
    if (!file)
    {
        cout << "\e[1;31mFailed to load file.\e[m\n";
        return;
    }

    
    file >> w.rows >> w.columns >> w.weightPerShelf;
    w.totalShelves = w.rows * w.columns;
    w.totalCapacity = w.totalShelves * w.weightPerShelf;

    w.shelfGrid.resize(w.rows, vector<shelf>(w.columns));
    for (int i = 0; i < w.rows; i++)
    {
        for (int j = 0; j < w.columns; j++)
        {
            int currentWeight, storedCount;
            file >> currentWeight >> storedCount;

            w.shelfGrid[i][j].currentWeight = currentWeight; 
            w.shelfGrid[i][j].maxWeight = w.weightPerShelf;  

            w.shelfGrid[i][j].storedItems.clear();
            file.ignore(); 
            for (int k = 0; k < storedCount; k++)
            {
                string itemDesc;
                file >> ws;                                        
                getline(file, itemDesc);                           
                w.shelfGrid[i][j].storedItems.push_back(itemDesc); 
            }
        }
    }

    
    int itemCount;
    file >> itemCount;
    itemsList.clear();
    for (int i = 0; i < itemCount; ++i)
    {
        item it;
        file >> it.name >> it.weight >> it.demand >> it.year >> it.month >> it.day;
        itemsList.push_back(it);
    }

    
    int locationCount;
    file >> locationCount;
    itemLocations.clear();
    for (int i = 0; i < locationCount; ++i)
    {
        string name;
        int locSize;
        file >> name >> locSize;
        for (int j = 0; j < locSize; ++j)
        {
            int r, c;
            file >> r >> c;
            itemLocations[name].push_back({r, c});
        }
    }

    file.close();
    cout << "\e[1;32mData loaded successfully!\e[m\n";
}
void populateItemsFromShelfGrid(const warehouse &w, vector<item> &itemsList)
{
    itemsList.clear(); 

    for (int i = 0; i < w.rows; ++i) {
        for (int j = 0; j < w.columns; ++j) {
            for (const string &desc : w.shelfGrid[i][j].storedItems) {
                item it;
                size_t nameEnd = desc.find(" (");
                if (nameEnd == string::npos) continue;

                it.name = desc.substr(0, nameEnd);

                size_t weightStart = desc.find('(') + 1;
                size_t weightEnd = desc.find("kg");
                if (weightStart == string::npos || weightEnd == string::npos) continue;

                string weightStr = desc.substr(weightStart, weightEnd - weightStart);
                it.weight = stoi(weightStr);

                size_t expStart = desc.find("Exp: ");
                if (expStart == string::npos) continue;

                string dateStr = desc.substr(expStart + 5); 
                sscanf(dateStr.c_str(), "%d-%d-%d", &it.year, &it.month, &it.day);

                it.demand = 'm'; 

                itemsList.push_back(it);
            }
        }
    }
}
void showExpiringSoonItems(const vector<item> &itemsList)
{
    int daysAhead;
    cout << "\n\e[35mEnter number of days ahead to check for expiry: \e[m";
    cin >> daysAhead;

    time_t now = time(0);

    
    tm *nowTm = localtime(&now);
    nowTm->tm_hour = 0;
    nowTm->tm_min = 0;
    nowTm->tm_sec = 0;
    now = mktime(nowTm);

    bool found = false; 

    for (const auto &it : itemsList)
    {
        tm expiryTm = {};
        expiryTm.tm_year = it.year - 1900;
        expiryTm.tm_mon = it.month - 1;
        expiryTm.tm_mday = it.day;
        expiryTm.tm_hour = 0;
        expiryTm.tm_min = 0;
        expiryTm.tm_sec = 0;

        time_t expiryTime = mktime(&expiryTm);

        if (expiryTime == -1)
        {
            continue; 
        }

        double diffSeconds = difftime(expiryTime, now);
        int diffDays = diffSeconds / (60 * 60 * 24);

        if (diffDays >= 0 && diffDays <= daysAhead)
        {
            cout << "\n\e[1;35mItems expiring within next " << daysAhead << " days:\e[m\n";
            found = true;
            cout << "\e[32mItem Name: " << it.name << "\n";
            cout << "Expiry Date: " << it.day << "-" << it.month << "-" << it.year << "\n";
            cout << "Days Left: " << diffDays << " days\e[m\n\n";
        }
    }

    if (!found)
    {
        cout << "\e[1;31mNo items expiring within " << daysAhead << " days.\e[m\n";
    }
}

void deleteItem(warehouse &w, vector<item> &itemsList, unordered_map<string, vector<pair<int, int>>> &itemLocations) {
    string itemName;
    cout << "\e[36mEnter the item name to delete: \e[m";
    cin >> itemName;

    
    if (itemLocations.find(itemName) == itemLocations.end()) {
        cout << "\n\n\e[1;31mItem " << itemName << " not found in warehouse.\e[m\n";
        clearScreen();
        return;
    }

    
    for (auto &loc : itemLocations[itemName]) {
        int r = loc.first;
        int c = loc.second;

        shelf &s = w.shelfGrid[r][c];
        for (auto it = s.storedItems.begin(); it != s.storedItems.end(); ) {
            if (it->find(itemName) != string::npos) {
                size_t start = it->find('(');
                size_t end = it->find("kg");
                if (start != string::npos && end != string::npos && end > start) {
                    string weightStr = it->substr(start + 1, end - start - 1);
                    int wt = stoi(weightStr);
                    s.currentWeight -= wt;
                }
                it = s.storedItems.erase(it);
            } else {
                ++it;
            }
        }        
        
        
        
    }

    
    itemLocations.erase(itemName);

    
    itemsList.erase(remove_if(itemsList.begin(), itemsList.end(),
                               [&](const item &it) { return it.name == itemName; }),
                    itemsList.end());

    cout << "\n\n\e[1;32mItem " << itemName << " deleted successfully!\e[m\n";

    
    saveDataToFile(w, itemsList, itemLocations);

}

int daysUntilExpiry(const item &it) {
    tm expiryDate = {0};
    expiryDate.tm_year = it.year - 1900;
    expiryDate.tm_mon = it.month - 1;
    expiryDate.tm_mday = it.day;

    time_t expTime = mktime(&expiryDate);
    time_t now = time(0);

    double seconds = difftime(expTime, now);
    int days = static_cast<int>(seconds / (60 * 60 * 24));
    return days;
}

void displayItems(const vector<item> &itemsList) {
    cout << "\n\e[1;34mItems in Warehouse:\e[m\n";
    for (const auto &it : itemsList) {
        int daysLeft = daysUntilExpiry(it);
        string color;

        if (daysLeft < 0)
            color = "\e[1;31m";  
        else if (daysLeft <= 60)
            color = "\e[1;33m";  
        else
            color = "\e[1;32m";  

        cout << color << "Item: " << it.name
             << ", Weight: " << it.weight
             << ", Demand: " << it.demand
             << ", Expiry: " << it.year << "-" << it.month << "-" << it.day
             << " (" << daysLeft << " days left)" << "\e[m\n";
    }
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

    cout << "\e[1m1. Load warehouse data from file\n";
    cout << "2. Enter new warehouse data \e[m\n";
    int LoadOrNew;
    cout << "\n\n\e[1;36mEnter your choice: \e[m";
    cin >> LoadOrNew;
    Trie itemTrie;
    switch (LoadOrNew)
    {
case 1:
    Cleardisplay();
    loadDataFromFile(w, itemsList, itemLocations); 
    populateItemsFromShelfGrid(w, itemsList);      
    itemTrie = Trie();                             
    for (const auto &it : itemsList)
    {
        itemTrie.insert(it.name);
    }
    clearScreen();
    break;
    case 2:
        Cleardisplay();
        getWareHouseValues(w);
        break;

    default:
        Cleardisplay();
        cout << "\e[1;31mInvalid choice. Please select 1 or 2.\e[m\n\n\n";
        clearScreen();
        break;
    }
    int choice;
    populateItemsFromShelfGrid(w, itemsList);
    do
    {
        cout << "\e[1;33mChoose an action:\e[m\n";
        cout << "1. Enter items to store\n";
        cout << "2. View shelf status/grid\n";
        cout << "3. Retrieve items (TSP optimized)\n";
        cout << "4. Search for an item\n";
        cout << "5. Save warehouse data to file\n";
        cout << "6. Show items expiring soon\n";
        cout << "7. Delete item\n";
        cout << "0. Exit\n\n";
        cout << "\e[1;36mEnter your choice: \e[m";
        cin >> choice;

        switch (choice)
        {
        case 1:
            Cleardisplay();
        
            
            itemsList.clear();               
        
            getItems(itemsList);            
        
            for (const auto& it : itemsList) {
                itemTrie.insert(it.name);   
            }
        
            placeItemsKnapsackBased(w, itemsList, itemLocations);  
        
            
            itemsList.erase(remove_if(itemsList.begin(), itemsList.end(), [](const item &it) {
                return it.weight == 0;
            }), itemsList.end());
        
            cout << "\e[1;33mFinal shelf status:\e[m\n";
            for (int i = 0; i < w.rows; i++) {
                for (int j = 0; j < w.columns; j++) {
                    cout << "\e[1mShelf (" << i << ", " << j << "): ";
                    for (const auto &itemName : w.shelfGrid[i][j].storedItems) {
                        cout << itemName << " ";
                    }
                    cout << " | Total weight: " << w.shelfGrid[i][j].currentWeight << " kg\e[m\n";
                }
            }
            clearScreen();
            break;        
        case 2:
            Cleardisplay();
            displayWarehouseGrid(w,itemsList);
            break;

        case 3:
            Cleardisplay();
            retrieveItemsWithTSP(itemLocations);
            clearScreen();
            break;

        case 4:
        {
            Cleardisplay();
            string query;
            cout << "\e[1;33mEnter item name (or prefix) to search: \e[m";
            cin >> query;
            searchItem(w, query); 
            break; 
        }   
        case 5:
        {
            Cleardisplay();
            saveDataToFile(w, itemsList, itemLocations);
            clearScreen();
            break;
        }

        case 6:
        {
            Cleardisplay();
            showExpiringSoonItems(itemsList);
            clearScreen();
            break;
        }

        case 7:
        {
            Cleardisplay();
            deleteItem(w,itemsList,itemLocations);
            clearScreen();
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

