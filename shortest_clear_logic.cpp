#include <iostream>
#include <cstdio>
#include <cstring>
#include <Eigen/Dense>
#include <chrono>
#include <map>
#include "puzzle/puzzle_logic.hpp"
#include "puzzle/graph_logic.hpp"


std::size_t to_hash(const Matrix54i &simple_puzzle)
{
    std::size_t hash;
    int rows = simple_puzzle.rows();
    int cols = simple_puzzle.cols();

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            int num = simple_puzzle(y, x);
            hash ^= num + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
    }
    return hash;
}

void dikstrqueue(
    const int i,
    const Matrix54i &now_puzzle,
    const std::vector<Matrix54i> &movable,
    comparative_index &puzzle_index,
    HashNodeMap &edges,
    std::vector<Node> &clear_nodes,
    std::queue<Matrix54i> &puzzle_list,
    std::vector<Matrix54i> &matrix_index)
{
  const Node now = edges.at(to_hash(board_simple(now_puzzle)));

  for (const Matrix54i &state : movable)
  {
    const Matrix54i state_simple = board_simple(state);
    const size_t hash_value = to_hash(state_simple);

    if (puzzle_index.find(hash_value) != puzzle_index.end()) {
      edges.at(hash_value).side_node.push_back(now);
      continue;
    }

    matrix_index.push_back(state);
    const Node new_node{matrix_index.size() - 1, i, std::vector<Node>{now}};
    edges[hash_value] = new_node;
    puzzle_index.insert(hash_value);
    puzzle_list.push(state);

    if (clear(state))
    {
      clear_nodes.push_back(new_node);
    }
  }
}

Node find_min_cost_edge(Node &now_node)
{
    // 最も小さいノードを定義
    Node min_node;
    int min_cost = -1;

    bool is_first = true;
    for (const Node & n : now_node.side_node) {
        if (is_first) {
            min_node = n;
            min_cost = min_node.cost;
            is_first = true;
            continue;
        }
        if (n.cost < min_cost) {
            min_node = n;
            min_cost = min_node.cost;
        }
    }

    return min_node;
}

std::vector<Node> find_clear_route(const Node & clear_node) {
    std::vector<Node> route;
    Node n = clear_node;
    while (n.cost > 0) {
        route.push_back(n);
        n = find_min_cost_edge(n);
    }
    return route;
}

std::vector<Node> shortestroute_find_dikstr(const std::vector<Node> & clear_nodes)
{
  bool is_first = true;
  std::vector<Node> shortest_route;
  for(const Node & clear_node : clear_nodes) {
    const std::vector<Node> route = find_clear_route(clear_node);
    if (is_first) {
      is_first = false;
      shortest_route = route;
      continue;
    }
    if (route.size() < shortest_route.size()) {
      shortest_route = route;
    }
  }
  return shortest_route;
}

std::vector<Node> breadth_first_search_dikstr(const Matrix54i &puzzle, std::vector<Matrix54i> &matrix_index)
{
    HashNodeMap edges;
    comparative_index puzzle_index;
    std::queue<Matrix54i> puzzle_list;
    std::vector<Node> clear_nodes;
    // dequeに初期盤面を追加
    puzzle_list.push(puzzle);
    // puzzleを複製
    // puzzleを比較するための形にする
    const Matrix54i simple_puzzle = board_simple(puzzle);

    matrix_index.push_back(puzzle);
    const size_t hash = to_hash(simple_puzzle);
    edges[hash] = Node{matrix_index.size() - 1, 0, std::vector<Node>{}};
    puzzle_index.insert(hash);

    int i = 0;
    while (!puzzle_list.empty())
    {
        const Matrix54i now_puzzle = puzzle_list.front();
        puzzle_list.pop();
        std::vector<Matrix54i> movable = moved_board_list(now_puzzle);
        i++;
        dikstrqueue(i, now_puzzle, movable,
                    puzzle_index, edges, clear_nodes, puzzle_list, matrix_index);
    }

    const std::vector<Node> s = shortestroute_find_dikstr(clear_nodes);
    std::cout << "総手数は" << puzzle_index.size() << "手です" << std::endl;
    std::cout << "クリアルートは" << s.size() << "手です" << std::endl;
    return s;
}
