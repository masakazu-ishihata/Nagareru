#pragma once

////////////////////////////////////////
// include
////////////////////////////////////////
// standard libraries
#include <iostream>
#include <fstream>

// tdzdd
#include <tdzdd/DdSpec.hpp>
#include <tdzdd/DdStructure.hpp>
#include <tdzdd/util/Graph.hpp>


////////////////////////////////////////
// namespace
////////////////////////////////////////
namespace nagareru
{

////////////////////////////////////////
// for CNF
////////////////////////////////////////
#define E(i, j) ((i < j) ? ("e_"+std::to_string(i)+"_"+std::to_string(j)) : ("e_"+std::to_string(j)+"_"+std::to_string(i)))
#define D(i) ("d_" + std::to_string(i))
#define X(i) ("x_" + std::to_string(i))
#define Y(i) ("y_" + std::to_string(i))


////////////////////////////////////////
// color & direction
////////////////////////////////////////
enum class Color : int {White=0, Black, Gray, No};
enum class Direction : int {Up=0, Down, Left, Right, No};

static const char* ColorNames[] = {"White", "Black", "Gray", "No"};
static const char* DirectionNames[] = {"Up", "Down", "Left", "Right", "No"};
static const char* DirectionArrows[] = {"↑", "↓", "←", "→", "*"};

std::string ctos(const Color c)
{
  std::string tmp( ColorNames[static_cast<int>(c)] );
  return tmp;
};
std::string dtos(const Direction d)
{
  std::string tmp( DirectionNames[static_cast<int>(d)] );
  return tmp;
}

const Direction getOppositeDirection(const Direction d)
{
  switch (d) {
    case Direction::Up:    return Direction::Down;
    case Direction::Down:  return Direction::Up;
    case Direction::Left:  return Direction::Right;
    case Direction::Right: return Direction::Left;
    default:               return Direction::No;
  }
}


////////////////////////////////////////
// Nagareru Problem
////////////////////////////////////////
class NagareruProblem
{
  ////////////////////////////////////////////////////////////////////////////////
  // private
  ////////////////////////////////////////////////////////////////////////////////
 private:

  ////////////////////////////////////////
  // struct
  ////////////////////////////////////////
  struct Cell {
    const int x;
    const int y;
    const char col;
    const char dir;
    Cell(const int x, const int y, const char col, const char dir)
        : x(x), y(y), col(col), dir(dir) {}
  };

  struct Edge {
    const int v1;
    const int v2;
    Edge(const int v1, const int v2) : v1(v1), v2(v2) {}
  };

  ////////////////////////////////////////
  // class members
  ////////////////////////////////////////
  int W;  // width
  int H;  // height

  int num_white_cells; // # white celss
  int num_white_edges; // # white edges
  int last_white_edge; // the index of the last white edge

  std::vector< Color > C;                    // colors of cells
  std::vector< std::vector< Direction > > D; // direction sets of cells
  std::vector< Edge > E;                     // edge list

  ////////////////////////////////////////
  // private set/add
  ////////////////////////////////////////
  void setColor(const int x, const int y, const Color col)
  {
    C[ getPos(x,y) ] = col;
  }

  void addDirection(const int x, const int y, const Direction dir)
  {
    D[ getPos(x,y) ].push_back(dir);
  }

  void setColor(const int x, const int y, const char c)
  {
    switch(c){
      case 'w': case 'W': setColor(x, y, Color::White); break;
      case 'b': case 'B': setColor(x, y, Color::Black); break;
      case 'g': case 'G': setColor(x, y, Color::Gray);  break;
      default: setColor(x, y, Color::No); break;
    }
  }

  void addDirection(const int x, const int y, const char d)
  {
    switch(d){
      case 'u': case 'U': addDirection(x, y, Direction::Up);    break;
      case 'd': case 'D': addDirection(x, y, Direction::Down);  break;
      case 'r': case 'R': addDirection(x, y, Direction::Right); break;
      case 'l': case 'L': addDirection(x, y, Direction::Left);  break;
      default: addDirection(x, y, Direction::No); break;
    }
  }

  void setCell(const Cell cell)
  {
    setColor(cell.x, cell.y, cell.col);
    addDirection(cell.x, cell.y, cell.dir);
  }

