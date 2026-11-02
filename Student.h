// Student.h - TV1: Student record, validation and safe keyboard input.
// Shared by all modules. Everything here is inline (header-only).
#ifndef STUDENT_H
#define STUDENT_H

#include <algorithm>
#include <cctype>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Allowed range for the enrollment year (kept in ONE place, easy to change).
inline constexpr int MIN_ENROLL_YEAR = 2000;
inline constexpr int MAX_ENROLL_YEAR = 2026;

struct Student {
    std::string id;     // unique BST key, e.g. "S030"
    std::string name;   // full name
    std::string dob;    // date of birth, format YYYY-MM-DD
    std::string major;  // e.g. "IT"
    float gpa;          // 0.0 - 4.0
    int year;           // enrollment year
};

// ---------------------------------------------------------------- text helpers

// Removes spaces / tabs / line breaks at both ends of a string.
inline std::string trimText(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// Lower-case copy of a string (for case-insensitive comparison).
inline std::string toLowerText(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// ---------------------------------------------------------------- printing

// Prints the column titles of the student table.
inline void printStudentHeader() {
    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(22) << "Full Name"
              << std::setw(12) << "DOB"
              << std::setw(12) << "Major"
              << std::setw(6)  << "GPA"
              << std::setw(6)  << "Year" << '\n'
              << std::string(66, '-') << '\n';
}

// Prints one student as one table row.
inline void printStudent(const Student& s) {
    std::cout << std::left
              << std::setw(8)  << s.id
              << std::setw(22) << s.name
              << std::setw(12) << s.dob
              << std::setw(12) << s.major
              << std::setw(6)  << std::fixed << std::setprecision(1) << s.gpa
              << std::setw(6)  << s.year << '\n';
    std::cout.unsetf(std::ios::floatfield);
}

// ---------------------------------------------------------------- validation

// ID: not empty and no white space inside.
inline bool isValidId(const std::string& id) {
    if (id.empty()) return false;
    for (unsigned char c : id)
        if (std::isspace(c)) return false;
    return true;
}

// Real calendar date written as YYYY-MM-DD (rejects 2005-02-30, month 13...).
inline bool isValidDate(const std::string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (size_t i = 0; i < d.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (d[i] < '0' || d[i] > '9') return false;
    }
    int y = std::stoi(d.substr(0, 4));
    int m = std::stoi(d.substr(5, 2));
    int day = std::stoi(d.substr(8, 2));
    if (m < 1 || m > 12 || day < 1) return false;
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = days[m - 1];
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (m == 2 && leap) maxDay = 29;
    return day <= maxDay;
}

// GPA: a number from 0.0 to 4.0. Returns false for text such as "abc".
inline bool parseGpa(const std::string& text, float& out) {
    try {
        size_t used = 0;
        double v = std::stod(text, &used);
        if (used != text.size()) return false;
        if (!(v >= 0.0 && v <= 4.0)) return false;  // also rejects nan
        out = static_cast<float>(v);
        return true;
    } catch (...) {
        return false;
    }
}

// Enrollment year: an integer in [MIN_ENROLL_YEAR, MAX_ENROLL_YEAR].
inline bool parseYear(const std::string& text, int& out) {
    try {
        size_t used = 0;
        int v = std::stoi(text, &used);
        if (used != text.size()) return false;
        if (v < MIN_ENROLL_YEAR || v > MAX_ENROLL_YEAR) return false;
        out = v;
        return true;
    } catch (...) {
        return false;
    }
}

// ---------------------------------------------------------------- safe input

// Prints a prompt and reads one trimmed line. Returns false when input is closed (EOF).
inline bool readLineSafe(const std::string& prompt, std::string& out) {
    std::cout << prompt;
    if (!std::getline(std::cin, out)) return false;
    out = trimText(out);
    return true;
}

// Asks again and again until the answer is valid.
// If 'current' is not null, an empty answer keeps the current value.
// Returns false only when input is closed.
inline bool askField(const std::string& label, const std::string* current,
                     const std::function<bool(const std::string&)>& isValid,
                     const std::string& errorMsg, std::string& out) {
    while (true) {
        std::string prompt = label;
        if (current != nullptr) prompt += " [" + *current + "]";
        prompt += ": ";
        std::string line;
        if (!readLineSafe(prompt, line)) return false;
        if (line.empty() && current != nullptr) { out = *current; return true; }
        if (isValid(line)) { out = line; return true; }
        std::cout << "Invalid input: " << errorMsg << std::endl;
    }
}

// Reads the 6 fields of a student. 'current' == nullptr: new student.
// 'current' != nullptr: update mode (Enter keeps the old value of a field).
inline bool readStudentFields(Student& s, const Student* current) {
    std::string curId, curName, curDob, curMajor, curGpa, curYear;
    if (current != nullptr) {
        curId = current->id; curName = current->name; curDob = current->dob; curMajor = current->major;
        std::ostringstream g; g << current->gpa; curGpa = g.str();
        curYear = std::to_string(current->year);
    }
    auto ref = [&](const std::string& v) { return current != nullptr ? &v : nullptr; };
    auto notEmpty = [](const std::string& t) { return !t.empty(); };
    std::string id, name, dob, major, gpaText, yearText;
    float gpa = 0; int year = 0;

    if (!askField("Student ID", ref(curId), isValidId, "ID must not be empty or contain spaces", id)) return false;
    if (!askField("Full name", ref(curName), notEmpty, "name must not be empty", name)) return false;
    if (!askField("Date of birth (YYYY-MM-DD)", ref(curDob), isValidDate, "need a real date like 2005-01-10", dob)) return false;
    if (!askField("Major", ref(curMajor), notEmpty, "major must not be empty", major)) return false;
    if (!askField("GPA (0.0 - 4.0)", ref(curGpa),
                  [&](const std::string& t) { float g; return parseGpa(t, g); },
                  "GPA must be a number from 0.0 to 4.0", gpaText)) return false;
    if (!askField("Enrollment year (" + std::to_string(MIN_ENROLL_YEAR) + "-" + std::to_string(MAX_ENROLL_YEAR) + ")", ref(curYear),
                  [&](const std::string& t) { int y; return parseYear(t, y); },
                  "year must be a whole number in the allowed range", yearText)) return false;

    parseGpa(gpaText, gpa);
    parseYear(yearText, year);
    s = Student{id, name, dob, major, gpa, year};
    return true;
}

// Reads a brand-new student. Returns false if input was closed.
inline bool readStudent(Student& s) { return readStudentFields(s, nullptr); }

// Reads new values for an existing student (Enter = keep current value).
inline bool readStudentUpdate(const Student& current, Student& out) { return readStudentFields(out, &current); }

#endif
