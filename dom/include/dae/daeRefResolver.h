/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_REF_RESOLVER_H__
#define __COLLADA_DOM__DAE_REF_RESOLVER_H__

#include "daeSmartRef.h"
#include "daeAtomicType.h"
#include "daeErrorHandler.h"

COLLADA_(namespace)
{//-.
//<-'
	
/**WARNING
 * COLLADA C++ class for retrieving the results of 
 * reference-based queries.
 * @see @c daeRefResolver.
 *
 * @warning This is a low-level structure-like object.
 * @c reset() must be called after it is used. This is
 * a source of errors. 
 * @warning @c object should not be overwritten unless
 * it's known to have a @c DAE_OK result. This is because
 * if it is not @c nullptr, @c object->getDOM() must be used
 * to locate the referenced object.
 * 
 * Resolvers must fill out @c object, and @c typeInstance if
 * @c DAE_OK. Otherwise they mustn't modify the request object.
 * If @c typeInstance is not @c nullptr, the remaining members
 * must be filled out.
 *
 * @c rangeMax holds @c sizeof(daeRefRequest). The meaning of any
 * data beyond @c rangeMax depends on @c daeRef::getRefType(), and
 * is caller initialized. 
*/
class daeRefRequest : public daeHashString
{
COLLADA_(public) //THIS CLASS IS STRUCT-LIKE
	/**WARNING
	 * @c typeInstance points to the first element in an array.
	 * @warning @c typeInstance does not factor in @c rangeMin.
	 */
	daeObjectRef object; daeOpaque typeInstance;	
	/**
	 * If @c type is a @c daeAtomicType::STRING the system reserves
	 * the right to set @c rangeMin and @c rangeMax to capture items
	 * in the list on a code-point to code-point basis. 
	 * In that case, the number of items must be discovered by looking
	 * for whitespace between the items. 
	 * @note An xs:string cannot be items in a list. Therefore it should
	 * be thought of as a pseudo-array (or xs:list.)
	 *
	 * TODO? @c type was @c daePrototype* but @c daeData cannot guarantee
	 * That, so it can be either be a @c daeTypewriter, or it can provide
	 * even more information by being a @c daeData. SID usage always goes
	 * by element values, but other refs may retreive attributes or child
	 * elements, or even more exotic/arbitrary data.
	 */
	daeTypewriter *type; size_t rangeMin,rangeMax;

	//SCHEDULED FOR REMOVAL
	/**
	 * Ex: @c req->a<domCOLLADA>() to call @c daeSafeCast().
	 * @remark This just feels wrong. It seems like it will
	 * definitely be removed or deprecated after some while.
	 * @return Returns @c nullptr, so -> must be chained so
	 * to only call a method that will check for @c nullptr.
	 */
	inline daeElement *operator->()const
	{
		assert(object==nullptr||object->_isElement());
		return (daeElement*&)object; 
	}
	
	/**WARNING
	 * Empty Request Constructor 
	 * @warning Maybe the string should be set to @c nullptr
	 * just to be safe? It's just now the extent is set to 0
	 * and the string is rarely used, so the pointer doesn't 
	 * have to be initialized.
	 */
	daeRefRequest(size_t extra=sizeof(daeRefRequest))
	:/*daeHashString(cp),*/rangeMax(extra)
	{}
	/**
	 * Constructor with arguments for the URL-like reference.
	 * The arguments are in the form of a string, that the ref must understand.
	 * E.g. for SIDREF this can be a profile. If @c nullptr SIDREFs use "profile_COMMON"
	 */
	daeRefRequest(daeHashString cp, size_t extra=sizeof(daeRefRequest))
	:daeHashString(cp),rangeMax(extra)
	{}

	/** WARNING: @c reset cannot initialize data beyond @c maxValue. */
	inline void reset(daeHashString cp=nullptr, size_t extra=sizeof(daeRefRequest))
	{
		new(this) daeRefRequest(cp,extra); 
	}

	/**
	 * Tells if @c typeInstance is a nonzero @c daeOpaque object.
	 */
	inline bool hasValue()const{ return &typeInstance!=nullptr; }

	/**
	 * Formerly "getAtomicType."
	 * Gets the atom's typewriter/typeinfo. 
	 * This API returns the @c daeTypewriter of @c typeInstance.
	 * @return Returns @c daeAtomicType_void if @c type==nullptr.
	 */
	inline daeTypewriter &getValueType()const
	{
		return &typeInstance==nullptr?daeAtomicType_void:type->where<daeAtom>();
	}
			
COLLADA_(public) //UNUSED (non COLLADA applications can have at it.)

	//Reminder: @c daeRef::get() was overloaded for getting values.
	//It seemed useful at first, but in practice the values should
	//be animated, and they are ultimately bound to be cast anyway.
	//BUT the functionality was removed because to be useful there
	//has to be a default value provided to "get" which must match
	//the overloaded template parameters that is extraneous to the
	//regular element/object getting API.

	template<typename daeT>
	/**UNUSED Implements @c getScalar(). */
	inline daeT &_got()const
	{
		return ((daeT*)&typeInstance)[rangeMin];
	}
	template<typename daeT, typename T>
	/**UNUSED Implements @c getValue(). */
	inline void _copy(daeOffset o, T *q)const
	{
		for(const daeT*p=&_got<daeT>();o>0;o--) *q++ = (T)*p++;
	}
	#ifdef NDEBUG
	#error This could support cross-module types by not using daeAtomicType.
	#endif
	template<typename T> COLLADA_NOINLINE
	/**UNUSED, EXPERIMENTAL
	 * Formerly "getVector."
	 * This operation is provided because the COLLADA user manual seems
	 * to suggest that values are to be "cast" or "directly cast" given
	 * an initial value.
	 * @see daeRef::get<scalar_type>() and daeRef::get<scalar_type,N>().
	 */
	inline daeOffset getValue(daeOffset N, T *vN, T def=T())const
	{
		daeOffset selected = N>1?rangeMax-rangeMin+1:1;
		daeOffset o = std::min(N,selected);
		//REMINDER: these can be equal, in which case switch won't work.
		//This can be done somewhat better, but the order is roughly in
		//the likelihood that a type is used, in case it matters at all.
		int at = getValueType().getAtomicType();
		if(at==daeAtom::DOUBLE) //COLLADA_DOM_ATOMIC_FLOAT(double)?
		_copy<daeDouble>(o,vN);
		else if(at==daeAtom::FLOAT) //COLLADA_DOM_ATOMIC_FLOAT(float)?
		_copy<daeFloat>(o,vN);			
		else if(at==daeAtom::INT)		
		_copy<daeInt>(o,vN);
		else if(at==daeAtom::UINT)		
		_copy<daeUInt>(o,vN);
		else if(at==daeAtom::LONG)		
		_copy<daeLong>(o,vN);
		else if(at==daeAtom::ULONG)		
		_copy<daeULong>(o,vN);					
		else if(at>=daeAtom::STRING)
		{
			//This is here to support anySimpleType (domAny) or union
			//data. An array of strings is typed, and won't be casted.
			if(at==type->getAtomicType())
			{
				const daeStringRef &s = typeInstance;				
				daeString p = s.data(); daeStringCP *e;
				for(size_t i=o=0;o<N;i++)
				{
					double d = strtod(p,&e);
					if(i>=rangeMin&&e>p) 
					vN[o++] = (T)d;
					p = e; while(COLLADA_isspace(*p)) p++;
					if(p==e) break;
				}
			}
			else o = 0; //A string list?
		}
		else if(at==daeAtom::BYTE)		
		_copy<daeByte>(o,vN);
		else if(at==daeAtom::UBYTE)		
		_copy<daeUByte>(o,vN);
		else if(at==daeAtom::SHORT)		
		_copy<daeShort>(o,vN);
		else if(at==daeAtom::USHORT)		
		_copy<daeUShort>(o,vN);
		else if(at==daeAtom::BOOLEAN)		
		_copy<daeBoolean>(o,vN);	
		else if(at==daeAtom::UBOOLEAN)		
		_copy<bool>(o,vN);
		else o = 0; for(daeOffset i=o;i<N;i++) vN[i] = def; return o;
	}	
};					

/**SCOPED-ENUM
 * These enums indicate types of class @c daeRef.
 * @c daeRefResolver advertises a matching value.
 * @c undifferentiated can mean different things,
 * depending on if it's a ref or a resolver that
 * says so: For resolvers it means it deals with
 * any and all refs; For a ref, it means that it
 * should be treated as if it's a pure @c daeRef.
 */
struct daeRefType
{
	enum{ undifferentiated=0,URI,IDREF,SIDREF };
};
  
/**ABSTRACT BASE-CLASS
 * Since 2.5 refs (URI,IDREF,etc.) are based on @c daeRef.
 */
class daeRef : public daeObject
{	
COLLADA_(public)
	/**
	 * Can reasonably assume that refs are not parent objects?
	 */
	typedef void __COLLADA__atomize;

COLLADA_(protected)
	/**
	 * Non-Constructor 
	 * This is used to extract a "vptr" or prior to placement-new.
	 */
	daeRef(){ /*NOP*/ }
	/**
	 * Constructor 
	 */								
	explicit daeRef(const DAEP::Object *c):daeObject(c){}

COLLADA_(public) //ABSTRACT INTERFACE
	/**PURE
	 * One of @c daeRefType, or negative if registered as a client type.
	 */
	virtual int __daeRef__v1__type()const = 0; 
	/**IMPORTANT
	 * Classes based on daeRef should override this with a static method.
	 */
	inline int getRefType()const{ return __daeRef__v1__type(); }

	/**PURE
	 * Gets this URI like reference's text--refreshed; cannot return @c nullptr.
	 */
	virtual daeString __daeRef__v1__text()const = 0;
	/**
	 * Gets this URI like reference's text--refreshed; cannot return @c nullptr.
	 */
	inline daeString getRefText()const{ return __daeRef__v1__text(); }

COLLADA_(protected) //RESERVED INTERFACE

	//The virtual table reserves these.
	//If added, @c daeObject::_getVersion() must be checked.
	//(The working assumption is that these methods can be renamed without breaking the system.)
	/*
	virtual daeError __daeRef__reserved_vtable_entry_0(){ return DAE_ERR_NOT_IMPLEMENTED; }
	virtual daeError __daeRef__reserved_vtable_entry_1(){ return DAE_ERR_NOT_IMPLEMENTED; }
	virtual daeError __daeRef__reserved_vtable_entry_2(){ return DAE_ERR_NOT_IMPLEMENTED; }
	virtual daeError __daeRef__reserved_vtable_entry_3(){ return DAE_ERR_NOT_IMPLEMENTED; }
	*/

COLLADA_(public) //LEGACY QUERY API

	/**WARNING, LEGACY-SUPPORT, CIRCULAR DEPENDENCY
	 * @warning Relies on @c daeDOM::getRefResolvers().
	 * Goes beyond @c get(), returning either an xs:list dataset, 
	 * -or, an individual unit of data. (This implements SIDREF.)
	 * @see ColladaDOM.inl header's definition.
	 */
	inline daeOK get(daeRefRequest &req)const;
	//{	
	//	daeDOMRef dom = 
	//	(req.object!=nullptr?req.object->getDOM():getDOM());
	//	if(dom!=nullptr) 
	//	return dom->getRefResolvers().resolve(*this,req);
	//	return DAE_ERR_INVALID_CALL;
	//}
	 
	//EXPERIMENTAL
	template<class T, bool=daeArrayAware<T>::is_class>
	/**
	 * This template exists so @c get() can return an element pointer
	 * or a scalar type (for COLLADA's SIDREF casting specification.)
	 */
	struct get_return
	{
		typedef typename T::__COLLADA__T *type;		

		static type cast(const daeRefRequest &req, type def=nullptr)
		{
			//There are other ways to do this, but this is the safest
			//way, and eventually it will be desirable to extract the
			//binary-compatibility identifier.
			if(nullptr==req.object||_unsafe(*dae(*(type*)&req.object)))
			const_cast<daeObjectRef&>(req.object) = dae(def);
			return *(type*)&req.object;
		}
		static bool _unsafe(const daeObject &o)
		{
			#ifdef COLLADA_dynamic_daeSafeCast
			return dynamic_cast<type>(&*req.object)==nullptr
			#else
			return o.__DAEP__Object__v1__model()
			!=typename T::__COLLADA__T().__DAEP__Object__v1__model()
			#endif
			?_true(o):false;
		}
		static bool _unsafe(const daeElement &e)
		{
			return daeUnsafe<T>(e)?_true(e):false;
		}
		static bool _true(const daeObject &o)
		{
			//This is good to know if a URI expects one type but gets another type.
			//(Ex. It might indicate that an id or fragment is entered incorrectly.)
			daeEH::Warning<<"Nullifying dereference because types are mismatched:\n"
			"Sought typeid("<<typeid(*(type*)0).name()<<")!=typeid("<<typeid(o).name()<<")";
			return true;
		}
	};
	/**PARTIAL-TEMPLATE-SPECIALIZATION
	 * @c get<T>() uses this return type if T is not a C++ class type.
	 */
	template<class Scalar> struct get_return<Scalar,false>
	{
		typedef Scalar type; 
		
		static type &cast(const daeRefRequest &req, type &out)
		{
			req.getValue<Scalar>(1,&out,out); return out;
		}
	};

	//EXPERIMENTAL
	//This was made much more complicated to work with @c getVector().
	//It turns out that in practice @c getVector() may rarely if ever
	//be used. The second argument was added for vectors and may need
	//to be removed. It's uncharacteristic of most other APIs, and it
	//affects the regular element version of @c get() as well because
	//both forms must be congruent.
	//
	template<class T> //element or element smart-ref or a scalar type
	/**LEGACY-SUPPORT 
	 * Formerly "getElement."
	 * @warning Resolution is based on @c getDOM()->getRefResolvers().
	 * @see @c daeRefRequest::getVector().
	 *
	 * @return If @a T is a class, a pointer copy of @c req.object is
	 * returned. The pointer is backed by a smart-ref until @a req is
	 * destructed.
	 */	
	typename get_return<T>::type get(const daeRefRequest &req=daeName())const
	{
		get(const_cast<daeRefRequest&>(req)); return get_return<T>::cast(req);
	}		
	/*REFERENCE Previously get() had a symmetric default argument too.
	//This form is removed to dispense with the default argument need.
	//This is EXPERIMENTAL, considered for 2.5 but did not make it in.
	template<typename T, int N, class S> //T can only be scalar types.
	inline S &get(S &out, T def, const daeRefRequest &req=daeName())const
	{
		get(const_cast<daeRefRequest&>(req)); 
		req.getValue<T>(N,(T*)&out,def); return out; daeCTC<sizeof(S)==sizeof(T)*N>();
	}*/


	/**LEGACY SUPPORT 
	 * @warning Relies on @c daeDOM::getRefResolvers().
	 * Gets the object that this ref resolves to in memory.
	 * @see @c dae.h header's definition.
	 * @return Returns @c getDOM()->getRefResolvers().resolveObject(*this). 
	 */
	inline daeObjectRef getTargetedObject(const daeRefRequest &req=daeName())const
	{
		 get(const_cast<daeRefRequest&>(req)); return req.object;
	}

	/**LEGACY SUPPORT
	 * @warning Relies on @c daeDOM::getRefResolvers().
	 * Gets @c getTargetedObject() as @c daeElementRef.
	 * @return Returns @c nullptr if unresolved, or the target is not an element.
	 * @remarks If the target is a document, 
	 * @c getTargetedFragment() should NOT return the document's root element.
	 */
	inline daeElementRef getTargetedFragment(const daeRefRequest &req=daeName())const
	{
		if(DAE_OK==get(const_cast<daeRefRequest&>(req))&&req.object->_isElement())
		return (daeElementRef&)req.object; return nullptr;
	}

	COLLADA_DEPRECATED("Use getTargetedObject()->a<daeDocument>()\n\
	Or getTargetedObject()->getDoc()->a<daeDocument>() depending on use-case.\n")
	/**NOT-IMPLEMENTING
	 * This API never existed; But this is here to guide users who're looking for
	 * just such an API.
	 */	
	inline void getTargetedDocument()const;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeRef)

COLLADA_(public) //daeSafeCast() SHORTHANDS

	/** Follows style of daeElement::a(). */
	template<class T> T *a()
	{
		//This class is unlikely to be nullptr, however this
		//family of short cast APIs normally does this check.
		if(daeObject::_this__nullptr(this)) return nullptr;

		//For this to work, T must have a static getRefType().
		return T::getRefType()==getRefType()?(T*)this:nullptr;
	}
	/**CONST-FORM Follows style of daeElement::a(). */
	template<class T> const T *a()const
	{
		return const_cast<daeRef*>(this)->a<T>();
	}

COLLADA_(public) //ACCESSORS

	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Use get(), or getElementObject(), depending on use-case.\n")
	/**NOT-IMPLEMENTING
	 * Use @c get(), or @c getElementObject(), depending on the purpose. */
	void getElement()const;	
	COLLADA_DEPRECATED("getElementObject")
	/**DEPRECATED: Prefer @c getElementObject(). */
	inline const daeElement *getContainer()const{ return getElementObject(); }
	#endif //COLLADA_NODEPRECATED
	/**
	 * Gets the parent object as @c daeElement. 
	 * @returns @c nullptr if @c getParentObject() is not an element @c daeObjectType.
	 */
	inline const daeElement *getElementObject()const 
	{
		const daeObject &c = getParentObject();
		//daeElement is not defined at this point
		if(c._isElement()) return (const daeElement*)&c; return nullptr;
	}

COLLADA_(protected) //daeURI string-view implementation

	//NEW: this adds dae_clear/dae_append support to _getT().
	template<enum dae_clear clear, class T> 
	/**HELPER
	 * This is called by @c daeURI's parse APIs. It's implemented
	 * here in case another @c daeRef based class makes use of it.
	 * @remark This code came from earlier days/attempts to carry
	 * on legacy APIs. I don't know if it's still relevant or not.
	 */
	static inline void _getT(T &str, daeString pos, daeUShort len)
	{
		if(clear) str.clear(); _getT2(str,pos,len);
	}
	//Reminder: GCC/C++ can't reasonably do explicit-specialization.
	template<enum dae_clear clear>
	/**TEMPLATE-SPECIALIZATION @c daeName cannot be appended to. */
	static inline void _getT(daeName &str, daeString pos, daeUShort len)
	{
		str.string = pos; str.extent = len-1; daeCTC<clear>();
	}
	template<class T>
	////////////////////////////////////////////////////////////////
	//There is a problem here. daeURI_base uses these to capture a//
	//string-span in an array-like container. Which arrays include//
	//a 0-terminator and which don't is ill-defined. They all have//
	//a 0, but the count can differ. daeArray<daeStringCP> did not//
	//at first, because it was vector-like. append_and_0_terminate//
	//adoption has changed that, and made this all much more murky//
	////////////////////////////////////////////////////////////////
	/**
	 * This is the non 0-terminated form. It had covered daeRefView.
	 * Things evolved until explicit specializaiton made more sense.
	 */
	static inline void _getT2(T &str, daeString pos, daeUShort len)
	{
		//This converts "hello" into a size 6 array. The standard is 5.
		daeCTC<0>("Disabled for now to avoid 0-terminator confusion.");
		str.append/*assign*/(pos,len); str[str.size()/*len*/-1] = '\0'; 
	}
	template<int N> 
	/** This is bad. "append_and_0_terminate" complicates this. */
	static inline void _getT2(daeArray<daeStringCP,N> &str, daeString pos, daeUShort len)
	{
		/*str.clear();*/ str.append_and_0_terminate(pos,len-1);
	}
	template<class B, class C> 
	/** @c std::string cannot have a 0 terminator appended to it. */
	static inline void _getT2(std::basic_string<daeStringCP,B,C> &str, daeString pos, daeUShort len)
	{
		str.append/*assign*/(pos,len-1);
	}	
};

