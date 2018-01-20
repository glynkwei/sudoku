# sudoku
A fast header-only solver for the game, Sudoku. Currently only supported for MSVC on x86/x64. When run with -O2 optimization, a board takes roughly 0.1 miliseconds to solve, but can take as long as 30 miliseconds.
## Examples
If you already have your inputs stored in a container, then simply pass in your ```ForwardIt``` when calling ```sudoku::evaluate```.
``` C++
int board[81] = {	
					0,0,0,0,8,3,4,0,0,
					3,0,0,0,0,4,8,2,1,
					7,0,0,0,0,0,0,0,0,
					0,0,9,4,0,1,0,8,3,
					0,0,0,0,0,0,0,0,0,
					4,6,0,5,0,7,1,0,0,
					0,0,0,0,0,0,0,0,7,
					1,2,5,3,0,0,0,0,9,
					0,0,7,2,4,0,0,0,0 };
if (sudoku::evaluate(board))
{
    // board's values successfully updated with solution
}
else
{
    // Puzzle is impossible to solve, so board is not updated
}
```

Otherwise, you can place your values into an input stream (such as opening a file using ```std::ifstream```) and specify your output stream when calling ```sudoku::evaluate```.
``` C++
std::stringstream in("--5-3--819-285--6-6----4-5---74-283-34976---5--83--49-15--87--2-9----6---26-495-3");
sudoku::evaluate(in, std::cout);
// 475936281932851764681274359517492836349768125268315497153687942794523618826149573


std::stringstream in2("--5-3--819-285--6-6----4-5---74-283-34976---5--83--49-15--87--2-9----6---26-495-3");
sudoku::evaluate(in2, std::cout, true);
// [4][7][5][9][3][6][2][8][1]
// [9][3][2][8][5][1][7][6][4]
// [6][8][1][2][7][4][3][5][9]
// [5][1][7][4][9][2][8][3][6]
// [3][4][9][7][6][8][1][2][5]
// [2][6][8][3][1][5][4][9][7]
// [1][5][3][6][8][7][9][4][2]
// [7][9][4][5][2][3][6][1][8]
// [8][2][6][1][4][9][5][7][3]
```
## Documentation
``` C++
template<class ForwardIt> 
bool evaluate(ForwardIt first)
```
### **Parameters**

- **first**
  - The beginning of the input range.
### **Type Requirements**
- ```ForwardIt``` must meet the requirements of a ForwardIterator
- The value type of ```first``` must be integral

### **Return value**
- Returns true if the puzzle was solved, and and false if the puzzle is impossible to solve (inconsistent)

### **Side effects**
- If the puzzle was solved successfully, then ForwardIterator contains the solved puzzle in index form

### **Exceptions**
- Throws ```std::runtime_error``` if hardware does not support the __popcnt instruction.

### **Notes**
- The forward iterator must be able to advance at least 81 times. 
- Values outside 1-9 are assumed to be blank.
##
``` C++
bool evaluate(std::istream & in, std::ostream & out, bool pretty = false)
```
### **Parameters**

- **in**
  - A reference to the input stream. New line characters are ignored. Characters 1-9 assume its value, and all other characters are assumed to be blank.
- **out**
  - A reference to the output stream.
- **pretty**
  - Determines if the output is pretty
  
### **Return value**
- Returns true if the puzzle was solved, and and false if the puzzle is impossible to solve (inconsistent)

### **Side effects**
- If the puzzle was solved successfully, then the completed puzzle is printed to the output stream.

### **Exceptions**
- Throws ```std::runtime_error``` if hardware does not support the ```__popcnt``` instruction.

### **Notes**
- If there are less than 81 characters in the input stream, then the rest is assumed to be blank.
- If there are more than 81 characters in the input stream, only the first 81 characters are read.




