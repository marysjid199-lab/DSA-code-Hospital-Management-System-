#include <iostream>      // For input/output operations
#include <string>        // For string class and string operations
#include <ctime>         // For time functions
#include <vector>        // For dynamic storage
#include <queue>         // For queue and priority_queue
#include <fstream>       // For file input/output operations
using namespace std;

// Get current date and time as a string (without trailing newline)
string getCurrentDateTime() {
    time_t now = time(0);
    char* dt = ctime(&now);
    string dateTime(dt);
    if (!dateTime.empty() && dateTime[dateTime.length() - 1] == '\n') {
        dateTime.erase(dateTime.length() - 1);
    }
    return dateTime;
}

class Patient;

class ServiceNode {
public:
    string serviceType;
    string diagnosis;
    string medication;
    string doctor;
    string serviceDate;
    double cost;
    int severity;
    bool admitted;
    int bedNumber;
    ServiceNode* next;   // Pointer to next service record (makes it a linked list)

    ServiceNode(string type, string diag, string med, string doc,
                double c, int sev, bool adm = false, int bed = -1) {
        serviceType = type;
        diagnosis = diag;
        medication = med;
        doctor = doc;
        cost = c;
        severity = sev;
        admitted = adm;
        bedNumber = bed;
        serviceDate = getCurrentDateTime();
        next = nullptr;
    }

    // Default constructor for loading from file
    ServiceNode() : cost(0.0), severity(0), admitted(false), bedNumber(-1), next(nullptr) {}

    void display() const {
        cout << "  Date: " << serviceDate << endl;
        cout << "  Service: " << serviceType << endl;
        cout << "  Diagnosis: " << diagnosis << endl;
        cout << "  Medication: " << medication << endl;
        cout << "  Doctor: " << doctor << endl;
        cout << "  Severity: " << severity << "/10" << endl;
        cout << "  Admitted: " << (admitted ? "Yes" : "No") << endl;
        if (bedNumber != -1) cout << "  Bed: #" << bedNumber << endl;
        cout << "  Cost: $" << cost << endl;
        cout << "  --------------------" << endl;
    }

    // Save service data to file
    void saveToFile(ofstream& file) const {
        file << "--- SERVICE RECORD ---" << endl;
        file << "Service Type: " << serviceType << endl;
        file << "Diagnosis: " << diagnosis << endl;
        file << "Medication: " << medication << endl;
        file << "Doctor: " << doctor << endl;
        file << "Service Date: " << serviceDate << endl;
        file << "Cost: $" << cost << endl;
        file << "Severity: " << severity << "/10" << endl;
        file << "Admitted: " << (admitted ? "Yes" : "No") << endl;
        file << "Bed Number: " << bedNumber << endl;
        file << "--- END SERVICE ---" << endl;
    }

    // Load service data from file
    void loadFromFile(ifstream& file) {
        string line;

        getline(file, line); // Skip "--- SERVICE RECORD ---"

        getline(file, line);
        serviceType = line.substr(14);   // Skip "Service Type: "

        getline(file, line);
        diagnosis = line.substr(11);     // Skip "Diagnosis: "

        getline(file, line);
        medication = line.substr(12);    // Skip "Medication: "

        getline(file, line);
        doctor = line.substr(8);         // Skip "Doctor: "

        getline(file, line);
        serviceDate = line.substr(14);   // Skip "Service Date: "

        // Read Cost
        getline(file, line);
        size_t dollarPos = line.find('$');
        if (dollarPos != string::npos) {
            cost = stod(line.substr(dollarPos + 1));
        } else {
            cost = stod(line.substr(6)); // Skip "Cost: "
        }

        // Read Severity
        getline(file, line);
        size_t slashPos = line.find('/');
        if (slashPos != string::npos) {
            severity = stoi(line.substr(10, slashPos - 10)); // Skip "Severity: "
        } else {
            severity = stoi(line.substr(10));
        }

        // Read Admitted
        getline(file, line);
        string admittedStr = line.substr(10); // Skip "Admitted: "
        admitted = (admittedStr == "Yes");

        // Read Bed Number
        getline(file, line);
        bedNumber = stoi(line.substr(12)); // Skip "Bed Number: "

        getline(file, line); // Skip "--- END SERVICE ---"
    }
};