template<class This>
/**WARNING, HELPER
 * @warning IT'S VERY TEMPTING TO REMOVE THIS CLASS, BUT REFERENCES
 * REALLY DO HAVE SPECIAL NEEDS THAT REQUIRE BASIC A @c operator==()
 * SUPPORT. IT'S UNLIKE THE LIBRARY TO IMPLEMENT COMPARISON APIs, AND
 * SO IT SEEMS LIKE THIS IS HERE TO STAY--IDREF HAS DOCUMENT SEMANTICS
 * AND anyURI NEEDS TO BE RESOLVED TO SAY IF TWO ARE TRULY EQUAL OR NOT.
 *
 * This is similar to @c daeString_support, but it doesn't do string
 * comparison, it only does comparison between related ref-objects. To
 * do string-comparison, use the objects' respective APIs to get strings.
 *
 * Implementations of @c daeRef are encouraged to derive from this class.
 * This class fulfills @c daeTypist<void> requirements. It also implements
 * object-pointer comparison logic, which is quite complex, and ensures that
 * a temporary object is not constructed by comparison operators.
 *
 * CUSTOMIZATION
 * @tparam This can override @c _eq(), @c _lt(), and @c _gt(), and must
 * implement @c operator==(daeString).
 */
class daeRef_support : public daeRef
{
COLLADA_(public) //daeArray traits, etc.
	/**
	 * Refs sould NOT implement @c __COLLADA__move() nor 
	 * @c __COLLADA__locate(), because objects cannot be 
	 * housed inside @c daeArray, and there isn't a non-
	 * contiguous container that can hold them. (They're
	 * objects, so if the vector is resized, their smart
	 * -refs would point at the old location in memory.)
	 *
	 * This is so @c DAEP::InnerValue can pass an object
	 * when it constructors a temporary reference object.
	 */
	typedef const DAEP::Object __COLLADA__Object;
															
COLLADA_(protected) //MIRRORING OF daeRef CONSTRUCTORS
	/**
	 * Non-Constructor 
	 * This is used to extract a "vptr" or prior to placement-new.
	 */
	daeRef_support(){ /*NOP*/ }
	/**
	 * Constructor 
	 */								
	explicit daeRef_support(const DAEP::Object *c):daeRef(c){}

