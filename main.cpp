#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

// Utility functions
string intToString(int value) {
    stringstream ss;
    ss << value;
    return ss.str();
}

int stringToInt(const string& str) {
    stringstream ss(str);
    int result = 0;
    ss >> result;
    return result;
}

double stringToDouble(const string& str) {
    stringstream ss(str);
    double result = 0.0;
    ss >> result;
    return result;
}

vector<string> split(const string& str, char delimiter) {
    vector<string> result;
    stringstream ss(str);
    string token;
    while(getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

// Customer class
class Customer {
private:
    int id;
    string name, phone, address, checkInDate, checkOutDate;
    int roomNumber;

public:
    Customer(int id, string name, string phone, string address) 
        : id(id), name(name), phone(phone), address(address), roomNumber(0) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }
    int getRoomNumber() const { return roomNumber; }
    string getCheckInDate() const { return checkInDate; }
    string getCheckOutDate() const { return checkOutDate; }

    // Setters
    void setRoomNumber(int room) { roomNumber = room; }
    void setCheckInDate(string date) { checkInDate = date; }
    void setCheckOutDate(string date) { checkOutDate = date; }

    void display() const {
        cout << "ID: " << id << " | Name: " << name << " | Phone: " << phone 
             << " | Room: " << (roomNumber == 0 ? "None" : intToString(roomNumber)) << endl;
    }

    string toFileString() const {
        return intToString(id) + "|" + name + "|" + phone + "|" + address + "|" + 
               intToString(roomNumber) + "|" + checkInDate + "|" + checkOutDate;
    }

    static Customer fromFileString(const string& line) {
        vector<string> parts = split(line, '|');
        Customer c(stringToInt(parts[0]), parts[1], parts[2], parts[3]);
        if(parts.size() > 4) {
            c.setRoomNumber(stringToInt(parts[4]));
            if(parts.size() > 5) c.setCheckInDate(parts[5]);
            if(parts.size() > 6) c.setCheckOutDate(parts[6]);
        }
        return c;
    }
};

// Room class
class Room {
public:
    enum Type { SINGLE = 1, DOUBLE, SUITE };
    
private:
    int number;
    Type type;
    double price;
    bool occupied;
    int customerId;

public:
    Room(int num, Type t, double p) : number(num), type(t), price(p), occupied(false), customerId(0) {}

    // Getters
    int getNumber() const { return number; }
    Type getType() const { return type; }
    double getPrice() const { return price; }
    bool isOccupied() const { return occupied; }
    int getCustomerId() const { return customerId; }

    void checkIn(int custId) { occupied = true; customerId = custId; }
    void checkOut() { occupied = false; customerId = 0; }

    string getTypeString() const {
        switch(type) {
            case SINGLE: return "Single";
            case DOUBLE: return "Double"; 
            case SUITE: return "Suite";
            default: return "Unknown";
        }
    }

    void display() const {
        cout << "Room " << number << " - " << getTypeString() << " - $" << price 
             << "/night - " << (occupied ? "Occupied" : "Available") << endl;
    }

    string toFileString() const {
        return intToString(number) + "|" + intToString(occupied) + "|" + intToString(customerId);
    }
};

// Bill class
class Bill {
private:
    int billId, customerId, roomNumber;
    double roomCharges, serviceCharges, total;
    string date;
    bool paid;

public:
    Bill(int id, int custId, int room, double roomCost, double serviceCost = 50.0) 
        : billId(id), customerId(custId), roomNumber(room), 
          roomCharges(roomCost), serviceCharges(serviceCost), paid(false) {
        total = roomCharges + serviceCharges;
        
        time_t now = time(0);
        char* dt = ctime(&now);
        date = string(dt).substr(0, 24);
    }

    // Getters
    int getBillId() const { return billId; }
    int getCustomerId() const { return customerId; }
    bool isPaid() const { return paid; }

    void pay() { paid = true; }

    void display() const {
        cout << "\n========== BILL ==========\n";
        cout << "Bill ID: " << billId << " | Customer ID: " << customerId << endl;
        cout << "Room: " << roomNumber << " | Room Charges: $" << roomCharges << endl;
        cout << "Service Charges: $" << serviceCharges << " | Total: $" << total << endl;
        cout << "Date: " << date << " | Status: " << (paid ? "PAID" : "UNPAID") << endl;
        cout << "==========================\n";
    }

    string toFileString() const {
        return intToString(billId) + "|" + intToString(customerId) + "|" + 
               intToString(roomNumber) + "|" + intToString(roomCharges) + "|" + 
               intToString(serviceCharges) + "|" + intToString(total) + "|" + 
               date + "|" + intToString(paid);
    }

    static Bill fromFileString(const string& line) {
        vector<string> parts = split(line, '|');
        Bill b(stringToInt(parts[0]), stringToInt(parts[1]), stringToInt(parts[2]), 
               stringToDouble(parts[3]), stringToDouble(parts[4]));
        b.paid = (parts[7] == "1");
        return b;
    }
};

// File Manager class - handles all file operations
class FileManager {
private:
    static const string CUSTOMER_FILE;
    static const string ROOM_FILE;
    static const string BILL_FILE;
    static const string CONFIG_FILE;

public:
    // Save functions
    static bool saveCustomers(const vector<Customer>& customers) {
        ofstream file(CUSTOMER_FILE.c_str());
        if(!file) return false;
        
        for(size_t i = 0; i < customers.size(); i++) {
            file << customers[i].toFileString() << endl;
        }
        return true;
    }

    static bool saveRooms(const vector<Room>& rooms) {
        ofstream file(ROOM_FILE.c_str());
        if(!file) return false;
        
        for(size_t i = 0; i < rooms.size(); i++) {
            file << rooms[i].toFileString() << endl;
        }
        return true;
    }

    static bool saveBills(const vector<Bill>& bills) {
        ofstream file(BILL_FILE.c_str());
        if(!file) return false;
        
        for(size_t i = 0; i < bills.size(); i++) {
            file << bills[i].toFileString() << endl;
        }
        return true;
    }

    static bool saveConfig(int nextCustomerId, int nextBillId) {
        ofstream file(CONFIG_FILE.c_str());
        if(!file) return false;
        
        file << nextCustomerId << endl << nextBillId << endl;
        return true;
    }

    // Load functions
    static bool loadCustomers(vector<Customer>& customers) {
        ifstream file(CUSTOMER_FILE.c_str());
        if(!file) return false;
        
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                customers.push_back(Customer::fromFileString(line));
            }
        }
        return true;
    }

    static bool loadRooms(vector<Room>& rooms) {
        ifstream file(ROOM_FILE.c_str());
        if(!file) return false;
        
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                vector<string> parts = split(line, '|');
                int roomNum = stringToInt(parts[0]);
                bool occupied = (parts[1] == "1");
                int custId = stringToInt(parts[2]);
                
                // Find room and update status
                for(size_t i = 0; i < rooms.size(); i++) {
                    if(rooms[i].getNumber() == roomNum && occupied) {
                        rooms[i].checkIn(custId);
                        break;
                    }
                }
            }
        }
        return true;
    }

    static bool loadBills(vector<Bill>& bills) {
        ifstream file(BILL_FILE.c_str());
        if(!file) return false;
        
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                bills.push_back(Bill::fromFileString(line));
            }
        }
        return true;
    }

    static bool loadConfig(int& nextCustomerId, int& nextBillId) {
        ifstream file(CONFIG_FILE.c_str());
        if(!file) return false;
        
        file >> nextCustomerId >> nextBillId;
        return true;
    }
};

