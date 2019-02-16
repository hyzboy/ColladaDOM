/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_META_NAMESPACE_H__
#define __COLLADA_DOM__DAE_META_NAMESPACE_H__

#include "daeElement.h" 

COLLADA_(namespace)
{//-.
//<-'	

/**VARIABLE-LENGTH
 * This class corresponds to <xs:simpleType> elements.
 *
 * @c XS::Schema creates these alongside @c daeMetaElement. 
 */
class XS::SimpleType
{
	friend class XS::Schema;
	friend class XS::Attribute;

COLLADA_(private) //LEADING DATA-MEMBERS

	/** This is the <xs:simpleType> "name" attribute. */
	daeName _name; 
	/** @c XS::List is a @c const @c XS::SimpleType. */
	XS::List *_itemType;
	/** Does string<->binary. Thunk is sold separately. */
	daeTypewriter *_value_typewriter;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(XS::SimpleType)

COLLADA_(public) //<xs:simpleType>
	/**
	 * This is the type name provided by the generator.
	 * Historically it wasn't the schema's. Ideally it
	 * matches. @c XS::Schema::findType() looks for it.
	 */
	inline daeClientString2 getName()const{ return _name; }

	/**WARNING
	 * Gets the @c daeTypewriter that's used by @c daeValue based classes.
	 *
	 * @warning The real type is @c DAEP::Value::underlying_type, where T
	 * might be assumed to be the binary-representation. Everyone is free
	 * to specialize @c DAEP::Value. The library is free to do so as well.
	 */
	inline daeTypewriter &getValueTypewriter()const{ return *_value_typewriter; }

COLLADA_(public) //<xs:list>

  //////////////////////////////////////////////////////////////////////////
  //Presently <xs:list> IS @c daeArray. XML Schema forbids lists-of-lists.//
  //////////////////////////////////////////////////////////////////////////

	/** Implements XS::List::Item<T>. */
	template<class T> struct Item{ typedef T type; };	
	/** TEMPLATE-SPECIALIZATION Implements XS::List::Item<T>. */
	template<class T> struct Item<DAEP::Class<T>>{ typedef T type; };
	/** TEMPLATE-SPECIALIZATION Implements XS::List::Item<T>. */
	template<class T, int N> struct Item<daeArray<T,N>>{ typedef T type; };

   	/**
	* Tells if this <xs:simpleType> has an <xs:list> child element.
	* @return Returns @c true if @c getList() is a valid <xs:list>.
	*/
	inline bool hasList()const{ return _itemType!=this; }

	/**WARNING
	* Gets the <xs:list> "itemType" attribute.
	* @warning If this type does not have an <xs:list> then it returns itself.
	* @remarks This could have the same semantics as @c getRestriction(), but
	* it'd only be extra typing--arguably for uniformity sake.
	* Except a single "atom" is a valid list--so in some use-cases this setup
	* lets code not jump through @c hasList().
	*/
	inline XS::List &getList()const{ return *_itemType; }

COLLADA_(public) //<xs:restriction>
	/**WARNING
	* Gets the <xs:restriction> (whether it exists or not.)
	* @warning The returned address is not necessarily valid beyond the first
	* member: XS::Restriction::_base.
	*/
	inline XS::Restriction &getRestriction()
	{
		return (XS::Restriction&)_restriction;
	}
	/**WARNING, CONST-FORM
	* Gets the <xs:restriction> (whether it exists or not.)
	* @warning The returned address is not necessarily valid beyond the first
	* member: XS::Restriction::_base.
	* @see @c XS::Restriction::operator bool.
	*/
	inline const XS::Restriction &getRestriction()const
	{
		return (XS::Restriction&)_restriction;
	}
	/**CIRCULAR-DEPENDENCY
	 * Destructor
	 */
	~SimpleType()/*{ getRestriction().~Restriction(); }*/;

COLLADA_(private) //FINAL DATA-MEMBER

	friend class XS::Restriction;
	/**VARIABLE-LENGTH
	 * This is actually an @c XS::Restriction if @c _restriction!=nullptr.
	 */
	const XS::SimpleType *_restriction;
};

/**VARIABLE-LENGTH
 * This class corresponds to <xs:restriction> elements.
 *
 * @c XS::Schema embeds this class into @c XS::SimpleType.
 */
class XS::Restriction
{
	/**VARIABLE-LENGTH
	 * The <xs:restriction> "base" attribute.
	 * This is the first member. If @c nullptr the remaining struct is invalid.
	 * @note @c XS::Enumeration is the same way.
	 */
	const XS::SimpleType *_base;

COLLADA_(public) //STRUCT VALIDATION OPERATOR
	/**IMPORTANT
	 * @c XS::Restriction is embedded inside XS::SimpleType. @c base exists
	 * if the <xs:simpleType> includes <xs:restriction>.
	 */
	inline operator bool()const{ return _base!=nullptr; }

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(XS::Restriction)

COLLADA_(private) //DATA-MEMBERS
	/**
	 * <xs:length> or <xs:minLength> and/or <xs:maxLength> decomposed as if
	 * minima and maxima are provided separately.
	 */
	int _length[2];
	/**WARNING
	 * @warning @c _minmax is in terms of @c DAEP::InnerValue::underlying_type.
	 * <xs:minInclusive>, <xs:minExlusive>, <xs:maxInclusive>, <xs:maxExlusive>
	 * expressed as binary values with initially nullified clusivity semantics.
	 */
	const daeAlloc<> *_minmax[2]; int _compare[2];

COLLADA_(public) //<xs:restriction>
	/**
	 * Gets the @c XS::SimpleType that the "base" attribute
	 * refers to. This isn't the type's name, like in schema.
	 * There's @c getName() for that. Still, the library won't
	 * likely use this. It's for GUIs, consoles, analyzers, etc.
	 */
	inline const XS::SimpleType &getBase()const
	{
		assert(*this); return *_base; 
	}

	/**
	 * Gets the derived type, in case its underlying C++ type 
	 * is different from the base's type.
	 */
	inline const XS::SimpleType &getRestrictedType()const
	{
		return daeOpaque(this)[-daeOffsetOf(XS::SimpleType,_restriction)]; 
	}

COLLADA_(public) //<xs:length>, <xs:minLength>, <xs:maxLength>
	/**WARNING
	 * @warning <xs:minLength> is unrelated to the other
	 * "getMinX" methods. This value is currently INT_MIN
	 * even if <xs:length> or <xs:minLength> aren't present.
	 * There's no way to know if they are. That isn't the job
	 * of this library.
	 */
	inline unsigned getMinLength()const{ return _length[0]; }
	/**WARNING
	 * @warning <xs:maxLength> is unrelated to the other
	 * "getMaxX" methods. This value is currently INT_MAX
	 * even if <xs:length> or <xs:maxLength> aren't present.
	 * There's no way to know if they are. That isn't the job
	 * of this library.
	 */
	inline unsigned getMaxLength()const{ return _length[1]; }
	/**GENERATOR-API
	 * Sets both <xs:minLength> and <xs:maxLength> together,
	 * -or optionally acts like <xs:length> in lieu of @a Max.
	 */
	inline void addLength(int Min=0, int Max=0)
	{
		if(Max==0) Max = Min; assert(Min>=0&&Max>=Min);
		_length[0] = Min; _length[1] = Max;
	}

COLLADA_(public) //<xs:min/maxInclusive>, <xs:min/maxExlusive>	

  //////////////////////////////////////////////////////////////////
  //Note that the library doesn't actually use this. It's provided//
  //as support; chiefly for editors. Implementing comparison isn't//
  //impossible, but it's very tricky to get right. There's no APIs//
  //in this class for actually comparing min/max to binary values.//
  //////////////////////////////////////////////////////////////////

	/**
	 * Tells if <xs:minInclusive> or <xs:minExclusive> is present.
	 */
	inline bool hasMin()const{ return INT_MIN!=_compare[0]; }
	/**
	 * Tells if <xs:maxInclusive> or <xs:maxExclusive> is present.
	 */
	inline bool hasMax()const{ return INT_MAX!=_compare[1]; }

	/**
	 * This seems like a common refrain. 
	 */
	daeTypewriter &getMinMaxTypewriter()const
	{
		daeTypewriter &tw = getRestrictedType().getValueTypewriter();
		assert(!tw.isAnySimpleType()); //Can be problem?
		return tw.where<daeArray>();
	}

	/**
	 * Formerly "getMinAU."
	 * Unfortunately <xs:minInclusive> and <xs:minExclusive> are not so simple.
	 * This API provides access to the binary form. 
	 * @return Returns an empty AU if @c hasMin()==false. A referenced pointer
	 * is returned so @c daeTypewriter::compare(&r.getMinAU()) works on arrays.
	 */
	const daeAlloc<>*const &getMinValueTypeAU()const{ return _minmax[0]; }
	/**
	 * Formerly "getMaxAU."
	 * Unfortunately <xs:maxInclusive> and <xs:maxExclusive> are not so simple.
	 * @see getMinValueTypeAU() Doxygentation.
	 */
	const daeAlloc<>*const &getMaxValueTypeAU()const{ return _minmax[1]; }
	
	/**WARNING
	 * @warning This is meaningless if @c !hasMin(). It will @c assert().
	 * Gets the <xs:minInclusive> or <xs:minExclusive> value as a string.
	 */
	inline daeOK getMin(daeArray<> &dst)const
	{
		assert(hasMin()); return _getMinMax(0,dst); 
	}
	/**WARNING, OVERLOAD
	 * @warning This is meaningless if @c !hasMin(). It will @c assert().
	 * Gets the <xs:minInclusive> or <xs:minExclusive> value as a string.
	 */
	inline const daeArray<> &getMin
	(class undefined*_=0,const daeArray<>&def=daeCharData())const
	{
		assert(hasMin());
		_getMinMax(0,const_cast<daeArray<>&>(def)); return def;
	}

	/**WARNING
	 * @warning This is meaningless if @c !hasMax(). It will @c assert().
	 * Gets the <xs:maxInclusive> or <xs:maxExclusive> value as a string.
	 */
	inline daeOK getMax(daeArray<> &dst)const
	{
		assert(hasMax()); return _getMinMax(1,dst); 
	}
	/**WARNING, OVERLOAD
	 * @warning This is meaningless if @c !hasMax(). It will @c assert().
	 * Gets the <xs:maxInclusive> or <xs:maxExclusive> value as a string.
	 */
	inline const daeArray<> &getMax
	(class undefined*_=0,const daeArray<>&def=daeCharData())const
	{
		assert(hasMax()); 
		_getMinMax(1,const_cast<daeArray<>&>(def)); return def;
	}	
																  
	/**
	 * Returns an @c int value to compare to a @c memcmp style
	 * comparison. It's satisfactory @c if(It>=getMinCompare()).
	 */
	inline int getMinCompare()const{ return _compare[0]; }
	/**
	 * Returns an @c int value to compare to a @c memcmp style
	 * comparison. It's satisfactory @c if(It<=getMaxCompare()).
	 */
	inline int getMaxCompare()const{ return _compare[1]; }
					   
	/**
	 * If this is @c true then there is no way to set out-of-bound
	 * values to an in-bounds value, via @c getMin().
	 */
	inline bool isMinExclusive()const{ return 1==_compare[0]; }
	/**
	 * If this is @c true then there is no way to set out-of-bound
	 * values to an in-bounds value, via @c getMax().
	 */
	inline bool isMaxExclusive()const{ return -1==_compare[1]; }

	/**GENERATOR-API
	 * Sets up <xs:minInclusive> so that: compare(a,b)>=r.getMinCompare();
	 */
	inline void addMinInclusive(daeHashString value){ _addMinMax(0,0,value); }
	/**GENERATOR-API
	 * Sets up <xs:minExclusive> so that: compare(a,b)>=r.getMinCompare();
	 */
	inline void addMinExclusive(daeHashString value){ _addMinMax(0,1,value); }
	/**GENERATOR-API
	 * Sets up <xs:maxInclusive> so that: compare(a,b)<=r.getMaxCompare();
	 */
	inline void addMaxInclusive(daeHashString value){ _addMinMax(1,0,value); }
	/**GENERATOR-API
	 * Sets up <xs:maxExclusive> so that: compare(a,b)<=r.getMaxCompare();
	 */
	inline void addMaxExclusive(daeHashString value){ _addMinMax(1,-1,value); }

COLLADA_(private)

	friend class XS::Schema;

	/** Implements @c getMin() and @c getMax(). */
	inline daeOK _getMinMax(int i, daeArray<> &dst)const
	{ 
		return getMinMaxTypewriter().memoryToString(_minmax+i,dst); 
	}
	/** Implements @c setMinInclusive(), etc. */
	inline void _addMinMax(int i, int compare, const daeHashString &value)
	{
		if(DAE_OK==getMinMaxTypewriter().stringToMemory(value,_minmax+i))
		_compare[i] = compare; else assert(0);
	}  
	template<class Atom>
	/**
	 * @c XS::Schema calls this to ensure there is a valid object in the
	 * min/max memory. It's easier to use arrays than to save a destructor
	 * and pre-allocate space for the objects. Either way access is fraught.
	 */
	inline void _setMinMax()
	{
		_minmax[0] = _minmax[1] = &daeAlloc<Atom>::localThunk(); assert(!hasMin()&&!hasMax());
	}

	friend class XS::SimpleType;
	/**INTERNAL
	 * Private Destructor
	 * The min/max allocation-units must be destructed/deallocated if present.
	 */
	~Restriction()
	{
		if(*this) for(int i=0;i<2;i++) if(0!=_minmax[i]->getCapacity())
		{
			//In this case, this should work. 
			getMinMaxTypewriter().stringToMemory(empty_daeString1,_minmax+i);
			_minmax[i]->deleteThis();
		}
	}

COLLADA_(public) //<xs:enumeration>
	/**WARNING
	* Gets the <xs:enumeration> (whether it exists or not.)
	* @warning The returned address is not necessarily valid beyond the first
	* member: XS::Enumeration::_size.
	* @see @c XS::Enumeration::operator bool.
	*/
	inline XS::Enumeration &getEnumeration()const
	{
		return (XS::Enumeration&)_enumeration;
	}

COLLADA_(private) //FINAL DATA-MEMBER

	/**VARIABLE-LENGTH
	 * This is actually an @c XS::Enumeration if @c _enumeration!=0.
	 */
	size_t _enumeration;
};

/**VARIABLE-LENGTH
 * This class corresponds to <xs:enumeration> elements.
 * @c XS::Schema embeds this class into @c XS::Restriction.
 *
 * @c XS::Enumeration comprises a series of <xs:enumeration> elements.
 * It has @c operator[] so to be more in keeping with how the element
 * appears in an XML Schema description file. Despite this, the order
 * is not necessarily congruent, as it depends on the integral values.
 * Normally these values cannot be specified, but COLLADA's schema do
 * something like "<xs:appinfo>value=0x0</xs:appinfo>" in order to do
 * so.
 */
class XS::Enumeration
{		
	/**VARIABLE-LENGTH
	 * The size of the @c _enum array and @c string_table.
	 * The number of <xs:enumeration> elements in this @c enum.
	 * This is the first member. If zero the remaining struct is invalid.
	 * @note @c XS::Restriction is the same way.
	 */
	size_t _size; 

COLLADA_(public) //STRUCT VALIDATION OPERATOR
	/**IMPORTANT
	 * @c XS::Enumeration is embedded inside XS::Restriction. @c base exists
	 * if the <xs:restriction> includes <xs:enumeration>.
	 */
	inline operator bool()const{ return _size!=0; }

COLLADA_(public) //SORTED STRING-TABLE ACCESS
	/**
	 * This class/object is parallel to the main @c enum like array.
	 * It's packaged this way to distinguish its string-like nature.
	 */
	class string_table
	{
		friend class XS::Schema;
		friend class XS::Enumeration;
		/**
		 * Similar to @c XS::Enumeration::_enum and its end-pointer.
		 * In this case it's ordered according to @c strcmp results.
		 */
		const daeClientEnum *_begin, *_end;

	COLLADA_(public)
		/**
		 * Gets the @c daeClientEnum object at @a i in the @c string_table.
		 */
		inline const daeClientEnum &operator[](size_t i)const
		{
			assert(_begin+i<_end); return _begin[i]; 
		}
		
		/**
		 * Forms a pointer-based "STL" style iterator for <algorithm>.
		 */
		inline const daeClientEnum *begin()const{ return _begin; } 
		/**
		 * Forms a pointer-based "STL" style delimiter for <algorithm>.
		 */
		inline const daeClientEnum *end()const{ return _end; } 

	}string_table;

COLLADA_(private)  	

	friend class XS::Schema;
	/**
	 * Pointers to client memory, sorted according to the integral constants.
	 * They may be equal to @c string_table if certain conditions can be met.
	 */
	const daeClientEnum *_enum, *_enumEnd;

	/** Implements @c find(). */
	static bool _strt_predicate(const daeClientEnum &a, daeString b)
	{
		return strcmp(a.string_constant,b)<0;
	}
	/** Implements @c find(). */
	static bool _enum_predicate(const daeClientEnum &a, daeEnumeration b)
	{
		return a.integral_constant<b;
	}

COLLADA_(public) //ARRAY-LIKE SEMANTICS
	/**
	 * Gets nonzero extent of the @c enum like array and @c string_table.
	 */
	inline size_t size()const{ return _size; }

	/**
	 * Gets the lowest @c enum like value. It can be negative.
	 */
	inline daeEnumeration floor()const{ return _enum[0].integral_constant; } 
	/**
	 * Gets the largest @c enum like value. It can be negative.
	 */
	inline daeEnumeration ceiling()const{ return _enumEnd[-1].integral_constant; } 

	/**
	 * Gets the @c daeClientEnum object at @a i in the @c enum like array.
	 */
	inline const daeClientEnum &operator[](size_t i)const{ assert(i<_size); return _enum[i]; }

	/**
	 * Forms a pointer-based "STL" style iterator for <algorithm>.
	 */
	inline const daeClientEnum *begin()const{ return _enum; } 
	/**
	 * Forms a pointer-based "STL" style delimiter for <algorithm>.
	 */
	inline const daeClientEnum *end()const{ return _enumEnd; } 

	/**
	 * Binary lookup by sorted @c string_table.
	 * @return Returns @c nullptr if @a value is not present.
	 */
	inline const daeClientEnum *find(daeString value)const
	{
		const daeClientEnum *lb = 
		std::lower_bound(string_table._begin,string_table._end,value,_strt_predicate);
		if(lb!=string_table._end&&0==strcmp(value,lb->string_constant)) 
		return lb; return nullptr;
	}
	/**
	 * Binary lookup by sorted @c enum like key.
	 * @return Returns @c nullptr if @a value is not present.
	 */
	inline const daeClientEnum *find(daeEnumeration value)const
	{	
		size_t n = value-_enum->integral_constant;
		if(value<0||value>=(int&)_size||value!=_enum[n].integral_constant)
		{
			n = std::lower_bound(_enum,_enumEnd,value,_enum_predicate)-_enum;
			if(n==_size||value!=_enum[n].integral_constant) 
			return nullptr;
		}return _enum+n;
	}

COLLADA_(public) //DAEP::Value SPECIALIZATION

	template<class SpecializedTypist>
	/**
	 * Don't call this unless inside a context where you know what
	 * to plug into @a SpecializedTypist.
	 * @note "knowing" includes being in the right linkage-context.
	 * @see @c XS::Schema::_addValueType() for non DAEP Class enum.
	 */
	inline daeType<daeEnumeration,SpecializedTypist> &getValueType()const
	{
		return (daeType<daeEnumeration,SpecializedTypist>&)this[1];
	}
};

/**EXPERIMENTAL
 * @see @c XS::Schema::getXMLNS().
 *
 * This can have a namespace and/or a schema matching
 * The namespace. If there isn't a namespace a schema
 * has imported something like <xs:group> that has in
 * turn imported something. In that case no namespace
 * is implicated.
 * @note This class corresponds to <xs:import> of the
 * XML Schema recommendation.
 */
class XS::Import
{	
COLLADA_(public) //MUTATORS	
	/**
	 * Namespace key and schema mapping/value.
	 * The schema is not known until a module
	 * loads it, and content-models import it.
	 * This may never happen. Likewise if the
	 * schema is not explicitly referenced it
	 * will not have a namespace. This can be
	 * the case when an <xs:group> element is
	 * imported.
	 */
	daeTags _xmlns; const XS::Schema *_schema;

	/**
	 * Constructor
	 */
	Import(daeTags ns=0, const XS::Schema *xs=nullptr)
	:_xmlns(ns),_schema(xs){}	

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;

	template<class I>
	/**
	 * Facilitates @c daeArray::find().
	 * @return Returns @c hasNamespaceTag(cmp).
	 */
	inline bool operator==(const I &cmp)const
	{
		return _xmlns.namespaceTag==(0xFF&cmp);
	}
	/**OVERLOAD
	 * Facilitates @c daeArray::find().
	 */
	inline bool operator==(const XS::Schema *cmp)const
	{
		return _schema==cmp;
	}	
	/**OVERLOAD
	 * Facilitates @c daeArray::find().
	 */
	inline bool operator==(const XS::Schema &cmp)const
	{
		return _schema==&cmp;
	}	
	/**OVERLOAD
	 * Facilitates @c daeArray::find().
	 */
	inline bool operator==(const XS::Import &cmp)const
	{
		return 0==memcmp(this,&cmp,sizeof(cmp));
	}
	
COLLADA_(public) //SCHEMA ACCESSORS
	/**
	 * Gets <xs:import schemaLocation> or maybe? even
	 * xsi:schemaLocation depending on how the client
	 * implements these features, or if the generator
	 * has filled this out for the schema accordingly.
	 * @return Returns @c "" if @c false==hasSchema().
	 */
	inline daeStringRef_base getSchemaLocation()const
	{
		if(nullptr==_schema) //XS::Schema is incomplete.
		return *(daeStringRef_base*)&daeStringRef_empty;
		return ((daeProcessShare*)_schema)->getLocation(); 
	}

	/**
	 * @return false if @c nullptr==getSchema().
	 * @see @c getSchema() Doxygentation.
	 */
	inline bool hasSchema()const{ return _schema!=nullptr; }

	/**
	 * @return Returns @c nullptr if @c false==hasSchema().
	 * @note An @c XS::Schema may not be associated if the
	 * schema is not loaded by the software or if no child
	 * uses the schema. 
	 * Right now data isn't divvied up among @c XS::Schema.
	 * As such, imported data types (xs:simpleType) do not
	 * link the schema.
	 */
	inline const XS::Schema *getSchema()const{ return _schema; }

COLLADA_(public) //NAMESPACE ACCESSORS

	template<class I>
	/**
	 * True if @c getNamespaceTags().namespaceTag is equal
	 * to @c cmp&0xFF.
	 */
	inline bool hasNamespaceTag(I cmp)const
	{
		return _xmlns.namespaceTag==(0xFF&cmp);
	}	
		
	/**
	 * Gets at the underlying @c daeElementTags object. Of
	 * which only @c namespaceTag and @c nameTag are valid.
	 * @note These do not correspond to element namespaces
	 * in the DOM, although they are compatible.
	 */
	inline daeTags getNamespaceTags()const{ return _xmlns; }

	/**
	 * Tells if this namespace/schema prefixes its element
	 * and simpleType references, etc. If not it is either
	 * a default namespace, or @c false==hasNamespace().
	 */
	inline bool hasQNamePrefix()const{ return 1>_xmlns.nameTag; }
															
	/**
	 * Tells if @c getSchema() has a corresponding xmlns declaration.
	 * @note @c getNamespaceTags().found() is equivalent.
	 */
	inline bool hasAttribute_xmlns()const{ return 0!=_xmlns.nameTag; }

	/**
	 * Gets an xmlns or <xs:import namespace> depending on
	 * the context. Currently @c XS::Schema::getXMLNS() is 
	 * the only context. It repurposes this class since it
	 * has everything it needs.
	 */
	inline daeStringRef_base getNamespace()const{ return _xmlns.first(); }

	/**
	 * Gets the colonized QName prefix used by the element
	 * and simpleType references, etc. using the namespace. 
	 */
	inline daeStringRef_base getQNamePrefix()const{ return _xmlns.second(); }
};

/**C++
 * This class holds invisible members of @c XS::Schema.
 * @see "__XS__Schema__construct/destruct()."
 */
struct __Schema__invisible
{	   	
#ifdef BUILDING_COLLADA_DOM
	
	/**
	 * This STL allocator is not always used
	 * if doing so would leak memory for the
	 * lifetime of the schema. Use is spotty.
	 * @c daeArray can't readily use it. And
	 * memory is lost whenever @c _reserve()
	 * is called. @c daeMeta uses it as much
	 * as it can to fill itself and @c daeCM 
	 * graph out.
	 */
	daeStringAlligator<> _A;
	daeStringAllocator<> _a(){ return &_A; }
	__Schema__invisible():_A(1024*sizeof(void*))
	{}

	/**
	 * The "complex-types" that are children of
	 * the <xs:schema> element.
	 */
	daeStringMap<daeMeta*> _globalTypes;

	/**
	 * All of the "simpleTypes" from the schema
	 * according to their names.
	 */
	daeStringMap<XS::SimpleType*> _simpleTypes;	
	
	/**
	 * Token returned by @c daeMetaElement::addCM().
	 */
	mutable XS::Element _temporary_working_memory;
	struct _MissingChild
	{
	daeMeta *ref,*meta; XS::Group *CM_graph_node;
	daeFeatureID fid; daeOffset os; daePseudonym name;
	};
	/**
	 * In order to transclude <xs:group> it's very seldom
	 * necessary to defer metadata registration until the 
	 * source is itself registered.
	 * Group inheritance is more complicated than regular
	 * base-class like inheritance.
	 */
	mutable std::vector<_MissingChild> _missingChildren;

	/**SKETCHY	 
	 * @c _agent was originally conceived of as being
	 * something like @c __DAEP__Make__maker.
	 * It's dual use. It can output information that is
	 * able to change. It's not clear how this is useful.
	 * It really depends on the client. 
	 * Because the schema is passed to it, it's positioned
	 * to monitor it, taking any action.
	 */
	daeClientString (*_agent)(XS::Schema*);
	/**
	 * @c _targetNamespace is <xs:schema> "targetNamespace."
	 * @c _version is <xs:schema> "version". COLLADA uses this.
	 */
	daeName _targetNamespace, _version;	

	/**SKETCHY
	 * Must be set from inside @c XS::Schema::setAgent() via @c _agent.
	 * @see XS::Schema::getIDs(). 
	 */
	daeArray<daeName,2> _IDs;

	/**
	 * Currently set if a namespace index cannot be allocated because
	 * there are more schemas and/or xmlns references than the system
	 * is designed to handle.
	 */
	daeError _error;

	/**
	 * @c daeMeta object tags.
	 */
	daeElementTags _elementTags; 

	/**
	 * @c addXMLNS() fills up this array.
	 */
	daeArray<XS::Import,10> _XMLNS;

	/**EXPERIMENTAL
	 * Counts how many documents contain (or once contained) elements
	 * using @c this schema.
	 * @see @c daeDocument::getSchemata().
	 */
	mutable size_t _users;

#endif //BUILDING_COLLADA_DOM
};

#include "../LINKAGE.HPP" //#define LINKAGE	

/** 
 * Post-2.5 generators house simple-types in schemas
 * so DOMs can share them, and there can be more than one.
 * The default xmlns and COLLADA version are discovered this way.
 *
 * @note This class corresponds to <xs:schema> of the XML Schema
 * recommendation.
 *
 * IMPORTANT NOTES 
 * ===============
 * @c XS::Schema is conceptually an XML Schema Instance namespace.
 * Still. apart from the "XS" namespace, it doesn't know about anything
 * that the schema it represents has imported, so you either need to use XS
 * as base types, or go ahead and define any imported types the schema requires.
 *
 * Elements also belong to the namespace. However they are registered automatically
 * as they are created. So it's only the case that the simple-types need be registered. 
 */
class XS::Schema : public daeProcessShare_base, private __Schema__invisible
{	
	friend class XS::Group;
	friend class XS::Element;	
	friend class COLLADA::daeDocument;
	friend class COLLADA::daeMetaElement;

COLLADA_(public)

	typedef void __COLLADA__atomize;

	enum{ __size_on_client_stack=256*sizeof(void*) };

#ifndef BUILDING_COLLADA_DOM

	char __client_padding[__size_on_client_stack-sizeof(daeProcessShare)];

		enum{ __invisible=1 };
#else	
		enum{ __invisible=0 };	

	explicit Schema(int); /**< @c domAny's Constructor */
	
#endif //BUILDING_COLLADA_DOM	
			  	
	//SCHEDULED FOR REMOVAL/REFACTOR
	/**EXPEDIENT
	 * Switching to using a shared allocator for XML Schema.
	 * @c __DAEP__Make__v1__operator_new() may need thought.
	 */
	virtual void *__DAEP__Make__v1__operator_new(size_t chars)const
	{
		return __XS__Schema__new(chars); //GCC
	}
	/**EXPEDIENT
	 * Switching to using a shared allocator for XML Schema.
	 * @c __DAEP__Make__v1__operator_new() may need thought.
	 */
	virtual void __DAEP__Make__v1__delete(const DAEP::Object *obj)const
	{
		return __XS__Schema__delete(obj); //GCC
	}

COLLADA_(public)
	/**
	 * Constructor
	 * This must happen in the user/client's build context.
	 * @param callback is one way for generators to set up
	 * a registration routine that's tied to a constructor.
	 */
	Schema(daeClientString2 targetNamespace, daeClientString2 version)
	{
		__XS__Schema__construct(__invisible,
		COLLADA_DOM_PHILOSOPHY,COLLADA_DOM_PRODUCTION,
		COLLADA_DOM_GENERATION,targetNamespace,version);
	}
	/**
	 * Virtual Destructor
	 * This must happen in the user/client's build context.
	 */
	virtual ~Schema(){ __XS__Schema__destruct(__invisible); }
	 	
COLLADA_(private)

	/** Implements @c Schema::Schema(). */
	LINKAGE void __XS__Schema__destruct(bool);
	/** Implements @c Schema::Schema(). */
	LINKAGE void __XS__Schema__construct(bool,int,int,int,daeClientString2,daeClientString2);

	/** GCC's linker isn't working with exported virtual methods. */
	LINKAGE void *__XS__Schema__new(size_t chars)const
	SNIPPET( return const_cast<XS::Schema*>(this)->_a().allocate(chars); )
	/** GCC's linker isn't working with exported virtual methods. */
	LINKAGE void __XS__Schema__delete(const DAEP::Object *obj)const
	SNIPPET( /*delete obj;*/ )

COLLADA_(public) //ACCESSORS
	/**
	 * Gets a count of how many documents (or other use-cases) 
	 * are in existence. A document doesn't remove itself from
	 * the count until it is deleted. In addition, each schema
	 * that is referenced by another has one more "user" while
	 * the referencing schema is in memory. There some remarks
	 * on cyclical references below.
	 * @see @c daeDocument::getSchemata().
	 * 
	 * If software is well-behaved, it can use this counter to
	 * decide if the schema itself can be unloaded from memory.
	 *
	 * @remarks Currently when a schema imports another schema
	 * it marks it as in use, as it depends on its meta object
	 * tree. If schema are interwed this methodology will form
	 * a "cycle" in the user graph, making it difficult to say
	 * if either can be unloaded. Since this is a hypothetical
	 * and at worst is merely suboptimal, it's not going to be
	 * fixed. @c getXMLNS() can be used to detect the scenario.
	 * Every schema in it that is not @c this will have a lock.
	 * Still, there is not an API way to break the mutual lock.
	 */
	NOALIAS_LINKAGE size_t getUsers()const
	SNIPPET( return _users; )

	/**WARNING
	 * This gets an array with a list of "id" attribute like
	 * attribute names. For global attributes they can be QName
	 * like, otherwise they must be local to this schema. The
	 * types must be @c daeAtomicType::TOKEN.
	 * @see daeMetaElement::getFirstID().
	 * @warning
	 * This list must be modified before the schema is filled
	 * in. Therefore the @c setAgent() system must be used. It
	 * initially includes "id" and "sid" for legacy purposes. It
	 * might be necessary to remove "sid."
	 */
	NOALIAS_LINKAGE daeArray<daeName> &getIDs()
	SNIPPET( return _IDs; )

	/**
	 * The generator calls this to link the module to the namespace.
	 * The "agent" can initialize this or that. Originally the idea
	 * was to preload the schema with extended simple-types so that
	 * any missing "base" types could be covered. But now it's done
	 * with templates, so types must be present/defined in any case.
	 * @see @c getError()
	 */
	LINKAGE void setAgent(daeClientString(*f)(XS::Schema*))
	SNIPPET( _agent = f; if(f!=nullptr) f(this); )	

	/**
	 * Tells if the schema is in an unworkable state. The user must
	 * call @c getError() and handle the status in order to prevent
	 * data loss or corruption. Error messages are emitted over the
	 * @c daeErrorHandler system. 
	 * Possible errors should be documented below:
	 * 1) If the number of namespaces exceeds 255 the system cannot
	 * track them. This includes xmlns references. @c daeObjectTags
	 * is an 8-bit data structure. 255 is the limit of 8-bit fields.
	 */
	NOALIAS_LINKAGE daeError &getError()
	SNIPPET( return _error; )	

	/**
	 * Gets the "targetNamespace" attribute of this <xs:schema>.
	 */
	NOALIAS_LINKAGE daeClientString2 getTargetNamespace()const 
	SNIPPET( return _targetNamespace; )

	/**
	 * Gets the @c daeElementTags that is copied into the target
	 * namespace elements.
	 */
	NOALIAS_LINKAGE daeTags getTargetNamespaceTags()const 
	SNIPPET( return _elementTags; )

	/**
	 * May be an empty string. A generator can set this if it has
	 * a canonical schema location. If not, it will be set if one
	 * of <xs:import schemaLocation> or xsi:schemaLocation refers
	 * to @c this schema.
	 */
	inline daeStringRef_base getSchemaLocation()const
	{
		return __DAEP__Make__location; 
	}

	/**
	 * This is provided for @c daePlatform::introduceToDocument()
	 * to use to look up imported schemata. If it must do so then
	 * an implementation must be recursive to find the namespaces
	 * of elements imported by the imported namespaces, etc., etc.
	 * It's important to appreciate that the namespaces may cycle.
	 *
	 * @note This is not in itself a copy of <xs:import> elements
	 * as implied by its return type. It repurposes @c XS::Import.
	 */
	NOALIAS_LINKAGE const daeArray<XS::Import> &getXMLNS()const
	SNIPPET( return _XMLNS; )

	/**
	 * Gets the "version" attribute of this <xs:schema>. This is
	 * an optional attribute that COLLADA uses. It is however part
	 * of the <xs:schema> element's schema definition.
	 */
	NOALIAS_LINKAGE daeClientString2 getVersion()const 
	SNIPPET( return _version; )

	/**
	 * Gets @c COLLADA_DOM_GENERATION as it was defined when the
	 * library was built. In theory the library could implement
	 * more than one "generation," supplying the correct version
	 * to the client's build. It's more likely however that if
	 * there is a mismatch, either the builds are incompatible, 
	 * -or you can just cross your fingers.
	 */
	NOALIAS_LINKAGE	int getGeneration()const 
	SNIPPET( return COLLADA_DOM_GENERATION; )
	
	template<class T>
	/**GENERATOR-API (though potentially avoidable)
	 * Acquire the @c XS::SimpleType for linking simple-types to shared metdata.
	 * @remarks @c findType() is used only briefly by the metadata registration step.
	 * @see @c addType().
	 */
	inline const XS::SimpleType *findType(const T &QName)const
	{
		return _findType(typename daeBoundaryString2<T>::type(QName));
	}
	/** Implements @c findType(). */
	NOALIAS_LINKAGE const XS::SimpleType *_findType(daeString QName)const
	SNIPPET( return _findType(daeName(QName)); )
	/** Implements @c findType(). */
	NOALIAS_LINKAGE const XS::SimpleType *_findType(const daeName &QName)const
	SNIPPET
	(
		daeStringMap<XS::SimpleType*>::const_iterator it = _simpleTypes.find(QName);
		return it==_simpleTypes.end()?nullptr:it->second;
	)

	template<class T>
	/**WARNING
	 * Implements @c XS::Any::findChild().
	 * This can be used to locate the @c daeMetaElement of non-simpleType types
	 * that happen to be immediate children of the <xs:schema> element. In theory,
	 * -any of these types can be the root node of the XML document--or so it seems.
	 * @see @c addElement().
	 *
	 * @param NCName is not a QName, because internal lookup is done on NCNames.
	 * If a schemata uses a prefixed namespace for itself then the keys in this
	 * table need to strip the prefix part out. Schemata aren't expected to use
	 * mixed prefixes for their own types. Debug builds are checking for colons.
	 * (@c addXMLNS() can be of use to implement prefixed schemata if need be.)
	 *
	 * @warning ////////////////////////////////////////////////////////////////
	 * /////NOTE: For COLLADA 1.5, despite what the PDF says, inside <technique>
	 * /////it's correct to not validate elements that are not <COLLADA>. So, if
	 * /////<param> appears, it's not validated. Unless <param_type> is used. As
	 * /////that's the type's real name in the global namespace. A workaround is
	 * /////to switch the <technique> over to the 1.4 namespace during authoring.
	 */
	inline daeMeta *findChild(const T &NCName)const
	{
		return _findChild(typename daeBoundaryString2<T>::type(NCName));
	}
	/** Implements @c findChild(). */
	NOALIAS_LINKAGE daeMeta *_findChild(const daeName &NCName)const
	SNIPPET
	(
		daeStringMap<daeMeta*>::const_iterator it = _globalTypes.find(NCName);
		return it==_globalTypes.end()?nullptr:it->second;
	)	
	/** Implements @c findChild(). */
	NOALIAS_LINKAGE daeMeta *_findChild(daeString NCName)const
	SNIPPET( return _findChild(daeName(NCName)); )

COLLADA_(protected) //GENERATOR-SIDE APIs
	
	/**GENERATOR-SIDE API, WARNING
	 * Adds an xmlns attribute from the main xs:schema element. A default xmlns format
	 * places its value in the first parameter; otherwise it os the namespace's prefix.
	 *
	 * @warning This facility wants schemas to use namespace prefixes consistently and
	 * if they do not, code generators will have to output alternative prefixes. Right
	 * now the stock generator is only taking xmlns attributes from xs:schema elements.
	 */	
	NOALIAS_LINKAGE void addXMLNS(daeClientString2 key_or_value, daeClientString2 value=nullptr);

	/**INTERNAL, MUTABLE
	 * Links schema to @c addXMLNS() and @c getXMLNS(). Note the metadata registration
	 * process uses this to fill out the import table as it goes. It's not intended to
	 * be used otherwise. It's exported and not prefixed with an underscore because it
	 * seems to have a primary role to play, even if in practice it might not have one.
	 *
	 * @return In addition to adding the schema to the import table, if it's not there
	 * already, the matching namespace is returned. In this way it doubles as a lookup
	 * and insertion procedure. 
	 */
	LINKAGE daeTags useXMLNS(const XS::Schema&);

	template<class Atom_or_List>
	/**GENERATOR-SIDE API
	 * Adds an <xs:simpleType> element to the <xs:schema>.
	 * This form of @c addType() permits explicit invocation of the template parameter.
	 * Basically it's to work around C++'s inability to partially-specialize functions.
	 * @note This is now converting to @c DAEP::Value<0,Atom_or_List>::underlying_type.
	 * @see @c _addValueType() Doxygentation.
	 */	
	inline XS::SimpleType &addType(daeClientString2 list_or_base)
	{
		//DAEP::Value can specialize Atom to be a different type.
		typename DAEP::Value<0,Atom_or_List>::underlying_type *nul;
		return _addValueType(nul,list_or_base,daeHashString(nullptr,0));
	}
	template<class Atom_or_List>
	inline XS::SimpleType &addType(daeClientString2 list_or_base, daeClientString2 name)
	{
		//DAEP::Value can specialize Atom to be a different type.
		typename DAEP::Value<0,Atom_or_List>::underlying_type *nul;
		return _addValueType(nul,list_or_base,name);
	}
	template<class Enum, int N>
	/**GENERATOR-SIDE API, OVERLOAD
	 * Adds an <xs:enumeration> based <xs:simpleType> to the <xs:schema>.
	 * This form of @c addType() permits explicit invocation of the template parameter.
	 * Basically it's to work around C++'s inability to partially-specialize functions.
	 * @note This is now converting to @c DAEP::Value<0,Enum>::underlying_type.
	 * @see @c _addValueType() Doxygentation.
	 */		
	inline XS::SimpleType &addType(const daeClientEnum (&e)[N], const daeClientEnum (&re)[N], daeClientString2 base, daeClientString2 name)
	{
		//HACK: _addValueType(DAEP::Class) wants wrapped_type. 		
		//DAEP::Value can specialize Enum to be a different type.
		typename DAEP::Value<0,Enum>::/*underlying*/wrapped_type *nul;
		return _addValueType(nul,e,re,base,name);
	}

	template<class VT>
	/**
	 * Adds an <xs:simpleType> element to the <xs:schema>.
	 * @param name is the type-name, by which findType() will find it.
	 * If @c name==nullptr then @a base must be an xs: namespace type, and it's
	 * as if @base becomes @a name, and in which case, there is no <xs:restriction> "base."
	 * @return Returns the newly created type. There can't be duplicates.
	 */	
	inline XS::SimpleType &_addValueType(VT*&/*C4700*/, daeClientString2 base, daeClientString2 name)
	{
		//This is just stripping DAEP::Class off of VT, since VT cannot be a List.
		typedef typename XS::List::Item<VT>::type NC;
		XS::SimpleType &o = _addAtom(_typewrit<VT>(),&base,&name);
		if(name!=nullptr) o.getRestriction()._setMinMax<NC>(); return o;
	}
	template<class Atom, int N>
	/**TEMPLATE-SPECIALIZATION
	 * @param base_or_item If @c findType(base_or_item) is a list (daeArray) type, then
	 * this parameter is treated as an <xs:restriction> "base" attribute. Otherwise it's
	 * understood to be an <xs:list> "itemType" attribute The distinction is not clear in
	 * the generators' output. Perhaps this is a defect. The rationale is for @c addType()
	 * to correspond with @c findType().
	 * @see unspecialized @c addType<T>() Doxygentation.
	 */	
	inline XS::SimpleType &_addValueType(daeArray<Atom,N>*&/*C4700*/, daeClientString2 base_or_item, daeClientString2 name)
	{
		XS::SimpleType &o = _addList(_typewrit<Atom>(),base_or_item,name);
		if(o.getRestriction()) o.getRestriction()._setMinMax<Atom>(); return o;
	}		
	template<class NC, int N>
	/**OVERLOAD
	 * Adds an <xs:enumeration> based <xs:simpleType> to the <xs:schema>.
	 *
	 * ColladaDOM 3 NOTES
	 * ColladaDOM 3 style ended up using @c struct for the xs:enumeration
	 * based types. The @c struct wrap around an @c __enum__ @c enum type.
	 * @c daeArrayAware::Class() has been specialized to see the __enum__
	 * pattern. This avoids writing ::__enum__ in DAEP Value's parameters.
	 * Using the raw type is undesirable, because it would mix its values
	 * with DAEP Value's values. That would be confusing/undercut schemas.
	 *
	 * @param e must be sorted in terms of the keys. 
	 * @param re must be sorted in terms of the values, or--
	 * @remarks @a re can be @a e if values are ordered: e.g. 1,2,5,6.
	 * @param name is the type-name, by which findType() will find it.
	 * @param base is the <xs:restriction> "base" attribute. It can't
	 * default. It's required. An <xs:union> @c enum currently has to
	 * be merged, and the base is taken from the union members, which
	 * should either be similarly based, or set to be xs:string based.
	 */				
	inline XS::SimpleType &_addValueType(DAEP::Class<NC>*&/*C4700*/, 
	const daeClientEnum (&e)[N], const daeClientEnum (&re)[N], daeClientString2 base, daeClientString2 name)
	{
		//Assuming NC is an enum type. std::is_enum is C++11.
		XS::SimpleType &o = _addEnum(N,e,re,base,name,0); daeCTC<sizeof(daeEnumeration)==sizeof(NC)>();
		o._value_typewriter->setTypeID<NC>(); return o;
	}
	template<class VT, int N>
	/**WARNING, OVERLOAD
	 * Adds an <xs:enumeration> based <xs:simpleType> to the <xs:schema> that
	 * has been specialized by the user/client to implement some useful logic.
	 *
	 * @warning The way this is implemented, @c daeType<VT> must be based on @c daeEnumType.
	 * It can't extend @c daeEnumTypist. It can add to @c daeType and locate the extra data 
	 * via @c EX::Enumeration::getSpecializedValueType().
	 * @c daeType<VT> MUST BE TRIVIALLY-DESTRUCTIBLE. This is because the destructor is not
	 * called.
	 */		
	inline XS::SimpleType &_addValueType(VT*&/*C4700*/, 
	const daeClientEnum (&e)[N], const daeClientEnum (&re)[N], daeClientString2 base, daeClientString2 name)
	{																		   
		XS::SimpleType &o = _addEnum(N,e,re,base,name,sizeof(daeType<VT>));
		XS::Enumeration &xe = o.getRestriction().getEnumeration();
		typedef typename daeType<VT>::Typist Typist;
		daeType<VT> &ext = xe.getValueType<Typist>();
		new(&ext) daeType<VT>(xe); o._value_typewriter = ext;
		o._value_typewriter->setTypeID<VT>(); return o;
	}
	
	friend class COLLADA::domAny;
	template<class T, unsigned long long, class>
	friend class DAEP::Elemental;
	template<class T>
	/**GENERATOR-SIDE API
	 * Begins adding metadata for an element to @c this schema.
	 * @param globalName must be the actual name in the schema, 
	 * -or else <xs:any> will not be able to work as described.
	 * @see @c findChild() Doxygentation. It uses global names.
	 */	
	inline daeMetaElement &addElement(T*&/*C4700*/toc, daeClientString2 QName)
	{
		//First the model is allocated/layed out, and added to the roster.
		DAEP::Parameters ee(toc); daeMetaElement &e = 
		_addElement(toc->__DAEP__Schema,sizeof(T),ee.content_feature,QName);
		#ifdef _DEBUG
		//This is to get a visual on the prototype.
		//NOTE: Visual Studio puts T in [Raw View]. Maybe because the vptr??
		toc = 
		#endif
		//Then it's given to daeMetaElement.h, which gets down in the weeds.
		e._continue_XS_Schema_addElement<T>(ee); return e;
	}

COLLADA_(protected)

	/** Implements @c addType(int,daeClientEnum,...). */
	NOALIAS_LINKAGE XS::SimpleType &_addEnum
	(int,const daeClientEnum*,const daeClientEnum*,daeClientString2,daeClientString2,int);
	/** Implements @c addElement(). */
	NOALIAS_LINKAGE daeMetaElement &_addElement(long long,int,daeFeatureID,daeClientString2);
	/** Implements @c addType(). */
	NOALIAS_LINKAGE XS::SimpleType &_addAtom(daeTypewriter*,const daeName*,const daeName*);
	/** Implements @c addType<daeArray<T>>(). */
	NOALIAS_LINKAGE XS::SimpleType &_addList(daeTypewriter*,daeClientString2,daeClientString2);
	
	template<class T, class Typist=daeTypist<T>, int atom=Typist::system_type>
	//Most (if not all) of the time this will hit the system_type version, which
	//is a static expression. The library has its own custom types for "xs:blah"
	//types. If the name isn't "xs:yadayadayada" then it switches on system_type.
	struct _typewrit
	{
		//Note, internally the string name of the type is used before this value.
		operator daeTypewriter*(){ return (daeTypewriter*)Typist::system_type; }
	};
	template<class T, class Typist> struct _typewrit<T,Typist,0>
	{
		operator daeTypewriter*(){ static daeType<T,Typist> loc; return loc; }		
	};
	/**GCC wants explicit-specializations outside classes...
	template<> struct _typewrit<daeAST::TypedUnion> //xs:anySimpleType
	{
		operator daeTypewriter*(){ return (daeTypewriter*)0;  }		
	};*/
};//GCC
template<> struct XS::Schema::_typewrit<daeAST::TypedUnion> //xs:anySimpleType
{
	operator daeTypewriter*(){ return (daeTypewriter*)0;  }
};

#include "../LINKAGE.HPP" //#undef LINKAGE

/**CIRCULAR-DEPENDENCY
 * This is used internally. Users can base their own types on it.
 * If it's not used with @c XS::Schema::addType() it's necessary
 * to call @c daeTypewriter::_registerTypeID() to have a type ID.
 * It's exposed in support of @c XS::Enumeration::getValueType().
 */
typedef daeType<daeEnumeration,daeEnumTypist> daeEnumType;
/**CIRCULAR-DEPENDENCY
 * The @c enum @c daeTypewriter are not required to be visible to 
 * @c XS::Schema::_typewrit() because each @c enum is a unique type.
 */
class daeEnumTypist : public daeTypist<> 
{
COLLADA_(public) //daeTypist IMPLEMENTATION

	enum{ system_type = daeAtom::ENUMERATION };

	const XS::Enumeration &enumeration;

	daeEnumTypist(const XS::Enumeration &e):enumeration(e){}
	
	inline daeOK encodeXML(std::istream &src, daeEnumeration &dst)
	{
		daeStringCP buf[512]; src >> std::setw(sizeof(buf)) >> buf;

		const daeClientEnum *found = enumeration.find(buf);
		if(found==nullptr)
		{
			#ifdef _DEBUG
			assert(buf[0]=='\0'&&src.eof()||strlen(buf)!=sizeof(buf)-1);
			#endif
			return DAE_ERR_QUERY_NO_MATCH;
		}
		dst = found->integral_constant; return DAE_OK;
	}
	inline daeOK decodeXML(std::ostream &dst, const daeEnumeration &src)
	{	
		const daeClientEnum *found = enumeration.find(src);
		if(found==nullptr) return DAE_ERR_QUERY_NO_MATCH;
		dst << found->string_constant; return DAE_OK;
	}
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_META_NAMESPACE_H__
/*C1071*/
