/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_META_ATTRIBUTE_H__
#define __COLLADA_DOM__DAE_META_ATTRIBUTE_H__

#include "daeAnySimpleType.h"

COLLADA_(namespace)
{//-.
//<-'

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
	 * Formerly "_default."
	 */
	daeString __vizDefault;
	#endif

	/**
	 * Not exposed by @c daeValue, but here
	 * so @c XS::Attribute::getName() can see it.
	 */
	daeClientString _attribute_name_string;

	/**INTERNAL Comes from @c daeAnyAttribute. */
	void _set_attribute_name_length_etc(const daeName &q)
	{
		//0xFF& is in case of narrowing warnings :(
		assert(q.extent!=0&&q.extent<256);
		//Not implementing truncation... assuming not.
		//size_t len = std::min<size_t>(255,q.extent);		
		size_t len = 0xFF&q.extent;		
		_attribute_name_length = 0xFF&len; 
		while(len-->0) if(':'==q[len])
		{
			_qualified_name_offset = 0xFF&(len+1);			
			//REMOVE ME
			//q.extent = len; //xmlns
		}/*Doing this & id in _reset_type upon assignment.
		//TODO? Would delay this until assingnment, since
		//the odds are very slim. But it's not so easy as
		//it seems, since an anySimpleType is initially a
		//string type. (Maybe if it started out as a fake
		//string type?
		if("xmlns"==q) setIsXMLNS();*/
	}

	/**BITFIELD (ALL FULL: int IS 32-BITS)
	 * @c _attribute_name_string may be a @c daeStringRef.
	 * Assuming attribute names are short.
	 */
	unsigned int _attribute_name_length:8;
	/**BITFIELD-CONTINUED
	 * @c _qualified_name_offset includes the : character
	 * in a QName.
	 *
	 * @todo ONE WAY TO IMPLEMENT form="qualified" IS FOR
	 * THE NAME TO BEGIN WITH ':' SO THAT THE PREFIX MUST
	 * BE FILLED IN. THE BITS ARE ALL USED UP, AND IT MAY
	 * EVEN BE A BETTER WAY TO DO IT THAN TO HAVE A CHECK.
	 * NOTE IT DEFAULTS TO THE attributeFormDefault VALUE.	 
	 * COLLADA is "unqualified". A feature can wait until 
	 * a schema requires it.
	 */
	unsigned int _qualified_name_offset:8;
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
	 * This should not affect @c getDefaultString().
	 */
	unsigned int _fixed_default:2;
	/**BITFIELD-CONTINUED
	 * Various <xs:attribute> attributes/options.
	 */
	unsigned int _attribute_use:2;
	/**BITFIELD-CONTINUED
	 * Tells if @c this is a @c daeValue or @c daeAttribute.
	 *
	 * @note Don't know if this needs 2 bits, but it's also
	 * used by @c isValue(), in which case it does. Static
	 * can also be implemented by @c _attribute_name_string
	 * by checking its first bit, as long as client strings
	 * are all word aligned pointers. It had been "_static"
	 * but has fewer instructions inverted in @c isMasked().
	 */
	unsigned int _anySimpleType:2;
	/**FINAL BITFIELD ENTRY
	 * Tells if the attribute has an "xmlns" qualified name.
	 * Static attributes must also be STRING formatted type.
	 */
	unsigned int _is_xmlns_attribute:1;

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
	/**
	 * @return Returns QName of @c this attribute.
	 */
	inline daeName getName()const	
	{
		return daeName(_attribute_name_string,_attribute_name_length); 
	}

	/**0-TERMINATED (as long as not truncated)
	 * @return Returns the part of the QName after a prefix/separator.
	 */
	inline daeName getNCName()const	
	{
		daeName o = getName();
		o.string+=_qualified_name_offset; 
		o.extent-=_qualified_name_offset; return o;
	}

	typedef unsigned char CP;
	/**
	 * @return Returns the offset to the beginning of the NCName part
	 * of the attribute's full QName. 
	 */
	inline CP getNCNameCP()const{ return _qualified_name_offset; }
	 
	//TODO: I think form="qualified" may have to start names with ':'
	//or else there is not room in the BITFIELD, and it might be best
	//to do it that way regardless.
	/**WARNING
	 * @return Returns @c true if the name includes a colon/separator.
	 * @warning This includes a QName that begins with a ':', whether
	 * this is valid or not. This library does not perform validation.
	 * (Note, thinking of doing it as a form="unqualified" strategy.)
	 */
	inline bool hasQName()const{ return 0!=_qualified_name_offset; }
	
	/**UNTERMINATED (':' or '\0' TERMINATED)
	 * @return Returns the part of the QName before a colon/separator.
	 * @warning A non-empty string is ':' terminated; not included in
	 * in the returned size. In theory "" prefixes are ':' terminated.
	 */
	inline daeName getQNamePrefix()const	
	{
		daeName o(_attribute_name_string,_qualified_name_offset); 
		if(!o.empty()) o.extent--; return o;
	}

	/**	 
	 * @return Returns @c true if this attribute's name is "xmlns" or
	 * the non-NCName part of the name is so.
	 * @remark This method checks a flag, rather than comparing names.
	 */
	inline bool getIsXmlns()const{ return 1==_is_xmlns_attribute; }

	/**INTERNAL */
	inline void _setIsXmlns(const daeElement *prototype)
	{
		_is_xmlns_attribute = 1; _setIsXml(1,prototype);
	}
	/**INTERNAL */
	static void _setIsXml(int b, const daeElement *prototype)
	{
		(*(DAEP::Element*)prototype) //FRIENDS/CYCLICAL
		.__DAEP__Element__data.xml_qualified_attribute_masks|=0xf&b;
	}

	/**WARNING, HINT
	 * @return Returns @c true if this is an attribute listed as an
	 * id-like attribute by XS::Schema::getIDs() (or if it's simply
	 * with @c setIsID() since this is only a hint to eliminate the 
	 * need to test against a battery of possible attribute names.)
	 * @note An "ID" is not an "id" and neither is it an xs:ID type.
	 * @see @c XS::Attribute::getThisID()
	 * @see @c daeMetaElement::getFirstID()
	 *
	 * @warning This is probably @c true for an "id" attribute, but
	 * it isn't necessarily one. See @c XS::Schema::getIDs(). SINCE
	 * THE ADVENT of @c daeAnyAttribute @c true==getIsID() whenever
	 * an attribute is not fully described by a @c XS::Schema or is
	 * a @c domAny attribute. Either case is either rare or already
	 * unideal by definition. 
	 * @see @c setIsID() for details.
	 */
	inline bool getIsID()const{ return 1==_this_attribute_is_ID; }

	/**WARNING, MUTABLE
	 * This is intended to be accessed via @c daeData which is a NEVER-CONST
	 * base class. For non-schema based attributes it's hard to determine if
	 * they are are ID-like or not. So to be safe they may be initially made
	 * to appear as so... since @c getIsID() is merely a hint. With this API
	 * any attribute can be flagged (or unflagged) by applications according
	 * to their need...
	 *
	 * @warning Nonstatic xs:anySimpleType values set this automatically for
	 * @c daeAtomicType::STRING values when first assigned. This is not good
	 * if the ID is a numeric string, and it is not set until after a change
	 * notice is processed, and so a change-notice handler must do this only
	 * after applying the change, if it relies on this setting. If set prior
	 * to assignment it is not unset. It is only a speculative hint, however
	 * if mistakenly set it will have to be manually unset, after assignment.
	 * This may not apply when assignment bypasses @c daeAnySimpleTypewriter.
	 */
	inline void setIsID(bool y=true)
	{
		assert(!isValue()); _this_attribute_is_ID = y?1:0;
	}

	/**
	 * Tells if the schema specifies a default value.
	*/
	inline bool hasDefault()const{ return 1==_fixed_default; }

	/**WARNING
	 * Gets the schema's default value, or @c nullptr.	 
	 * @return Returns @c nullptr if @c daeValue::getDefaultString() is
	 * an empty string. 	 
	 *
	 * @warning There are still default values when not expressly given
	 * owing to C++'s constructors; however this API is only concerning
	 * I/O operations, in which case, those are not actually meaningful.
	 * @see @c isMasked().
	 */
	inline daeValue *getDefault()const
	{
		return hasDefault()?&getKnownValue():nullptr;
	}

	/**UNUSED
	 * Adding for completion sake. COLLADA doesn't have 
	 * fixed values, but in order to support XML Schema.
	 *
	 * Both <xs:attribute> & <xs:element> can be fixed.
	 * In which case @c _default becomes the fixed string.
	 */
	inline bool getIsFixed()const{ return 2==_fixed_default; }	

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
	 *
	 * @return Returns 0 if @c this is a @c daeValue or if it is the @c VOID 
	 * pseudo-attribute. If necessary @c isValue() distinguishes between them.
	 */
	inline daeXS getXS()const{ return 0==_attribute_name_length?(daeXS)0:XS::ATTRIBUTE; }

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
		return (daeTypewriter2*)&getDefaultType();
	}	

	/**
	 * Tells if @c this is a @c daeValue. If so it means @c this value
	 * is part of the schema. If not, its type is @c daeTypedUnion and
	 * it is part of @c daeAnySimpleType::Data. Note that it can still
	 * be xs:anySimpleType if a schema uses this type, though uncommon.
	 */
	inline bool isValue()const
	{
		//This should also work, although isMasked would 
		//have to test more than one word of memory then.
		//return 0==((daeOffset)_attribute_name_string&1);
		return 0==(1&_anySimpleType); 
	}	

	/**
	 * Gets @c this data as a @c daeValue if it is static. 
	 * @return Returns @c nullptr if @c false==isValue().
	 * @see @c isValue() Doxygentation.
	 */
	inline daeValue *getValue()const{ return isValue()?(daeValue*)this:nullptr; }

	/**CAST
	 * Gets @c *(daeValue*)this with @c assert(isValue()) check.
	 * @see @c isValue() Doxygentation.
	 */
	inline daeValue &getKnownValue()const{ assert(isValue()); return *(daeValue*)this; }	
	
	/**
	 * Tells if the data is @c daeAST::TypedUnion. Note that this does
	 * not say if the value is nonstatic, because xs:anySimpleType can
	 * be a schema-based value. @c isAnyExtra() or @c !isValue() means
	 * data is nonstatic.
	 */
	inline bool isUnion()const{ return 0!=_anySimpleType; }	

	/**
	 * Tells if the data is nonstatic. That is it isn't described by a
	 * a schema, and so is part of @c this object.
	 */
	inline bool isAnyExtra()const{ return !isValue(); }	

	/**
	 * Gets @c this as @c daeAST::Data object's @c daeAST::TypedUnion.
	 */
	inline const daeAST::TypedUnion &getKnownAnyExtra()const
	{
		assert(isAnyExtra()); return *(daeAST::TypedUnion*)(this+1); 
	}	
	/**
	 * Gets @c this as @c daeAST::Data object's @c daeAST::TypedUnion.
	 */
	inline daeAST::TypedUnion &getKnownAnyExtra(DAEP::Object *e)const
	{
		daeAST::TypedUnion &o = *(daeAST::TypedUnion*)(this+1);
		assert((daeOffset)e+getOffset()==(daeOffset)&o._union);
		assert(isAnyExtra()); return o; 
	}		
	/**
	 * Gets @c this as @c daeAST::Data object's @c daeAST::TypedUnion.
	 */
	inline const daeAST::TypedUnion &getKnownAnyExtra(const DAEP::Object *e)const
	{
		return getKnownAnyExtra(const_cast<DAEP::Object*>(e));
	}	

	/**
	 * Tells if @c this attribute has use for its mask bit considering
	 * that if it has a fixed or default value or is nonstatic then it
	 * for purposes of reading/writing XML documents the mask is of no
	 * practical use. For example, an integer defaults to 0, and so if
	 * the mask is not set, it's ambiguous if a document contains that
	 * attribute if it doesn't have a nonzero default value.
	 *
	 * @return Returns @c false if not optional or if the attribute is
	 * fixed, or has a default value, or is nonstatic. May also return
	 * @c false if masking is disabled, or the data's type is a string.
	 *
	 * @note It is also no use if the underlying type is a string, but
	 * this is not checked by the current implementation. There is not
	 * a bit dedicated to tracking this status presently.
	 *
	 * To set/unset the mask use @c daeElement::getAttributeMask(), or
	 * lower-level, use @c daeAnyAttribute::mask().
	 */
	inline bool isMasked()const
	{
		//Also including _attribute_use (required/prohibited) since it
		//should just get factored into a test instruction's bits mask.
		return isValue()&&0==_fixed_default&&0==_attribute_use; 
	}

