#pragma once

////////////////////////////////////////
// include
////////////////////////////////////////
#include "FrontierManager.hpp"
#include "NagareruProblem.hpp"


////////////////////////////////////////
// name space
////////////////////////////////////////
namespace nagareru
{


////////////////////////////////////////
// state of a vertex : comp
////////////////////////////////////////
class NagareruState {
 public:
  int  M;       // mate
  bool U;       // upper or not
  Direction N;  // relative direction from neighbors to self
};


////////////////////////////////////////
// Spec of Nagareru
////////////////////////////////////////
class NagareruSpec : public tdzdd::PodArrayDdSpec<NagareruSpec, NagareruState, 2>
{
  ////////////////////////////////////////
  // class members
  ////////////////////////////////////////
 private:

  const NagareruProblem& P;
  const tdzdd::Graph& G;
  const FrontierManager fm;

  ////////////////////////////////////////
  // accessors
  ////////////////////////////////////////
 private:

  int getM(NagareruState* s, const int v) const
  {
    return s[ fm.getPositionOf(v) ].M;
  }

  bool getU(NagareruState *s, const int v) const
  {
    return s[ fm.getPositionOf(v) ].U;
  }

  Direction getN(NagareruState* s, const int v) const
  {
    return s[ fm.getPositionOf(v) ].N;
  }

  void setM(NagareruState* s, const int v, const int u) const
  {
    s[ fm.getPositionOf(v) ].M = u;
  }

  void setU(NagareruState *s, const int v, const bool b) const
  {
    s[ fm.getPositionOf(v) ].U = b;
  }

  void setN(NagareruState *s, const int v, const Direction d) const
  {
    s[ fm.getPositionOf(v) ].N = d;
  }

  ////////////////////////////////////////
  // Initialize
  ////////////////////////////////////////
 private:

  void initializeState(NagareruState* s) const
  {
    for (int i = 0; i < fm.getWidth(); ++i){
      s[i].M = 0;
      s[i].U = false;
      s[i].N = Direction::No;
    }
  }

  ////////////////////////////////////////
  // util
  ////////////////////////////////////////
 private:

  int getCellId(const int v) const
  {
    return std::stoi( G.vertexName(v) );
  }

  ////////////////////////////////////////
  // print state
  ////////////////////////////////////////
 public:

  void printState(NagareruState* s, const int level, std::ostream& os=std::cout) const
  {
    const int v1  = fm.getV1(level);
    const int v2  = fm.getV2(level);
    const int ci1 = getCellId(v1);
    const int ci2 = getCellId(v2);

    os << "<<<< level = " << level << " >>>>" << std::endl;
    os << "v1 = " << v1 << " (ci1 = " << ci1 << ")"  << std::endl;
    os << "v2 = " << v2 << " (ci2 = " << ci2 << ")" << std::endl;
    os << "edge direction = " << dtos( P.getRelativeDirection(ci1, ci2) ) << std::endl;

    // frontier infromation
    const std::vector<int>& F = fm.getF(level);
    for (int i = 0; i < F.size(); ++i) {
      const int v = F[i];
      const int m = getM(s, v);
      const Direction n = getN(s, v);
      const bool u = getU(s, v);
      std::string dir = dtos(n);
      os << G.vertexName(v) << " : " << (m==0 ?"-":G.vertexName(m)) << ", " << dir << ", " << u << std::endl;
    }
  }


  ////////////////////////////////////////
  // Constructor
  ////////////////////////////////////////
 public:

  NagareruSpec(const NagareruProblem& _P, const tdzdd::Graph& _G)
      : P(_P), G(_G), fm(_G)
  {
    setArraySize( fm.getWidth() );
  }

  ////////////////////////////////////////
  // create a root
  ////////////////////////////////////////
 public:

  int getRoot(NagareruState* s) const
  {
    initializeState(s);
    return fm.getEdgeSize();
  }

  ////////////////////////////////////////
  // create a child
  ////////////////////////////////////////
 public:

