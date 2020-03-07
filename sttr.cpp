// sttr_visitor.cpp
//

#include "sttr_visitor.h"
#define LZZ_INLINE inline
namespace sttr {
  void * Visitor_Base::getSignature () {
	// Must be present in derived classes
	return getSignatureStatic();
	}
}
namespace sttr {
  void * Visitor_Base::getSignatureStatic () {
	// Must be present in derived classes
	return (void*) sttr::getTypeSignature<Visitor_Base>();
	}
}
#undef LZZ_INLINE
// sttr.cpp
//

#include "sttr.h"
#define LZZ_INLINE inline
namespace sttr {
  RegBase::RegBase (char const * _name)
    : name (_name), isStatic (false), isConst (false), isVariable (false), isFunction (false), mNamespace (NULL), userFlags (0), userString (""), userData (NULL) {}
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
  std::string RegBase::getTypeName () { return ""; }
}
namespace sttr {
  std::string RegBase::getTypePointingTo () { return ""; }
}
namespace sttr {
  unsigned char const * RegBase::getAddr () const { return 0; }
}
namespace sttr {
  long long int const RegBase::getOffset () const { return 0; }
}
namespace sttr {
  RegNamespace::RegNamespace (char const * _name)
    : parent (NULL), name (_name), thisClass (NULL), thisClassSig (NULL) {}
}
namespace sttr {
  RegNamespace::~ RegNamespace () {
	for (RegBase * RB : members) delete RB;
	for (RegNamespace * RN : classes) delete RN;
	if (thisClass) delete thisClass;
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
  RegNamespace & RegNamespace::endClass () {
	if (parent) return *parent;
	return *this;
	}
}
namespace sttr {
  void * RegNamespace::construct_retVoidPtr () {
	if (thisClass) {
		return thisClass->construct();
		}
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
  void RegNamespace::visitRecursive (Visitor_Base * v) {
	// Recusively visits all classes and members
	for (RegNamespace * R : classes) {
		R->visitRecursive(v);
		}
	visit(v);
	}
}
namespace sttr {
  void RegNamespace::visit (Visitor_Base * v) {
	// Recusively visits members
	for (RegBase * RB : members) {
		RB->visit(v);
		}
	}
}
namespace sttr {
  std::string RegNamespace::toString (int const indent) {
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
  RegNamespace * getGlobalNamespace () {
	static RegNamespace R("global");
	return &R;
	}
}
#undef LZZ_INLINE
