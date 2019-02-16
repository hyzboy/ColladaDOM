/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_ANY_SIMPLE_TYPE_H__
#define __COLLADA_DOM__DAE_ANY_SIMPLE_TYPE_H__

#include "daeAtomicType.h"
#include "daeRefCountedObj.h"

COLLADA_(namespace)
{//-.
//<-'

/**NEVER-CONST
 * @c daeAnySimpleTypewriter facilitates xs:anySimpleType logic.
 * @note This class is derived (backward) from @c daeData. It's
 * more of an afterthought, given that xs:anySimpleType is very
 * low priority, by nature.
 */
class daeAnySimpleTypewriter : public daeTypewriter
{	
	friend class domAny;
	friend class daeData;
	friend class daeDefault;
	friend class daeAnySimpleType;
	/**
	 * @c domAny sets this up for @c daeAST::Positive datatypes.
	 */
	daeOpaqueWriter *_unionWriter;

	typedef void *A; //alignment
	enum{ _A = -(int)sizeof(A) };	
	size_t _size(daeOpaque);
	bool _reset_type(daeOpaque,size_t);	
	static daeAnySimpleTypewriter *_type(daeString,size_t,size_t&);
	static daeAnySimpleTypewriter*&_type(daeOpaque o){ return o[_A]; }

COLLADA_(public) //daeTypewriter methods

	virtual void copy(const daeOpaque src, daeOpaque dst)
	{	
		_assert_this_and_type(dst);
		if(this!=_type(src)) 
		return _type(src)->copy(src,dst);
		if(this==_type(dst)||_reset_type(dst,_size(src)))
		_unionWriter->copy(src,dst);
		else assert(0); //programmer error
	}

	virtual int compare(const daeOpaque a, const daeOpaque b)
	{
		_assert_this_and_type(b);
		if(this!=_type(a)) 
		return _type(a)->compare(a,b);
		if(this==_type(b)) 
		return _unionWriter->compare(a,b);
		//This works but the integer family of types could
		//stand to be comparable...
		//return this>_type(b)?1:-1;		
		int s = getAtomicType(); 
		int t = _type(b)->getAtomicType();		
		//WARNING: This sorts arrays between -1 and 0. But		
		//if s==0||t==0 were the condition arrays would be
		//interleaved alongside their types instead. Which
		//is not enough of an improvement to make a change.
		if(s==t)
		{
			assert(s==daeAtom::EXTENSION);
			//TODO? Array comparison in terms of values is
			//not really practical.			
			return this>_type(b)?1:-1;		
		}
		//HACK? This allows for the following relationship
		//xs:long < xs:int < xs:uint < xs:ulong
		//(daeAnySimpleTypewriter doesn't use the byte and
		//short types, but they would work too if it did.)
		//daeAtomicType just happens to be set up this way
		//and it just happens that negative values are set
		//to use xs:int and xs:long, and 64-bit values are
		//always more than what the 32-bit value will hold.	
		return s-t;
	}

	virtual daeOK serialize(const daeOpaque src, daeArray<> &dstIn)	
	{
		_assert_this_and_type(src);
		return _type(src)->_unionWriter->serialize(src,dstIn);
	}

	virtual daeOK unserialize(const daeStringCP *srcIn, size_t srcN, daeOpaque dst)
	{
		_assert_this_and_type(dst);
		
		daeAnySimpleTypewriter *type; size_t size;

		//NEW: Adding appending model/srcN.
		if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
		{
			type = _type(dst);			
			//NOTE: This is because string/binary shouldn't get
			//appended onto itself. This resolves any ambiguity.
			if(!type->hasListType())
			{
				//Hoping compiler works this into _unserialize2.			
				return _failure_to_unserialize(srcIn,srcN,dst,0);
			}
		}
		else type = _type(srcIn,srcN,size);

		return type->_unserialize2(srcIn,srcN,dst,size);
	}	
	inline daeError _unserialize2(daeString srcIn, size_t srcN, daeOpaque dst, size_t size)
	{
		if(_type(dst)!=this&&!_reset_type(dst,size)
		||DAE_OK!=_unionWriter->unserialize(srcIn,srcN,dst).error)
		return _failure_to_unserialize(srcIn,srcN,dst,size);
		return DAE_OK;		
	}
	/**PROGRAMMER ERROR?
	 * @note This is promoting 32-bit values to 64-bit types in order
	 * to not have to parse the individual values of arrays carefully.
	 * Falls back to xs:string in cases where @c _type() has selected
	 * an incompatible binary representation. 
	 * @note This should not occur, but if data were dropped it would
	 * be unacceptable.
	 *
	 * Note, it's also being used to force xmlns attributes to always
	 * use system-pool strings. But they must be assigned to at least
	 * once. They do not acquire @c getIsXmlns() status until written
	 * to via @c daeAnySimpleTypewriter().
	 */
	daeError _failure_to_unserialize(daeString,size_t,daeOpaque,size_t);

COLLADA_(public) //daeAnySimpleType relationship
	/**
	 * @c daeAnySimpleType Constructor 
	 *
	 * @param decayed_type is an invariant typewriter
	 * pair which @c daeTypewriter::where<>() returns.
	 */
	daeAnySimpleTypewriter(daeTypewriter &decayed_type)
	:daeTypewriter(decayed_type),_unionWriter(&decayed_type)
	{
		assert(isAnySimpleType()&&!decayed_type.isAnySimpleType());
	}

COLLADA_(private) //_DEBUG	
	/**
	 * Checks to see if there is really a typewriter pointer
	 * in front of the data. These typewriters must be known
	 * to the COLLADA-DOM library.
	 */
	inline void _assert_this_and_type(daeOpaque b)
	{
		#if defined BUILDING_COLLADA_DOM && defined(_DEBUG)
		extern void daeAnySimpleType_assert(daeTypewriter*);
		this->_assert_daeAnySimpleTypewriter();
		_type(b)->_assert_daeAnySimpleTypewriter();
		#endif
	}
	/** Implements @c _assert_this_and_type(). */
	void _assert_daeAnySimpleTypewriter();
};

/**
 * @c daeAnySimpleType implements @c daeType for an xs:anySimpleType 
 * based data. In addition it has some unrelated inner classes which
 * are organized below it mainly so it acts like a @c namespace. Its
 * inner classes would have very convoluted names if they weren't so.
 * Plus, it's easier to manage C++ friendship relationships this way.
 *
 * CORRECTION: NOT 4... 2 (There was 4, before TypedUnion::_offset.)
 * @note This class mainly simplifies the 4 typewriter setup that is
 * required by @c daeAnySimpleTypewriter. Although @c daeArray based
 * typewriters are often not required, it's often desirable to cover
 * both types together.
 */
class daeAnySimpleType
{
COLLADA_(public) //NAMESPACE

	//TypedUnion needs to be public since 
	//it's xs::anySimpleType.

	class TypedUnion; 
	class Data;

	typedef daeAnySimpleTypewriter AnyWriter;

COLLADA_(private) //INTERNAL
	
	AnyWriter _atomWriter,_arrayWriter;

COLLADA_(public) //INTERNAL
	/**
	 * @c daeAnySimpleTypewriter Constructor
	 */
	daeAnySimpleType(daeTypewriter &decayed_type)
	:_atomWriter(decayed_type.where<daeAtom>()),
	_arrayWriter(decayed_type.where<daeArray>()){}

#ifdef BUILDING_COLLADA_DOM

	template<class I, class J>
	/**
	 * This implements a positive number that is an @c int
	 * until it is assigned a value larger than @c INT_MAX.
	 * This allows @c TypedUnion::is<int>() to be standard
	 * for values not more than @c INT_MAX.
	 */
	struct Positive
	{
		struct AtomWriter : daeAtomWriter<I>
		{
			inline daeOK unserialize(daeString,size_t,daeOpaque);
		};
		struct ArrayWriter : daeArrayWriter<I>
		{
			inline daeOK unserialize(daeString,size_t,daeOpaque);
		};		

		AtomWriter first; ArrayWriter second;
	};
	typedef Positive<int,unsigned int> PositiveI;
	typedef Positive<long long int,unsigned long long int> PositiveLL;

#endif

COLLADA_(public) //OPERATORS
	/**INTERNAL
	 * Use [1] to access the @c daeArray typewriter.
	 */
	inline operator AnyWriter*(){ return &_atomWriter; }
	/**INTERNAL
	 * C2819 says -> is different from a conversion
	 * to pointer operator. This accesses the owned
	 * atom typewriter in cases where any will work.
	 */
	inline AnyWriter *operator->(){ return &_atomWriter; }
};

COLLADA_DOM_LINKAGE2
extern daeAnySimpleTypewriter daeAnySimpleType_void;
COLLADA_DOM_LINKAGE2
extern daeAnySimpleTypewriter*daeAnySimpleType_reps[];

//TODO: Some internals need to be external globals.
//Afterward a more complete feature set can be had.
/**NEW/ROUGH
 * This is @c xs::anySimpleType from daeDomTypes.h.
 * It is used (internally) by attributes that come
 * without an XML Schema.
 */
class daeAST::TypedUnion
{	
	friend class daeData;
	friend class daeMetaElement;
	friend class daeAnySimpleType;
	friend class daeAnySimpleTypewriter;	

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;
		
	//REMINDER: DAEP::Container prevents
	//<xs:list itemType="xs:anySimpleType">
	//from being a daeArray so that isAnyExtra
	//can reliably isolate daeAST::Data values.
	typedef const daeObject __COLLADA__Object;
	inline void __COLLADA__place(const daeObject *obj, const undefined &cp)
	{
		//assert(0==_offset); //UNIMPLEMENTED
	}
	static void __COLLADA__move(const daeObject *obj, undefined *lv, undefined *rv, size_t iN)
	{
		//assert(0==rv->_offset); //UNIMPLEMENTED
	}

	template<bool POD>
	COLLADA_NOINLINE //MSCV
	static void _buffered_copy
	(daeOpaqueWriter &tw, daeOpaque src,
	daeOpaqueWriter &tw2, daeOpaque dst)
	{
		//NOTE: Either tw or tw2 is _type... depending on case.
		daeArray<daeStringCP,POD?32:COLLADA_MAX_PATH> buf; 
		if(tw.serialize(src,buf)!=DAE_OK
		||tw2.unserialize(buf.begin(),buf.size(),dst)!=DAE_OK) assert(0);
	}
	/**EXPERIMENTAL
	 * Trying to construct/assign to daeArray and daeStringRef
	 * where @c operator based conversion cannot use templates
	 * and is suboptimal. 
	 * @note @c daeStringRef::daeStringRef is unimplemented as
	 * it's ambiguous about @c getObject() and doesn't seem to
	 * be a good idea to encourage careless use of system-pool.
	 */
	struct __COLLADA__Value
	{
		static void copy(const TypedUnion &src, daeCharData &dst)
		{
			src.getType().memoryToString(src.get(),*(daeArray<>*)&dst);
		}
		template<class T>
		static void copy(const TypedUnion &src, daeArray<T> &dst)
		{
			daeArrayWriter<T> tw;
			if(!src._type->hasListType()
			||tw.atomic_type!=src.getType().getAtomicType())
			src._buffered_copy<false>(src.getType(),src.get(),tw,&dst);
			else tw.copy(src.get(),&dst);
		}		
		static void copy(const TypedUnion &src, daeStringRef &dst)
		{
			daeAtomWriter<daeStringRef> tw;
			if(!src.is<daeStringRef>())
			src._buffered_copy<false>(src.getType(),src.get(),tw,&dst);			
			else tw.copy(src.get(),&dst);
		}		
	};		

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
	 *
	 * REMINDER: Had an idea to park an AU in
	 * hexBinary.AU while not using list's AU.
	 * Otherwise it's depressing to be 128bit.
	 */
	mutable union Union
	{
		daeString string;
		int _int;		
		//Large values may be assigned
		//to hexBinary.
		long long int _long; 
		//float _float;
		double _double;
		//0 and 1 become int.
		bool boolean; //daeBoolean
		//NOTE: Short binary sequences 
		//are assigned to string.
		struct Binary
		{
			int base:32,surplus:8;
			daeAlloc<> *AU;  
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

	/** Implements destructor logic. */
	daeAlloc<char>* &_TypedUnion();

COLLADA_(public)
	/**MAKESHIFT
	 * @c daeArray Constructor and Destructor
	 * @todo Can construct from a global copy.
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
			switch(_type->_atomType)
			{		  
			case daeAtom::BINARY: //xs:hexBinary 
				
				//daeDefault::_setDefaultString is going
				//to some length to convert hexBinary to
				//STRING.
				assert(0);
				break;
				
			case daeAtom::EXTENSION: //xs:list
			
				//If the default is an array, it will have
				//a space in its default string, otherwise
				//the compiler can elide this.
				if(pt.has_default>1||pt.is_fixed>1)
				_prototype_construct();
				break;
				
			case daeAtom::STRING: //xs:string
			
				((daeStringRef*)&_union)->_ref(*DOM);
				break;
			}
		}
	}
	 
COLLADA_(public) //ACCESSORS
	/**
	 * Database access (similar to @c daeArray.)
	 *
	 * @note "getElement" might be a better name.
	 * It's always going to be an element, since
	 * the system presumes that it is internally.
	 * The name suggests this is more for memory
	 * management than element access.
	 * @see @c daeAST::Data::getElement().
	 */
	inline daeNullable<> getObject()const
	{
		daeNullable<> o = {_getObject()}; return o;
	}
	inline daeObject *_getObject()const
	{
		if(0==_offset) return nullptr;
		return reinterpret_cast<daeObject*>((char*)&_union-_offset);
	}

	/**NEVER-NULLPTR
	 * Gets @c this union's allocation-unit. The AU is shared if
	 * the type changes, and is 0 capacity thunk if the type has
	 * never required an AU.
	 *
	 * @note This AU may have the wrong offset, and the capacity
	 * is in terms of its last use-case.
	 *
	 * It's returned by address if you absolutely have to manage
	 * it. Doing so is not recommended. @c getAU() is const-only.
	 */
	inline const daeAlloc<>*const &getAU()const
	{
		daeAlloc<>* &o =
		_type->_list==1?_union.list:_union.hexBinary.AU;
		return *(const daeAlloc<>**)&o;
	}	
	
	/**
	 * Tells if @c this is not part of an element class's object.
	 */
	inline bool isFree()const{ return 0==_offset; }

	/**
	 * Tells if @c this is embedded in an element class's object.
	 */
	inline bool isEmbedded()const
	{
		//NOTE: daeAnySimpleTypewriter::_reset_type uses similar
		//logic, but implements it by way of daeElement::getMeta.
		if(0==_offset) return false;				
		return _getObject()->__DAEP__Object__v1__model()
		.__DAEP__Model__object_sizeof()>(size_t)_offset;
		daeCTC<sizeof(size_t)==sizeof(daeOffset)>();
	}	

	//REMINDER: This works because DAEP::Container is collapsing
	//<xs:list itemType="xs:anySimpleType"> to xs::anySimpleType.
	//A "daeAnyAtomicType" class will be the best of both worlds.
	//But it's not worth implementing unless there is a real use.
	/**
	 * Tells if @c this is part of a @c daeAST::Data object that
	 * is among the back/nonstatic section of @c daeAnyAttribute.
	 */
	inline bool isAnyExtra()const{ return !isFree()&&!isEmbedded(); }
	
	/**
	 * Gets @c this union's current RTTI descriptor. It reflects
	 * the union's current type.
	 */
	inline daeTypewriter &getType()const{ return *_type; }
		
	/**WARNING
	 * Tells if the underlying type is compatible with numerics.
	 * @see @c int, @c long long, @c double, @c bool, @c signed
	 * or @c unsigned.
	 * Unlike @c hasPOD_representation() @c void isn't included.
	 */
	inline bool isPOD()const{ return _type->_atomType<daeAtom::VOID; }
	
	/**
	 * Tells if the type is an @c int that transcends @c INT_MAX.
	 * Namely @c unsigned and @c long int types are not selected
	 * unless a value will not fit into @c int.
	 * @see exotic().
	 *
	 * @return Returns @c !is<int>()&&isPOD_int(). 
	 */
	inline bool isPOD_exotic()const{ return _type->isExotic_int(); }
	/**
	 * Tells if @c int types/lists transcend @c INT_MAX.
	 * Namely @c unsigned and @c long int types are not selected
	 * unless a value will not fit into @c int.
	 * @return Returns getType().where<daeAtom>().isExotic_int().
	 * @see @c isPOD_exotic() Doxygentation.
	 */
	inline bool exotic()const{ return _type->_atomWriter->isExotic_int(); }	 

	enum{ _sizeof_i=sizeof(int), _sizeof_ll=sizeof(long long int) };

	/**WARNING
	 * Tells if the underlying type includes the @c int keyword.
	 * @see @c int, @c long long, @c signed or @c unsigned.	 
	 */
	inline bool isPOD_int()const
	{
		return _type->hasKeyword_int(); 
	}
	template<size_t I>
	/**
	 * Tells if the POD type is an @c int that is size-of @a I.
	 * @a I is @c sizeof(int) or "long int" or "long long int".
	 */
	inline bool isPOD_int()const
	{
		daeCTC<I==_sizeof_i||I==_sizeof_ll>();
		return I==std::abs(_type->_atomType); 
	}
	template<class I>
	/**
	 * @tparam I must be @c signed or @c unsigned.
	 * @see Non-template @c isPOD_int Doxygentation.
	 *
	 * Tells if the underlying type is compatible with numerics.
	 * This overload restricts types to using the @c signed and
	 * @c unsigned keywords, however the type is only assuredly
	 * POD if @c true.
	 * @note This class is restricted to @c int and @c long long
	 * types. 
	 *
	 * @return Returns @c false if the value is either not @a I
	 * or not POD.
	 */
	inline bool isPOD_int()const{ return _int((I*)nullptr); }
	inline bool _int(signed*)const{ return _type->hasKeyword_signed(); }
	inline bool _int(unsigned*)const{ return _type->hasKeyword_unsigned(); }	

	/**WARNING
	 * Tells if the underlying type is compatible with numerics.
	 * @see @c int, @c long long, @c double, @c bool, @c signed
	 * or @c unsigned, and @c void.
	 * @c setPOD_representation() can be set for plain-old-data.
	 * @c isPOD() differs only in that it returns @c false when
	 * @c void is the type, which is the case at initialization.
	 */
	inline bool hasPOD_representation()const
	{
		return _type->_atomType<=daeAtom::VOID; 
	}

	template<class T>
	/**EXPERIMENTAL
	 * @tparam T corresponds to @c is<T>().
	 * @return Returns @c false if type is unchanged
	 * or @c false==hasPOD_represenation().
	 */
	inline bool setPOD_representation(T cp)
	{
		if(!setPOD_representation(_pod_keywords((T*)nullptr),0))
		return false;
		//memcpy is not necessarily to defeat "strict-aliasing"
		//but may not be a bad idea.
		daeCTC<(sizeof(cp)<=_sizeof_ll)>();
		memcpy(&_union,&cp,sizeof(cp));
		memset((char*)&_union+sizeof(cp),0x00,_sizeof_ll-sizeof(cp));
		return true;
	}
	template<class T>
	/**EXPERIMENTAL
	 * @warning @c getType() will change. Bits won't.
	 * Change from one "plain-old-data" type to @a T.
	 * However @c daeAtomicType::VOID is zeroed.
	 *
	 * @tparam T corresponds to @c is<T>().
	 * @return Returns @c false if type is unchanged
	 * or @c false==hasPOD_represenation().
	 */
	inline bool setPOD_representation()
	{
		int at = _pod_keywords((T*)nullptr);
		enum dae_clear clr = at==daeAtom::VOID?dae_clear:0;
		return setPOD_representation(at,clr);
	}
	COLLADA_NOINLINE
	/**LOW-LEVEL
	 * @param clr zeroes the POD memory, and makes it possible to
	 * access this overload.
	 */
	inline bool setPOD_representation(int at, enum dae_clear clr)
	{
		//TODO: Can replace this with a more direct lookup.
		daeAnySimpleTypewriter **p = daeAnySimpleType_reps; 
		if(hasPOD_representation()) do if(at==(*p)->_atomType)
		{
			//Is _union._long=0 a "strict aliasing" no-no? I can never say.
			//_type = *p; if(clr==dae_clear) _union._long = 0; return true; 
			_type = *p; 
			if(clr==dae_clear) memset(&_union,0,_sizeof_ll); return true; 
		}
		while((*p)->_atomType<daeAtom::_NON_POD_BEGIN_); return false;
	}
		
	/**
	 * xs:anySimpleType data has no way of distinguishing tokens from
	 * strings.
	 * @return Returns @c string().token() to guard against documents
	 * having whitespace around strings.
	 * @warning @c token() cannot collapse inner whitespace, which is
	 * a quality of some of XML Schema's string based datatypes.
	 */
	inline daeHashString token()const{ return string().token(); }

	/**WARNING
	 * @warning The return string does not partake in reference count
	 * tracking, and so should not be held onto to for long. Use this
	 * at your own risk.
	 *
	 * Gets this unions's value IF-AND-ONLY-IF its underlying binary
	 * representation matches that of xs:string.
	 * @return Returns an empty string if the value is a number or an
	 * other kind of non-string data.
	 * @note This avoids copying the string into a new buffer if code
	 * is highly confident about the value's underlying type, or does
	 * not care if it is not a string.
	 */
	inline daeHashString string()const
	{
		if(is<daeString>())
		return *(daeStringRef*)&_union; return empty_daeString1; 
	}	

	/**LOW-LEVEL
	 * Implements compatibility with @c daeHashString and @c daeURI.
	 * @return Returns @c nullptr if @c false==is<daeString>().
	 * @see @c size()
	 */
	inline daeString data()const
	{
		//REMINDER: daeURI needs nullptr to construct a URI. This API 
		//is mainly for compatibility.
		return is<daeString>()?_union.string:nullptr;
	}

	/**WARNING
	 * @warning This is not the size of the underyling data, nor that
	 * of array data. It is the Standard Library definition of "size"
	 * here applied only to the string type.
	 *
	 * Implements compatibility with @c daeHashString.
	 * @return Returns 0 if @c false==is<daeString>().
	 * @see @c data()
	 */
	inline size_t size()const
	{
		return is<daeString>()?((daeStringRef*)&_union)->size():0;
	}

	template<class T>
	/**
	 * Tells if the underlying type is @a T.	 
	 * @c daeString or @c daeStringRef. The
	 * syntax is cool, but it's constrained
	 * to actual internal types.
	 * @c daeBinary and @c void.
	 * @c daeArray and @c daeArray<int> etc.
	 * @c daeArray<daeBinary<>>. Its double
	 * angle-brackets can't be omitted here.
	 * @c daeAtom indicates non-array types.
	 */
	inline bool is()const{ return _is((T*)nullptr); }	
	bool _is(daeAtom*)const{ return _type->_list==0; }
	#define _(x,y) \
	bool _is(x*)const{ return _type->_atomType==y; }\
	static int _pod_keywords(x*){ return y; }
	#define __(x,y) _(x,y)\
	bool _is(daeArray<x>*&)const\
	{ return _type->_list!=0&&y==_type->_atomWriter->_atomType; }
	_(void,daeAtom::VOID)
	__(double,COLLADA_DOM_ATOMIC_FLOAT(double))
	__(int,-int(sizeof(int)))
	__(unsigned int,+int(sizeof(int)))
	__(long int,-int(sizeof(long int)))
	__(unsigned long int,+int(sizeof(long int)))
	__(long long int,-int(sizeof(long long int)))
	__(unsigned long long int,+int(sizeof(long long int)))
	__(bool,daeAtom::UBOOLEAN) //daeBoolean
	__(daeBinary<>,daeAtom::BINARY)	
	_(daeString,daeAtom::STRING) //xs:string
	_(daeStringRef,daeAtom::STRING) //xs:list		
	#undef __
	#undef _	
	template<template<int,int>class daeBinary>
	/**
	 * Tells if the underlying type is @a daeBinary.
	 * This works like @c is<daeBinary>().
	 */
	inline bool is()const;

	template<template<class,int>class daeArray>
	/**
	 * Tells if the underlying type is @a daeArray.
	 * This works like @c is<daeArray>().
	 */
	inline bool is()const;
	 
	/**LOW-LEVEL
	 * Gets the underlying union memory.
	 * @see @c is().
	 */
	inline daeOpaque get(){ return &_union; }
	/**CONST-FORM
	 * Gets the underlying union memory.
	 * @see @c is().
	 */
	inline const daeOpaque get()const{ return &_union; }
			
	//UNFINISHED: This class & daeAnyAttribute require development.
	//Momentarily only their low-level feature-set are implemented.
	//It's better to use daeElement than domAny to work with these
	//values until this stuff stabilizes.

COLLADA_(public) //ASSIGNMENT OPERATOR
	/**
	 * Because XML allows "" values in attributes, @c clear() sets
	 * attributes to this value, as does assigning an empty string
	 * by any other means than @c operator=(daeString1) which is a
	 * symbolic mechanism for assigning @c daeAtom::VOID status to
	 * the xs:anySimpleType value, which is interpreted as erasing
	 * the attribute.
	 */
	inline void clear(){ _type->clear(&_union); }

	template<class T>
	/**MAKESHIFT 
	 * Copy constructor
	 */
	TypedUnion(const T &cp)
	{
		new(this) TypedUnion(); operator=(cp);
	}
	/**MAKESHIFT 
	 * C++ requires this copy constructor.
	 */
	TypedUnion(const TypedUnion &cp)
	{ 
		new(this) TypedUnion(); operator=(cp); 
	}

	/**EXPERIMENTAL
	 * Copy Assignment operator
	 * @todo Can inline trivial atomic types.
	 */
	inline TypedUnion &operator=(const TypedUnion &cp)
	{
		_copy(cp); return *this;
	}	  
	template<int ID,class T,class CC,typename CC::_ PtoM>
	/**
	 * Something like this is unavoidable for
	 * classes that do assignment by template.
	 */
	inline TypedUnion &operator=(const DAEP::Value<ID,T,CC,PtoM> &cp)
	{
		return operator=(*cp.operator->());
	}
	//I'M PRETTY CONFIDENT THIS CATCHES EVERYTHING.
	template<class T>
	/**EXPERIMENTAL, WARNING
	 * Trying leverage @c daeTypewriter facilities
	 * to develop/explore a high-level approach to 
	 * opaque value assignment/initialization. Use
	 * "" to change to @c daeAtomicType::VOID.
	 *
	 * @warning @c daeStringCP when @c char has to
	 * be assigned here as an 8-bit integer rather
	 * than a string or a character.
	 */
	inline TypedUnion &operator=(const T &cp)
	{
		//HACK: This promotes char/short types to int.
		//The daeBoolean overload opts out xs:boolean.
		const typename daeTypic<(sizeof(T)<4),int,T>::type &cp2 = cp;
		_reassign(cp2); return *this;
	}
	inline TypedUnion &operator=(const float &cp4B)
	{
		_reassign_atom((double)cp4B,&cp4B); return *this;
	}
	inline TypedUnion &operator=(const bool &cp1B)
	{
		_reassign_atom(cp1B,&cp1B); return *this;
	}		
	inline TypedUnion &operator=(const daeBoolean &cp1B)
	{
		_reassign_string(cp1B); return *this;
	}
	template<class B, class C>
	inline TypedUnion &operator=(const std::basic_string<daeStringCP,B,C> &cp)
	{
		_reassign_string(cp); return *this;
	}
	template<class S, int N>	
	/** @a S is const or non-const @c daeStringCP. */
	inline TypedUnion &operator=(S (&cp)[N])
	{
		_reassign_string(cp); return *this;
	}	
	/**WARNING
	 * Assigns @c daeAtomicType::VOID representation.
	 * 
	 * @warning As XML allows "" values, strings are
	 * other interpreted as an empty string. This is 
	 * the most straightforward way. @c clear() sets
	 * the type to an empty string. @c "" is treated
	 * as a placeholder for erasing the attribute as
	 * that is the more commonly desired outcome and
	 * consistent with how elements assignments work.
	 */
	inline TypedUnion &operator=(daeString1 empty_string_literal)
	{
		_TypedUnion(); _type = &daeAnySimpleType_void; 		
		memset(&_union,0x00,_sizeof_ll);
		assert('\0'==*empty_string_literal); return *this;
	}
	inline void _reassign_string(const daeHashString &cp)
	{
		_type->unserialize(cp.string,cp.extent,&_union);
	}			
	//DEDUCING TYPES!
	template<class S> 
	inline void _reassign(const S &cp)
	{		
		_reassign_atom(cp,&cp); //Atom or string?
	}
	template<class S, int N> //List
	/**
	 * Includes @c daeArray<daeStringCP> because @c char 
	 * must be interpreted as the 8-bit integer: xs:byte.
	 */
	inline void _reassign(const daeArray<S,N> &cp)
	{
		daeArrayWriter<S> tw;		
		if(!_type->hasListType()
		||tw.atomic_type!=_type->_atomWriter->_atomType)
		_buffered_copy<false>(tw,&cp,*_type,&_union);	
		else tw.copy(&cp,&_union);
	}			
	template<class S> //Atom
	inline void _reassign_atom(const S &cp,...)
	{		
		daeAtomWriter<S> tw;		
		if(tw.atomic_type!=_type->_atomType)
		_buffered_copy<((int)tw.atomic_type<(int)daeAtom::VOID)>
		(tw,&cp,*_type,&_union); else tw.copy(&cp,&_union);
	}
	/** Conflicts with @c operator=(S(&cp)[N]) */
	inline void _reassign_atom(daeString cp, daeString*)
	{
		_reassign_string(cp); //String
	}	
	template<class S, class T>
	inline void _reassign_atom(const S &cp, const daeString_support<T>*)
	{
		_reassign_string(cp); //String 
	}	

COLLADA_(public) //CONVERSION OPERATOR (POD TYPES)
	/**
	 * Converts to C-string if @c is<daeString>().
	 * @note Adds @c xs::string assignment parity.
	 */
	inline operator daeString()const
	{
		return is<daeString>()?_union.string:empty_daeString1;
	}

	//Other types will need constructors.
	#define l long
	#define _(x) inline operator x()const\
	{ x i=(x)0; if(_type->isPOD()) _pod_convert(i); return i; }
	//NOTE: "long" uses "sizeof(int)" or "long long".
	/**EXPERIMENTAL
	 * If not one of these POD types the data cannot
	 * be represented as a number.
	 * @return Returns zero/default initialized data
	 * if the union's type is non-POD.	 
	 * @see isPOD()
	 *
	 * @note @c is<T>() is enough to eliminate cases
	 * that don't apply. Alternatively: @c (T&)get().
	 */
	_(int)_(l)_(l l)_(double)_(bool)_(unsigned)_(unsigned l)_(unsigned l l)
	#undef _
	#undef l
	template<class S>
	inline void _pod_convert(S &cp)const
	{	
		switch(int at=_type->_atomType)
		{
		default:
			if(0!=(at&sizeof(int)))
			cp = S(_union._int);
			else
			cp = S(_union._long); break;
		case COLLADA_DOM_ATOMIC_FLOAT(double):
			cp = S(_union._double); break;
		case daeAtom::UBOOLEAN:
			cp = S(_union.boolean); break;
		}	
	}
	//bool is done separate as compilers go
	//nuts when you assign things to a bool.
	inline void _pod_convert(bool &cp)const
	{	
		/*Is this test worthwhile? Note that
		//0==_union._long only works because
		//bool is used instead of daeBoolean.
		//daeBoolean is of no use here. Even
		//an array of "1 true 0" is a string.
		if(daeAtom::UBOOLEAN==_type->_atomType)
		cp = _union.boolean;
		else*/cp = 0==_union._long;
	}
};
template<>
/**
 * Tells if the underlying type is @a T.
 * This works like @c is<daeBinary>().
 */
inline bool daeAST::TypedUnion::is<daeBinary>()const
{
	return is<daeBinary<>>();
}
template<>
/**
 * Tells if the underlying type is @a daeArray.
 * This works like @c is<daeArray>().
 */
inline bool daeAST::TypedUnion::is<daeArray>()const
{
	return _type->hasListType();
}

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ATOMIC_TYPE_H__
/*C1071*/
