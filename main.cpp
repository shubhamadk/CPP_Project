#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// Utility functions
string doubleToString(double value) {
    stringstream ss;
    ss << fixed << setprecision(2) << value;
    return ss.str();
}

string intToString(int value) {
    stringstream ss;
    ss << value;
    return ss.str();
}

int stringToInt(const string& str) {
    try {
        stringstream ss(str);
        int result = 0;
        ss >> result;
        if (ss.fail() || !ss.eof()) {
            return 0; // Return default value on failure
        }
        return result;
    } catch (...) {
        return 0;
    }
}

double stringToDouble(const string& str) {
    try {
        stringstream ss(str);
        double result = 0.0;
        ss >> result;
        if (ss.fail() || !ss.eof()) {
            return 0.0; // Return default value on failure
        }
        return result;
    } catch (...) {
        return 0.0;
    }
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

// Enhanced input validation functions
bool isValidDate(const string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;
    
    int day, month, year;
    if (sscanf(date.c_str(), "%d/%d/%d", &day, &month, &year) != 3) return false;
    
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1) return false;
    
    // Days in each month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeapYear && month == 2) {
        daysInMonth[1] = 29;
    }
    
    // Validate day against the actual days in the month
    if (day > daysInMonth[month - 1]) return false;
    
    return true;
}

bool isCheckOutDateValid(const string& checkIn, const string& checkOut) {
    if (!isValidDate(checkIn) || !isValidDate(checkOut)) return false;
    
    int d1, m1, y1, d2, m2, y2;
    sscanf(checkIn.c_str(), "%d/%d/%d", &d1, &m1, &y1);
    sscanf(checkOut.c_str(), "%d/%d/%d", &d2, &m2, &y2);

    tm in = {}, out = {};
    in.tm_mday = d1;
    in.tm_mon = m1 - 1;
    in.tm_year = y1 - 1900;

    out.tm_mday = d2;
    out.tm_mon = m2 - 1;
    out.tm_year = y2 - 1900;

    time_t t1 = mktime(&in);
    time_t t2 = mktime(&out);

    if (t1 == -1 || t2 == -1) return false;
    return t2 > t1; // Check-out must be after check-in
}

int getValidInt(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            return value;
        } else {
            cout << "Invalid input. Please enter a valid number";
            if (min != INT_MIN || max != INT_MAX) {
                cout << " between " << min << " and " << max;
            }
            cout << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getValidString(const string& prompt, int maxLength = 100) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (!input.empty() && input.length() <= maxLength) {
            return input;
        }
        cout << "Invalid input. Please enter a non-empty string (max " << maxLength << " characters).\n";
    }
}

string getValidDate(const string& prompt) {
    string date;
    while (true) {
        cout << prompt;
        cin >> date;
        
        // Clear any remaining input
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (isValidDate(date)) {
            return date;
        }
        cout << "Invalid date! Please enter a valid date in DD/MM/YYYY format.\n";
        cout << "Examples: 01/08/2025, 15/12/2024, 28/02/2025\n";
    }
}

string getValidCheckOutDate(const string& prompt, const string& checkInDate) {
    string date;
    while (true) {
        cout << prompt;
        cin >> date;
        
        // Clear any remaining input
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (isValidDate(date) && isCheckOutDateValid(checkInDate, date)) {
            return date;
        }
        if (!isValidDate(date)) {
            cout << "Invalid date! Please enter a valid date in DD/MM/YYYY format.\n";
            cout << "Examples: 01/08/2025, 15/12/2024, 28/02/2025\n";
        } else {
            cout << "Check-out date must be after check-in date (" << checkInDate << ").\n";
        }
    }
}

// Calculate number of nights between two dates (DD/MM/YYYY)
int calculateNights(const string& checkIn, const string& checkOut) {
    if (!isValidDate(checkIn) || !isValidDate(checkOut)) return 1;
    
    int d1, m1, y1, d2, m2, y2;
    sscanf(checkIn.c_str(), "%d/%d/%d", &d1, &m1, &y1);
    sscanf(checkOut.c_str(), "%d/%d/%d", &d2, &m2, &y2);

    tm in = {}, out = {};
    in.tm_mday = d1;
    in.tm_mon = m1 - 1;
    in.tm_year = y1 - 1900;

    out.tm_mday = d2;
    out.tm_mon = m2 - 1;
    out.tm_year = y2 - 1900;

    time_t t1 = mktime(&in);
    time_t t2 = mktime(&out);

    if (t1 == -1 || t2 == -1) return 1;
    double seconds = difftime(t2, t1);
    int days = (int)(seconds / (60 * 60 * 24));
    return (days < 1) ? 1 : days;
}