class ServiceHistory {
private:
    ServiceNode* head;
    ServiceNode* tail;
    int count;

public:
    ServiceHistory() : head(nullptr), tail(nullptr), count(0) {}

    void addService(string type, string diag, string med, string doc,
                    double cost, int severity, bool admitted = false, int bed = -1) {
        ServiceNode* newNode = new ServiceNode(type, diag, med, doc, cost, severity, admitted, bed);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void displayAll() const {
        if (head == nullptr) {
            cout << "No service history available." << endl;
            return;
        }

        ServiceNode* current = head;
        int visitNum = 1;
        while (current != nullptr) {
            cout << "\nVisit #" << visitNum++ << ":" << endl;
            current->display();
            current = current->next;
        }
    }

    double getTotalCost() const {
        double total = 0.0;
        ServiceNode* current = head;
        while (current != nullptr) {
            total += current->cost;
            current = current->next;
        }
        return total;
    }

    int getVisitCount() const { return count; }

    void displayBill() const {
        if (head == nullptr) {
            cout << "No services to generate bill." << endl;
            return;
        }

        ServiceNode* current = head;
        int itemNo = 1;
        double total = 0.0;

        cout << "\n=== MEDICAL BILL ===" << endl;
        cout << "Item | Service | Cost" << endl;
        cout << "---------------------" << endl;

        while (current != nullptr) {
            cout << itemNo++ << ". " << current->serviceType << " | $" << current->cost << endl;
            total += current->cost;
            current = current->next;
        }

        cout << "---------------------" << endl;
        cout << "TOTAL: $" << total << endl;
        cout << "=====================" << endl;
    }

    // Save all services to file
    void saveToFile(ofstream& file) const {
        file << "SERVICE HISTORY (" << count << " visits)" << endl;

        ServiceNode* current = head;
        while (current != nullptr) {
            current->saveToFile(file);
            current = current->next;
        }
    }

    // Load all services from file
    void loadFromFile(ifstream& file) {
        string line;

        getline(file, line); // "SERVICE HISTORY (X visits)"

        // Extract count from header
        int loadedCount = 0;
        size_t openParen = line.find('(');
        size_t closeParen = line.find(')');   // FIXED: single character ')' (was ') ')
        if (openParen != string::npos && closeParen != string::npos && closeParen > openParen) {
            string countStr = line.substr(openParen + 1, closeParen - openParen - 1);
            try {
                loadedCount = stoi(countStr);   // "3 visits" -> 3
            } catch (...) {
                loadedCount = 0;
            }
        }

        // Read services (addService-style linking; count is set after loading)
        for (int i = 0; i < loadedCount; i++) {
            ServiceNode* newNode = new ServiceNode();
            newNode->loadFromFile(file);

            if (head == nullptr) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            count++;
        }
    }

    ~ServiceHistory() {
        ServiceNode* current = head;
        while (current != nullptr) {
            ServiceNode* next = current->next;
            delete current;
            current = next;
        }
    }
};

class Patient {
private:
    int id;
    string name;
    int age;
    string gender;
    string contact;
    string bloodGroup;

public:
    string allergies;
    string chronicConditions;
    int currentSeverity;
    string assignedDepartment;
    ServiceHistory* history;
    string registrationDate;
    bool isAdmitted;
    int currentBedNumber;

    Patient(int id, string name, int age, string gender, string contact,
            string blood, string allergies, string chronic,
            int severity, string dept) {
        this->id = id;
        this->name = name;
        this->age = age;
        this->gender = gender;
        this->contact = contact;
        this->bloodGroup = blood;
        this->allergies = allergies;
        this->chronicConditions = chronic;
        this->currentSeverity = severity;
        this->assignedDepartment = dept;
        this->history = new ServiceHistory();
        this->registrationDate = getCurrentDateTime();
        this->isAdmitted = false;
        this->currentBedNumber = -1;
    }

    // Constructor for loading from file
    Patient() {
        this->history = new ServiceHistory();
        this->isAdmitted = false;
        this->currentBedNumber = -1;
        this->currentSeverity = 0;
        this->id = 0;
        this->age = 0;
    }