  int getChild(NagareruState* s, const int level, const int value) const
  {
    ////////////////////////////////////////
    // vertices that should be checked
    ////////////////////////////////////////
    const std::vector<int>& E = fm.getE(level);
    const std::vector<int>& F = fm.getF(level);
    const std::vector<int>& L = fm.getL(level);

    ////////////////////////////////////////
    // For v \in E : vertices that are entering the frontier
    ////////////////////////////////////////
    for(int i = 0; i < E.size(); ++i) {
      const int v = E[i];
      setM(s, v, v);
      setU(s, v, false);
      setN(s, v, Direction::No);
    }

    ////////////////////////////////////////
    // edge information
    ////////////////////////////////////////
    // edge (v1, v2)
    const int v1 = fm.getV1(level);
    const int v2 = fm.getV2(level);

    // cell index of the original problem
    const int ci1 = getCellId(v1);
    const int ci2 = getCellId(v2);

    // color of v1/v2
    const Color c1 = P.getColor(ci1);
    const Color c2 = P.getColor(ci2);

    // directions of v1/v2
    const std::vector<Direction>& ds1 = P.getDirection(ci1);
    const std::vector<Direction>& ds2 = P.getDirection(ci2);

    // relative direction of v1/v2
    const Direction v1_to_v2 = P.getRelativeDirection(ci1, ci2);
    const Direction v2_to_v1 = P.getRelativeDirection(ci2, ci1);

    // mate of v1/v2
    const int m1 = getM(s, v1);
    const int m2 = getM(s, v2);

    // relative directin from its neighbor to v1/v2
    const Direction n1 = getN(s, v1);
    const Direction n2 = getN(s, v2);

    // upper of v1/v2/m1/m2
    const bool u_v1 = getU(s, v1);
    const bool u_v2 = getU(s, v2);
    const bool u_m1 = getU(s, m1);
    const bool u_m2 = getU(s, m2);

    ////////////////////////////////////////
    // Take the edge
    ////////////////////////////////////////
    if(value == 1){
      ////////////////////////////////////////
      // Pruning
      ////////////////////////////////////////
      // Pruning : detect branch
      if (m1 == 0 || m2 == 0){
        return 0;
      }

      // Pruning : detect backflow by connecting two paths
      if ((u_v1 && u_v2) || (u_m1 && u_m2)){
        return 0;
      }

      // Pruning : the edge flows backward
      if (c1 == Color::White || c1 == Color::Gray) {
        // v side is upper -> flow must be (v -> u)
        if(u_m1 || u_v2){
          for (int i = 0; i < ds1.size(); ++i){
            if (v2_to_v1 == ds1[i]){
              return 0;
            }
          }
        }
        // u side is upper -> flow must be (v <- u)
        else if(u_m2 || u_v1){
          for (int i = 0; i < ds1.size() ; ++i){
            if (v1_to_v2 == ds1[i]){
              return 0;
            }
          }
        }
      }
      if (c2 == Color::White || c2 == Color::Gray) {
        // u side is upper -> flow must be (u -> v)
        if(u_m2 || u_v1){
          for (int i = 0; i < ds2.size(); ++ i){
            if (v1_to_v2 == ds2[i]){
              return 0;
            }
          }
        }
        // v side is upper -> flow must be (v -> u)
        else if(u_m1 || u_v2){
          for (int i = 0; i < ds2.size(); ++ i){
            if (v2_to_v1 == ds2[i]){
              return 0;
            }
          }
        }
      }

      // Pruning : ignore a gray flow two times in a row
      if (c1 == Color::Gray) {
        for (int i = 0; i < ds1.size(); ++i){
          if (n1==v1_to_v2 && v1_to_v2!=ds1[i] && v2_to_v1!=ds1[i]){
            return 0;
          }
        }
      }
      if (c2 == Color::Gray) {
        for (int i = 0; i < ds2.size(); ++i){
          if (n2==v2_to_v1 && v1_to_v2!=ds2[i] && v2_to_v1!=ds2[i]){
            return 0;
          }
        }
      }

      // Pruning : detect cycle
      if(m1 == v2 && m2 == v1){
        // there is a redundant fragment
        for (int i = 0; i < F.size(); ++i){
          const int v = F[i];
          const int m = getM(s, v);
          if (v!=v1 && v!=v2 && m!=0 && m!=v){
            return 0;
          }
        }

        // there is non-used white edge
        //        if (ss < P.getNumWhiteEdges() ){
        if (fm.getEdgeSize() - level < P.getLastWhiteEdge() ) {
          return 0;
        }

        // successfully construct a cycle
        else {
          return -1;
        }
      }

      ////////////////////////////////////////
      // Update
      ////////////////////////////////////////
      // update M
      setM(s, m1, m2);
      setM(s, m2, m1);
      if (m1 != v1) setM(s, v1, 0);
      if (m2 != v2) setM(s, v2, 0);

      // update N : relative directin from neighbor to self
      setN(s, v1, m1==v1 ? v2_to_v1 : Direction::No);
      setN(s, v2, m2==v2 ? v1_to_v2 : Direction::No);

      // update U : the edge flows along the cell direction
      if (c1 == Color::White || c1 == Color::Gray){
        for (int i = 0; i < ds1.size(); ++i){
          if (v1_to_v2 == ds1[i]) {
            if (u_m2 || u_v1) return 0;
            setU(s, m1, true);
          }
          else if (v2_to_v1 == ds1[i]) {
            if (u_m1 || u_v2) return 0;
            setU(s, m2, true);
          }
        }
      }
      if (c2 == Color::White || c2 == Color::Gray){
        for (int i = 0; i < ds2.size(); ++i){
          if (v1_to_v2 == ds2[i]) {
            if (u_m2 || u_v1) return 0;
            setU(s, m1, true);
          }
          else if (v2_to_v1 == ds2[i]) {
            if (u_m1 || u_v2) return 0;
            setU(s, m2, true);
          }
        }
      }
      if (m1 != v1 && u_v1) setU(s, m2, true);
      if (m2 != v2 && u_v2) setU(s, m1, true);
      if (m1 != v1) setU(s, v1, false);
      if (m2 != v2) setU(s, v2, false);
    }

    ////////////////////////////////////////
    // Don't take the edge
    ////////////////////////////////////////
    else {
      // pruning : omit a white edge
      if (c1 == Color::White || c2 == Color::White) {
        return 0;
      }
    }

    ////////////////////////////////////////
    // For v \in L : vertices that are leaving the frontier
    ////////////////////////////////////////
    for (int i = 0; i < L.size(); ++i) {
      const int v = L[i];
      const int m = getM(s, v);

      // Pruning : v is fixed as an endpoint
      if (m != 0 && m != v){
        return 0;
      }

      // initalize state
      setM(s, v, 0);
      setN(s, v, Direction::No);
      setU(s, v, false);
    }

    // Pruning : reach the last edge without completing a cycle
    if (level == 1){
      return 0;
    }

    return level - 1;
  }
};

}
