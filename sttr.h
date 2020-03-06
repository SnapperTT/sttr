// sttr.h
//

#ifndef LZZ_sttr_h
#define LZZ_sttr_h
// STTR - STT Reflection Library
// By Liam Twigger - 2020
// @SnapperTheTwig
// Public Domain

#include <type_traits>
#include <vector>
#include <cassert>
#include <cstring>
#include <string>

#ifndef STTR_VISITOR_HEADER
	#include "sttr_visitor.h"
#endif
	
#define STTR_BTOS(B) (B ? "true" : "false")

// STTR_VISITORS Must be defined before #include "sttr.h"
//#define STTR_VISITORS //	STTR_ADD_VISITOR(visitoclass_x) //	STTR_ADD_VISITOR(visitoclass_y) //	STTR_ADD_VISITOR(visitoclass_z) 

//#define STTR_VISITORS
// 	STTR_ADD_VISITOR(Typename_Extractor_Visitor);
namespace sttr {
	class RegNamespace;
	}

// Macro for registering field
#define STTR_REG(C,X) regField<C>(&C::X,#X)

#define LZZ_INLINE inline
namespace sttr {
  class RegBase {
  public:
    char const * name;
    bool isStatic;
    bool isConst;
    bool isVariable;
    bool isFunction;
    RegNamespace * mNamespace;
    uint32_t userFlags;
    std::string userString;
    void * userData;
    RegBase (char const * _name);
    virtual ~ RegBase ();
    virtual void visit (Visitor_Base * V);
    virtual std::string getTypeName ();
    virtual std::string getTypePointingTo ();
    virtual unsigned char const * getAddr () const;
    virtual long long int const getOffset () const;
  };
}
namespace sttr {
  template <typename T, typename CT>
  class Reg : public RegBase {
  public:
    T func;
    Reg (T v, char const * _name);
    virtual ~ Reg ();
    void visit (Visitor_Base * v);
    std::string getTypeName ();
    std::string getTypePointingTo ();
    unsigned char const * getAddr () const;
    long long int const getOffset () const;
  };
}
namespace sttr {
  class RegNamespace {
  public:
    RegNamespace * parent;
    char const * name;
    std::vector <RegBase*> members;
    std::vector <RegNamespace*> classes;
    RegBase * thisClass;
    RegNamespace (char const * _name);
    ~ RegNamespace ();
    template <typename CT = sttr::NullType, typename T = sttr::NullType>
    RegNamespace & regField (T v, char const * _name);
    template <typename CT, typename T>
    RegNamespace & regField (T CT::* v, char const * _name);
    RegNamespace & setUserFlags (uint32_t const & userFlags);
    RegNamespace & setUserString (std::string const & userString);
    RegNamespace & setUserData (void * userData);
    template <typename T>
    RegNamespace & beginClass (char const * _name);
    RegNamespace & endClass ();
    RegNamespace & findClass (char const * class_name);
    RegNamespace * findClassPointer (char const * class_name);
    void visitRecursive (Visitor_Base * v);
    void visit (Visitor_Base * v);
    std::string toString (int const indent = 0);
  };
}
namespace sttr {
  RegNamespace * getGlobalNamespace ();
}
namespace sttr {
  template <typename T, typename CT>
  Reg <T, CT>::Reg (T v, char const * _name)
    : RegBase (_name), func (v) {}
}
namespace sttr {
  template <typename T, typename CT>
  Reg <T, CT>::~ Reg () {}
}
namespace sttr {
  template <typename T, typename CT>
  void Reg <T, CT>::visit (Visitor_Base * v) {
	// Upcast and get the right visitor
	#ifdef STTR_VISITORS
		STTR_VISITORS
	#else
		#warning STTR_VISITORS macro not defined! To suppress this warning define it as an empty string "#define STTR_VISITORS". To add a visitor use #define STTR_VISITORS STTR_ADD_VISITOR(visitoclass_type) STTR_ADD_VISITOR(anothevisitoclass_type) ... etc.
	#endif
	v->visit(this);
	}
}
namespace sttr {
  template <typename T, typename CT>
  std::string Reg <T, CT>::getTypeName () { return sttr::getTypeName<T>(); }
}
namespace sttr {
  template <typename T, typename CT>
  std::string Reg <T, CT>::getTypePointingTo () { return sttr::getTypeName<decltype(sttr::getTypePointingTo(func))>(); }
}
namespace sttr {
  template <typename T, typename CT>
  unsigned char const * Reg <T, CT>::getAddr () const {
	unsigned char const * first  = reinterpret_cast<unsigned char const *>(&func);
	return reinterpret_cast<unsigned char *>(*first);
	}
}
namespace sttr {
  template <typename T, typename CT>
  long long int const Reg <T, CT>::getOffset () const {
	return reinterpret_cast<size_t>(getAddr());
	}
}
namespace sttr {
  template <typename CT, typename T>
  RegNamespace & RegNamespace::regField (T v, char const * _name) {
	// for registration of anything
	RegBase * R = new Reg<T, CT>(v,_name);
	R->isFunction = sttr::is_function<T>::value;
	R->isVariable = sttr::is_variable<T>::value;
	R->isConst = sttr::is_const<T>::value;
	R->isStatic = sttr::is_static<T>::value;
	R->mNamespace = this;
	members.push_back(R);
	return *this;
	}
}
namespace sttr {
  template <typename CT, typename T>
  RegNamespace & RegNamespace::regField (T CT::* v, char const * _name) {
	// easy reigstation of class members
	return regField<CT,T CT::*>(v, _name);
	}
}
namespace sttr {
  template <typename T>
  RegNamespace & RegNamespace::beginClass (char const * _name) {
	RegNamespace * R = new RegNamespace(_name);
	R->parent = this;
	R->thisClass = new Reg<T*, sttr::NullType>(NULL, _name);
	classes.push_back(R);
	return *(classes[classes.size()-1]);
	}
}
#undef LZZ_INLINE
#endif