    void addService(string type, string diag, string med, string doc,
                    double cost, int severity, bool admitted = false, int bed = -1) {
        history->addService(type, diag, med, doc, cost, severity, admitted, bed);
        currentSeverity = severity;

        if (admitted) {
            isAdmitted = true;
            currentBedNumber = bed;
        }
    }

    void dischargePatient() {
        isAdmitted = false;
        currentBedNumber = -1;
        cout << "Patient " << name << " (ID: " << id << ") has been discharged." << endl;
    }

    void displayBasic() const {
        cout << "ID: " << id << " | Name: " << name << " | Age: " << age
             << " | Severity: " << currentSeverity
             << " | Dept: " << assignedDepartment;

        if (isAdmitted) {
            cout << " | Status: ADMITTED | Bed: #" << currentBedNumber;
        } else {
            cout << " | Status: NOT ADMITTED";
        }

        cout << " | Visits: " << history->getVisitCount() << endl;
    }

    void displayDetailed() const {
        cout << "\n=== PATIENT DETAILED RECORD ===" << endl;
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << " | Gender: " << gender << endl;
        cout << "Contact: " << contact << endl;
        cout << "Blood Group: " << bloodGroup << endl;
        cout << "Allergies: " << allergies << endl;
        cout << "Chronic Conditions: " << chronicConditions << endl;
        cout << "Current Severity: " << currentSeverity << "/10" << endl;
        cout << "Department: " << assignedDepartment << endl;
        cout << "Admission Status: " << (isAdmitted ? "ADMITTED" : "NOT ADMITTED") << endl;
        if (isAdmitted) {
            cout << "Bed Number: " << currentBedNumber << endl;
        }
        cout << "Registration Date: " << registrationDate << endl;
        cout << "Total Visits: " << history->getVisitCount() << endl;
        cout << "===============================" << endl;
    }

    void displayServiceHistory() {
        cout << "\n=== SERVICE HISTORY ===" << endl;
        cout << "Patient: " << name << " (ID: " << id << ")" << endl;
        history->displayAll();
    }

    void displayBill() {
        cout << "\n=== BILL FOR PATIENT ===" << endl;
        cout << "Name: " << name << " (ID: " << id << ")" << endl;
        history->displayBill();
    }

    double getTotalSpending() {
        return history->getTotalCost();
    }

    int getId() const { return id; }
    string getName() const { return name; }
    bool getIsAdmitted() const { return isAdmitted; }

    // Save patient data to file
    void saveToFile(ofstream& file) {
        file << "=== PATIENT RECORD ===" << endl;
        file << "ID: " << id << endl;
        file << "Name: " << name << endl;
        file << "Age: " << age << endl;
        file << "Gender: " << gender << endl;
        file << "Contact: " << contact << endl;
        file << "Blood Group: " << bloodGroup << endl;
        file << "Allergies: " << allergies << endl;
        file << "Chronic Conditions: " << chronicConditions << endl;
        file << "Current Severity: " << currentSeverity << "/10" << endl;
        file << "Department: " << assignedDepartment << endl;
        file << "Registration Date: " << registrationDate << endl;
        file << "Admission Status: " << (isAdmitted ? "ADMITTED" : "NOT ADMITTED") << endl;
        file << "Bed Number: " << currentBedNumber << endl;
        file << "=== END PATIENT ===" << endl;

        history->saveToFile(file);
    }

