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

#define LZZ_INLINE inline
namespace sttr {
  std::string getTypeName_filterTypeOut (std::string const & in);
}
namespace sttr {
  template <typename T>
  std::string getTypeName ();
}
namespace sttr {
  class RegBase {
  public:
    char const * name;
    bool isStatic;
    bool isConst;
    bool isVariable;
    bool isFunction;
    uint32_t userFlags;
    std::string userString;
    void * userData;
    RegBase (char const * _name);
    virtual ~ RegBase ();
    virtual void visit (Visitor_Base * V);
    virtual std::string getTypeName ();
    virtual unsigned char const * getAddr ();
  };
}
namespace sttr {
  template <typename T>
  class Reg : public RegBase {
  public:
    T func;
    Reg (T v, char const * _name);
    virtual ~ Reg ();
    void visit (Visitor_Base * v);
    std::string getTypeName ();
    unsigned char const * getAddr ();
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
    template <typename T>
    RegNamespace & regField (T v, char const * _name);
    RegNamespace & setUserFlags (uint32_t const & userFlags);
    RegNamespace & setUserString (std::string const & userString);
    RegNamespace & setUserData (void * userData);
    template <typename T>
    RegNamespace & beginClass (char const * _name);
    RegNamespace & endClass ();
    RegNamespace & findClass (char const * class_name);
    RegNamespace * findClassPointer (char const * class_name);
    void visit (Visitor_Base * v);
    std::string toString (int const indent = 0);
  };
}
namespace sttr {
  template <typename T>
  std::string getTypeName () {
	return sttr::getTypeName_filterTypeOut(__PRETTY_FUNCTION__);
	}
}
namespace sttr {
  template <typename T>
  Reg <T>::Reg (T v, char const * _name)
    : RegBase (_name), func (v) {}
}
namespace sttr {
  template <typename T>
  Reg <T>::~ Reg () {}
}
namespace sttr {
  template <typename T>
  void Reg <T>::visit (Visitor_Base * v) {
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
  template <typename T>
  std::string Reg <T>::getTypeName () { return sttr::getTypeName<T>(); }
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
	RegNamespace * R = new RegNamespace(_name);
	R->parent = this;
	R->thisClass = new Reg<T*>(NULL, _name);
	classes.push_back(R);
	return *(classes[classes.size()-1]);
	}
}
#undef LZZ_INLINE
#endif
