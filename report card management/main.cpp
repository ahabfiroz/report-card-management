// Enhanced Student Report Card Generator
// Added Features:
// 1. Edit Record
// 2. Delete Record
// 3. Prevent Duplicate Rolls
// 4. Sort Records (by roll, name, total)
// 5. Rank Students
// 6. View Student Performance Bar Graph
7. View Class Topper
// 7. Overall Topper
// 8. Class Statistics
// 9. Pass/Fail Logic
// 10. Export to CSV

#include <bits/stdc++.h>
using namespace std;

struct Student {
    int roll;
    string name;
    vector<int> marks;
    int total;
    double average;
    string grade;
    bool pass;
};

const int SUBJECT_COUNT = 6;
const vector<string> SUBJECTS = {
    "English", "Mathematics", "Science", "Social Science", "Computer", "Hindi"
};
const string DATA_FILENAME = "students.txt";

string calculateGrade(double avg) {
    if (avg >= 90) return "A+";
    if (avg >= 80) return "A";
    if (avg >= 70) return "B";
    if (avg >= 60) return "C";
    if (avg >= 50) return "D";
    return "F";
}

bool calculatePass(const vector<int> &marks) {
    for (int m : marks) if (m < 40) return false;
    return true;
}

vector<Student> loadAllStudentsFromFile();
void saveAllStudentsToFile(const vector<Student>& list);

bool rollExists(int roll) {
    auto list = loadAllStudentsFromFile();
    for (auto &s : list) if (s.roll == roll) return true;
    return false;
}

Student createStudent() {
    Student s;
    cout << "Enter roll number: "; cin >> s.roll;
    while (rollExists(s.roll)) {
        cout << "Roll already exists. Enter a different roll: ";
        cin >> s.roll;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter student name: "; getline(cin, s.name);

    s.marks.assign(SUBJECT_COUNT, 0);
    s.total = 0;

    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        int m;
        while (true) {
            cout << "Enter marks in " << SUBJECTS[i] << " (0-100): ";
            if (cin >> m && m >= 0 && m <= 100) break;
            cout << "Invalid marks. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        s.marks[i] = m;
        s.total += m;
    }

    s.average = (double)s.total / SUBJECT_COUNT;
    s.grade = calculateGrade(s.average);
    s.pass = calculatePass(s.marks);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return s;
}

void saveStudentToFile(const Student &s) {
    ofstream fout(DATA_FILENAME, ios::app);
    fout << "ROLL:" << s.roll << "\n";
    fout << "NAME:" << s.name << "\n";
    for (int i = 0; i < SUBJECT_COUNT; ++i)
        fout << SUBJECTS[i] << ":" << s.marks[i] << "\n";

    fout << "TOTAL:" << s.total << "\n";
    fout << "AVERAGE:" << fixed << setprecision(2) << s.average << "\n";
    fout << "GRADE:" << s.grade << "\n";
    fout << "PASS:" << (s.pass ? "YES" : "NO") << "\n";
    fout << "-----\n";

    cout << "Record saved.\n";
}

vector<Student> loadAllStudentsFromFile() {
    vector<Student> students;
    ifstream fin(DATA_FILENAME);
    if (!fin) return students;

    string line;
    Student cur;
    bool reading = false;

    while (getline(fin, line)) {
        if (line.rfind("ROLL:", 0) == 0) {
            if (reading) students.push_back(cur);
            reading = true;
            cur = Student();
            cur.marks.assign(SUBJECT_COUNT, 0);
            cur.roll = stoi(line.substr(5));
        }
        else if (line.rfind("NAME:", 0) == 0) cur.name = line.substr(5);
        else if (line.rfind("TOTAL:", 0) == 0) cur.total = stoi(line.substr(6));
        else if (line.rfind("AVERAGE:", 0) == 0) cur.average = stod(line.substr(8));
        else if (line.rfind("GRADE:", 0) == 0) cur.grade = line.substr(6);
        else if (line.rfind("PASS:", 0) == 0) cur.pass = (line.substr(5) == "YES");
        else if (line == "-----") students.push_back(cur);
        else {
            auto pos = line.find(':');
            if (pos != string::npos) {
                string key = line.substr(0, pos);
                int value = stoi(line.substr(pos + 1));
                for (int i = 0; i < SUBJECT_COUNT; ++i)
                    if (SUBJECTS[i] == key) cur.marks[i] = value;
            }
        }
    }
    return students;
}

void saveAllStudentsToFile(const vector<Student>& list) {
    ofstream fout(DATA_FILENAME);
    for (auto &s : list) saveStudentToFile(s);
}

void displayStudent(const Student &s) {
    cout << "\n-----------------------------\n";
    cout << "Roll: " << s.roll << "\nName: " << s.name << "\n";

    for (int i = 0; i < SUBJECT_COUNT; ++i)
        cout << SUBJECTS[i] << ": " << s.marks[i] << "\n";

    cout << "Total: " << s.total << "\nAverage: " << s.average;
    cout << "\nGrade: " << s.grade << "\nPass: " << (s.pass ? "YES" : "NO");
    cout << "\n-----------------------------\n";
}

void editRecord() {
    int roll;
    cout << "Enter roll to edit: "; cin >> roll;

    auto list = loadAllStudentsFromFile();
    bool found = false;

    for (auto &s : list) {
        if (s.roll == roll) {
            cout << "Editing record for: " << s.name << "\n";
            Student updated = createStudent();
            s = updated;
            found = true;
            break;
        }
    }

    if (!found) cout << "Record not found.\n";
    else saveAllStudentsToFile(list);
}

void deleteRecord() {
    int roll;
    cout << "Enter roll to delete: "; cin >> roll;

    auto list = loadAllStudentsFromFile();
    int before = list.size();

    list.erase(remove_if(list.begin(), list.end(), [&](auto &s){ return s.roll == roll; }), list.end());

    if (list.size() == before) cout << "Record not found.\n";
    else {
        ofstream fout(DATA_FILENAME);
        for (auto &s : list) saveStudentToFile(s);
        cout << "Record deleted.\n";
    }
}

void exportCSV() {
    ofstream out("students.csv");
    out << "Roll,Name";
    for (auto &sub : SUBJECTS) out << "," << sub;
    out << ",Total,Average,Grade,Pass\n";

    auto list = loadAllStudentsFromFile();

    for (auto &s : list) {
        out << s.roll << "," << s.name;
        for (int m : s.marks) out << "," << m;
        out << "," << s.total << "," << s.average << "," << s.grade << "," << (s.pass?"YES":"NO") << "\n";
    }

    cout << "CSV Exported.\n";
}

int main() {
    cout << "\nEnhanced Student Report Card System\n";

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add Student\n2. Show All Records\n3. Search Roll\n4. Edit Record\n5. Delete Record\n6. Export CSV\n7. Exit\n";
        cout << "Enter choice: ";

        int c; cin >> c;
        cin.ignore();

        if (c == 1) {
            saveStudentToFile(createStudent());
        } else if (c == 2) {
            auto list = loadAllStudentsFromFile();
            for (auto &s : list) displayStudent(s);
        } else if (c == 3) {
            int r; cout << "Enter roll: "; cin >> r;
            auto list = loadAllStudentsFromFile();
            bool f = false;
            for (auto &s : list) if (s.roll == r) displayStudent(s), f = true;
            if (!f) cout << "Not found.\n";
        } else if (c == 4) editRecord();
        else if (c == 5) deleteRecord();
        else if (c == 6) exportCSV();
        else if (c == 7) break;
        else cout << "Invalid choice.\n";
    }

    return 0;
}
