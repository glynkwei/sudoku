#pragma once
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include <boost/multi_array.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/algorithm/string.hpp>
#undef BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 
#undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 

#include <vector>
#include <string>
#include <math.h> 


//
 // Sudoku Solver
 //  Solve any nxn game of Sudoku using sudoku::solve

namespace sudoku
{
		// Domains of each cell are represented by a bitset.
	// If bit i is set, then the domain of the cell can contain value i+1

	inline boost::dynamic_bitset<> make_mask(const std::size_t val, const std::size_t N)
	{
		if (val > N)
		{
			throw std::runtime_error("N is not large enough to fit bitmask for val");
		}
		boost::dynamic_bitset<> mask(N,1);
		mask <<= (val - 1);
		return mask;
	}

	inline boost::dynamic_bitset<> make_mask_all(const std::size_t N)
	{
		boost::dynamic_bitset<> mask(N);
		mask.set();
		return mask;
	}

	// An inference can reach 1 of 3 conclusions: Inconsistent, Inconclusive, or Solved
	// Solved : The puzzle is in a solved state
	// Inconclusive : Not enough information is given to conclude anything
	// Inconsistent : The puzzle is inconsistent and is impossible to solve
	enum class Status
	{
		Inconsistent, Inconclusive, Solved
	};
	
	struct Arc
	{
		std::size_t from;
		std::size_t to;
	};	
	

	template<typename OutputIt>
	struct Analysis
	{		
		OutputIt last;
		int solutions_found;

		/* TODO: Copy stack trace, so it's possible to continue later on*/
	};

	class Solver
	{
		using DomainSet = std::vector<boost::dynamic_bitset<>>;
		const std::size_t block_row_count;
		const std::size_t block_col_count;
		const std::size_t N;
		const std::size_t num_neighbors;
		boost::multi_array<std::size_t, 2> neighborhoods;
		DomainSet masks;
		boost::dynamic_bitset<> mask_all;	

		std::size_t to_index(const std::size_t row, const std::size_t col) const
		{
			return N * row + col;
		}

		std::size_t neighbor_count()	const
		{
			//2 * (N - 1) are all neighbors within same row/column
			// N - (block_row_count + block_col_count - 1) are all neighbors in same block that are not in the same row/column
			return 2 * (N - 1) + (N - (block_row_count + block_col_count - 1));
		}

		// Have to do pass in neighborhood as output parameter as boost::multi_array has overly strict requirements for assignment operator
		template<typename ForwardIt>
		inline void populate_neighborhood(ForwardIt first, const std::size_t index)	const
		{
			const auto row = index / N;
			const auto col = index % N;
			for (std::size_t row_neighbor = 0; row_neighbor < N; row_neighbor++)
			{
				if (row != row_neighbor)
				{
					*first = to_index(row_neighbor, col);
					++first;
				}
			}
			for (std::size_t col_neighbor = 0; col_neighbor < N; col_neighbor++)
			{
				if (col != col_neighbor)
				{
					*first = to_index(row, col_neighbor);
					++first;
				}
			}
			const auto base_row = block_row_count * (row / block_row_count);
			const auto base_col = block_col_count * (col / block_col_count);
			const auto base = to_index(base_row, base_col);
			for (std::size_t block_neighbor = 0; block_neighbor < N; block_neighbor++)
			{
				const auto rel_row = block_neighbor / block_col_count;
				const auto rel_col = block_neighbor % block_col_count;
				if (base_row + rel_row != row && base_col + rel_col != col)
				{
					*first = base + to_index(rel_row, rel_col);
					++first;
				}
			}
		}
		void generate_neighborhood_tables(boost::multi_array<std::size_t, 2> & empty_neighborhoods) const
		{
			empty_neighborhoods.resize(boost::extents[N*N][num_neighbors]);
			for (std::size_t index = 0; index < N * N; index++)
			{
				populate_neighborhood(empty_neighborhoods[index].begin(), index);
			}
		}


		// Whenever a value is changed from the domain of an index, we need to propogate these changes to arcs
		void revise(std::size_t index, std::vector<Arc> & arcs) const
		{
			for (auto neighbor : neighborhoods[index])
			{
				arcs.push_back({ neighbor, index });
			}
		}

		// Create a container with all valid arcs
		std::vector<Arc> make_arcs() const
		{
			std::vector<Arc> arcs;
			arcs.reserve(N*N*num_neighbors*2);
			for (std::size_t index = 0; index < N * N; index++)
			{				 
				revise(index, arcs);				
			}
			return arcs;
		}
		// Runs the AC3 inference algorithm
		Status make_consistent(DomainSet & domains,std::vector<Arc> & arcs)	const
		{
			while (!arcs.empty())
			{
				const auto arc = arcs.back();
				arcs.pop_back();				
				auto & to_domain = domains[arc.to];
				auto & from_domain = domains[arc.from];
				if (to_domain.count() == 1)
				{
					auto prev_domain = from_domain;					
					from_domain &= ~to_domain;			
					if (from_domain != prev_domain) {
						if (from_domain.none())
						{
							return Status::Inconsistent;
						}						
						revise(arc.from, arcs);
					}
				}
			}
			return Status::Inconclusive;
		}		

		bool solved(const DomainSet & domains) const
		{
			for (std::size_t index = 0; index < N * N; index++)
			{
				if (domains[index].count() != 1)
				{
					return false;
				}
			}
			return true;	
		}

