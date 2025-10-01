// 6806021410124 ธนวิชญ์ ดอบุตร
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

int menu();

void Addstudent(string FN); // ฟังก์ชัน Addstudent เพิ่มข้อมูล
void DisplayStudent(string FN); // ฟังก์ชัน displaystudent แสดงรายชื่อ
void FindName(string FN); // ฟังก์ชัน findname ค้นหาชื่อ 


char calGrade(int score); // ฟังก์ชัน ใช้คำนวณเกรด

int main() {
    const string filename = "student.dat"; // ข้อมูลจะเก็บที่ไฟล์ student.dat 
    ifstream infile; // การอ่าน
    ofstream outfile; // การเขียน

    int c; // กำหนดตัวแปร c เป็นตัวแปร switch case
    do {
        c = menu();  
        switch (c) {
        case 1: Addstudent(filename); break; // เคส 1 จะทำงาน ฟังก์ชัน addstudent
        case 2: DisplayStudent(filename); break; // เคส 2 จะทำงาน ฟังก์ชัน displaystudent
        case 3: FindName(filename); break; // เคส 3 จะทำงาน ฟังก์ชัน findname
        default: cout << "must be 0,1,2,3 try again" << endl; // แจ้งเตือนเมื่อไม่เลือกตามเคส
        }
    } while (c!= 0);   

    cout << "Exit program" << endl;  // เลือก 0 จะออกโปรแกรม
    return 0;
}

    // ให้ผู้ใช้งานเลือก เมนู
int menu() {
    string line(25,'=');   // กำหนดเส้น = 25 line
    int choose; // สร้างตัวแปร choose 
    cout << line << endl; 
    cout << ": Main menu"<<endl;
    cout << line <<endl;
    cout << ": 0 - Exit :\n";
    cout << ": 1 - Add Student :\n";
    cout << ": 2 - Display Student :\n";
    cout << ": 3 - FindName :\n";
    cout << line << endl;
    cout << " Enter choose : "; // ให้เลือกทำรายการ 0-3 ดังนี้
    cin >> choose; 
    return choose;
}

void Addstudent(string FN) {
    ofstream outfile(FN.c_str(), ios_base::out | ios_base::app);
    if (outfile.is_open()) { 
        string Id, Name; // สร้างตัวแปร id name
        int score; // สร้างตัวแปร score
        cout << "\n Add Student \n";
        cout << "Enter id : "; //ให้ พิมพ์ id
        cin >> Id; // รับค่า id 
        cout << "Enter name : "; //ให้ พิมพ์ name
        cin >> Name; // รับค่า  name
        cout << "Enter score : "; //ให้ พิมพ์ score
        cin >> score; // รับค่า score

        outfile << Id << " " << Name << " " << score << endl; 
        outfile.close();

        char Wait;
        cin.get(Wait);
        cout << "\nSaved already ,Press Enter to continue"; //  บันทึกเรียบร้อย กด enter ทำขั้นตอนต่อไป
        cin.get(Wait);
    }
    else cout << "File could not opened." << endl;
}


// การคำนวณเกรด
char calGrade(int score) {
    if (score >= 80){
        return 'A';
    }else if (score >= 70){
        return 'B';
    }else if (score >= 60){
        return 'C';
    }
    else if (score >= 50){
        return 'D';
    }
    else{
        return 'F';
    }
}

void DisplayStudent(string FN){

    ifstream Infile(FN.c_str(), ios_base::in);
    if (Infile.is_open())
    {
        string id,name; //สร้างตัวแปร id name
        int score; // สร้างตัวแปร score
        string line(10,'='); // สร้างตัวแปร line เส้น = มี10 เส้น
        int n = 0; //กำหนด ตัวแปร n = 0
        cout << "List Student\n"; 
        cout << line << endl; // แสดงเส้น
        cout << "No. id name score Grade" << endl; 
        cout << line << endl; 
        Infile >> id >> name >> score; 
        while (!Infile.eof())
        {
            n = n + 1;

            // เป็นการกำหนดความห่างตัวอักษร
        cout << right << setw(3) << n << " : ";
        cout << left  << setw(8)  << id;            
        cout << setw(20) << name;             
        cout << right << setw(6)  << score;    
        cout << setw(6)  << calGrade(score) << endl; 

        Infile >> id >> name >> score;
        }
        Infile.close();
        char Wait;
        cin.get(Wait);
        cout << "Press Enter to continue";
        cin.get(Wait);
    }else{
        cout << "File could not opened." << endl;
    }
    
}

void FindName(string FN) {
    ifstream Infile(FN.c_str(), ios_base::in);
    if (Infile.is_open()) {
        string searchName;  // กำหนด ตัวแปร 
        cout << "Enter name to find: ";
        cin >> searchName; // รับค่า ค้นหาชื่อ

        string id, name;
        int score;
        bool found = false;

        while (Infile >> id >> name >> score) {
            if (name == searchName) {
                cout << "\nFound student!\n";
                cout << "ID: " << id << endl;
                cout << "Name: " << name << endl;
                cout << "Score: " << score << endl;
                cout << "Grade: " << calGrade(score) << endl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Student with name " << searchName << " not found.\n";  // คำสั่งนี้ ถ้าเกิดจริง จะโชว์ข้อมูลนักเรียน ถ้าไม่จริง จะโชว์ student with name not found
        }
        Infile.close();

        char Wait;
        cin.get(Wait);
        cout << "Press Enter to continue";
        cin.get(Wait);
    } else {
        cout << "File could not opened." << endl;
    }
}