    // Load patient data from file
    void loadFromFile(ifstream& file) {
        string line;
        getline(file, line); // Skip "=== PATIENT RECORD ==="

        getline(file, line);
        id = stoi(line.substr(4));                 // Skip "ID: "

        getline(file, line);
        name = line.substr(6);                     // Skip "Name: "

        getline(file, line);
        age = stoi(line.substr(5));                // Skip "Age: "

        getline(file, line);
        gender = line.substr(8);                   // Skip "Gender: "

        getline(file, line);
        contact = line.substr(9);                  // Skip "Contact: "

        getline(file, line);
        bloodGroup = line.substr(13);              // Skip "Blood Group: "

        getline(file, line);
        allergies = line.substr(11);               // Skip "Allergies: "

        getline(file, line);
        chronicConditions = line.substr(20);       // FIXED: "Chronic Conditions: " is 20 chars (was 19)

        getline(file, line);
        size_t slashPos = line.find('/');
        if (slashPos != string::npos) {
            currentSeverity = stoi(line.substr(18, slashPos - 18)); // Skip "Current Severity: "
        } else {
            currentSeverity = stoi(line.substr(18));
        }

        getline(file, line);
        assignedDepartment = line.substr(12);      // Skip "Department: "

        getline(file, line);
        registrationDate = line.substr(19);        // Skip "Registration Date: "

        getline(file, line);
        string status = line.substr(18);           // Skip "Admission Status: "
        isAdmitted = (status == "ADMITTED");

        getline(file, line);
        currentBedNumber = stoi(line.substr(12));  // Skip "Bed Number: "

        getline(file, line); // Skip "=== END PATIENT ==="

        history->loadFromFile(file);
    }

    ~Patient() {
        delete history;
    }
};

class PatientDatabase {
private:
    vector<Patient*> patients;

public:
    void insertPatient(Patient* p) {
        patients.push_back(p);
    }

    Patient* findPatient(int id) {
        for (Patient* p : patients) {
            if (p->getId() == id) {
                return p;
            }
        }
        return nullptr;
    }

    void displayAllPatients() {
        if (patients.empty()) {
            cout << "No patients in database." << endl;
            return;
        }
        for (Patient* p : patients) {
            p->displayBasic();
        }
    }

    vector<Patient*> getAdmittedPatients() {
        vector<Patient*> admittedList;
        for (Patient* p : patients) {
            if (p->getIsAdmitted()) {
                admittedList.push_back(p);
            }
        }
        return admittedList;
    }

    bool isEmpty() { return patients.empty(); }

    // Save all patients to file
    void saveToFile(const string& filename) {
        ofstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Error: Cannot open file for writing: " << filename << endl;
            return;
        }

        file << "=== HOSPITAL PATIENT DATABASE ===" << endl;
        file << "Total Patients: " << patients.size() << endl;
        file << "Generated on: " << getCurrentDateTime() << endl;
        file << "=================================" << endl << endl;

        for (size_t i = 0; i < patients.size(); i++) {      // FIXED: size_t (no signed/unsigned warning)
            file << "Patient #" << (i + 1) << " of " << patients.size() << endl;
            patients[i]->saveToFile(file);
            if (i + 1 < patients.size()) {
                file << endl << endl;
            }
        }

        file << "\n=== END OF DATABASE ===" << endl;
        file.close();
        cout << "Data saved to " << filename << endl;
    }

    // Load all patients from file
    void loadFromFile(const string& filename, int& nextId) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "No existing data found. Starting fresh." << endl;
            nextId = 0;
            return;
        }

        string header;
        getline(file, header);
        // Remove possible '\r' (Windows line endings)
        if (!header.empty() && header[header.length() - 1] == '\r') {
            header.erase(header.length() - 1);
        }
        if (header != "=== HOSPITAL PATIENT DATABASE ===") {
            cout << "Invalid data file." << endl;
            file.close();
            nextId = 0;
            return;
        }

        string infoLine;
        getline(file, infoLine); // Skip "Total Patients: X"
        getline(file, infoLine); // Skip "Generated on: ..."
        getline(file, infoLine); // Skip "====="
        getline(file, infoLine); // Skip blank line

        int maxId = 0;
        int patientCount = 0;

        // Read patients until we hit the end marker
        while (file.peek() != EOF && file.peek() != '=') {
            string patientNumLine;
            getline(file, patientNumLine); // Skip "Patient #X of Y"

            Patient* patient = new Patient();
            patient->loadFromFile(file);
            patients.push_back(patient);

            if (patient->getId() > maxId) {
                maxId = patient->getId();
            }
            patientCount++;

            // Skip blank lines between patients
            while (file.peek() == '\n' || file.peek() == '\r') {
                file.get();
            }
        }

        file.close();
        nextId = maxId + 1;
        cout << "Data loaded from " << filename << " (" << patientCount << " patients)" << endl;
    }

    ~PatientDatabase() {
        for (Patient* p : patients) {
            delete p;
        }
    }
};

