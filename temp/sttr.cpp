// sttr.cpp
//

#include "sttr.h"
// Converts a type to a unique address
namespace sttr {
template<typename T>
static char * getTypeSignature() {
	static char R;
	return &R;
	}
}
	
#define LZZ_INLINE inline
namespace sttr {
  std::string getTypeName_filterTypeOut (std::string const & in) {
	// Find "with T = " and stop when reaching ";"
	size_t start = in.find("T = ");
	if (start == std::string::npos) return in;
	std::string r = in.substr(start+4);
	size_t end = r.find(";");
	return r.substr(0,end);
	}
}
namespace sttr {
  void * Visitor_Base::getSignature () {
	return getSignatureStatic();
	}
}
namespace sttr {
  void * Visitor_Base::getSignatureStatic () {
	return (void*) sttr::getTypeSignature<Visitor_Base>();
	}
}
namespace sttr {
  void * Typename_Extractor_Visitor::getSignature () {
	return getSignatureStatic();
	}
}
namespace sttr {
  void * Typename_Extractor_Visitor::getSignatureStatic () {
	return (void*) sttr::getTypeSignature<Typename_Extractor_Visitor>();
	}
}
namespace sttr {
  RegBase::RegBase (char const * _name)
    : name (_name), isStatic (false), isConst (false), isVariable (false), isFunction (false), userFlags (0), userString (""), userData (NULL) {}
}
namespace sttr {
  void RegBase::visit (Visitor_Base * V) {}
}
namespace sttr {
  unsigned char const * RegBase::getAddr () { return 0; }
}
namespace sttr {
  RegNamespace::RegNamespace (char const * _name)
    : parent (NULL), name (_name), thisClass (NULL) {}
}
namespace sttr {
  RegNamespace::~ RegNamespace () {
	for (RegBase * RB : members) delete RB;
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
  RegNamespace & RegNamespace::findClass (char const * class_name) {
	RegNamespace * R = findClassWorker(class_name);
	assert(R && "sttr::RegNamespace::findClass : class not found");
	return *R;
	}
}
namespace sttr {
  RegNamespace * RegNamespace::findClassWorker (char const * class_name) {
	for (RegNamespace & R : classes) {
		if (R.thisClass) {
		if (!strcmp(R.thisClass->name, class_name))
			return &R;
		}	
		}
	for (RegNamespace & R : classes) {
		RegNamespace * R2 = R.findClassWorker(class_name);
		if (R2)
		return R2;
		}
	return NULL;
	}
}
namespace sttr {
  std::string RegNamespace::toString (int const indent) {
	std::string r = "";
	Typename_Extractor_Visitor RTEV;
	
	for (RegBase * RB : members) {
		RB->visit(&RTEV);
		r += std::string(indent,'\t') + "\tField: "+ RB->name + "\tTypedef: " + RTEV.type_name_out + " isStatic: " + STTR_BTOS(RB->isStatic) + " , isConst: " + STTR_BTOS(RB->isConst) + ", isFunction: " + STTR_BTOS(RB->isFunction) + ", isVariable: " + STTR_BTOS(RB->isVariable) + "\n";
		}
	for (RegNamespace & RS : classes) {
		r += std::string(indent,'\t') + "class " + RS.name + ":\n";
		r += RS.toString(indent+1);
		}
	return r;
	}
}
#undef LZZ_INLINE
// sttr.tpl
//

#include "sttr.h"
namespace sttr {
  template <typename T>
  std::string getTypeName () {
	return sttr::getTypeName_filterTypeOut(__PRETTY_FUNCTION__);
	}
}
namespace sttr {
  template <typename T>
  void Visitor_Base::visit (sttr::Reg <T> * RB) {
	//cout << "Visitor_Base" << endl;
	}
}
namespace sttr {
  template <typename T>
  void Typename_Extractor_Visitor::visit (Reg <T> * RB) {
	type_name_out = sttr::getTypeName<decltype(RB->func)>();
	}
}
namespace sttr {
  template <typename T>
  Reg <T>::Reg (T v, char const * _name)
    : func (v), RegBase (_name) {}
}
namespace sttr {
  template <typename T>
  void Reg <T>::visit (Visitor_Base * v) {
	// Upcast and get the right visit
	STTR_ADD_VISITOR(Typename_Extractor_Visitor)
	
	#ifdef STTR_VISITORS
		STTR_VISITORS
	#else
		#warning STTR_VISITORS macro not defined! To suppress this warning define it as an empty string "#define STTR_VISITORS". To add a visitor use #define STTR_VISITORS STTR_ADD_VISITOR(visitoclass_type) STTR_ADD_VISITOR(anothevisitoclass_type) ... etc.
	#endif
	v->visit(this);
	}
}
namespace sttr {
  template <typename T>
  unsigned char const * Reg <T>::getAddr () {
	unsigned char const * first  = reinterpret_cast<unsigned char *>(&func);
	return reinterpret_cast<unsigned char *>(*first);
	}
}
namespace sttr {
  template <typename T>
  RegNamespace & RegNamespace::regField (T v, char const * _name) {
	RegBase * R = new Reg<T>(v,_name);
	R->isFunction = std::is_function<typename std::remove_pointer<T>::type>::value;
	R->isVariable = !R->isFunction;
	R->isConst = std::is_const<typename std::remove_pointer<T>::type>::value;
	R->isStatic = parent && !std::is_member_pointer<T>::value;
	members.push_back(R);
	return *this;
	}
}
namespace sttr {
  template <typename T>
  RegNamespace & RegNamespace::beginClass (char const * _name) {
	RegNamespace R (_name);
	R.parent = this;
	R.thisClass = new Reg<T*>(NULL, _name);
	classes.push_back(R);
	return classes[classes.size()-1];
	}
}