COLLADA_(public) //SUPPLEMENTAL
	/**
	 * Formerly "getType."
	 * Gets the type of the default, which is fixed for xs:anySimpleType, and for
	 * other data, avoids the overhead of @c getTypeWRT(), but shouldn't be mixed
	 * with @c getWRT(). This is nonessential but users may find it helpful.
	 * @see daeDefault::getDefaulValue().
	 */
	inline daeTypewriter &getDefaultType()const
	{
		//REMINDER: Don't put "assert(isValue())" here.
		return *((daeAST::TypedUnion*)(this+1))->_type;
	}

	/**
	 * Formerly "getSize."
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
		if(2==_anySimpleType) 
		return *((daeAST::AnyWriter*)&tw)->_type(getWRT(e));
		return tw; 
	}

	template<class T> //daeElement
	/**LEGACY-SUPPORT
	 * Gets the value's memory pointer from containing element @a e.
	 * @param e Element from which to get the value.
	 * @return Returns the memory pointer corresponding to this value out of parent element @a e.
	 */
	inline typename daeConstOf<T,daeOpaque>::type getWRT(T &e)const
	{	
		const daeElement &uc = dae(*e); return daeOpaque(e)[getOffset()]; (void)uc;
	}
	template<class This> //daeElement
	/**LEGACY-SUPPORT
	 * Gets the value's memory pointer from containing element @a e.
	 * This overload is just to receive @c this pointers/conceivably other rvalues.
	 * @param e Element from which to get the value.
	 * @return Returns the memory pointer corresponding to this value out of parent element @a e.
	 */
	inline typename daeConstOf<This,daeOpaque>::type getWRT(This *e)const
	{
		const daeElement &uc = dae(*e); return daeOpaque(e)[getOffset()]; (void)uc;
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

	/**WARNING, EXPERIMENTAL
	 * @warning Possibly harebrained.
	 * Compares this value to binary 0. Maybe helpful with @c isMasked() depending
	 * on how it's being used.
	 * @remark Won't work for arrays. (Nor strings, though they're plainly empty.)
	 */
	inline bool compareIsZeroedWRT(const daeElement *e)const
	{
		const char *cmp = &getWRT(e);		
		//return '\0'==*cmp&&0==memcmp(cmp,cmp+1,getDefaultSize()-1);
		for(size_t i=0,iN=getDefaultSize();i<iN;i++) if(cmp[i]!=0x00) 
		return false; return true;
	}
		
	template<class T> //const daeStringCP* or int
	/**LEGACY, OVERLOAD
	 * Converts a string to a memory value in the specified element.
	 * @see daeTypewriter::stringToMemory().
	 * CHANGE-NOTICES
	 * Do "getType()->stringToMemory("xyz",getWRT(e));" to bypass @c noteChangeWRT().
	 */
	inline daeOK stringToMemoryWRT(daeElement *e, daeString src, T len_or_end)const
	{
		_op_unserialize<T,daeOpaque> op(e,this,src,len_or_end,getWRT(e)); return noteChangeWRT(op);
	}
	template<class T>
	/**LEGACY
	 * Converts a string to a memory value in the specified element.
	 * @param src Source string, shorter than 128 characters.
	 * (This is to prevent really long @c strlen calls for potentially short values.)
	 * @see daeTypewriter::stringToMemory().
	 * CHANGE-NOTICES
	 * Do "getType()->stringToMemory("xyz",getWRT(e));" to bypass @c noteChangeWRT().
	 */
	inline daeOK stringToMemoryWRT(daeElement *e, const T &src)const
	{
		return _stringToMemoryWRT(e,typename daeBoundaryString2<T>::type(src));
	}
	/**Facilitates @c daeCharData and @c std::string. */
	inline daeOK _stringToMemoryWRT(daeElement *e, daeString src)const
	{
		_op_unserialize<daeOpaque> op(e,this,src,getWRT(e)); return noteChangeWRT(op);
	}
	/**Facilitates @c daeCharData and @c std::string. */
	inline daeOK _stringToMemoryWRT(daeElement *e, const daeHashString &src)const
	{
		_op_unserialize<daeString,daeOpaque> op(e,this,src,src+src.extent,getWRT(e)); return noteChangeWRT(op);
	}

	/**LEGACY
	 * Converts an element's attribute value to a string.
	 */
	inline daeOK memoryToStringWRT(const daeElement *e, daeArray<> &dst)const
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
		
	Data(daeElement*);

	inline const daeElement &getElement()
	{
		return *(daeElement*)(daeOffset(&_union)-_offset);
	}	
}; 