	/**
	 * Disabled Assignment Operator
	 * This is defined to catch compiler generator @c operator= definitions.
	 * It's nice that this class is a template, so it's not instantiated if
	 * it's not called.
	 */
	daeRef_support &operator=(const daeRef_support&){ daeCTC<0>(); }

COLLADA_(public) //GENERIC COMPARISON-OPERATOR SUPPORT

	///////////////////////////////////////////////////
	//TODO? IT'S VERY TEMPTING TO REMOVE ALL OF THESE//
	//Reminder: daeTypist::collate() depends on these//
	///////////////////////////////////////////////////		
	
	/** Implements @c operator==() without conversion. */
	inline bool _eq(const This *cmp)const
	{
		return ((This*)this)->size()==cmp->size()
		&&0==memcmp(((This*)this)->data(),cmp->data(),cmp->size());
	}
	template<class T> 
	/** @c COLLADA_DOM_OBJECT_OPERATORS. */
	inline bool _eq(T *const*cmp){ return (void*)this==*cmp; }	
	template<class T> 
	/** @c COLLADA_DOM_OBJECT_OPERATORS. CONST-FORM */
	inline bool _eq(T *const*cmp)const{ return (void*)this==*cmp; } 
	/** Implements @c operator>() without conversion. */
	inline bool _gt(const This *cmp)const
	{
		return strcmp(((This*)this)->data(),cmp->data())>0;
	}
	/** Implements @c operator>() without conversion. */
	inline bool _lt(const This *cmp)const
	{
		return strcmp(((This*)this)->data(),cmp->data())<0;
	}

