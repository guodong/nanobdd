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
  Bdd operator|(const Bdd& r) const {
    return nodeTable->bddOr(root_, r.root());
  };
  Bdd operator!() const {
    return nodeTable->bddNot(root_);
  };
  Bdd operator-(const Bdd& r) const {
    return nodeTable->bddDiff(root_, r.root());
  }

 private:
  Node* root_;
};

} // namespace nanobdd