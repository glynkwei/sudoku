# sudoku
A header-only solver for the game, Sudoku. Currently only supported for MSVC on x86/x64.

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
### **Type Requirements**
- ```first``` must meet the requirements of a ForwardIterator
- The value type of ```first``` must be integral

### **Return value**
- Returns true if the puzzle was solved, and and false if the puzzle is impossible to solve (inconsistent)
##