// Static member definitions
const string FileManager::CUSTOMER_FILE = "customers.dat";
const string FileManager::ROOM_FILE = "rooms.dat";
const string FileManager::BILL_FILE = "bills.dat";
const string FileManager::CONFIG_FILE = "config.dat";

// Main Hotel Management System
class HotelSystem {
private:
    vector<Customer> customers;
    vector<Room> rooms;
    vector<Bill> bills;
    int nextCustomerId;
    int nextBillId;

    void initRooms() {
        rooms.clear();
        // Create sample rooms
        for(int i = 101; i <= 105; i++) rooms.push_back(Room(i, Room::SINGLE, 100.0));
        for(int i = 201; i <= 203; i++) rooms.push_back(Room(i, Room::DOUBLE, 150.0));
        for(int i = 301; i <= 302; i++) rooms.push_back(Room(i, Room::SUITE, 250.0));
    }

    Customer* findCustomer(int id) {
        for(size_t i = 0; i < customers.size(); i++) {
            if(customers[i].getId() == id) return &customers[i];
        }
        return NULL;
    }

    Room* findRoom(int number) {
        for(size_t i = 0; i < rooms.size(); i++) {
            if(rooms[i].getNumber() == number) return &rooms[i];
        }
        return NULL;
    }

    Bill* findBill(int id) {
        for(size_t i = 0; i < bills.size(); i++) {
            if(bills[i].getBillId() == id) return &bills[i];
        }
        return NULL;
    }