  void setCells(const std::vector<Cell> cells)
  {
    for (int i = 0; i < cells.size(); ++i) {
      setCell(cells[i]);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  // public
  ////////////////////////////////////////////////////////////////////////////////
 public:

  ////////////////////////////////////////
  // public set/add
  ////////////////////////////////////////
  void setCell(const int x, const int y, const Color col, const Direction dir)
  {
    // assert
    assert(1 <= x && x <= W);
    assert(1 <= y && y <= H);
    assert(col == Color::White || col == Color::Black);

    // set
    const int i = getPos(x, y);
    C[i] = col;
    D[i].clear();
    D[i].push_back(dir);
  }

  void resetCell(const int x, const int y)
  {
    // assert
    assert(1 <= x && x <= W);
    assert(1 <= y && y <= H);

    // reset
    C[ getPos(x,y) ] = Color::No;
    D[ getPos(x,y) ].clear();
  }

  ////////////////////////////////////////
  // accessors
  ////////////////////////////////////////
  const int getW() const
  {
    return W;
  }

  const int getH() const
  {
    return H;
  }

  const int getNumWhiteCells() const
  {
    return num_white_cells;
  }

  const int getNumWhiteEdges() const
  {
    return num_white_edges;
  }

  const int getLastWhiteEdge() const
  {
    return last_white_edge;
  }

  const int getPos(const int x, const int y) const
  {
    return W * (y-1) + (x-1);
  }

  const Color getColor(const int i) const
  {
    return C[i];
  }

  const Color getColor(const int x, const int y) const
  {
    return C[ getPos(x,y) ];
  }

  const std::vector<Direction>& getDirection(const int i) const
  {
    return D[i];
  }

  const std::vector<Direction>& getDirection(const int x, const int y) const
  {
    return D[ getPos(x,y) ];
  }

  Direction getRelativeDirection(const int i, const int j) const
  {
    if(i+1 == j) return Direction::Right;
    if(i == j+1) return Direction::Left;
    if(i+W == j) return Direction::Down;
    if(i == j+W) return Direction::Up;
    return Direction::No;
  }

  std::vector<Direction> getWinds(const int x, const int y) const
  {
    std::vector<Direction> winds;

    // check Up
    for (int z = y + 1; z <= H; ++z) {
      if (getColor(x,z) == Color::Black) {
        if (getDirection(x,z)[0] == Direction::Up) {
          winds.push_back(Direction::Up);
        }
        break;
      }
    }

    // chek Down
    for (int z = y - 1; z > 0; --z) {
      if (getColor(x,z) == Color::Black) {
        if (getDirection(x,z)[0] == Direction::Down){
          winds.push_back(Direction::Down);
        }
        break;
      }
    }

    // check Left
    for (int z = x + 1; z <= W; ++z) {
      if (getColor(z,y) == Color::Black) {
        if (getDirection(z,y)[0] == Direction::Left) {
          winds.push_back(Direction::Left);
        }
        break;
      }
    }

    // check Right
    for (int z = x - 1; z > 0; --z) {
      if (getColor(z,y) == Color::Black) {
        if (getDirection(z,y)[0] == Direction::Right) {
          winds.push_back(Direction::Right);
        }
        break;
      }
    }

    return winds;
  }

  ////////////////////////////////////////
  // is
  ////////////////////////////////////////
  bool isOnBoard(const int i) const
  {
    return (0 <= i && i < W*H);
  }

  bool isOnBoard(const int x, const int y) const
  {
    return (1 <= x && x <= W && 1 <= y && y <= H);
  }

  bool isValidNode(const int i) const
  {
    // out of board
    if(!isOnBoard(i)) return false;

    // black
    if(C[i] == Color::Black) return false;

    // gray with inconsistent direction pair
    if(C[i] == Color::Gray && D[i].size() > 1){
      for (int j = 0; j < D[i].size()-1; ++j) {
        for (int k = j+1; k < D[i].size(); ++k){
          if (D[i][j] == getOppositeDirection(D[i][k])) {
            return false;
          }
        }
      }
    }

    return true;
  }

  bool isValidEdge(const int i, const int j) const
  {
    if (i > j) return isValidEdge(j, i);

    // inconsistent with board
    if (i+1 != j && i+W != j)       return false;
    if (i+1 == j && (i+1) % W == 0) return false;
    if (i+W == j && (i+W) >= W * H) return false;

    // at least one endpoint is not valid
    if (!isValidNode(i)) return false;
    if (!isValidNode(j)) return false;

    // edge direction is inconsist with flow direction
    const Direction dij = getRelativeDirection(i, j);
    const Direction dji = getRelativeDirection(j, i);
    if (C[i]==Color::White && dij!=D[i][0] && dji!=D[i][0]) return false;
    if (C[j]==Color::White && dij!=D[j][0] && dji!=D[j][0]) return false;

    return true;
  }

  bool isMeaninglessWhiteCell(const int x, const int y) const
  {
    if (getColor(x, y) != Color::White) return false;

    // its direction
    const Direction d = getDirection(x, y)[0];

    // white without direction
    if (d == Direction::No) return true;

    // (x,y) should be windless or winds = {d}
    std::vector<Direction> win = getWinds(x, y);
    if (win.size() > 1 || (win.size() == 1 && win[0] != d)) return true;

    // d -> (dx, dy)
    int dx=0, dy=0;
    switch (d) {
      case Direction::Up:    dx =  0; dy = -1; break;
      case Direction::Down:  dx =  0; dy = +1; break;
      case Direction::Left:  dx = -1; dy =  0; break;
      case Direction::Right: dx = +1; dy =  0; break;
      default: dx = 0; dy = 0; break;
    }

    // (x+dx,y+dy) and (x-dx,y-dy) are not on the board
    if (!isOnBoard(x+dx, y+dy)) return true;
    if (!isOnBoard(x-dx, y-dy)) return true;

    // (x+dx,y+dy) and (x-dx,y-dy) are white or black
    const Color cf = getColor(x+dx,y+dy);
    const Color cb = getColor(x-dx,y-dy);
    if (cf==Color::Black) return true;
    if (cb==Color::Black) return true;
    if (cf==Color::White && getDirection(x+dx,y+dy)[0] != getDirection(x,y)[0]) return true;
    if (cb==Color::White && getDirection(x-dx,y-dy)[0] != getDirection(x,y)[0]) return true;

    return false;
  }

  bool isMeaninglessBlackCell(const int x, const int y) const
  {
    if (getColor(x, y) != Color::Black) return false;

    // its direction
    const Direction d = getDirection(x, y)[0];

    // (x,y) should be consistent with all winds
    std::vector<Direction> win = getWinds(x, y);
    if (win.size() > 0) {
      bool find = false;
      for (int l = 0; l < win.size(); ++l) {
        if (win[l] == getOppositeDirection(d)) {
          find = true;
          break;
        }
      }
      if (find) {
        return true;
      }
    }

    // d -> (dx, dy)
    int dx=0, dy=0;
    switch (d) {
      case Direction::Up:    dx =  0; dy = -1; break;
      case Direction::Down:  dx =  0; dy = +1; break;
      case Direction::Left:  dx = -1; dy =  0; break;
      case Direction::Right: dx = +1; dy =  0; break;
      default: dx = 0; dy = 0; break;
    }

    // Black faces black or anthor black
    if (d != Direction::No) {
      if (!isOnBoard(x+dx,y+dy)) return true;
      if (getColor(x+dx,y+dy)==Color::Black) return true;
    }

    return false;
  }

  bool isMeaningless(void) const
  {
    for (int x = 1; x <= W; ++x) {
      for (int y = 1; y <= H; ++y) {
        if (isMeaninglessWhiteCell(x, y) ) return true;
        if (isMeaninglessBlackCell(x, y) ) return true;
      }
    }
    return false;
  }

  bool isProper(void) const
  {
    // colorless cell has no direction
    for (int x = 1; x <= W; ++x) {
      for (int y = 1; y <= H; ++y) {
        const Color col = getColor(x, y);
        const std::vector<Direction> dir = getDirection(x, y);
        const std::vector<Direction> win = getWinds(x, y);
        switch (col)
        {
          case Color::White:
            if (dir.size() != 1 || dir[0] == Direction::No) {
              return false;
            }
            break;

          case Color::Black:
            if (dir.size() != 1) {
              return false;
            }
            break;

          case Color::Gray:
            if (dir.size() != win.size() ) {
              return false;
            }

            for (int i = 0; i < dir.size(); ++i) {
              bool find = false;
              for (int j = 0; j< win.size(); ++j){
                if (dir[i] == win[j]) {
                  find = true;
                  break;
                }
              }
              if (!find) {
                return false;
              }
            }
            break;

          default:
            if (dir.size() != 0) {
              return false;
            }
            break;
        }
      }
    }

    return true;
  }

  bool isConnected(void) const
  {
    // require proparized

    // flag of visit
    std::vector<bool> visited;
    for (int i = 0; i < W*H; ++i) {
      visited.push_back(false);
    }

    // search
    const int r = E[0].v1;
    visited[r] = true;
    for (int i = 0; i < E.size(); ++i) {
      const Edge e = E[i];
      if (visited[e.v1]) {
        visited[e.v2] = true;
      }
    }

    // check
    for (int i = 0; i < W*H; ++i) {
      if (C[i] != Color::Black && !visited[i]) {
        return false;
      }
    }

    return true;
  }

  ////////////////////////////////////////
  // properize
  ////////////////////////////////////////
  bool properize(void)
  {
    // reset current Gray cells
    for (int x = 1; x <= W; ++x) {
      for (int y = 1; y <= H ; ++y) {
        if (getColor(x, y) == Color::Gray){
          resetCell(x, y);
        }
      }
    }

    // (re)compute Gray cells
    for (int x = 1; x <= W; ++x) {
      for (int y = 1; y <= H; ++y) {
        if (getColor(x,y) == Color::Black) {
          const Direction d =  getDirection(x,y)[0];
          switch (d){
            case Direction::Up:
              for (int z = y - 1; z > 0; --z) {
                if (getColor(x,z) == Color::Black) break;
                if (getColor(x,z) == Color::White) continue;
                setColor(x, z, Color::Gray);
                addDirection(x, z, d);
              }
              break;
            case Direction::Down:
              for (int z = y + 1; z <= H; ++z) {
                if (getColor(x,z) == Color::Black) break;
                if (getColor(x,z) == Color::White) continue;
                setColor(x, z, Color::Gray);
                addDirection(x, z, d);
              }
              break;
            case Direction::Left:
              for (int z = x - 1; z > 0; --z) {
                if (getColor(z,y) == Color::Black) break;
                if (getColor(z,y) == Color::White) continue;
                setColor(z, y, Color::Gray);
                addDirection(z, y, d);
              }
              break;
            case Direction::Right:
              for (int z = x + 1; z <= W; ++z) {
                if (getColor(z,y) == Color::Black) break;
                if (getColor(z,y) == Color::White) continue;
                setColor(z, y, Color::Gray);
                addDirection(z, y, d);
              }
              break;
            default:
              break;
          }
        }
      }
    }

    // (re)construct edge list E
    E.clear();
    for (int i = 0; i < W * H; ++i){
      if (isValidEdge(i, i+1)) E.push_back( Edge(i, i+1) );
      if (isValidEdge(i, i+W)) E.push_back( Edge(i, i+W) );
    }

    // (re)count white cells
    num_white_cells = 0;
    for (int i = 0; i< W * H; ++i) {
      if (C[i] == Color::White) {
        num_white_cells++;
      }
    }

    // (re)count white edges
    num_white_edges = 0;
    last_white_edge = 0;
    for (int i = 0; i < E.size(); ++i) {
      if (C[E[i].v1] == Color::White || C[E[i].v2] == Color::White) {
        num_white_edges++;
        last_white_edge = i;
      }
    }

    return true;
  }

  ////////////////////////////////////////
  // const from pzprv3 file
  ////////////////////////////////////////
  NagareruProblem(const std::string file)
      : W(0), H(0), num_white_cells(0), num_white_edges(0)
  {
    // open file
    std::ifstream ifs(file, std::ios::in);
    if (!ifs) throw std::runtime_error(strerror(errno));

    std::string line;

    // line 1 : format
    getline(ifs, line);
    assert(line == "pzprv3");

    // line 2 : puzzle name
    getline(ifs, line);
    assert(line == "nagare");

    // line 3 : H
    getline(ifs, line);
    H = std::stoi(line);

    // line 4 : W
    getline(ifs, line);
    W = std::stoi(line);

    // lint 5-5+H : row
    std::vector<Cell> cells;
    for (int y = 1; y <= H; ++y) {
      getline(ifs, line);
      for (int x = 1; x <= W; ++x) {
        const char c = line[2*(x-1)];
        if (c == '.') continue;
        const char col = (std::isupper(c)) ? 'B' : 'W';
        const char dir = ('a' <= c && c <= 'z') ? c-32 : c;
        Cell cell(x, y, col, dir);
        cells.push_back(cell);
      }
    }

    // close file
    ifs.close();

    // initialize
    C.resize(W * H);
    D.resize(W * H);
    for (int i = 0; i < W * H; ++i) {
      C[i] = Color::No;
    }
    setCells(cells);
    properize();
  }

  ////////////////////////////////////////
  // constractor : default
  ////////////////////////////////////////
  NagareruProblem(const int W, const int H)
      : W(W), H(H), num_white_cells(0), num_white_edges(0)
  {
    C.resize(W * H);
    D.resize(W * H);
    for (int i = 0; i < W * H; ++i) {
      C[i] = Color::No;
    }
  }

  ////////////////////////////////////////
  // print
  ////////////////////////////////////////
  void print(std::ostream& os=std::cout) const
  {
    os << "Width   : " << W << std::endl;
    os << "Height  : " << H << std::endl;
    os << "# White Cells : " << num_white_cells << std::endl;
    os << "# White Edges : " << num_white_edges << std::endl;
    os << "Proper ? : " << (isProper() ? "Yes" : "No") << std::endl; 
    for (int i = 0; i < W; ++i)
      for (int j = 0; j < H; ++j)
        printCell(i+1, j+1, os);
  }

  void printCell(const int x, const int y, std::ostream &os = std::cout) const
  {
    std::string cstr = ctos( getColor(x,y) );
    std::string dstr = "";
    const std::vector<Direction>& Ds = getDirection(x, y);
    for (int i = 0; i < Ds.size(); ++i) {
      dstr += dtos( Ds[i] );
      if (i != Ds.size() - 1) {
        dstr += "&";
      }
    }
    os << "(" << x << "," << y << ") = [" << cstr << ":" << dstr << "]" << std::endl;
  }

  ////////////////////////////////////////
  // dump the board (with a solution) as dot
  ////////////////////////////////////////
  void dumpBoard(const std::string file,  const std::set<int>& S=std::set<int>()) const
  {
    std::ofstream fout;
    fout.open(file);
    fout << "graph {" << std::endl;

    // nodes
    for (int i = 0; i < W * H; ++i)
      dumpBoardNode(fout, i);

    // edges
    for (int i = 0; i < W * H; ++i){
      if ((i+1) % W != 0)
        dumpBoardEdge(fout, i, i + 1);

      if (i + W < W * H)
        dumpBoardEdge(fout, i, i + W);
    }

    // Solution
    std::set<int>::iterator itr = S.begin();
    while (itr != S.end()) {
      int i = E.size() - *itr;
      fout << E[i].v1 << "--" << E[i].v2;
      fout << " [color=red, style=bold];" << std::endl;
      ++itr;
    }

    // rank
    for (int i = 0; i < W * H; ++i){
      if ((i+1) % W == 1) fout << "{rank = same";
      fout << ";" << i;
      if ((i+1) % W == 0) fout << "};" << std::endl;
    }

    fout << "}" << std::endl;
    fout.close();
  }

  void dumpBoardNode(std::ofstream &fout, const int i) const
  {
    fout << i << " [shape=square,style=filled,";

    // color
    switch( C[i] ){
      case Color::White:
        fout << "fillcolor=white,fontcolor=black,";
        break;
      case Color::Black:
        fout << "fillcolor=black,fontcolor=white,";
        break;
      case Color::Gray:
        fout << "fillcolor=gray,fontcolor=white,";
        break;
      default:
        fout << "fillcolor=blue,";
        break;
    }

    // label
    std::string label = "";
    for(int j = 0; j < D[i].size(); ++j) {
      label += DirectionArrows[ static_cast<int>(D[i][j]) ];
    }

    fout << "label=\"" << i << ":" << label << "\"];" << std::endl;
  }

  void dumpBoardEdge(std::ofstream& fout, const int i, const int j) const
  {
    fout << i << "--" << j << " [";
    fout << ((i+1==j) ? "headport=w,tailport=e" : "headport=n,tailport=s");
    if (!isValidEdge(i, j) ) fout << ",style=dotted";
    fout << "];" << std::endl;
  }

  ////////////////////////////////////////
  // dump the problem graph as edge list
  ////////////////////////////////////////
  void dumpGraph(const std::string file) const
  {
    std::ofstream fout;
    fout.open(file);
    for (int i = 0; i < E.size(); ++i)
      fout << E[i].v1 << " " << E[i].v2 << std::endl;
    fout.close();
  }

  ////////////////////////////////////////
  // dump as pzprv3 file
  ////////////////////////////////////////
  void dump(const std::string file) const
  {
    std::ofstream fout;
    fout.open(file);
    fout << "pzprv3" << std::endl;
    fout << "nagare" << std::endl;
    fout << H << std::endl;
    fout << W << std::endl;

    char d;
    for (int y = 1; y <= H; ++y) {
      for (int x = 1; x <= W; ++x) {
        switch (getColor(x, y)) {
          case Color::White:
            d = dtos( getDirection(x, y)[0] )[0];
            fout << char(d + 32);
            break;
          case Color::Black:
            d = dtos( getDirection(x, y)[0] )[0];
            fout << d;
            break;
          default:
            fout << ".";
        }
        fout << ((x == W) ? "\n" : " ");
      }
    }
    fout.close();
  }

  ////////////////////////////////////////
  // dump as CNF
  ////////////////////////////////////////
  void dumpCNF(const std::string file) const
  {
    std::ofstream fout;
    fout.open(file);

    // # of valid nodes & edges
    int num_nodes = 0;
    int num_edges = 0;
    for (int i = 0; i < W * H; ++i)
      if (isValidNode(i)) num_nodes++;
    for (int i = 0; i < W * H; ++i) {
      if (isValidEdge(i, i+1)) num_edges++;
      if (isValidEdge(i, i+W)) num_edges++;
    }

    ////////////////////////////////////////
    // define variables
    ////////////////////////////////////////
    fout << "(domain degree (0 2))" << std::endl;

    for (int i = 0; i < W * H; ++i) {
      if (isValidEdge(i, i+1)) {
        fout << "(int " + E(i,i+1) + " -1 1)" << std::endl;
      }
      if (isValidEdge(i, i+W)) {
        fout << "(int " + E(i,i+W) + " -1 1)" << std::endl;
      }
    }
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(int " + D(i) + " degree)" << std::endl;
      }
    }
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(int " << X(i) << " 0 " << num_nodes << ")" << std::endl;
      }
    }
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(int " << Y(i) << " 0 1)" << std::endl;
      }
    }

    ////////////////////////////////////////
    // cocnstraints
    ////////////////////////////////////////
    const std::vector<int> diffs = {-W, -1, 1, W};

    // d_i = \sum |e_i_j|
    // (= d_i  (+ (abs e_i_j) ...))
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(= " << D(i) << " (+";
        for (int l = 0; l < diffs.size(); ++l) {
          const int j = i + diffs[l];
          if (isValidEdge(i,j)) {
            fout << " (abs " << E(i, j) << ")";
          }
        }
        fout << "))" << std::endl;
      }
    }

    // indegree = outdegree
    // (= (+ e_(i-1)_i e_(i-W)_i (neg e_i_(i+1)) (neg e_i_(i+W))))
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(= (+";
        for (int l = 0; l < diffs.size(); ++l) {
          const int j = i + diffs[l];
          if (isValidEdge(i,j)) {
            if (j < i) {
              fout << " " << E(i, j);
            }
            else {
              fout << " (neg " << E(i, j) << ")";
            }
          }
        }
        fout << ") 0)" << std::endl;
      }
    }

    // d_i > 0 <=> x_i > 0
    // (iff (> d_i 0) (> x_i 0))
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(iff (> " << D(i) << " 0) (> " << X(i) << " 0))" << std::endl;
      }
    }

    // x_i = 1 <=> y_i = 1
    // (=> (= x_i 1) (= y_i 1))
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << "(iff (= " << X(i) << " 1) (= " << Y(i) << " 1))" << std::endl;
      }
    }

    // e_i_j =  1 => (> x_j x_i) or (x_j = 1)
    // e_i_j = -1 => (> x_i x_j) or (x_i = 1)
    // (=> (> e_i_j 0) (or (> x_j x_i) (= x_j 1)))
    // (=> (< e_i_j 0) (or (> x_j x_i) (= x_i 1)))
    for (int i = 0; i < W * H; ++i) {
      for (int l = 2; l < diffs.size(); ++l) {
        const int j = i + diffs[l];
        if (isValidEdge(i, j)) {
          fout << "(=> (> " << E(i,j) << " 0) (or (> " << X(j) << " " << X(i) << ") (= " << X(j) << " 1)))" << std::endl;
          fout << "(=> (< " << E(i,j) << " 0) (or (> " << X(i) << " " << X(j) << ") (= " << X(i) << " 1)))" << std::endl;
        }
      }
    }

    // i is White
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i) && C[i] == Color::White) {
        // (> x_i 0) : redundant
        fout << "(> "  << X(i) << " 0)" << std::endl;

        // follow the arrow : (= e_j_i b) & (= e_i_k b)
        // j -> i -> j
        const Direction d = D[i][0];
        const int j = (d==Direction::Up||d==Direction::Down) ? (i-W) : (i-1);
        const int k = (d==Direction::Up||d==Direction::Down) ? (i+W) : (i+1);
        const int b = (d==Direction::Up||d==Direction::Left) ? -1 : +1;
        if (isValidEdge(j, i)) {
          fout << "(= " << E(j, i) << " " << b << ")" << std::endl;
        }
        if (isValidEdge(i, k)) {
          fout << "(= " << E(i, k) << " " << b << ")" << std::endl;
        }
      }
    }

    // i is Gray
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i) && C[i] == Color::Gray) {
        // do not against the wind : (!= e_i_j b) : i < j
        for (int l = 0; l < D[i].size(); ++l) {
          const int j = (D[i][l]==Direction::Up||D[i][l]==Direction::Down) ? (i+W) : (i+1);
          const int b = (D[i][l]==Direction::Up||D[i][l]==Direction::Left) ? 1 : -1;
          if (isValidEdge(i, j)) {
            fout << "(!= " << E(i, j) << " " << b << ")" << std::endl;
          }
        }

        // do not cross the wind : (=> (!= e_i_j/) 0) (= e_i_k/j 0))
        // j -> j -> k : Orthogonal to D[i][l] -> |E(j,i)| = 0 or |E(i,k)| = 0
        for (int l = 0; l < D[i].size(); ++l){
          const int j = (D[i][l]==Direction::Up||D[i][l]==Direction::Down) ? (i-1) : (i-W);
          const int k = (D[i][l]==Direction::Up||D[i][l]==Direction::Down) ? (i+1) : (i+W);
          if (isValidEdge(j, i) && isValidEdge(i, k)) {
            fout << "(or (= " << E(j,i) << " 0) (= " << E(i,k) << " 0))" << std::endl;
          }
        }
      }
    }

    // sum_i y_i = 1
    // (= (+ y_1 ... y_num_nodes) 1)
    fout << "(= (+";
    for (int i = 0; i < W * H; ++i) {
      if (isValidNode(i)) {
        fout << " " << Y(i);
      }
    }
    fout << ") 1)" << std::endl;

    fout.close();
  }

  ////////////////////////////////////////
  // dump all solutions
  ////////////////////////////////////////
  void dumpSolutions(const tdzdd::DdStructure<2>& D, const std::string header) const
  {
    int i = 0;
    tdzdd::DdStructure<2>::const_iterator itr = D.begin();
    while (itr != D.end()) {
      std::set<int> S = *itr;
      std::ostringstream oss;
      oss << header << "_solution_" << ++i << ".dot";
      dumpBoard(oss.str(), S);
      ++itr;
    }
  }
};

}
