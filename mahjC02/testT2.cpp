#include <stdlib.h>
#include <iostream>
#include <typeinfo> 
#include <stdio.h>
using namespace std;
#include "mahj3.h"
 class A { };

 int main( ){

 A a;

 A b;

 cout << typeid(CUnitsSet).name() << endl;
 cout << typeid(CUnits).name() << endl;
 cout << typeid(Unit).name() << endl;

 cout << typeid(b).name() << endl;

 if (typeid(a) == typeid(b)) cout << "a==b" << endl;

 else cout << "a!=b" << endl << endl;

 cout << endl;

 int i;

 cout << typeid(int).name() << endl;

 cout << typeid(i).name() << endl;

 if (typeid(int) == typeid(i)) cout << "typeid(int) = typeid(i)" << endl;

 else cout << "typeid(int) != typeid(i)"<< endl;

 cout << endl;

 }


