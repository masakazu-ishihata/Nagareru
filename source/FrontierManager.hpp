#pragma once

////////////////////////////////////////
// include & namespace
////////////////////////////////////////
#include <vector>
#include <tdzdd/util/Graph.hpp>
using namespace tdzdd;


////////////////////////////////////////
// Frontier Manager
////////////////////////////////////////
class FrontierManager {

  ////////////////////////////////////////
  // typedef
  ////////////////////////////////////////
 private:

  typedef std::pair<int, int> Edge;


  ////////////////////////////////////////
  // members
  ////////////////////////////////////////
 private:

  // G
  const int n;          // # vertices
  const int m;          // # edges
  int w;                // Width = Maximum Frontier Size
  std::vector<Edge> EL; // EdgeList

  // Frontier
  std::vector<std::vector<int> > Es; // Entering vertices
  std::vector<std::vector<int> > Ls; // Leaving vertices
  std::vector<std::vector<int> > Fs; // Frontier vetices
  std::vector<std::vector<int> > Rs; // Remaining vertices

  // map : vertex <=> position of PodArray
  std::vector<int> v2p;
  std::vector<std::vector<int> > p2v;


  ////////////////////////////////////////
  // constructor
  ////////////////////////////////////////
 public:

  // construct from EL
  FrontierManager(const int n, const std::vector<Edge>& EL) :
      n(n), m(EL.size()), EL(EL)
  {
    initialize();
  }

  // construct from G
  FrontierManager(const tdzdd::Graph& G) : n(G.vertexSize()), m(G.edgeSize())
  {
    // initialize EL
    for (int i = 0; i < m; ++i){
      const tdzdd::Graph::EdgeInfo& e = G.edgeInfo(i);
      const int v1 = e.v1;
      const int v2 = e.v2;
      EL.push_back( Edge(v1, v2) );
    }

    // initialize Vertex Sets & Maps
    initialize();
  }


  ////////////////////////////////////////
  // construct Vertex Sets & Maps
  ////////////////////////////////////////
 private:

  // initialize
  void initialize()
  {
    constructEs();
    constructLs();
    constructFs();
    constructRs();
    constructMap();
  }

  // construct Es
  void constructEs()
  {
    std::set<int> S;
    this->Es.resize(this->m);

    for (int k = 0; k < this->m; ++k) {
      // e_k = {u, v}
      const int u = this->EL[k].first;
      const int v = this->EL[k].second;

      // u enters frontier
      if (S.count(u) == 0) {
        this->Es[k].push_back(u);
        S.insert(u);
      }

      // v enters frontier
      if (S.count(v) == 0) {
        this->Es[k].push_back(v);
        S.insert(v);
      }
    }
  }

  // construct Ls
  void constructLs()
  {
    std::set<int> S;
    this->Ls.resize(this->m);

    for (int k = this->m-1; k >= 0; --k) {
      // e_k = {u, v}
      const int u = EL[k].first;
      const int v = EL[k].second;

      // u leaves frontier
      if (S.count(u) == 0) {
        this->Ls[k].push_back(u);
        S.insert(u);
      }

      // u leaves frontier
      if (S.count(v) == 0) {
        this->Ls[k].push_back(v);
        S.insert(v);
      }
    }
  }

  // construct Fs : require Es & Ls
  void constructFs()
  {
    std::set<int> S;
    this->Fs.resize(this->m);
    this->w = 0;

    for (int k = 0; k < this->m; ++k) {
      // add Es to S
      for (auto itr = this->Es[k].begin(); itr != this->Es[k].end(); ++itr) {
        S.insert( *itr );
      }

      // copy S to Fs[k]
      for (auto itr = S.begin(); itr != S.end(); ++itr) {
        this->Fs[k].push_back(*itr);
      }

      // update max frontier size
      if (this->w < S.size()) {
        w = S.size();
      }

      // remove Ls from S
      for (auto itr = this->Ls[k].begin(); itr != this->Ls[k].end(); ++itr) {
        S.erase( *itr );
      }
    }
  }

  // construct Rs : require Fs & Ls
  void constructRs()
  {
    this->Rs.resize(this->m);

    // Rs[i] = Fs[i] \setminus Ls[i]
    for (int k = 0; k < this->m; ++k) {
      for (auto itr = this->Fs[k].begin(); itr != this->Fs[k].end(); ++itr) {
        if(std::find(this->Ls[k].begin(), this->Ls[k].end(), *itr) == this->Ls[k].end()) {
          Rs[k].push_back(*itr);
        }
      }
    }
  }

