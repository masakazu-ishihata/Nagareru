#pragma once

////////////////////////////////////////
// include
////////////////////////////////////////
#include <random>
#include <tdzdd/DdSpec.hpp>
#include <tdzdd/DdStructure.hpp>
#include <tdzdd/util/Graph.hpp>

#include "NagareruSpec.hpp"
#include "NagareruProblem.hpp"


////////////////////////////////////////
// namespace
////////////////////////////////////////
namespace nagareru
{


////////////////////////////////////////
// Nagareru Problem Generator
////////////////////////////////////////
class NagareruProblemGenerator
{
  ////////////////////////////////////////////////////////////////////////////////
  // private
  ////////////////////////////////////////////////////////////////////////////////
 private:

  ////////////////////////////////////////
  // Cell
  ////////////////////////////////////////
  struct Cell {
    int x;
    int y;
    Color col;
    Direction dir;
    Cell(const int x, const int y, const Color col, const Direction dir)
        : x(x), y(y), col(col), dir(dir) {};
  };

  ////////////////////////////////////////
  // Private member variables
  ////////////////////////////////////////
  bool debug;
  const unsigned seed;
  int num_call_countSolutions;
  std::mt19937_64 random_number_generator;


  ////////////////////////////////////////////////////////////////////////////////
  // public methods
  ////////////////////////////////////////////////////////////////////////////////
 public:

  ////////////////////////////////////////
  // accessors
  ////////////////////////////////////////
  int getNumCallCountSolutions(void) const
  {
    return num_call_countSolutions;
  }

  unsigned int getSeed(void) const
  {
    return seed;
  }

  unsigned int random(const int n)
  {
    return (random_number_generator() % n);
  }

  ////////////////////////////////////////
  // constructor
  ////////////////////////////////////////
  NagareruProblemGenerator(const unsigned int seed, const bool debug=false) :
      debug(debug),
      seed(seed),
      num_call_countSolutions(0),
      random_number_generator(std::mt19937_64(seed))
  {
  }

  ////////////////////////////////////////
  // reset
  ////////////////////////////////////////
  void reset(void)
  {
    num_call_countSolutions = 0;
  }

  ////////////////////////////////////////
  // print
  ////////////////////////////////////////
  void print(std::ostream& os=std::cout) const
  {
    os << "seed   : " << seed << std::endl;
    os << "# call : " << num_call_countSolutions << std::endl;
  }

  ////////////////////////////////////////
  // generate a random problem
  ////////////////////////////////////////
  NagareruProblem generate(const int W, const int H, const int N)
  {
    std::string cnt = "0";
    NagareruProblem P = getInitialBoard(W, H, N);

    // generate initial board
    cnt = countSolutions(P);
    while (cnt == "0") {
      if (debug) std::cout << "Unsatisfiable Board" << std::endl;
      P = getInitBoard(W, H, N);
      cnt = countSolutions(P);
    }
    if (debug) std::cout << "Init : " << cnt << std::endl;

    // add & dell
    for (int i = 0; i < 10; i++){
      cnt = addCells(P, cnt);
      cnt = delCells(P, cnt);
      if (cnt == "1") {
        break;
      }
    }

    // restart if cnt > 1
    if (cnt == "1") {
      return P;
    }
    else {
      return generate(W, H, N);
    }
  }

  ////////////////////////////////////////
  // initial board
  ////////////////////////////////////////
  NagareruProblem getInitialBoard(const int W, const int H, const int N)
  {
    NagareruProblem P(W, H);

    // random permutation over [W * H]
    std::vector<int> perm;
    for (int i = 0; i < W * H; ++i) {
      perm.push_back(i);
    }
    std::shuffle(perm.begin(), perm.end(), random_number_generator);

    // add W blacks without winds
    int num_added_cell = 0;
    if (N > 0){
      for (int l = 0; l < W * H; ++l){
        const int i = perm[l];

        // i = (x-1) + W(y-1)
        const int x = i % W + 1;
        const int y = (i - x) / W + 1;
        const Direction dir = static_cast<Direction>(random(5));

        // set (x, y) as Black without wind
        P.setCell(x, y, Color::Black, dir);
        P.properize();
        if (P.isMeaningless() || !P.isConnected()) {
          P.resetCell(x, y);
        }
        else {
          if (++num_added_cell == N) {
            break;
          }
        }
      }
    }

    // properize
    P.properize();

    return P;
  }