/**
 * Formerly "daeValue."
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
				   
COLLADA_(public) //HOW-TO? 
	/**THEORETICAL
	 * Formerly "getContainer()." 
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
	 */
	inline const XS::SimpleType &getSimpleType()const
	{
		return *_type.datatype; 
	}

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
	 * Formerly "getDefaultString."
	 * Gets the "default" for this value as a string.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=getDefault() indicates that the schema specifies a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * Converts an element's default value to a string.
	 * @note The generator provided string had been stored separately, but that is
	 * just taking up space, and only of very limited value. THAT SAID COLLADA-DOM
	 * isn't intended to replicate the schema precisely, and there is some LOSS in
	 * the conversion, for example "1.0" becomes "1" and "false" becomes 0, and so
	 * on. An additional API could doctor these strings, and client code can do so.
	 */
	inline daeOK defaultToString(daeArray<> &dst)const
	{
		return _type.writer->memoryToString(_type.value,dst);
	}

COLLADA_(public) //"WRT" APIs. (With Respect To.)
	
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
		assert(DAEP::Element::__DAEP__Element__meta(toElement,fromElement));
		assert(isValue());
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
		assert(DAEP::Element::__DAEP__Element__meta(elt1,elt2));
		assert(isValue());
		return getType()->compare(getWRT(elt1),getWRT(elt2));
	}	

	/**WARNING, LEGACY-SUPPORT
	 * @param cp Can be @c *this but is mandated for @c daeData::getDefault().
	 * 
	 * Copies the default value of this value to the element.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=getDefault() indicates that the schema specifies a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * @param toElement Pointer to a @c daeElement to copy the default value to.
	 * CHANGE-NOTICES
	 * Do "getType()->copy(getType().value,getWRT(element));" to bypass @c noteChangeWRT().
	 */
	inline void copyDefaultWRT(daeData &cp, daeElement *toElement)const
	{
		_op_assign op(toElement,&cp,_type.value); return !cp.noteChangeWRT(op);
	}

	/**WARNING, LEGACY
	 * @param cmp Can be @c *this but is mandated for @c daeData::getDefault().
	 *
	 * Compares the value of this value from the given element to the default value
	 * of this value.
	 * @warning This API doesn't check to see if the schema specifies a default or
	 * not. @c nullptr!=getDefault() indicates that the schema specifies a default.
	 * (If not, the string matches the result of one or other default-constructor.)
	 *
	 * @param e The element whose value should be compared to the default value.
	 * @return Returns a positive integer if value > default, a negative integer if 
	 * value < default, and 0 if value == default.
	 */
	inline bool compareIsDefaultWRT(daeData &cmp, const daeElement *e)const
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
	inline void setIsFixed(){ _fixed_default = 2; }

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

