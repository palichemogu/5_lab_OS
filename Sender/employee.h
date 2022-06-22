//
// Created by Ivan on 22.06.2022.
//

#ifndef CLIENT_EMPLOYEE_H
#define CLIENT_EMPLOYEE_H

class Employee{
public:
    Employee() : id(-1), name(), hours() {}

    void clear()
    {
        id = -1;
        for (int i = 0; i < 10; i++) name[i] = 0;
        hours = 0.0;
    }

    void print(std::ostream& os)
    {
        os << "ID : " << id << std::endl;
        os << "Name : " << name << std::endl;
        os << "Hours : " << hours << std::endl;
    }

    friend std::fstream& operator<<(std::fstream& fs, const Employee& emp);
    friend std::fstream& operator>>(std::fstream& fs, Employee& emp);

    int id;
    char name[10];
    double hours;
};

std::fstream &operator<<(std::fstream &fs, const Employee &emp)
{
    fs.write((const char*)&emp, sizeof(Employee));
    return fs;
}

std::fstream &operator>>(std::fstream &fs, Employee &emp)
{
    fs.read((char*)&emp, sizeof(Employee));
    return fs;
}

#endif //CLIENT_EMPLOYEE_H