  ////////////////////////////////////////
  // get an initial board with N cells
  ////////////////////////////////////////
  NagareruProblem getInitBoard(const int W, const int H, const int N)
  {
    NagareruProblem P(W, H);

    for (int i = 0; i < N; i++) {
      // get addable cells
      std::vector<Cell> cells = getMeaningfullAddition(P);
      if (cells.size() == 0) {
        std::cout << "error : invalid N = " << N << std::endl;
        exit(1);
      }

      // choose a cell
      const int r = random(cells.size());
      const Cell cell = cells[r];

      // add & count
      P.setCell(cell.x, cell.y, cell.col, cell.dir);
    }

    // properize
    P.properize();

    return P;
  }


  ////////////////////////////////////////
  // randomlly add a cell to P if possible
  ////////////////////////////////////////
  std::string addCells(NagareruProblem &P, std::string prev_cnt)
  {
    if (debug) std::cout << "addCells : " << prev_cnt << std::endl;

    // P is good
    if (prev_cnt == "1") return prev_cnt;

    // get addable cells
    std::vector<Cell> cells = getMeaningfullAddition(P);
    std::string cnt;

    // choose one
    while (cells.size() > 0) {
      // choose a cell
      const int r = random(cells.size());
      const Cell cell = cells[r];
      cells.erase(cells.begin() + r);

      // add & count
      P.setCell(cell.x, cell.y, cell.col, cell.dir);
      P.properize();
      cnt = countSolutions(P);

      // P is valid
      if (cnt != "0") {
        return addCells(P, cnt);
      }

      // P is invalid : back track
      P.resetCell(cell.x, cell.y);
      P.properize();
    }

    return prev_cnt;
  }

  ////////////////////////////////////////
  // delete all redundant cells from P
  ////////////////////////////////////////
  std::string delCells(NagareruProblem &P, std::string prev_cnt)
  {
    if (debug) std::cout << "delCells : " << prev_cnt << std::endl;

    bool deleted = false;

    for (int x = 1; x <= P.getW(); ++x) {
      for (int y = 1; y <= P.getH(); ++y) {
        // skip if (x, y) is neither Black nor White
        Color col = P.getColor(x, y);
        if (col == Color::No || col == Color::Gray) {
          continue;
        }

        // del (x,y)
        NagareruProblem Q = P;
        Q.resetCell(x, y);
        Q.properize();

        // skip if Q is meaningless
        if (Q.isMeaningless()) {
          continue;
        }

        // call recursion if (x,y) is redundant
        const std::string cnt = countSolutions(Q);
        if (debug) std::cout << x << ", " << y << ", " << cnt << std::endl;
        if (cnt == prev_cnt) {
          P.resetCell(x, y);
          P.properize();
          deleted = true;
        }
      }
    }
    if (deleted) {
      return delCells(P, prev_cnt);
    }
    else {
      return prev_cnt;
    }
  }

  ////////////////////////////////////////
  // # solutions of P
  ////////////////////////////////////////
  const std::string countSolutions(const NagareruProblem &P)
  {
    num_call_countSolutions++;

    // P -> G
    const std::string file("__temp__.lst");
    P.dumpGraph(file);
    tdzdd::Graph G;
    G.readEdges(file);

    // G -> ZDD
    NagareruSpec spec(P, G);
    DdStructure<2> D = tdzdd::DdStructure<2>(spec);
    D.zddReduce();

    // count
    return D.zddCardinality();
  }

  ////////////////////////////////////////
  // get cells that P + the cell is meaningfull
  ////////////////////////////////////////
  std::vector<Cell> getMeaningfullAddition(const NagareruProblem& P) const
  {
    NagareruProblem Q = P;
    std::vector<Cell> cells;
    std::vector<Color> cols = {Color::White, Color::Black};

    for (int x = 1; x <= P.getW(); ++x) {
      for (int y = 1; y <= P.getH(); ++y) {
        // skip (x, y) if it is white or black
        if (Q.getColor(x, y) == Color::White) continue;
        if (Q.getColor(x, y) == Color::Black) continue;

        // try to change (x, y)
        for (int i = 0; i < cols.size(); ++i) {
          const Color col = cols[i];
          for (int j = 0; j < 5; ++j) {
            const Direction dir = static_cast<Direction>(j);
            Q.setCell(x, y, col, dir);
            Q.properize();
            if (!Q.isMeaningless() && Q.isConnected()) {
              cells.push_back(Cell(x, y, col, dir));
            }
            Q.resetCell(x, y);
          }
        }
      }
    }
    return cells;
  }
};

}