// Customer class
class Customer {
private:
    int id;
    string name, phone, address, checkInDate, checkOutDate;
    int roomNumber;

public:
    Customer(int id, const string& name, const string& phone, const string& address) 
        : id(id), name(name), phone(phone), address(address), roomNumber(0) {}

    int getId() const { return id; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    string getAddress() const { return address; }
    int getRoomNumber() const { return roomNumber; }
    string getCheckInDate() const { return checkInDate; }
    string getCheckOutDate() const { return checkOutDate; }

    void setRoomNumber(int room) { roomNumber = room; }
    void setCheckInDate(const string& date) { checkInDate = date; }
    void setCheckOutDate(const string& date) { checkOutDate = date; }

    void display() const {
        cout << "ID: " << id << " | Name: " << name << " | Phone: " << phone 
             << " | Room: " << (roomNumber == 0 ? "None" : intToString(roomNumber));
        if (roomNumber != 0) {
            cout << " | Check-in: " << checkInDate;
            if (!checkOutDate.empty()) {
                cout << " | Check-out: " << checkOutDate;
            }
        }
        cout << endl;
    }

    string toFileString() const {
        return intToString(id) + "|" + name + "|" + phone + "|" + address + "|" + 
               intToString(roomNumber) + "|" + checkInDate + "|" + checkOutDate;
    }

    static Customer fromFileString(const string& line) {
        vector<string> parts = split(line, '|');
        if (parts.size() < 4) {
            // Return a default customer if data is corrupted
            return Customer(0, "Unknown", "Unknown", "Unknown");
        }
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
        cout << "Room " << number << " - " << getTypeString() << " - $" << fixed << setprecision(2) << price 
             << "/night - " << (occupied ? "Occupied" : "Available") << endl;
    }

    string toFileString() const {
        return intToString(number) + "|" + intToString(occupied) + "|" + intToString(customerId);
    }
};

// Bill class
class Bill {
private:
    int billId, customerId, roomNumber, nights;
    double roomCharges, serviceCharges, total;
    string date, checkInDate, checkOutDate;
    bool paid;

public:
    Bill(int id, int custId, int room, double pricePerNight, int numNights, 
         const string& checkIn, const string& checkOut, double serviceCost = 50.0) 
        : billId(id), customerId(custId), roomNumber(room), nights(numNights),
          serviceCharges(serviceCost), paid(false), checkInDate(checkIn), checkOutDate(checkOut) {
        roomCharges = pricePerNight * nights;
        total = roomCharges + serviceCharges;
        
        time_t now = time(0);
        char* dt = ctime(&now);
        if (dt != nullptr) {
            date = string(dt).substr(0, 24);
        } else {
            date = "Unknown Date";
        }
    }

    int getBillId() const { return billId; }
    int getCustomerId() const { return customerId; }
    bool isPaid() const { return paid; }
    double getTotal() const { return total; }

    void pay() { paid = true; }

    void display() const {
        cout << "\n========== BILL ==========\n";
        cout << "Bill ID: " << billId << " | Customer ID: " << customerId << endl;
        cout << "Room: " << roomNumber << " | Nights Stayed: " << nights << endl;
        cout << "Check-in: " << checkInDate << " | Check-out: " << checkOutDate << endl;
        cout << "Room Charges: $" << fixed << setprecision(2) << roomCharges 
             << " (Rate: $" << (roomCharges/nights) << "/night)" << endl;
        cout << "Service Charges: $" << fixed << setprecision(2) << serviceCharges << endl;
        cout << "Total Amount: $" << fixed << setprecision(2) << total << endl;
        cout << "Bill Date: " << date << endl;
        cout << "Status: " << (paid ? "PAID" : "UNPAID") << endl;
        cout << "==========================\n";
    }

    string toFileString() const {
        return intToString(billId) + "|" + intToString(customerId) + "|" + 
               intToString(roomNumber) + "|" + intToString(nights) + "|" +
               doubleToString(roomCharges) + "|" + doubleToString(serviceCharges) + "|" + 
               doubleToString(total) + "|" + date + "|" + intToString(paid) + "|" +
               checkInDate + "|" + checkOutDate;
    }

