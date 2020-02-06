// sttr.tpl
//

#include "sttr.h"
namespace sttr
{
  template <typename T>
  std::string getTypeName ()
                          {
	return sttr::getTypeName_filterTypeOut(__PRETTY_FUNCTION__);
	}
}
namespace sttr
{
  template <typename T>
  void Visitor_Base::visit (sttr::Reg <T> * RB)
                                      {
		//cout << "Visitor_Base" << endl;
		}
}
namespace sttr
{
  template <typename T>
  void Typename_Extractor_Visitor::visit (Reg <T> * RB)
                                {
		type_name_out = sttr::getTypeName<decltype(RB->func)>();
		}
}
namespace sttr
{
  template <typename T>
  Reg <T>::Reg (T v, char const * _name)
    : func (v), RegBase (_name)
                                                               {}
}
namespace sttr
{
  template <typename T>
  void Reg <T>::visit (Visitor_Base * v)
                                     {
		// Upcast and get the right visit
		STTR_ADD_VISITOR(Typename_Extractor_Visitor)
		
		#ifdef STTR_VISITORS
			STTR_VISITORS
		#else
			#warning STTR_VISITORS macro not defined! To suppress this warning define it as an empty string "#define STTR_VISITORS". To add a visitor use #define STTR_VISITORS STTR_ADD_VISITOR(visitoclass_type) STTR_ADD_VISITOR(anothevisitoclass_type) ... etc.
		#endif
		v->visit(this);
		}
}
namespace sttr
{
  template <typename T>
  unsigned char const * Reg <T>::getAddr ()
                                        {
		unsigned char const * first  = reinterpret_cast<unsigned char *>(&func);
		return reinterpret_cast<unsigned char *>(*first);
		}
}
namespace sttr
{
  template <typename T>
  RegNamespace & RegNamespace::regField (T v, char const * _name)
                                                         {
		RegBase * R = new Reg<T>(v,_name);
		R->isFunction = std::is_function<typename std::remove_pointer<T>::type>::value;
		R->isVariable = !R->isFunction;
		R->isConst = std::is_const<typename std::remove_pointer<T>::type>::value;
		R->isStatic = parent && !std::is_member_pointer<T>::value;
		members.push_back(R);
		return *this;
		}
}
namespace sttr
{
  template <typename T>
  RegNamespace & RegNamespace::beginClass (char const * _name)
                                                      {
		RegNamespace R (_name);
		R.parent = this;
		R.thisClass = new Reg<T*>(NULL, _name);
		classes.push_back(R);
		return classes[classes.size()-1];
		}
}
