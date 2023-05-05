#pragma once

#include <nanobdd/Node.h>

namespace nanobdd {

class Bdd {
 public:
  Bdd();
  Bdd(Node* root);
  ~Bdd();

  Bdd& operator=(Bdd r);

  Node* root() const;

  bool isFalse() const;

  bool isTrue() const;

  Bdd operator&(const Bdd& r) const;

  Bdd& operator&=(const Bdd& r);

  Bdd operator|(const Bdd& r) const;

  Bdd& operator|=(const Bdd& r);

  Bdd operator!() const;

  Bdd operator-(const Bdd& r) const;

  Bdd& operator-=(const Bdd& r);

  bool operator==(const Bdd& r) const;

  bool operator!=(const Bdd& r) const;

  bool operator<(const Bdd& r) const;

 protected:
  Node* root_;
};

} // namespace nanobdd