class HospitalSystem {
private:
    PatientDatabase patientDatabase;

    queue<Patient*> regularQueue;

    struct CompareSeverity {
        bool operator()(Patient* a, Patient* b) {
            return a->currentSeverity < b->currentSeverity;
        }
    };
    priority_queue<Patient*, vector<Patient*>, CompareSeverity> priorityQueue;

    int nextPatientId;
    const string DATA_FILE = "hospital_data.txt";

public:
    HospitalSystem() : nextPatientId(1001) {
        patientDatabase.loadFromFile(DATA_FILE, nextPatientId);
        if (nextPatientId == 0 || nextPatientId < 1001) {
            nextPatientId = 1001;
        }
        cout << "=== HOSPITAL MANAGEMENT SYSTEM ===" << endl;
        cout << "System Initialized Successfully!" << endl;
        cout << "Next Patient ID: " << nextPatientId << endl;
    }

    ~HospitalSystem() {
        patientDatabase.saveToFile(DATA_FILE);
        cout << "Data saved to file." << endl;
    }

    void registerPatient() {
        cout << "\n=== REGISTER NEW PATIENT ===" << endl;

        string name, gender, contact, blood, allergies, chronic;
        int age, severity;

        cout << "Name: ";
        getline(cin, name);
        cout << "Age: ";
        cin >> age;
        cin.ignore();
        cout << "Gender (M/F): ";
        getline(cin, gender);
        cout << "Contact: ";
        getline(cin, contact);
        cout << "Blood Group: ";
        getline(cin, blood);
        cout << "Allergies: ";
        getline(cin, allergies);
        cout << "Chronic Conditions: ";
        getline(cin, chronic);
        cout << "Initial Severity (1-10): ";
        cin >> severity;
        cin.ignore();

        cout << "\nSelect Department:" << endl;
        cout << "1. Emergency\n2. Cardiology\n3. Neurology\n4. Orthopedics\n";
        cout << "5. Pediatrics\n6. Dermatology\n7. General Medicine\n8. Surgery\n";
        cout << "Choice: ";
        int deptChoice;
        cin >> deptChoice;
        cin.ignore();

        vector<string> departments = {
            "Emergency", "Cardiology", "Neurology", "Orthopedics",
            "Pediatrics", "Dermatology", "General Medicine", "Surgery"
        };

        string dept;
        if (deptChoice >= 1 && deptChoice <= 8) {
            dept = departments[deptChoice - 1];
        } else {
            dept = "General Medicine";
        }

        Patient* newPatient = new Patient(nextPatientId++, name, age, gender,
                                          contact, blood, allergies,
                                          chronic, severity, dept);

        patientDatabase.insertPatient(newPatient);

        if (severity >= 7) {
            priorityQueue.push(newPatient);
            cout << "Patient added to PRIORITY queue (High Severity)" << endl;
        } else {
            regularQueue.push(newPatient);
            cout << "Patient added to REGULAR queue" << endl;
        }

        cout << "\nPatient registered successfully!" << endl;
        cout << "Patient ID: " << newPatient->getId() << endl;
        cout << "Assigned to: " << dept << " department" << endl;

        patientDatabase.saveToFile(DATA_FILE);
    }

    void serveNextPatient() {
        cout << "\n=== SERVE NEXT PATIENT ===" << endl;

        Patient* patient = nullptr;

        if (!priorityQueue.empty()) {
            patient = priorityQueue.top();
            priorityQueue.pop();
            cout << "Serving PRIORITY patient (Severity: "
                 << patient->currentSeverity << ")" << endl;
        } else if (!regularQueue.empty()) {
            patient = regularQueue.front();
            regularQueue.pop();
            cout << "Serving REGULAR patient" << endl;
        } else {
            cout << "No patients in queue!" << endl;
            return;
        }

        if (patient != nullptr) {
            patient->displayBasic();
            recordTreatment(patient);
            patientDatabase.saveToFile(DATA_FILE);
        }
    }

