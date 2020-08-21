#ifndef STTR_CLASS_SIG
	// Create auto 
	#define STTR_CLASS_SIG(X) virtual void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } 	\
							virtual const char * const sttr_getClassName() const { return sttr::getTypeName<X>(); }
	
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
	#define STTR_CLASS_SIG_NON_POLY(X) void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } \
									const char * const sttr_getClassName() const { return sttr::getTypeName<X>(); }
							
	#define STTR_AUTO_REG_FUNC(REG_FUNC) static char sttr_regAutoInvoke = (REG_FUNC(), 1);
	
	#define STTR_AUTO_CLASS(X) 	STTR_CLASS_SIG(X) 								\
								STTR_AUTO_REG_FUNC(sttr_register)
								
	#define STTR_AUTO_CLASS_POLY(BASE,X) 	STTR_CLASS_SIG(X) 					\
											STTR_AUTO_REG_FUNC(sttr_register)	\
											STTR_AUTO_UPCAST(BASE,X)
								
	#define STTR_AUTO_CLASS_NON_POLY(X) 	STTR_CLASS_SIG_NON_POLY(X) 			\
											STTR_AUTO_REG_FUNC(sttr_register)
		
#endif


