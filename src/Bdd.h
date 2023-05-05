#pragma once

#include <Node.h>

namespace nanobdd {

extern NodeTable* nodeTable;

class Bdd {
 public:
  Bdd(Node* root) : root_(root) {};
  ~Bdd();

  Node* root() const {
    return root_;
  }

  Bdd operator&(const Bdd& r) const {
    return nodeTable->bddAnd(root_, r.root());
  };
  Bdd operator|(const Bdd& r) const;
  Bdd operator!() const;

 private:
  Node* root_;
};

} // namespace nanobdd