    void serveAdmittedPatient() {
        cout << "\n=== SERVE ADMITTED PATIENT ===" << endl;

        vector<Patient*> admittedPatients = patientDatabase.getAdmittedPatients();

        if (admittedPatients.empty()) {
            cout << "No admitted patients in the hospital." << endl;
            return;
        }

        cout << "\nCurrently Admitted Patients:" << endl;
        cout << "=============================" << endl;
        for (size_t i = 0; i < admittedPatients.size(); i++) {
            cout << i + 1 << ". ";
            admittedPatients[i]->displayBasic();
        }

        cout << "\nEnter patient number to serve (0 to cancel): ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice < 1 || choice > (int)admittedPatients.size()) {
            cout << "Cancelled or invalid choice." << endl;
            return;
        }

        Patient* patient = admittedPatients[choice - 1];

        cout << "\nServing admitted patient: " << patient->getName()
             << " (ID: " << patient->getId() << ")" << endl;

        recordTreatmentForAdmitted(patient);

        cout << "\nShould this patient be discharged? (y/n): ";
        char dischargeChoice;
        cin >> dischargeChoice;
        cin.ignore();

        if (dischargeChoice == 'y' || dischargeChoice == 'Y') {
            patient->dischargePatient();
        }

        patientDatabase.saveToFile(DATA_FILE);
    }

    void recordTreatment(Patient* patient) {
        cout << "\n=== RECORD TREATMENT ===" << endl;

        string diagnosis, medication, doctor, serviceType;
        double cost;
        int severity;

        cout << "Service Type (Emergency/Surgery/Checkup/Test): ";
        getline(cin, serviceType);
        cout << "Diagnosis: ";
        getline(cin, diagnosis);
        cout << "Current Severity (1-10): ";
        cin >> severity;
        cin.ignore();
        cout << "Medication Prescribed: ";
        getline(cin, medication);
        cout << "Doctor: ";
        getline(cin, doctor);
        cout << "Cost: $";
        cin >> cost;
        cin.ignore();

        bool needsAdmission = false;
        int bedNumber = -1;

        if (!patient->getIsAdmitted() && severity >= 7) {
            cout << "\nPatient needs admission! Assign bed? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();

            if (choice == 'y' || choice == 'Y') {
                needsAdmission = true;
                cout << "Assign bed number: ";
                cin >> bedNumber;
                cin.ignore();
            }
        } else if (patient->getIsAdmitted()) {
            bedNumber = patient->currentBedNumber;
            needsAdmission = false;
            cout << "\nPatient is already admitted in Bed #" << bedNumber << endl;
        }

        patient->addService(serviceType, diagnosis, medication, doctor,
                            cost, severity, needsAdmission, bedNumber);

        cout << "\nTreatment recorded successfully!" << endl;
        cout << "Total visits: " << patient->history->getVisitCount() << endl;
        cout << "Total spending: $" << patient->getTotalSpending() << endl;

        if (needsAdmission) {
            cout << "Patient ADMITTED to hospital" << endl;
        }
    }

    void recordTreatmentForAdmitted(Patient* patient) {
        cout << "\n=== RECORD TREATMENT FOR ADMITTED PATIENT ===" << endl;

        string diagnosis, medication, doctor, serviceType;
        double cost;
        int severity;

        cout << "Service Type (Daily Checkup/Treatment/Test): ";
        getline(cin, serviceType);
        cout << "Diagnosis Update: ";
        getline(cin, diagnosis);
        cout << "Current Severity (1-10): ";
        cin >> severity;
        cin.ignore();
        cout << "Medication Prescribed: ";
        getline(cin, medication);
        cout << "Doctor: ";
        getline(cin, doctor);
        cout << "Cost: $";
        cin >> cost;
        cin.ignore();

        int bedNumber = patient->currentBedNumber;

        patient->addService(serviceType, diagnosis, medication, doctor,
                            cost, severity, false, bedNumber);

        cout << "\nTreatment recorded successfully!" << endl;
        cout << "Total visits: " << patient->history->getVisitCount() << endl;
        cout << "Total spending: $" << patient->getTotalSpending() << endl;
    }

    void viewAdmittedPatients() {
        cout << "\n=== CURRENTLY ADMITTED PATIENTS ===" << endl;

        vector<Patient*> admittedPatients = patientDatabase.getAdmittedPatients();

        if (admittedPatients.empty()) {
            cout << "No patients are currently admitted." << endl;
            return;
        }

        cout << "Total Admitted Patients: " << admittedPatients.size() << endl;
        cout << "===================================" << endl;

        for (size_t i = 0; i < admittedPatients.size(); i++) {
            cout << "\nPatient #" << (i + 1) << ":" << endl;
            admittedPatients[i]->displayDetailed();
        }
    }

    void dischargePatient() {
        cout << "\n=== DISCHARGE PATIENT ===" << endl;

        vector<Patient*> admittedPatients = patientDatabase.getAdmittedPatients();

        if (admittedPatients.empty()) {
            cout << "No patients are currently admitted." << endl;
            return;
        }

        cout << "\nSelect patient to discharge:" << endl;
        for (size_t i = 0; i < admittedPatients.size(); i++) {
            cout << i + 1 << ". ID: " << admittedPatients[i]->getId()
                 << " | Name: " << admittedPatients[i]->getName()
                 << " | Bed: #" << admittedPatients[i]->currentBedNumber << endl;
        }

        cout << "\nEnter patient number to discharge (0 to cancel): ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice < 1 || choice > (int)admittedPatients.size()) {
            cout << "Cancelled or invalid choice." << endl;
            return;
        }

        Patient* patient = admittedPatients[choice - 1];
        patient->dischargePatient();

        patientDatabase.saveToFile(DATA_FILE);
    }

    void searchPatient() {
        cout << "\n=== SEARCH PATIENT ===" << endl;

        int choice;
        cout << "Search by:\n1. Patient ID\n2. Display All Patients\n3. Generate Bill\nChoice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            int id;
            cout << "Enter Patient ID: ";
            cin >> id;
            cin.ignore();

            Patient* patient = patientDatabase.findPatient(id);

            if (patient != nullptr) {
                cout << "\nPatient Found!" << endl;
                patient->displayDetailed();

                cout << "\nOptions:\n1. View Service History\n2. Generate Bill\n3. Back\nChoice: ";
                int opt;
                cin >> opt;
                cin.ignore();

                if (opt == 1) {
                    patient->displayServiceHistory();
                } else if (opt == 2) {
                    patient->displayBill();
                }
            } else {
                cout << "Patient not found!" << endl;
            }
        } else if (choice == 2) {
            cout << "\n=== ALL PATIENTS ===" << endl;
            patientDatabase.displayAllPatients();
        } else if (choice == 3) {
            int id;
            cout << "Enter Patient ID to generate bill: ";
            cin >> id;
            cin.ignore();

            Patient* patient = patientDatabase.findPatient(id);
            if (patient != nullptr) {
                patient->displayBill();
            } else {
                cout << "Patient not found!" << endl;
            }
        }
    }

    void run() {
        int choice;

        do {
            cout << "\n=== HOSPITAL MANAGEMENT SYSTEM ===" << endl;
            cout << "1. Register New Patient" << endl;
            cout << "2. Serve Next Patient" << endl;
            cout << "3. Serve Admitted Patient" << endl;
            cout << "4. View Admitted Patients" << endl;
            cout << "5. Discharge Patient" << endl;
            cout << "6. Search Patient / Generate Bill" << endl;
            cout << "7. Exit" << endl;
            cout << "===================================" << endl;
            cout << "Enter your choice (1-7): ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    registerPatient();
                    break;
                case 2:
                    serveNextPatient();
                    break;
                case 3:
                    serveAdmittedPatient();
                    break;
                case 4:
                    viewAdmittedPatients();
                    break;
                case 5:
                    dischargePatient();
                    break;
                case 6:
                    searchPatient();
                    break;
                case 7:
                    cout << "\nSaving data..." << endl;
                    patientDatabase.saveToFile(DATA_FILE);
                    cout << "Thank you for using Hospital Management System!" << endl;
                    cout << "Goodbye!" << endl;
                    break;
                default:
                    cout << "Invalid choice! Please enter 1-7." << endl;
            }
        } while (choice != 7);
    }
};

int main() {
    HospitalSystem hospital;
    hospital.run();

    return 0;
}