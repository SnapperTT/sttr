STT Reflection
==============
A very simple and small reflection library. This *only* does reflection.

## Contact:
Liam Twigger - @SnapperTheTwig

## Motivation:
I wanted to expose class members to various utilities, such as Lua scripting, JSON and binary serialising. However writing bindings for each will result in a lot of work duplication. I wanted to write a class schema *once*, assign flags to members and be able to automatically create lua bindings and serialisation functions. Hence this.

## Features:
* No build system - just `#include "sttr.h"` to access, and `#include "sttr.cpp"` *once* in your program to build.
* Supports visitors to get underlying function & member pointers
* No macros required to register stuff. (You do need a simple macro to register)
* Supports variables, functions and classes
* Can register userData - every entry has a uint32_t, a std::string and a (void*) that you can use for whatever purpose
* No RTTI
* Human written documentation. Scroll down to see it
* Only ~300 lines of code
* Only dependencies are standard C and C++ libraries
* Public Domain

## Usage:
Including:
```
#include "sttr.h"
```

Building:
```
#define STTR_VISITORS
#include "sttr.cpp"
```
Do this in *one* module to add it to your program. If you want to define your own visitors (to access the underlying function pointers, etc) you'll have to register the visitor classes. `example.cpp` demonstrates registering using a visitor


## Example:
See `example.cpp`
```
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
	// Registration
	sttr::RegNamespace mNamespace("testns");
	mNamespace.beginClass<C>("C")
		.regField(&C::i, "i")
		.regField(&C::d, "d")
			.setUserFlags(0x00F00BAA) // Sets the user flags for the last added item
			.setUserString("You can use this for whatever")
			.setUserData((void *) &someStruct)
		.regField(&C::func, "func")
		
		.regField(&C::static_var, "static_var")
		.regField(&C::static_func, "static_func")
	.endClass()
	.findClass("C").beginClass<D>("D") // Class inherritance. Also you can just beginClass<D>("D") inside C
		.regField(&D::di, "di")
	.endClass();

	// Itterating over members
	sttr::RegNamespace * R = mNamespace.findClass("C");
	for (sttr::RegBase * RB : R->members) {		
		// Outputing a member's class, address
		std::cout << "Name: " << RB->name
				<< ", typeDef: " << RB->getTypeName()
				<< ", isStatic: " << STTR_BTOS(RB->isStatic)
				<< ", isConst: " << STTR_BTOS(RB->isConst)
				<< ", isFunction: " << STTR_BTOS(RB->isFunction)
				<< ", isVariable: " << STTR_BTOS(RB->isVariable) 
				<< ", isVariable: " << STTR_BTOS(RB->isVariable) 
				<< ", userFlags: " << RB->userFlags		// for custom purposes
				<< ", userString: " << RB->userString 	// for custom purposes
				<< ", userData: " << RB->userData 	// for custom purposes
				<< std::endl;
		}

	}

```


## Classes & Functions
Everything is in the sttr namespace

### Free Functions
* `template <typename T> std::string getTypeName()` - 
Converts a type to a string

* `template <typename T> char * getTypeSignature()` - 
Converts a type to a unique pointer. Used to identify classes without RTTI

### RegBase
* `class RegBase` - Base class used to stored registered fields (variables, functions, classes)
* `const char * name` - The name of a registered field. Set on creation
* `bool isStatic`	- Is this a static variable or a static function? (Auto detected and set on creation). Only works for class members and methods
* `bool isConst` - Is the field marked as const? (Auto)
* `bool isVariable` - Is this a variable (Auto)
* `bool isFunction` - Is this a function (Auto)
* `uint32_t userFlags` - You can use this for whatever you want. I use it for bit flags
* `std::string userString` - You can use this for whatever you want. I use it for documentation
* `void * userData` - You can use this for whatever you want

* `virtual void visit(RegVisitor * R)` - Used by Reg<T>

### Reg<T>
* `template<typename T> class Reg : public RegBase` - 
Class derived from RegBase, used to store pointers to registered fields (eg, if you register a function, a function pointer is stored here)
* `T func` - The pointer to whatever has been registered
* `void visit(Visitor_Base * v)` - Use a Visitor to access `T func` see `example.cpp` on how to do this

### RegNamespace
`class RegNamespace` - Tree-like container to store registered stuff. Also used to represent a registered class. Most of these methods return the object that you called it from so that you can chain calls (`mNamespace.addField().setUserFlags().addField(). ... etc`).

* `template <typename T> RegNamespace & regField(T v, const char * _name)` - Registers a field. This may be a function, a variable, either belonging to a class or free.

* `RegNamespace & setUserFlags (const uint32_t & userFlags)` - Sets field->userFlags for the last added field. Will trip an assert if no fields are present
* `RegNamespace & setUserString (const std::string & userString)` - Sets field->userString for the last added field. Will trip an assert if no fields are present
* `RegNamespace & setUserData (void * userData)` - Sets field->userData for the last added field. Does not take ownership of userData, userData will not be deleted on destruction
* `template<typename T> RegNamespace & beginClass(const char * _name)` - Starts a class. A class is represented internally as a RegNamespace, so this returns the new one created. 
* `RegNamespace & endClass()` - Ends the class you're working on
* `RegNamespace & findClass(const char * class_name)` - Searches for a class. Will trip an assert if target is not found.
* `RegNamespace * findClassPointer(const char * class_name)` - Searches for a class. Will NOT trip an assert if target is not found, returns a pointer
* `std::string toString()` - Recursively prints out the layout of the namespace tree
* `void visit(sttr::Visitor_Base * v)` - Visits all registered members and classes with v
* `void visitRecursive(sttr::Visitor_Base * v)` - Recursively visits all registered members and classes with v

## Visitors
To access the raw pointer to a registered field you must use a visitor. Copy and paste the following code (change MyVisitor to something else)
```
#include "sttr_visitor.h"

class MyVisitor : public sttr::Visitor_Base {
public:	
	template<typename T>
	void visit(sttr::Reg<T> * RB) {
		// Your code here!
		// RB->func is the pointer you're after 
		//std::cout << "Visiting! " << sttr::getTypeName<T>() << std::endl;
		}
	
	// These functions must be present for sttr to figure out which
	// visitor class to upcast too
	void * getSignature() {
		return getSignatureStatic();
		}
	static void * getSignatureStatic() {
		return (void*) sttr::getTypeSignature<MyVisitor>();
		}
	};
```

Finally you must define the `STTR_VISITORS` macro before `#include "sttr.h"`, otherwise sttr won't know your visitor exists!
```
#define STTR_VISITORS \
	STTR_ADD_VISITOR(MyVisitor) \
	STTR_ADD_VISITOR(MyOtherVisitor)
	
#include "sttr.h"
```

## See Also:
* [RTTR](https://github.com/rttrorg/rttr)  - All bells and whistles
* [Ponder](https://github.com/billyquith/ponder) Ponder

## Liscense
Public Domain
