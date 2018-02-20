#include "sudoku.hpp"
extern "C" {
	int wrapper_sudoku_solver(int block_row_count, int block_col_count, int* buffer_in, int* buffer_out)
	{
		auto N = block_row_count * block_col_count;
		sudoku::Solver solver(block_row_count, block_col_count);
		auto analysis = solver.solve(buffer_in, buffer_out);
		if (analysis.solutions_found == 1)
		{
			return 0;
		}
		return 1;
	}
}