	template<class Explicit> //daeURI_base or daeURI*->daeObject*
	/**LEGACY
	 * Comparison Operator
	 * @return Returns true if URI's are equal.
	 */
	inline bool operator==(Explicit &cmp){ return ((This*)this)->_eq(&cmp); }
	template<class Explicit> //daeURI_base or daeURI*->daeObject*
	/**CONST-FORM, LEGACY
	 * Comparison Operator
	 * @return Returns true if URI's are equal.
	 */
	inline bool operator==(Explicit &cmp)const{ return ((This*)this)->_eq(&cmp); }
		
	template<class Explicit> 
	/**LEGACY-SUPPORT */
	inline bool operator!=(Explicit &cmp){ return !operator==(cmp); }
	template<class Explicit> 
	/**CONST-FORM, LEGACY-SUPPORT */
	inline bool operator!=(Explicit &cmp)const{ return !operator==(cmp); }

	template<class Explicit> //daeURI_base
	/**LEGACY-SUPPORT
	 * Post-2.5 adds this for sorting? Anyway, == dangerous, and
	 * would be hard to justify without < (and >) supporting ops.
	 */
	inline bool operator>(Explicit &cmp)const{ return ((This*)this)->_gt(&cmp); }
	 
	template<class Explicit> //daeURI_base
	/**LEGACY-SUPPORT
	 * Post-2.5 adds this for sorting? Anyway, == dangerous, and
	 * would be hard to justify without < (and >) supporting ops.
	 */
	inline bool operator<(Explicit &cmp)const{ return ((This*)this)->_lt(&cmp); }		
};

