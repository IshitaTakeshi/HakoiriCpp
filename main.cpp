
#include <iostream>
#include <cstdio>
#include <cstring>
#include <Eigen/Dense>
#include <vector>
#include <chrono>

#include "puzzle/graph_logic.hpp"
#include "puzzle/puzzle_logic.hpp"

int main()
{
    Matrix54i puzzle = init_puzzle();
    std::vector<Matrix54i> matrix_index;
    const std::vector<Node> min = breadth_first_search_dikstr(puzzle, matrix_index);

    std::ofstream myfile;
    myfile.open("example.csv");
    for (const Node & n : min)
    {
        std::string one_colmn = mat_to_str_pluscomma(matrix_index[n.puzzle]);
        one_colmn.pop_back();
        one_colmn += "\n";
        myfile << one_colmn;
    }
    myfile.close();

    return 0;
}