    static Bill fromFileString(const string& line) {
        vector<string> parts = split(line, '|');
        if (parts.size() < 11) {
            // Return a default bill if data is corrupted
            return Bill(0, 0, 0, 0.0, 1, "01/01/2024", "02/01/2024", 0.0);
        }
        Bill b(stringToInt(parts[0]), stringToInt(parts[1]), stringToInt(parts[2]), 
               stringToDouble(parts[4])/stringToInt(parts[3]), stringToInt(parts[3]), 
               parts[9], parts[10], stringToDouble(parts[5]));
        b.paid = (parts[8] == "1");
        return b;
    }
};

// FileManager class
class FileManager {
private:
    static const string CUSTOMER_FILE;
    static const string ROOM_FILE;
    static const string BILL_FILE;
    static const string CONFIG_FILE;

public:
    static bool saveCustomers(const vector<Customer>& customers) {
        ofstream file(CUSTOMER_FILE.c_str());
        if(!file.is_open()) {
            cout << "Error: Could not save customers to file.\n";
            return false;
        }
        for(const auto& c : customers) {
            file << c.toFileString() << endl;
        }
        file.close();
        return true;
    }

    static bool saveRooms(const vector<Room>& rooms) {
        ofstream file(ROOM_FILE.c_str());
        if(!file.is_open()) {
            cout << "Error: Could not save rooms to file.\n";
            return false;
        }
        for(const auto& r : rooms) {
            file << r.toFileString() << endl;
        }
        file.close();
        return true;
    }

    static bool saveBills(const vector<Bill>& bills) {
        ofstream file(BILL_FILE.c_str());
        if(!file.is_open()) {
            cout << "Error: Could not save bills to file.\n";
            return false;
        }
        for(const auto& b : bills) {
            file << b.toFileString() << endl;
        }
        file.close();
        return true;
    }

    static bool saveConfig(int nextCustomerId, int nextBillId) {
        ofstream file(CONFIG_FILE.c_str());
        if(!file.is_open()) {
            cout << "Error: Could not save configuration to file.\n";
            return false;
        }
        file << nextCustomerId << endl << nextBillId << endl;
        file.close();
        return true;
    }

    static bool loadCustomers(vector<Customer>& customers) {
        ifstream file(CUSTOMER_FILE.c_str());
        if(!file.is_open()) {
            cout << "Note: Customer file not found. Starting with empty customer list.\n";
            return false;
        }
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                Customer c = Customer::fromFileString(line);
                if (c.getId() != 0) { // Only add valid customers
                    customers.push_back(c);
                }
            }
        }
        file.close();
        return true;
    }

    static bool loadRooms(vector<Room>& rooms) {
        ifstream file(ROOM_FILE.c_str());
        if(!file.is_open()) {
            cout << "Note: Room file not found. Using default room configuration.\n";
            return false;
        }
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                vector<string> parts = split(line, '|');
                if (parts.size() >= 3) {
                    int roomNum = stringToInt(parts[0]);
                    bool occupied = (parts[1] == "1");
                    int custId = stringToInt(parts[2]);
                    
                    for(auto& r : rooms) {
                        if(r.getNumber() == roomNum && occupied) {
                            r.checkIn(custId);
                            break;
                        }
                    }
                }
            }
        }
        file.close();
        return true;
    }

    static bool loadBills(vector<Bill>& bills) {
        ifstream file(BILL_FILE.c_str());
        if(!file.is_open()) {
            cout << "Note: Bill file not found. Starting with empty bill list.\n";
            return false;
        }
        string line;
        while(getline(file, line)) {
            if(!line.empty()) {
                Bill b = Bill::fromFileString(line);
                if (b.getBillId() != 0) { // Only add valid bills
                    bills.push_back(b);
                }
            }
        }
        file.close();
        return true;
    }

    static bool loadConfig(int& nextCustomerId, int& nextBillId) {
        ifstream file(CONFIG_FILE.c_str());
        if(!file.is_open()) {
            cout << "Note: Config file not found. Using default configuration.\n";
            return false;
        }
        file >> nextCustomerId >> nextBillId;
        file.close();
        return true;
    }
};

const string FileManager::CUSTOMER_FILE = "customers.dat";
const string FileManager::ROOM_FILE = "rooms.dat";
const string FileManager::BILL_FILE = "bills.dat";
const string FileManager::CONFIG_FILE = "config.dat";

// HotelSystem class
class HotelSystem {
private:
    vector<Customer> customers;
    vector<Room> rooms;
    vector<Bill> bills;
    int nextCustomerId;
    int nextBillId;

    void initRooms() {
        rooms.clear();
        for(int i = 101; i <= 105; i++) rooms.push_back(Room(i, Room::SINGLE, 100.0));
        for(int i = 201; i <= 203; i++) rooms.push_back(Room(i, Room::DOUBLE, 150.0));
        for(int i = 301; i <= 302; i++) rooms.push_back(Room(i, Room::SUITE, 250.0));
    }

