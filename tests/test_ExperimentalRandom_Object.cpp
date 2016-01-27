#include <boost/test/unit_test.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

/*TODO ALL*/
// using namespace std;
using boost::format;
using namespace crave;

CRAVE_BETTER_ENUM(color_enum, RED,GREEN,BLUE);
CRAVE_BETTER_ENUM(football_enum,
  GK,   // Goalkeeper
  SW,   // Sweeper
  LWB,  // Left-Wing-Back
  LB,   // Left-Back
  LCB,  // Left-Centre-Back
  RCB,  // Right-Centre-Back
  RB,   // Right-Back
  RWB,  // Right-Wing-Back
  DM,   // Defensive Midfielder
  LM,   // Left Wide Midfielder
  CM,   // Centre Midfielder
  RM,   // Right Wide Midfielder
  AM,   // Attacking Midfielder
  LW,   // Left Winger (Striker)
  SS,   // Secondary Striker
  RW,   // Right Winger
  CF    // Centre Striker
);

BOOST_FIXTURE_TEST_SUITE(Random_Object_t, Context_Fixture)

class my_rand_obj : public crv_sequence_item {
 public:
  crv_constraint constraint{"constraint"};
  crv_variable<color_enum> color;
  crv_variable<int> x;

  my_rand_obj(crv_object_name){
      constraint={color() == x()};}
};

BOOST_AUTO_TEST_CASE(t_rand_enum) {
  my_rand_obj obj("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(obj.randomize());
    std::cout << obj.color << " " << obj.x << std::endl;
    BOOST_REQUIRE(obj.color == color_enum::RED || obj.color == color_enum::GREEN || obj.color == color_enum::BLUE);
    BOOST_REQUIRE(obj.color == obj.x);
  }
}
/*
 * TODO
BOOST_AUTO_TEST_CASE(t_rand_enum_standalone) {
  crv_variable<color_enum>* color;
  BOOST_CHECK_THROW(color = new crv_variable<color_enum>(), std::runtime_error);
}*/

class tall_rand_enum_obj : public crv_sequence_item {
 public:
  crv_constraint constraint{"constraint"};
  crv_variable<football_enum> player;

  tall_rand_enum_obj(crv_object_name) {
    constraint={player() == football_enum::GK && player() != football_enum::CF};
  }
};

class tall_rand_enum_obj_gt : public crv_sequence_item {
 public:
  crv_variable<football_enum> player;
  crv_constraint constraint{"constraint"};
  tall_rand_enum_obj_gt(crv_object_name) { constraint={player() > football_enum::AM};}
};

BOOST_AUTO_TEST_CASE(enum_no_overflow) {
  tall_rand_enum_obj obj("obj");
  BOOST_REQUIRE(obj.randomize());

  BOOST_REQUIRE_EQUAL(obj.player, football_enum::GK);
}

BOOST_AUTO_TEST_CASE(enum_gt) {
  tall_rand_enum_obj_gt obj("obj");

  for (int i = 0; i < 100; ++i) {
    BOOST_REQUIRE(obj.randomize());
    BOOST_REQUIRE_GT(obj.player, football_enum::AM);
    BOOST_REQUIRE(obj.player == football_enum::LW || obj.player == football_enum::SS || obj.player == football_enum::RW || obj.player == football_enum::CF);
  }
}

class item : public crv_sequence_item {
 public:
  item(crv_object_name){ constraint={a() + b() == c()}; }

 public:
  crv_constraint constraint{"constraint"};
  crv_variable<int> a;
  crv_variable<int> b;
  crv_variable<int> c;
};

class item1 : public item {
 public:
  crv_constraint constraint{"constraint"};
  item1(crv_object_name name) : item(name){
    constraint={10 <= a() && a() <= 20,a() + b() + c() <= 200};
  }
};

class item2 : public item1 {
 public:
     crv_constraint constraint{"constraint"};
  item2(crv_object_name name) : item1(name){ constraint={a() + b() + c() == 100}; }
  crv_variable<int> d;
};

