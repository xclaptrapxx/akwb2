#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Edge {
public:
  int from;
  int to;

  Edge(int from, int to) : from(from), to(to) {}
  Edge() : from(0), to(0) {}
};

struct OriginalEdge {
  int id;
  int from;
  int to;

  OriginalEdge(int id, int from, int to) : id(id), from(from), to(to) {}
  OriginalEdge() : id(0), from(0), to(0) {}
};

class Graph {
public:
  std::vector<Edge> edgeList;
  std::vector<std::vector<int>> predecessorsList;
  std::vector<std::vector<int>> successorsList;
  std::vector<OriginalEdge> originalEdges;
  int originalGraphSize;

  Graph(int numVertices)
      : predecessorsList(numVertices + 1), successorsList(numVertices + 1) {}

  void addEdge(int from, int to) {
    Edge newEdge(from, to);
    edgeList.push_back(newEdge);

    successorsList[from].push_back(to);
    predecessorsList[to].push_back(from);
  }

  void displayGraph() {
    std::cout << "wczytany graf:\n";

    for (int i = 1; i < successorsList.size(); ++i) {
      std::cout << i << ": ";
      for (auto succ : successorsList[i]) {
        std::cout << succ << " ";
      }
      std::cout << '\n';
    }
  }

  bool isOneGraph() {
    for (int i = 1; i < successorsList.size(); ++i) {
      int currentVer = i;
      for (int j = 1; j < successorsList[i].size(); ++j) {
        if (successorsList[i][j - 1] == successorsList[i][j]) {
          return false;
        }
      }
    }
    return true;
  }

  void reindex() {
    std::vector<int> verInOrder;

    for (const auto &e : originalEdges) {
      verInOrder.push_back(e.from);
      verInOrder.push_back(e.to);
    }

    std::sort(verInOrder.begin(), verInOrder.end());

    std::unordered_map<int, int> hashmap;
    int counter = 1;
    for (int i = 0; i < verInOrder.size(); ++i) {
      if (hashmap.find(verInOrder[i]) == hashmap.end()) {
        hashmap[verInOrder[i]] = counter;
        ++counter;
      }
    }

    for (auto &e : originalEdges) {
      e.from = hashmap[e.from];
      e.to = hashmap[e.to];
    }

    originalGraphSize = originalEdges.back().to; 
  }

  bool isAdjoint() {
    for (int i = 1; i < successorsList.size() - 1; ++i) {
      for (int j = i + 1; j < successorsList.size(); ++j) {

        std::vector<int> intersecRes;
        intersecRes.clear();
        std::set_intersection(
            successorsList[i].begin(), successorsList[i].end(),
            successorsList[j].begin(), successorsList[j].end(),
            std::back_inserter(intersecRes));

        if (!intersecRes.empty()) {
          if (successorsList[i] != successorsList[j]) {
            std::cout << "successors of: " << i << j << "\n inters ";
            return false;
          }
        }
      }
    }
    return true;
  }

  bool isLine() {
    for (int i = 1; i < predecessorsList.size() - 1; ++i) {
      for (int j = i + 1; j < predecessorsList.size(); ++j) {

        std::vector<int> succIntersec;
        succIntersec.clear();
        std::set_intersection(
            successorsList[i].begin(), successorsList[i].end(),
            successorsList[j].begin(), successorsList[j].end(),
            std::back_inserter(succIntersec));

        std::vector<int> preIntersec;
        preIntersec.clear();
        std::set_intersection(
            predecessorsList[i].begin(), predecessorsList[i].end(),
            predecessorsList[j].begin(), predecessorsList[j].end(),
            std::back_inserter(preIntersec));

        if (!succIntersec.empty() && !preIntersec.empty()) {

          return false;
        }
      }
    }
    return true;
  }

  void transformation() {

    std::cout << "\ntransformation into original graph\n\n";

    for (int i = 1; i < successorsList.size(); ++i) {
      OriginalEdge e;
      e.id = i;
      e.from = (2 * i) - 1;
      e.to = (2 * i);
      originalEdges.push_back(e);
    }

    for (int i = 0; i < edgeList.size(); ++i) {

      int x_arc = edgeList[i].from;
      int y_arc = edgeList[i].to;

      int currentvaltochange = originalEdges[y_arc - 1].from;

      for (auto &e : originalEdges) {
        if (e.to == currentvaltochange) {
          e.to = originalEdges[x_arc - 1].to;
        }
      }

      originalEdges[y_arc - 1].from = originalEdges[x_arc - 1].to;
    }
  }

  void saveOriginalToFile() {

    std::vector<std::vector<int>> originalAdjList(originalGraphSize + 1);

    for (const auto &e : originalEdges) {
      originalAdjList[e.from].push_back(e.to);
    }

    std::ofstream output("output.txt");
    if (output.is_open()) {

      output << originalGraphSize << "\n";
      std::cout << originalGraphSize << "\n";

      for (int i = 1; i <= originalGraphSize; ++i) {
        output << i << ": ";
        std::cout << i << ": ";

        for (const auto &edge : originalAdjList[i]) {
          output << edge << " ";
          std::cout << edge << " ";
        }

        output << "\n";
        std::cout << "\n";
      }
      output.close();
    }
  }

  enum { OneGraph = 1, AdjointGraph = 2, LineGraph = 3, UnknownGraph = 0 };

  int graphType() {
    if (!isOneGraph()) {
      return UnknownGraph;
    }

    if (!isAdjoint()) {
      return OneGraph;
    }

    if (!isLine()) {
      return AdjointGraph;
    }
    return LineGraph;
  }

  std::string enum_str(int e) {
    switch (e) {
    case OneGraph:
      return "1-graph";
    case AdjointGraph:
      return "adjoint graph";
    case LineGraph:
      return "line graph";
    default:
      return "unknown graph type";
    }
  }

  void print_graph_type(int e) {
    std::cout << "\nloaded graph is a " << enum_str(e) << '\n';

    if (e == AdjointGraph || e == LineGraph) {
      transformation();
      reindex();
      saveOriginalToFile();
    }
  }
};

int main() {

  // std::cout << "program przyjmuje plik w formacie wierzcholek: nastepnik1
  // nastepnik2...\n"; std::string file; std::cin >> file;
  std::fstream input("adjList.txt");
  if (!input.is_open()) {
    std::cerr << "cannot open the file\n";
    return 1;
  }

  int vertices;
  input >> vertices;
  Graph g(vertices);

  std::string line;
  while (std::getline(input, line)) {
    std::istringstream iss(line);
    int from;
    char separator;
    iss >> from >> separator;

    int to;

    while (iss >> to) {
      g.addEdge(from, to);
    }

    iss.clear();
  }

  g.displayGraph();
  int graph_type = g.graphType();
  g.print_graph_type(graph_type);
  return 0;
}