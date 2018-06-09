/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_META_ATTRIBUTE_H__
#define __COLLADA_DOM__DAE_META_ATTRIBUTE_H__

#include "DAEP.h"
#include "daeAtomicType.h"

COLLADA_(namespace)
{//-.
//<-'
				
/**NEW/ROUGH
 * This is @c xs::anySimpleType from daeDomTypes.h.
 * It is used (internally) by attributes that come
 * without an XML Schema.
 */
class daeAST::TypedUnion
{	
	friend class daeData;
	friend class daeMetaElement;
	friend class daeAnySimpleTypewriter;
	friend class daeAnySimpleType::Data;	

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;

COLLADA_(private) //DATA
	/**
	 * This is the same offset as @c daeDefault.
	 * It would belong to @c daeData except for 
	 * 32-bit alignment issues, since @c _union
	 * requires 64-bit alignment.
	 */
	daeOffset _offset;

	/**
	 * A @c daeAnySimpleTypewriter typewriter.
	 */
	daeAnySimpleTypewriter *_type;

	/**8B-ALIGNMENT
	 * Only these types are allowed. They are
	 * being set by @c daeAnySimpleTypewriter
	 * but it'd be better if a TypedUnion API
	 * did so.
	 */
	union Union
	{
		daeString string;
		int _int;		
		//Large values may be assigned
		//to hexBinary.
		long long int _long; 
		//float _float;
		double _double;
		//0 and 1 become int or double.
		bool boolean;		
		//NOTE: Short binary sequences 
		//are assigned to string.
		struct Binary
		{
			int header[2];
			daeAlloc<char,64> *AU;  
		}hexBinary;	
		//Note: Data uses this, however
		//it cannot be an element of an
		//xs:list from xs:anySimpleType
		daeAlloc<> *list; 

	}_union;

	/** Implements operator=(). */
	COLLADA_DOM_LINKAGE void _copy(const TypedUnion&);

	/** Prototype constructs arrays. */
	COLLADA_DOM_LINKAGE void _prototype_construct();

COLLADA_(public)
	/**
	 * @c daeArray Constructor and Destructor
	 */
	COLLADA_DOM_LINKAGE TypedUnion(),~TypedUnion();
	 	
	template<class Type> //DAEP::Value<...>
	/**
	 * Prototype Constructor
	 *
	 * This constructor transfers the schema's default strings
	 * to the database string-ref pool. The empty-string cases
	 * go along an @c inline pathway. Default-strings go along
	 * a @c COLLADA_DOM_LINKAGE path.
	 */
	explicit TypedUnion(const DAEP::Proto<Type> &pt)
	{	
		//REMINDER: Even if you believe xsAnySimpleType
		//with default is uncommon or not worth support
		//domAny still requires its string to be set to
		//the owner DOM's empty string.
		const COLLADA_INCOMPLETE(Type)
		daeDOM *DOM = dae(pt.object()).getDOM();		
		if(pt.has_default||pt.is_fixed)
		{
			//Reminder: This is compiled only if schema
			//have a default.
			switch(_type->getAtomicType())
			{		  
			case daeAtomicType::BINARY: //xs:hexBinary 
				
				//daeDefault::_setDefaultString is going
				//to some length to convert hexBinary to
				//STRING.
				assert(0);
				break;
				
			case daeAtomicType::EXTENSION: //xs:list
			
				//If the default is an array, it will have
				//a space in its default string, otherwise
				//the compiler can elide this.
				if(pt.has_default>1||pt.is_fixed>1)
				_prototype_construct();
				break;
				
			case daeAtomicType::STRING: //xs:string
			
				((daeStringRef*)&_union.string)->_ref(*DOM);
				break;
			}
		}
		else _union.string = DOM->getEmptyURI().data(); 
	}

COLLADA_(public) //OPERATORS
	/**
	 * Copy Assignment operator
	 */
	inline TypedUnion &operator=(const TypedUnion &cp)
	{
		_copy(cp); return *this;
	}

COLLADA_(public) //ACCESSORS
	/**
	 * Database access (similar to @c daeArray.)
	 */
	inline daeObject *getObject()
	{
		if(0==_offset) return nullptr;
		return reinterpret_cast<daeObject*>((char*)&_union-_offset);
	}
};

/**NEVER-CONST (const is for some derived types)
 * @c daeData is a base class of @c daeValue and 
 * @c daeAttribute that can also represent newer
 * xs:anyAttribute values while hiding non-const
 * methods and members that deal with the schema.
 *
 * @note Since the bitfields are available there
 * are methods for peeking at them. However this
 * class is not really intended for schema-level
 * operations.
 */
class daeData 
{
	friend class daeElement;
	friend class daeAnyAttribute;

COLLADA_(protected) //XML Schema values

	//__vizDefault needs this, but it's
	//best practice.
	COLLADA_ALIGN(8)  
	#ifdef _DEBUG
	/**NATVIS
	 * Previously "_default."
	 */
	daeString __vizDefault;
	#endif

	/**
	 * Not exposed by @c daeValue, but here
	 * so @c XS::Attribute::getName() can see it.
	 */
	daeClientString _attribute_name_string;

  ////TODO?//////////////////////////////////
  //
  // These are 0 for daeAST::Data. They could
  // be moved to daeDefault, and isStatic can
  // test (daeOffset)_attribute_name_string&1
  // to determine if the name is a string-ref
  // Making these objects smaller can tighten
  // up the daeAnyAttribute allocation margin

	/**BITFIELD
	 * @c _attribute_name_string may be a @c daeStringRef.
	 * Assuming attribute names are short.
	 */
	unsigned int _attribute_name_length:8;
	/**BITFIELD-CONTINUED
	 * @c getNextID() uses this flag.
	 * @remarks 8 was going to be 1, but it had relied on
	 * reordering the attributes, except that the generated
	 * compile-time-constant attribute indices realy ought to
	 * accurately reflect the ordering of the static attributes.
	 */
	unsigned int _next_attribute_is_ID:8,_this_attribute_is_ID:1;
	/**BITFIELD-CONTINUED
	 * Both <xs:attribute> & <xs:element> can be fixed.
	 * In which case @c _default becomes the fixed string.
	 * This should not effect @c getDefaultString().
	 */
	unsigned int _fixed_default:2;
	/**BITFIELD-CONTINUED
	 * Various <xs:attribute> attributes/options.
	 */
	unsigned int _attribute_use:2;
	/**BITFIELD-CONTINUED
	 * Tells if @c this is a @c daeValue or @c daeAttribute.
	 */
	unsigned int _static:1,_static_anySimpleType:1;
	/**BITFIELD-CONTINUED
	 * Tells if this is not an attribute in order to handle
	 * the unnamed nonstatic attribute case.
	 */
	unsigned int _is_not_attribute:1;

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;

COLLADA_(public) //OPERATORS
	/**LEGACY-SUPPORT
	 * @c daeValue, @c XS::Attribute and @c daeAttribute inherit
	 * this.
	 * This is a limited form of @c COLLADA_DOM_OBJECT_OPERATORS.
	 */
	inline operator daeData*()const{ return const_cast<daeData*>(this); }
	/**LEGACY-SUPPORT
	 * @c daeValue, @c XS::Attribute and @c daeAttribute inherit
	 * this.
	 * This is a limited form of @c COLLADA_DOM_OBJECT_OPERATORS.
	 */
	inline daeData *operator->()const{ return const_cast<daeData*>(this); }

COLLADA_(public) //LIMITED SCHEMA-LEVEL ACCESSORS
	/**LEGACY
	 * @return Returns the name of this attribute.
	 */
	inline daeName getName()const	
	{
		return daeName(_attribute_name_string,_attribute_name_length); 
	}

	/**WARNING, HINT
	 * @return Returns @c true if this is an attribute listed as an
	 * id-like attribute by XS::Schema::getIDs() (or if it's simply
	 * with @c setIsID() since this is only a hint to eliminate the 
	 * need to test against a battery of possible attribute names.)
	 * @note An "ID" is not an "id" and neither is it an xs:ID type.
	 * (It should be a @c daeString value, but it's not possible to
	 * guarantee that it is.)
	 *
	 * @warning This is probably @c true for an "id" attribute, but
	 * it isn't necessarily one. See @c XS::Schema::getIDs(). SINCE
	 * THE ADVENT of @c daeAnyAttribute @c true==getIsID() whenever
	 * an attribute is not fully described by a @c XS::Schema or is
	 * a @c domAny attribute. Either case is either rare or already
	 * unideal by definition. (NOTE: ideally these are only so when
	 * the underlying @c daeAtomicType are non-empty @c daeString.)
	 * @see @c setIsID()
	 * @see @c XS::Attribute::getThisID()
	 * @see @c daeMetaElement::getFirstID()
	 */
	inline bool getIsID()const{ return 1==_this_attribute_is_ID; }

	/**SEMI-MUTABLE
	 * This is intended to be accessed via @c daeData which is a NEVER-CONST
	 * base class. For non-schema based attributes it's hard to determine if
	 * they are are ID-like or not. So to be safe they may be initially made
	 * to appear as so... since @c getIsID() is merely a hint. With this API
	 * any attribute can be flagged (or unflagged) by applications according
	 * to their need.
	 */
	inline void setIsID(bool y=true){ _this_attribute_is_ID = y?1:0; }

	/**WARNING
	 * Finds the schema's default value.
	 * @return Returns @c nullptr if @c daeValue::getDefaultString() is
	 * an empty string. 	 
	 *
	 * @note This was so to not have to do @c static_cast<daeValue&> to
	 * gain access to @c daeValue::compareIsDefaultWRT(), as the plugin
	 * code otherwise no longer makes use of the @c daeMeta descriptors.
	 * 
	 * @remarks This doubles as a test to determine if there is default
	 * information for this element, because there is no scenario where
	 * the value is not required in addition to the knowledge it exists.
	 *
	 * @warning There are still default values when not expressly given
	 * owing to C++'s constructors; however this API is only concerning
	 * I/O operations, in which case, those are not actually meaningful.
	 */
	inline daeValue *findDefault()const
	{
		if(0==(1&_fixed_default)) return nullptr;
		//TODO: If schema-based attributes are located outside of the
		//element (e.g. because they are uncommon/great in number) it
		//will be necessary to locate the corresponding metadata here.
		assert(1==_static); return (daeValue*)this;
	}

	/**UNUSED
	 * Adding for completion sake. COLLADA doesn't have 
	 * fixed values, but in order to support XML Schema.
	 *
	 * Both <xs:attribute> & <xs:element> can be fixed.
	 * In which case @c _default becomes the fixed string.
	 */
	inline bool getIsFixed()const{ return 0!=(2&_fixed_default); }	

	/**LEGACY
	 * @note This corresponds to <xs:attribute use="required">.
	 * Tells if the schema indicates that this is a required attribute.
	 * @return Returns true if this is a required attribute, false if not.
	 */
	inline bool getIsRequired()const{ return 1==_attribute_use; }	

	/**IMPORTANT	 
	 * Marks a value as one that should not be permitted to be.
	 * I/O plugins must decide what to do here.
	 * @important This doubles as a marker for dummy data; like
	 * the value of an element that isn't supposed to have one.
	 * @note This corresponds to <xs:attribute use="prohibited">.
	 */
	inline bool getIsProhibited()const{ return 2==_attribute_use; }
	
	/**EXPERIMENTAL
	 * This is a roundabout way to determine if the value is an <xs:attribute>.
	 * @note "isAttribute" reads somewhat funny amidst XS::Attribute's members.
	 */
	inline daeXS getXS()const
	{
		//This way can fail if "" is passed to getAttribute().
		//return 0==_attribute_name_length?(daeXS)0:XS::ATTRIBUTE; 
		return 1==_is_not_attribute?(daeXS)0:XS::ATTRIBUTE;
	}

COLLADA_(public) //TYPE-SYSTEM ACCESSORS
	/**LEGACY
	 * Gets the @c sizeof(char) offset (from @ this) where this value's storage 
	 * is found in its container element class.
	 * @return Returns @c sizeof(char) offset from the element's @c this pointer.
	 */
	inline daeOffset getOffset()const
	{
		return ((daeAST::TypedUnion*)(this+1))->_offset; //return _offset; 
	}

	/**LEGACY-ISH, NEVER-NULLPTR
	 * Gets the @c daeTypewriter used by this value.
	 * Prefer @c getTypeWRT() when binary level information is required.	 
	 * @return Returns the @c daeTypewriter2 that this value uses for its
	 * implementation.	 
	 * see @c getTypeWRT(). (@c getDefaultType() is also provided.)
	 *
	 * @note RETURNS POINTER TO BE IN LINE WITH @c daeDefault::getType().
	 */
	inline daeTypewriter2 *getType()const
	{
		//return ((daeAST::TypedUnion*)(this+1))->_type; //return _type; 
		return (daeTypewriter2*)&getDefaultType();
	}	

	/**
	 * Tells if @c this is a @c daeValue. This is nonessential but users may find
	 * it helpful.
	 */
	inline bool isStatic()const
	{
		//This would also work, should _static be removed.
		//return 0==((daeOffset)_attribute_name_string&1);
		return 1==_static; 
	}

COLLADA_(public) //SUPPLEMENTAL
	/**
	 * Gets the type of the default, which is fixed for xs:anySimpleType, and for
	 * other data, avoids the overhead of @c getTypeWRT(), but shouldn't be mixed
	 * with @c getWRT(). This is nonessential but users may find it helpful.
	 * @see daeDefault::getDefaulValue().
	 */
	inline daeTypewriter &getDefaultType()const
	{
		return *((daeAST::TypedUnion*)(this+1))->_type;
	}

	/**
	 * Gets the size of the default, which is fixed for xs:anySimpleType, and for
	 * other data. This is nonessential but users may find it helpful.
	 * Supplementing deprecated @c getSize().	 
	 * @see daeDefault::getDefaulValue().
	 */
	inline daeSize getDefaultSize()const{ return getDefaultType().getSize(); }
		
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("getTypeWRT or getDefaultSize")
	void getSize()const;//{ return getType()->getSize(); }
	COLLADA_DEPRECATED("getTypeWRT -> hasListType")
	void isArrayValue()const;//{ return getType()->hasListType(); }
	#endif
							
COLLADA_(public) //"WRT" APIs. (With Respect To.)
	/**
	 * Gets full type information, where @c getType() is unable.
	 * @param e in order to handle static xs:anySimpleType data.
	 */	
	inline daeTypewriter &getTypeWRT(const daeElement *e)const
	{
		daeTypewriter &tw = getDefaultType();
		if(0!=_static_anySimpleType) 
		return *((daeAnySimpleTypewriter*)&tw)->_type(getWRT(e));
		return tw; 
	}

	template<class T> //daeElement
	/**LEGACY, NOT-RECOMMENDED
	 * Gets the value's memory pointer from containing element @a e.
	 * @param e Element from which to get the value.
	 * @return Returns the memory pointer corresponding to this value out of parent element @a e.
	 */
	inline typename daeConstOf<T,daeOpaque>::type getWRT(T &e)const
	{	
		const daeElement &upcast = dae(*e); return daeOpaque(e)[getOffset()]; (void)upcast;
	}
	template<class This> //daeElement
	/**LEGACY, NOT-RECOMMENDED
	 * Gets the value's memory pointer from containing element @a e.
	 * This overload is just to receive @c this pointers/conceivably other rvalues.
	 * @param e Element from which to get the value.
	 * @return Returns the memory pointer corresponding to this value out of parent element @a e.
	 */
	inline typename daeConstOf<This,daeOpaque>::type getWRT(This *e)const
	{
		const daeElement &upcast = dae(*e); return daeOpaque(e)[getOffset()]; (void)upcast;
	}
	
	template<class Change> //DAEP::Change
	/**
	 * Send note-of-change to database if certain conditions are met.
	 */
	inline Change &noteChangeWRT(Change &note)const
	{ 
		#ifdef NDEBUG
		#error Need a mechanism for suppressing notification.
		#endif
		if(bool notify=true) 
		daeNoteChange(note,const_cast<daeData*>(this)); return note;
	}

	/**WARNING, LEGACY
	 * @warning Implicit in using this API is @c this applies equally
	 * to both inputs. That means both elements are the same type and
	 * that underlying @c this is a @c daeValue.
	 *
	 * Copies the value of this value from fromElement into toElement.
	 * @param toElement Pointer to a @c daeElement to copy this value to.
	 * @param fromElement Pointer to a @c daeElement to copy this value from.
	 * CHANGE-NOTICES
	 * Do "getType()->copy(getWRT(from),getWRT(to));" to bypass @c noteChangeWRT().
	 */
	inline void copyWRT(daeElement *toElement, const daeElement *fromElement)const
	{
		//Todo: How to make this compile?
		assert(1==_static/*&&toElement->getMeta()==fromElement->getMeta()*/);
		_op_assign op(toElement,this,getWRT(fromElement)); return !noteChangeWRT(op);
	}

	/**WARNING, LEGACY
	 * @warning Implicit in using this API is @c this applies equally
	 * to both inputs. That means both elements are the same type and
	 * that underlying @c this is a @c daeValue.
	 *
	 * Compares the value of this value in the given elements.
	 * @param elt1 The first element whose value value should be compared.
	 * @param elt2 The second element whose value value should be compared.
	 * @return Returns a positive integer if value1 > value2, a negative integer if 
	 * value1 < value2, and 0 if value1 == value2.
	 */
	inline int compareWRT(const daeElement *elt1, const daeElement *elt2)const
	{
		//Todo: How to make this compile?
		assert(1==_static/*&&toElement->getMeta()==fromElement->getMeta()*/);
		return getType()->compare(getWRT(elt1),getWRT(elt2));
	}
	
	/**LEGACY
	 * Converts a string to a memory value in the specified element.
	 * @param src Source string, shorter than 128 characters.
	 * (This is to prevent really long @c strlen calls for potentially short values.)
	 * @see daeTypewriter::stringToMemory().
	 * CHANGE-NOTICES
	 * Do "getType()->stringToMemory("xyz",getWRT(e));" to bypass @c noteChangeWRT().
	 */
	inline daeOK stringToMemoryWRT(daeElement *e, daeString src)const
	{
		_op_unserialize<daeOpaque> op(e,this,src,getWRT(e)); return noteChangeWRT(op);
	}
	template<class T> //const daeStringCP* or int
	/**LEGACY, OVERLOAD
	 * Converts a string to a memory value in the specified element.
	 * @see daeTypewriter::stringToMemory().
	 * CHANGE-NOTICES
	 * Do "getType()->stringToMemory("xyz",getWRT(e));" to bypass @c noteChangeWRT().
	 */
	inline daeOK stringToMemoryWRT(daeElement *e, const daeStringCP *src, T len_or_end)const
	{
		_op_unserialize<T,daeOpaque> op(e,this,src,len_or_end,getWRT(e)); return noteChangeWRT(op);
	}
	/**OVERLOAD, NEW/MAY HAVE ISSUES, LEGACY-SUPPORT
	 * Converts a string to a memory value in the specified element.
	 * @see daeTypewriter::stringToMemory().
	 * CHANGE-NOTICES
	 * Do "getType()->stringToMemory("xyz",getWRT(e));" to bypass @c noteChangeWRT().
	 */
	inline daeOK stringToMemoryWRT(daeElement *e, const daeHashString &src)const
	{
		_op_unserialize<daeString,daeOpaque> op(e,this,src,src+src.extent,getWRT(e)); return noteChangeWRT(op);
	}

	/**LEGACY
	 * Converts an element's attribute value to a string.
	 */
	inline daeOK memoryToStringWRT(const daeElement *e, daeArray<daeStringCP> &dst)const
	{
		return getType()->memoryToString(getWRT(e),dst);
	}

	template<class S=void, class T=int>
	/**EXPERIMENTAL, SEMI-INTERNAL
	 * Implements change-notice logic. 
	 */
	class _op_unserialize : public DAEP::Change
	{	
		S s; T t; 		
		daeTypewriter2*tw; daeString src;		
		mutable daeOK OK; mutable bool b; 		
		virtual void carry_out_change()const
		{
			b = true; OK = tw->stringToMemory(src,s,t);
		}public:
		_op_unserialize(daeElement *e, const daeData *v, daeString src, S s, T t=0)
		//Reminder: getXS here is intended to be provisional.
		:Change(e,0==+v->getXS()?+DAEP::CONTENT:+DAEP::ATTRIBUTE)
		,s(s),t(t),tw(v->getType()),src(src),b(){}
		inline operator daeOK(){ if(!b) carry_out_change(); return OK; } 		
	};
	/**TEMPLATE-SPECIALIZATION
	 * Implements @c daeTypeWriter::copy(). 
	 */
	class _op_assign : public DAEP::Change
	{	
		daeOpaque s,t;
		daeTypewriter2*tw; mutable bool b; 		
		virtual void carry_out_change()const
		{
			b = true; tw->copy(s,t);
		}public:
		_op_assign(daeElement *e, const daeData *v, const daeOpaque src)
		//Reminder: getXS here is intended to be provisional.
		:Change(e,0==+v->getXS()?+DAEP::CONTENT:+DAEP::ATTRIBUTE)
		,s(src),t(v->getWRT(e)),tw(v->getType()),b(){}
		//operator! works surprisingly well for this purpose.
		inline void operator!(){ if(!b) carry_out_change(); } 		
	};
};

/**NEVER-CONST, INTERNAL (more or less)
 * This class is the @c daeData equivalent for xs:anySimpleType
 * based data.
 */
class daeAST::Data : public daeData, public TypedUnion
{	
COLLADA_(public) //daeArray traits

	//MULTI-INHERITANCE
	//MSVC treats using daeData::__COLLADA__atomize
	//different from typedef void???
	typedef void __COLLADA__atomize;

COLLADA_(public) //internal/mutual friendship
		
	Data(daeElement*,daeString);

	inline const daeElement &getElement()
	{
		return *(daeElement*)(daeOffset(&_union)-_offset);
	}
}; 

/**
 * Previously "daeValue."
 * The @c daeDefault class describes a C++ COLLADA-DOM element's value.
 * @c XS::Attribute is based on @c daeDefault.
 * @see @c XS::Attribute a.k.a. @c daeAttribute.
 * @see @c daeValue
 * @note The common name of this class is @c daeValue. This is so that
 * it is not necessary to write @c const before its name. From the POV
 * of a generator, it is used to set defaults.
 */
class daeDefault : public daeData
{
	friend class daeMetaElement;

COLLADA_(protected)
	/**
	 * This is the same offset as @c TypedUnion
	 * It would belong to @c daeData except for 
	 * 32-bit alignment issues, since @c _union
	 * requires 64-bit alignment.
	 */
	daeOffset _offset;

	/**FIRST-MEMBER, LEGACY-SUPPORT
	 * Houses RTTI, default-value, typename, and min/maxLength.
	 * (Only the first two matter)
	 */
	daePrototype _type;	

COLLADA_(protected) //XML Schema relationships	
	/**
	 * This is used to destruct the prototypes.
	 * TODO?
	 * Thinking this can go if @c daeTypewriter or @c daePlatonic had a
	 * "destruct" solution.
	 */
	void (*_destructor)(const void*);

COLLADA_(protected) //XML Schema values	
	/**COURTESY
	 * Not used internally. (I think maybe it is used somewhere, once?)
	 * Equivalent to @c getMeta().getSchema().findType(getType().alias).
	 * It'd be right to let this go if it's the only value remaining at
	 * the end of the day.
	 */
	const XS::SimpleType *_simpletype;
				   
COLLADA_(public) //HOW-TO? 
	/**THEORETICAL
	 * Previously "getContainer()." 
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~[C++]
	 * daeMeta &getMeta()const
	 * {
	 *	return *_howto_getPrototype().__DAEP__Element__data.meta; 
	 * }
	 * const DAEP::Element &_howto_getPrototype()const
	 * {
	 *	return _type.value[-_offset]; 
	 * }
	 */
//	daeMeta &getMeta()const;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeDefault)
	/**
	 * These are the same class under the hood. 
	 */
	inline operator daeAttribute&()const{ return *(daeAttribute*)this; }

COLLADA_(public) //ACCESSORS  
	/**LEGACY	
	 * Gets the @c daeTypewriter used by this value.
	 * Post-2.5 @c daePrototype converts into @c daeTypewriter.
	 * @return Returns the @c daeTypewriter that this value uses for its
	 * implementation.
	 *
	 * Prefer @c getTypeWRT() when binary level information is required.	 
	 * @return Returns the @c daeTypewriter2 that this value uses for its
	 * implementation (via @c daeProtype::operator->(), etc.)	 
	 * see @c getTypeWRT().
	 */
	inline const daePrototype &getType()const{ return _type; }

	/**COURTESY
	 * Gets the @c XS::SimpleType used by this value.
	 * The library doesn't use this. It's provided if user/client code does.
	 * 
	 * @TODO There are ways to compute this. 
	 */
	inline const XS::SimpleType &getSimpleType()const{ return *_simpletype; }

	#ifndef COLLADA_NODEPRECATED	
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Don't believe this is used or required. Conflicts with nul-value workflow.")
	/**NOT-IMPLEMENTING 
	 * Gets the containing @c daeMetaElement for this value.
	 * @return Returns the @c daeMetaElement to which this @c daeValue belongs.
	 */
	void getContainer()const;	
	COLLADA_DEPRECATED("defaultToString (reducing memory footprint)")
	/**NOT-IMPLEMENTING 
	 * Gets the default for this value as a string.
	 */
	void getDefaultString()const;
	#endif //COLLADA_NODEPRECATED

	/**LEGACY-SUPPORT
	 * Gets the default for this value as a memory value. 
	 * @return Returns a @c daeOpaque representing the default value.
	 *
	 * @note This was deprecated in favor of "getDefaultString," but now that 
	 * the string is removed it is complementing @c daeData::getDefaultType().
	 * @see @c defaultToString().
	 */
	const daeOpaque getDefaultValue()const{ return getType().value; }

	/**WARNING, LEGACY-SUPPORT
	 * Previously "getDefaultString."
	 * Gets the "default" for this value as a string.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=findDefault() indicates that the schema provides a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * Converts an element's default value to a string.
	 * @note The generator provided string had been stored separately, but that is
	 * just taking up space, and only of very limited value. THAT SAID COLLADA-DOM
	 * isn't intended to replicate the schema precisely, and there is some LOSS in
	 * the conversion, for example "1.0" becomes "1" and "false" becomes 0, and so
	 * on. An additional API could doctor these strings, and client code can do so.
	 */
	inline daeOK defaultToString(daeArray<daeStringCP> &dst)const
	{
		return _type.writer->memoryToString(_type.value,dst);
	}

COLLADA_(public) //"WRT" APIs. (With Respect To.)
	/**WARNING, LEGACY
	 * Copies the default value of this value to the element.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=findDefault() indicates that the schema provides a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * @param toElement Pointer to a @c daeElement to copy the default value to.
	 * CHANGE-NOTICES
	 * Do "getType()->copy(getType().value,getWRT(element));" to bypass @c noteChangeWRT().
	 */
	inline void copyDefaultWRT(daeElement *toElement)const
	{
		_op_assign op(toElement,this,_type.value); return !noteChangeWRT(op);
	}

	/**WARNING, LEGACY
	 * Compares the value of this value from the given element to the default value
	 * of this value.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=findDefault() indicates that the schema provides a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * @param e The element whose value should be compared to the default value.
	 * @return Returns a positive integer if value > default, a negative integer if 
	 * value < default, and 0 if value == default.
	 */
	inline bool compareIsDefaultWRT(const daeElement *e)const
	{
		//Note: This API was "compareToDefault" but it seems like testing for 0 is
		//not as user-friendly as a bool result. When is cmp<0, cmp>0 ever useful?
		return 0==_type.writer->compare(getWRT(e),_type.value);
	}

COLLADA_(public) //GENERATOR-SIDE APIs

	/**GENERATOR-SIDE API, UNUSED
	 * Adding for completion sake. COLLADA doesn't have 
	 * fixed values, but in order to support XML Schema.
	 */
	inline void setIsFixed(){ _fixed_default|=2; }

	template<int N>
	/**GENERATOR-SIDE API, LEGACY
	 * Sets the default for this attribute via a string.
	 * @param defaultVal @c daeString representing the default value.
	 */
	inline void setDefaultString(const daeStringCP (&def)[N])
	{
		//This API has to disable the prototype data-bit temporarily.
		//That could be avoided if there is a dedicated finalization
		//API that the generator must call. It doesn't seem worth it
		//to add such an API just for this, although addContentModel
		//could call it, so it would only be needed for simple-types.
		if(N) _setDefaultString(def); assert(N);
	}
	/** Implements setDefaulString(). */
	COLLADA_DOM_LINKAGE void _setDefaultString(daeHashString);
};

/**TODO, NEVER-CONST, NEW/ROUGH
 *
 * @todo Instead of @c daeData* a @c union with @c daeAST::Data* can
 * help with debugging. It might also be useful (in theory) to place
 * in each subscript an 8-bit size and a @c daeStringCP for matching
 * the first letter in the attribute's name. In theory this can make
 * lookups more cache-friendly, but it also can help with @c _grow().
 * ALSO it can help in case the @c daeData flag/size fields are ever
 * moved into @c daeDefault. (They are always 0 with @c daeAST::Data
 * and the size in their case can be extracted from the string-ref.)
 *
 * @c daeAnyAttribute is a new lightweight attributes container that
 * is able to implement <xs:anyAttribute> semantics but is important
 * also to effciently implementing @c domAny elements. Its name is a
 * slight misnomer since it tracks all attributes. 
 * @see @c daeElement::getAttributes().
 *
 * @ramark The value and attribute records maintain the same address.
 * This is in keeping with static attribute behavior, so there isn't
 * any additional consideration, and algorithms have all flexibility.
 */
class daeAnyAttribute : private daeArray<daeData*>
{	
	typedef daeArray<daeData*> _base;

	using _base::_au;

COLLADA_(public) //daeArray traits

	template<class>
	friend struct daeAtomOf;
	void __COLLADA__locate();

COLLADA_(public) //daeArray-LIKE ACESSORS

	using _base::size;
	using _base::empty;
	using _base::begin;
	using _base::end;
	using _base::capacity;
	using _base::getCapacity;
	using _base::getCount;
	using _base::find;

	/**
	 * Increases the capacity of the @c daeArray 
	 * while allocating the @c daeData nodes that
	 * it points to at the same time.
	 *
	 * @param toCapacity_1 excludes the 0-terminator
	 * that holds the @c daeAtomicType::VOID element.
	 * Read: @c capacity()-1.
	 */
	inline void grow(size_t toCapacity_1)
	{	
		//WARNING
		//>= is used to include the 0-terminator.
		//(This diverges from capacity/getCapacity()!)		
		if(toCapacity_1>=getCapacity()) _grow(toCapacity_1);
	}
	/**Implements @c grow(). */
	COLLADA_DOM_LINKAGE void _grow(size_t);

	/**
	 * This class is always instantiated inside of an element and is the
	 * same size as a pointer. So it's possible to locate the element in
	 * reverse, but client code should avoid this backward approach when
	 * possible. @c const_cast is needed to elide the @c const qualifier.
	 */
	const daeElement &getElement()
	{
		return daeOpaque(this)[-daeOffset(sizeof(DAEP::Element))];
	}

COLLADA_(public) //xs:anyAttribute ACCESSORS
	/**
	 * Tells if there is <xs:anyAttribute> attributes
	 * or any attributes undescribed by schemas. This
	 * includes @c domAny attributes.
	 */
	inline bool hasAnyExtra()
	{
		return size()!=(daeSize)_au->getOffset();
	}

	/**
	 * Gets a range enclosing the back half of @c this
	 * @c daeArray based container. Attributes in this
	 * range are <xs:anyAttribute> attributes or those
	 * undescribed by schemas, or @c domAny attributes.
	 */
	inline const daePartition &getAnyExtraPart()
	{
		daeAllocThunk *t = (daeAllocThunk*)this;
		assert((void*)&t->_counter==&t->_offset+1);
		return *(daePartition*)&t->_offset;
	}		 	

COLLADA_(public) //daeArray-LIKE OPERATORS

	template<class I>
	/**UNORTHODOX
	 * @tparam T if integer type accesses @c daeData* elements identical
	 * to @c daeArray. If non-integer, it is a string-based name that is
	 * inserted. Newly inserted values assume the schema's default value.
	 *
	 * UNORTHODOXY
	 * @note An integer index is allowed to index 1 extra element beyond
	 * @c size() without triggering an @c assert(). This index holds the
	 * @c daeAtomicType::VOID value that will @c assert() if overwritten
	 * and has a blanked name.
	 */
	daeData*const &operator[](const I &i_or_Name)
	{
		return _get<0>((typename daeBoundaryString2<I>::type*)0,i_or_Name);
	}
	template<class I>
	/**	 
	 * Implements @c daeElement::getAttribute().
	 * @return Returns a dummy/empty attribute if the attribute is
	 * addressed by name, and is not found to exist. Integer indexes 
	 * are forwarded to make adaptation straightforward.
	 */
	daeData &_maybe_get(const I &i_or_Name)
	{
		return *_get<1>((typename daeBoundaryString2<I>::type*)0,i_or_Name);
	}
	template<int, class I>
	/**Implements @c operator[](). */
	daeData*const &_get(daeString*, I Index)
	{
		//daeElement::getAttributeIndex() is returning a VOID
		//index equal to size() instead of -1 and so it helps
		//if users can access this index like any other index.
		//return _base::operator[i];
		assert(Index<=getCount()); return _au->_varray[Index];
	}
	template<int Maybe, class C_string>
	/**Implements @c operator[](). */
	daeData*const &_get(daeString*, C_string *Name)
	{
		return _get<Maybe>((daeName*)0,daeName(Name));
	}	
	template<int Maybe, class T>
	/**
	 * Implements @c operator[](). 
	 * @return Returns @c daeAtomicType::VOID type @c daeData
	 * if @c 1==Maybe or @c true==Name.empty().
	 */
	daeData*const &_get(daeName*, T &Name)
	{
		//This may aid compilers around goto i; 
		daeData**va = _au->_varray;
		//HACK: Tell compiler that it should not be concerned
		//with if the size of Name is more than 255.		
		const daeName &fast = Name;
		unsigned char size8 = fast.extent&0xFF;
		//TODO? Can do <= here to filter out empty names, but
		//just doing an assert() in _blind_push_back() so any
		//empty strings will just become irregular attributes.
		//(There's no real reason that "" should be a VOID/""
		//attribute anyhow (two wrongs don't a make a right.)		
		size_t i; for(i=0;i<size();i++) 
		{
			//if(Name==_au->_varray[i]->getName()) 
			if(size8==va[i]->_attribute_name_length
			&&0==memcmp(fast,va[i]->_attribute_name_string,size8*sizeof(daeStringCP)))
		i:	return _au->_varray[i]; //Not va!
		}
		if(!Maybe) _extern_push_back(i,Name); goto i;	
	}

	/**USEFUL?
	 * Can't say if this is useful.
	 */
	inline daeAnyAttribute &operator=(daeAnyAttribute &cp)
	{
		_assign<false,false>(cp); return *this;
	}
	template<bool clone, bool empty>
	/**EXPERIMENTAL
	 * Implementing @c daeElement::clone(). Actually clone should
	 * optimize this a lot.
	 * @tparam clone means the owning elements are the same types.
	 * @tparam empty means no external attributes must be cleared.
	 */
	inline void _assign(daeAnyAttribute &cp)
	{
		#ifdef NDEBUG
		#error Should test this with daeElement::clone/operator=.
		#endif
		size_t cpN = cp.size();
		size_t i,iN = std::min(size(),cpN);
		const daeElement *src = &cp.getElement();
		daeElement *dst = const_cast<daeElement*>(&getElement());			
		//Optimistically assuming involving same type of elements.
		for(i=0;i<iN;i++)
		{	
			daeData *cmp = _au->_varray[i];
			if(clone||cmp==cp[i]) 				
			cmp->copyWRT(dst,src);
			else break;
		}
		if(i==cpN) return;
		//If not, the best that can be done is to set to defaults.
		//(Reminder: Planning to rectify this shortly.)
		size_t jN = 0;
		if(!clone&&!empty) 
		jN = _extern_clear(size(),i);
		grow(cpN);
		if(!clone) for(size_t j=i;j<jN;j++)
		{		
			daeData *clr = _au->_varray[j];
			((daeDefault*)clr)->copyDefaultWRT(dst);
		}
		//Just trying to emulate daeElement APIs without including
		//daeElement.h for right now.
		daeArray<daeStringCP,32> buf; for(;i<cpN;i++)
		{
			daeData *cpy = cp[i];
			cpy->memoryToStringWRT(src,buf);			
			if(clone)
			{
				_blind_push_back(i,
				*(daeStringRef*)&cpy->_attribute_name_string);
				cpy = back();
			}
			else
			{
				//TODO? If all attribute names were daeStringRef
				//it would be safe to cast here. Alternatively a
				//loop could go before this one.
				cpy = operator[](cpy->getName());
			}
			cpy->stringToMemoryWRT(dst,buf);
		}
	}

COLLADA_(public) //VARIOUS
	/**
	 * Disabled Constructor
	 */
	daeAnyAttribute();

	template<class Type> //DAEP::Value<...>
	/**
	 * Prototype Constructor (No-op)
	 */
	explicit daeAnyAttribute(const DAEP::Proto<Type>&)
	:_base(*_au){} //NOP

	/**TODO
	 * Destructor
	 * @todo C++11 Might want to delete this destructor.
	 */
	~daeAnyAttribute()
	{
		if(hasAnyExtra()) _extern_free2();

		//HACK: Foil ~daeArray().
		static daeAllocThunk retain_static_attribs;
		*(daeAllocThunk**)this = &retain_static_attribs;
	}

COLLADA_(private) //INTERNAL	
	/**USEFUL?
	 * Removes everything after the last static attribute.
	 * Probably the only context it makes sense to do so
	 * is if you don't want to think about double-entries.
	 */
	inline size_t _extern_clear(size_t i, size_t hint)
	{
		//TODO? Might want to make the types of these
		//attributes "VOID" via some xs:anySimplyType 
		//framework. Attributes aren't ref-counted so
		//for now they just retain their older values.
		while(_au->_varray[hint]->isStatic()) hint++;
		std::swap(_au->_varray[i],_au->_varray[hint]);
		_au->setInternalCounter(hint); return hint;
	}

	template<class T>
	/**
	 * Pushes onto @c back(), regarding @a Name as unique.
	 */
	inline void _extern_push_back(size_t i, const T &Name)
	{
		//6 is a minimum to fit a single Data object into
		//discarded buffers in release builds with domAny
		//(starting with no static attributes.)
		if(++i==getCapacity())
		_grow(i+6); _blind_push_back(i,Name);
	}
	template<class T>
	/**Implements @c _extern_push_back(), etc. */
	inline void _blind_push_back(size_t i, const T &Name)
	{
		//Note the main cost of having a VOID data-type on
		//the end is swapping these pointers. Probably the
		//pattern is lost on compilers or may be too messy.
		_au->setInternalCounter(i);
		std::swap(_au->_varray[i-1],_au->_varray[i]);		
		//Note: Name can be a daeStringRef at this stage.
		*(daeStringRef*)&back()->_attribute_name_string = Name;
		//Note: Name can be a string-literal at this stage.
		size_t len = daeHashString(Name).size(); 
		assert(len!=0&&len<256);
		back()->_attribute_name_length = 0xFF&len;
	}

COLLADA_(private)

	//REMINDER: The primary motivation for these is so the
	//daeData attributes, once created, do not move around
	//in memory; Neither data nor metadata.
	friend daeElement;
	inline daeAlloc<> *_head_AU();
	inline daeAlloc<> *_next_AU(daeAlloc<>*);
	inline void _llist_AU(daeAlloc<>*,void*);		
	inline void _extern_free(const void *AU);
	COLLADA_DOM_LINKAGE void _extern_free2();
};

//-------------------.
	namespace XS //<-'
	{//-.
//<-----'

/**
 * @c XS::Attribute describes C++ COLLADA-DOM elements' attributes.
 * @note The common name of this class is @c daeAttribute. This is
 * so that it is not necessary to write @c const before its name.
 * @see daeValue.
 */
class Attribute : public daeDefault
{
#ifdef BUILDING_COLLADA_DOM

COLLADA_(public) //INVISIBLE
	/**INTERNAL
	 * @c daeAtomicType::VOID
	 */
	Attribute(int); Attribute(){}

#endif //BUILDING_COLLADA_DOM

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(XS::Attribute)

COLLADA_(public) //ACCESSORS
	/**WARNING
	 * @warning This is probably not @c nullptr for an "id" attribute, but
	 * it isn't necessarily one. See @c XS::Schema::getIDs().
	 *
	 * Tells if this attribute is an ID attribute. This is test if @c this
	 * is part of the @c getNextID() chain given an arbitrary @c daeAttribute.
	 * There isn't a backward iterator at this time.
	 * @see @c daeMetaElement::getFirstID()
	 */
	inline daeAttribute *getThisID()const
	{
		return 0==_this_attribute_is_ID?nullptr:this; 
	}
	/**
	 * Gets the next ID attribute. IDs can be anything. The user provides
	 * the list on a per @c XS::Schema basis. COLLADA uses "id" and "sid."
	 * @see @c daeMetaElement::getFirstID()
	 *
	 * @note This can reach up to 255 attributes away, but if it must be
	 * stretched further, it may return a non-ID attribute to make up the
	 * difference.
	 */
	inline daeAttribute *getNextID()const
	{
		return 0==_next_attribute_is_ID?nullptr:this+_next_attribute_is_ID; 
	}

COLLADA_(public) //GENERATOR-SIDE APIs

	/**GENERATOR-SIDE API, LEGACY
	 * Unsets @c setIsProhibited().
	 * This corresponds to <xs::attribute use="required">.
	 * Sets the value that indicates that this attribute is required by the schema. 
	 * If set, the attribute should be exported by the API regardless of its value.	 
	 */
	inline void setIsRequired(){ _attribute_use = 1; }

	/**GENERATOR-SIDE API
	 * Unsets @c setIsRequired().
	 * This corresponds to <xs::attribute use="prohibited">.
	 * @note This may be set on the element's value of non simple-content type elements.
	 */
	inline void setIsProhibited(){ _attribute_use = 2; }

	/**GENERATOR-SIDE API	 
	 * Unsets @c setIsRequired() and @c setIsProhibited(). 
	 * Avoid this one. It's probably of no practical use. This family of APIs should be 
	 * regarded as one way only settings.
	 */
	inline void setIsOptional(){ _attribute_use = 0; }
};

//-------.
	}//<-'
}

#endif //__COLLADA_DOM__DAE_META_ATTRIBUTE_H__
/*C1071*/
