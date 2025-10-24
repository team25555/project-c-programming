// dorm_system.cpp
// Full Dormitory Management System (file-based)
// Features: Room / Tenant / Contract / Utility / Invoice / Payment / Admin / User / Report

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <map>
#include <ctime>
#include <set>
#include <cmath>

using namespace std;

// ---------- Helpers ----------
vector<string> split(const string &s, char delim='|'){
    vector<string> out;
    string token;
    stringstream ss(s);
    while(getline(ss, token, delim)) out.push_back(token);
    return out;
}
string join(const vector<string>& parts, char delim='|'){
    string out;
    for(size_t i=0;i<parts.size();++i){
        out += parts[i];
        if(i+1<parts.size()) out += delim;
    }
    return out;
}
string trim(const string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    if(a==string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}
string genID(const string &prefix){
    static int seq = 0;
    seq++;
    stringstream ss;
    ss << prefix << (time(nullptr) % 100000) << seq;
    return ss.str();
}

// ---------- Filenames ----------
const string ROOM_FILE = "Room.dat";
const string TENANT_FILE = "Tenant.dat";
const string CONTRACT_FILE = "Contract.dat";
const string UTILITY_FILE = "Utility.dat";
const string INVOICE_FILE = "Invoice.dat";
const string PAYMENT_FILE = "Payment.dat";
const string ADMIN_FILE = "Admin.dat";
const string REPORT_FILE = "report.txt";

// ---------- Structs ----------
struct Room {
    string roomNo;
    string type;     // Single/Double/Suite
    string status;   // Available/Occupied/Maintenance
};

struct Tenant {
    string tenantID;
    string name;
    string phone;
    string citizenID;
    string birthDate;
    string address;
    string roomNo; // room assigned ("" if none)
};

struct Contract {
    string contractID;
    string tenantID;
    string roomNo;
    string startDate; // YYYY-MM-DD
    string endDate;   // YYYY-MM-DD
    double roomPrice;
    double internetFee;
};

struct Utility {
    string roomNo;
    string month; // MM
    string year;  // YYYY
    int prevWater;
    int currWater;
    int prevElectric;
    int currElectric;
    double waterRate;
    double electricRate;
};

struct Invoice {
    string invoiceID;
    string contractID;
    string roomNo;
    string month; // MM
    string year;  // YYYY
    double roomPrice;
    double internetFee;
    double waterBill;
    double electricBill;
    double total;
    string status; // UNPAID/PAID
};

struct Payment {
    string invoiceID;
    double amount;
    string date; // YYYY-MM-DD
};

struct Admin {
    string username;
    string password;
};

// ---------- Load / Save ----------
vector<Room> loadRooms(){
    vector<Room> v;
    ifstream f(ROOM_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=3) v.push_back(Room{p[0], p[1], p[2]});
    }
    return v;
}
void saveRooms(const vector<Room>& v){
    ofstream f(ROOM_FILE, ios::trunc);
    for(auto &r: v) f << join({r.roomNo, r.type, r.status}) << "\n";
}

vector<Tenant> loadTenants(){
    vector<Tenant> v;
    ifstream f(TENANT_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=7) v.push_back(Tenant{p[0], p[1], p[2], p[3], p[4], p[5], p[6]});
    }
    return v;
}
void saveTenants(const vector<Tenant>& v){
    ofstream f(TENANT_FILE, ios::trunc);
    for(auto &t: v) f << join({t.tenantID, t.name, t.phone, t.citizenID, t.birthDate, t.address, t.roomNo}) << "\n";
}

vector<Contract> loadContracts(){
    vector<Contract> v;
    ifstream f(CONTRACT_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=7) v.push_back(Contract{p[0], p[1], p[2], p[3], p[4], stod(p[5]), stod(p[6])});
    }
    return v;
}
void saveContracts(const vector<Contract>& v){
    ofstream f(CONTRACT_FILE, ios::trunc);
    for(auto &c: v) f << join({c.contractID, c.tenantID, c.roomNo, c.startDate, c.endDate, to_string(c.roomPrice), to_string(c.internetFee)}) << "\n";
}

