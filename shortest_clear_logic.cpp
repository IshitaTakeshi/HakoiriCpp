#include <iostream>
#include <cstdio>
#include <cstring>
#include <Eigen/Dense>
#include <chrono>
#include <map>
#include "puzzle/puzzle_logic.hpp"
#include "puzzle/graph_logic.hpp"


size_t to_hash(const Matrix54i &simple_puzzle)
{
    size_t hash;
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

size_t hash_simplified_board(const Matrix54i & board) {
  return to_hash(board_simple(board));
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

std::vector<Node> breadth_first_search_dikstr(const Matrix54i &puzzle)
{
    HashNodeMap hash_node_map;
    std::unordered_set<size_t> simplified_board_hashes;
    std::queue<Matrix54i> puzzle_list;
    std::vector<size_t> clear_node_hashes;

    puzzle_list.push(puzzle);

    const size_t hash = hash_simplified_board(puzzle);
    hash_node_map[hash] = Node{puzzle, 0, std::vector<Node>{}};

    simplified_board_hashes.insert(hash);

    int i = 0;
    while (!puzzle_list.empty()) {
      i++;

      const Matrix54i now_puzzle = puzzle_list.front();
      puzzle_list.pop();
      const Node now = hash_node_map.at(hash_simplified_board(now_puzzle));

      for (const Matrix54i &board : moved_board_list(now_puzzle)) {
        const size_t hash = hash_simplified_board(board);

        if (simplified_board_hashes.find(hash) != simplified_board_hashes.end()) {
          // TODO compare cost and overwrite if it is less than existing
          hash_node_map.at(hash).side_node.push_back(now);
          continue;
        }

        const Node new_node{board, i, std::vector<Node>{now}};
        hash_node_map[hash] = new_node;

        simplified_board_hashes.insert(hash);
        puzzle_list.push(board);

        if (clear(board)) {
          clear_node_hashes.push_back(hash);
        }
      }
    }

    std::vector<Node> clear_nodes;
    for (const size_t hash : clear_node_hashes) {
      clear_nodes.push_back(hash_node_map.at(hash));
    }
    const std::vector<Node> s = shortestroute_find_dikstr(clear_nodes);
    std::cout << "総手数は" << simplified_board_hashes.size() << "手です" << std::endl;
    std::cout << "クリアルートは" << s.size() << "手です" << std::endl;
    return s;
}
