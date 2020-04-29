#ifdef STTR_ENABLED
	#ifndef STTR_CLASS_SIG
		#define STTR_CLASS_SIG(X) virtual void * sttr_getClassSig() const { return (void*) sttr::getTypeSignature<X>(); } \
								virtual const char * const sttr_getClassName() const { return sttr::getTypeName<X>(); }
	#endif
#endif