//NOTE: 1 is equivalent to 0 but used as a dummy because
//IntelliSense cannot display whichever value is used by
//daeURI.h.
template<int size_on_stack=1>
/**ADVANCED
 * COLLADA C++ class that houses 3 kinds of strings in 1.
 *
 * @c daeRefString is a versatile string implementation that 
 * happens to be used by all of the internal @c daeRef based
 * classes. (Ie: @c daeURI, @c daeIDREF, and @c daeSIDREF.)
 *
 * This class must be encapsulated within an object that is
 * set up to supply a thunk via @c __DAEP__Object__v1__model().
 */
class daeRefString
{	
COLLADA_(public) //daeRefString is an encapsulated class.

	//This looks like it might be BIG-ENDIAN safe?
	//_string is last to cast to daeRefString<N>&.
	daeString _varray;	
	//_size_of_string isn't accessed whenever _isAU is 1.
	union{ const short _size_of_string; unsigned _isAU:1; };
	daeStringCP _string[size_on_stack>0?size_on_stack:1];										   
	/**
	 * When @c _isAU!=0, temporarily convert to an array,
	 * that must be undone afterward by setting @c _varray.
	 * @note It's not enough to use a pointer, because the 
	 * thunk @c _offset must agree with the array's address.
	 */
	inline daeArray<> &_varrayToArray()
	{	
		_varray-=daeOffsetOf(daeAlloc<daeStringCP>,_varray);
		return *(daeArray<>*)&_varray;
	}

COLLADA_(public) //CONSTRUCTORS
	/**
	 * Default Constructor
	 */
	daeRefString():COLLADA_SUPPRESS_C(4355)
	_size_of_string(short((size_t(this+1)-size_t(_string)))/sizeof(*_string))
	{
		_varray = _string; _string[0] = '\0';
	}
	/**
	 * View Constructor
	 */
	daeRefString(daeString view):COLLADA_SUPPRESS_C(4355)
	_size_of_string(short((size_t(this+1)-size_t(_string)))/sizeof(*_string))
	{
		_varray = view;
	}
	/** 
	 * Destructor
	 */
	~daeRefString(){ if(_isAU==1) _varrayToArray().~daeArray(); }

COLLADA_(public) //Accessors & Mutators
	/** 
	 * Gets the internal string.
	 */
	inline daeString getString()const{ return _varray; }	
	/** 
	 * This is for registering the thunk for the dynamic array.
	 */
	inline const daeString &getInternalStorage()const{ return _varray; }

	/** 
	 * @return Returns true if internal representation is @c daeString pointer. 
	 * @remarks Originally the plan was to use a @c daeStringRef and say "isRef"
	 * but this wasn't flexible enough. @c daeRefString is an encapsulated class.
	 * (It's meant to be @c private and never returned via any "getter" accessors.)
	 */
	inline bool isView(){ return _varray!=_string&&_isAU==0; }
	/**
	 * @return Returns true if internal representation is @c daeArray<daeStringCP>. 
	 */
	inline bool isArray(){ return _isAU==1; } 	
	/**
	 * @return Returns true if internal representation is an internalized @c daeString. 
	 */
	inline bool isString(){ return _varray==_string; }	

