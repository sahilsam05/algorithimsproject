# Hashing and Hash Maps – CA2 Pair Project

**Module:** DATA I8Z01 – Algorithms & Data Structures  
**Students:** Sahil Samuel and Aman Akhtar

## About This Project

For our Algorithms and Data Structures CA2 project, we decided to focus on Hashing and Hash Maps.

We created our own Hash Map using C++. To make the project easier to understand, we used a student database as an example. The program stores student IDs and names and allows us to insert, search for and remove student records.

We also looked at how collisions happen and how they can be handled using separate chaining.

As part of the project, we added automatic resizing and load factor calculations. We also created tests and benchmarks to see how our Hash Map performs compared with other C++ data structures.

## Table of Contents

The following documents explain the different parts of our project:

1. [Overview](overview.md) – Explains what Hash Maps are, why we chose this topic and where they can be used.
2. [Design](design.md) – Explains how our Hash Map works, including hashing, collisions, separate chaining and resizing.
3. [Code](code.md) – Explains our C++ implementation, the main functions and how to build and run the project.
4. [Analysis](analysis.md) – Covers Big-O complexity, performance benchmarks and comparisons with other data structures.
5. [References](references.md) – Lists the sources and materials used for the project.

## Project Structure

We organised our project into different folders to keep the code separate and easier to manage.

```text
algorithmsproject/
│
├── include/
│   └── HashMap.h
│
├── src/
│   ├── HashMap.cpp
│   └── main.cpp
│
├── tests/
│   └── tests.cpp
│
├── benchmark/
│   └── benchmark.cpp
│
├── CMakeLists.txt
├── README.md
├── overview.md
├── design.md
├── code.md
├── analysis.md
└── references.md
```

## Technologies Used

- **C++** – Used to implement our Hash Map.
- **CMake** – Used to build and organise the project.
- **Git and GitHub** – Used to manage our code and work together.
- **Visual Studio** – Used for development and testing.

## How to Build the Project

We used CMake to build the project.

First, open a terminal in the main project folder and run:

```bash
cmake -S . -B build
```

Then build the project:

```bash
cmake --build build
```

This builds three separate programs:

- `demo` – Runs our Hash Map demonstration.
- `tests` – Runs our unit tests.
- `benchmark` – Runs our performance benchmarks.

To run the tests, use:

```bash
ctest --test-dir build --output-on-failure
```

For more detailed instructions, see [code.md](code.md).

## Screencast

Our teaching screencast will demonstrate how Hash Maps work, explain our C++ implementation and show examples of the different operations.

**Video:** To be added.  
**Transcript/Captions:** To be added.

## Conclusion

Through this project, we wanted to understand how Hash Maps work and how useful they can be when storing and searching for information.

We implemented our own version in C++, tested the different operations and compared its performance with other data structures.
