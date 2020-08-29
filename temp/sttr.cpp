// sttr.cpp
//

#include "sttr.h"
#define LZZ_INLINE inline
namespace sttr {
  RegBase::RegBase (char const * _name)
    : name (_name), isStatic (false), isConst (false), isVariable (false), isFunction (false), nullValueSet (false), nullValueValue (0.0), mNamespace (NULL), userFlags (0), userString (""), userData (NULL) {}
}
namespace sttr {
  RegBase::~ RegBase () {}
}
namespace sttr {
  void * RegBase::construct () { return (void *) 0; }
}
namespace sttr {
  void RegBase::visit (Visitor_Base * V) {}
}
namespace sttr {
  void RegBase::visitClass (Visitor_Base * V) {}
}
namespace sttr {
  std::string RegBase::getTypeName () { return ""; }
}
namespace sttr {
  std::string RegBase::getTypePointingTo () { return ""; }
}
namespace sttr {
  unsigned long long int const RegBase::getOffset () const { return 0; }
}
namespace sttr {
  RegNamespace::RegNamespace (char const * _name)
    : parent (NULL), name (_name), thisClass (NULL), thisClassSig (NULL), uninstantiatedParent (NULL) {}
}
namespace sttr {
  RegNamespace::~ RegNamespace () {
	for (RegBase * RB : members) delete RB;
	for (RegNamespace * RN : classes) delete RN;
	if (thisClass) delete thisClass;
//	if (baseClassTuple) delete baseClassTuple;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setUserFlags (uint32_t const & userFlags) {
	// Sets the userFlags for the last inserted member
	assert (members.size() && "Trying to sttr::RegNamespace::setUserFlags without registering a field");
	RegBase * R = members[members.size()-1];
	R->userFlags = userFlags;
	return *this;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setUserString (std::string const & userString) {
	// Sets the userString for the last inserted member
	assert (members.size() && "Trying to sttr::RegNamespace::setUserString without registering a field");
	RegBase * R = members[members.size()-1];
	R->userString = userString;
	return *this;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setUserData (void * userData) {
	// Sets the userString for the last inserted member
	assert (members.size() && "Trying to sttr::RegNamespace::setUserData without registering a field");
	RegBase * R = members[members.size()-1];
	R->userData = userData;
	return *this;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setClassUserFlags (uint32_t const & userFlags) {
	// Sets the userFlags for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::setClassUserFlags with a namespace that is not a class");
	thisClass->userFlags = userFlags;
	return *this;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setClassUserString (std::string const & userString) {
	// Sets the userString for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::setClassUserString with a namespace that is not a class");
	thisClass->userString = userString;
	return *this;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::setClassUserData (void * userData) {
	// Sets the userString for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::setClassUserData with a namespace that is not a class");
	thisClass->userData = userData;
	return *this;
	}
}
namespace sttr {
  uint32_t RegNamespace::getClassUserFlags () const {
	// Sets the userFlags for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::getClassUserFlags with a namespace that is not a class");
	return thisClass->userFlags;
	}
}
namespace sttr {
  std::string RegNamespace::setClassUserString () const {
	// Sets the userString for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::getClassUserString with a namespace that is not a class");
	return thisClass->userString;
	}
}
namespace sttr {
  void * RegNamespace::getClassUserData () const {
	// Sets the userString for the last inserted member
	assert (thisClass && "Trying to sttr::RegNamespace::getClassUserData with a namespace that is not a class");
	return thisClass->userData;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::endClass () {
	if (parent) return *parent;
	return *this;
	}
}
namespace sttr {
  void * RegNamespace::construct_retVoidPtr () {
	if (thisClass)
		return thisClass->construct();
	return NULL;
	}
}
namespace sttr {
  RegNamespace & RegNamespace::findClass (char const * class_name) {
	RegNamespace * R = findClassPointer(class_name);
	assert(R && "sttr::RegNamespace::findClass : class not found");
	return *R;
	}
}
namespace sttr {
  RegNamespace * RegNamespace::findClassPointer (char const * class_name) {
	for (RegNamespace * R : classes) {
		if (R->thisClass) {
		//std::cout << "comp a: " << name << "->" << R->thisClass->name << " " << class_name << std::endl;
		if (!strcmp(R->thisClass->name, class_name))
			return R;
		}
		}
	for (RegNamespace * R : classes) {
		RegNamespace * R2 = R->findClassPointer(class_name);
		if (R2)
		return R2;
		}
	return NULL;
	}
}
namespace sttr {
  RegNamespace * RegNamespace::findClassPointerBySig (void * target) {
	if (target == thisClassSig) return this;
	for (RegNamespace * R : classes) {
		if (R->thisClass) {
		//std::cout << "comp b: " << name << "->" << R->name << " " << R->thisClassSig << " " << target<< std::endl;
		if (R->thisClassSig == target)
			return R;
		}
		}
	for (RegNamespace * R : classes) {
		RegNamespace * R2 = R->findClassPointerBySig(target);
		if (R2)
		return R2;
		}
	return NULL;
	}
}
namespace sttr {
  RegNamespace * RegNamespace::getBaseClass () {
	// Returns the base class of this class
	if (thisClass && parent) {
		if (parent->thisClass)
		return parent->getBaseClass();
		}
	return this;
	}
}
namespace sttr {
  bool RegNamespace::isDerivedFromSig (void * target) {
	// Returns true if this type is a instance of or derived from target
	if (thisClassSig == target) return true;
	if (parent) return parent->isDerivedFromSig(target);
	return false;
	}
}
namespace sttr {
  void RegNamespace::visitRecursive (Visitor_Base * v) {
	// Recusively visits all classes and members. Goes down the tree
	for (RegNamespace * R : classes) {
		R->visitRecursive(v);
		}
	visit(v);
	}
}
namespace sttr {
  void RegNamespace::visitPolymorthic (Visitor_Base * v) {
	// Recusively visits all classes and members, goes up the class tree
	// Classes are visited in order Base->Derived1->Derived2->...->This
	RegNamespace * R = this;
	if (!R) return;
	std::vector<RegNamespace*> chain;
	while (R->thisClass) {
		chain.push_back(R);
		R = R->parent;
		if (!R) break;
		}
		
	for (unsigned int i = chain.size()-1; i < chain.size(); --i) {
		chain[i]->visit(v);
		}
	}
}
namespace sttr {
  void RegNamespace::visit (Visitor_Base * v) {
	// Visits this class. Does not account for polymorthisim
	if (thisClass) thisClass->visitClass(v);
//	if (baseClassTuple) baseClassTuple->visitBaseClassTuple(v);
	for (RegBase * RB : members)
		RB->visit(v);
	}
}
namespace sttr {
  std::string RegNamespace::toString (int const indent) const {
	std::string r = "";
	for (RegBase * RB : members) {
		r += std::string(indent,'\t') + "\tField: "+ RB->name + "\tTypedef: " + RB->getTypeName() + ", Pointing To: " + RB->getTypePointingTo()+ ", isStatic: " + STTR_BTOS(RB->isStatic) + " , isConst: " + STTR_BTOS(RB->isConst) + ", isFunction: " + STTR_BTOS(RB->isFunction) + ", isVariable: " + STTR_BTOS(RB->isVariable) + "\n";
		}
	for (RegNamespace * RS : classes) {
		r += std::string(indent,'\t') + "class " + RS->name + ":\n";
		r += RS->toString(indent+1);
		}
	return r;
	}
}
namespace sttr {
  std::string RegNamespace::toClassTreeString (int const indent) const {
	std::string r = "";
	for (RegNamespace * RS : classes) {
		r += std::string(indent,'\t') + RS->name + "\n";
		r += RS->toClassTreeString(indent+1);
		}
	return r;
	}
}
namespace sttr {
  RegNamespace * getGlobalNamespace () {
	static RegNamespace R("global");
	return &R;
	}
}
#undef LZZ_INLINE
