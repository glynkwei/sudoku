# sudoku
A blazing fast header-only solver for the game, Sudoku. Currently only supported for MSVC on x86/x64. When run with -O2 optimization, a board takes roughly 0.4 miliseconds to solve.

## Documentation
``` C++
template<class ForwardIt> 
bool evaluate(ForwardIt first)
```
### **Parameters**

- **first**
  - The beginning of the input range.
### **Type Requirements**
- ```first``` must meet the requirements of a ForwardIterator
- The value type of ```first``` must be integral

### **Return value**
- Returns true if the puzzle was solved, and and false if the puzzle is impossible to solve (inconsistent)

### **Notes**
- The forward iterator must be able to advance at least 81 times. 
##
``` C++
bool evaluate(std::istream & in, std::ostream & out, bool pretty = false)
```
### **Parameters**

- **in**
  - A reference to the corresponding input stream.
- **out**
  - A reference to the output stream.
- **pretty**
  - Determines if the output is pretty
  
### **Return value**
- Returns true if the puzzle was solved, and and false if the puzzle is impossible to solve (inconsistent)
##

### **Notes**
- New line characters are ignored. All non-numerical characters outside of 1-9 is assumed to be a blank cell, while characters within 1-9 assume its value. Each character _i_ corresponds to cell _i_, in row-major order. If there are less than 81 characters to be read, then the remaining cells are assumed to be blank. If there are more than 81 characters, only the first 81 characters will be read.