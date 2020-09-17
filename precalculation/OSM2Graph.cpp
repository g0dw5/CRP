//
// Created by wang.song on 9/17/20.
//

#include <iostream>

#include "../io/OSMParser.h"
#include "../io/GraphIO.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " path_to_osm.bz2 path_to_output.graph.bz2" << std::endl;
    return 1;
  }

  std::string input(argv[1]);
  std::string graphOutput(argv[2]);

  CRP::OSMParser osmParser;
  CRP::Graph graph;
  bool ok = osmParser.parseGraph(input, graph);
  if (ok) {
    ok = CRP::GraphIO::writeGraph(graph, graphOutput);
  }

  if (!ok) {
    std::cout << "An error occured during parsing" << std::endl;
  }

  return 0;
}