/**NEVER-CONST, VARIABLE-LENGTH
 * @c daeAnyAttribute is a new lightweight attributes container that
 * implements <xs:anyAttribute> semantics but is primarily concerned
 * with @c domAny.
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

	using _base::size; //getCount;
	using _base::empty;
	using _base::cbegin; //begin
	using _base::cend; //end
	using _base::capacity; //getCapacity;
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
		daeAllocThunk *t = (daeAllocThunk*)_au;
		assert((void*)&t->_counter==&t->_offset+1);
		return *(daePartition*)&t->_offset;
	}		 	

COLLADA_(public) //DAEP::Value

	/**CONST-ONLY (POINTER)
	 * Don't confused this with @c __COLLADA__atomize.
	 * Here. "Atom" is stemming from @c daeAtomicType.
	 */
	typedef daeData*const __COLLADA__Atom,*const Atom;
	/**STL-like*/
	typedef Atom value_type,*pointer,*iterator,&reference;
	
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
	Atom &operator[](const I &i_or_Name)
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
	Atom &_maybe_get(const I &i_or_Name)
	{
		return _get<1>((typename daeBoundaryString2<I>::type*)0,i_or_Name);
	}
	template<int, class I>
	/**Implements @c operator[](). */
	Atom &_get(daeString*, I Index)
	{
		//daeElement::getAttributeIndex() is returning a VOID
		//index equal to size() instead of -1 and so it helps
		//if users can access this index like any other index.
		//return _base::operator[i];
		//Adding (I) so 0 won't raise signed/unsigned warning.
		assert(Index<=(I)getCount()); return _au->_varray[Index];
	}
	template<int Maybe, class C_string>
	/**Implements @c operator[](). */
	Atom &_get(daeString*, C_string *Name)
	{
		return _get<Maybe>((daeName*)0,daeName(Name));
	}	
	template<int Maybe, class T>
	/**
	 * Implements @c operator[](). 
	 * @return Returns @c daeAtomicType::VOID type @c daeData
	 * if @c 1==Maybe or @c true==Name.empty().
	 */
	Atom &_get(daeName*, T &Name)
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
			cmp->getKnownValue().copyWRT(dst,src);
			else break;
		}
		if(i==cpN) return;
		//If not, the best that can be done is to set to defaults.
		//(Reminder: Planning to rectify this shortly. HOW?)
		size_t jN = 0;
		if(!clone&&!empty) 
		jN = _extern_clear(size(),i);
		grow(cpN);
		if(!clone) for(size_t j=i;j<jN;j++)
		{		
			daeData *clr = _au->_varray[j];
			clr->getKnownValue().copyDefaultWRT(*clr,dst);
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

COLLADA_(protected) //daeAA_Value implementation
	/**
	 * Prototype Constructor
	 */
	daeAnyAttribute():_base(*_au){ /*NOP*/ } 
	/**
	 * Destructor
	 * @todo C++11 Might want to delete this destructor.
	 */
	~daeAnyAttribute()
	{ 
		//daeElement::__0 is emptying/plugging assuming
		//the database has no need to access attributes.
		COLLADA_ASSUME(0==capacity()&&0==size())
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
		while(_au->_varray[hint]->isValue()) hint++;
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
		_au->setInternalCounter(i);
		//Note the main cost of having a VOID data-type on
		//the end is swapping these pointers. Possibly the
		//pattern is lost on compilers or may be too messy.
		//std::swap(_au->_varray[i-1],_au->_varray[i]);
		daeData**pi = _au->_varray+i;
		daeData *p = *pi; *pi = pi[-1]; pi[-1] = p;		
		//Note: Name can be a daeStringRef at this stage.		
		*(daeStringRef*)&p->_attribute_name_string = Name;
		//Note: Name can be a string-literal at this stage.		
		p->_set_attribute_name_length_etc(Name);		
	}

COLLADA_(private) //INTERNAL MEMORY FUNCTIONS

	//REMINDER: The primary motivation for these is so the
	//daeData attributes, once created, do not move around
	//in memory; Neither data nor metadata.
	friend class daeElement;
	inline daeAlloc<> *_head_AU();
	inline daeAlloc<> *_next_AU(daeAlloc<>*);
	inline void _llist_AU(daeAlloc<>*,void*);		
	inline void _extern_free(const void *AU);
				   
COLLADA_(public) //Formerly "daeElement::isAttributeSet."	

	/**NEVER-CONST, VARIABLE-LENGTH
	 * Packs/unpacks bit array behind the @c daeAnyAttribute
	 * pointer.
	 * @see @c daeAnyAttribute::mask() below.
	 * @see @c daeAA_Value 
	 * @see @c DAEP::Value<0,daeAnyAttribute> specialization.
	 */
	class Mask : public daeBit_bits<>
	{	
	COLLADA_(public)
		/**
		 * Gets the bit belonging to a static attribute that
		 * is at @a ID in the attribute array.
		 */
		inline const daeBit operator[](daeSize i)
		{
			daeBit o = daeBit_bits::operator[](i);
			if(i>=size()) o.clear(); return o;
		}

		/**
		 * Gets a bit with only an @c assert(i<size()) check.
		 */
		inline const daeBit bit(daeSize i)
		{
			assert(i<size());
			return daeBit_bits::operator[](i);
		}

		/**
		 * Tells if any static attributes exist and therefor
		 * if a variable-length bitfield exists.
		 */
		inline bool empty(){ return 0!=size(); }

		/**
		 * Gets the number of static attributes belonging to 
		 * the owning @c daeAnyAttribute container.
		 */
		inline daeSize size()
		{
			return ((daeAnyAttribute*)this-1)->_au->getOffset();
		}
	};	

	/**NON-PORTABLE (DAEP)
	 * Gets the attribute write-mask.
	 * @see @c daeAnyAttribute::Mask
	 */
	inline Mask &mask(){ return *(Mask*)(this+1); }	
};

