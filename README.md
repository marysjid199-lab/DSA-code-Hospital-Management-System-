# DSA-code-Hospital-Management-System-
 Hospital Management System (C++)

A console-based Hospital Management System built for my **Data Structures & Algorithms** course (Semester 3). It manages a patient's full journey (registration, triage, treatment, admission, discharge and billing) and stores everything permanently in a text file.

Features

- Smart triage: critical patients (severity ≥ 7) are always served first
- Patient registration with automatic ID generation
- Treatment records: diagnosis, medication, doctor, cost and timestamp for every visit
- Admission & bed management, with discharge support
- Search patients by ID and view full service history
- Automatic billing: itemized bills with totals
- Persistent storage: data is saved to `hospital_data.txt` and reloaded on startup

 Data Structures Used

| Data Structure | Where it is used | Complexity |
|---|---|---|
| Singly linked list | Patient visit history | Add: O(1) with tail pointer |
| Priority queue | Critical patients (severity ≥ 7) | Push / Pop: O(log n) |
| Queue (FIFO) | Regular patients | Push / Pop: O(1) |
| Vector | Patient database | Search by ID: O(n) |
| File handling | Saving and loading all records | O(n) |

 How to Run
--bash
g++ -std=c++17 hospital_management.cpp -o hospital
./hospital

Keep `hospital_data.txt` in the same folder as the program to load existing records. If it is missing, the system starts with an empty database and creates the file automatically.

 Menu

| Option | Action |
|---|---|
| 1 | Register new patient |
| 2 | Serve next patient (priority queue first) |
| 3 | Serve admitted patient |
| 4 | View admitted patients |
| 5 | Discharge patient |
| 6 | Search patient / generate bill |
| 7 | View waiting queues |
| 8 | About / data structures used |
| 0 | Save & exit |

 How Triage Works

1. A patient registers with a severity score from 1 to 10.
2. Severity (7–10) goes into the priority queue; severity (1–6) goes into the regular queue.
3. When serving the next patient, the priority queue is always checked first.
4. After treatment, a patient with severity ≥ 7 can be admitted and assigned a bed.

> Example: a patient with severity 9 who registers *after* a patient with severity 3 is still served first.

Project Structure


├── hospital_management.cpp   # Source code
├── hospital_data.txt         # Patient database (auto-generated)
├── Hospital_Management_System_Report.docx   # Project report
└── README.md


 Future Improvements

- Hash table or binary search tree for faster patient search
- Bed-management module to prevent duplicate bed assignments
- Doctor scheduling and appointment booking
- Graphical interface and role-based login