BOOST_AUTO_TEST_CASE(t2) {
  item it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
}

BOOST_AUTO_TEST_CASE(t3) {
  item1 it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
  BOOST_REQUIRE(it.a + it.b + it.c <= 200);
}

BOOST_AUTO_TEST_CASE(t4) {
  item2 it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
  BOOST_REQUIRE(it.a + it.b + it.c == 100);
}

class obj : public crv_sequence_item {
 public:
  obj(crv_object_name){
    constraint={dist(a(), distribution<int>::simple_range(-20, -10)),
    dist(b(), distribution<unsigned int>::simple_range(10, 20)),
    dist(c(), distribution<short>::simple_range(-20, -10)),
    dist(d(), distribution<unsigned short>::simple_range(10, 20))};
    e_con={dist(e(), distribution<char>::simple_range('a', 'z'))};
    e_con={dist(f(), distribution<unsigned char>::simple_range('A', 'Z'))};
  }
  crv_constraint constraint{"constraint"};
  crv_constraint e_con{"e"};
  crv_constraint f_con{"f"};
  crv_variable<int> a;
  crv_variable<unsigned int> b;
  crv_variable<short> c;
  crv_variable<unsigned short> d;
  crv_variable<char> e;
  crv_variable<unsigned char> f;

  friend ostream& operator<<(ostream& os, const obj& o) {
    os << "(" << o.a << " " << o.b << " " << o.c << " " << o.d << " " << o.e << " " << o.f << ")";
    return os;
  }
};

class obj1 : public obj {
 public:
  crv_constraint constraint{"constraint"};
  obj1(crv_object_name name) : obj(name){
    e_con.deactivate();
    f_con.deactivate();
    constraint={dist(e(), distribution<char>::simple_range('A', 'Z'))};
    constraint={dist(f(), distribution<unsigned char>::simple_range('a', 'z'))};
  }
};

class obj2 : public obj1 {
 public:
   crv_constraint constraint{"constraint"};
   obj2(crv_object_name name) : l("l") , obj1(name) {
   constraint={dist(g(), distribution<long>::simple_range(-20, -10)),
   dist(h(), distribution<unsigned long>::simple_range(10, 20)),
   dist(i(), distribution<long long>::simple_range(-20, -10)),
   dist(j(), distribution<unsigned long long>::simple_range(10, 20))};
  }
  crv_variable<long> g;
  crv_variable<unsigned long> h;
  crv_variable<long long> i;
  crv_variable<unsigned long long> j;
  crv_variable<bool> k;
  obj1 l;

  friend ostream& operator<<(ostream& os, const obj2& o1) {
    os << o1.l << " " << o1.g << " " << o1.h << " " << o1.i << " " << o1.j << " " << o1.k;
    return os;
  }
};

BOOST_AUTO_TEST_CASE(t5) {
  obj it("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it.randomize());
    BOOST_REQUIRE(-20 <= it.a && it.a <= -10);
    BOOST_REQUIRE(10 <= it.b && it.b <= 20);
    BOOST_REQUIRE(-20 <= it.c && it.c <= -10);
    BOOST_REQUIRE(10 <= it.d && it.d <= 20);
    BOOST_REQUIRE('a' <= it.e && it.e <= 'z');
    BOOST_REQUIRE('A' <= it.f && it.f <= 'Z');
  }

  obj1 it1("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it1.randomize());
    BOOST_REQUIRE(-20 <= it1.a && it1.a <= -10);
    BOOST_REQUIRE(10 <= it1.b && it1.b <= 20);
    BOOST_REQUIRE(-20 <= it1.c && it1.c <= -10);
    BOOST_REQUIRE(10 <= it1.d && it1.d <= 20);
    BOOST_REQUIRE('a' <= it1.f && it1.f <= 'z');
    BOOST_REQUIRE('A' <= it1.e && it1.e <= 'Z');
  }
  
  obj2 it2("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it2.randomize());
    BOOST_REQUIRE(-20 <= it2.g && it2.g <= -10);
    BOOST_REQUIRE(10 <= it2.h && it2.h <= 20);
    BOOST_REQUIRE(-20 <= it2.i && it2.i <= -10);
    BOOST_REQUIRE(10 <= it2.j && it2.j <= 20);
    BOOST_REQUIRE(-20 <= it2.l.a && it2.l.a <= -10);
    BOOST_REQUIRE(10 <= it2.l.b && it2.l.b <= 20);
    BOOST_REQUIRE(-20 <= it2.l.c && it2.l.c <= -10);
    BOOST_REQUIRE(10 <= it2.l.d && it2.l.d <= 20);
    BOOST_REQUIRE('a' <= it2.l.f && it2.l.f <= 'z');
    BOOST_REQUIRE('A' <= it2.l.e && it2.l.e <= 'Z');
  }
}

