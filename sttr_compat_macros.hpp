#ifndef STTR_CLASS_SIG
	// Create auto 
	#define STTR_NULL_ARG
	#define STTR_CLASS_SIG_WORKER(X,VIRTUAL,NAME) VIRTUAL void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } 	\
													VIRTUAL const char * const sttr_getClassName() const { return NAME; }
							
	#define STTR_CLASS_SIG(X) STTR_CLASS_SIG_WORKER(X, virtual, sttr::getTypeName<X>())
	
	// Automatic safe upcast function without RTTI
	#define STTR_AUTO_UPCAST(BASE,DERIVED)																				\
							static DERIVED * upcast(BASE * B) {															\
								if (sttr::isType<DERIVED>(*B)) return (DERIVED*) B;										\
								return NULL;																			\
								}																						\
							static const DERIVED * const upcastC(const BASE * const B) {								\
								if (sttr::isType<DERIVED>(*B)) return (const DERIVED* const) B;							\
								return NULL;																			\
								}
	// STTR_CLASS_SIG without virtual			
	#define STTR_CLASS_SIG_NON_POLY(X) STTR_CLASS_SIG_WORKER(X, STTR_NULL_ARG, sttr::getTypeName<X>())
							
	#define STTR_AUTO_REG_FUNC(REG_FUNC) static char sttr_regAutoInvoke = (REG_FUNC(), 1);
	
	#define STTR_AUTO_CLASS(X) 	STTR_CLASS_SIG(X) 								\
								STTR_AUTO_REG_FUNC(sttr_register)
								
	#define STTR_AUTO_CLASS_POLY(BASE,X) 	STTR_CLASS_SIG(X) 					\
											STTR_AUTO_REG_FUNC(sttr_register)	\
											STTR_AUTO_UPCAST(BASE,X)
								
	#define STTR_AUTO_CLASS_NON_POLY(X) 	STTR_CLASS_SIG_NON_POLY(X) 			\
											STTR_AUTO_REG_FUNC(sttr_register)
		
	
	// HPP/CPP variants
	#define STTR_CLASS_SIG_NON_POLY_HPP(X) void* sttr_getClassSig() const; \
											const char * const sttr_getClassName() const;
	
	#define STTR_CLASS_SIG_NON_POLY_CPP(X) void* X::sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } \
											const char * const X::sttr_getClassName() const { return sttr::getTypeName<X>(); }
											
	#define STTR_AUTO_REG_FUNC_HPP(X, REG_FUNC) static char sttr_regAutoInvoke;
	
	#define STTR_AUTO_REG_FUNC_CPP(X, REG_FUNC) char X::sttr_regAutoInvoke = (X::REG_FUNC(), 1);
#endif


