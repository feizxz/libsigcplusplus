// -*- c++ -*-
/* Copyright 2002, The libsigc++ Development Team
 *  Assigned to public domain.  Use as you wish without restriction.
 */

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/slot.h>
#include <iostream>

struct foo : public sigc::functor_base
{
#ifdef SIGC_CXX_TYPEOF
  // if the compiler supports typeof(), result_type must only match the
  // return type of the operator() overload with 1 argument (cannot be auto-detected in C++).
  typedef bool result_type;
  bool operator()(int i)
    {std::cout << "foo(int "<<i<<")" << std::endl; return (i>0);}
  int operator()(int i,int j)
    {std::cout << "foo(int "<<i<<",int "<<j<<")" << std::endl; return i+j;}
  void operator()(int i,int j,int k)
    {std::cout << "foo(int "<<i<<",int "<<j<<", int "<<k<<")" << std::endl;}
#else
  // choose a type that can hold all return values
  typedef int result_type;
  int operator()(int i)
    {std::cout << "foo(int "<<i<<")" << std::endl; return (i>0);}
  int operator()(int i,int j)
    {std::cout << "foo(int "<<i<<",int "<<j<<")" << std::endl; return i+j;}
  int operator()(int i,int j,int k)
    {std::cout << "foo(int "<<i<<",int "<<j<<", int "<<k<<")" << std::endl; return 0;}
#endif
};

struct foo_void : public sigc::functor_base
{
  typedef void result_type;
  void operator()(int i)
    {std::cout << "foo_void(int "<<i<<")" << std::endl;}
};


int bar(int i,int j) 
  {std::cout << "bar(int "<<i<<",int "<<j<<")" << std::endl; return i+j;}

bool simple(bool test)
  {std::cout << "simple(bool "<<test<<")" << std::endl; return test;}

void egon(std::string& str)
  {std::cout << "egon(string '"<<str<<"')" << std::endl; str="egon was here";}


struct book : public sigc::trackable
{
  book(const std::string& name) : name_(name) {}
  operator std::string& () {return name_;}
  std::string name_;
};


int main()
{
  // replacing bind1st, bind2nd
  std::cout << sigc::bind<1>(foo(),-12345)(5) << std::endl;
  std::cout << sigc::bind<2>(foo(),-12345)(5) << std::endl;

  // multiple
  sigc::bind<1>(foo(),1,2)();

  // bind from end
  sigc::bind<0>(foo(),4)(3);

  // multiple beginning from end
  sigc::bind<0>(foo(),5,6)();

  // used together
  sigc::bind<1>(sigc::bind<1>(foo(),7),8)();

  // void return
  sigc::bind<1>(foo(),9,10)(11); // (only returns void if typeof() is supported)
  sigc::bind<0>(foo_void(),12)();

  // function pointer instead of functor
  sigc::bind<1>(&bar,13,14)();

  // test return type of bind_functor::operator() overload with no arguments
  std::cout << sigc::bind<0>(foo(),15)() << std::endl;
  std::cout << sigc::bind<0>(&simple, true)() << std::endl;

  // test references
  std::string str("guest book");
  sigc::bind<0>(&egon,sigc::ref(str))(); // Tell bind that is shall store a reference.
  std::cout << str << std::endl;     // (This cannot be the default behaviour: just think about what happens if str dies!)

  sigc::slot<void> sl;
  {
    book guest_book("karl");
    sl = sigc::bind<0>(&egon, sigc::ref(guest_book));
    sl();
    std::cout << (std::string&)guest_book << std::endl;
  }    // auto-disconnect
  sl(); // :-)

  const bool b1 = sigc::is_base_and_derived<sigc::trackable, void (*)(std::string&)>::value;
}