	/** 
	 * Sets @a ref as the internal string. 
	 * NOTE: @a ref IS NOT A @c daeStringRef. THE OUTER CLASS IS
	 * RESPONSIBLE FOR MANAGING @a ref BY WAY OF @c isView().
	 * NEITHER IS IT A "daeRefView."
	 */
	inline void setView(daeString ref)
	{
		if(_isAU==1){ _isAU = 0; _varrayToArray().~daeArray(); }	
		(daeString&)_varray = ref;
	}

	/**OVERLOAD
	 * Sets @a str or "" as the internal string, using @c strlen(). 
	 */
	inline void setString(const daeObject &c, daeString str)
	{
		if(str==nullptr) str = empty_daeString1;
		((daeRefString<0>*)this)->_setString(c,str,strlen(str)+1);
	}
	/**OVERLOAD
	 * Sets @a str as the internal string, where @c str[size-1]=='\0'. 
	 */
	inline void setString(const daeObject &c, daeString str, size_t size)
	{
		assert(size!=0&&'\0'==str[size-1]);	
		((daeRefString<0>*)this)->_setString(c,str,size);
	}
	/**OVERLOAD
	 * Sets @a str as the internal string, setting @c str[size-1] = '\0'. 
	 * @param zt This should be '\0' and is just illustrative/unambiguous.
	 */
	inline void setString(const daeObject &c, daeString str, size_t size, char zt)
	{
		assert(size!=0&&zt=='\0');	
		((daeRefString<0>*)this)->_setString(c,str,size); 
		const_cast<daeStringCP&>(_varray[size-1]) = '\0';
	}
	/**NEW, OVERLOAD
	 * This form leverages @c daeHashString. Note, the philosophy on "size"
	 * differs.
	 */
	inline void setString(const daeObject &c, const daeHashString &str)
	{
		setString(c,str.string,str.extent+1,'\0');
	}
	COLLADA_NOINLINE
	/**OVERLOAD
	 * "noinline" implementation of @c setString() via @c daeRefString<0>. 
	 */
	inline void _setString(const daeObject &c, daeString str, size_t size)
	{		
		switch(_isAU)
		{
		case 0: assert(size<65535/2&&_size_of_string>0);
			if((unsigned short&)size<=(unsigned short&)_size_of_string) 
			{
				_varray = _string; break;
			}
			_isAU = 1; //__DAEP__Object__v1__model() must have set up a thunk.
			_varray = ((daeAlloc<daeStringCP>&)c[&_varray].getAllocThunk())._varray; 
		case 1:
			daeArray<> &a = _varrayToArray();
			a.grow(size); _varray = a.getAU()->_varray;
		}memcpy((void*)_varray,str,size);		
	}
};

//NOTICE THE TEMPLATE ARGUMENTS ARE REVERSE OF daeArray/daeAlloc.
template<int size_on_stack=0, class T=daeContainedObject>
/**VARIABLE-LENGTH
 * This is a simple object for the legacy resolver caches to use.
 * At first it was going to expose @c daeObject::_reparent(), but
 * it turned out that this wasn't flexible enough, and so it's now
 * using @c _container instead of the parent, which can be anything.
 * @see @c daeContainedObject.
 */
class daeContainerObject 
:
//See daeURI_size or daeContents_size's explanation of this.
public COLLADA::NCOMPLETE<size_on_stack>::daeContainedObject
{
COLLADA_(protected)

	friend daeContainedObject;
	/**TODO, SHADOWING (ACCIDENTALLY)
	 * @a T will be based on @c daeContainedObject.
	 */
	daeArray<daeSmartRef<T>,size_on_stack> _contained;

COLLADA_(protected) //daeContainedObject method

	/**PURE-OVERRIDE */
	virtual void __daeContainedObject__v1__atomize(){ clear(); }

COLLADA_(public)
	/**
	 * Does @c __COLLADA__atomize() for each object contained,
	 * -but stops short of clearing the contained-object array.
	 * This is for clearing a list of cache-objects even though
	 * it doesn't expressly say so.
	 * At first @c __daeContainedObject__v1__atomize() was doing
	 * this, but __COLLADA__atomize() semantics are such that the
	 * container should be cleared in that case, since it's smart-
	 * ref based. (For a moment it was going to use raw-pointers.)
	 */	
	inline void atomizeContained()
	{
		for(size_t i=0;i<_contained.size();i++) 
		{
			_contained[i]->__COLLADA__atomize();		
		}
	}
	/**
	 * Gets the container in the form of an immutable array.
	 */	
	inline const daeArray<daeSmartRef<T>> &getContained()const
	{
		return _contained; 
	};

	typedef daeContainerObject<> _Vanilla;
	/**
	 * Returns @c this as @c daeContainerObject<>.
	 */
	_Vanilla *_plain_vanilla_this(){ return (_Vanilla*)this; }
	/**
	 * Adds an object to the container, if it's not already
	 * contained. If an other container contains the object,
	 * the object is first removed from the other container.
	 */	
	inline void contain(T *o, size_t ins=-1)
	{
		daeContainedObject *upcast = (T*)o; (void)upcast;
		_Vanilla *v = _plain_vanilla_this();
		if(v==o->_container) return;

		if(o->_container!=nullptr) 
		o->_self_remove();
			size_t sz = _contained.size();
			size_t mv = (sz-ins)*sizeof(o);
		_contained.push_back(o);		
			if(ins<sz)
			((T**)memmove(&_contained[ins]+1,&_contained[ins],mv))[-1] = o;		
		o->_container = v; 
	}

	/**
	 * Virtual Destructor
	 */
	virtual ~daeContainerObject()
	{
		clear(); //Letting compiler optimize.
	}
	/**
	 * The DOM must clear the resolvers list.
	 */
	inline void clear()
	{	
		for(size_t i=0;i<_contained.size();i++) 
		{
			_contained[i]->__COLLADA__atomize();
			_contained[i]->_container = nullptr;
		}		
		_contained.clear();
	}
};
/**ABSTRACT-BASE CLASS
 * This is a simple object for the legacy resolver caches to use.
 * At first it was going to expose @c daeObject::_reparent(), but
 * it turned out that this wasn't flexible enough, and so it's now
 * using @c _container instead of the parent, which can be anything.
 * @see @c daeContainerObject.
 */