		// Returns the index which has the smallest non-one hamming weight
		std::size_t min_weight(const DomainSet & domains) const
		{
			std::size_t min_index = 0;
			auto min = N + 1;
			for (std::size_t index = 0; index < N * N; index++)
			{
				auto hamming_weight = domains[index].count();
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

		// Returns a domain with index set to val
		DomainSet branch(const DomainSet & domains, const std::size_t index,const std::size_t val) const
		{
			auto br = domains;
			br[index] = masks[val - 1];
			return br;
		}

		// Depth-first search to find a solved configuration, using AC3 inference along the way		
		template <typename OutputIt>
		OutputIt search(DomainSet & domains, std::vector<Arc> arcs, OutputIt & current_iterator, int & solutions_found, int solution_count,bool is_pretty)
		{			
			if (make_consistent(domains, arcs) == Status::Inconsistent)
			{
				return current_iterator;
			}
			if (solved(domains))
			{			
				if (is_pretty)
				{
					*current_iterator = pretty_str(domains);
				}
				else
				{
					*current_iterator = str(domains);
				}
				++current_iterator;
				solutions_found++;
				return current_iterator;
			}
			std::size_t value = 0;
			auto unassigned_index = min_weight(domains);
			auto current = domains[unassigned_index];
			// Iterates through all values within the domain
			while (solutions_found < solution_count)
			{
				auto pos =current.find_first();
				if (pos == boost::dynamic_bitset<>::npos)
				{
					break;
				}
				value += (pos + 1);
				current >>= (pos + 1);
				auto br = branch(domains, unassigned_index, value);
				auto br_arcs = arcs;
				revise(unassigned_index, br_arcs);
				search(br, br_arcs, current_iterator, solutions_found, solution_count, is_pretty);
			}
			return current_iterator;
		}
		std::string pretty_str(const DomainSet & domains) const
		{
			auto val_length = static_cast<int>(log10(N)) + 1;
			std::string repr;			
			auto length = (1+val_length)*N + 2 * block_row_count + 1;			
			repr.reserve(length * length);
			for (std::size_t row = 0; row < N; row++)
			{
				if (row % block_row_count == 0)
				{
					repr.insert(repr.end(), length, '-');
					repr.push_back('\n');
				}
				for (std::size_t col = 0; col < N; col++)
				{
					if (col % block_col_count == 0)
					{
						repr.append("| ");
					}
					auto index = to_index(row, col);
					if (domains[index].count() == 1)
					{
						auto val = domains[index].find_first() + 1;
						//Prepad with 0's if necessary
						auto padding = val_length - 1 - static_cast<int>(log10(val));
						repr.insert(repr.end(), padding, '0');
						repr.append(std::to_string(val));
					}
					else
					{
						repr.push_back(' ');
					}
					repr.push_back(' ');

				}
				repr.append("|\n");
			}
			repr.insert(repr.end(), length, '-');
			return repr;
		}
		std::string str(const DomainSet & domains) const
		{
			std::string str;
			str.reserve(N * N);
			for (std::size_t row = 0; row < N; row++)
			{
				for (std::size_t col = 0; col < N; col++)
				{
					auto index = to_index(row, col);
					if (domains[index].count() == 1)
					{
						str.append(std::to_string(domains[index].find_first() + 1));
					}
					else
					{
						str.push_back('0');
					}
					if (index != N*N - 1)
					{
						str.push_back(' ');
					}

				}
			}
			return str;
		}
		public:
			explicit Solver(const std::size_t b_row_count, const std::size_t b_col_count) :	block_row_count(b_row_count),
																							block_col_count(b_col_count),
																							N(block_row_count * block_col_count),
																							num_neighbors(neighbor_count())
			{
				if (b_row_count == 0 || b_col_count == 0)
				{
					throw std::runtime_error("Parameters cannot be 0");
				}
				generate_neighborhood_tables(neighborhoods);
				// Generate masks
				masks.reserve(N);
				for (std::size_t val = 1; val <= N; val++)
				{
					masks.push_back(make_mask(val, N));
				}
				mask_all = make_mask_all(N);			
			}

		template<typename OutputIt>
		Analysis<OutputIt> solve(std::string input, OutputIt output_iterator, int solution_count = 1, bool is_pretty = false)
		{			
			DomainSet domains(N*N, mask_all);
			auto index = 0;
			for (const auto c : input)
			{
				if (index == N*N)
				{
					break;
				}
				if (c != '\n' && c != '\r')
				{
					auto val = c - '0';
					domains[index] = (val > 0 && val < 10) ? masks[val - 1] : mask_all;
					index++;
				}
			}			
			auto solutions_found = 0;
			auto last_it = search(domains,make_arcs(),output_iterator, solutions_found, solution_count, is_pretty);
			return { last_it, solutions_found };
		}

		//TODO: Rewrite so search can handle numerical results, instead of having many conversions

		template<typename InputIt, typename OutputIt>
		Analysis<OutputIt> solve(InputIt input_iterator, OutputIt output_iterator, int solution_count = 1)
		{
			DomainSet domains(N*N, mask_all);
			for (auto index = 0; index < N * N; index++, ++input_iterator)
			{				
				auto val = *input_iterator;
				domains[index] = (val > 0 && val < 10) ? masks[val - 1] : mask_all;							
			}
			auto solutions_found = 0;
			std::vector<std::string> solutions(solution_count);
			auto begin = solutions.begin();
			auto last_it = search(domains, make_arcs(), begin, solutions_found, solution_count, false);

			
			std::for_each(solutions.begin(), solutions.end(), [& output_iterator](auto solution)
			{
				std::vector<std::string> delimited_solution;
				boost::split(delimited_solution, solution, boost::is_any_of(" "), boost::token_compress_on);
				std::for_each(delimited_solution.begin(), delimited_solution.end(), [& output_iterator](auto val_as_str)
				{
					*output_iterator = std::stoi(val_as_str);
					++output_iterator;
				});
			});

			return { output_iterator, solutions_found };
		}
	};
	
}
