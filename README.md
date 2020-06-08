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
* Only ~400 lines of code
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
	.deriveClass<C,D>("D") // Class inherritance. driveClass("D") can safely be called before or after C is registered.
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

### Macros
See `sttr_compat_macros.hpp`. These can be used to add class type information to classes without RTTI.
* `STTR_CLASS_SIG(X)`: Defines two functions - sttr_getClassSig() and sttr_getClassName()
* `STTR_AUTO_UPCAST(BASE, DERIVED)`: Defines two functions - upcast() and upcastC(). These perform a dynamic cast without RTTI.
* `STTR_CLASS_SIG_NON_POLY(X)`: `STTR_CLASS_SIG` without `virtual`. Removes compiler warnings for non-polymorphic classes

### Free Functions
* `template <typename T> std::string getTypeName()` - 
* `template <typename T> const char * getTypeName()` - 
Converts a type to a string. If you are compiling with c++17 or later the const char* version will be used which is resolved at compile time
* `template <typename T, R> bool isType(const R & r)` - Returns true if and only if R is of type T. Needs function R::sttr_getClassSig() defined.
* `template <typename T, R> bool isType(const R * const r)` - Returns true if and only if R is of type T. Needs function R::sttr_getClassSig() defined.

* `template <typename T> char * getTypeSignature()` - 
Converts a type to a unique pointer. Used to identify classes without RTTI
* `template <typename T> T* constructIfDefaultConstructible()` - Constructs an instance of T (with new) if T is default constructable, else returns NULL

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

* `RegNamespace * getGlobalNamespace()` - singleton instance of a namespace. Useful for acting as a global root for a tree. You do not have to use this, but it is convenient if you only need one global tree for everything

* `template <typename T> RegNamespace & regField(T v, const char * _name)` - Registers a field. This may be a function, a variable, either belonging to a class or free. If you are registering a static variable or functions you need to reigister the class type too with `regField<classType, fieldType>(&classType::field, name)`.

* `RegNamespace & setUserFlags (const uint32_t & userFlags)` - Sets field->userFlags for the last added field. Will trip an assert if no fields are present
* `RegNamespace & setUserString (const std::string & userString)` - Sets field->userString for the last added field. Will trip an assert if no fields are present
* `RegNamespace & setUserData (void * userData)` - Sets field->userData for the last added field. Does not take ownership of userData, userData will not be deleted on destruction
* `RegNamespace & setClassUserFlags (const uint32_t & userFlags)` - If this namespace is a class, sets flags for that class. Else trips an assert.
* `RegNamespace & setClassUserString (const std::string & userString)` - If this namespace is a class, sets the userString for that class. Else trips an assert.
* `RegNamespace & setClassUserData (void * userData)` - If this namespace is a class, sets the userdata for that class. Else trips an assert.
* `template<typename T> RegNamespace & beginClass(const char * _name)` - Starts a class. A class is represented internally as a RegNamespace, so this returns the new one created. 
* `template<typename BASE, typename DERIVED> RegNamespace & deriveClass(const char * _name)` - Starts a class that is derived from another. The BASE does not have to be registered when you call this, you may register it before or later. If the base is not registered then the derived class is added as a child to the root of the class tree and then placed as a child of the base class when that is registered. *WARNING*: Calling `endClass()` after using this may place you in a different position of the class tree than you expect! You should make a seperate `getNamespaceSomehow().beginClass()` call after `endClass()` if using this. 
* `RegNamespace & endClass()` - Ends the class you're working on
* `RegNamespace & findClass(const char * class_name)` - Searches for a class by name. Will trip an assert if target is not found.
* `RegNamespace * findClassPointer(const char * class_name)` - Searches for a class by name. Will NOT trip an assert if target is not found, returns a pointer
* `RegNamespace * findClassPointer(void * class_signature)` - Searches for a class by signature (use (void*) sttr::getTypeSignature<T>() to get one). Will NOT trip an assert if target is not found, returns a pointer
* `template<typename T> T* construct()` - If this is a class and this is trivially constructable, construct an instance of the class with `new T;`
* `void* construct_retVoidPtr()` - Same as above, but returns `new T` casted to a void pointer.
* `std::string toString()` - Recursively prints out the layout of the namespace tree
* `void visit(sttr::Visitor_Base * v)` - Visits all registered members and classes with v
* `void visitRecursive(sttr::Visitor_Base * v)` - Recursively visits all registered members and classes with v. This goes *down* the tree from the current leaf
* `void visitPolymorthic(sttr::Visitor_Base * v)` - Recursively visits all registered members with v. This goes *up* the tree from the current leaf until the base class is done being visited.

## Visitors
To access the raw pointer to a registered field you must use a visitor. Copy and paste the following code (change MyVisitor to something else)
```
#include "sttr_visitor.h"

class MyVisitor : public sttr::Visitor_Base {
public:	
	template<typename T, CT>
	void visit(sttr::Reg<T, CT> * RB) {
		// Your code here!
		// RB->func is the pointer you're after 
		//std::cout << "Visiting! " << sttr::getTypeName<T>() << std::endl;
		}

	void visitClass(sttr::Reg<T, CT> *RB) {
		// Called when a class (a RegNamespace) is called
		// T here is a Class*, use std::remove_pointer<T> to get the class type
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
	STTR_ADD_VISITOR(MyVisitor) \			// Use this to call visit() on members/methods of classes
	STTR_ADD_VISITOR(MyOtherVisitor)
	
#define STTR_CLASS_VISITORS \
	STTR_ADD_VISITOR(MyVisitor) 			// Use this to call visitClass() on classes themselves
	
#include "sttr.h"
```


## See Also:
* [RTTR](https://github.com/rttrorg/rttr)  - All bells and whistles
* [Ponder](https://github.com/billyquith/ponder) Ponder

## Liscense
Public Domain