class daeContainedObject : public daeObject
{
COLLADA_(public)
	/**
	 * Initiates "teardown" sequence. 
	 */
	inline void __COLLADA__atomize()
	{
		__daeContainedObject__v1__atomize();
	}

	/**
	 * Constructor
	 */
	daeContainedObject(const DAEP::Object *c):daeObject(c)
	,_container(){}
	/**
	 * Default Constructor
	 */
	daeContainedObject():COLLADA_SUPPRESS_C(4355)daeObject(this)
	,_container(){}	

COLLADA_(protected) //Virtual method
	/**PURE
	 * @c __COLLADA__atomize() will call this method.
	 */
	virtual void __daeContainedObject__v1__atomize() = 0;
	/**RESERVING
	 * Reserving. In theory. Is this useful?
	 */
	virtual daeError __daeContainedObject__reserved(){ return DAE_ERR_NOT_IMPLEMENTED; }

COLLADA_(public) //DAEP Object methods

	/**PURE-OVERRIDE 
	 * This is just a dummy. It's implemented really just
	 * because @c daeContainerObject is a template and so
	 * there should be a "daeContainerObject_base" for it
	 * to have a single font for its models, or it should
	 * specialize the models. It's less trouble like this.
	 * It also means the resolvers/caches can let this go.
	 */
	virtual DAEP::Model &__DAEP__Object__v1__model()const
	{
		return getDefaultProcessShare().getOpaqueModel();
	}

COLLADA_(protected) //DATA-MEMBER

	template<int,class> friend class daeContainerObject;
	/**
	 * The contained object is contained inside @c _container.
	 * There's just too many issues that arise from assuming the
	 * parent-object is the container. Sub-objects cannot be reparented.
	 *
	 * ZAE
	 * In order to form a parental link from @c daeImage to @c daeAtlas
	 * this member (@c _container) is made into a "weak" reference back
	 * to the parent-like container. The trouble is @c daeAtlas doesn't
	 * have implicit ownership semantics, and so can't be atomized, say
	 * when a @c daeArchive is closed.
	 * //mutable daeSmartRef<daeContainerObject<>> _container;
	 */
	mutable daeContainerObject<> *_container;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeContainedObject)

COLLADA_(public) //Various
	/**MUTABLE
	 * Code can freely set this if it knows the outermost type of
	 * its object. It's just free memory to store a simple status.
	 * Use @c getObjectTags().objectTag to retrieve the set value.
	 */
	inline void setObjectTag(unsigned char i)const
	{
		_getObjectTags().objectTag = i;
	}

	/**MUTABLE
	 * Gets the container object containing @c this, or @c nullptr.
	 */	
	inline daeContainerObject<> *getContainer()const{ return _container; };

COLLADA_(protected) //Internal	
	/**OVERLOAD
	 * Removes this object from its container. It's not exposed, so base
	 * classes can define their own semantics.
	 */
	inline void _self_remove()const
	{
		if(_container!=nullptr)
		_container->_contained.remove(this); _container = nullptr;
	}
	/**OVERLOAD
	 * Removes this object from its container. It's not exposed, so base
	 * classes can define their own semantics.
	 */
	inline void _self_remove(size_t i)const
	{
		assert(_container->_contained[i]==this);
		_container->_contained.removeIndex(i); _container = nullptr;
	}
};

/**ABSTRACT-BASE-CLASS
 * The @c daeRefResolver class is the plugin point for URI-like ref resolution.
 * This class is an abstract base class that defines an interface for
 * resolving URI-like refs. (Or, a character string, that is a link, to an object.)
 * Every @c daeRef is passed through this list of @c daeRefResolver for resolution.
 * The list is ordered on a first come, first serve basis, and resolution
 * terminates after any resolver instance resolves the URI.
 */
class daeRefResolver : public daeContainedObject
{	
COLLADA_(private) //Virtual method

	friend class daeRefResolverList;
	/**PURE
	 * Provides an abstract interface for converting a @c daeRef into a @c daeObject.
	 * @param ref @c daeRef to resolve.
	 * @return Returns the resolved object, or null if resolving failed.
	 * returns false otherwise.
	 */
	virtual daeError _resolve(const daeRef &ref, daeRefRequest &req)const = 0;	

COLLADA_(private) //DATA-MEMBER
	/**
	 * Corresponds to @c getRefType().
	 */
	const int _refType;

COLLADA_(public) //Various
	/**
	 * Default Constructor
	 */
	explicit daeRefResolver(int type):_refType(type){}
	/**
	 * Virtual Destructor
	 */
	virtual ~daeRefResolver(){}

	/**
	 * One of @c daeRefType, or negative if registered as a client type.
	 * @note This is fast access for iterating over the RefResolverList.
	 */
	inline int getRefType()const{ return _refType; }

	/**
	 * Gets the name of this resolver.
	 * @return Returns the resolver name as a string.
	 *
	 * @remarks This name isn't necessarily pretty/friendly. If
	 * it is lexically identical, the implementations must be so.
	 * @see @c isSame().
	 */
	virtual daeClientString getName()const = 0;
	/**ILLUSTRATING
	 * @return Returns @c true if the resolvers can be considered identical.
	 * @see @c is() template for a more commonly useful approach.
	 */
	inline bool isSame(const daeRefResolver &cmp)const
	{
		return _refType==cmp._refType&&!strcmp(getName(),cmp.getName());
	}	
	/**
	 * Static version of @c isSame(). (It usually makes more sense to do this.) 
	 */
	template<class T> inline bool is()const
	{
		return 0==strcmp(getName(),T::getRefResolverNameString()); 
	}
	
