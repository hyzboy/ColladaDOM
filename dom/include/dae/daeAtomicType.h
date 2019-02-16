/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_ATOMIC_TYPE_H__
#define __COLLADA_DOM__DAE_ATOMIC_TYPE_H__

//daeBoolean::operator>> is unable to construct
//daeHashString from char[8].
//#include "daeArray.h"
//#include "daeStringRef.h"
#include "../DAEP.h"

COLLADA_(namespace)
{//-.
//<-'

COLLADA_DOM_LINKAGE2
extern daeTypewriter daeAtomicType_void;

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
 * SHORTHAND/UPDATE NOTE
 * @c daeAtom is doubling as a shorthand notation for
 * writing these @c enum names. It was created to use
 * with @c daeTypewriter::where<daeAtom>().
 * 
 * @warning Historically "int" and "float" types will
 * depend on daePlatform.h macros and/or build script
 * parameters. For example, @c DOUBLE may be @c float.
 * The values themselves uniquely identify types, but
 * the names do not. It's possible to use client code
 * that has different mappings than the shared module.
 */
struct daeAtomicType
{
	enum
	{
		_ANY_ = 0, //system_type

	//NOTE: These types are so arranged so that the
	//signed/unsigned keywords can be extracted from
	//these values. 
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
				
		_INT_FAMILY_END_ = 8+1, //9
				  
	//This is used by the daeTypist<float> specialization.
	#define COLLADA_DOM_ATOMIC_FLOAT(x) 6+sizeof(x)
	/** daeFloat atomic type. 10 if @c float. */
	FLOAT = COLLADA_DOM_ATOMIC_FLOAT(daeFloat),
	/** daeDouble atomic type. 14 if @c double. */
	DOUBLE = COLLADA_DOM_ATOMIC_FLOAT(daeDouble),

	//NOTE: sizeof(char) is avoiding doing sizeof(daeBoolean).
	/** @c daeBoolean atomic type */
	BOOLEAN = 11,
	/** @c daeEnumeration atomic type */
	ENUMERATION = 12,
	/** @c daeTypist<bool> union type */
	UBOOLEAN = sizeof(char)==sizeof(bool)?BOOLEAN:ENUMERATION,
	
  //Non "plain-old-data" (POD) types..//

	//VOID is not true for isPOD but is true
	//for hasPOD_representation in the union
	//context.
	/**QUASI-POD
	 * This type always yields an empty string. 
	 * Its purpose is to appear as if there is no 
	 * value so code can be written without @c nullptr
	 * tests, and so on. 
	 * FURTHER MORE: @c daeAST::TypedUnion is initially
	 * @c VOID type. Because it's POD it can readily be
	 * switched into another POD type without involving
	 * memory ownership code.
	 * @see @c daeVoidWriter
	 */
	VOID = 18,_NON_POD_BEGIN_,
  							   				   
	/** daeBinary atomic type */
	BINARY = 20,

	//Planning on using daeTags for xmlns values.
	//XMLNS, 

  //EXTENSION (30) followed by string types...//

	//NOTE: If user code registers 
	//extensions by allocating new
	//codes here, _STRINGS_BEGIN_ 
	//would be set to a very large
	//number to make room for them.

	//WARNING: Formerly assigned 0.
	/** 
	 * This value is used for anything that doesn't have
	 * a type.
	 * 
	 * @note All @c daeArray use this value. In addition
	 * for xs:anySimpleType, when the array's subscripts
	 * vary in type the @c daeAtom type is this type too.
	 * In short this value says it alone is insufficient 
	 * to draw a conclusion. It's ambiguous.
	 */
	EXTENSION = 30,_STRINGS_BEGIN_,

	//NOTE: In theory there can be
	//many string based types. The
	//need to distinguish each one
	//is debatable. To make things
	//easy, any equal-to/in-excess
	//of STRING are strings values.
		 		  
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
	 */
	STRING,TOKEN, 

  //NOTHING FOLLOWS STRINGS//
	};

	daeAtomicType()
	{
		daeCTC<(BYTE>=SHORT&&SHORT>=INT&&INT>=LONG&&DOUBLE>=FLOAT)>();
		daeCTC<BOOLEAN==UBOOLEAN||sizeof(daeEnumeration)==sizeof(bool)>();
	}
};

typedef class daeOpaqueWriter daeVoidWriter;
/**EXPERIMENTAL
 * Trying to develop a transparent typewriter without an RTTI component.
 * @note @c daeAtomicType::VOID is implemented by the default inferface.
 */
class daeOpaqueWriter
{
COLLADA_(public) //ABSTRACT-INTERFACE
	/**
	 * Non-virtual Destructor
	 */
	~daeOpaqueWriter(){}
	
	/**ABSTRACT-INTERFACE
	 * Performs a virtual copy operation.
	 * @param src Memory location of the value to copy from.
	 * @param dst Memory location of the value to copy to.
	 */
	virtual void copy(const daeOpaque src, daeOpaque dst){ return; }
	
	/**ABSTRACT-INTERFACE
	 * Performs a virtual comparison operation between two values of the same atomic type.
	 * @param value1 Memory location of the first value.
	 * @param value2 Memory location of the second value.
	 * @return Returns a positive integer if value1 > value2, a negative integer if 
	 * value1 < value2, and 0 if value1 == value2.
	 */
	virtual int compare(const daeOpaque value1, const daeOpaque value2){ return 0; }
	
	/**ABSTRACT-INTERFACE
	 * Appends @a src to @ dst. @a src is untagged, opaque memory (not type-checked.)
	 * @note @c dst.clear() is NOT done, whereas @c memoryToString() DOES @c clear().
	 * @return Returns a client defined @c DAE_ERROR @c enum. @a dst is 0 terminated.
	 */
	virtual daeOK serialize(const daeOpaque src, daeArray<> &dst){ return DAE_OK; }	
	
	/**ABSTRACT-INTERFACE
	 * @warning If @c nullptr==string @c daeAnySimpleType is
	 * assigned to @c daeAtom::VOID type.
	 *
	 * Writes @a string to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param extent is the length of @a string. If the high bit is set it's understood 
	 * to be an append operation with @c ~extent as the size.
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 * @see @c append()
	 *
	 * REMINDER: If a desire is to minimize reallocations, the 
	 * optimal route is for the loader to daeObject::reAlloc().
	 * But it's only best to do so if the array is empty or if
	 * the type of the daeArray is raw-copyable plain-old-data.
	 * IN THIS CASE THE LOADER SHOULD COUNT THE ' ' CHARACTERS.
	 */	
	virtual daeOK unserialize(const daeStringCP *string, size_t extent, daeOpaque dst)
	{
		//I/O plugins need to check Simple content-model values in order to avoid
		//this assert. Use daeElement::hasCharData.
		assert(!"daeAtomicType_void::unserialize"); return DAE_ERR_NOT_IMPLEMENTED;
	}

  ////TODO: MAYBE THE REST IS BETTER IN daeTypewriter ////

COLLADA_(public) //BUFFERED DATA EXTENSIONS USING unserialize	

	//I don't know what these are for. I implemented 
	//them though, and so am willing to wait and see.
	//They may be useful at buffered input in theory. 
	//In practice buffering the input will likely do
	//better. Some input buffering is unavoidable. I
	//have long felt like it seems a shame to always
	//clear the arrays/string, since it's so easy to
	//just append. I implemented these while working 
	//on daeAnySimpleType::TypedUnion. I wasn't sure
	//they would help, but I was thinking about ways
	//to change the type midstream without having to
	//start over. I saw that adding an append option
	//would work. Still, I don't know what it's good
	//for. Especially because change-notices need to
	//implement back-to-back append calls inside the
	//change-notice handler to be correct. Appending
	//adds allocation-unit management overhead/usage
	//to the database.

	/**EXPERIMENTAL/QUESTIONABLE
	 * Clears array/list data, and string data, or possibly supported user-defined data.
	 * @return Returns and error code if data cannot be cleared. Note, if a type can be
	 * cleared, then it must also implement @c append(). @c clear() can be used to find
	 * out if it's safe to append, however other than simple atomic types, user-defined
	 * types are the only type that might not implement @c append().
	 * Unlike data having a fixed binary format, @c daeAnySimpleType data may implement
	 * unique semantics for clearing.
	 */
	inline daeOK clear(daeOpaque dst)
	{
		//Using nullptr sets nonstatic types to daeAtom::VOID.
		//return unserialize(nullptr,0,dst); 
		return unserialize(empty_daeString1,0,dst); 
	}

	/**EXPERIMENTAL/QUESTIONABLE
	 * @warning @c daeAnySimpleType::TypedUnion and @c daeArray data is ambiguous about
	 * whitespace. In order to append to their data @c where<daeAtom>() must be used to
	 * isolate the atomic data; otherwise they always append new elements to their list.
	 * @see @c daeTypewriter::appendToBack()
	 * !!!!!FURTHER-WARNING!!!!!
	 * Token-string data has rules about trimming whitespace that must be observed when
	 * using @c append(). Leading whitespace is removed only if appending to empty data.
	 * Trailing whitespace is always removed:
	 * FROM THIS IT FOLLOWS that to correctly append to string data TRAILING whitespace
	 * must be trimmed from the input until the final append operation. In this way the
	 * leading spaces become inner spaces, and the final space is included in xs:string
	 * strings, but will be trimmed from other datatypes.
	 *
	 * Appends data to array/list or string data, or @c daeBinary data. Only string and
	 * binary types are safe to append without a whitespace or other separator boundary.
	 * With @c daeTypewriter::appendToBack() it's possible to append without separators.
	 *
	 * @note The goal of appending is to read/copy buffered data into the binary object.
	 * As-such the text should be split on whitespace, and should not be attempted with
	 * runs of text that are reasonably short. @c daeBinary buffers overflow for a byte.
	 * @c daeAtomicType::EXTENSION data requires a whitespace for array data; non-array
	 * data (i.e. @c !daeTypewriter::hasListType()) is a user-defined type, which might
	 * not implement @c append. In that case if @c clear() does not return an error, it
	 * can be considered safe to use @c append(); and if not, then use @c unserialize().
	 *
	 * @return Returns and error code if data cannot be appended.
	 * Unlike data having a fixed binary format, @c daeAnySimpleType data may implement
	 * unique semantics for appending.
	 */
	inline daeOK append(const daeStringCP *string, size_t extent, daeOpaque dst)
	{
		return unserialize(string,~extent,dst);
	}

COLLADA_(public) //LEGACY-SUPPORT
	/**LEGACY
	 * Writes @a src to @ dst. @a src is untagged, opaque memory (not type-checked.)
	 * @return Returns a client defined @c DAE_ERROR @c enum. @a dst is 0 terminated.
	 */
	inline daeOK memoryToString(const daeOpaque src, daeArray<> &dst)
	{
		dst.clear(); return serialize(src,dst);
	}

	COLLADA_NOINLINE
	/**LEGACY
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param srcEnd The CP after the last CP to be written.
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */	
	inline daeOK stringToMemory(daeString srcBegin, daeString srcEnd, daeOpaque dst)
	{
		return unserialize(srcBegin,srcEnd-srcBegin,dst);
	}	
	/**OVERLOAD
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)
	 * @param srcLen Length of the @a src string. 
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */
	inline daeOK stringToMemory(daeString string, size_t extent, daeOpaque dst)
	{
		return unserialize(string,extent,dst);
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
		return unserialize(src,srcLen,dst); 
	}
	/**OVERLOAD, NEW/MAY HAVE ISSUES
	 * Writes @a src to @ dst. @a dst is untagged, opaque memory (not type-checked.)	 
	 * @return Returns a client defined @c DAE_ERROR @c enum.
	 */
	inline daeOK stringToMemory(const daeHashString &src, daeOpaque dst, /*template-parity*/...)
	{
		return unserialize(src.string,src.extent,dst); 
	}
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
	daeOStringBuf&operator=(const daeOStringBuf&);

	daeArray<> &obuf; char buf[buffer_size+1];

public:

	daeOStringBuf(daeArray<> &obuf):obuf(obuf)
	{
		setp(buf,buf+buffer_size);
	}	
	//Necessary on windows. A commentator think so. It is.
	//http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
	~daeOStringBuf()
	COLLADA_NOEXCEPT //MSVC2013 C2694
	{ sync(); }

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

template<class This>
/**EXPERIMENTAL
 * Trying to develop a transparent typewriter without an RTTI component.
 */
struct daeAtomWriter_support : daeOpaqueWriter
{		
	//C++ is goofy sometimes.	
	#define T typename This::type
	//GCC won't work with ::.
	//#define Typist ((This*)this)->typist
	//NOTE: GCC won't work with "This &_this()" either.
	#define	Typist static_cast<typename This::typist*>((This*)this)
							
	virtual void copy(const daeOpaque src, daeOpaque dst)
	{
		Typist->assign(*(T*)&dst,*(const T*)&src);
	}

	virtual int compare(const daeOpaque a, const daeOpaque b)
	{
		return Typist->collate(*(const T*)&a,*(const T*)&b);
	}

	virtual daeOK serialize(const daeOpaque src, daeArray<> &dstIn)	
	{
		daeOStringBuf<This::typist::buffer_size> buf(dstIn); std::ostream dst(&buf);
		Typist->template formatXML<T>(dst,~dst.skipws);
		daeOK OK = Typist->decodeXML(dst,*(const T*)&src);
		if(OK==DAE_OK&&dst.fail()) OK = DAE_ERROR; return OK;
	}

	virtual daeOK unserialize(const daeStringCP *srcIn, size_t srcN, daeOpaque dst)
	{
		//NEW: Adding appending model/srcN.
		if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
		return Typist->appendXML(*(T*)&dst,srcIn,~srcN);
		daeString srcEnd = srcIn+srcN;

		Typist->normalize(srcIn,srcEnd);
		daeIStringBuf buf(srcIn,srcEnd);
		std::istream src(&buf);
		Typist->template formatXML<T>(src,~src.skipws);
		daeOK OK = Typist->encodeXML(src,*(T*)&dst);
		if(OK==DAE_OK&&(!src.eof()||src.fail())) OK = DAE_ERROR; return OK;
	}
};

template<class This>
/**EXPERIMENTAL							   
 * Trying to develop a transparent typewriter without an RTTI component.
 */
struct daeArrayWriter_support : daeOpaqueWriter
{
	virtual void copy(const daeOpaque src, daeOpaque dst)
	{
		Typist->assign(*(daeArray<T>*)&dst,*(const daeArray<T>*)&src);
	}

	//Pre-2.5 this had first compared the sizes, and passed if unequal.
	//The original comments don't comment on the purpose/role of compare().
	virtual int compare(const daeOpaque aIn, const daeOpaque bIn)
	{
		const daeArray<T> &a = aIn; const daeArray<T> &b = bIn;

		//This ALGORITHM is identical to daeTypist<void> except for
		//Typist::collate() has replaced daeTypist<void>::collate().
		int i,cmp,iN; for(i=0,iN=(int)std::min(a.size(),b.size());i<iN;i++) 
		{
			cmp = Typist->collate(a[i],b[i]); if(cmp!=0) return cmp;
		}
		return (int)a.size()-(int)b.size();
	}

	virtual daeOK serialize(const daeOpaque srcIn, daeArray<> &dstIn)	
	{
		const daeArray<T> &src = srcIn; 
		daeOStringBuf<4096> buf(dstIn); std::ostream dst(&buf);
		daeOK OK; 
		size_t i = 0, iN = src.size();
		Typist->template formatXML<T>(dst,~0);
		if(iN!=0) for(;;)
		{
			OK = Typist->decodeXML(dst,src[i]);
			if(OK==DAE_OK&&!dst.good()) OK = DAE_ERROR;
			if(++i<iN&&OK==DAE_OK) dst << ' ';
			else break;
		}			
		return OK;
	}
		
	virtual daeOK unserialize(const daeStringCP *srcIn, size_t srcN, daeOpaque dstIn)
	{	
		daeArray<T> &dst = dstIn;	

		//NEW: Promise not appended in err.
		size_t rollback;

		//NEW: Adding appending model/srcN.
		if(0!=srcN>>sizeof(srcN)*CHAR_BIT-1)
		{
			srcN = ~srcN;			
			if(srcN!=0){ rollback = dst.size(); goto append; }
		}
		else dst.clear(); rollback = 0; append:
		daeString srcEnd = srcIn+srcN;

		Typist->normalize(srcIn,srcEnd);
		daeIStringBuf buf(srcIn,srcEnd); std::istream src(&buf);
		Typist->template formatXML<T>(src,~0);
		//Passing tmp as pointer to make this MSVC warning shutup.
		//"warning C4700: uninitialized local variable 'tmp' used."
		//COLLADA_SUPPRESS_C(4700) //no effect???
		//-Wmaybe-uninitialized is here in -On modes. GCC wants a full push/pop song and dance????
		daeOK OK; T tmp; Typist->supercharge(&dst,&tmp);
		for(;!src.eof();)
		{
			OK = Typist->encodeXML(src,tmp);
			if(OK!=DAE_OK) goto rollback;
			if(src.fail())
			{
				OK = DAE_ERROR; goto rollback;
			}
			dst.push_back(tmp);
		}
		return DAE_OK;
		rollback: dst.setCountLess(rollback); return OK;
	}

	#undef Typist
	#undef T
};
   
template<class T, class Typist=daeTypist<T>>
/**EXPERIMENTAL
 * Trying to develop a transparent typewriter without an RTTI component.
 */
struct daeAtomWriter 
:
daeAtomWriter_support<daeAtomWriter<T,Typist>>, Typist
{	
	typedef T type; typedef Typist typist;

	enum{ atomic_type=Typist::system_type|Typist::binary_type };
};
template<class T, class Typist=daeTypist<T>>
/**EXPERIMENTAL
 * Trying to develop a transparent typewriter without an RTTI component.
 */
struct daeArrayWriter 
:
daeArrayWriter_support<daeArrayWriter<T,Typist>>, Typist
{
	typedef T type; typedef Typist typist;

	//NOTE: Not same as getAtomicType().
	//enum{ atomic_type=daeAtomicType::EXTENSION };
	enum{ atomic_type=Typist::system_type|Typist::binary_type };
};

/**NEVER-CONST
 * Formerly @c daeTypewriter.
 * Trying to develop a transparent typewriter without an RTTI component.
 */
class daeTypeInfo
{
	friend class daeTypewriter;
	friend class daeAnySimpleType;
	friend class daeAnySimpleTypewriter;
	template<class,class> friend class daeType;	

COLLADA_(private) 
	
	enum{ _versions=1 };
	unsigned int _version:4,_list:1;
	int _atomType;
	daeSize _sizeof;
	daeString _typeID;
	/**
	 * @note @c this is one of these unless it is xs:anySimpleType
	 * based, in which case there are 3 @c daeTypewriter available
	 * and @c unserialize() expects @c daeAnySimpleType based data.
	 */
	daeTypewriter *_atomWriter,*_arrayWriter;
	
	/**REFACTOR @c daeType checks this relationship. */
	static daeTypeInfo *_ti(daeTypewriter *p)
	{
		return (daeTypeInfo*)(((daeOpaqueWriter*)p)+1); //INCOMPLETE
	}
	/**REFACTOR @c daeType checks this relationship. */
	static daeTypewriter *_tw(daeTypeInfo *p)
	{
		return (daeTypewriter*)(((daeOpaqueWriter*)p)-1); //INCOMPLETE
	}

	COLLADA_NOALIAS
	#ifdef NDEBUG
	#error Replace daeStringRef with something nonprovisional.
	#endif		
	//NOT THREAD-SAFE
	COLLADA_DOM_LINKAGE static daeString _ID(daeString id)
	COLLADA_DOM_SNIPPET( return daeStringRef(id); )

COLLADA_(private) 
	
	template<class T>
	/**
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypeInfo(T*,daeTypist<T>*):_list()
	{
		_init(typeid(T),sizeof(T));
	}
	template<class T>
	/**
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypeInfo(daeArray<T>*,daeTypist<T>*):_list(1)
	{
		_init(typeid(daeArray<T>),sizeof(daeArray<T>));
	}
	template<class T>
	/**HYPOTHETICAL?
	 * @c typeid can be deduced from the typist in this case.
	 */
	daeTypeInfo(daeArray<T>*,daeTypist<daeArray<T>>*):_list(1)
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
	daeTypeInfo(T*,...):_list()
	{
		_init(typeid(void),sizeof(T)); _typeID = nullptr;
	}	
	/**WORKAROUND
	 * @c daeAtomicType::VOID Constructor
	 */
	daeTypeInfo():_list(),_atomType(daeAtom::VOID)
	{
		_init(typeid(void),0); 
		
		_atomWriter = _arrayWriter = _tw(this);
	}
	void _init(const std::type_info &ti, size_t so)
	{
		_version = _versions; _sizeof = so; _typeID = _ID(ti.name());
	}

COLLADA_(public)	
	/**
	 * Gets the string associated with this type.
	 * The pointer will be unique for a given string.
	 * @c type_info::name must be the same process-wide.
	 * @return Returns the string associated with this type.
	 */
	inline daeString getTypeID(){ return _typeID; }	

	/**
	* Manually sets the string returned by @c getTypeID() for both
	* of the lined atom & array typewriters, if not previously set.
	* @note @c XS::Enumeration needs this to assign the @c enum ID.
	*/
	template<class T> inline void setTypeID(bool list=false)
	{
		if(_typeID==nullptr) 
		{
			_ti(_arrayWriter)->_list = list?1:0;
			_ti(_atomWriter)->_typeID = _ID(typeid(T).name());
			_ti(_arrayWriter)->_list = 1;
			_ti(_arrayWriter)->_typeID = _ID(typeid(daeArray<T>).name());
		}
		else assert(0);
	}

	/**STATIC UTILITY
	* Gets a type ID string for direct pointer-to-pointer comparisons
	* with @c getTypeID(). 
	* @note String based comparison is unnecessary and impractical as
	* the values of the strings are not "portable" and so must be got
	* from @c ID(). IOW: They are not known until each/every run-time.
	*/
	template<class T> static daeString ID(){ return _ID(typeid(T).name()); }

COLLADA_(public) //daeTyperwriter accessors	  
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
	 * that doing @c this==&_arrayWriter would fail for
	 * xs:anySimpleType types, even in the xs:list scenario.
	 */
	inline bool hasListType()
	{
		//I think this would work, but it is a far deeper
		//comparison than this==_arrayWriter and would
		//not work for the 2D array case. (The array case
		//might not have real world implications.)
		//return _typeID==_arrayWriter->typeID; 
		return 1==_list; 
	}

	/**RECOMMENDED
	 * Helps to test for both "TOKEN" and "STRING" types.
	 * It's desirable to remove TOKEN, so if that occurs
	 * code that uses this method will be left unchanged.
	 */
	inline bool hasStringType()
	{
		//Note: >=daeAtom::STRING would also work.
		return _atomType>daeAtom::_STRINGS_BEGIN_;
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
	 * @c _atomWriter->_atomType. While nice in theory, it's
	 * surprisingly easy to write code that corrupts array values.
	 * @see @c where<daeAtom>().
	 */
	int getAtomicType(){ return /*_atomWriter->*/_atomType; }

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
	bool hasAtomicType(){ return daeAtom::EXTENSION!=_atomType; }	

COLLADA_(public) //daeAnySimpleType::TypedUnion support
	/**
	 * Tells if the type is "plain-old-data" not needing
	 * memory management.
	 */
	inline bool isPOD(){ return _atomType<daeAtom::VOID; }

	//NOTE: Mainly daeAST::TypedUnion::exotic() puts this to use.
	/**
	 * Tells if the type is an @c int that transcends @c INT_MAX.
	 * Namely @c unsigned and @c long int types are not selected
	 * unless a value will not fit into @c int.
	 */
	inline bool isExotic_int()
	{		
		//TODO: Thinking a COLLADA_DOM_EXOTIC_INT might be of
		//use to change daeAST::TypedUnion::exotic's behavior
		//to better fit 64-bit architectures in 64-bit future.
		//Except that even on Windows, a "long int" is 32-bit.
		return hasKeyword_int()&&_atomType!=-int(sizeof(int));
	}

	/**
	 * Tells if the type uses the @c unsigned keyword.
	 * If so it is an @c int based type.
	 * @see @c hasKeyword_signed()
	 */
	inline bool hasKeyword_unsigned()
	{		
		return (unsigned)_atomType<=sizeof(long long int);
	}
	/**
	 * Tells if the type uses the @c signed keyword.
	 * If so it is an @c int based type.
	 * @see @c hasKeyword_unsigned()
	 */
	inline bool hasKeyword_signed()
	{		
		return (unsigned)_atomType>(unsigned)-int(sizeof(long long int));
	}
	/**
	 * Tells if the type uses the @c int keyword.
	 * If so it is an @c int based type.
	 */
	inline bool hasKeyword_int(){ return _atomType<=(int)sizeof(long long int); }
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
class daeTypewriter : public daeOpaqueWriter, public daeTypeInfo
{	
COLLADA_(public) //daeOpaqueWriter extensions

	/**EXPERIMENTAL/QUESTIONABLE
	 * Assists when appending unseparated text by providing the
	 * back element of a list when appropriate while defaulting 
	 * to the data itself when not. 
	 *
	 * Don't use a small amount of text to intialize values for
	 * appending, since @c daeAST::TypedUnion cannot change its
	 * type, and so may not have enough data to correctly guess
	 * the type. It can only append to strings and @c daeBinary
	 * via @c appendToBack(). If the appended data would change
	 * the type, an error is emitted, which code must deal with.
	 *
	 * @warning Strings demand very special whitespace handling.
	 * If not observed documents will be spoiled. The following
	 * API ("see") Doxygen comments describe the issue in depth.
	 * @see @c daeOpaqueWriter::append()
	 */
	inline daeOK appendToBack(daeString string, size_t extent, daeOpaque dst)
	{
		daeTypewriter *tw; if(hasListType())
		{
			daeAlloc<>* &au = dst;
			size_t i = au->getInternalCounter();
			if(0==i) goto list;
			dst = au->getRaw(i-1); goto atom;
		}	
		else if(daeAtom::VOID!=_atomType)
		{
			//NOTE: VOID is protecting daeAnySimpleType::TypedUnion.
			atom: tw = _atomWriter;
		}
		else list: tw = this; return tw->append(string,extent,dst);
	}

COLLADA_(public) //OPERATOR
	/**OPERATOR
	 * Converts to pointer to ease comparison to @c where<daeArray>().
	 */
	inline operator daeTypewriter*(){ return this; }

COLLADA_(public) //ACCESSORS
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
	inline bool isAnySimpleType()
	{
		return _atomWriter!=this&&this!=_arrayWriter; 
	}

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
inline daeTypewriter &daeTypewriter::where<daeAtom>()
{
	return *_atomWriter; 
}
template<> 
/**CUTE, EXPLICIT-SPECIALIZATOIN
 * Gets a @c daeTypewriter appropriate for @c daeArray containers.
 * @tparam class must be @c daeArray.
 * GCC/C++ require this to be defined outside of @c daeTypewriter.
 */
inline daeTypewriter &daeTypewriter::where<daeArray>()
{
	return *_arrayWriter; 
}	
  
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

template<class T, class Typist=daeTypist<T>>
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
COLLADA_(public) 
	
	struct AtomWriter 
	: 
	daeAtomWriter_support<AtomWriter>, daeTypeInfo, Typist
	{
		typedef T type; typedef Typist typist;
		
		template<class A>
		AtomWriter(const A &a)
		:daeTypeInfo((T*)0,(Typist*)0),Typist(a){}
		template<class A, class B>
		AtomWriter(const A &a, const B &b)
		:daeTypeInfo((T*)0,(Typist*)0),Typist(a,b){}
		AtomWriter():daeTypeInfo((T*)0,(Typist*)0){}
	};

	struct ArrayWriter 
	:
	daeArrayWriter_support<ArrayWriter>, daeTypeInfo, Typist
	{	
		typedef T type; typedef Typist typist;
		
		template<class A>
		ArrayWriter(const A &a)
		:daeTypeInfo((daeArray<T>*)0,(Typist*)0),Typist(a){}
		template<class A, class B>
		ArrayWriter(const A &a, const B &b)
		:daeTypeInfo((daeArray<T>*)0,(Typist*)0),Typist(a,b){}
		ArrayWriter():daeTypeInfo((daeArray<T>*)0,(Typist*)0){}
	};

COLLADA_(protected) //DATA MEMBERS

	/**
	 * These are @c daeTypewriter compatible by virture of being
	 * a @c daeOpaqueWriter followed by @c daeTypeInfo in memory.
	 */
	AtomWriter _atomWriter; ArrayWriter _arrayWriter;

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
		//daeCTC<!daeAtomicType::EXTENSION>(); 
		daeCTC<!Typist::binary_type||!Typist::system_type>();		
		int at = Typist::system_type|Typist::binary_type;
		_atomWriter._atomType = at!=0?at:daeAtom::EXTENSION;
		_arrayWriter._atomType = daeAtom::EXTENSION;
		_atomWriter._atomWriter = (daeTypewriter*)&_atomWriter;
		_atomWriter._arrayWriter = (daeTypewriter*)&_arrayWriter;
		_arrayWriter._atomWriter = (daeTypewriter*)&_atomWriter;
		_arrayWriter._arrayWriter = (daeTypewriter*)&_arrayWriter;

		//YUCK: Confirm daeTypeInfo is where it's expected to be.
		void *check = static_cast<daeTypeInfo*>(&_atomWriter);
		assert(((daeOpaqueWriter*)check)-1==&_atomWriter);
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
	daeType(const A &a):_atomWriter(a),_arrayWriter(a)
	{
		_daeType_init(); 
	}
	template<class A, class B>
	/**
	 * Constructor forwarding one or two arguments to @a Typist.
	 * @a Typist must fill in @a B with @c ... if it's not used.
	 */
	daeType(const A &a, const B &b=B()):_atomWriter(a,b),_arrayWriter(a,b)
	{
		_daeType_init(); 
	}

COLLADA_(public) //OPERATORS
	/**
	 * Converts to atomic-typewriter.
	 */
	inline operator daeTypewriter*(){ return (daeTypewriter*)&_atomWriter; }
	/**
	 * Converts to atomic-typewriter.
	 */
	inline operator daeTypewriter&(){ return *(daeTypewriter*)&_atomWriter; }
};

template<class>
/**NEVER-CONST
 * This class is specialized to implement @c daeType.
 * This is a default/starter implementation which is
 * typically used as a base class by specializations.
 */
class daeTypist
{
COLLADA_(public) //PRIMARY FUNCTIONS

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

	template<class T>
	static daeError appendXML(T&,daeString,size_t)
	{
		return DAE_ERR_NOT_IMPLEMENTED;
	}

	//HMMM: Maybe could be src,dst like daeOpaqueWriter::copy?
	template<class T> static void assign(T &dst, const T &src)
	{
		dst = src;
	}
	template<class T> static daeError encodeXML(std::istream &src, T &dst)
	{
		src >> dst; return DAE_OK;
	}
	template<class T> static daeError decodeXML(std::ostream &dst, const T &src)
	{
		dst << src; return DAE_OK;
	}
	//GCC can't do this. Just checking on Win32.
	//Maybe they can just be overloaded instead?
	//This is ensuring daeOpaque isn't assigned.
	#ifdef _MSC_VER
	static int collate(const daeOpaque&, const daeOpaque&)
	{
		daeCTC<0>(); return 0;
	}
	template<> static void assign(daeOpaque&,const daeOpaque&)
	{
		daeCTC<0>();
	}
	template<> static daeError encodeXML(std::istream &src, daeOpaque &dst)
	{
		daeCTC<0>();
	}
	template<> static daeError decodeXML(std::ostream &dst, const daeOpaque &src)
	{
		daeCTC<0>();
	}
	#endif

	//NOTE: This is "compare" but with a different name so that
	//it's not ambiguous multi-inheritance with daeOpaqueWriter.
	template<class T> static int collate(const T &a, const T &b)
	{
		return a<b?-1:a==b?0:+1;
	}
	template<class T, int M, int N>
	//Muti-dimensional array compare support. e.g. list_of_hex_binary.
	static int collate(const daeArray<T,M> &a, const daeArray<T,N> &b)
	{
		//This ALGORITHM is identical to Typewriter<daeArray>
		//except for collate() has replaced Typist::collate().
		int i,cmp,iN; for(i=0,iN=std::min(a.size(),b.size());i<iN;i++)
		{
			cmp = collate(a[i],b[i]); if(cmp!=0) return cmp;
		}
		return (ptrdiff_t)a.size()-(ptrdiff_t)b.size(); daeCTC<M==N>();
	}

COLLADA_(public) //SHAPING FUNCTIONS

	//GCC/C++ won't tolerate explicit specialization in classes.
	template<class T> static void formatXML(std::ostream &dst, int fmtflags)
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
	template<class T> static void formatXML(std::istream &src, int fmtflags)
	{
		_formatXML((T*)0,&src,fmtflags); //Quick Fix for GCC/C++.
	}
	static void _formatXML(daeStringRef*, std::istream *src, int fmtflags)
	{
		if(0==(fmtflags&src->skipws)) *src >> std::noskipws;
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
		new(C4700) daeStringRef(dst->_getObject());
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

	#ifdef NDEBUG
	#error Implement (unsigned&) for 1/2/8 byte int types.
	#endif
	//TODO? This is just handling 'int' but really it should be
	//done for the other types. Like maybe J should be unsigned.
	/**
	 * This is added to use an @c unsigned type so the default
	 * value of <stencil_mask> doesn't overflow (it ought to be
	 * xs:unsignedLong instead of xs:long.)
	 */
	static daeError encodeXML(std::istream &src, I &dst)
	{
		//NOTE: daeAnySimpleTypewriter uses an alternative path.
		if(system_type<0&&sizeof(int)==sizeof(I))
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

	static daeError decodeXML(std::ostream &dst, const I &src)
	{
		int proxy = src; return daeTypist<>::decodeXML(dst,proxy);
	}
	static daeError encodeXML(std::istream &src, I &dst)
	{
		int proxy; daeError err = daeTypist<>::encodeXML(src,proxy); 
		if(!src.fail()) dst = (I)proxy; return err;
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

	static daeError decodeXML(std::ostream &dst, const FP &src)
	{
		//A GOOD IS-FINITE TEST HERE COULD WINNOW ALL THREE.
		if(src!=src) dst << "NaN";
		else if((I&)src==INF) dst << "INF"; //formatXML() sets up formatting.
		else if((I&)src==_INF) dst << "-INF"; else dst << src; return DAE_OK;
	}
	static daeError encodeXML(std::istream &src, FP &dst)
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
	enum{ system_type=daeAtom::STRING, buffer_size=128 };
	
	static int collate(const daeStringRef &a, const daeStringRef &b)
	{
		return strcmp(a,b);
	}
	
	static daeError decodeXML(std::ostream &dst, const daeStringRef &src)
	{
		dst << (daeString)src; return DAE_OK;
	}
						 
	static daeError encodeXML(std::istream &src, daeStringRef &dst)
	{
		//MUST IMPLEMENT <xs:list itemType="xs:string">??
		//DAEP::Container<daeString>::type makes this unlikely.
		assert(0==(src.flags()&src.skipws)); //return DAE_ERROR;
		assert(dynamic_cast<daeIStringBuf*>(src.rdbuf())!=nullptr);
		daeIStringBuf *dae = (daeIStringBuf*)src.rdbuf();
		COLLADA_SUPPRESS_C(4244)
		dst.set(dae->begin,dae->end-dae->current); 		
		src.setstate(src.eofbit); return DAE_OK;
	}

	static daeError appendXML(daeStringRef &dst, daeString src, size_t len)
	{
		dst.append(src,len); return DAE_OK; 
	}
};
/**LEGACY, WISE?
 * @see @c daeAtom::TOKEN concerns.
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
		
	/**C++
	 * Default Constructor
	 */
	daeTokenRef(){}

	template<class Type> //DAEP::Value<...>
	/**
	 * Prototype Constructor
	 *
	 * This constructor transfers the schema's default strings
	 * to the database string-ref pool. The empty-string cases
	 * go along an @c inline pathway. Default-strings go along
	 * a @c COLLADA_DOM_LINKAGE path.
	 */
	explicit daeTokenRef(const Type &pt):daeStringRef(pt)
	{}

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
	enum{ system_type=daeAtom::TOKEN };

	static daeError appendXML(daeStringRef &dst, daeString src, size_t len)
	{
		//This is described in the append() Doxygen comment.
		//Without this compromise appending is not possible.
		if(dst.empty())
		while(len!=0&&COLLADA_isspace(src[0])){ src++; len--; }		
		while(len!=0&&COLLADA_isspace(src[len-1])){ len--; }		

		dst.append(src,len); return DAE_OK; 
	}

	static daeError encodeXML(std::istream &src, daeTokenRef &dst)
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
	static daeError decodeXML(std::ostream &dst, const daeTokenRef &src)
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

/**
 * @c daeBinary needs this data to be up front so its array is
 * variable-length. This structure needs to be 8 bytes so that
 * nonzero @c size_on_stack arrays position the AU pointers at
 * the same address.
 */
class daeBinary_base
{ 	
COLLADA_(protected)

	//TODO: Let daeOpaqueWriter::append overflow.
	#ifdef NDEBUG
	#error This structure should buffer overflow.
	#endif

	//BITFIELD for systems that have 8B int.
	//REMINDER: Don't change this to "short" since there
	//must be 8 bytes to __daeBinary__static in total...
	int _base:32; 		
	//HACK: daeBinary needs this structure to be 8 bytes
	//to make daeBinary<N,0> and daeBinary<N,!0> line up.
	//NOTE: "terminate_bits" is something else, and not padding.
	//#if 8!=CHAR_BIT
	unsigned int _surplus:8;
	//#endif

	/**
	 * Trying to move methods out of the templatein case that
	 * that helps with compile times.
	 */
	daeArray<char> &_array()const{ return *(daeArray<char>*)(this+1); }

COLLADA_(public) //OPERATORS

	template<int N>
	/** @c base() is just keep tracking the XML Schema type. */
	inline operator daeBinary<N>&(){ return *(daeBinary<N>*)this; }
	template<int N>
	/** @c base() is just keep tracking the XML Schema type. */
	inline operator const daeBinary<N>&()const{ return *(daeBinary<N>*)this; }

	/**WARNING 
	 * Assignment Operator
	 * @warning @c base() is not copied, as it's part of the type.
	 */
	daeBinary_base &operator=(const daeBinary_base &cp)
	{
		_array() = cp._array(); 
		terminate_bits(cp.terminate_bits()); return *this;
	}

COLLADA_(public) //Binary accessors & mutators
	/**
	 * Empties the @c char array and sets @c terminate_bits(0).
	 */
	void clear(){ _array().clear(); terminate_bits(0); }

	//NOTE: There's not currently a means to change the "base".
	//You can alias the memory if you absolutey have to rebase.
	/**
	 * Gets base used to construct *this.
	 * @return Returns 16 for hexBinary or 64 for base64Binary.
	 * These are XML Schema types. Other vales may be returned.
	 */
	inline int base()const{ return _base; }

	template<int N>
	/**
	 * Casts to base @c N @c daeBinary reference. @c base() is
	 * unaltered.
	 */
	inline daeBinary<N> &base(){ return *this; }
	template<int N>
	/**CONST-FORM
	 * Casts to base @c N @c daeBinary reference. @c base() is
	 * unaltered.
	 */
	inline const daeBinary<N> &base()const{ return *this; }	
		
	/**
	 * @return Returns the number of unused bits on the end of
	 * the binary-blob. Users must set it with @c terminate_bits().
	 */
	inline unsigned char terminate_bits()const
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
	void terminate_bits(unsigned char bits)
	{
		#if 8!=CHAR_BIT
		assert(bits<CHAR_BIT);
		_surplus = bits;
		#else
		assert(bits==0);
		#endif
	}

COLLADA_(public) //String serialization APIs	
 
	//NOTE: These are called assign_base to not collide with
	//daeArray<char>::assign.

	template<int N>
	/**
	 * String input.
	 * @note @c assign() assigns plain memory.
	 * @tparam N if 0 uses the @c base() of the contructor. 
	 */
	daeOK assign_base(const daeHashString &cp)
	{
		clear(); return append_base<N>(cp);
	}
	template<int N>
	/**
	 * String append.
	 * @note @c append() appends plain memory.
	 * @tparam N if 0 uses the @c base() of the contructor. 
	 */
	daeOK append_base(const daeHashString &cp)
	{
		daeIStringBuf buf(cp.string,cp.string+cp.extent);
		std::istream src(&buf);		
		unserialize<N>(src); return src.fail()?DAE_ERROR:DAE_OK;
	}

COLLADA_(public) //Low-level serialization APIs

	//This is implemented as a switch since GCC/C++ can't
	//explicitly specialize class methods. It matters not.
	template<int N>
	/**
	 * @tparam N must be 0, 16, or 64, where 0 is @c base().
	 */
	inline void serialize(std::ostream &dst)const
	{
		switch(N!=0?N:base())
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
		const char *p = _array().data();
		const char *d = p+_array().size();
		if(0!=terminate_bits()) d--; 
		signed char i,j,c; 
		for(;p<d;p++) 
		for(i=0;i<CHAR_BIT;i+=8) //This is academic.
		for(j=i+4,c=*p;j>=i;j-=4) terminate_bits:
		{
			//Can Endianness be related to CHAR_BIT?
			char nibble = c>>j&0xF;
			dst.put((nibble>=10?'a'-10:'0')+nibble); 
		}
		if(0<terminate_bits()&&p==d) //This is academic.
		{
			i = terminate_bits();
			j = i+4; c = *++p>>i;
			if(i%8==0) goto terminate_bits;
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
		if(!_array().empty())
		{
			dst.setstate(dst.failbit); assert(0);
		}
	}

	//This is implemented as a switch since GCC/C++ can't
	//explicitly specialize class methods. It matters not.
	template<int N>
	/**WARNING
	 * @warning @c clear() is not called.
	 * @tparam N must be 0, 16, or 64, where 0 is @c base().
	 *
	 * Extracts @c base() char-data from an @c std::istream.
	 */
	inline void unserialize(std::istream &src)
	{
		switch(base())
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
		//TODO? Could use dae_clear but std::istream
		//is not exactly user-friendly to begin with.
		//clear(); 

		//NEW: Promise not appended in err.
		size_t rollback = _array().size();

		std::istream::sentry s(src); //Skip whitespace.
		if(!s) return;		
		signed char i,j,nibble,value=0; 
		for(;;_array().push_back(value),value=0)
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
						terminate_bits(CHAR_BIT-j-4); 
						_array().push_back(value);
					}
					else 
					{
						//And what about _failure_to_unserialize?
						//These could become overwhelming. (Could store a flag?)
						//daeEH::Error<<"hexBinary had odd character?";

						fail: src.setstate(src.failbit); 

						_array().setCountLess(rollback); //NEW
					}
					return;
				}
				else nibble = 10+nibble-'a';
			}//Can Endianness be related to CHAR_BIT?
			else nibble-='0'; value+=nibble<<j;
		}		
		assert(0); //UNREACHABLE?
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
};
template<int M, int size_on_stack> //These defaults are 0,0.
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
class daeBinary : public daeBinary_base, public daeArray<char,size_on_stack>
{
	inline void _init()
	{
		//An enum classification of acceptible bases is in order.
		_base = M; daeCTC<M==16||M==64>(); terminate_bits(0);
		//Something is off??? __daeBinary__static::_surplus props this up.
		assert(daeOffsetOf(daeBinary,getAU())==daeOffsetOf(daeBinary<M>,getAU()));
		assert(daeOffsetOf(daeBinary,getAU())==sizeof(daeBinary_base));
	}

COLLADA_(public) //CONSTRUCTORS	
	/**
	 * Default Constructor
	 */
	daeBinary(){ _init(); }
	/**WARNING 
	 * Copy Constructor
	 * @warning @c base() is not copied, as it's part of the type.
	 */
	daeBinary(const daeBinary &cp):daeBinary_base(cp){ _base = M; }
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
	/**WARNING 
	 * Assignment Operator
	 * @warning @c base() is not copied, as it's part of the type.
	 */
	daeBinary &operator=(const daeBinary &cp)
	{
		daeBinary_base::operator=(cp); return *this; //C++
	}

COLLADA_(public) //Binary accessors & mutators

	using daeBinary_base::clear;

COLLADA_(public) //Low-level serialization APIs
	/**
	 * Writes @c base() binary char-data to @c std::ostream.
	 */
	inline void serialize(std::ostream &dst)const
	{
		daeBinary_base::serialize<M>(dst);
	}

	/**WARNING
	 * @warning @c clear() is not called.
	 * Extracts @c base() char-data from an @c std::istream.
	 */
	inline void unserialize(std::istream &src)const
	{
		daeBinary_base::unserialize<M>(src);
	}

COLLADA_(public) //daeArray traits

	typedef void __COLLADA__atomize;
	//daeArray has helpers that make this vastly simpler than it would otherwise be.
	static void __COLLADA__move(const daeObject *obj, daeBinary *lv, daeBinary *rv, size_t iN)
	{
		daeArray<char,size_on_stack>::_derive__COLLADA__move(obj,lv,rv,iN,_derive__COLLADA__mover);
	}		
	static void _derive__COLLADA__mover(daeBinary &lv, const daeBinary &rv)
	{
		lv._base = rv._base; lv.terminate_bits(rv.terminate_bits());
	}
	inline void __COLLADA__locate(const daeObject *obj, const daeBinary &cp)
	{
		daeArray<char,size_on_stack>::__COLLADA__locate(obj,cp); _derive__COLLADA__mover(*this,cp);
	}

COLLADA_(public) //daeTypewriter support
	/** 
	 * Inputs a string.
	 * The base is tied to @c base(). It's safe to cast to a different base. 
	 */
	friend std::istream &operator>>(std::istream &i, daeBinary &dst)
	{
		dst.clear(); dst.daeBinary_base::unserialize<M>(i); return i;
	}

	/** 
	 * Outputs a string.
	 * The base is tied to @c base(). It's safe to cast to a different base. 
	 */
	friend std::ostream &operator<<(std::ostream &o, const daeBinary &src)
	{
		src.daeBinary_base::serialize<M>(o); return o;
	}
};
template<int M, int N> struct daeTypist<daeBinary<M,N>> : daeTypist<>
{	
	enum{ binary_type=daeAtom::BINARY, buffer_size=4096 };

	inline int collate(const daeBinary<> &a, const daeBinary<> &b)
	{
		int cmp = memcmp(a.data(),b.data(),std::min(a.size(),b.size()));
		return cmp!=0?cmp:(int)a.size()-(int)b.size();
	}
	 	
	static daeError appendXML(daeBinary<M,N> &dst, daeString src, size_t len)
	{
		return dst.template append_base<M>(daeHashString(src,len)).error;
	}
};
//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeBinary<16/*,N*/>> : daeTypist<daeBinary<>> //xs:hexBinary
{	
	enum{ binary_type,system_type=daeAtom::BINARY };
};
//REMINDER: system_type must be visible to XS::Schema::_typewrit().
template<> struct daeTypist<daeBinary<64/*,N*/>> : daeTypist<daeBinary<>> //xs:base64Binary
{	
	enum{ binary_type,system_type=daeAtom::BINARY };
};

/**EXPERIMENTAL
 * Implements XML Schema type that holds "0" and "1", or
 * "true" and "false".
 * Presentation mode is found with @c 1==size().
 */
class daeBoolean : public daeString_support<daeBoolean>
{
	unsigned char _storage;

COLLADA_(public) //daeArray traits / construction disposition
	/**LOW-LEVEL
	 * Valid @c _storage is 0, 1, 2, 5.
	 * 2 is @c false_token; 5 is @c true_token.
	 * 4 is @c e; reserved for error state.
	 */
	enum{ false_token=2,e=4,true_token=5 };

	template<class T>
	/**
	 * Copy Constructor
	 * Default to wordy format when constructed outside of an array.
	 */
	daeBoolean(const T &cp):_storage((bool)cp?true_token:false_token){}
	/**
	 * Default Constructor
	 * Default to wordy format when constructed outside of an array.
	 */
	daeBoolean(int s=false_token):_storage(0x7&s){}
	/**
	 * Copy Constructor
	 * Copy presentation.
	 */
	daeBoolean(const daeBoolean &cp):_storage(cp._storage){}

	typedef void __COLLADA__atomize,__COLLADA__Object;
	template<class T>
	/**HACK
	 * Default to compact format when constructed inside of an array.
	 */
	inline void __COLLADA__place(void*, const T &cp)
	{
		_storage = 0; *this = cp;
	}
	/**HACK
	 * Use of @c __COLLADA__place() requires this to be implemented.
	 */
	static void __COLLADA__move(void*, daeBoolean *lv, daeBoolean *rv, size_t iN)
	{
		memcpy(lv,rv,iN*sizeof(daeBoolean)); //Whatever.
	}	

COLLADA_(public) //OPERATORS
	/**
	 * Converts to @c bool.
	 */
	inline operator bool()const{ return (_storage&1)!=0; }

	//Resolve daeString_support amiguity.
	inline bool operator==(bool cmp)const
	{
		return (bool)*this==cmp; 
	}
	//Resolve daeString_support amiguity.
	inline bool operator!=(bool cmp)const
	{
		return (bool)*this!=cmp; 
	}	
	//Resolve daeString_support amiguity.
	inline bool operator==(const daeBoolean &cmp)const
	{
		return (bool)*this==(bool)cmp;
	}
	//Resolve daeString_support amiguity.
	inline bool operator!=(const daeBoolean &cmp)const
	{
		return (bool)*this==(bool)cmp;
	}
	//Resolve daeString_support amiguity.
	friend bool operator==(bool a, const daeString_support &b)
	{
		return a==b._this().operator bool();
	}
	//Resolve daeString_support amiguity.
	friend bool operator!=(bool a, const daeString_support &b)
	{
		return a!=b._this().operator bool();
	}

	template<class T>
	/**
	 * Assignment
	 * Preserving presentation.
	 */
	inline daeBoolean &operator=(T &cp)
	{
		int i = (bool)cp?1:0;
		if(0!=(_storage&~1)) i = 3*i+2; //2 or 5
		return operator=(i);
		daeCTC<(sizeof(T)<=sizeof(int))>();
	}
	/**
	 * Direct assignment of 0 1 (or @c true_token @c false_token.)
	 */
	inline daeBoolean &operator=(int cp)
	{
		assert(cp<6); _storage = cp&0x7; return *this;
	}

	template<class T>
	/**
	 * Direct assignment of "0" "1" "true" or "false".
	 */
	inline daeBoolean &operator=(const T *cp)
	{
		_unserialize<true>(cp); return *this;
	}
	template<class T, int N>
	/**
	 * Direct assignment of "0" "1" "true" or "false".
	 */
	inline daeBoolean &operator=(T (&cp)[N])
	{
		_unserialize<true>(cp); return *this;
	}
	template<bool A> 
	inline bool _unserialize(const daeHashString &v)
	{
		if(v.extent==1)
		{
			if(A) assert('0'==*v||'1'==*v);
			else if('0'!=*v&&'1'!=*v) return false;
			_storage = '1'==*v?1:0;
		}
		else 
		{
			if(A) assert("false"==v||"true"==v);
			else if("true"!=v&&"false"!=v) return false;
			_storage = v.extent==4?true_token:false_token;
		}
		return true;
	}	

COLLADA_(public) //Standard Library compatibility layer	
	/**
	 * This is practically unavoidable.
	 */
	inline bool empty()const
	{
		assert(0!=size()); return false; 
	}

	/**
	 * @c daeString_support requires this.
	 */
	inline size_t size()const
	{
		assert(_storage<6);
		unsigned char o[8] = {1,1,5,0,0,4,0,0}; //0x7
		return o[_storage]; 
	}	

	/**
	 * @c daeString_support requires this.
	 */
	inline daeString data()const
	{
		assert(_storage<6);
		static const daeStringCP o[15] = {"0\0001\0false\0true"}; //0x7
		return o+2*_storage;
	}	

COLLADA_(public) //daeTypewriter support
	/** 
	 * Inputs a string.
	 */
	friend std::istream &operator>>(std::istream &i, daeBoolean &dst)
	{
		char b[8]; i >> std::setw(sizeof(b)) >> b;

		if(!dst._unserialize<false>(b)) i.setstate(i.failbit); return i;
	}

	/** 
	 * Outputs @c data().
	 */
	friend std::ostream &operator<<(std::ostream &o, const daeBoolean &src)
	{
		o << src.data(); return o;
	}
};
/**xs:boolean
 * @c daeBoolean replaces the old @c daeTypist<bool> implementation
 * of xs:boolean. The old implementation now uses "bool" and "false"
 * exclusively as it's associated with @c daeAST::TypedUnion.
 * @note @c xs::boolean now uses @c bool, similar to how @c daeString
 * is used by @c xs::string. @c daeBoolean is its storage medium.
 */
template<> struct daeTypist<daeBoolean> : daeTypist<> 
{	
	enum{ system_type=daeAtom::BOOLEAN };
	
	static int collate(const daeBoolean &a, const daeBoolean &b)
	{
		return !a&&b?-1:a==b?0:+1;
	}
};
template<> struct daeTypist<bool> : daeTypist<> //daeAST::TypedUnion
{	
	enum{ binary_type=daeAtom::UBOOLEAN };

	static int collate(const bool &a, const bool &b)
	{
		return !a&&b?-1:a==b?0:+1;
	}

	template<class, class IO> 
	static void formatXML(IO &io, int)
	{
		io.setf(io.boolalpha); 
	}	
	static daeError encodeXML(std::istream &src, bool &dst)
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
		char b[8]; src >> std::setw(sizeof(b)) >> b;
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

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ATOMIC_TYPE_H__
/*C1071*/