vector<Utility> loadUtilities(){
    vector<Utility> v;
    ifstream f(UTILITY_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=9){
            v.push_back(Utility{p[0], p[1], p[2], stoi(p[3]), stoi(p[4]), stoi(p[5]), stoi(p[6]), stod(p[7]), stod(p[8])});
        }
    }
    return v;
}
void saveUtilities(const vector<Utility>& v){
    ofstream f(UTILITY_FILE, ios::trunc);
    for(auto &u: v) f << join({u.roomNo, u.month, u.year,
                               to_string(u.prevWater), to_string(u.currWater),
                               to_string(u.prevElectric), to_string(u.currElectric),
                               to_string(u.waterRate), to_string(u.electricRate)}) << "\n";
}

vector<Invoice> loadInvoices(){
    vector<Invoice> v;
    ifstream f(INVOICE_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=11){
            v.push_back(Invoice{p[0], p[1], p[2], p[3], p[4], stod(p[5]), stod(p[6]), stod(p[7]), stod(p[8]), stod(p[9]), p[10]});
        }
    }
    return v;
}
void saveInvoices(const vector<Invoice>& v){
    ofstream f(INVOICE_FILE, ios::trunc);
    for(auto &inv: v){
        f << join({inv.invoiceID, inv.contractID, inv.roomNo, inv.month, inv.year,
                   to_string(inv.roomPrice), to_string(inv.internetFee),
                   to_string(inv.waterBill), to_string(inv.electricBill),
                   to_string(inv.total), inv.status}) << "\n";
    }
}

vector<Payment> loadPayments(){
    vector<Payment> v;
    ifstream f(PAYMENT_FILE);
    string line;
    while(getline(f, line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=3) v.push_back(Payment{p[0], stod(p[1]), p[2]});
    }
    return v;
}
void savePayments(const vector<Payment>& v){
    ofstream f(PAYMENT_FILE, ios::trunc);
    for(auto &p: v) f << join({p.invoiceID, to_string(p.amount), p.date}) << "\n";
}

vector<Admin> loadAdmins(){
    vector<Admin> v;
    ifstream f(ADMIN_FILE);
    string line;
    while(getline(f,line)){
        if(line.empty()) continue;
        auto p = split(line);
        if(p.size()>=2) v.push_back(Admin{p[0], p[1]});
    }
    return v;
}
void saveAdmins(const vector<Admin>& v){
    ofstream f(ADMIN_FILE, ios::trunc);
    for(auto &a: v) f << join({a.username, a.password}) << "\n";
}

// ---------- Finders ----------
Room* findRoom(vector<Room>& rooms, const string &roomNo){
    for(auto &r: rooms) if(r.roomNo==roomNo) return &r;
    return nullptr;
}
Tenant* findTenant(vector<Tenant>& tenants, const string &tenantID){
    for(auto &t: tenants) if(t.tenantID==tenantID) return &t;
    return nullptr;
}
Contract* findContract(vector<Contract>& contracts, const string &contractID){
    for(auto &c: contracts) if(c.contractID==contractID) return &c;
    return nullptr;
}
Invoice* findInvoice(vector<Invoice>& invs, const string &invoiceID){
    for(auto &i: invs) if(i.invoiceID==invoiceID) return &i;
    return nullptr;
}