	/**LEGACY
	 * Provides an abstract interface for converting a @c daeRef into a @c daeElement.
	 * @param ref @c daeRef to resolve.
	 * @return Returns the resolved element, or null if resolving failed.
	 * returns false otherwise.
	 */
	inline daeElementRef resolveElement(const daeRef &ref)const
	{
		if(_refType!=ref.getRefType())
		if(_refType!=daeRefType::undifferentiated)		
		return nullptr;
		daeRefRequest req;
		if(DAE_OK==_resolve(ref,req)&&req.object->_isElement())
		return (daeElementRef&)req.object;
		return nullptr;
	}

	/**
	 * Use this to remove the resolver from its resolver-list.
	 */
	inline void unlist(){ _self_remove(); }

COLLADA_(private) 
	/**
	 * Disabled Copy Constructor
	 * As @c private, derived classes will error if copy-constructed.
	 */
	daeRefResolver(const daeRefResolver&):_refType(){}
	/**
	 * Disabled Assignment Operator
	 * As @c private, derived classes will error if ever assigned to.
	 */
	void operator=(const daeRefResolver&){}
};		 

/**
 * Container for storing a modifiable list of @c daeRefResolver objects.
 *
 * @remarks @c daeContainerObject isn't strictly necessary in this case,
 * but their code is very similar, and should probably evolve alongside
 * one another. What's not required is A) it being object based, and B)
 * it being a parent to its resolvers. @c daeContainerObject exists for
 * the result caches, and is only halfheartedly repurposed in this case.
 */
class daeRefResolverList : public daeContainerObject<10,daeRefResolver>
{
COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeRefResolverList)

COLLADA_(public) 
	/**
	 * Default Constructor & Virtual Destructor
	 * @todo Provide a path to parent the object?
	 */
	daeRefResolverList(){} ~daeRefResolverList(){}

	//FYI: This could be the wrong idea with time, since "atomize"
	//really just means "prepare to be destroyed," but for now, it
	//takes for granted that the resolvers just clear cache-objects.
	/**NOT-RECOMMENDED
	 * This is probably useless in practice, but should work to clear
	 * all cached results. Note, any modification to any part of a DOM
	 * (any document, element, etc.) will clear the caches just as well.
	 * IF THAT IS, THE NOTICES-OF-CHANGE ARE NOT SUPPRESSED-NOR-BYPASSED.
	 */
	inline void clearResolverResults(){ atomizeContained(); }

	/**
	 * This humble method returns @c DAE_OK or @c DAE_ERR_QUERY_NO_MATCH.
	 */
	inline daeOK resolve(const daeRef &ref, daeRefRequest &req)const
	{
		int t,rt = ref.getRefType();		
		for(size_t i=0;i<_contained.size();i++)
		{
			if(t=_contained[i]->getRefType()) //undifferentiated?
			if(t!=rt) 
			continue;
			if(DAE_OK==_contained[i]->_resolve(ref,req))
			return DAE_OK;
		}
		return DAE_ERR_QUERY_NO_MATCH;
	}

	/**LEGACY
	 * Provides an abstract interface for converting a @c daeRef into a @c daeElement.
	 * @param ref @c daeRef to resolve.
	 * @return Returns the resolved element, or null if resolving failed.
	 * returns false otherwise.
	 */
	inline daeElementRef resolveElement(const daeRef &ref)const
	{
		daeRefRequest req;
		if(DAE_OK==resolve(ref,req)&&req.object->_isElement())
		return (daeElementRef&)req.object;
		return nullptr;
	}

	//Maybe daeRefResolversList::List is not ideal, but it's short.
	typedef const daeArray<daeSmartRef<const daeRefResolver>> List;
	/**LEGACY-SUPPORT
	 * Post-2.5 this is a @c const only API. Some basic APIs
	 * for manipulating the resolvers-list are newly provided.
	 */
	inline List &list()const
	{
		return (List&)daeContainerObject::getContained(); 
	}	
	/**LEGACY-SUPPORT
	 * Equivalent to older list().removeIndex(). 
	 */
	inline void list_remove(size_t i)
	{
		_contained[i]->_self_remove(i);
	}	
	/**LEGACY-SUPPORT
	 * Equivalent to older list().remove(). 
	 */
	inline void list_remove(daeRefResolver *p)
	{
		assert((void*)p->_container==this); p->unlist();	
	}
	/**LEGACY-SUPPORT
	 * Equivalent to the old list().append(). 
	 * @note @c daeArray::push_back() is now
	 * preferred for adding a single element.
	 * @return The return is a hack in order
	 * to chain @c __DAEP__Object__unembed().
	 */
	inline daeRefResolver *list_append(daeRefResolver *p)
	{
		daeContainerObject::contain(p); assert(_contained.back()==p); 
		return p;
	}	
	/**LEGACY-SUPPORT
	 * Equivalent to older list().insertAt(). 
	 * @return The return is a hack in order
	 * to chain @c __DAEP__Object__unembed().
	 */
	inline daeRefResolver *list_insert(size_t i, daeRefResolver *p)
	{
		if(list_append(p)==_contained.back()&&i<_contained.size()-1) 
		{	
			//HACK: list_append() has additional parenting logic.
			//This is swapping the smart-refs, as if plain pointers.
			memmove(&_contained[i]+1,&_contained[i],(_contained.size()-1-i)*sizeof(void*));
			(daeRefResolver*&)_contained[i] = p;
		}
		return p;
	}
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_REF_RESOLVER_H__
/*C1071*/