    void saveAll() {
        FileManager::saveCustomers(customers);
        FileManager::saveRooms(rooms);
        FileManager::saveBills(bills);
        FileManager::saveConfig(nextCustomerId, nextBillId);
    }

public:
    HotelSystem() : nextCustomerId(1001), nextBillId(5001) {
        cout << "Initializing Hotel Management System...\n";
        initRooms();
        
        // Load existing data
        FileManager::loadCustomers(customers);
        FileManager::loadRooms(rooms);
        FileManager::loadBills(bills);
        FileManager::loadConfig(nextCustomerId, nextBillId);
        
        cout << "System ready! Loaded " << customers.size() << " customers, " 
             << bills.size() << " bills.\n\n";
    }

    ~HotelSystem() {
        cout << "\nSaving all data...\n";
        saveAll();
        cout << "Data saved successfully. Goodbye!\n";
    }

    void addCustomer() {
        string name, phone, address;
        cout << "Enter customer name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter phone: ";
        getline(cin, phone);
        cout << "Enter address: ";
        getline(cin, address);

        customers.push_back(Customer(nextCustomerId++, name, phone, address));
        cout << "Customer added! ID: " << (nextCustomerId - 1) << endl;
        saveAll();
    }

    void checkIn() {
        int custId, roomNum;
        string date;

        cout << "Enter Customer ID: ";
        cin >> custId;

        Customer* customer = findCustomer(custId);
        if(!customer) {
            cout << "Customer not found!\n";
            return;
        }

        if(customer->getRoomNumber() != 0) {
            cout << "Customer already checked in!\n";
            return;
        }

        displayAvailableRooms();
        cout << "Enter Room Number: ";
        cin >> roomNum;

        Room* room = findRoom(roomNum);
        if(!room || room->isOccupied()) {
            cout << "Room not available!\n";
            return;
        }

        cout << "Enter check-in date (DD/MM/YYYY): ";
        cin >> date;

        room->checkIn(custId);
        customer->setRoomNumber(roomNum);
        customer->setCheckInDate(date);

        cout << "Check-in successful!\n";
        saveAll();
    }

    void checkOut() {
        int custId;
        string date;

        cout << "Enter Customer ID: ";
        cin >> custId;

        Customer* customer = findCustomer(custId);
        if(!customer || customer->getRoomNumber() == 0) {
            cout << "Customer not found or not checked in!\n";
            return;
        }

        Room* room = findRoom(customer->getRoomNumber());
        cout << "Enter check-out date (DD/MM/YYYY): ";
        cin >> date;

        // Create bill
        bills.push_back(Bill(nextBillId++, custId, room->getNumber(), room->getPrice()));

        // Check out
        room->checkOut();
        customer->setCheckOutDate(date);
        customer->setRoomNumber(0);

        cout << "Check-out successful! Bill generated.\n";
        bills.back().display();
        saveAll();
    }

    void displayAvailableRooms() {
        cout << "\n=== AVAILABLE ROOMS ===\n";
        bool found = false;
        for(size_t i = 0; i < rooms.size(); i++) {
            if(!rooms[i].isOccupied()) {
                rooms[i].display();
                found = true;
            }
        }
        if(!found) cout << "No rooms available!\n";
    }

    void displayCustomers() {
        cout << "\n=== ALL CUSTOMERS ===\n";
        if(customers.empty()) {
            cout << "No customers found!\n";
            return;
        }
        for(size_t i = 0; i < customers.size(); i++) {
            customers[i].display();
        }
    }

    void displayBills() {
        cout << "\n=== ALL BILLS ===\n";
        if(bills.empty()) {
            cout << "No bills found!\n";
            return;
        }
        for(size_t i = 0; i < bills.size(); i++) {
            bills[i].display();
        }
    }

    void payBill() {
        int billId;
        cout << "Enter Bill ID: ";
        cin >> billId;

        Bill* bill = findBill(billId);
        if(!bill) {
            cout << "Bill not found!\n";
            return;
        }

        if(bill->isPaid()) {
            cout << "Bill already paid!\n";
            return;
        }

        bill->pay();
        cout << "Payment successful!\n";
        saveAll();
    }

    void showMenu() {
        cout << "\n=== HOTEL MANAGEMENT SYSTEM ===\n";
        cout << "1. Add Customer\n2. Check-In\n3. Check-Out\n";
        cout << "4. Available Rooms\n5. All Customers\n6. All Bills\n";
        cout << "7. Pay Bill\n0. Exit\n";
        cout << "Choice: ";
    }

    void run() {
        int choice;
        do {
            showMenu();
            cin >> choice;

            switch(choice) {
                case 1: addCustomer(); break;
                case 2: checkIn(); break;
                case 3: checkOut(); break;
                case 4: displayAvailableRooms(); break;
                case 5: displayCustomers(); break;
                case 6: displayBills(); break;
                case 7: payBill(); break;
                case 0: cout << "Thank you!\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 0);
    }
};

int main() {
    HotelSystem hotel;
    hotel.run();
    return 0;
}