    Customer* findCustomer(int id) {
        for(size_t i = 0; i < customers.size(); i++) {
            if(customers[i].getId() == id) return &customers[i];
        }
        return nullptr;
    }

    Room* findRoom(int number) {
        for(size_t i = 0; i < rooms.size(); i++) {
            if(rooms[i].getNumber() == number) return &rooms[i];
        }
        return nullptr;
    }

    Bill* findBill(int id) {
        for(size_t i = 0; i < bills.size(); i++) {
            if(bills[i].getBillId() == id) return &bills[i];
        }
        return nullptr;
    }

    void saveAll() {
        bool success = true;
        success &= FileManager::saveCustomers(customers);
        success &= FileManager::saveRooms(rooms);
        success &= FileManager::saveBills(bills);
        success &= FileManager::saveConfig(nextCustomerId, nextBillId);
        
        if (!success) {
            cout << "Warning: Some data may not have been saved properly.\n";
        }
    }

public:
    HotelSystem() : nextCustomerId(1001), nextBillId(5001) {
        cout << "========================================\n";
        cout << "   HOTEL MANAGEMENT SYSTEM \n";
        cout << "========================================\n";
        cout << "Initializing system...\n";
        initRooms();
        FileManager::loadCustomers(customers);
        FileManager::loadRooms(rooms);
        FileManager::loadBills(bills);
        FileManager::loadConfig(nextCustomerId, nextBillId);
        cout << "System ready! Loaded " << customers.size() << " customers, " << bills.size() << " bills.\n";
        cout << "Available rooms: " << count_if(rooms.begin(), rooms.end(), [](const Room& r) { return !r.isOccupied(); }) << "/" << rooms.size() << "\n\n";
    }

    ~HotelSystem() {
        cout << "\nSaving all data...\n";
        saveAll();
        cout << "Data saved successfully. Thank you for using Hotel Management System!\n";
    }

    void addCustomer() {
        cin.ignore(); // Clear input buffer
        string name = getValidString("Enter customer name: ");
        string phone = getValidString("Enter phone number: ");
        string address = getValidString("Enter address: ");

        customers.push_back(Customer(nextCustomerId++, name, phone, address));
        cout << "\n✓ Customer added successfully! Customer ID: " << (nextCustomerId - 1) << endl;
        saveAll();
    }

    void checkIn() {
        int custId = getValidInt("Enter Customer ID: ", 1000, 999999);

        Customer* customer = findCustomer(custId);
        if(!customer) {
            cout << "✗ Customer not found!\n";
            return;
        }

        if(customer->getRoomNumber() != 0) {
            cout << "✗ Customer is already checked in to room " << customer->getRoomNumber() << "!\n";
            return;
        }

        displayAvailableRooms();
        if (count_if(rooms.begin(), rooms.end(), [](const Room& r) { return !r.isOccupied(); }) == 0) {
            cout << "✗ No rooms available for check-in!\n";
            return;
        }

        int roomNum = getValidInt("Enter Room Number: ", 100, 999);

        Room* room = findRoom(roomNum);
        if(!room) {
            cout << "✗ Room not found!\n";
            return;
        }
        
        if(room->isOccupied()) {
            cout << "✗ Room is already occupied!\n";
            return;
        }

        string date = getValidDate("Enter check-in date (DD/MM/YYYY): ");

        room->checkIn(custId);
        customer->setRoomNumber(roomNum);
        customer->setCheckInDate(date);

        cout << "\n✓ Check-in successful!\n";
        cout << "Customer: " << customer->getName() << " | Room: " << roomNum << " | Date: " << date << endl;
        saveAll();
    }

    void checkOut() {
        int custId = getValidInt("Enter Customer ID: ", 1000, 999999);

        Customer* customer = findCustomer(custId);
        if(!customer || customer->getRoomNumber() == 0) {
            cout << "✗ Customer not found or not currently checked in!\n";
            return;
        }

        Room* room = findRoom(customer->getRoomNumber());
        if (!room) {
            cout << "✗ Error: Room data corrupted!\n";
            return;
        }

        cout << "Customer: " << customer->getName() << " | Current Room: " << room->getNumber() 
             << " | Check-in Date: " << customer->getCheckInDate() << endl;

        string date = getValidCheckOutDate("Enter check-out date (DD/MM/YYYY): ", customer->getCheckInDate());

        // Calculate nights stayed and generate bill
        int nights = calculateNights(customer->getCheckInDate(), date);
        double totalRoomCost = room->getPrice() * nights;
        
        bills.push_back(Bill(nextBillId++, custId, room->getNumber(), room->getPrice(), nights, 
                           customer->getCheckInDate(), date));

        // Process checkout
        room->checkOut();
        customer->setCheckOutDate(date);
        customer->setRoomNumber(0);

        cout << "\n✓ Check-out successful!\n";
        cout << "Nights stayed: " << nights << " | Total room charges: $" << fixed << setprecision(2) << totalRoomCost << endl;
        bills.back().display();
        saveAll();
    }

