#pragma once
#include <intrin.h>
#include <queue>
#include <array>
#include <optional>
#include <iostream>
#include <string>
#include <numeric>
#pragma intrinsic(_BitScanForward)   

namespace sudoku
{	
	typedef uint32_t U32;
	constexpr std::array<int, 9> block_region(const int index)
	{
		const auto start = 27 * (index / 3) + 3 * (index % 3);
		return std::array<int, 9>{	start, start + 1, start + 2,
			start + 9, start + 10, start + 11,
			start + 18, start + 19, start + 20 };

	}	
	constexpr std::array<int, 9> row_region(const int col)
	{
		return std::array<int, 9>{ col, col + 9, col + 18, col + 27, col + 36, col + 45, col + 54, col + 63, col + 72};
	}
	constexpr std::array<int, 9> col_region(const int row)
	{
		return std::array<int, 9>{9*row, 9*row + 1, 9*row + 2, 9*row + 3, 9*row + 4, 9*row + 5, 9*row + 6, 9*row + 7, 9*row + 8};
	}
	const int NEIGHBOR_TABLE[1620] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 18, 27, 36, 45, 54, 63, 72, 10, 11, 19, 20, 0, 2, 3, 4, 5, 6, 7, 8, 10, 19, 28, 37, 46, 55, 64, 73, 9, 11, 18, 20, 0, 1, 3, 4, 5, 6, 7, 8, 11, 20,
		29, 38, 47, 56, 65, 74, 9, 10, 18, 19, 0, 1, 2, 4, 5, 6, 7, 8, 12, 21, 30, 39, 48, 57, 66, 75, 13, 14, 22, 23, 0, 1, 2, 3, 5, 6, 7, 8, 13, 22, 31, 40, 49, 58, 67, 76, 12, 14, 21, 23, 0, 1, 2, 3, 4, 6, 7, 8,
		14, 23, 32, 41, 50, 59, 68, 77, 12, 13, 21, 22, 0, 1, 2, 3, 4, 5, 7, 8, 15, 24, 33, 42, 51, 60, 69, 78, 16, 17, 25, 26, 0, 1, 2, 3, 4, 5, 6, 8, 16, 25, 34, 43, 52, 61, 70, 79, 15, 17, 24, 26, 0, 1, 2, 3, 4,
		5, 6, 7, 17, 26, 35, 44, 53, 62, 71, 80, 15, 16, 24, 25, 10, 11, 12, 13, 14, 15, 16, 17, 0, 18, 27, 36, 45, 54, 63, 72, 1, 2, 19, 20, 9, 11, 12, 13, 14, 15, 16, 17, 1, 19, 28, 37, 46, 55, 64, 73, 0, 2, 18, 20,
		9, 10, 12, 13, 14, 15, 16, 17, 2, 20, 29, 38, 47, 56, 65, 74, 0, 1, 18, 19, 9, 10, 11, 13, 14, 15, 16, 17, 3, 21, 30, 39, 48, 57, 66, 75, 4, 5, 22, 23, 9, 10, 11, 12, 14, 15, 16, 17, 4, 22, 31, 40, 49, 58, 67,
		76, 3, 5, 21, 23, 9, 10, 11, 12, 13, 15, 16, 17, 5, 23, 32, 41, 50, 59, 68, 77, 3, 4, 21, 22, 9, 10, 11, 12, 13, 14, 16, 17, 6, 24, 33, 42, 51, 60, 69, 78, 7, 8, 25, 26, 9, 10, 11, 12, 13, 14, 15, 17, 7, 25,
		34, 43, 52, 61, 70, 79, 6, 8, 24, 26, 9, 10, 11, 12, 13, 14, 15, 16, 8, 26, 35, 44, 53, 62, 71, 80, 6, 7, 24, 25, 19, 20, 21, 22, 23, 24, 25, 26, 0, 9, 27, 36, 45, 54, 63, 72, 1, 2, 10, 11, 18, 20, 21, 22, 23,
		24, 25, 26, 1, 10, 28, 37, 46, 55, 64, 73, 0, 2, 9, 11, 18, 19, 21, 22, 23, 24, 25, 26, 2, 11, 29, 38, 47, 56, 65, 74, 0, 1, 9, 10, 18, 19, 20, 22, 23, 24, 25, 26, 3, 12, 30, 39, 48, 57, 66, 75, 4, 5, 13, 14,
		18, 19, 20, 21, 23, 24, 25, 26, 4, 13, 31, 40, 49, 58, 67, 76, 3, 5, 12, 14, 18, 19, 20, 21, 22, 24, 25, 26, 5, 14, 32, 41, 50, 59, 68, 77, 3, 4, 12, 13, 18, 19, 20, 21, 22, 23, 25, 26, 6, 15, 33, 42, 51, 60,
		69, 78, 7, 8, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 7, 16, 34, 43, 52, 61, 70, 79, 6, 8, 15, 17, 18, 19, 20, 21, 22, 23, 24, 25, 8, 17, 35, 44, 53, 62, 71, 80, 6, 7, 15, 16, 28, 29, 30, 31, 32, 33, 34, 35, 0,
		9, 18, 36, 45, 54, 63, 72, 37, 38, 46, 47, 27, 29, 30, 31, 32, 33, 34, 35, 1, 10, 19, 37, 46, 55, 64, 73, 36, 38, 45, 47, 27, 28, 30, 31, 32, 33, 34, 35, 2, 11, 20, 38, 47, 56, 65, 74, 36, 37, 45, 46, 27, 28,
		29, 31, 32, 33, 34, 35, 3, 12, 21, 39, 48, 57, 66, 75, 40, 41, 49, 50, 27, 28, 29, 30, 32, 33, 34, 35, 4, 13, 22, 40, 49, 58, 67, 76, 39, 41, 48, 50, 27, 28, 29, 30, 31, 33, 34, 35, 5, 14, 23, 41, 50, 59, 68,
		77, 39, 40, 48, 49, 27, 28, 29, 30, 31, 32, 34, 35, 6, 15, 24, 42, 51, 60, 69, 78, 43, 44, 52, 53, 27, 28, 29, 30, 31, 32, 33, 35, 7, 16, 25, 43, 52, 61, 70, 79, 42, 44, 51, 53, 27, 28, 29, 30, 31, 32, 33, 34,
		8, 17, 26, 44, 53, 62, 71, 80, 42, 43, 51, 52, 37, 38, 39, 40, 41, 42, 43, 44, 0, 9, 18, 27, 45, 54, 63, 72, 28, 29, 46, 47, 36, 38, 39, 40, 41, 42, 43, 44, 1, 10, 19, 28, 46, 55, 64, 73, 27, 29, 45, 47, 36, 37,
		39, 40, 41, 42, 43, 44, 2, 11, 20, 29, 47, 56, 65, 74, 27, 28, 45, 46, 36, 37, 38, 40, 41, 42, 43, 44, 3, 12, 21, 30, 48, 57, 66, 75, 31, 32, 49, 50, 36, 37, 38, 39, 41, 42, 43, 44, 4, 13, 22, 31, 49, 58, 67,
		76, 30, 32, 48, 50, 36, 37, 38, 39, 40, 42, 43, 44, 5, 14, 23, 32, 50, 59, 68, 77, 30, 31, 48, 49, 36, 37, 38, 39, 40, 41, 43, 44, 6, 15, 24, 33, 51, 60, 69, 78, 34, 35, 52, 53, 36, 37, 38, 39, 40, 41, 42, 44,
		7, 16, 25, 34, 52, 61, 70, 79, 33, 35, 51, 53, 36, 37, 38, 39, 40, 41, 42, 43, 8, 17, 26, 35, 53, 62, 71, 80, 33, 34, 51, 52, 46, 47, 48, 49, 50, 51, 52, 53, 0, 9, 18, 27, 36, 54, 63, 72, 28, 29, 37, 38, 45, 47,
		48, 49, 50, 51, 52, 53, 1, 10, 19, 28, 37, 55, 64, 73, 27, 29, 36, 38, 45, 46, 48, 49, 50, 51, 52, 53, 2, 11, 20, 29, 38, 56, 65, 74, 27, 28, 36, 37, 45, 46, 47, 49, 50, 51, 52, 53, 3, 12, 21, 30, 39, 57, 66, 75,
		31, 32, 40, 41, 45, 46, 47, 48, 50, 51, 52, 53, 4, 13, 22, 31, 40, 58, 67, 76, 30, 32, 39, 41, 45, 46, 47, 48, 49, 51, 52, 53, 5, 14, 23, 32, 41, 59, 68, 77, 30, 31, 39, 40, 45, 46, 47, 48, 49, 50, 52, 53, 6,
		15, 24, 33, 42, 60, 69, 78, 34, 35, 43, 44, 45, 46, 47, 48, 49, 50, 51, 53, 7, 16, 25, 34, 43, 61, 70, 79, 33, 35, 42, 44, 45, 46, 47, 48, 49, 50, 51, 52, 8, 17, 26, 35, 44, 62, 71, 80, 33, 34, 42, 43, 55, 56,
		57, 58, 59, 60, 61, 62, 0, 9, 18, 27, 36, 45, 63, 72, 64, 65, 73, 74, 54, 56, 57, 58, 59, 60, 61, 62, 1, 10, 19, 28, 37, 46, 64, 73, 63, 65, 72, 74, 54, 55, 57, 58, 59, 60, 61, 62, 2, 11, 20, 29, 38, 47, 65, 74,
		63, 64, 72, 73, 54, 55, 56, 58, 59, 60, 61, 62, 3, 12, 21, 30, 39, 48, 66, 75, 67, 68, 76, 77, 54, 55, 56, 57, 59, 60, 61, 62, 4, 13, 22, 31, 40, 49, 67, 76, 66, 68, 75, 77, 54, 55, 56, 57, 58, 60, 61, 62, 5,
		14, 23, 32, 41, 50, 68, 77, 66, 67, 75, 76, 54, 55, 56, 57, 58, 59, 61, 62, 6, 15, 24, 33, 42, 51, 69, 78, 70, 71, 79, 80, 54, 55, 56, 57, 58, 59, 60, 62, 7, 16, 25, 34, 43, 52, 70, 79, 69, 71, 78, 80, 54, 55,
		56, 57, 58, 59, 60, 61, 8, 17, 26, 35, 44, 53, 71, 80, 69, 70, 78, 79, 64, 65, 66, 67, 68, 69, 70, 71, 0, 9, 18, 27, 36, 45, 54, 72, 55, 56, 73, 74, 63, 65, 66, 67, 68, 69, 70, 71, 1, 10, 19, 28, 37, 46, 55, 73,
		54, 56, 72, 74, 63, 64, 66, 67, 68, 69, 70, 71, 2, 11, 20, 29, 38, 47, 56, 74, 54, 55, 72, 73, 63, 64, 65, 67, 68, 69, 70, 71, 3, 12, 21, 30, 39, 48, 57, 75, 58, 59, 76, 77, 63, 64, 65, 66, 68, 69, 70, 71, 4,
		13, 22, 31, 40, 49, 58, 76, 57, 59, 75, 77, 63, 64, 65, 66, 67, 69, 70, 71, 5, 14, 23, 32, 41, 50, 59, 77, 57, 58, 75, 76, 63, 64, 65, 66, 67, 68, 70, 71, 6, 15, 24, 33, 42, 51, 60, 78, 61, 62, 79, 80, 63, 64,
		65, 66, 67, 68, 69, 71, 7, 16, 25, 34, 43, 52, 61, 79, 60, 62, 78, 80, 63, 64, 65, 66, 67, 68, 69, 70, 8, 17, 26, 35, 44, 53, 62, 80, 60, 61, 78, 79, 73, 74, 75, 76, 77, 78, 79, 80, 0, 9, 18, 27, 36, 45, 54, 63,
		55, 56, 64, 65, 72, 74, 75, 76, 77, 78, 79, 80, 1, 10, 19, 28, 37, 46, 55, 64, 54, 56, 63, 65, 72, 73, 75, 76, 77, 78, 79, 80, 2, 11, 20, 29, 38, 47, 56, 65, 54, 55, 63, 64, 72, 73, 74, 76, 77, 78, 79, 80, 3, 12,
		21, 30, 39, 48, 57, 66, 58, 59, 67, 68, 72, 73, 74, 75, 77, 78, 79, 80, 4, 13, 22, 31, 40, 49, 58, 67, 57, 59, 66, 68, 72, 73, 74, 75, 76, 78, 79, 80, 5, 14, 23, 32, 41, 50, 59, 68, 57, 58, 66, 67, 72, 73, 74,
		75, 76, 77, 79, 80, 6, 15, 24, 33, 42, 51, 60, 69, 61, 62, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80, 7, 16, 25, 34, 43, 52, 61, 70, 60, 62, 69, 71, 72, 73, 74, 75, 76, 77, 78, 79, 8, 17, 26, 35, 44, 53, 62, 71,
		60, 61, 69, 70 };
	const U32 MASK[9] = {
		0x00000001,
		0x00000002,
		0x00000004,
		0x00000008,
		0x00000010,
		0x00000020,
		0x00000040,
		0x00000080,
		0x00000100 };
	const U32 MASK_ALL = 0x000001FF;

	/* a REGION can be either a BLOCK, ROW, or COL*/
	const std::array<int, 9> BLOCK[9] = {	block_region(0), block_region(1), block_region(2),
											block_region(3), block_region(4), block_region(5),
											block_region(6), block_region(7), block_region(8) };
	const std::array<int, 9> ROW[9] = { row_region(0), row_region(1), row_region(2),
										row_region(3),row_region(4), row_region(5),
										row_region(6), row_region(7), row_region(8)};
	const std::array<int, 9> COL[9] = {	col_region(0), col_region(1), col_region(2),
										col_region(3),col_region(4), col_region(5),
										col_region(6), col_region(7), col_region(8) };
	enum class Status
	{
		Inconsistent, Inconclusive, Solved		
	};

	struct Arc
	{
		int priority;
		int from;
		int to;
	};

	inline bool operator<(const Arc & lhs, const Arc & rhs)
	{
		return lhs.priority > rhs.priority;
	}


	inline void insert_span(const int index, std::vector<int> & dst)
	{		
		
		if (!std::binary_search(dst.begin(), dst.end(), index))
		{
			int neighbors[20];
			auto length = 0;
			std::copy_if(NEIGHBOR_TABLE + index * 20, NEIGHBOR_TABLE + (index + 1) * 20, neighbors, [&length, &dst](const int i)
			{
				if (!std::binary_search(dst.begin(), dst.end(), i))
				{
					length++;					
					return true;
				}
				return false;
			});
			dst.push_back(index);
			dst.insert(dst.end(), neighbors, neighbors + length);	
			sort(dst.begin(), dst.end());
		}
	}
	
	template<typename ForwardIterator>
	bool evaluate(ForwardIterator forward_iterator);

	template<typename ForwardIterator>
	class SudokuBoard
	{
		friend bool evaluate<ForwardIterator>(ForwardIterator forward_iterator);		
		std::array<U32,81> domains;
		std::array<std::vector<int>,9> span;
		ForwardIterator forward_iterator;
		explicit SudokuBoard(ForwardIterator it) : forward_iterator(it)
		{
			for (auto i = 0; i < 81; i++)
			{
				if (*it == 0)
				{
					domains[i] = MASK_ALL;
				}
				else
				{
					domains[i] = MASK[*it- 1];
				}
				++it;
			}
			for (auto i = 0; i < 9; i++)
			{
				span[i] = make_span(i + 1);
			}
		}

		Status make_consistent(std::priority_queue<Arc> & arcs)
		{
			while (!arcs.empty())
			{
				const auto arc = arcs.top();
				arcs.pop();
				unsigned long to_zval;				
				if (__popcnt(domains[arc.to]) == 1)
				{
					_BitScanForward(&to_zval, domains[arc.to]);
					auto prev_domain = domains[arc.from];
					domains[arc.from] &= ~domains[arc.to];
					if (domains[arc.from] != prev_domain) {
						if (!domains[arc.from])
						{
							return Status::Inconsistent;
						}
						int hamming_weight = __popcnt(domains[arc.from]);
						if (hamming_weight == 1)
						{							
							unsigned long from_zval;
							_BitScanForward(&from_zval, domains[arc.from]);
							insert_span(arc.from, span[from_zval]);
						}
						for (auto i = 0; i < 20; i++)
						{
							auto neighbor = NEIGHBOR_TABLE[20 * arc.from + i];
							if (neighbor != arc.to)
							{
								arcs.push(Arc{ hamming_weight, neighbor, arc.from });
							}
						}
					}
				}				
			}
			return Status::Inconclusive;
		}

		Status AC3()
		{
			std::priority_queue<Arc> arcs;
			int priorities[81];
			for (auto index = 0; index < 81; index++)
			{
				priorities[index] = __popcnt(domains[index]);
			}
			for (auto i = 0; i < 1620; i++)
			{
				arcs.push(Arc{priorities[NEIGHBOR_TABLE[i]],  i / 20, NEIGHBOR_TABLE[i] });
			}
			return make_consistent(arcs);
		}

		std::vector<int> make_span(const int value) const
		{
			std::vector<int> dst;
			dst.reserve(81);
			for (auto index = 0; index < 81; index++)
			{
				if (__popcnt(domains[index]) == 1 && (domains[index] & MASK[value - 1]))
				{
					insert_span(index, dst);
				}
			}
			return dst;
		}

		bool coerce(std::priority_queue<Arc> & arcs, const std::array<int, 9> REGION[9])
		{
			auto return_flag = false;
			for (auto id = 0; id < 9; id++)
			{
				auto block_domain = MASK_ALL;
				for (const auto index : REGION[id])
				{
					if (__popcnt(domains[index]) == 1)
					{
						block_domain &= ~domains[index];
					}
				}
				auto value = 0;
				unsigned long pos;
				auto current = block_domain;
				while (_BitScanForward(&pos, current))
				{
					value += (pos + 1);
					current >>= (pos + 1);
					int illegal_region[90];
					// Corresponds to same-valued indices and its neighbors
					std::copy(span[value-1].begin(), span[value-1].end(), illegal_region);
					
					
					// Corresponds to all neighbors in the same region
					auto sublength = 0;						
					std::copy_if(REGION[id].data(), REGION[id].data() + 9, illegal_region + span[value-1].size(), [this, & sublength](int i)
					{
						if (__popcnt(this->domains[i]) == 1)
						{
							sublength++;
							return true;
						}
						return false;
					});

					auto length = sublength + span[value-1].size();
					std::sort(illegal_region, illegal_region + length);			

					std::optional<int> coerced_index;
					for (const auto index : REGION[id])
					{
						if (!std::binary_search(illegal_region, illegal_region + length, index ))
						{
							if (coerced_index)
							{
								coerced_index = {};
								break;
							}
							else
							{
								coerced_index = std::make_optional(index);
							}
						}
					}
					if (coerced_index)
					{
						return_flag = true;
						auto to = coerced_index.value();						
						domains[to] = MASK[value-1];						
						insert_span(to, span[value-1]);
						for (auto i = 0; i < 20; i++)
						{
							auto from = NEIGHBOR_TABLE[20 * to + i];
							arcs.push(Arc{ 1, from, to });
						}
						
					}
					
				}
			}
			return return_flag;
		}

		Status infer(std::priority_queue<Arc> & arcs)
		{
			while (!arcs.empty())
			{
				if (make_consistent(arcs) == Status::Inconsistent )
				{
					return Status::Inconsistent;
				}				
				coerce(arcs, BLOCK);
				coerce(arcs, ROW);
				coerce(arcs, COL);		
			}
			return Status::Inconclusive;
		}

		Status make_inferences()
		{
			std::priority_queue<Arc> arcs;
			int priorities[81];
			for (auto index = 0; index < 81; index++)
			{
				priorities[index] = __popcnt(domains[index]);
			}
			for (auto i = 0; i < 1620; i++)
			{
				arcs.push(Arc{ priorities[NEIGHBOR_TABLE[i]],  i / 20, NEIGHBOR_TABLE[i] });
			}
			return infer(arcs);
		}
	
		bool solved()
		{
			for (auto domain : domains)
			{
				if (__popcnt(domain) != 1)
				{
					return false;
				}
			}
			return true;
		}

		int min_weight()
		{
			auto min_index = -1;
			auto min = 10;
			for (auto index = 0; index < 81; index++)
			{
				int hamming_weight = __popcnt(domains[index]);
				// 2 is a lower-bound
				if (hamming_weight == 2)
				{
					return index;
				}
				if (hamming_weight != 1 && hamming_weight < min)
				{
					min = hamming_weight;
					min_index = index;
				}
			}
			return min_index;
		}

		SudokuBoard branch(int index, int val) const
		{
			auto clone(*this);
			clone.domains[index] = MASK[val - 1];
			insert_span(index, clone.span[val - 1]);
			return clone;
		}

		bool evaluate()
		{			
			auto res = make_inferences();
			switch (res) {
			case Status::Solved:		return true;
			case Status::Inconsistent:	return false;
				case Status::Inconclusive:	break;
			}
			if (solved())
			{
				update_iterator();
				return true;
			}
			auto unassigned_index = min_weight();
			auto value = 0;
			unsigned long pos;			
			auto current = domains[unassigned_index];
			while (_BitScanForward(&pos, current))
			{
				value += (pos + 1);
				current >>= (pos + 1);
				auto br = branch(unassigned_index, value);
				 if (br.evaluate())
				 {
					 return true;
				 }				
			}
			return false;
		}

		void update_iterator()
		{	
			for (auto index = 0; index < 81; index++)
			{
				if (__popcnt(domains[index]) == 1)
				{
					unsigned long pos;
					_BitScanForward(&pos, domains[index]);
					auto val = pos + 1;
					*forward_iterator = val;
					++forward_iterator;
				}
			}
		}		
	};
	inline std::string str(int dst[81])
	{
		std::string str;
		str.reserve(81);
		for (auto index = 0; index < 81; index++)
		{
			str.append(std::to_string(dst[index]));
		}
		return str;
	}
	inline std::string pretty_str(int dst[81])
	{
		std::string str;
		str.reserve(90);
		for (auto row = 0; row < 9; row++)
		{
			for (auto col = 0; col < 9; col++)
			{
				auto index = row * 9 + col;
				if (dst[index] == 0)
				{
					str.append("[ ]");
				}
				else
				{
					str.append("[");
					str.append(std::to_string(dst[index]));
					str.append("]");
				}
			}
			str.append("\n");
		}
		return str;
	}


	template<typename ForwardIterator>
	inline bool evaluate(ForwardIterator forward_iterator)
	{
		SudokuBoard<ForwardIterator> su(forward_iterator);		
		return su.evaluate();
	}

	inline bool evaluate(std::istream & in, std::ostream & out, bool pretty = false)
	{
		int board[81];
		auto i = 0;
		for (std::string line; std::getline(in, line) && i < 81;)
		{
			for (const auto c : line)
			{
				auto val = c - '0';
				board[i++] = (val > 0 && val < 10) ? val : 0;
			}
		}
		std::fill(board + i, board + 81, 0);
		auto status = evaluate(board);
		if (pretty)
		{
			out << pretty_str(board);
		}
		else
		{
			out << str(board);
		}
		return status;
	}
}
