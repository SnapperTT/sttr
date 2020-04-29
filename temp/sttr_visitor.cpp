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
