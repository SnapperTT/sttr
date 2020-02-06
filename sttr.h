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

#define STTR_ADD_VISITOR(mVisitorType) 					if (v->getSignature() == mVisitorType::getSignatureStatic()) { 		mVisitorType * vv = (mVisitorType *) v; 				vv->visit(this); 							return;								}
	
#define STTR_BTOS(B) (B ? "true" : "false")

// STTR_VISITORS Must be defined before #include "sttr.h"
//#define STTR_VISITORS //	STTR_ADD_VISITOR(visitoclass_x) //	STTR_ADD_VISITOR(visitoclass_y) //	STTR_ADD_VISITOR(visitoclass_z) 

//#define STTR_VISITORS
// 	STTR_ADD_VISITOR(Typename_Extractor_Visitor);

namespace sttr {
template<typename T>
class Reg;

// Converts a type to a unique address
template<typename T>
static char * getTypeSignature();
}
#define LZZ_INLINE inline
namespace sttr {
  std::string getTypeName_filterTypeOut (std::string const & in);
}
namespace sttr {
  template <typename T>
  std::string getTypeName ();
}
namespace sttr {
  class Visitor_Base {
  public:
    template <typename T>
    void visit (sttr::Reg <T> * RB);
    virtual void * getSignature ();
    static void * getSignatureStatic ();
  };
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
    std::string toString (int const indent = 0);
  };
}
#undef LZZ_INLINE
#endif
