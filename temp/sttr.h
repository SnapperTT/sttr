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
#include <cstdint>
#include <string>

#ifndef STTR_VISITOR_HEADER
	#include "sttr_visitor.h"
#endif
	
#define STTR_BTOS(B) (B ? "true" : "false")
#ifndef STTR_CLASS_SIG
	#define STTR_CLASS_SIG(X) virtual void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } 				virtual const char * const sttr_getClassName() const { return sttr::getTypeName<X>(); }
#endif
// STTR_VISITORS Must be defined before #include "sttr.h"
//#define STTR_VISITORS //	STTR_ADD_VISITOR(visitoclass_x) //	STTR_ADD_VISITOR(visitoclass_y) //	STTR_ADD_VISITOR(visitoclass_z) 

//#define STTR_VISITORS
// 	STTR_ADD_VISITOR(Typename_Extractor_Visitor);
namespace sttr {
	class RegNamespace;
	}

// Macro for registering field
#define STTR_REG(C,X) regField<C>(&C::X,#X)
#define STTR_REGF(C,X,F) regField<C,decltype(&C::X),F>(&C::X,#X).setUserFlags(F)

// Workaround for lazy-c++
#define STTR_VARADIC_TEMPLATE_ARGS1 ...ARGS
#define STTR_VARADIC_TEMPLATE_ARGS2 ARGS...

// SFINAE test for function existance
#define STTR_HAS_FUNC(func)						template <typename T>							class has_##func {							    typedef char one;							    struct two { char x[2]; };						    template <typename C> static one test( decltype(&C::func) ) ;	    template <typename C> static two test(...);    			public:									    enum { value = (sizeof(test<T>(0)) == sizeof(char)) };			};

namespace sttr {
	STTR_HAS_FUNC(sttr_getClassSig)	// creates class has_sttr_getClassSig<Class>
	STTR_HAS_FUNC(sttr_getClassName)	// creates class has_sttr_getClassName<Class>
	}

