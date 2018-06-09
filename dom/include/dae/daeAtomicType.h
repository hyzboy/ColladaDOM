/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_ATOMIC_TYPE_H__
#define __COLLADA_DOM__DAE_ATOMIC_TYPE_H__

#include "daeArray.h"
#include "daeStringRef.h"

COLLADA_(namespace)
{//-.
//<-'

/**CIRCULAR-DEPENDENCY
 * Class @c daeEnumTypist is in the @c XS::Schema header.
 * It's only visible for users/clients looking to extend
 * @c enum based types via @c DAEP::Value specialization.
 */
class daeEnumTypist;
template<class,class> class daeType;
typedef daeType<daeEnumeration,daeEnumTypist> daeEnumType; 	

/**WARNING, SCOPED-ENUM, LEGACY-SUPPORT
 * An enum for identifying the different atomic types
 * @warning If these @c enum are used, it's imperative
 * to also use a corresponding @typedef: @c daeInt, etc.
 */
struct daeAtomicType
{
	enum
	{
		//NOTICE: Do NOT add "LIST" or "ARRAY" to this!
		//Lists are not "atomic" and so use 0 along with
		//daeAtomicType::EXTENSION. Note that, these enum
		//only signify binary compatibility independent of
		//schema-based types.
		
	/** extension/list atomic type 
	 * 
	 * @note All @c daeArray use this value. In addition
	 * for xs:anySimpleType, when the array's subscripts
	 * vary in type the @c daeAtom type is this type too.
	 * In short this value says it alone is insufficient 
	 * to draw a conclusion. It's ambiguous.
	 */
	EXTENSION = 0,
	/** daeByte atomic type */
	BYTE = -int(sizeof(daeByte)),
	/** daeUByte atomic type */
	UBYTE = sizeof(daeByte),
	/** daeShort atomic type */
	SHORT = -int(sizeof(daeShort)),
	/** daeUShort atomic type */
	USHORT = sizeof(daeShort),
	/** daeInt atomic type */
	INT = -int(sizeof(daeInt)),
	/** daeUInt atomic type */
	UINT = sizeof(daeInt),
	/** daeLong atomic type */
	LONG = -int(sizeof(daeLong)),
	/** daeULong atomic type */
	ULONG = sizeof(daeLong),
	//This is used by the daeTypist<float> specialization.
	#define COLLADA_DOM_ATOMIC_FLOAT(x) -16-int(sizeof(x))
	/** daeFloat atomic type */
	FLOAT = COLLADA_DOM_ATOMIC_FLOAT(daeFloat),
	/** daeDouble atomic type */
	DOUBLE = COLLADA_DOM_ATOMIC_FLOAT(daeDouble),	
	/** daeBinary atomic type */
	BINARY = 17,
	/** daeBoolean atomic type */
	BOOLEAN,
	/** daeEnumeration atomic type */
	ENUMERATION,
	//GOTCHA!?
	//TOKEN feels like a bastard type. 
	//It seems schizophrenic to have TOKEN and STRING?
	//FYI: It's impractical to do STRING|TOKEN because
	//a negative value will result in a false positive.
	/**WARNING
	 * @see @c daeTypewriter::hasStringType().
	 *
	 * On STRING:
	 * daeStringRef/daeTokenRef atomic type 
	 * @warning This is not the only string type.
	 * Maybe it should be? @c TOKEN may even be more
	 * common. They are identical in binary terms.
	 * @see @c daeTypist<daeStringRef>
	 *
	 * On TOKEN:
	 * @note @c TOKEN is treated as a short text and
	 * is subject to having spaces replaced with #x20
	 * and normalized. CURRENTLY the library is very
	 * unsophisticated in the distinciton. It's in no
	 * way synonymous with xs:token and if a document
	 * saves/writes/outputs a space it may be as #x20.
	 * @see @c daeTypist<daeTokenRef>
	 *
	 * STRONG CASE FOR TOKEN?
	 * @c daeStringRef_sys_typewrit() is why TOKEN is
	 * ultimately worth keeping around, even while it
	 * seems like a chore to do 2 tests to single out
	 * strings. I BELIEVE OTHERWISE IT CAN BE REMOVED.
	 */
	STRING,TOKEN, 
	/**
	 * This type always yields an empty string. 
	 * Its purpose is to appear as if there is no 
	 * value so code can be written without @c nullptr
	 * tests, and so on. @c daeTypewriter::unserialize() 
	 * is incompatible with @c VOID... it returns 
	 * DAE_ERR_NOT_IMPLEMENTED and calls the @c assert()
	 * API and may involve @c daeErrorHandler too.
	 */
	VOID,
	};

	daeAtomicType()
	{
		daeCTC<(BYTE>=SHORT&&SHORT>=INT&&INT>=LONG&&FLOAT>=DOUBLE)>();
	}
};

template<class=void> 
/**NEVER-CONST
 * This class is specialized to implement @c daeType.
 * This is a default/starter implementation which is
 * typically used as a base class by specializations.
 */
struct daeTypist
{
COLLADA_(public) //primary functions

	/**TRAITS
	 * 0 is @c daeAtomicType::EXTENSION.
	 * To not get into heavy "template-metaprogramming"
	 * territory, the library expects both of these to
	 * be defined, but one or both must be 0.
	 *
	 * These are defaults. Set @c binary_type if types
	 * are binary-compatible with the @c daeAtomicType.
	 * @c system_type is used by the library to manage
	 * fundamental types, and the XML Schema datatypes.
	 */
	enum{ system_type=0,binary_type=0, buffer_size=64 };

	template<class T> static void assign(T &dst, const T &src)
	{
		dst = src; 
	}
	template<class T> static daeOK encodeXML(std::istream &src, T &dst)
	{
		src >> dst; return DAE_OK;
	}
	template<class T> static daeOK decodeXML(std::ostream &dst, const T &src)
	{
		dst << src; return DAE_OK;
	}
	//GCC can't do this. Just checking on Win32.
	//Maybe they can just be overloaded instead?
	//This is ensuring daeOpaque isn't assigned.
	#ifdef _MSC_VER
	template<> static void assign(daeOpaque&,const daeOpaque&)
	{
		daeCTC<0>();
	}
	template<> static daeOK encodeXML(std::istream &src, daeOpaque &dst)
	{
		daeCTC<0>();
	}
	template<> static daeOK decodeXML(std::ostream &dst, const daeOpaque &src)
	{
		daeCTC<0>();
	}
	#endif

	//This is ensuring daeOpaque isn't compared.
	static int compare(const daeOpaque&, const daeOpaque&)
	{
		daeCTC<0>(); return 0; //Bogus.
	}
	template<class T> static int compare(const T &a, const T &b)
	{
		return a<b?-1:a==b?0:+1;
	}
	template<class T, int M, int N> 
	//Muti-dimensional array compare support. e.g. list_of_hex_binary.
	static int compare(const daeArray<T,M> &a, const daeArray<T,N> &b)
	{
		//This ALGORITHM is identical to Typewriter<daeArray> 
		//except for compare() has replaced Typist::compare().
		int i,cmp,iN; for(i=0,iN=std::min(a.size(),b.size());i<iN;i++) 
		{
			cmp = compare(a[i],b[i]); if(cmp!=0) return cmp;
		}
		return (ptrdiff_t)a.size()-(ptrdiff_t)b.size(); daeCTC<M==N>(); 
	}

COLLADA_(public) //shaping functions
	
	//GCC/C++ won't tolerate explicit specialization in classes.
	template<class T> static void formatXML(std::ostream &dst, int fmtflags=~0)
	{
		_formatXML((T*)0,&dst,fmtflags); //Quick Fix for GCC/C++.
	}
	static void _formatXML(double*, std::ostream *dst, int)
	{
		//https://support.microsoft.com/en-us/kb/29557
		//TODO: float_array has members for describing the magnitude etc.
		//Supposedly ostream switches to E-mode if the exponent is large.
		//It's difficult to find reasonable documentation about iostream facilities.
		//Note; scientific was never the case. It seems to adds 0s, and so won't compress well.
		//dst.scientific(); 

		//ADDING +1 OR +2 (WHICH EQUALS C++11'S max_digits10) INTRODUCES
		//ROUNDING ERROR ON MSVC2015 THAT ACCUMULATES AND PRODUCES LARGE
		//NUMBERS FILLING ALL PRECISION DIGITS IN EACH ONE IN A FEW RUNS.
		dst->precision(std::numeric_limits<double>::digits10/*+1*/); //max_digits10
	}
	template<class T> static void formatXML(std::istream &src, int fmtflags=~0)
	{
		_formatXML((T*)0,&src,fmtflags); //Quick Fix for GCC/C++.
	}
	static void _formatXML(daeStringRef*, std::istream *src, int fmtflags)
	{
		if(fmtflags&src->skipws) *src >> std::noskipws;
	}
	static void _formatXML(...){} //Quick Fix for GCC/C++.

	/** 
	 * supercharge() is used to imbue @a tmp with superpowers. 
	 * This is a dummy implementation.	 
	 * @see the below @c daeStringRef based use-case/example.
	 * @see @c daeType<T>::Typwriter<daeArray<T>>::unserialize().
	 */
	static void supercharge(...)
	{}
	template<class T>
	/**OVERLOAD, EXAMPLE
	 * supercharge() is used to imbue @a tmp with superpowers. 
	 * In this version, the temporary is assigned an allocator.
	 *
	 * @note the second argument is a pointer, only because of
	 * MSVC's C4700 warning; which can't seem to be suppressed.
	 */
	static void supercharge(daeArray<T> *dst, daeStringRef *C4700)
	{
		//If this is not done, the insterter will allocate a 
		//string in the system string-table, and then allocate
		//it again, the second time, in the array's string-table.
		new(C4700) daeStringRef(dst->getObject());
	}

	//EXPERIMENTAL
	/**
	 * This is mainly added so @c std::istream doesn't trigger an
	 * error when it encounters trailing spaces followed by @c '\0'.
	 * Checking for the error after the fact is to much of a chore.
	 * Some exotic types might need to preserve the whitespace. Most
	 * do not want it there. An error more or less rejects the input.
	 */
	static void normalize(daeString &srcIn, daeString &srcEnd)
	{
		while(srcIn<srcEnd&&COLLADA_isspace(*srcIn)) srcIn++; 
		while(srcEnd>srcIn&&COLLADA_isspace(srcEnd[-1])) srcEnd--;
	}
};

/**NEVER-CONST
 * The @c daeTypewriter class implements a standard interface for
 * data elements in the reflective object system.
 *
 * @c daeTypewriter provides a central virtual interface that can be
 * used by the rest of the reflective object system.
 *
 * The atomic type system if very useful for file IO and building
 * automatic tools for data inspection and manipulation,
 * such as hierarchy examiners and object editors.
 */
class daeTypewriter
{	
COLLADA_(public) //ABSTRACT INTERFACE
	
	virtual ~daeTypewriter(){}
	
	/**PURE, ABSTRACT INTERFACE
	 * Performs a virtual copy operation.
	 * @param src Memory location of the value to copy from.
	 * @param dst Memory location of the value to copy to.
	 */
	virtual void copy(const daeOpaque src, daeOpaque dst){ return; }

	/**PURE, ABSTRACT INTERFACE
	 * Performs a virtual comparison operation between two values of the same atomic type.
	 * @param value1 Memory location of the first value.
	 * @param value2 Memory location of the second value.
	 * @return Returns a positive integer if value1 > value2, a negative integer if 
	 * value1 < value2, and 0 if value1 == value2.
	 */
	virtual int compare(const daeOpaque value1, const daeOpaque value2){ return 0; }
	
	/**PURE, ABSTRACT INTERFACE
	 * Writes @a src to @ dst. @a src is untagged, opaque memory (not type-checked.)
	 * @return Returns a client defined @c DAE_ERROR @c enum. @a dst is 0 terminated.
	 */
	virtual daeOK serialize(const daeOpaque src, daeArray<daeStringCP> &dst){ return DAE_OK; }	
		
	/**ABSTRACT INTERFACE
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param srcEnd The CP after the last CP to be written.
	 * @return Returns a client defined @c DAE_ERROR @c enum.

	 * REMINDER: If a desire is to minimize reallocations, the 
	 * optimal route is for the loader to daeObject::reAlloc().
	 * But it's only best to do so if the array is empty or if
	 * the type of the daeArray is raw-copyable plain-old-data.
	 * IN THIS CASE THE LOADER SHOULD COUNT THE ' ' CHARACTERS.
	 */	
	virtual daeOK unserialize(const daeStringCP *srcBegin, const daeStringCP *srcEnd, daeOpaque dst)
	{
		assert(!"daeMetaElement_voidwriter::unserialize"); return DAE_ERR_NOT_IMPLEMENTED;
	}


COLLADA_(public) //LEGACY-SUPPORT
	/**LEGACY
	 * Writes @a src to @ dst. @a src is untagged, opaque memory (not type-checked.)
	 * @return Returns a client defined @c DAE_ERROR @c enum. @a dst is 0 terminated.
	 */
	inline daeOK memoryToString(const daeOpaque src, daeArray<daeStringCP> &dst)
	{
		dst.clear(); return serialize(src,dst);
	}

	/**LEGACY
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param srcEnd The CP after the last CP to be written.
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */	
	inline daeOK stringToMemory(const daeStringCP *srcBegin, const daeStringCP *srcEnd, daeOpaque dst)
	{
		return unserialize(srcBegin,srcEnd,dst);
	}
	/**OVERLOAD
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param srcLen Length of the @a src string. 
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */
	inline daeOK stringToMemory(const daeStringCP *src, int srcLen, daeOpaque dst)
	{
		return unserialize(src,src+srcLen,dst); 
	}
	/**OVERLOAD, LEGACY-SUPPORT
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param src Source string, shorter than 128 characters.
	 * (This is to prevent really long @c strlen calls for potentially short values.)
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */
	inline daeOK stringToMemory(daeString src, daeOpaque dst, /*template-parity*/...)
	{
		size_t srcLen = strlen(src); //Maybe return DAE_ERR_INVALID_CALL is too harsh?
		if(srcLen>127){ assert(srcLen<128); /*return DAE_ERR_INVALID_CALL;*/ }
		return unserialize(src,src+srcLen,dst); 
	}
	/**OVERLOAD, NEW/MAY HAVE ISSUES
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)	 
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */
	inline daeOK stringToMemory(const daeHashString &src, daeOpaque dst, /*template-parity*/...)
	{
		return unserialize(src.string,src.string+src.extent,dst); 
	}

COLLADA_(public) //ACCESSORS
	/**
	 * Gets @c sizeof the underlying type.
	 * This is different from @c getSize() for arrays, etc.
	 */
	daeSize getSize(){ return _sizeof; }
	
	/**
	 * Tells if the type is in fact @c daeArray or not.
	 * @note @c where<daeArray>() is always so, however
	 * @c where<daeAtom>() can be for higher-dimensional
	 * arrays. The addition of @c isAnySimpleType() meant
	 * that doing @c this==&_daeArrayWriter would fail for
	 * xs:anySimpleType types, even in the xs:list scenario.
	 */
	inline bool hasListType()const
	{
		//I think this would work, but it is a far deeper
		//comparison than this==_daeArrayWriter and would
		//not work for the 2D array case. (The array case
		//might not have real world implications.)
		//return _typeID==_daeArrayWriter->typeID; 
		return 1==_list; 
	}

	/**RECOMMENDED
	 * Helps to test for both "TOKEN" and "STRING" types.
	 * It's desirable to remove TOKEN, so if that occurs
	 * code that uses this method will be left unchanged.
	 */
	inline bool hasStringType()const
	{
		if(daeAtomicType::TOKEN==_atomType
		||daeAtomicType::STRING==_atomType)
		return true; return false;
	}

	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("getAtomicType")
	/**LEGACY
	 * @see @c getAtomicType() Doxygentation.
	 */
	int getTypeEnum(){ return _atomType; }
	#endif //COLLADA_NODEPRECATED
	/**WARNING, LEGACY-SUPPORT
	 * Gets the enum associated with this atomic type. 
	 * This is not scalable and only works for base types.
	 *
	 * @warning Originally the concept was for this to return
	 * @c _daeAtomWriter->_atomType. While nice in theory, it's
	 * surprisingly easy to write code that corrupts array values.
	 * @see @c where<daeAtom>().
	 */
	int getAtomicType(){ return /*_daeAtomWriter->*/_atomType; }

	/**
	 * Tells if @c daeAtomicType::EXTENSION!=getAtomicType(), in 
	 * which case there is a standard binary representation, other
	 * than an array (see @c hasListType()) or an extension.
	 * @note This should be used to determine if the underlying type is
	 * a standard binary representation.
	 *
	 * @note "ATOMIC" is not a type. Here it means all built-in types
	 * except for lists.
	 * @see @c hasListType().
	 */
	bool hasAtomicType(){ return daeAtomicType::EXTENSION!=_atomType; }
	
	/**
	 * Gets the string associated with this type.
	 * The pointer will be unique for a given string.
	 * @c type_info::name must be the same process-wide.
	 * @return Returns the string associated with this type.
	 */
	inline daeString getTypeID()const{ return _typeID; }	
			  
	/**
	 * Tells if @c unserialize() expects its @c daeOpaque to
	 * be the @c union value inside of its @c daeAnySimpleType.
	 * @c where() must be used to obtain a compatible @c daeAtom
	 * or @c daeArray based @c daeTypewriter in order to assign to
	 * a value of the same type that is not a @ daeAnySimpleType or
	 * where changing the value's type is being avoided.
	 * @see @c where().
	 *
	 * @note NOTE "hasAnySimpleType" since this isn't testing
	 * the underlying (dynamic) type and the ending -Type comes
	 * from "xs:anySimpleType." 
	 */
	inline bool isAnySimpleType(){ return _daeAtomWriter!=this&&this!=_daeArrayWriter; }

COLLADA_(private) 

	friend class daeAnySimpleTypewriter;
	template<class,class> friend class daeType;	

	enum{ _versions=1 };
	unsigned _version:4,_list:1;
	int _atomType;
	daeSize _sizeof;
	daeString _typeID;
	/**
	 * @note @c this is one of these unless it is xs:anySimpleType
	 * based, in which case there are 3 @c daeTypewriter available
	 * and @c unserialize() expects @c daeAnySimpleType based data.
	 */
	daeTypewriter *_daeAtomWriter,*_daeArrayWriter;

	COLLADA_NOALIAS
	#ifdef NDEBUG
	#error Replace daeStringRef with something nonprovisional.
	#endif		
	/**NOT THREAD-SAFE
	 * This API requires a critical section.
	 */
	COLLADA_DOM_LINKAGE static daeString _ID(daeString id)
	COLLADA_DOM_SNIPPET( return daeStringRef(id); )

COLLADA_(public) //These have to be public.
	
	template<class T>
	/**
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypewriter(T*,daeTypist<T>*):_list()
	{
		_init(typeid(T),sizeof(T));
	}
	template<class T>
	/**
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypewriter(daeArray<T>*,daeTypist<T>*):_list(1)
	{
		_init(typeid(daeArray<T>),sizeof(daeArray<T>));
	}
	template<class T>
	/**HYPOTHETICAL?
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypewriter(daeArray<T>*,daeTypist<daeArray<T>>*):_list(1)
	{			
		//ASSUMING WON'T WORK since 2D array serialization is
		//impossible. Most likely a template is automatically
		//making such a type and so a review will be in order.
		assert(!_list); 

		_init(typeid(daeArray<T>),sizeof(daeArray<T>));
	}
	template<class T> //daeEnumeration vis-a-vis daeEnumTypist
	/**
	 * Call @c setTypeID() manually if the typist is @c not daeTypist<T>.
	 */
	daeTypewriter(T*,...):_list()
	{
		_init(typeid(void),sizeof(T)); _typeID = nullptr;
	}	
	/**WORKAROUND
	 * @c daeAtomicType::VOID Constructor
	 */
	daeTypewriter():_list(),_atomType(daeAtomicType::VOID)
	{
		_init(typeid(void),0); _daeAtomWriter = _daeArrayWriter = this;
	}
	void _init(const std::type_info &ti, size_t so)
	{
		_version = _versions; _sizeof = so; _typeID = _ID(ti.name());
	}
	/**
	* Manually sets the string returned by @c getTypeID() for both
	* of the lined atom & array typewriters, if not previously set.
	*/
	template<class T> inline void setTypeID(bool list=false)
	{
		if(_typeID==nullptr)
		{
			_daeArrayWriter->_list = list?1:0;
			_daeAtomWriter->_typeID = _ID(typeid(T).name());
			_daeArrayWriter->_list = 1;
			_daeArrayWriter->_typeID = _ID(typeid(daeArray<T>).name());
		}
		else assert(0);
	}
	/**STATIC
	* Gets a type ID string for direct pointer-to-pointer comparisons
	* with @c getTypeID(). 
	* @note String based comparison is unnecessary and impractical as
	* the values of the strings are not "portable" and so must be got
	* from @c ID(). IOW: They are not known until each/every run-time.
	*/
	template<class T> static daeString ID(){ return _ID(typeid(T).name()); }

COLLADA_(public)
	/**OPERATOR
	 * Converts to pointer to ease comparison to @c per<daeArray>().
	 */
	inline operator daeTypewriter*(){ return this; }

	/**CUTE
	 * Gets a @c daeTypewriter appropriate for atomic types.	 
	 * GCC/C++ require this to be defined outside the class.
	 */
	template<class daeAtom> inline daeTypewriter &where();
	/**
	 * Gets a @c daeTypewriter appropriate for @c daeArray containers.	 
	 * GCC/C++ require this to be defined outside of @c daeTypewriter.
	 */
	template<template<class,int>class daeArray> inline daeTypewriter &where();	
};
template<> 
/**CUTE, EXPLICIT-SPECIALIZATOIN
 * Gets a @c daeTypewriter appropriate for atomic types.
 * GCC/C++ require this to be defined outside the class.
 */
inline daeTypewriter &daeTypewriter::where<daeAtom>(){ return *_daeAtomWriter; }
template<> 
/**CUTE, EXPLICIT-SPECIALIZATOIN
 * Gets a @c daeTypewriter appropriate for @c daeArray containers.
 * @tparam class must be @c daeArray.
 * GCC/C++ require this to be defined outside of @c daeTypewriter.
 */
inline daeTypewriter &daeTypewriter::where<daeArray>(){ return *_daeArrayWriter; }	
  
/**NEVER-CONST
 * This is a restricted typewriter for xs:anySimpleType support where
 * vales are static.
 * @see @c XS::Attribute::getType() versus @c XS::Attribute::getTypeWRT().
 */
class daeTypewriter2 : private daeTypewriter
{
COLLADA_(public)

	using daeTypewriter::copy;
	using daeTypewriter::compare;
	using daeTypewriter::serialize;
	using daeTypewriter::unserialize;
	using daeTypewriter::memoryToString;
	using daeTypewriter::stringToMemory;
	using daeTypewriter::isAnySimpleType;
};

template<int buffer_size>
/**INTERNAL
 * INTERNAL & undocumented as std::streambuf is too cryptic for its own good.
 * This is taken from http://www.mr-edd.co.uk/blog/beginners_guide_streambuf.
 */
class daeOStringBuf : public std::streambuf
{
	//copy ctor and assignment not implemented.
	daeOStringBuf(const daeOStringBuf&);
	daeOStringBuf &operator=(const daeOStringBuf&);

	daeArray<daeStringCP> &obuf; char buf[buffer_size+1];

public:

	daeOStringBuf(daeArray<daeStringCP> &obuf):obuf(obuf)
	{
		setp(buf,buf+buffer_size);
	}	
	//Necessary on windows. A commentator think so. It is.
	//http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
	~daeOStringBuf(){ sync(); }

private:

	virtual int_type overflow(int_type ch)
	{
		if(ch!=traits_type::eof())
		{
			assert(pptr()<=epptr());
			*pptr() = char(ch);
			pbump(1);
			daeOStringBuf::sync();
			return ch;
		}
		return traits_type::eof();
	}	
	virtual int sync()
	{
		std::ptrdiff_t n = pptr()-pbase();
		pbump((int)-n);
		obuf.append_and_0_terminate(pbase(),(size_t)n);
		return 0;
	}
};
/**INTERNAL
 * INTERNAL & undocumented as std::streambuf is too cryptic for its own good.
 * This is taken from http://www.mr-edd.co.uk/blog/beginners_guide_streambuf.
 */
class daeIStringBuf : public std::streambuf
{		
	//copy ctor and assignment not implemented.
	daeIStringBuf(const daeIStringBuf&);
	daeIStringBuf &operator=(const daeIStringBuf&);

public:

	const char *const begin, *const end, *current;
	daeIStringBuf(const char *begin, const char *end)
	:begin(begin),end(end),current(begin)
	{
		assert(begin<=end); 
		//setg((char*)begin,(char*)begin,(char*)end);
	}

public: //NOT GOOD //NOT GOOD //NOT GOOD //NOT GOOD //NOT GOOD

	//These work by calling at least two virtual methods for
	//every character; This is because setg() is never setup
	//and so the stream is in a constantly unbuffered status.
	//
	//"There's simply no advantage to having one because the 
	//data is the buffer" --www.mr-edd.co.uk
	//
	//While true, for this library the blind virtual methods
	//defeat any ability for the compiler to optimize output.
	//This will suffice for now/until there are measurements.
	//(Because daeType constructs the streambuf, the virtual
	//methods are known, and so could be treated like inline
	//methods WRT optimizations; meaning it'd do no better.)
	//
	//daeType will eventually implement an alternate pathway.

	virtual int_type underflow()
	{
		return current==end?traits_type::eof():*current;
	}
	virtual int_type uflow()
	{
		return current==end?traits_type::eof():*current++;
	}
	virtual int_type pbackfail(int_type ch)
	{
		if(current==begin||(ch!=traits_type::eof()&&ch!=current[-1]))
		return traits_type::eof(); return *--current;
	}
	virtual std::streamsize showmanyc()
	{
		assert(current<=end); return end-current;
	}
};
						  
template<class T, class /*Typist*/U=daeTypist<T>>
/**NEVER-CONST
 * @c daeType implements an instance of @c daeTypewriter. It has
 * facilities for tandom "atom" & "array" typewriters. It's easy
 * to use: You simply construct it, and then get the atom writer
 * via the @c daeTypewriter& conversion operator. Generally this
 * process is automatic. The array typewriter is obtained via an
 * API belonging to @c daeTypewriter. It cannot be done directly.
 * @see @c daeTypewriter::where().
 */
class daeType
{
COLLADA_(public) typedef U Typist;

	struct AtomWriter : daeTypewriter, Typist
	{
		virtual void copy(const daeOpaque src, daeOpaque dst)
		{
			Typist::assign(*(T*)&dst,*(const T*)&src);
		}

		virtual int compare(const daeOpaque a, const daeOpaque b)
		{
			return Typist::compare(*(const T*)&a,*(const T*)&b);
		}

		virtual daeOK serialize(const daeOpaque src, daeArray<daeStringCP> &dstIn)	
		{
			daeOStringBuf<Typist::buffer_size> buf(dstIn); std::ostream dst(&buf);
			Typist::template formatXML<T>(dst);
			daeOK OK = Typist::decodeXML(dst,*(const T*)&src);
			if(OK==DAE_OK&&dst.fail()) OK = DAE_ERROR; return OK;
		}

		virtual daeOK unserialize(const daeStringCP *srcIn, const daeStringCP *srcEnd, daeOpaque dst)
		{
			Typist::normalize(srcIn,srcEnd);
			daeIStringBuf buf(srcIn,srcEnd);
			std::istream src(&buf);
			Typist::template formatXML<T>(src);
			daeOK OK = Typist::encodeXML(src,*(T*)&dst);
			if(OK==DAE_OK&&(!src.eof()||src.fail())) OK = DAE_ERROR; return OK;
		}

		template<class A>
		AtomWriter(const A &a)
		:daeTypewriter((T*)0,(Typist*)0),Typist(a){}
		template<class A, class B>
		AtomWriter(const A &a, const B &b)
		:daeTypewriter((T*)0,(Typist*)0),Typist(a,b){}
		AtomWriter():daeTypewriter((T*)0,(Typist*)0){}
	};

	struct ArrayWriter : daeTypewriter, Typist
	{
		typedef daeArray<T> Array;
		
		template<class A>
		ArrayWriter(const A &a)
		:daeTypewriter((Array*)0,(Typist*)0),Typist(a){}
		template<class A, class B>
		ArrayWriter(const A &a, const B &b)
		:daeTypewriter((Array*)0,(Typist*)0),Typist(a,b){}
		ArrayWriter():daeTypewriter((Array*)0,(Typist*)0){}
		
		virtual void copy(const daeOpaque src, daeOpaque dst)
		{
			Typist::assign(*(Array*)&dst,*(const Array*)&src);
		}

		//Pre-2.5 this had first compared the sizes, and passed if unequal.
		//The original comments don't comment on the purpose/role of compare().
		virtual int compare(const daeOpaque aIn, const daeOpaque bIn)
		{
			const Array &a = aIn; const Array &b = bIn;

			//This ALGORITHM is identical to daeTypist<void> except for
			//Typist::compare() has replaced daeTypist<void>::compare().
			int i,cmp,iN; for(i=0,iN=(int)std::min(a.size(),b.size());i<iN;i++) 
			{
				cmp = Typist::compare(a[i],b[i]); if(cmp!=0) return cmp;
			}
			return (int)a.size()-(int)b.size();
		}

		virtual daeOK serialize(const daeOpaque srcIn, daeArray<daeStringCP> &dstIn)	
		{
			const Array &src = srcIn; 
			daeOStringBuf<4096> buf(dstIn); std::ostream dst(&buf);
			daeOK OK; 
			size_t i = 0, iN = src.size();
			Typist::template formatXML<T>(dst,~dst.skipws);
			if(iN!=0) for(;;)
			{
				OK = Typist::decodeXML(dst,src[i]);
				if(OK==DAE_OK&&!dst.good()) OK = DAE_ERROR;
				if(++i<iN&&OK==DAE_OK) dst << ' ';
				else break;
			}			
			return OK;
		}
		
		virtual daeOK unserialize(const daeStringCP *srcIn, const daeStringCP *srcEnd, daeOpaque dstIn)
		{		
			Typist::normalize(srcIn,srcEnd);
			Array &dst = dstIn;
			//REMINDER: THERE SHOULDN'T BE A PRE-ALLOCATION STRATEGY.
			//SEE unserialize() DOXYGENTATION NOTES ON REALLOCATIONS.
			dst.clear(); 
			daeIStringBuf buf(srcIn,srcEnd); std::istream src(&buf);
			Typist::template formatXML<T>(src,~src.skipws);
			//Passing tmp as pointer to make this MSVC warning shutup.
			//"warning C4700: uninitialized local variable 'tmp' used."
			//COLLADA_SUPPRESS_C(4700) //no effect???
			//-Wmaybe-uninitialized is here in -On modes. GCC wants a full push/pop song and dance????
			daeOK OK; T tmp; Typist::supercharge(&dst,&tmp); 
			for(;!src.eof();)
			{
				OK = Typist::encodeXML(src,tmp);
				if(OK!=DAE_OK) break;
				if(src.fail()) return DAE_ERROR;					
				dst.push_back(tmp);
			}
			return OK;
		}
	};

COLLADA_(protected) //DATA MEMBERS

	AtomWriter _daeAtomWriter;
	ArrayWriter _daeArrayWriter;
	/**
	 * This is the Constructor logic.
	 */
	inline void _daeType_init() 
	{
		//daeModel/Feature will need to link to the system types.
		#ifndef BUILDING_COLLADA_DOM		
		//Add a binary_type enum to and typedef void system_type?
		daeCTC<!Typist::system_type>();		
		#endif
		daeCTC<!daeAtomicType::EXTENSION>();
		daeCTC<!Typist::binary_type||!Typist::system_type>();		
		_daeAtomWriter._atomType = Typist::system_type|Typist::binary_type;
		_daeArrayWriter._atomType = daeAtomicType::EXTENSION;
		_daeAtomWriter._daeAtomWriter = &_daeAtomWriter;
		_daeAtomWriter._daeArrayWriter = &_daeArrayWriter;
		_daeArrayWriter._daeAtomWriter = &_daeAtomWriter;
		_daeArrayWriter._daeArrayWriter = &_daeArrayWriter;
	}

COLLADA_(public) //CONSTRUCTOR								
	/**
	 * Default Constructor
	 * For when @a Typist has only a default-constructor.
	 */
	daeType(){ _daeType_init(); }

	template<class A>
	/**
	 * Constructor forwarding one or two arguments to @a Typist.
	 * @a Typist must fill in @a B with @c ... if it's not used.
	 */
	daeType(const A &a):_daeAtomWriter(a),_daeArrayWriter(a)
	{
		_daeType_init(); 
	}
	template<class A, class B>
	/**
	 * Constructor forwarding one or two arguments to @a Typist.
	 * @a Typist must fill in @a B with @c ... if it's not used.
	 */
	daeType(const A &a, const B &b=B()):_daeAtomWriter(a,b),_daeArrayWriter(a,b)
	{
		_daeType_init(); 
	}

COLLADA_(public) //OPERATORS
	/**
	 * Converts to atomic-typewriter.
	 */
	inline operator daeTypewriter&(){ return _daeAtomWriter; }
	/**
	 * Converts to atomic-typewriter.
	 */
	inline operator daeTypewriter*(){ return &_daeAtomWriter; }
};

//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeBoolean> : daeTypist<> //xs:boolean
{	
	enum{ system_type=daeAtomicType::BOOLEAN };

	static daeOK decodeXML(std::ostream &dst, const bool &src)
	{
		dst << src; if(dst.fail())
		{
			dst.clear(); dst.setf(dst.flags()^dst.boolalpha);
			dst << src;
		}
		return DAE_OK;
	}
	static daeOK encodeXML(std::istream &src, bool &dst)
	{
		/*I give up. A GCC/GNU implementation on Cygwin doesn't
		//read the delimiter and so doesn't set eofbit and would
		//presumably accept 1111 as four bool values, etc. Though
		//I only witnessed with boolalpha (falsefalsefalse.)
		src >> dst; if(src.fail())
		{
			src.clear(); src.setf(src.flags()^src.boolalpha);
			src >> dst;
		}*/
		//+1 is for MSVC. I don't get why it doesn't set eofbit??
		char b[6+1]; src >> std::setw(sizeof(b)) >> b;
		switch(b[0])
		{
		case '0': case '1':

			if(b[1]!='\0') break;
			else dst = b[0]=='1'; return DAE_OK;

		case 'f':

			if('a'==b[1]&&'l'==b[2]&&'s'==b[3]&&'e'==b[4]&&b[5]=='\0')
			dst = false; else break; return DAE_OK;

		case 't':

			if('r'==b[1]&&'u'==b[2]&&'e'==b[3]&&b[4]=='\0')
			dst = true; else break; return DAE_OK;
		}
		return DAE_ERROR;
	}
};

//REMINDER: system_type must be visible to XS::Schema::XS::Schema::_typewrit().
//REMINDER: The "size" parameter is strictly for specializing for 1==sizeof(I).
template<class I, int size=sizeof(I)>
/**
 * This is very simple now, but later when std::iostream isn't relied upon, it's
 * going to come in handy to have the integer implementation under a single roof.
 */
struct daeTypist10 : daeTypist<>
{
	enum{ system_type=(I(-1)<0?-1:1)*int(sizeof(I)) };

	/**
	 * This is added to use an @c unsigned type so the default
	 * value of <stencil_mask> doesn't overflow (it ought to be
	 * xs:unsignedLong instead of xs:long.)
	 */
	static daeOK encodeXML(std::istream &src, I &dst)
	{
		//The '-' check is for daeAST::TypedUnion handling, but
		//also, Visual Studio, while converting to unsigned int
		//does not fail if the figure after sign exceeds INTMAX.
		if(system_type<0&&sizeof(int)==sizeof(I)&&'-'!=src.peek())
		{				
			daeCTC<sizeof(int)==4>();
			//I think -1 will convert to an unsigned value, but
			//4294967295 (0xFFFFFFFF) into 32 signed bits fails.
			//TWO'S-COMPLEMENT
			//A quote from some standard suggesting -1 is valid.
			//"scanf converts %u by matching an optionally signed decimal integer"
			src >> (unsigned int&)dst;
		}
		else src >> dst; return DAE_OK;
	}	
};
//REMINDER: system_type must be visible to XS::Schema::XS::Schema::_typewrit().
template<class I>
/**PARTIAL-TEMPLATE-SPECIALIZATION
 * This specialization handles char-size integers, which std::iostream otherwise
 * will apply char-semantics to, which are unsuited to xs:byte & xs:unsignedByte.
 */
struct daeTypist10<I,sizeof(char)> : daeTypist<>
{
	enum{ system_type=I(-1)<0?-1:1 };

	static daeOK decodeXML(std::ostream &dst, const I &src)
	{
		int proxy = src; return daeTypist<>::decodeXML(dst,proxy);
	}
	static daeOK encodeXML(std::istream &src, I &dst)
	{
		int proxy; daeOK OK = daeTypist<>::encodeXML(src,proxy); 
		if(!src.fail()) dst = (I)proxy; return OK;
	}
};
template<> struct daeTypist<char> : daeTypist10<char> //xs:byte?
{};
template<> struct daeTypist<signed char> : daeTypist10<signed char> //xs:byte?
{};
template<> struct daeTypist<unsigned char> : daeTypist10<unsigned char> //xs:unsignedByte?
{};
template<> struct daeTypist<short> : daeTypist10<short> //xs:short?
{};
template<> struct daeTypist<unsigned short> : daeTypist10<unsigned short> //xs:unsignedShort?
{};
template<> struct daeTypist<int> : daeTypist10<int> //xs:int?
{};
template<> struct daeTypist<unsigned int> : daeTypist10<unsigned int> //xs:unsignedInt?
{};
template<> struct daeTypist<long> : daeTypist10<long> //xs:long?
{};
template<> struct daeTypist<unsigned long> : daeTypist10<unsigned long> //xs:unsignedLong?
{};
template<> struct daeTypist<long long> : daeTypist10<long long> //xs:long?
{};
template<> struct daeTypist<unsigned long long> : daeTypist10<unsigned long long> //xs:unsignedLong?
{};

template<class FP, class I, I NaN, I INF, I _INF>
//REMINDER: system_type must be visible to XS::Schema::XS::Schema::_typewrit().
struct daeTypistFP : daeTypist<> 
{	
	//COLLADA_DOM_ATOMIC_FLOAT isn't because this is a template.
	//This daeType must match the width of the type as it is now.
	//The daeAtomicType enum must also match. Even if it's unused.
	enum{ system_type=COLLADA_DOM_ATOMIC_FLOAT(FP) };

	daeTypistFP(){ daeCTC<sizeof(I)==sizeof(FP)>(); }

	static daeOK decodeXML(std::ostream &dst, const FP &src)
	{
		//A GOOD IS-FINITE TEST HERE COULD WINNOW ALL THREE.
		if(src!=src) dst << "NaN";
		else if((I&)src==INF) dst << "INF"; //formatXML() sets up formatting.
		else if((I&)src==_INF) dst << "-INF"; else dst << src; return DAE_OK;
	}
	static daeOK encodeXML(std::istream &src, FP &dst)
	{	
		char sign = src.peek(); //Lost on failure.
		src >> dst; if(!src.fail()) return DAE_OK;		
		src.clear();		
		
		char buf[8]; src >> std::setw(sizeof(buf)) >> buf;
		switch(buf[0])
		{
		case 'N': if(buf[1]!='a'||buf[2]!='N'||buf[3]!='\0') break;
		//These could become overwhelming. (Could store a flag?)
		//daeEH::Warning<<"NaN encountered while setting an attribute or value";
		(I&)dst = NaN; return DAE_OK;
		case 'I': if(buf[1]!='N'||buf[2]!='F'||buf[3]!='\0') break;
		//These could become overwhelming. (Could store a flag?)
		//daeEH::Warning<<"INF encountered while setting an attribute or value";
		(I&)dst = sign=='-'?_INF:INF; return DAE_OK;
		//KEEPING THIS HERE IN CASE COMPILERS BEHAVE DIFFERENTLY.
		case '-': if(buf[1]!='I'||buf[2]!='N'||buf[3]!='F'||buf[4]!='\0') break;
		//DOCUMENT ME (compiler quirks... MSVC2015 eats the sign.)
		assert(0);
		//These could become overwhelming. (Could store a flag?)
		//daeEH::Warning<<"-INF encountered while setting an attribute or value";
		(I&)dst = _INF; return DAE_OK;
		}
		return DAE_ERR_QUERY_SYNTAX;
	}
};
template<> struct daeTypist<float> : //xs:float (although daeFloat can be double.)
daeTypistFP<float,unsigned,0x7f800002U,0x7f800000U,0xff800000U>
{}; 
template<> struct daeTypist<double> : //xs:double (although daeDouble can be float.)
daeTypistFP<double,unsigned long long,0x7ff0000000000002ULL,0x7ff0000000000000ULL,0xfff0000000000000ULL>
{}; 

//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeStringRef> : daeTypist<> //xs:string
{	
	enum{ system_type=daeAtomicType::STRING };
	 
	static int compare(const daeStringRef &a, const daeStringRef &b)
	{
		return strcmp(a,b);
	}
	
	static daeOK decodeXML(std::ostream &dst, const daeStringRef &src)
	{
		dst << (daeString)src; return DAE_OK;
	}
						 
	static daeOK encodeXML(std::istream &src, daeStringRef &dst)
	{
		//MUST IMPLEMENT <xs:list itemType="xs:string">??
		//DAEP::Container<daeString>::type makes this unlikely.
		assert(~src.flags()&src.skipws); //return DAE_ERROR;
		assert(dynamic_cast<daeIStringBuf*>(src.rdbuf())!=nullptr);
		daeIStringBuf *dae = (daeIStringBuf*)src.rdbuf();
		COLLADA_SUPPRESS_C(4244)
		dst.set(dae->begin,dae->end-dae->current); 		
		src.setstate(src.eofbit); return DAE_OK;
	}	
};
/**LEGACY, WISE?
 * @see @c daeAtomicType::TOKEN concerns.
 * This class distinguishes xs:token types from xs:string types. 
 * (They treat whitespace differently.)
 * It's currently used for all short-text types, even for types
 * that should never have spaces. 
 * There's no real reason why. Except the types are technically
 * supposed to be "normalized."
 *
 * @note @c daeDomTypes.h will use @c daeStringRef unless there
 * is an <xs:list> involved ALTHOUGH system types use this type
 * internally when reading/writing values via the meta back end.
 */
class daeTokenRef : public daeStringRef
{
COLLADA_(public) //C++ formalities

	using daeStringRef::__COLLADA__move;
	using daeStringRef::__COLLADA__place;

	//using daeStringRef::operator=;
	template<class T>
	/**
	 * "using daeStringRef::operator=;" is ambiguous.
	 * Besides, it should return its own type.
	 */
	inline daeTokenRef &operator=(const T &cp)
	{
		return (daeTokenRef&)daeStringRef::operator=(cp);
	}
	/**C++ BUSINESS
	 * This must be present or C++ will generate @c operator=.
	 */
	inline daeTokenRef &operator=(const daeTokenRef &cp)
	{
		return (daeTokenRef&)daeStringRef::operator=(cp);
	}
};
template<> struct daeTypist<daeTokenRef> : daeTypist<daeStringRef> //xs:token
{
	enum{ system_type=daeAtomicType::TOKEN };

	static daeOK encodeXML(std::istream &src, daeTokenRef &dst)
	{
		//daeURI and maybe more are copying this implementation;
		//albeit with generic global @c operator>> implementations.
		daeStringCP buf[512]; src >> std::setw(sizeof(buf)) >> buf;
		assert(buf[0]=='\0'&&src.eof()||strlen(buf)!=sizeof(buf)-1);
		#ifdef NDEBUG
		#error Would like to to use dst.set(buf,extent) here.		
		#endif		
		if(!src.fail()) 
		{
			//This is so BEZIER BEZIER BEZIER style name arrays do
			//not invoke the string-table for subsequent identical
			//tokens. It's a good strategy in general, but COLLADA
			//should've used its regular indexing model with these.
			if(0==src.precision(0)) //HACK: Don't touch the memory!
			{
				size_t len = dst.size(); 
				assert(len!=0&&len*sizeof(daeStringCP)<sizeof(buf));
				if(buf[len]=='\0'&&dst[len-1]==buf[len-1]
				&&0==memcmp(dst,buf,len*sizeof(daeStringCP)))			
				return DAE_OK;
			}
			//TODO? daeStringRef could use a non-const buffer setter.
			//It would not help here, but it would make users' lives
			//easier. Unfortunately it'll take some thought to do it.
			dst = (daeString)buf;
		}
		return DAE_OK;
	}
	static daeOK decodeXML(std::ostream &dst, const daeTokenRef &src)
	{
		//Write out %20s just in case the string is loaded with spaces.
		_write_20(dst,src); return DAE_OK;
	}
	static void _write_20(std::ostream &o, daeString p)
	{
		for(daeStringCP c;;) switch(c=*p++)
		{
		case '\0': return; case ' ': o.write("%20",3); break;		
		case '\t': case '\r': case '\n': assert(0); default: o.put(c);
		}		
	}
};	

/**TODO, INTERNAL 
 * 
 * @todo This header takes up a lot of space for very little.
 * It especially increases the footprint of xs:anySimpleType
 * data units, and doesn't add much to arrays of binary data.
 * A time type might eventually make xs:anySimpleType larger
 * anyway. The only way to excise this header is to make the
 * @c daeBinary class to not build on @c daeArray, or to not
 * use @c public inheritance.
 */
struct __daeBinary__static
{ 
	//REMINDER: Don't change this to "short" since there
	//must be 8 bytes to __daeBinary__static in total...
	int _base; 	
	//HACK: daeBinary needs this structure to be 8 bytes
	//to make daeBinary<N,0> and daeBinary<N,!0> line up.
	//NOTE: "surplus" is something else, and not padding.
	//#if 8!=CHAR_BIT
	unsigned char _surplus;
	//#endif
};
template<int base, int size_on_stack> //These defaults are 0,0.
/**WARNING, VARIABLE-LENGTH
 * This class implements XML Schema hexBinary and base64Binary
 * The internal storage unit is @c char, so that the size of a
 * @c daeByte is not dependent on it. They're likely unrelated.
 *
 * @warning This is a literal interpretation of the XML Schema
 * binary types. It seems like a safe default. But if there is
 * time, it seems like a better approach would be to store the
 * separators in a separate array, so that the data itself can
 * be back-to-back in memory. The rationale for an alternative
 * is that base64Binary for whatever back-compatibility reason
 * can include whitespace. This means that functionally a list
 * is identical to a non-list. The standard doesn't comment on
 * hexBinary, but it seems appropriate that binary types would
 * be similar. The rationale for whitespace is either to shape
 * the data, or to meet olden mail-gateway restrictions. Why??
 * is not stated. Arrays-of-arrays can have benefits, but if a
 * block of data is chopped up considerably for these purposes
 * then the harm would seem to outweigh any benefit. 
 * @see @c unserialize<16>() & @c unserialize<64>().
 */
class daeBinary : __daeBinary__static, public daeArray<char,size_on_stack>
{
	inline void _init()
	{
		//An enum classification of acceptible bases is in order.
		_base = base; daeCTC<base==16||base==64>(); setSurplus();
		//Dread if this fails and multi-inheritance order is reversed.
		assert(0==daeOffsetOf(daeBinary,_base));
		//Something is off??? __daeBinary__static::_surplus props this up.
		assert(daeOffsetOf(daeBinary,getAU())==daeOffsetOf(daeBinary<base>,getAU()));
	}

COLLADA_(public) //CONSTRUCTORS	
	/**
	 * Default Constructor
	 */
	daeBinary(){ _init(); }
	/**
	 * Default Constructor for derived/unembedded prototyping support.
	 * The 0 is intentional, as long as no compiler refuses to use it.
	 */
	daeBinary(const daeAlloc<char> &AU):daeArray<char,0>(AU){ _init(); }
	/**
	 * Embedded Constructor that requires @c size_on_stack be nonzero.
	 */
	daeBinary(const daeObject *c, size_t real_size=size_on_stack)
	:daeArray<char,size_on_stack>(c,real_size){ _init(); }

	template<class Type> //DAEP::Value<...>
	/**
	 * Prototype Constructor
	 */
	explicit daeBinary(const DAEP::Proto<Type> &pt)
	:daeArray<char,size_on_stack>(pt){ /*NOP //_init();*/ }

COLLADA_(public) //OPERATORS

	template<int N>
	/** @c base is just keep tracking the XML Schema type. */
	inline operator daeBinary<N>&(){ return *(daeBinary<N>*)this; }
	template<int N>
	/** @c base is just keep tracking the XML Schema type. */
	inline operator const daeBinary<N>&()const{ return *(daeBinary<N>*)this; }

	template<int N>
	/**WARNING 
	 * Assignment Operator
	 * @warning @c base is not copied, as it's part of the type.
	 */
	daeBinary &operator=(const daeBinary<N> &cp)
	{
		static_cast<daeArray<char>&>(*this) = cp; 
		setSurplus(cp.getSurplus()); return *this;
	}
	/** Suppress compiler-generated assignment operator. */
	daeBinary &operator=(const daeBinary &cp)
	{
		static_cast<daeArray<char>&>(*this) = cp; 
		setSurplus(cp.getSurplus()); return *this;
	}

COLLADA_(public) //Binary accessors & mutators
	/**
	 * Empties the @c char array and sets @c getSurplus() to 0.
	 */
	void clear(){ daeArray<char>::clear(); setSurplus(); }

	/**
	 * @return Returns 16 for hexBinary or 64 for base64Binary.
	 * These are XML Schema types. Other vales may be returned.
	 */
	inline int getBase()const{ return _base; }
	
	/**
	 * @return Returns the number of unused bits on the end of
	 * the binary-blob. Users must set it with @c setSurplus().
	 */
	inline unsigned char getSurplus()const
	{
		#if 8!=CHAR_BIT
		return _surplus; 
		#endif
		return 0;
	}
	/**WARNING
	 * @warning This is strictly in case @c CHAR_BIT is larger
	 * than 8. That is exceedingly unlikely on today's systems. 
	 */	
	void setSurplus(unsigned char bits=0)
	{
		#if 8!=CHAR_BIT
		assert(bits<CHAR_BIT);
		_surplus = bits;
		#else
		assert(bits==0);
		#endif
	}

COLLADA_(public) //Serialization APIs
	/**
	 * Writes @c base binary char-data to @c std::ostream.
	 */
	inline void serialize(std::ostream &dst)const
	{
		serialize<base>(dst);
	}
	//This is implemented as a switch since GCC/C++ can't
	//explicitly specialize class methods. It matters not.
	template<int N>
	/**
	 * @tparam N must be 0, 16, or 64, where 0 is @c base.
	 */
	inline void serialize(std::ostream &dst)const
	{
		switch(N!=0?N:getBase())
		{
		case 16: _serialize_16(dst); break;
		case 64: _serialize_64(dst); break;
		default: dst.setstate(dst.failbit); assert(0);
		}
	}
	/**
	 * Inserts hexBinary char-data into a @c std::ostream. 
	 * @note The algorithm is made complex because a char
	 * may not be 8-bits. But that is very unlikely to be.
	 * It's an open-question if such systems treat a char
	 * as a char or a word. This implementation assumes a
	 * char. (Compilers should eliminate the added code.)
	 */
	inline void _serialize_16(std::ostream &dst)const
	{
		const char *p = daeArray<char>::data();
		const char *d = p+daeArray<char>::size();
		if(0!=getSurplus()) d--; 
		signed char i,j,c; 
		for(;p<d;p++) 
		for(i=0;i<CHAR_BIT;i+=8) //This is academic.
		for(j=i+4,c=*p;j>=i;j-=4) surplus:
		{
			//Can Endianness be related to CHAR_BIT?
			char nibble = c>>j&0xF;
			dst.put((nibble>=10?'a'-10:'0')+nibble); 
		}
		if(0<getSurplus()&&p==d) //This is academic.
		{
			i = getSurplus();
			j = i+4; c = *++p>>i;
			if(i%8==0) goto surplus;
			daeCTC<CHAR_BIT%8==0>();
			dst.setstate(dst.failbit); assert(0);
		}
	}
	/**UNIMPLEMENTED: ASK AND YOU SHALL RECEIVE.
	 * Inserts base64Binary char-data into a @c std::ostream.
	 */
	inline void _serialize_64(std::ostream &dst)const
	{
		//UNIMPLEMENTED: ASK AND YOU SHALL RECEIVE.
		if(!daeArray<char>::empty())
		{
			dst.setstate(dst.failbit); assert(0);
		}
	}

	/**
	 * Extracts @c base char-data from an @c std::istream.
	 */
	inline void unserialize(std::istream &src)const
	{
		unserialize<base>(src);
	}
	//This is implemented as a switch since GCC/C++ can't
	//explicitly specialize class methods. It matters not.
	template<int N>
	/**
	 * @tparam N must be 0, 16, or 64, where 0 is @c base.
	 *
	 * Extracts @c getBase() char-data from an @c std::istream.
	 */
	inline void unserialize(std::istream &src)
	{
		switch(getBase())
		{
		case 16: _unserialize_16(src); break;
		case 64: _unserialize_64(src); break;
		default: src.setstate(src.failbit); assert(0);
		}
	}
	/** 
	 * Extracts hexBinary char-data from an @c std:istream. 
	 * @remarks This class stops on whitespace. It doesn't
	 * check the skipws formatting flag. The standard says
	 * this is optional. Whenever there is time & interest
	 * thought can be given to an alternative class--which
	 * could be made the default or only implementation if
	 * that is decided. The @c daeBinary Doxygentation has
	 * more to say about this.
	 * @see @c serialize<16>() notes.
	 */
	inline void _unserialize_16(std::istream &src)
	{
		clear();
		std::istream::sentry s(src); //Skip whitespace.
		if(!s) return;		
		signed char i,j,nibble,value=0; 
		for(;;daeArray<char>::push_back(value),value=0)
		for(i=0;i<CHAR_BIT;i+=8) //This is academic.
		for(j=i+4;j>=i;j-=4) 
		{
			nibble = src.get(); 			
			if(nibble<'0'||nibble>'9')
			{
				//Give priority to 0~9.
				nibble = tolower(nibble);
				if(nibble<'a'||nibble>'f')
				{
					if(!src.eof()) 
					{
						//Preserve state?
						src.putback(nibble);
						//Signal illegal character so list
						//processing halts. 
						if(!COLLADA_isspace(nibble))
						goto fail;
					}
					else //clear();
					{
						//EOF==get() is setting the failbit???
						src.clear(~src.failbit&src.rdstate());
					}
					//Enforce two chars per value?
					if(j!=4) if(j%8!=0)
					{
						setSurplus(CHAR_BIT-j-4); 
						daeArray<char>::push_back(value);
					}
					else 
					{
						//And what about _failure_to_unserialize?
						//These could become overwhelming. (Could store a flag?)
						//daeEH::Error<<"hexBinary had odd character?";

						fail: src.setstate(src.failbit); 
					}
					return;
				}
				else nibble = 10+nibble-'a';
			}//Can Endianness be related to CHAR_BIT?
			else nibble-='0'; value+=nibble<<j;
		}		
	}
	/**UNIMPLEMENTED: ASK AND YOU SHALL RECEIVE.
	 * Extracts base64Binary char-data from an @c std::istream.
	 */
	inline void _unserialize_64(std::istream &src)
	{
		//REMINDER: According to the XML Schema standard, it seems
		//like whitespace is permitted in order to facilitate olden
		//practices of adding line-breaks for email servers. But if
		//so it is not permitted in the <xs:list> context. Or so it
		//seems. (Sources are few, the standards crypic and dense.)
		//It's unclear if hexBinary should enjoy whitespace. Maybe?

		std::istream::sentry s(src); //Skip whitespace.
		//UNIMPLEMENTED: ASK AND YOU SHALL RECEIVE.
		if(src.get()>0){ src.setstate(src.failbit); assert(0); }
	}

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;
	//daeArray has helpers that make this vastly simpler than it'd otherwise be.
	static void __COLLADA__move(const daeObject *obj, daeBinary *lv, daeBinary *rv, size_t iN)
	{
		daeArray<char,size_on_stack>::_derive__COLLADA__move(obj,lv,rv,iN,_derive__COLLADA__mover);
	}		
	static void _derive__COLLADA__mover(daeBinary &lv, const daeBinary &rv)
	{
		lv._base = rv._base; lv.setSurplus(rv.getSurplus());
	}
	inline void __COLLADA__locate(const daeObject *obj, const daeBinary &cp)
	{
		daeArray<char,size_on_stack>::__COLLADA__locate(obj,cp); _derive__COLLADA__mover(*this,cp);
	}

COLLADA_(public) //daeTypewriter support
	/** 
	 * Inputs a string.
	 * The base is tied to @c base. It's safe to cast to a different base. 
	 */
	friend std::istream &operator>>(std::istream &i, daeBinary &bin)
	{
		bin.unserialize<base>(i); return i;
	}
	/** 
	 * Outputs a string.
	 * The base is tied to @c base. It's safe to cast to a different base. 
	 */
	friend std::ostream &operator<<(std::ostream &o, const daeBinary &bin)
	{
		bin.serialize<base>(o); return o;
	}
};
template<int M, int N> struct daeTypist<daeBinary<M,N>> : daeTypist<>
{	
	enum{ binary_type=daeAtomicType::BINARY, buffer_size=4096 };

	inline int compare(const daeBinary<> &a, const daeBinary<> &b)
	{
		int cmp = memcmp(a.data(),b.data(),std::min(a.size(),b.size()));
		return cmp!=0?cmp:(int)a.size()-(int)b.size();
	}
};
//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeBinary<16/*,N*/>> : daeTypist<daeBinary<>> //xs:hexBinary
{	
	enum{ binary_type,system_type=daeAtomicType::BINARY };
};
//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeBinary<64/*,N*/>> : daeTypist<daeBinary<>> //xs:base64Binary
{	
	enum{ binary_type,system_type=daeAtomicType::BINARY };
};

/**NEVER-CONST
 * @c daeAnySimpleTypewriter facilitates xs:anySimpleType logic.
 * @note This class is derived (backward) from @c daeData. It's
 * more of an afterthought, given that xs:anySimpleType is very
 * low priority, by nature.
 */
class daeAnySimpleTypewriter : public daeTypewriter
{	
COLLADA_(private) //UTILITIES

	typedef void *A; //alignment
	enum{ _A = -(int)sizeof(A) };	
	size_t _size(daeOpaque);
	bool _reset_type(daeOpaque,size_t);
	inline daeTypewriter *_typewriter()
	{
		return hasListType()?_daeArrayWriter:_daeAtomWriter;
	}
	friend daeData;
	static daeAnySimpleTypewriter*&_type(daeOpaque o){ return o[_A]; }
	static daeAnySimpleTypewriter *_type(daeString,daeString,size_t&);

COLLADA_(public) //daeTypewriter methods

	virtual void copy(const daeOpaque src, daeOpaque dst)
	{	
		_assert_this_and_type(dst);
		if(this!=_type(src)) 
		return _type(src)->copy(src,dst);
		if(this==_type(dst)||_reset_type(dst,_size(src)))
		_typewriter()->copy(src,dst);
		else assert(0); //programmer error
	}

	virtual int compare(const daeOpaque a, const daeOpaque b)
	{
		_assert_this_and_type(b);
		if(this!=_type(a)) 
		return _type(a)->compare(a,b);
		if(this==_type(b)) 
		return _typewriter()->compare(a,b);
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
			assert(s==0);
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

	virtual daeOK serialize(const daeOpaque src, daeArray<daeStringCP> &dstIn)	
	{
		_assert_this_and_type(src);
		return _type(src)->_typewriter()->serialize(src,dstIn);
	}

	virtual daeOK unserialize(const daeStringCP *srcIn, const daeStringCP *srcEnd, daeOpaque dst)
	{
		_assert_this_and_type(dst);
		daeAnySimpleTypewriter *type; size_t size;
		type = _type(srcIn,srcEnd,size);
		return type->_unserialize2(srcIn,srcEnd,dst,size);
	}
	friend class daeDefault;
	inline daeOK _unserialize2(daeString srcIn, daeString srcEnd, daeOpaque dst, size_t size)
	{
		if(_type(dst)!=this&&!_reset_type(dst,size)
		||DAE_OK!=_typewriter()->unserialize(srcIn,srcEnd,dst).error)
		return _failure_to_unserialize(srcIn,srcEnd,dst,size);
		return DAE_OK;		
	}
	/**PROGRAMMER ERROR?
	 * @note This is promoting 32-bit values to 64-bit types in order
	 * to not have to parse the individual values of arrays carefully.
	 * Falls back to xs:string in cases where @c _type() has selected
	 * an incompatible binary representation. 
	 * @note This should not occur, but if data were dropped it would
	 * be unacceptable.
	 */
	daeOK _failure_to_unserialize(daeString,daeString,daeOpaque,size_t);

COLLADA_(private) //daeAnySimpleType relationship

	friend class daeAnySimpleType;
	/**
	 * @c daeAnySimpleType Constructor 
	 *
	 * @param decayed_type is an invariant typewriter
	 * pair which @c daeTypewriter::where<>() returns.
	 */
	daeAnySimpleTypewriter(daeTypewriter &decayed_type)
	:daeTypewriter(decayed_type)
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

COLLADA_(private) //INTERNAL
	
	daeAnySimpleTypewriter _daeAtomWriter;
	daeAnySimpleTypewriter _daeArrayWriter;

COLLADA_(public) //INTERNAL
	/**
	 * @c daeAnySimpleTypewriter Constructor
	 */
	daeAnySimpleType(daeTypewriter &decayed_type)
	:_daeAtomWriter(decayed_type.where<daeAtom>()),
	_daeArrayWriter(decayed_type.where<daeArray>())
	{}

COLLADA_(public) //OPERATORS
	/**INTERNAL
	 * Use [1] to access the @c daeArray typewriter.
	 */
	inline operator daeAnySimpleTypewriter*()
	{
		return &_daeAtomWriter;
	}
	/**INTERNAL
	 * C2819 says -> is different from a conversion
	 * to pointer operator. This accesses the owned
	 * atom typewriter in cases where any will work.
	 */
	inline daeAnySimpleTypewriter *operator->()
	{
		return &_daeAtomWriter;
	}
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ATOMIC_TYPE_H__
/*C1071*/
