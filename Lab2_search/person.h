#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <stdexcept>
#include <ostream>

class Person {
private:
    std::string firstName;
    std::string middleName;
    std::string lastName;
    int birthYear = 0;

public:
    Person() = default;    // нужен для new T[n] внутри DynamicArray

    Person(const std::string& first, const std::string& middle, const std::string& last, int year){
        firstName = first;
        middleName = middle;
        lastName = last;
        birthYear = year;
    }

    // ---------- простые геттеры ----------
    const std::string& GetFirstName() const {
        return firstName;
    }

    const std::string& GetMiddleName() const {
        return middleName;
    }

    const std::string& GetLastName() const {
        return lastName;
    }

    int GetBirthYear() const {
        return birthYear;
    }

    // ---------- вычисляемые ----------
    std::string GetFullName() const {          // "Ivanov Ivan Petrovich"
        std::string fullName = lastName + " " + firstName;
        if (!middleName.empty()) {             // без отчества — без лишнего пробела в конце
            fullName += " " + middleName;
        }
        return fullName;
    }

    std::string GetFIO() const {               // "Ivanov I.P."
        std::string result = lastName;
        if (!firstName.empty()) {
            result += " ";
            result += firstName[0];            // char добавляем к std::string — так можно
            result += ".";
        }
        if (!middleName.empty()) {
            result += middleName[0];
            result += ".";
        }
        return result;
    }

    int GetAge(int year) const {
        if (year < birthYear ){
            throw std::invalid_argument("InvalidYear");
        }
        return year - birthYear;
    }
};

// вывод: std::cout << person
inline std::ostream& operator<<(std::ostream& out, const Person& p) {
    out << p.GetFullName() << " (" << p.GetBirthYear() << ")";
    return out;
}

#endif