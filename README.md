### Project Overview
The memory debugger is a tool designed to integrate into software projects to detect and log issues related to dynamic memory management. Such issues include memory leaks, buffer overflows, and incorrect memory deallocation practices. The goal is to help developers identify potential memory issues that could lead to software instability or crashes.

### Integration
To effectively integrate the memory debugger into an existing project, developers should include specific files from the memory debugger's source code. Here's how to proceed:

1. **Include Header and Implementation Files**:
    - **`CustomDebugger.h` and `CustomDebugger.cpp`**: These files contain the core functionality of the memory debugger. They should be included in the project to manage memory operations and log any discrepancies.
    - **`Common.h`**: This header might contain common definitions and utility functions used by the debugger. It ensures that all parts of the project have access to necessary macros or global settings.
    - **`mallocator.h`**: This header defines a custom memory allocator that the debugger uses to track dynamic memory allocation without interfering with the standard library's memory management.

2. **Project Configuration**:
    - Ensure that these files are correctly referenced in your project's build configuration. This might involve setting the correct include directories in your build system or IDE.
    - If using CMake, make sure to add these files to the CMakeLists.txt so they are compiled and linked correctly.

3. **Initialization**:
    - Initialize the memory debugger in the main entry point of your application to start monitoring memory operations from the beginning of the program execution.

### Output
When a memory-related error is detected, such as a memory leak, the debugger logs detailed information to facilitate debugging:

- **File**: `DebugLog.csv`
- **Location**: Typically found in the solution directory alongside the executable.
- **Logged Information**:
  - **Message**: Describes the type of error detected (e.g., "Memory Leak").
  - **File Location**: The source file where the erroneous memory operation occurred.
  - **Line**: The specific line number in the source file of the erroneous operation.
  - **Bytes**: The amount of memory involved in the operation, which could be the size of a leak or an overflow.
  - **Memory Address**: The actual memory address at which the operation was attempted.
  - **Additional Info**: This could include the instruction pointer or other context-specific information that helps trace the error back to its source.

**Example Entry in `DebugLog.csv`**:
```
        Message        File        Line       Bytes      Address     Additional Info
        ----------------------------------------------------------------------
        Memory Leak  C:\memory_debugger\TestScenarios.cpp   50  4    0x0000028F71110FFC  Instruction Pointer=  0x00007FF709557187
        Memory Leak  C:\memory_debugger\TestScenarios.cpp   61  40   0x0000028F711B0FD8  Instruction Pointer=  0x00007FF709557264
        Memory Leak  C:\memory_debugger\src\TestScenarios.cpp   41  8    0x0000028F711C0FF8  Instruction Pointer=  0x00007FF7095577D0
```

### Key Features and Functionalities
- **Memory Leak Detection**: Identifies instances where allocated memory is not freed before program termination.
- **Buffer Overflow Detection**: Catches any overflows when writing to a buffer, which could overwrite adjacent memory.
- **Buffer Underflow Detection**: (Optional) Detects underflows, which occur when a program writes data before the start of an allocated buffer.
- **Detection of Access After Free**: Identifies attempts to read from or write to memory after it has been freed.
- **Detection of Double Deletes**: Catches attempts to free memory that has already been freed.

### Examples of How the Debugger Works

### 1. Memory Leak Detection
A memory leak occurs when dynamically allocated memory is not freed, leading to wasted memory resources.

**Example:**
```cpp
void allocate_without_free() {
    int* leak = new int[100];  // Allocation without corresponding delete
    // Suppose the function exits without freeing 'leak'
}
```
**Debugger Action:**
The debugger would note that the allocated memory was never freed before the program terminated. It would log an entry specifying the file and line number where the memory was allocated, along with the size of the memory leak.

### 2. Buffer Overflow Detection
Buffer overflow happens when data written to a buffer exceeds its boundary, potentially overwriting other data.

**Example:**
```cpp
void buffer_overflow() {
    char buffer[10];
    for (int i = 0; i < 15; i++) {
        buffer[i] = 'a';  // Writing outside the bounds of the buffer
    }
}
```
**Debugger Action:**
The debugger detects the write beyond the buffer's end and logs detailed information about the overflow. This includes the address where the overflow occurred and the overflow size.

### 3. Buffer Underflow Detection
Buffer underflow is similar to overflow but involves writing data before the start of the buffer.

**Example:**
```cpp
void buffer_underflow() {
    char buffer[10];
    char* buf_ptr = buffer;
    buf_ptr[-1] = 'x';  // Writing before the start of the buffer
}
```
**Debugger Action:**
If configured to detect underflows, the debugger would catch this invalid write operation and log it, providing similar details as in an overflow error.

### 4. Access After Free
Accessing memory after it has been freed can lead to undefined behavior and security vulnerabilities.

**Example:**
```cpp
void access_after_free() {
    int* data = new int[10];
    delete[] data;
    data[0] = 42;  // Accessing memory after it has been freed
}
```
**Debugger Action:**
The debugger intercepts the illegal access, noting the memory address and the operation attempted. It logs the issue with specifics on where the freed memory was accessed.

### 5. Double Deletes
Attempting to delete the same memory more than once can cause program crashes and other critical issues.

**Example:**
```cpp
void double_delete() {
    int* ptr = new int;
    delete ptr;
    delete ptr;  // Attempting to delete memory that has already been freed
}
```
**Debugger Action:**
Upon the second delete attempt, the debugger logs an error detailing the double deletion, including the memory address and the context of both delete operations.

### 6. Mismatched Memory Operations
Using mismatched forms of new/delete (e.g., `new[]` with `delete` instead of `delete[]`) can lead to heap corruption.

**Example:**
```cpp
void mismatched_new_delete() {
    int* array = new int[10];
    delete array;  // Should use delete[] to deallocate
}
```
**Debugger Action:**
The debugger identifies the mismatch in memory operation types and logs a detailed error, specifying the type of mismatch and providing the file and line number for both the allocation and incorrect deallocation.

### Integration in Real Use Cases
In practical use, the debugger is integrated into the development and testing workflows. It runs in the background during application execution, particularly in testing environments, to catch and report memory management issues. Developers can then address these issues based on the detailed logs provided by the debugger, ensuring that the application manages memory efficiently and safely.

### Integration and Testing
- The memory debugger integrates with minimal code intrusion, requiring developers to include a specific header file and link against the debugger's library. The integration is demonstrated in the project's `main.cpp`, where test scenarios are defined.
- Automated testing scripts like `runall.ps1` and `runall.sh` run the compiled debugger against various predefined scenarios to ensure robust detection capabilities across different types of memory management errors.

### Documentation and Reporting
- Errors and issues detected by the debugger are logged into a CSV file named `DebugLog.csv`, which documents each incident with detailed information such as the type of error, the memory address involved, and the source code location (file and line number).

### Design Decision
1. Where did you put no-man's lands and why?
    I reserved one more page than was necessary to serve as a no man's land for overflow detection.
2. Did you use a linked list to track the allocations or something else?
    AllocInfo structure, which contains basePointer, currentPointer, program counter, size, and operator type, was stored using a std::list with a mallocator container.
3. How did you replace new? 
    The overloaded custom new function in the CustomDebugger file is executed each time "new" is called in the test.cpp. It always calls two internal functions before capturing the context. The custom debugger, therefore, makes two StackWalk64() calls to get the return address, which is the PC in the test.cpp. Later, it calls the GetSymbols() with the return address to obtain the line and file names of the memory leak.   
