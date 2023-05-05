#pragma once

#include <Node.h>

namespace nanobdd {

extern NodeTable* nodeTable;

class Bdd {
 public:
  Bdd() : root_(nodeTable->falseNode()) {}
  Bdd(Node* root) : root_(root) {};
  ~Bdd();

  Bdd& operator=(Bdd r) {
    root_ = r.root();
    return *this;
  }

  Node* root() const {
    return root_;
  }

  Bdd operator&(const Bdd& r) const {
    return nodeTable->bddAnd(root_, r.root());
  }
  Bdd operator|(const Bdd& r) const {
    return nodeTable->bddOr(root_, r.root());
  }
  Bdd operator!() const {
    return nodeTable->bddNot(root_);
  }
  Bdd operator-(const Bdd& r) const {
    return nodeTable->bddDiff(root_, r.root());
  }
  bool operator==(const Bdd& r) const {
    return root_ == r.root();
  };
  bool operator!=(const Bdd& r) const {
    return root_ != r.root();
  };
  bool operator<(const Bdd& r) const {
    return root_ < r.root();
  };

 private:
  Node* root_;
};

} // namespace nanobdd