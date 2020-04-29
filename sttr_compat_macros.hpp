#ifndef STTR_CLASS_SIG
	#define STTR_CLASS_SIG(X) virtual void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } \
							virtual const char * const sttr_getClassName() const { return sttr::getTypeName<X>(); }
							
	#define STTR_AUTO_REG_FUNC(REG_FUNC) static char sttr_regAutoInvoke = (REG_FUNC(), 1);
	#define STTR_AUTO_CLASS(X) 	STTR_CLASS_SIG(X) \
								STTR_AUTO_REG_FUNC(sttr_register)
#endif