template<>
/**TEMPLATE-SPECIALIZATION
 * @c DAEP::Value version of @c daeAnyAttribute not needing a mask.
 */
class daeAA_Value<> : public daeAA
{	
COLLADA_(protected)

	template<int,class,class CC,typename CC::_,class>
	friend class DAEP::InnerValue;

	template<class T>
	/**INTERNAL
	 * @c DAEP::Value Prototype Constructor
	 */
	daeAA_Value(const T&){ /*NOP*/ }

COLLADA_(public) //DAEP::Value implementation?

	//UNFINISHED? This class and daeTypedUnion require development.
	//Momentarily only their low-level feature-set are implemented.
};
template<int words>
/**VARIABLE-LENGTH
 * @c DAEP::Value version of @c daeAnyAttribute including the mask.
 * @tparam words is not "bits" because compilers struggle with all
 * of the templates.
 */
class daeAA_Value : public daeAA_Value<>
{	
COLLADA_(protected)

	daeOffset _write_mask_bits[words];

	template<int,class,class CC,typename CC::_,class>
	friend class DAEP::InnerValue;

	template<class T>
	/**INTERNAL
	 * @c DAEP::Value Prototype Constructor
	 */
	daeAA_Value(const T &pt):daeAA_Value<>(pt){ /*NOP*/ }
};