#define LZZ_INLINE inline
namespace sttr {
  class RegBase {
  public:
    char const * name;
    bool isStatic;
    bool isConst;
    bool isVariable;
    bool isFunction;
    bool nullValueSet;
    double nullValueValue;
    RegNamespace * mNamespace;
    uint32_t userFlags;
    std::string userString;
    void * userData;
    RegBase (char const * _name);
    virtual ~ RegBase ();
    virtual void * construct ();
    virtual void visit (Visitor_Base * V);
    virtual void visitClass (Visitor_Base * V);
    virtual std::string getTypeName ();
    virtual std::string getTypePointingTo ();
    virtual unsigned long long int const getOffset () const;
  };
}
namespace sttr {
  template <typename T>
  void * construct_worker (T func, std::false_type isVariable);
}
namespace sttr {
  template <typename T>
  void * construct_worker (T func, std::true_type isVariable);
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS = 0>
  class Reg : public RegBase {
  public:
    T func;
    Reg (T v, char const * _name);
    virtual ~ Reg ();
    void visit (Visitor_Base * v);
    void visitClass (Visitor_Base * v);
    void * construct ();
    std::string getTypeName ();
    std::string getTypePointingTo ();
    unsigned long long int const getOffset () const;
  };
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  class Reg <T, sttr::NullType, FLAGS> : public RegBase {
  public:
    T func;
    Reg (T v, char const * _name);
    virtual ~ Reg ();
    void visit (Visitor_Base * v);
    void visitClass (Visitor_Base * v);
    void * construct ();
    std::string getTypeName ();
    std::string getTypePointingTo ();
    unsigned long long int const getOffset () const;
  };
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  class Reg <T, sttr::ParameterPack<ARGS...>, FLAGS> : public RegBase {
  public:
    T func;
    Reg (T v, char const * _name);
    virtual ~ Reg ();
    void visit (Visitor_Base * v);
    void visitClass (Visitor_Base * v);
    void * construct ();
    std::string getTypeName ();
    std::string getTypePointingTo ();
    unsigned long long int const getOffset () const;
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
    void * thisClassSig;
    void * uninstantiatedParent;
    RegNamespace (char const * _name);
    ~ RegNamespace ();
    void clear ();
    template <typename CT = sttr::NullType, typename T = sttr::NullType, unsigned int FLAGS = 0>
    RegNamespace & regField (T v, char const * _name);
    template <typename CT, typename T>
    RegNamespace & regField (T CT::* v, char const * _name);
    RegNamespace & setUserFlags (uint32_t const & userFlags);
    RegNamespace & setUserString (std::string const & userString);
    RegNamespace & setUserData (void * userData);
    RegNamespace & setClassUserFlags (uint32_t const & userFlags);
    RegNamespace & setClassUserString (std::string const & userString);
    RegNamespace & setClassUserData (void * userData);
    uint32_t getClassUserFlags () const;
    std::string setClassUserString () const;
    void * getClassUserData () const;
    template <typename T, unsigned int FLAGS = 0, typename PP = sttr::NullType>
    RegNamespace & beginClass (char const * _name);
    template <typename BASE, typename DERIVED, unsigned int FLAGS = 0, typename STTR_VARADIC_TEMPLATE_ARGS1>
    RegNamespace & deriveClass (char const * name);
    RegNamespace & endClass ();
    void * construct_retVoidPtr ();
    template <typename T>
    T * construct ();
    template <typename T, typename V>
    T * safeUpcast (V * v);
    RegNamespace & findClass (char const * class_name);
    RegNamespace * findClassPointer (char const * class_name);
    RegNamespace * findClassPointerBySig (void * target);
    RegNamespace * getBaseClass ();
    bool isDerivedFromSig (void * target);
    void visitRecursive (Visitor_Base * v);
    void visitPolymorthic (Visitor_Base * v);
    template <typename T>
    void visitPolymorthic (Visitor_Base * v, T * mClass);
    void visit (Visitor_Base * v);
    std::string toString (int const indent = 0) const;
    std::string toClassTreeString (int const indent = 0) const;
  };
}
namespace sttr {
  RegNamespace * getGlobalNamespace ();
}
namespace sttr {
  template <typename T>
  void * construct_worker (T func, std::false_type isVariable) { return NULL; }
}
namespace sttr {
  template <typename T>
  void * construct_worker (T func, std::true_type isVariable) {
	using TBase = typename std::remove_pointer<T>::type;
	return (void*) constructIfDefaultConstructible<TBase>();
	}
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  Reg <T, CT, FLAGS>::Reg (T v, char const * _name)
    : RegBase (_name), func (v) {}
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  Reg <T, CT, FLAGS>::~ Reg () {}
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  void Reg <T, CT, FLAGS>::visit (Visitor_Base * v) {
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
  template <typename T, typename CT, unsigned int FLAGS>
  void Reg <T, CT, FLAGS>::visitClass (Visitor_Base * v) {}
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  void * Reg <T, CT, FLAGS>::construct () { return construct_worker(func, sttr::is_variable<T>()); }
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  std::string Reg <T, CT, FLAGS>::getTypeName () { return sttr::getTypeName<T>(); }
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  std::string Reg <T, CT, FLAGS>::getTypePointingTo () { return sttr::getTypeName<decltype(sttr::getTypePointingTo(func))>(); }
}
namespace sttr {
  template <typename T, typename CT, unsigned int FLAGS>
  unsigned long long int const Reg <T, CT, FLAGS>::getOffset () const {
	const unsigned long long int * first = reinterpret_cast<const unsigned long long int *>(&func);
	unsigned char const * first2 = reinterpret_cast<unsigned char *>(*first);
	return reinterpret_cast<size_t>(first2);
	}
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  Reg <T, sttr::NullType, FLAGS>::Reg (T v, char const * _name)
    : RegBase (_name), func (v) {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  Reg <T, sttr::NullType, FLAGS>::~ Reg () {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  void Reg <T, sttr::NullType, FLAGS>::visit (Visitor_Base * v) {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  void Reg <T, sttr::NullType, FLAGS>::visitClass (Visitor_Base * v) {
	// Used for the actual class types (RegNamespace::thisClass)
	#ifdef STTR_CLASS_VISITORS
		STTR_CLASS_VISITORS
	#endif
	v->visitClass(this);
	}
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  void * Reg <T, sttr::NullType, FLAGS>::construct () { return construct_worker(func, sttr::is_variable<T>()); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  std::string Reg <T, sttr::NullType, FLAGS>::getTypeName () { return sttr::getTypeName<T>(); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  std::string Reg <T, sttr::NullType, FLAGS>::getTypePointingTo () { return sttr::getTypeName<decltype(sttr::getTypePointingTo(func))>(); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS>
  unsigned long long int const Reg <T, sttr::NullType, FLAGS>::getOffset () const {
	const unsigned long long int * first = reinterpret_cast<const unsigned long long int *>(&func);
	unsigned char const * first2 = reinterpret_cast<unsigned char *>(*first);
	return reinterpret_cast<size_t>(first2);
	}
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::Reg (T v, char const * _name)
    : RegBase (_name), func (v) {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::~ Reg () {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  void Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::visit (Visitor_Base * v) {}
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  void Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::visitClass (Visitor_Base * v) {
	// Used for the actual class types (RegNamespace::thisClass)
	#ifdef STTR_CLASS_VISITORS
		STTR_CLASS_VISITORS
	#endif
	v->visitClass(this);
	}
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  void * Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::construct () { return construct_worker(func, sttr::is_variable<T>()); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  std::string Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::getTypeName () { return sttr::getTypeName<T>(); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  std::string Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::getTypePointingTo () { return sttr::getTypeName<decltype(sttr::getTypePointingTo(func))>(); }
}
namespace sttr {
  template <typename T, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  unsigned long long int const Reg <T, sttr::ParameterPack<ARGS...>, FLAGS>::getOffset () const {
	const unsigned long long int * first = reinterpret_cast<const unsigned long long int *>(&func);
	unsigned char const * first2 = reinterpret_cast<unsigned char *>(*first);
	return reinterpret_cast<size_t>(first2);
	}
}
namespace sttr {
  template <typename CT, typename T, unsigned int FLAGS>
  RegNamespace & RegNamespace::regField (T v, char const * _name) {
	// for registration of anything
	RegBase * R = new Reg<T, CT, FLAGS>(v,_name);
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
  template <typename T, unsigned int FLAGS, typename PP>
  RegNamespace & RegNamespace::beginClass (char const * _name) {
	RegNamespace * R = new RegNamespace(_name);
	R->parent = this;
	R->thisClass = new Reg<T*, PP, FLAGS>(NULL, _name);
//	R->baseClassTuple = new Reg<VaradicWrap<>*, T>(NULL, "");
	if (FLAGS) R->thisClass->userFlags = FLAGS;
	R->thisClassSig = sttr::getTypeSignature<T>();
	classes.push_back(R);
	
	// Find any children and add them
	RegNamespace * Base = getBaseClass();
	void * thisSig = sttr::getTypeSignature<T>();
	for (size_t i = 0; i < Base->classes.size(); ++i) {
		if (Base->classes[i]->uninstantiatedParent == thisSig) {
		Base->classes[i]->uninstantiatedParent = NULL;
		R->classes.push_back(Base->classes[i]);
		Base->classes[i]->parent = R;
		Base->classes.erase(Base->classes.begin() + i);
		--i;
		}
		}
	
	return *R;
	}
}
namespace sttr {
  template <typename BASE, typename DERIVED, unsigned int FLAGS, typename STTR_VARADIC_TEMPLATE_ARGS1>
  RegNamespace & RegNamespace::deriveClass (char const * name) {
	/// Usage R.deriveClass < BaseClass, FinalClass, (Flags), (IntermediateClass1), ... (IntermediateClassN)>("FinalClass")
	/// Can be used to define a class even if its parent hasn't be defined yet.
	// Should only be used for a base of a tree
	assert((!parent) && "sttr::RegNamespace::deriveClass being called on a RegNamespace that is not the base (it has a parent)");
	
	static_assert(std::is_base_of<BASE,DERIVED>::value, "Class is not derived from base");
	#if __cplusplus >= 201703L
		static_assert((std::is_base_of<BASE,ARGS>::value && ...), "Class is not derived from bases (varadic pack member)");
	#endif
		
	void* baseSig = sttr::getTypeSignature<BASE>();
	RegNamespace * R = findClassPointerBySig(baseSig);
	if (R) {
		RegNamespace & RR = R->beginClass<DERIVED, FLAGS, sttr::ParameterPack<BASE,ARGS...> >(name);
//		RR.storeBaseClassTuple<DERIVED,BASE>();
		return RR;
		}
		
	RegNamespace & RR = beginClass<DERIVED, FLAGS, sttr::ParameterPack<BASE,ARGS...> >(name);
//	RR.storeBaseClassTuple<DERIVED, BASE>();
	RR.uninstantiatedParent = baseSig;
	return RR;
	}
}
namespace sttr {
  template <typename T>
  T * RegNamespace::construct () {
	return (T*) construct_retVoidPtr();
	}
}
namespace sttr {
  template <typename T, typename V>
  T * RegNamespace::safeUpcast (V * v) {
	//std::cout << "safe upcast: " << sttr::getTypeName<V>() << " to " << sttr::getTypeName<T>() << ", actual class = " << findClassPointerBySig(v->sttr_getClassSig())->name << std::endl;
	RegNamespace * R = findClassPointerBySig(sttr::getTypeSignature<T>());
	if (!R) return NULL;
	//std::cout << "found R!! : " << sttr::getTypeName<V>() << " to " << sttr::getTypeName<T>() << std::endl;
	if (R->isDerivedFromSig(v->sttr_getClassSig())) return (T*) v;
	return NULL;
	}
}
namespace sttr {
  template <typename T>
  void RegNamespace::visitPolymorthic (Visitor_Base * v, T * mClass) {
	// Calls visit for T. Searches the tree for the instance type. Requires sttr_getClassSig() to be defined for all classes in the chain
	RegNamespace * R = findClassPointerBySig(mClass->sttr_getClassSig());
	// Find the base class
	//std::cout << "mClass->sttr_getClassSig() " << mClass->sttr_getClassSig() << " this " << thisClass->name << " sig: " << thisClassSig <<  " R " << R << std::endl;
	if (!R) return;
	while (R->thisClass) {
		R->visit(v);
		R = R->parent;
		if (!R) break;
		}
	}
}
#undef LZZ_INLINE
#endif