// ---------- Room Management ----------
void RoomManagement(){
    vector<Room> rooms = loadRooms();
    while(true){
        cout << "\n--- Room Management ---\n";
        cout << "1) Add Room\n2) Edit Room\n3) Delete Room\n4) List All Rooms\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveRooms(rooms); break; }
        if(c==1){
            Room r;
            cout << "Room No: "; cin >> r.roomNo;
            cout << "Type (Single/Double/Suite) : "; cin >> ws; getline(cin, r.type);
            cout << "Status (Available/Occupied/Maintenance): "; cin >> ws; getline(cin, r.status);
            rooms.push_back(r);
            cout << "Added.\n";
        } else if(c==2){
            string rn; cout << "Room No to edit: "; cin >> rn;
            Room* pr = findRoom(rooms, rn);
            if(!pr){ cout << "Not found.\n"; }
            else {
                cout << "New Type (enter to keep: " << pr->type << "): "; string tmp; cin.ignore(); getline(cin,tmp);
                if(trim(tmp)!="") pr->type = tmp;
                cout << "New Status (enter to keep: " << pr->status << "): "; getline(cin,tmp);
                if(trim(tmp)!="") pr->status = tmp;
                cout << "Updated.\n";
            }
        } else if(c==3){
            string rn; cout << "Room No to delete: "; cin >> rn;
            auto it = remove_if(rooms.begin(), rooms.end(), [&](const Room& r){ return r.roomNo==rn; });
            if(it!=rooms.end()){ rooms.erase(it, rooms.end()); cout << "Deleted.\n"; }
            else cout << "Not found.\n";
        } else if(c==4){
            cout << left << setw(10) << "RoomNo" << setw(15) << "Type" << setw(12) << "Status" << "\n";
            cout << string(37,'-') << "\n";
            for(auto &r: rooms) cout << left << setw(10) << r.roomNo << setw(15) << r.type << setw(12) << r.status << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Tenant Management ----------
void TenantManagement(){
    vector<Tenant> tenants = loadTenants();
    while(true){
        cout << "\n--- Tenant Management ---\n";
        cout << "1) Add Tenant\n2) Edit Tenant\n3) Delete Tenant\n4) Find Tenant\n5) List Tenants\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveTenants(tenants); break; }
        if(c==1){
            Tenant t;
            t.tenantID = genID("T");
            cout << "Name: "; cin >> ws; getline(cin, t.name);
            cout << "Phone: "; cin >> t.phone;
            cout << "CitizenID: "; cin >> t.citizenID;
            cout << "BirthDate (YYYY-MM-DD): "; cin >> ws; getline(cin, t.birthDate);
            cout << "Address: "; cin >> ws; getline(cin, t.address);
            cout << "RoomNo (or leave empty): "; cin >> ws; getline(cin, t.roomNo);
            tenants.push_back(t);
            cout << "Added Tenant ID = " << t.tenantID << "\n";
        } else if(c==2){
            string id; cout << "TenantID to edit: "; cin >> id;
            Tenant* pt = findTenant(tenants, id);
            if(!pt) cout << "Not found.\n";
            else {
                cout << "New Name (empty to keep): "; string tmp; cin.ignore(); getline(cin,tmp); if(trim(tmp)!="") pt->name = tmp;
                cout << "New Phone (empty to keep): "; getline(cin,tmp); if(trim(tmp)!="") pt->phone = tmp;
                cout << "New Address (empty to keep): "; getline(cin,tmp); if(trim(tmp)!="") pt->address = tmp;
                cout << "Updated.\n";
            }
        } else if(c==3){
            string id; cout << "TenantID to delete: "; cin >> id;
            auto it = remove_if(tenants.begin(), tenants.end(), [&](const Tenant& x){ return x.tenantID==id; });
            if(it!=tenants.end()){ tenants.erase(it, tenants.end()); cout << "Deleted.\n"; } else cout << "Not found.\n";
        } else if(c==4){
            cout << "Search by (1) Name (2) TenantID (3) RoomNo: "; int s; cin >> s;
            if(s==1){
                cout << "Enter name: "; string q; cin >> ws; getline(cin,q);
                for(auto &t: tenants) if(t.name.find(q)!=string::npos) cout << t.tenantID << " | " << t.name << " | " << t.roomNo << "\n";
            } else if(s==2){
                cout << "Enter TenantID: "; string q; cin >> q;
                for(auto &t: tenants) if(t.tenantID==q) cout << t.tenantID << " | " << t.name << " | " << t.roomNo << "\n";
            } else {
                cout << "Enter RoomNo: "; string q; cin >> q;
                for(auto &t: tenants) if(t.roomNo==q) cout << t.tenantID << " | " << t.name << " | " << t.roomNo << "\n";
            }
        } else if(c==5){
            cout << left << setw(12) << "TenantID" << setw(20) << "Name" << setw(12) << "Phone" << setw(12) << "RoomNo" << "\n";
            cout << string(56,'-') << "\n";
            for(auto &t: tenants) cout << left << setw(12) << t.tenantID << setw(20) << t.name << setw(12) << t.phone << setw(12) << t.roomNo << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Contract Management ----------
void ContractManagement(){
    vector<Contract> contracts = loadContracts();
    vector<Room> rooms = loadRooms();
    vector<Tenant> tenants = loadTenants();

    while(true){
        cout << "\n--- Contract Management ---\n";
        cout << "1) Add Contract\n2) Edit Contract\n3) End/Delete Contract\n4) List Contracts\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveContracts(contracts); saveRooms(rooms); break; }
        if(c==1){
            Contract co;
            co.contractID = genID("C");
            cout << "TenantID: "; cin >> co.tenantID;
            cout << "RoomNo: "; cin >> co.roomNo;
            cout << "StartDate (YYYY-MM-DD): "; cin >> co.startDate;
            cout << "EndDate (YYYY-MM-DD): "; cin >> co.endDate;
            cout << "RoomPrice: "; cin >> co.roomPrice;
            cout << "InternetFee: "; cin >> co.internetFee;
            // update room status
            Room* pr = findRoom(rooms, co.roomNo);
            if(pr) pr->status = "Occupied";
            // also assign to tenant
            for(auto &t: tenants) if(t.tenantID==co.tenantID) t.roomNo = co.roomNo;
            contracts.push_back(co);
            cout << "Contract added ID: " << co.contractID << "\n";
        } else if(c==2){
            string id; cout << "ContractID to edit: "; cin >> id;
            Contract* pc = findContract(contracts, id);
            if(!pc) cout << "Not found.\n";
            else {
                cout << "New EndDate (current " << pc->endDate << "): "; string tmp; cin >> tmp; if(trim(tmp)!="") pc->endDate = tmp;
                cout << "New RoomPrice (current " << pc->roomPrice << "): "; string tmp2; cin >> tmp2; if(trim(tmp2)!="") pc->roomPrice = stod(tmp2);
                cout << "New InternetFee (current " << pc->internetFee << "): "; string tmp3; cin >> tmp3; if(trim(tmp3)!="") pc->internetFee = stod(tmp3);
                cout << "Updated.\n";
            }
        } else if(c==3){
            string id; cout << "ContractID to end/delete: "; cin >> id;
            auto it = remove_if(contracts.begin(), contracts.end(), [&](const Contract& x){ return x.contractID==id; });
            if(it==contracts.end()){ cout << "Not found.\n"; }
            else {
                contracts.erase(it, contracts.end());
                // rebuild room statuses based on remaining contracts:
                for(auto &r: rooms) r.status = "Available";
                for(auto &c2: contracts) for(auto &r: rooms) if(r.roomNo==c2.roomNo) r.status = "Occupied";
                // clear tenants room if not in any contract
                for(auto &t: tenants){
                    bool has=false;
                    for(auto &c2: contracts) if(c2.tenantID==t.tenantID) has=true;
                    if(!has) t.roomNo = "";
                }
                cout << "Contract removed and statuses updated.\n";
            }
        } else if(c==4){
            cout << left << setw(12) << "ContractID" << setw(10) << "TenantID" << setw(8) << "RoomNo" 
                 << setw(12) << "Start" << setw(12) << "End" << setw(10) << "Price" << "\n";
            cout << string(70,'-') << "\n";
            for(auto &c2: contracts){
                cout << left << setw(12) << c2.contractID << setw(10) << c2.tenantID << setw(8) << c2.roomNo 
                     << setw(12) << c2.startDate << setw(12) << c2.endDate << setw(10) << c2.roomPrice << "\n";
            }
        } else cout << "Invalid.\n";
    }
}

// ---------- Utility Calculation ----------
void UtilityCalculation(){
    vector<Utility> utils = loadUtilities();
    while(true){
        cout << "\n--- Utility Calculation ---\n";
        cout << "1) Add/Update Utility Reading\n2) Calculate Units for a Room (month/year)\n3) List Utilities\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveUtilities(utils); break; }
        if(c==1){
            Utility u;
            cout << "RoomNo: "; cin >> u.roomNo;
            cout << "Month (MM): "; cin >> u.month;
            cout << "Year (YYYY): "; cin >> u.year;
            cout << "Previous Water: "; cin >> u.prevWater;
            cout << "Current Water: "; cin >> u.currWater;
            cout << "Previous Electric: "; cin >> u.prevElectric;
            cout << "Current Electric: "; cin >> u.currElectric;
            cout << "Water Rate per unit: "; cin >> u.waterRate;
            cout << "Electric Rate per unit: "; cin >> u.electricRate;
            // replace if exists same room+month+year
            bool found=false;
            for(auto &x: utils) if(x.roomNo==u.roomNo && x.month==u.month && x.year==u.year){ x=u; found=true; break; }
            if(!found) utils.push_back(u);
            cout << "Saved readings.\n";
        } else if(c==2){
            string rn, mo, yr; cout << "RoomNo: "; cin >> rn; cout << "Month: "; cin >> mo; cout << "Year: "; cin >> yr;
            bool found=false;
            for(auto &u: utils) if(u.roomNo==rn && u.month==mo && u.year==yr){
                int wUnits = u.currWater - u.prevWater;
                int eUnits = u.currElectric - u.prevElectric;
                double wBill = wUnits * u.waterRate;
                double eBill = eUnits * u.electricRate;
                cout << "Water units: " << wUnits << " -> Bill: " << wBill << "\n";
                cout << "Electric units: " << eUnits << " -> Bill: " << eBill << "\n";
                found=true;
                break;
            }
            if(!found) cout << "No utility reading for that room/month.\n";
        } else if(c==3){
            cout << left << setw(8) << "Room" << setw(6) << "MM" << setw(6) << "YYYY" << setw(8) << "PrevW" << setw(8) << "CurW" << setw(8) << "PrevE" << setw(8) << "CurE" << setw(8) << "WRate" << setw(8) << "ERate" << "\n";
            cout << string(84,'-') << "\n";
            for(auto &u: utils) cout << left << setw(8) << u.roomNo << setw(6) << u.month << setw(6) << u.year << setw(8) << u.prevWater << setw(8) << u.currWater << setw(8) << u.prevElectric << setw(8) << u.currElectric << setw(8) << u.waterRate << setw(8) << u.electricRate << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Invoice Calculation ----------
void InvoiceCalculation(){
    vector<Contract> contracts = loadContracts();
    vector<Utility> utils = loadUtilities();
    vector<Invoice> invoices = loadInvoices();

    while(true){
        cout << "\n--- Invoice Calculation ---\n";
        cout << "1) Create Invoice for Contract (month/year)\n2) List Invoices\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveInvoices(invoices); break; }
        if(c==1){
            string cid; cout << "ContractID: "; cin >> cid;
            Contract* pc = findContract(contracts, cid);
            if(!pc){ cout << "Contract not found.\n"; continue; }
            string mo, yr; cout << "Month (MM): "; cin >> mo; cout << "Year (YYYY): "; cin >> yr;
            double waterBill=0, electricBill=0;
            for(auto &u: utils) if(u.roomNo==pc->roomNo && u.month==mo && u.year==yr){
                int wUnits = u.currWater - u.prevWater;
                int eUnits = u.currElectric - u.prevElectric;
                waterBill = wUnits * u.waterRate;
                electricBill = eUnits * u.electricRate;
                break;
            }
            Invoice inv;
            inv.invoiceID = genID("I");
            inv.contractID = pc->contractID;
            inv.roomNo = pc->roomNo;
            inv.month = mo;
            inv.year = yr;
            inv.roomPrice = pc->roomPrice;
            inv.internetFee = pc->internetFee;
            inv.waterBill = waterBill;
            inv.electricBill = electricBill;
            inv.total = inv.roomPrice + inv.internetFee + inv.waterBill + inv.electricBill;
            inv.status = "UNPAID";
            invoices.push_back(inv);
            cout << "Invoice created ID: " << inv.invoiceID << " Total: " << inv.total << "\n";
        } else if(c==2){
            cout << left << setw(10) << "InvoiceID" << setw(10) << "Contract" << setw(8) << "Room" << setw(8) << "MM" << setw(8) << "YYYY" << setw(10) << "Total" << setw(8) << "Status" << "\n";
            cout << string(70,'-') << "\n";
            for(auto &inv: invoices) cout << left << setw(10) << inv.invoiceID << setw(10) << inv.contractID << setw(8) << inv.roomNo << setw(8) << inv.month << setw(8) << inv.year << setw(10) << inv.total << setw(8) << inv.status << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Payment Checking ----------
void PaymentChecking(){
    vector<Invoice> invoices = loadInvoices();
    vector<Payment> payments = loadPayments();
    while(true){
        cout << "\n--- Payment Checking ---\n";
        cout << "1) Find Invoice\n2) Mark Invoice PAID\n3) List Payments\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveInvoices(invoices); savePayments(payments); break; }
        if(c==1){
            string id; cout << "InvoiceID: "; cin >> id;
            Invoice* pi = findInvoice(invoices, id);
            if(!pi) cout << "Not found.\n";
            else {
                cout << "Invoice " << pi->invoiceID << " Total: " << pi->total << " Status: " << pi->status << "\n";
            }
        } else if(c==2){
            string id; cout << "InvoiceID to mark PAID: "; cin >> id;
            Invoice* pi = findInvoice(invoices, id);
            if(!pi){ cout << "Not found.\n"; continue; }
            if(pi->status=="PAID"){ cout << "Already PAID.\n"; continue; }
            double amt; cout << "Amount received: "; cin >> amt;
            string date; cout << "Date (YYYY-MM-DD): "; cin >> date;
            pi->status = "PAID";
            payments.push_back(Payment{id, amt, date});
            cout << "Marked PAID and recorded payment.\n";
        } else if(c==3){
            cout << left << setw(12) << "InvoiceID" << setw(10) << "Amount" << setw(12) << "Date" << "\n";
            cout << string(36,'-') << "\n";
            for(auto &p: payments) cout << left << setw(12) << p.invoiceID << setw(10) << p.amount << setw(12) << p.date << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Admin Management ----------
void AdminManagement(){
    vector<Admin> admins = loadAdmins();
    while(true){
        cout << "\n--- Admin Management ---\n";
        cout << "1) Create Admin\n2) Edit Admin Password\n3) Delete Admin\n4) List Admins\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0){ saveAdmins(admins); break; }
        if(c==1){
            Admin a;
            cout << "Username: "; cin >> a.username;
            cout << "Password: "; cin >> a.password;
            admins.push_back(a);
            cout << "Admin created.\n";
        } else if(c==2){
            string name; cout << "Username to edit: "; cin >> name;
            bool found=false;
            for(auto &a: admins) if(a.username==name){ cout << "New password: "; cin >> a.password; found=true; break; }
            if(!found) cout << "Not found.\n";
            else cout << "Updated.\n";
        } else if(c==3){
            string name; cout << "Username to delete: "; cin >> name;
            auto it = remove_if(admins.begin(), admins.end(), [&](const Admin& x){ return x.username==name; });
            if(it!=admins.end()){ admins.erase(it, admins.end()); cout << "Deleted.\n"; } else cout << "Not found.\n";
        } else if(c==4){
            cout << left << setw(15) << "Username" << "\n";
            cout << string(15,'-') << "\n";
            for(auto &a: admins) cout << left << setw(15) << a.username << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- User Management (Tenant View) ----------
void UserManagement(){
    vector<Tenant> tenants = loadTenants();
    vector<Invoice> invoices = loadInvoices();
    cout << "\n--- Tenant (User) View ---\n";
    cout << "Enter your TenantID: "; string id; cin >> id;
    Tenant* pt = findTenant(tenants, id);
    if(!pt){ cout << "Tenant not found.\n"; return; }
    cout << "Welcome, " << pt->name << " Room: " << pt->roomNo << "\n";
    while(true){
        cout << "1) View Profile\n2) View My Invoices\n0) Back\nChoose: ";
        int c; cin >> c;
        if(c==0) break;
        if(c==1){
            cout << "TenantID: " << pt->tenantID << "\nName: " << pt->name << "\nPhone: " << pt->phone << "\nCitizenID: " << pt->citizenID << "\nBirthDate: " << pt->birthDate << "\nAddress: " << pt->address << "\nRoomNo: " << pt->roomNo << "\n";
        } else if(c==2){
            cout << left << setw(10) << "InvoiceID" << setw(10) << "MM" << setw(10) << "YYYY" << setw(10) << "Total" << setw(8) << "Status" << "\n";
            cout << string(48,'-') << "\n";
            for(auto &inv: invoices) if(inv.roomNo==pt->roomNo) cout << left << setw(10) << inv.invoiceID << setw(10) << inv.month << setw(10) << inv.year << setw(10) << inv.total << setw(8) << inv.status << "\n";
        } else cout << "Invalid.\n";
    }
}

// ---------- Report Management ----------
void ReportManagement(){
    // Load payments (to compute actual received income by payment.date)
    vector<Payment> payments = loadPayments();
    vector<Invoice> invoices = loadInvoices();
    vector<Utility> utils = loadUtilities();

    // monthly income from payments (by payment date YYYY-MM-DD)
    map<string, double> incomeByMonth; // key = MM/YYYY
    for(auto &p: payments){
        if(p.date.size() >= 7){
            // expect YYYY-MM-DD -> take MM and YYYY
            string year = p.date.substr(0,4);
            string month = p.date.substr(5,2);
            string key = month + "/" + year;
            incomeByMonth[key] += p.amount;
        }
    }
    // Also include unpaid invoices? We'll compute "invoiced" totals separately
    map<string, double> invoicedByMonth;
    for(auto &inv: invoices){
        string key = inv.month + "/" + inv.year;
        invoicedByMonth[key] += inv.total;
    }

    // Utility stats per month (aggregate units per room-month)
    struct Stat { vector<int> waterUnits; vector<int> electricUnits; };
    map<string, Stat> utilStats;
    for(auto &u: utils){
        string key = u.month + "/" + u.year;
        int w = u.currWater - u.prevWater;
        int e = u.currElectric - u.prevElectric;
        utilStats[key].waterUnits.push_back(w);
        utilStats[key].electricUnits.push_back(e);
    }

    // Print report header
    cout << "\n========== Monthly Report ==========\n";
    cout << left << setw(10) << "Month" << setw(15) << "Invoiced" << setw(15) << "Received" << setw(12) << "AvgW" << setw(12) << "AvgE" << setw(10) << "MaxW" << setw(10) << "MaxE" << "\n";
    cout << string(84,'-') << "\n";

    // Build set of all months present
    set<string> months;
    for(auto &p: incomeByMonth) months.insert(p.first);
    for(auto &p: invoicedByMonth) months.insert(p.first);
    for(auto &p: utilStats) months.insert(p.first);

    for(auto &m: months){
        double invoiced = invoicedByMonth.count(m) ? invoicedByMonth[m] : 0.0;
        double received = incomeByMonth.count(m) ? incomeByMonth[m] : 0.0;
        double avgW=0, avgE=0; int maxW=0, maxE=0, minW=0, minE=0;
        if(utilStats.count(m)){
            auto &vecW = utilStats[m].waterUnits;
            auto &vecE = utilStats[m].electricUnits;
            if(!vecW.empty()){
                avgW = accumulate(vecW.begin(), vecW.end(), 0.0) / vecW.size();
                maxW = *max_element(vecW.begin(), vecW.end());
                minW = *min_element(vecW.begin(), vecW.end());
            }
            if(!vecE.empty()){
                avgE = accumulate(vecE.begin(), vecE.end(), 0.0) / vecE.size();
                maxE = *max_element(vecE.begin(), vecE.end());
                minE = *min_element(vecE.begin(), vecE.end());
            }
        }
        cout << left << setw(10) << m << setw(15) << fixed << setprecision(2) << invoiced << setw(15) << fixed << setprecision(2) << received
             << setw(12) << (avgW>0? to_string((int)round(avgW)) : string("-"))
             << setw(12) << (avgE>0? to_string((int)round(avgE)) : string("-"))
             << setw(10) << (maxW>0? to_string(maxW) : string("-"))
             << setw(10) << (maxE>0? to_string(maxE) : string("-"))
             << "\n";
    }
    cout << string(84,'-') << "\n";

    // Save textual report file
    ofstream rf(REPORT_FILE, ios::trunc);
    rf << "========== Monthly Report ==========\n";
    rf << left << setw(10) << "Month" << setw(15) << "Invoiced" << setw(15) << "Received" << setw(12) << "AvgW" << setw(12) << "AvgE" << setw(10) << "MaxW" << setw(10) << "MaxE" << "\n";
    rf << string(84,'-') << "\n";
    for(auto &m: months){
        double invoiced = invoicedByMonth.count(m) ? invoicedByMonth[m] : 0.0;
        double received = incomeByMonth.count(m) ? incomeByMonth[m] : 0.0;
        double avgW=0, avgE=0; int maxW=0, maxE=0;
        if(utilStats.count(m)){
            auto &vecW = utilStats[m].waterUnits;
            auto &vecE = utilStats[m].electricUnits;
            if(!vecW.empty()){
                avgW = accumulate(vecW.begin(), vecW.end(), 0.0) / vecW.size();
                maxW = *max_element(vecW.begin(), vecW.end());
            }
            if(!vecE.empty()){
                avgE = accumulate(vecE.begin(), vecE.end(), 0.0) / vecE.size();
                maxE = *max_element(vecE.begin(), vecE.end());
            }
        }
        rf << left << setw(10) << m << setw(15) << fixed << setprecision(2) << invoiced << setw(15) << fixed << setprecision(2) << received
           << setw(12) << (avgW>0? to_string((int)round(avgW)) : string("-"))
           << setw(12) << (avgE>0? to_string((int)round(avgE)) : string("-"))
           << setw(10) << (maxW>0? to_string(maxW) : string("-"))
           << setw(10) << (maxE>0? to_string(maxE) : string("-"))
           << "\n";
    }
    rf << string(84,'-') << "\n";
    rf.close();

    cout << "Report saved to '" << REPORT_FILE << "'\n";
}

// ---------- Program Entry ----------
int main(){
    // Ensure admin exists (if none, create default admin/admin)
    vector<Admin> admins = loadAdmins();
    if(admins.empty()){
        admins.push_back(Admin{"admin","admin"});
        saveAdmins(admins);
        cout << "Default admin created: admin / admin\n";
    }

    while(true){
        cout << "\n========== Dormitory Management System ==========\n";
        cout << "1) Room Management \n2) Tenant Management\n3) Contract Management\n4) Utility Calculation\n5) Invoice Calculation\n6) Payment Checking\n7) User (Tenant) View\n8) Report / Statistics\n9) AdminManagement\n0) Exit\nChoose: ";
        int c; cin >> c;
        switch(c){
            case 1: RoomManagement(); break;
            case 2: TenantManagement(); break;
            case 3: ContractManagement(); break;
            case 4: UtilityCalculation(); break;
            case 5: InvoiceCalculation(); break;
            case 6: PaymentChecking(); break;
            case 7: UserManagement(); break;
            case 8: ReportManagement(); break;
            case 9: AdminManagement(); break;
            case 0: cout << "Exit.\n"; return 0;
            default: cout << "Invalid option.\n"; break;
        }
    }
    return 0;
}
