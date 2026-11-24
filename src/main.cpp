#include <iostream>
#include "HashMap.h"

int main()
{
    HashMap students;

    // Insert students
    students.insert(101, "Sahil");
    students.insert(102, "Daniel");
    students.insert(103, "Ewan");
    students.insert(104, "Aman");
    students.insert(111, "John");
    students.insert(101, "Sahil Updated");

    // Display all students
    std::cout << "Student Hash Map:" << std::endl;
    students.display();

    // Search for a student
    std::cout << "\nSearching for student 102: ";
    std::cout << students.search(102) << std::endl;
    std::cout << "\nSearching for student 999: ";
    std::cout << students.search(999) << std::endl;

    // Remove a student
    students.remove(103);

    std::cout << "\nHash Map after removing student 103:" << std::endl;
    students.display();

    students.remove(111);

    std::cout << "\nAfter removing student 111:" << std::endl;
    students.display();

    return 0;
}