//DEFINING THIS HERE TO AVOID #including "daeDomTypes.h" PREMATURELY.
//---------------------.
	namespace DAEP //<-'
	{//-.
//<-----'	  
template<class CC, typename CC::_ PtoM>
/**PARTIAL-TEMPLATE-SPECIALIZATION
 * Adds a bitfield behind the attributes array. The bitfield is used
 * to mark attributes for inclusion/exclusion in outputted documents.
 * @note Each bit is a hint. The interpretation is subject to rather
 * convoluted criteria.
 */
class Value<0,daeAnyAttribute,CC,PtoM> 
:
public DAEP::InnerValue<0,daeAA,CC,PtoM,typename CC::COLLADA_DOM_0>
{
	COLLADA_DOM_INNER_OPERATORS //C2679

	//UNFINISHED: This class and daeTypedUnion require development.
	//Momentarily only their low-level feature-set are implemented.
};
//-------.
	}//<-'//.	
//<---------'

/**
 * @c XS::Attribute describes C++ COLLADA-DOM elements' attributes.
 * @note The common name of this class is @c daeAttribute. This is
 * so that it is not necessary to write @c const ahead of its name.
 * @see daeValue.
 */
class XS::Attribute : public daeDefault
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

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_META_ATTRIBUTE_H__
/*C1071*/