    void displayAvailableRooms() {
        cout << "\n========== AVAILABLE ROOMS ==========\n";
        bool found = false;
        for(const auto& r : rooms) {
            if(!r.isOccupied()) {
                r.display();
                found = true;
            }
        }
        if(!found) {
            cout << "No rooms available at the moment!\n";
        }
        cout << "=====================================\n";
    }

    void displayAllRooms() {
        cout << "\n============ ALL ROOMS ============\n";
        for(const auto& r : rooms) {
            r.display();
            if(r.isOccupied()) {
                Customer* customer = findCustomer(r.getCustomerId());
                if(customer) {
                    cout << "   └─ Occupied by: " << customer->getName() << " (ID: " << customer->getId() << ")\n";
                }
            }
        }
        cout << "===================================\n";
    }

    void displayCustomers() {
        cout << "\n========== ALL CUSTOMERS ==========\n";
        if(customers.empty()) {
            cout << "No customers registered!\n";
        } else {
            for(const auto& c : customers) {
                c.display();
            }
        }
        cout << "===================================\n";
    }

    void displayBills() {
        cout << "\n============ ALL BILLS ============\n";
        if(bills.empty()) {
            cout << "No bills generated yet!\n";
        } else {
            double totalRevenue = 0.0;
            int paidBills = 0;
            for(const auto& b : bills) {
                b.display();
                if(b.isPaid()) {
                    totalRevenue += b.getTotal();
                    paidBills++;
                }
            }
            cout << "\n--- SUMMARY ---\n";
            cout << "Total Bills: " << bills.size() << " | Paid: " << paidBills << " | Unpaid: " << (bills.size() - paidBills) << endl;
            cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << endl;
        }
        cout << "===================================\n";
    }

    void payBill() {
        int billId = getValidInt("Enter Bill ID: ", 5000, 999999);

        Bill* bill = findBill(billId);
        if(!bill) {
            cout << "✗ Bill not found!\n";
            return;
        }

        if(bill->isPaid()) {
            cout << "✗ Bill has already been paid!\n";
            return;
        }

        bill->display();
        cout << "\nConfirm payment of $" << fixed << setprecision(2) << bill->getTotal() << "? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if(confirm == 'y' || confirm == 'Y') {
            bill->pay();
            cout << "\n✓ Payment processed successfully!\n";
            saveAll();
        } else {
            cout << "Payment cancelled.\n";
        }
    }

    void searchCustomer() {
        cin.ignore();
        string searchTerm = getValidString("Enter customer name or phone to search: ");
        
        cout << "\n========== SEARCH RESULTS ==========\n";
        bool found = false;
        for(const auto& c : customers) {
            if(c.getName().find(searchTerm) != string::npos || c.getPhone().find(searchTerm) != string::npos) {
                c.display();
                found = true;
            }
        }
        if(!found) {
            cout << "No customers found matching '" << searchTerm << "'\n";
        }
        cout << "====================================\n";
    }

    void showMenu() {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Add Customer           6. All Bills\n";
        cout << "2. Check-In               7. Pay Bill\n";
        cout << "3. Check-Out              8. Search Customer\n";
        cout << "4. Available Rooms        9. All Rooms Status\n";
        cout << "5. All Customers          0. Exit\n";
        cout << "===============================\n";
        cout << "Enter your choice: ";
    }

    void run() {
        int choice;
        do {
            showMenu();
            choice = getValidInt("", 0, 9);
            cout << endl;
            
            switch(choice) {
                case 1: addCustomer(); break;
                case 2: checkIn(); break;
                case 3: checkOut(); break;
                case 4: displayAvailableRooms(); break;
                case 5: displayCustomers(); break;
                case 6: displayBills(); break;
                case 7: payBill(); break;
                case 8: searchCustomer(); break;
                case 9: displayAllRooms(); break;
                case 0: cout << "Thank you for using Hotel Management System!\n"; break;
                default: cout << "Invalid choice! Please try again.\n";
            }
            
            if(choice != 0) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
        } while(choice != 0);
    }
};

int main() {
    try {
        HotelSystem hotel;
        hotel.run();
    } catch (const exception& e) {
        cout << "An error occurred: " << e.what() << endl;
        return 1;
    } catch (...) {
        cout << "An unknown error occurred." << endl;
        return 1;
    }
    return 0;
}