struct Item1 : public crv_sequence_item {
  
  
  Item1(crv_object_name) : pivot(0) {
    c1={x() * x() >= 24};
    c2={x() <= reference(pivot)};
  }

  bool next() {
    int lower = 0;
    int upper = 100;
    while (lower < upper) {
      std::cout << lower << " " << upper << std::endl;
      pivot = (upper + lower) / 2;
      if (this->randomize())
        upper = x;
      else
        lower = pivot + 1;
    }
    x = upper;
    return true;
  }
  crv_constraint c1{"c1"};
  crv_constraint c2{"c2"};
  crv_variable<unsigned> x;
  int pivot;
};

BOOST_AUTO_TEST_CASE(binary_search_test) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Item1 it("it");
  it.next();
  BOOST_REQUIRE_EQUAL(it.x, 5);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}
/*
 * TODO
struct Item2 : public crv_sequence_item {
  Item2() : i() {
    constraint={
    address() % 4 == 0,
    address() <= 1000u,
    data().size() == 4,
    foreach(data(), -50 <= data()[i] && data()[i] <= 50),
    foreach(data(), data()[i - 1] <= data()[i]),
  }

  placeholder i;
  crv_variable<unsigned> address;
  crv_vector<int> data;
  crv_constraint constraint{"constraint"};
};

BOOST_AUTO_TEST_CASE(item_with_vector) {
  Item2 it;
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it.randomize());
    BOOST_REQUIRE_EQUAL(it.data.size(), 4);
    std::cout << "@" << it.address << ": " << it.data[0] << " " << it.data[1] << " " << it.data[2] << " " << it.data[3]
              << std::endl;
    BOOST_REQUIRE_EQUAL(it.address % 4, 0);
    BOOST_REQUIRE_LE(it.address, 1000);
    for (uint i = 0; i < it.data.size(); i++) {
      BOOST_REQUIRE_LE(-50, it.data[i]);
      BOOST_REQUIRE_LE(it.data[i], 50);
      if (i > 0) BOOST_REQUIRE_LE(it.data[i - 1], it.data[i]);
    }
  }
}*/
//TODO
class Constraint_base : public Generator {
 public:
  Constraint_base() : Generator(), constraint(*this) {}

 protected:
  Generator& constraint;
};
//TODO
class Constraint1 : public Constraint_base {
 public:
  Variable<unsigned> x;

  Constraint1() : Constraint_base() { constraint(x < 10); }
};
//TODO
class Constraint2 : public Constraint1 {
 public:
  Constraint2() : Constraint1() { constraint(x > 6); }
};
//TODO
BOOST_AUTO_TEST_CASE(t1) {
  Constraint2 c2;

  c2();
  unsigned r = c2[c2.x];
  BOOST_REQUIRE_LT(r, 10);
  BOOST_REQUIRE_GT(r, 6);

  c2(c2.x != r)();
  r = c2[c2.x];
  BOOST_REQUIRE_LT(r, 10);
  BOOST_REQUIRE_GT(r, 6);

  c2(c2.x != r)();
  r = c2[c2.x];
  BOOST_REQUIRE_LT(r, 10);
  BOOST_REQUIRE_GT(r, 6);

  c2(c2.x != r);
  BOOST_REQUIRE(!c2.next());
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
