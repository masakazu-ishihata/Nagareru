#include "main.hpp"

#define DEBUG 0

int main(int argc, char *argv[])
{
  ////////////////////////////////////////
  // arguments
  ////////////////////////////////////////
  bool flag_dump  = false;
  bool flag_print = false;
  bool flag_rand  = false;
  bool flag_cnf   = false;

  std::random_device seed_gen;
  unsigned int seed = seed_gen();
  int W = 5;
  int H = 5;
  int N = 0;

  std::string problem_file = "example.txt";

  for (int i = 1; i < argc; ++i) {
    const std::string arg(argv[i]);
    if (arg == "--dump") {
      flag_dump = true;
    }
    else if (arg == "--print") {
      flag_print = true;
    }
    else if (arg == "--rand") {
      flag_rand = true;
    }
    else if (arg == "--cnf") {
      flag_cnf = true;
    }
    else if (arg == "--width") {
      W = atoi( argv[++i] );
    }
    else if (arg == "--height") {
      H = atoi( argv[++i] );
    }
    else if (arg == "--init") {
      N = atoi( argv[++i] );
    }
    else if (arg == "--seed") {
      seed = atoi( argv[++i] );
    }
    else if (arg == "--file") {
      problem_file = argv[++i];
    }
    else {
      std::cout << "Options" << std::endl;
      std::cout << "--file [string] : the name of a problem file" << std::endl;
      std::cout << "--print : print the problem" << std::endl;
      std::cout << "--dump  : export various dot files" << std::endl;
      std::cout << "--cnf   : export the cnf file" << std::endl;
      std::cout << "<<<< generation >>>>" << std::endl;
      std::cout << "--rand : create a random instance" << std::endl;
      std::cout << "--width  [int] : width" << std::endl;
      std::cout << "--height [int] : height" << std::endl;
      std::cout << "--init   [int] : # black cells on init board" << std::endl;
      std::cout << "--seed   [int] : seed of a random instance" << std::endl;
      exit(1);
    }
  }

  // get basename
  const int i = problem_file.find_last_of("/") + 1;
  const int j = problem_file.find_last_of(".");
  std::string base = problem_file.substr(i, j-i);

  // file names
  std::string graph_lst_file = base + "_graph.lst";
  std::string cnf_file = base + ".cnf";
  std::string board_file = base + ".dot";
  std::string graph_dot_file = base + "_graph.dot";
  std::string zdd_file= base + "_zdd.dot";


  ////////////////////////////////////////
  // generate
  ////////////////////////////////////////
  if (flag_rand) {
    std::cout << "////////////////////////////////////////" << std::endl;
    std::cout << "// Random Generation" << std::endl;
    std::cout << "////////////////////////////////////////" << std::endl;
    std::cout << "Problem File : " << problem_file << std::endl;
    std::cout << "CNF File     : " << cnf_file << std::endl;
    std::cout << "Board Width  : " << W << std::endl;
    std::cout << "Board Height : " << H << std::endl;
    std::cout << "Init # Cells : " << N << std::endl;
    std::cout << "Seed         : " << seed << std::endl;

    ////////////////////////////////////////
    // Generate a random problem
    ////////////////////////////////////////
    nagareru::NagareruProblemGenerator g(seed, flag_print);
    nagareru::NagareruProblem R = g.generate(W, H, N);
    R.dump(problem_file);
    R.dumpCNF(cnf_file);
    std::cout << "# count call : " << g.getNumCallCountSolutions() << std::endl;
  }

  ////////////////////////////////////////
  // solve
  ////////////////////////////////////////
  else {
    std::cout << "////////////////////////////////////////" << std::endl;
    std::cout << "// Solving" << std::endl;
    std::cout << "////////////////////////////////////////" << std::endl;
    std::cout << "Problem File : " << problem_file << std::endl;

    ////////////////////////////////////////
    // Read a problem
    ////////////////////////////////////////
    nagareru::NagareruProblem P(problem_file);
    if (flag_print) P.print();
    P.dumpGraph(graph_lst_file);


    ////////////////////////////////////////
    // Load the input graph G
    ////////////////////////////////////////
    tdzdd::Graph G;
    G.readEdges(graph_lst_file);
    std::cout << "# board nodes : " << G.vertexSize() << std::endl;
    std::cout << "# board edges : " << G.edgeSize() << std::endl;


    ////////////////////////////////////////
    // Execute the frontier method */
    ////////////////////////////////////////
    nagareru::NagareruSpec spec(P, G);
    DdStructure<2> D = tdzdd::DdStructure<2>(spec);
    D.zddReduce();
    std::cout << "# zdd nodes (non-reduced) : " << D.size() << std::endl;
    std::cout << "# zdd nodes (reduced)     : " << D.size() << std::endl;
    std::cout << "# solutions : " << D.zddCardinality() << std::endl;


    ////////////////////////////////////////
    // Export dot files
    ////////////////////////////////////////
    if (flag_cnf) {
      P.dumpCNF(cnf_file);
    }

    if (flag_dump) {
      // boad
      P.dumpBoard(board_file);

      // graph
      std::ofstream gos(graph_dot_file);
      G.dump(gos);
      gos.close();

      // zdd
      std::ofstream zos(zdd_file);
      D.dumpDot(zos);
      zos.close();

      // solutions
      P.dumpSolutions(D, base);
    }
  }
}
