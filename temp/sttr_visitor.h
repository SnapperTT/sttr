// sttr_visitor.h
//

#ifndef LZZ_sttr_visitor_h
#define LZZ_sttr_visitor_h
#include <type_traits>
#include <string>

#include "type_to_string.hpp"

#define STTR_VISITOR_HEADER

#define STTR_ADD_VISITOR(mVisitorType) 					if (v->getSignature() == mVisitorType::getSignatureStatic()) { 		mVisitorType * vv = (mVisitorType *) v; 				vv->visit(this); 							return;								}
	
#define STTR_ADD_CLASS_VISITOR(mVisitorType) 				if (v->getSignature() == mVisitorType::getSignatureStatic()) { 		mVisitorType * vv = (mVisitorType *) v; 				vv->visitClass(this); 						return;								}
	
namespace sttr {
	class NullType {};
	
	template<typename ...ARGS>
	struct VaradicWrap{};

	template<typename T, typename CT>
	class Reg;
	
	// Utility templates
	
	// Yanks out the type of a member pointer 
	template <class CT, typename T>
	T getTypePointingTo(T CT::*v);
	
	template <typename T>
	T getTypePointingTo(T * v);
	
	template <typename T>
	T getTypePointingTo(T v);
	
	
//	template<class CT, typename T>
//	inline T getBaseType(T *v) {}
	
	// Disjunction - for versions of c++ older than std::c++17
	template<class...> struct disjunction : std::false_type { };
	template<class B1> struct disjunction<B1> : B1 { };
	template<class B1, class... Bn>
	struct disjunction<B1, Bn...> 
	: std::conditional_t<bool(B1::value), B1, disjunction<Bn...>> { };
	
	template <typename T>
	using negate = std::integral_constant<bool, !T::value>;

	template <typename T>
	using is_function = sttr::disjunction<std::is_function<typename std::remove_pointer<T>::type>, std::is_member_function_pointer<T>>;

	template <typename T>
	using is_variable = sttr::negate<sttr::is_function<T>>;

	template <typename T>
	using is_static = sttr::negate<std::is_member_pointer<T>>;

	template <typename T>
	using is_const = std::is_const<typename std::remove_pointer<T>::type>;
	
//	template<typename T>
//static char * getTypeSignature(); // Should this be static??

	inline std::string getTypeName_filterTypeOut(const std::string & in) {
	// Find "with T = " and stop when reaching ";"
	size_t start = in.find("T = ");
	if (start == std::string::npos) return (in);
	const std::string r = in.substr(start+4);
	size_t end = r.find(";");
	return (r.substr(0,end));
	}

	/// Prints the type of a function
	#if __cplusplus >= 201703L
	template<typename T>
	inline const char * getTypeName() {
	// This is set at compile time
	return sttr::type_to_string_impl::TypeName<T>();
	}
	#else
	template<typename T>
	std::string getTypeName() {
	// has to compile getTypeName_filterTypeOut - will spit out of heaps of asm (179 instructions for an int)
	#ifndef STTR_SUPPRESS_CXX17_WARNING
		#warning sttr::getTypeName - using slow code path. Enable c++17 or later to solve. (define STTR_SUPPRESS_CXX17_WARNING to suppress warning)
	#endif
	return sttr::getTypeName_filterTypeOut(__PRETTY_FUNCTION__).c_str();
	}
	#endif

	}
	
#define LZZ_INLINE inline
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible_worker (std::false_type isDefaultConstructible);
}
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible_worker (std::true_type isDefaultConstructible);
}
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible ();
}
namespace sttr {
  template <typename T>
  char * getTypeSignature ();
}
namespace sttr {
  template <typename T, typename R>
  bool isType (R const & r);
}
namespace sttr {
  template <typename T, typename R>
  bool isType (R const * const r);
}
namespace sttr {
  class Visitor_Base {
  public:
    template <typename T, typename CT>
    void visit (sttr::Reg <T, CT> * RB);
    template <typename T, typename CT>
    void visitClass (sttr::Reg <T, CT> * CLASS);
    virtual void * getSignature ();
    static void * getSignatureStatic ();
  };
}
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible_worker (std::false_type isDefaultConstructible) { return NULL; }
}
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible_worker (std::true_type isDefaultConstructible) { return new T; }
}
namespace sttr {
  template <typename T>
  T * constructIfDefaultConstructible () {
	return constructIfDefaultConstructible_worker<T>(std::is_default_constructible<T>());
	}
}
namespace sttr {
  template <typename T>
  char * getTypeSignature () {
	static char R;
	return &R;
	}
}
namespace sttr {
  template <typename T, typename R>
  bool isType (R const & r) { return r.sttr_getClassSig() == sttr::getTypeSignature<T>(); }
}
namespace sttr {
  template <typename T, typename R>
  bool isType (R const * const r) { return r->sttr_getClassSig() == sttr::getTypeSignature<T>(); }
}
namespace sttr {
  template <typename T, typename CT>
  void Visitor_Base::visit (sttr::Reg <T, CT> * RB) {
	// Your code here if deriving from this class 
	}
}
namespace sttr {
  template <typename T, typename CT>
  void Visitor_Base::visitClass (sttr::Reg <T, CT> * CLASS) {
	// This is called for RegNamespace::thisClass. Used to visit class types themselves
	}
}
#undef LZZ_INLINE
#endif