  // construct mapping : require Es & Ls
  void constructMap()
  {
    // resize
    this->v2p.resize(this->n + 1);
    this->p2v.resize(this->m);
    for (int k = 0; k < this->m; ++k) {
      this->p2v[k].resize(this->n + 1);
    }

    // unused Positions of PodArray
    std::vector<int> P;
    for (int i = this->w - 1; i >= 0; --i) {
      P.push_back(i);
    }

    for (int k = 0; k < this->m; ++k) {
      // copy the previous map
      if (k > 0) {
        for (int i = 0; i < this->n + 1; ++i) {
          p2v[k][i] = p2v[k - 1][i];
        }
      }

      // enterings : assign a new PodArray index
      for (auto itr = this->Es[k].begin(); itr != this->Es[k].end(); ++itr) {
        const int v = *itr;
        const int i = P.back();
        this->v2p[v] = i;
        this->p2v[k][i] = v;
        P.pop_back();
      }

      // leavings : collect the PodArray index
      for (auto itr = this->Ls[k].begin(); itr != this->Ls[k].end(); ++itr) {
        const int v = *itr;
        P.push_back( this->v2p[v] );
      }
    }
  }


  ////////////////////////////////////////
  // get graph info: n, m, w
  ////////////////////////////////////////
 public:

  int getVertexSize() const
  {
    return this->n;
  }

  int getEdgeSize() const
  {
    return this->m;
  }

  int getWidth() const
  {
    return this->w;
  }


  ////////////////////////////////////////
  // get edge info: eid, edge, v1, v2 of level, width W
  ////////////////////////////////////////
 public:

  int getEdgeId(const int level) const
  {
    return this->m - level;
  }

  const Edge& getEdge(const int level) const
  {
    return this->EL[ this->getEdgeId(level) ];
  }

  const std::vector<Edge> getEdgeList(void) const
  {
    return this->EL;
  }

  int getV1(const int level) const
  {
    return this->EL[ this->getEdgeId(level) ].first;
  }

  int getV2(const int level) const
  {
    return this->EL[ this->getEdgeId(level) ].second;
  }


  ////////////////////////////////////////
  // get frontier info: E, L, F, R of level
  ////////////////////////////////////////
 public:

  const std::vector<int>& getE(const int level) const
  {
    return this->Es[ this->getEdgeId(level) ];
  }

  const std::vector<int>& getL(const int level) const
  {
    return this->Ls[ this->getEdgeId(level) ];
  }

  const std::vector<int>& getF(const int level) const
  {
    return this->Fs[ this->getEdgeId(level) ];
  }

  const std::vector<int>& getR(const int level) const
  {
    return this->Rs[ this->getEdgeId(level) ];
  }


  ////////////////////////////////////////
  // maps : vertex <=> position of PodArray
  ////////////////////////////////////////
 public:

  int getPositionOf(const int v) const
  {
    return this->v2p[v];
  }

  int getVertexOf(const int level, const int p) const
  {
    return this->p2v[ this->getEdgeId(level) ][p];
  }


  ////////////////////////////////////////
  // print
  ////////////////////////////////////////
 public:

  void print(std::ostream &os = std::cout) const
  {
    os << "n = " << this->n << std::endl;
    os << "m = " << this->m << std::endl;
    os << "w = " << this->w << std::endl;

    for (int level = this->m; level > 0; --level) {
      const int k = getEdgeId(level);
      const int u = EL[k].first;
      const int v = EL[k].second;
      os << "<<<< level = " << level << " : " << u << ", " << v << " >>>>" << std::endl;

      // E
      os << "E =";
      for (auto itr = this->Es[k].begin(); itr != this->Es[k].end(); ++itr) {
        os << " " << *itr;
      }
      os << std::endl;

      // L
      os << "L =";
      for (auto itr = this->Ls[k].begin(); itr != this->Ls[k].end(); ++itr) {
        os << " " << *itr;
      }
      os << std::endl;

      // F
      os << "F =";
      for (auto itr = this->Fs[k].begin(); itr != this->Fs[k].end(); ++itr) {
        os << " " << *itr;
      }
      os << std::endl;

      // R
      os << "R =";
      for (auto itr = this->Rs[k].begin(); itr != this->Rs[k].end(); ++itr) {
        os << " " << *itr;
      }
      os << std::endl;
    }

    // Map
    os << "v2p =";
    for (int v = 1; v <= this->n; ++v) {
      os << " " << this->v2p[v];
    }
    os << std::endl;
  }
};
