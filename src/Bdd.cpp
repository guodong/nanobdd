#include <NodeTable.h>
#include <nanobdd/Bdd.h>

namespace nanobdd {

extern NodeTable* nodeTable;

Bdd::Bdd() : root_(nodeTable->falseNode()) {
  root_->ref();
}
Bdd::Bdd(Node* root) : root_(root){
  root_->ref();
};
Bdd::~Bdd(){
  root_->deref();
};

Bdd&
Bdd::operator=(Bdd r) {
  root_ = r.root();
  return *this;
}

Node*
Bdd::root() const {
  return root_;
}

bool
Bdd::isFalse() const {
  return root_ == nodeTable->falseNode();
}

bool
Bdd::isTrue() const {
  return root_ == nodeTable->trueNode();
}

Bdd
Bdd::operator&(const Bdd& r) const {
  return nodeTable->bddAnd(root_, r.root());
}

Bdd&
Bdd::operator&=(const Bdd& r) {
  root_ = nodeTable->bddAnd(root_, r.root());
  return *this;
}

Bdd
Bdd::operator|(const Bdd& r) const {
  return nodeTable->bddOr(root_, r.root());
}

Bdd&
Bdd::operator|=(const Bdd& r) {
  root_ = nodeTable->bddOr(root_, r.root());
  return *this;
}

Bdd
Bdd::operator!() const {
  return nodeTable->bddNot(root_);
}

Bdd
Bdd::operator^(const Bdd& r) const {
  return nodeTable->bddXor(root_, r.root());
}

Bdd&
Bdd::operator^=(const Bdd& r) {
  root_ = nodeTable->bddXor(root_, r.root());
  return *this;
}

Bdd
Bdd::operator-(const Bdd& r) const {
  return nodeTable->bddDiff(root_, r.root());
}

Bdd&
Bdd::operator-=(const Bdd& r) {
  root_ = nodeTable->bddDiff(root_, r.root());
  return *this;
}

Bdd
Bdd::operator>>(const Bdd& r) const {
  return nodeTable->bddImp(root_, r.root());
}

Bdd&
Bdd::operator>>=(const Bdd& r) {
  root_ = nodeTable->bddImp(root_, r.root());
  return *this;
}

bool
Bdd::operator==(const Bdd& r) const {
  return root_ == r.root();
}

bool
Bdd::operator!=(const Bdd& r) const {
  return root_ != r.root();
}

bool
Bdd::operator<(const Bdd& r) const {
  return root_ < r.root();
}

} // namespace nanobdd