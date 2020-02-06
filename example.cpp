// Includes
#include <iostream>
#include "sttr.h"


// Example visitor - register
// Copy and paste this into your own application
// and fill void visit(sttr::Reg<T> * RB) with youe own stuff
class MyVisitor : public sttr::Visitor_Base {
public:
	template<typename T>
	void visit(sttr::Reg<T> * RB) {
		//std::cout << "Visiting! " << sttr::getTypeName<T>() << std::endl;
		}
	
	void * getSignature() {
		return getSignatureStatic();
		}
	static void * getSignatureStatic() {
		return (void*) sttr::getTypeSignature<MyVisitor>();
		}
	};

////////////////////////////////////////////////////////////////////////
// Compiling sttr:
// Simply include sttr.cpp somewhere!

#define STTR_VISITORS \
	STTR_ADD_VISITOR(::MyVisitor)
	
// STTR_VISITORS must be defined before including sttr.cpp
// Any classes using STTR_VISITORS

#include "sttr.cpp"

////////////////////////////////////////////////////////////////////////
// Example classes

class C {
public:
	int i;
	double d;
	int func(int z) const {
		return z;
		}
	
	static int static_var;
	static int static_func() { return 5; }
	};
int C::static_var = 7;
	
struct D : public C {
	int di;
	};
	


int main(int argc, char ** argv) {
	
	sttr::RegNamespace mNamespace("testns");
	mNamespace.beginClass<C>("C")
		.regField(&C::i, "i")
		.regField(&C::d, "d")
		.regField(&C::func, "func")
		
		.regField(&C::static_var, "static_var")
		.regField(&C::static_func, "static_func")
	.endClass()
	.findClass("C").beginClass<D>("D")
		.regField(&D::di, "di")
	.endClass();
		
	// Converting a type to a string
	std::cout << "Decltype: " << sttr::getTypeName<decltype(&C::func)>() << std::endl;
	
	MyVisitor V;
	
	// Itterating over a registration
	for (sttr::RegBase * RB : mNamespace.classes[0].members) {
		std::cout << "RB: " << RB->name << ", addr: " << ((void *)RB->getAddr()) << std::endl;
		RB->visit(&V);
		}
	
	// Dump function - good for debugging
	std::cout << std::endl << "DUMP: " << std::endl << mNamespace.toString() << std::endl;
	
	return 1;
	}

//
//	OUTPUT:
//	Decltype: int (C::*)(int) const
//	RB: i, addr: 0
//	RB: d, addr: 0x8
//	RB: func, addr: 0x2
//	RB: static_var, addr: 0x28
//	RB: static_func, addr: 0x12
//
//	DUMP:
//	class C:
//			Field: i	Typedef: int C::* isStatic: false , isConst: false, isFunction: false, isVariable: true
//			Field: d	Typedef: double C::* isStatic: false , isConst: false, isFunction: false, isVariable: true
//			Field: func	Typedef: int (C::*)(int) const isStatic: false , isConst: false, isFunction: false, isVariable: true
//			Field: static_var	Typedef: int* isStatic: true , isConst: false, isFunction: false, isVariable: true
//			Field: static_func	Typedef: int (*)() isStatic: true , isConst: false, isFunction: true, isVariable: false
//		class D:
//				Field: di	Typedef: int D::* isStatic: false , isConst: false, isFunction: false, isVariable: true
//
