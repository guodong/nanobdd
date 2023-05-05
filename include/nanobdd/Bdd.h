#pragma once

#include <nanobdd/Node.h>
#include <nanobdd/NodeTable.h>

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

  bool isFalse() const {
    return root_ == nodeTable->falseNode();
  }

  bool isTrue() const {
    return root_ == nodeTable->trueNode();
  }

  Bdd operator&(const Bdd& r) const {
    return nodeTable->bddAnd(root_, r.root());
  }

  Bdd&
  operator&=(const Bdd& r) {
    root_ = nodeTable->bddAnd(root_, r.root());
    return *this;
  }

  Bdd operator|(const Bdd& r) const {
    return nodeTable->bddOr(root_, r.root());
  }

  Bdd&
  operator|=(const Bdd& r) {
    root_ = nodeTable->bddOr(root_, r.root());
    return *this;
  }

  Bdd operator!() const {
    return nodeTable->bddNot(root_);
  }

  Bdd operator-(const Bdd& r) const {
    return nodeTable->bddDiff(root_, r.root());
  }

  Bdd&
  operator-=(const Bdd& r) {
    root_ = nodeTable->bddDiff(root_, r.root());
    return *this;
  }

  bool operator==(const Bdd& r) const {
    return root_ == r.root();
  }

  bool operator!=(const Bdd& r) const {
    return root_ != r.root();
  }

  bool operator<(const Bdd& r) const {
    return root_ < r.root();
  }

 protected:
  Node* root_;
};

} // namespace nanobdd