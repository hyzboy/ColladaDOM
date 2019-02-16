/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_SMART_REF_H__
#define __COLLADA_DOM__DAE_SMART_REF_H__

#include "daeRefCountedObj.h"

COLLADA_(namespace)
{//-.
//<-'		
	
class daeSmartRef_base
{					
	//RESERVED
	//Short-circuiting built-in interaction with T* operator.
	void operator[](int)const{}

	union
	{
		/**
		 * The pointer to the element which is being reference counted. 
		 */
		daeObject *_ptr;

		daeNullable<> _nptr;
	};		

COLLADA_(public) //QUIRKS
	/**WARNING
	 * Compacting ~ operator
	 *
	 * This quirky operator relates to destructor ~notation.
	 * It doesn't do @c nullptr checks. The pointer mustn't
	 * be @c nullptr.
	 * 
	 * @c operator~ is used to compact @c daeDOM::_closedDocs.
	 * @return Returns @c true if the reference was released as 
	 * a result.
	 * @warning It's dangerous to use this strategy, because if two
	 * holders do so--and conditions do not change--they are assured
	 * to deadlock.
	 * THE WISDOM (OR LACK THEREOF) OF USING ~ FOR THIS DEBATABLE.
	 */
	inline bool operator~(){ return _ptr->_compact()&&(_ptr=nullptr,true); }

COLLADA_(protected) //INITIALIZATION
	/**
	 * Default Constructor
	 */
	inline daeSmartRef_base() : _ptr(nullptr){}	
	/**
	 * Destructor
	 */
	inline ~daeSmartRef_base(){ _nptr._release(); }
	 
	inline void _init(const DAEP::Object *cp) /** Sees T is DAEP::Object. */
	{			
		_ptr = (daeObject*)cp; _nptr._ref();
	}
	inline void _assign(const DAEP::Object *cp) /** Sees T is DAEP::Object. */
	{		
		daeNullable<> nptr = {(daeObject*)cp}; nptr._ref();

		_nptr._release(); _ptr = (daeObject*)cp;
	}

COLLADA_(public) //COLLADA_(protected)

	//The __DAEP__Legacy form can't use T from daeSmartRef<T>.
	//Writing C++ is tortured.

	template<class T, class U>
	static T *_decay(U &nonptr,...){ return nonptr; }
	template<class T, class U>
	static T *_decay(U *ptr,...){ T &lateral_cast = *ptr; return &lateral_cast; }
	template<class T, class U>
	static T *_decay(U &nonptr,char(*)[U::__COLLADA__T::__DAEP__Legacy-T::__DAEP__Legacy])
	{
		typename daeConstOf<T,DAEP::Element>::type *p = nonptr; return (daeElement*)p; 
	}
};

template<class>
struct daeSmartTagOf{ typedef class daeSmartTag type; };
template<class T>
struct daeSmartTagOf<const T>{ typedef const_daeSmartTag type; };

template<class T>
//2.5 SIGNIFICANTLY RELAXES daeSmartRef TO MAKE IT BROADLY MORE
//AVAILABLE FOR INTERNAL USE, BY BREAKING CYCLICAL DEPENDENCIES.
//AS SUCH IT ACCEPTS ANY KIND OF T FOR PURPOSES OF CONSTRUCTION.
/**
 * The @c daeSmartRef template class automates reference counting for
 * objects derived from @c daeObject (previously daeRefCountedObj.)
 *
 * REQUIREMENTS
 * ============
 * This class should act like a pointer. It shouldn't overload operators
 * in such a way that makes it behave differently from a pointer. 
 * (Not even to get rid of nagging compiler warnings.)
 */
class daeSmartRef : public daeSmartRef_base
{	
COLLADA_(public) //QUIRKS	

	/**WORKAROUND
	 * @c __COLLADA__T is a "loopback" mechanism.
	 * It cannot be @c __COLLADA__Element because @a T is an incomplete type.
	 * So, @c daeSmartRef<T>::__COLLADA__T::__COLLADA__Element is
	 * the same as @c T::__COLLADA__T::__COLLADA__Element; and so templates can
	 * this way work with @c T or @c daeSmartRef<T> transparently.
	 *
	 * @c __COLLADA__POD indicates @c this wraps plain-old-data.
	 */
	typedef T __COLLADA__T;

	/**DAEP (ELEMENTS-ONLY)
	 * Implements DAEP QName accessor.
	 * Below are some benign examples.
	 * COLLADA->*xs::NCName() = "COLLADA";
	 * xs::NCName ncn = COLLADA->*xs::NCName();
	 * xmlns::tuple qn = COLLADA->*xs::QName();	
	 * //Many ways to do the same thing:
	 * xmlns dae = COLLADA->*xs::QName();	 
	 * xmlns dae = xmlns("dae","http://www.collada.org/2008/03/COLLADASchema");	 
	 * xmlns dae = qn.first; dae.prefix("dae");
	 * COLLADA->*xs::QName() = dae("COLLADA"); //<dae:COLLADA></dae:COLLADA>
	 * //Copy the returned object:
	 * xs::const_complexType ct = COLLADA->*xs::complexType();
	 * const xs::complexType&ct = COLLADA->*xs::complexType();
	 */
	inline const typename daeSmartTagOf<T>::type &operator->*(daeString)const
	{
		const daeElement *uc = dae((T*)nullptr);
		(void)uc;
		return *(typename daeSmartTagOf<T>::type*)this;
	}

COLLADA_(public) //daeArray traits

	typedef const daeObject __COLLADA__Object;
	
	inline void __COLLADA__place(const daeObject *obj, const daeSmartRef &cp)
	{
		new(this) daeSmartRef(cp); 
	}
	static void __COLLADA__move(const daeObject *obj, daeSmartRef *lv, daeSmartRef *rv, size_t iN)
	{
		memcpy(lv,rv,iN*sizeof(daeSmartRef)); //raw-copyable style move
	}		

COLLADA_(public) //Factory constructor

	template<class U>
	/**EXPERIMENTAL
	 * Unified/factory constructor
	 */
	daeSmartRef(U &cp){ _init(_decay_or_add(cp,nullptr)); }
	template<class U>
	/**EXPERIMENTAL
	 * Unified/factory constructor
	 */
	daeSmartRef(const U &cp){ _init(_decay_or_add(cp,nullptr)); }
	template<class U>
	static T *_decay_or_add(U &other,...){ return _decay<T>(other,nullptr); }
	template<class U>
	static T *_decay_or_add(U &DOM,typename U::__DAEP__Object__signed*)
	{
		COLLADA_INCOMPLETE(T) daeDOM &i = DOM;
		return  i.template _add<typename daeConstOf<int,T>::type>();
	}
		
COLLADA_(public) //Non-factory constructors
	/**C++
	 */
	daeSmartRef(){}

	/**C++
	 * Copy Constructor
	 * @param smartRef a daeSmartRef of the same template type to copy from
	 */
	daeSmartRef(const daeSmartRef &cp){ _init((T*&)cp); }
	
COLLADA_(public) //OPERATORS
	/**C++
	 * Overloaded assignment operator.
	 * @param other a daeSmartRef to the object to copy from.  Must be of the same template type.
	 * @return Returns a reference to this object.
	 */
	inline daeSmartRef &operator=(const daeSmartRef &cp)
	{
		_assign((T*&)cp); return *this;
	}

	template<class U>
	/**EXPERIMENTAL
	 * Unified assignment operator
	 */
	inline daeSmartRef &operator=(U &cp)
	{
		_assign(_decay<T>(cp,nullptr)); return *this;
	}
	template<class U>
	/**EXPERIMENTAL
	 * Unified assignment operator
	 */
	inline daeSmartRef &operator=(const U &cp)
	{
		_assign(_decay<T>(cp,nullptr)); return *this;
	}

	/**
	 * Overloaded member selection operator.
	 * @return a pointer of the template class to the object.
	 */
	inline T *operator->()const
	{
		//Disabling this assert so the new deferred dereferencing
		//feature set can be used with ->.
		//dae_Array::operator->() and (Type*) conversion operator,
		//DAEP::InnerValue::operator->*()
		//assert(nullptr!=_ptr); 
		return *(T**)this;
	}

	/**
	 * Overloaded cast operator.
	 * @return Returns an @a T pointer.
	 */
	inline operator T*()const{ return *(T**)this; }
	
	/**
	 * Overloaded dereference operator.
	 * @return Returns an @a T reference.
	 */
	inline T &operator*()const{ return **(T**)this; }
		   
COLLADA_(public) //DEPRECATIONS

	#ifndef COLLADA_NODEPRECATED
	/**
	 * Function that returns a pointer to object being reference counted.
	 * @return the object being reference counted.
	 */
	inline T *cast()const{ return *(T**)this; }
	//SCHEDULED FOR REMOVAL
	//(seems very awkward to do)
	template<class U> //BOTH class T & U MUST BE DEFINED.
	/**LEGACY, UNUSED (INTERNALLY)
	 * Static cast function.
	 * @param smartRef a smartRef to cast from
	 * @return a pointer to an object of this template class.
	 */	
	inline static T *staticCast(const daeSmartRef<U> &smartRef)
	{
		return &static_cast<T&>(*smartRef);
	}
	#endif //COLLADA_NODEPRECATED
};

/**BIG-ENDIAN FACILITY
 * 32-bit structure that overlays with @c daeObjectTags.
 *
 * If two elements have equal tags, then you can assume several
 * things about them:
 *
 * A) They belong to the same modular XML namespace.
 * B) They have the same colon (:) based XML prefix: E.g. <xs:element>.
 * C) The virtual-table layout is identical. This one's for librarians.
 * D) They hail from the same client module; That is hosting the space.
 */
class daeElementTags
{
COLLADA_(public) //ACCESSORS
	/**
	 * Sets to "empty" XML namespace.
	 *
	 * @note This can't be done to elements' tags.
	 * But they should be @c const.
	 */
	inline void clear(){ *this = 0x100; }	

	/**
	 * Signals "empty" XML namespace.
	 */
	inline bool empty()const
	{
		return 0==namespaceTag&&1==nameTag;

	}

	/**
	 * Signals @c daeElement::findNamespace_xmlns()
	 * success.
	 */
	inline bool found()const{ return 0!=nameTag; }

	/**
	 * Gets XML namespace of XML Schema association.
	 * @see @c daeElement::getNamespace()
	 */
	inline daeStringRef_base first()const
	{
		return daeStringRef_xmlns[namespaceTag][0];
	}

	/**
	 * Gets xmnls attribute QName style prefix code.
	 * @see @c daeElement::getQNamePrefix()
	 */
	inline daeStringRef_base second()const
	{
		return daeStringRef_xmlns[namespaceTag][nameTag];
	}

COLLADA_(public) //OPERATOR
	/**
	 * Casts to the type of @c __DAEP__Object__tags.
	 * See struct-level Doxygentation for rationale.
	 */
	inline operator unsigned int()const{ return *(unsigned int*)this; }

COLLADA_(public) //daeObjectTags members
	/**BITFIELD
	 * These correspond to @c daeObjectTags except for @c namespaceTag
	 * and @c nameTag that identify an xmlns namespace and prefix pair.
	 * Taken as a whole, if the 32-bit value is equal, an element must
	 * share the same prefix. It is also true for just the 16-bit pair.
	 *
	 * @note In theory if a namespace sees than 255 prefixes, it might
	 * be possible to overflow into a new value for @c namespaceTag in
	 * order to accommodate 254 more values at a time. 
	 */
	unsigned int namespaceTag:8,nameTag:8,interfaceTag:8,moduleTag:8;

COLLADA_(public) //MUTATORS
	/**
	 * Default Constructor
	 */
	daeElementTags(unsigned int i=0){ *(unsigned int*)this = i; }

	COLLADA_DOM_LINKAGE
	/**WARNING
	 * Adds namespace and prefix pair to global table, assigning index
	 * to @c namespaceTag and @c nameTag respectively.
	 * @note The values of @c namespaceTag is taken as a hint in order
	 * to accelerate look ups. It's used in place of the 1st parameter
	 * if @c nullptr. (Assign @c dae_append to do this trick.) 
	 *
	 * @warning Both parameters are system-pool string-refs. If either
	 * is not or are too large (currently 126B) it is considered fatal
	 * error... but can be recovered with @c found(). The limit can be
	 * increased. Large strings are subject to ref-counting and aren't
	 * added to the lookup-table, since they are expected to be unique.
	 *
	 * @return Upon return these tags will index @c daeStringRef_xmlns.
	 * The remaining 2 tags are unused/inherited from @c daeObjectTags.
	 *
	 * @c daeStringRef_base is kind of a pain, but is the same type as
	 * the returned values, and is able to work with default arguments. 
	 */
	void assign(daeStringRef_base,daeStringRef_base=dae_default);

	/**OPTIMIZATION Eliminate @c ~daeStringRef() code. */
	static daeStringRef_base _make_base(const daeName &n)
	{
		void *hack; return *new(&hack)daeStringRef(n);
	}

	/**OPTIMIZATION
	 * Change the XMLNS prefix. This is more a feature for @c daeXMLNS.
	 * This is a little more optimal on the front and back ends if you
	 * have to produce a new prefix from an existing @c daeElementTags.
	 */
	inline class daeXMLNS &prefix(const daeName &pf)
	{
		assign(dae_append,_make_base(pf)); return *(daeXMLNS*)this;
	}
};

/**HELPER
 * This class simplifies @c daeElementTags construction. It's best for
 * when an XML QName prefix and namespace pair is to be reused as if a
 * constant value.
 *
 * If constructed with one string, that string is the namespace having
 * no QName prefix. If not it's important to realize the order differs
 * from @c daeElementTags::assign(). If entered incorrectly the global
 * table values cannot be recovered.
 */
class daeXMLNS : public daeElementTags
{
COLLADA_(public)
	/**C++
	 * Default Constructor
	 */
	daeXMLNS(unsigned int i=0):daeElementTags(i){}
	/**
	 * Constructor following xmlns attribute convention placing prefix
	 * on left. This constructor omits the empty prefix.
	 */
	explicit daeXMLNS(const daeName &first)
	{
		assign(_make_base(first)); 
	}
	/**AMBIGUOUS
	 * Copies tags instead of using the NCName as the namespace string.
	 */
	explicit daeXMLNS(const daeSmartTag_base &i)
	{
		*(daeObjectTags*)this = (*(daeObject**)&i)->getObjectTags();		
	}
	/**
	 * Constructor following xmlns attribute convention placing prefix
	 * on left and namespace right. 
	 */
	daeXMLNS(const daeName &second, const daeName &first)
	{
		assign(_make_base(first),_make_base(second));
	}

COLLADA_(public) //daeSmartTag and COLLADA::xmlns support
	/**
	 * Implements DAEP @c xmlns::tuple object.
	 */
	typedef std::pair<daeXMLNS,daeName> tuple;
	/**
	 * Creates a tuple object for @c daeSmartTag::operator=() consumption.
	 */
	inline tuple operator()(const daeName &cp)const{ return tuple(*this,cp); }
};

/**DAEP
 * Implements @c operator->*(daeString) of various classes.
 * @see @c daeSmartRef, @c dae_Array / @c DAEP::InnerChild.
 */
class daeSmartTag_base : public daeString_support<daeSmartTag_base>
{
COLLADA_(protected)

	union{ daeObject *_o; DAEP::Element *_e; };

	inline daeName &_name()const
	{	
		return _e->__DAEP__Element__data.NCName;
	}
	inline daeXMLNS &_tags()const
	{	
		return *(daeXMLNS*)&_o->_getObjectTags();
	}

COLLADA_(public) //CONSTRUCTOR/DESTRUCTOR
	/**
	 * Default constructor
	 */
	daeSmartTag_base(){ _e = nullptr; }
	/**
	 * @c daeSmartRef like copy constructor
	 */
	daeSmartTag_base(const daeSmartTag_base &cp)
	{
		new(this) daeElementRef((daeElement*)cp._e);
	}
	/**
	 * @c daeSmartRef like assignment operator
	 */
	daeSmartTag_base &operator=(const daeSmartTag_base &cp)
	{
		*(daeElementRef*)this = (daeElement*)cp._e; return *this;
	}

	/**
	 * @c daeSmartRef like destructor
	 */
	~daeSmartTag_base(){ ((daeElementRef*)this)->~daeSmartRef(); }

COLLADA_(public) //daeStringSupport implementation

	inline size_t size()const
	{
		assert(_e!=nullptr); return _name().extent; 
	}

	inline bool empty()const
	{
		assert(_e!=nullptr); return 0==_name().extent; 
	}

	inline daeString data()const
	{
		assert(_e!=nullptr); return _name().string; 
	}

COLLADA_(public) //ACCESSORS

	inline const daeXMLNS first()const
	{
		assert(_e!=nullptr); return _tags();
	}

	inline const daeName second()const
	{
		assert(_e!=nullptr); return _name();
	}	

COLLADA_(public) //OPERATORS	
	/**
	 * Basic @c daeSmartRef like support.
	 */
	inline const daeElement *operator->()const
	{
		return (daeElement*)_e;
	}

	template<class T>
	/**
	 * Basic @c daeSmartRef like support.
	 *
	 * @note @c explicit style construction
	 * works for @c daeSmartRef, @c xs::any.
	 */
	inline operator T*()const
	{
		T *p = (const daeElement*)_e; return p;
	}
	/**SPECULATIVE
	 * Converts to @c xmlns, AKA @c daeXMLNS.
	 */
	inline operator daeXMLNS()const
	{
		return _e!=nullptr?_tags():daeXMLNS(); 
	}	
	/**SPECULATIVE
	 * Converts to @c xs::string_view.
	 * @todo Can use @c template to support more
	 * kinds of strings.
	 */
	inline operator daeHashString()const
	{
		return _e!=nullptr?_name():empty_daeString1; 
	}
	/**SPECULATIVE
	 * Converts to @c xs::string, @c xs::NCName, etc.
	 */
	inline operator daeString()const
	{
		return _e!=nullptr?_name().string:empty_daeString1; 
	}
	/**SPECULATIVE
	 * Converts to @c xmlns::tuple, AKA @c daeXMLNS::tuple.
	 */
	inline operator daeXMLNS::tuple()const
	{
		daeXMLNS::tuple o(0,empty_daeString1);
		if(_e!=nullptr) o.first = _tags();
		if(_e!=nullptr) o.second = _name(); return o;
	}
};
/**DAEP
 * Implements @c operator->*(daeString) of various classes.
 * @see @c daeSmartRef, @c dae_Array / @c DAEP::InnerChild.
 */
class daeSmartTag : public daeSmartTag_base
{
COLLADA_(public) //OPERATORS
	/**
	 * Basic @c daeSmartRef like support.
	 */
	inline daeElement *operator->()const
	{
		return (daeElement*)_e;
	}

	template<class T>
	/**
	 * Basic @c daeSmartRef like support.
	 *
	 * @note @c explicit style construction
	 * works for @c daeSmartRef, @c xs::any.
	 */
	inline operator T*()const
	{
		T *p = (daeElement*)_e; return p;
	}
	/**
	 * Converts to @c xmlns, AKA @c daeXMLNS.
	 */
	inline operator daeXMLNS()const
	{
		assert(_e!=nullptr); return _tags(); 
	}
	/**TODO
	 * Converts to @c xs::string_view.
	 * @todo Can use @c template to support more
	 * kinds of strings.
	 * @remark Hiding @c daeSmartTag_base version.
	 */
	inline operator daeHashString()const
	{
		assert(_e!=nullptr); return _name(); 
	}
	/**
	 * Converts to @c xs::string, @c xs::NCName, etc.
	 */
	inline operator daeString()const
	{
		assert(_e!=nullptr); return _name().string; 
	}
	/**
	 * Converts to @c xmlns::tuple, AKA @c daeXMLNS::tuple.
	 */
	inline operator daeXMLNS::tuple()const
	{
		COLLADA_ASSUME(_e!=nullptr)
		return daeSmartTag_base::operator daeXMLNS::tuple(); 
	}	

COLLADA_(private) //MUTATORS
	/**DISABLED
	 * Ordinarily this would copy @c data() and @c size() into
	 * @c operator->().getNCName().
	 */
	inline const daeSmartTag &operator=(const daeSmartTag&)const
	{
		assert(0); return *this;
	}

COLLADA_(public) //MUTATORS
	/**
	 * @c daeSmartRef like assignment operator
	 */
	inline daeSmartTag &operator=(const daeSmartTag &cp)
	{
		daeSmartTag_base::operator=(cp); return *this;
	}

	#ifdef COLLADA_INITIALIZER_LIST
	#ifdef COLLADA_MSVC2013
	template<class daeName> //MSVC2015 works without template.
	#endif
	/**EXPERIMENTAL
	 * Assigns either {"prefix"} or {"prefix","NCName"} by way 
	 * of @c daeElementTags::prefix. The schema's namespace is
	 * unmodified.
	 *
	 * @remark This is recommended to avoid the possibility of
	 * assigning an element to a namespace different from their
	 * @c XS::Schema namespace. daeMetaElement.cpp considers the
	 * parent's namespace to decide to try to switch the schemata.
	 * If the parent only appears to match important processing is
	 * skipped.
	 */
	inline const daeSmartTag &operator=(std::initializer_list<daeName> QName)const
	{
		assert(_e!=nullptr); 
		assert(QName.size()<=2);

		if(QName.size()>0) _tags().prefix(QName.begin()[0]);
		if(QName.size()>1) _name() = QName.begin()[1]; 
		
		return *this;		
	}
	#endif

	template<class T>
	/**
	 * Assigns @a NCName part of @c this element's QName tuple.
	 */
	inline const daeSmartTag &operator=(const T &NCName)const
	{
		assert(_e!=nullptr); _name() = NCName; return *this;
	}
	/**
	 * Assigns namespace/prefix pair part of @c this element's
	 * QName tuple.
	 */
	inline const daeSmartTag &operator=(const daeXMLNS &xmlns)const
	{	
	  //REMINDER: setNamespace can also set the namespace with
	  //identical code. There's a BIG PROBLEM for schema-based
	  //elements if the namespaceTag does not match the schema.
	  //It may be that this is just a bad design. An assert on
	  //that case doesn't do much to help, since it can happen
	  //anywhere, in algorithms that might not be fully tested.

		assert(_e!=nullptr);

		//Note: 0 indicates findNamespace_xmlns mismatch.
		assert(xmlns.found());
		//Note: In x86 ideally this copies a 16-bit word.
		_tags().namespaceTag = xmlns.namespaceTag;
		_tags().nameTag = xmlns.nameTag;	

		return *this;
	}
	/**
	 * Assigns namespace/prefix pair and NCName parts of @c this 
	 * element's QName tuple.
	 */
	inline const daeSmartTag &operator=(daeXMLNS::tuple QName)const
	{
		return operator=(QName.first).operator=(QName.second);
	}
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_SMART_REF_H__
/*C1071*/
