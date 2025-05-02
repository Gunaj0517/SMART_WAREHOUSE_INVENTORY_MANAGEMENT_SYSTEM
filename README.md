# 🏭 SuperMart Smart Warehouse Management System

A terminal-based warehouse management system built in C++ that helps manage inventory in a structured grid of shelves. This project simulates intelligent storage, retrieval, and monitoring of goods using concepts like TSP optimization, expiration tracking, and autocomplete search.

---

## 🚀 Features

- 📦 Insert items based on **weight and demand priority**
- 🗺️ View warehouse grid and **navigate shelves**
- 🔍 Search items with **Trie-based autocomplete**
- 🔁 Retrieve items via **TSP (Travelling Salesman Problem)** for minimal walking
- 📁 Save and load warehouse data
- ⏳ Highlight items **expiring soon**
- 🗑️ Delete specific items from warehouse

---

## 📐 Tech Stack

- **C++17**
- **Windows API (for terminal coloring and interaction)**
- STL: `vector`, `unordered_map`, `algorithm`, `fstream`, etc.

---

## 🧠 Logic Highlights

- **Shelf Grid System:** User-defined rows/columns with max weight per shelf
- **Knapsack-based placement:** Items placed by priority (demand + expiry)
- **Expiration detection:** System highlights items about to expire within `n` days
- **TSP Optimization:** Calculates shortest retrieval path for multiple items
- **Trie Structure:** Allows fast item name prefix search

---

## 🧑‍💻 Contributors

- **Gunaj Chugh**
- **Parth Tiwari**
- **Yashi Gupta**
- **Brinda Gupta**

---

## 💡 Future Scope

- GUI using **Qt** or **React + Flask**
- Database integration (SQLite or MySQL)
- Barcode support
- User authentication and roles

---

## 📸 Screenshots

![Cover Page](screenshots/image.png)
![Loading Product Details or Entering New Products](screenshots/image-1.png)
![Drop Down Menu](screenshots/image-2.png)
![Display-Grid View](screenshots/image-3.png)
![Display-Navigation View](screenshots/image-4.png)
![Retrieval via TSP](screenshots/image-5.png)
![Search via Tries and Naive Method](screenshots/image-6.png)
![Shelf Life Left](screenshots/image-7.png)
![Deleting Items from the Warehouse](screenshots/image-8.png)
---

## 📂 How to Run

1. Open the project in your preferred C++ IDE (e.g. Code::Blocks / VSCode)
2. Compile using a C++17-compatible compiler
3. Run the executable in a Windows terminal for full color/navigation support

```bash
g++ main2.cpp -o warehouse.exe
./warehouse.exe
