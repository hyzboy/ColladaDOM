/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_TYPES_H__
#define __COLLADA_DOM__DAE_TYPES_H__
				
#include "daeError.h"

#ifndef COLLADA_DOM_USERDATA
#define COLLADA_DOM_USERDATA void //LEGACY
#endif 

COLLADA_(namespace)
{	
	//SCHEDULED FOR REMOVAL
	#ifndef COLLADA_DOM_MAKER
	inline const char *_COLLADA_DOM_MAKER();
	#define COLLADA_DOM_MAKER _COLLADA_DOM_MAKER()
	#endif

	typedef class daeElementTags daeTags;
		
	/**BIG-ENDIAN FACILITY
	 * This class exists because bitfields are the simplest
	 * way to expose these fields on big-endian systems.
	 * @see daeObject
	 */
	struct daeObjectTags
	{			
		/**
		 * Casts to the type of @c __DAEP__Object__tags.
		 */
		operator unsigned int()const{ return *(unsigned int*)this; }
		/**
		 * Constructor
		 */
		daeObjectTags(unsigned int i=0){ *(unsigned int*)this = i; }

		/**BITFIELD
		 * This is not implemented with @c char because XML/COLLADA
		 * may have applications on unusual workstations, and it is
		 * more fair.
		 *
		 * @c classTag is shared by classes of inheritable objects.
		 * @c objectTag is private to a class that's not inherited.
		 * @c interfaceTag is internal/theoretically provides a way
		 * for interfaces to be updated separate from their clients.
		 * @c moduleTag is internal/keeps a record of the module an 
		 * object belongs to. The first bit is 1 if an object lives
		 * in a @c daeDatabase based database. Leftover bits can be
		 * included in this tag, in order to allow for more modules.
		 */
		unsigned int classTag:8,objectTag:8,interfaceTag:8,moduleTag:8;
	};

	/**SINGLETON
	 * @c COLLADA::DOM_process_share is wrapped up in
	 * this class so it's auto-initialized at startup.
	 */
	extern struct daePShare
	{
		/**
		 * @c tag.interfaceTag is set to the version of
		 * @c DAEP::Object. Classes can modify it after.
		 * @c tag.moduleTag is set to the process share.
		 */
		daeObjectTags tags;
		
		daeOK OK;

		COLLADA_DOM_LINKAGE 
		/**NOT-THREAD-SAFE
		 * This is more or less a @c private "init" API. 
		 * It was "COLLADA::DOM_grant_process_share()."
		 */
		daeOK grant(size_t=sizeof(daePShare));
		
		/**
		 * Dynamic clients must define: 
		 * ~~~~~~~~~~~~~~~~~~~~~~{.C++}
		 * //Initialize the process-share.
		 * extern COLLADA::daePShare COLLADA::DOM_process_share = 0;
		 *
		 * @param _extern_ is just so C++ lets the @c extern keyword
		 * decorate the declaration. It is ignored.
		 */
		daePShare(int _extern_=0):tags(),_maker(COLLADA_DOM_MAKER)
		{
			//There's a limit of 127 shares. That's a lot of modules.
			OK = grant(); assert(OK==DAE_OK); (void)_extern_;
		}
		/** 
		 * The library will use this to free up a process-share slot.
		 */
		~daePShare(){ grant(0); }

		//SCHEDULED FOR REMOVAL?
		/**
		 * This is to decouple the process-share from the preprocessor
		 * so static libraries don't have to know @c COLLADA_DOM_MAKER.
		 */
		const char *const _maker;

	/**SINGLETON
	 * @see @c COLLADA::DAEP::Object::Object().	 
	 * User/client code must add this to a translation-unit.
	 *
	 * @todo Probably as long as the user is required to define
	 * this, the structure could be grown to include whatever's
	 * available to inline more of the library's infrastructure.
	 */
	}DOM_process_share;	

	//Here "CP" stands for "code-point."
	//NOTE: Historically the library doesn't commit to UTF8.
	//There had been a "Latin-1" option available via daeDOM,
	//but it was only implemented by the legacy LIBXML plugin.
	//Now it's up to the user provided platform; daeDOM is now
	//agnostic. The settings are now found in the legacy plugin.
	typedef char daeStringCP;	
	typedef const daeStringCP *daeString;
	typedef const daeStringCP (&daeString1)[1];	
	//static daeString1 empty_daeString1 = {};
	static daeStringCP empty_daeString1[1] = {};
	//daeStringCP may be unsigned as well
	typedef unsigned char daeUStringCP;
	//"client" here must be interpreted to mean
	//a string literal or functional equivalent
	typedef const daeStringCP *daeClientString;
	template<class,int>struct daeStringAllocator;
	//Shared/Standard allocator
	typedef daeStringAllocator<char,0> daeSA;

	//Pseudonym is neither a proper NCName nor QName.
	typedef class daeHashString daePseudonym,daeName;
	typedef const daeHashString &daeClientString2;

	//#ifndef COLLADA_DOM_MAKER
	/**
	 * This is used by @c __DAEP__Make__maker to visually identify
	 * the process-share. It's a default. The address is available
	 * regardless. It's recommended to define @c COLLADA_DOM_MAKER
	 * to a string-literal that describes the output of the linker.
	 */
	inline daeClientString _COLLADA_DOM_MAKER()
	{
		//Reminder: %p is non-portable.
		static daeStringCP address[2+16+1] = "0x";
		if('\0'==address[2]) COLLADA_SUPPRESS_C(4996)			
		if(sprintf(address+2,"%p",&DOM_process_share)>=(int)sizeof(address)) assert(0);
		return address;
	}
	//#endif

	/**
	 * XML considers these to be whitespace; whereas @c ::isspace()
	 * consults the locale which might produce surprises, and anyway
	 * is not technically correct.
	 */
	inline int COLLADA_isspace(int i)
	{
		return i==' '||i=='\t'||i=='\r'||i=='\n'?1:0; 
	}
	
	class daeStringRef; class daeTokenRef;

	//Formerly "daeBool."
	class daeBoolean;
	//Formerly "daeEnum."
	typedef int daeEnumeration; 

	//MOVING AWAY FROM daeDomTypes.h.
	//Still these are related constructs.
	//These represent the enum itself; both ways.
	struct daeClientEnum
	{
		daeClientString const string_constant; 
		daeEnumeration const integral_constant;  
	};
	
  ////REMINDER/////////////////////////////////////////////////
  //These need to be macros to work around access-controllers//
  /////////////////////////////////////////////////////////////

	//assigned to daeAllocThunk
	typedef ptrdiff_t daeOffset;
	//alternative to C offsetof macro
	//(Probably to suppress warnings.)
	#define daeOffsetOf(class,member) daeOffsetOf2(class,->member)
	//This is introduced for pointer-to-member
	//thanks to GCC's "Token Spacing" practice.
	#define daeOffsetOf2(class,___member) \
	((daeOffset)(((char*)&(((class*)0x0100)___member))-(char*)0x0100))
	#define daeObjectOf(this,class,member) \
	(*(class*)(daeOffset(this)-daeOffsetOf(class,member)))
	typedef size_t daeSize,daeHash;
	#define daeSizeOf(class,member) ((daeSize)sizeof(((class*)0x100)->member))

	//No relation to daeBoundaryStringIn; Computes a memory "alignment" offset.
	inline daeOffset daeBoundaryOffset(daeOffset os, daeOffset a=sizeof(void*))
	{
		//It's surprisingly hard to find sources for this simple function.
		//(os+(a-1))&~(a-1) works also, but -os&(a-1) is obviously better.
		return os+(-os&(a-1));
	}
	  								 
	//predeclaration
	//NOTE: typedef types are not 
	//defined here, so that Visual Studio
	//will associate the comment text with the typedef.
	//Their deprecated forms must be postponed as well.	
	namespace DAEP
	{	
		template<class> class Proto;		
		template<class> struct NoValue;
		class Change; 
		class Object; class Element; class Model;	
		template<class,unsigned long long,class>
		class Elemental;
	}		
	template<class=void> class daePlatonic;
	class daeAllocThunk;	
	template<class=daeStringCP,int=0> class daeArray;
	template<class,class> class daeArrayAware;
	//4*4 is anything to avoid 0. It's 3D matrix sized.
	template<class=void,int=4*4> class daeAlloc;
	class daeObject;			
	template<int=0> class daeAA_Value;
	typedef class daeAnyAttribute daeAA;
	template<int> class daeCharData_size;
	typedef daeCharData_size<48> daeCharData;
	typedef class daeAnySimpleType daeAST;
	class daeText;
	class daeEOText;	
	union daeContent;	
	class daeContents_base;		
	template<class=void> struct dae_;	
	//can be daeDocument::_PseudoElement
	typedef class daeElement daePseudoElement;	
	//Note: 1 is unnamed, 0 is for generic reference.
	template<class=daeElement,int=0> class dae_Array;
	class daeDoc;
	class daeDocument;
	class daeDOM;
	class daeImage;
	class daeAtlas;
	class daeArchive;
	template<class T=void> struct daeDocRoot;
	class daeDatabase;	
	class daePlatform;	
	class daeIO;
	class daeIOPlugin;
	class daeIORequest;
	//Letting daeAtom serve as shorthand.
	//class daeAtom{};
	typedef struct daeAtomicType daeAtom;
	template<class=void> class daeTypist;
	class daeTypewriter;
	class daeTypewriter2;
	template<int=0,int=0> class daeBinary;	
	class daeModel;	
	class daeFeature;
	//EXPERIMENTAL/INTERNAL
	//These are non-integer operator[] subscripts. 
	enum daeFeatureID{}; 
	//EXPERIMENTAL Affords limited iteration.
	inline daeFeatureID operator++(daeFeatureID &ID,int)
	{
		daeFeatureID old = ID; ID = (daeFeatureID)(int(ID)+1); return old;
	}	
	//EXPERIMENTAL Affords limited iteration.
	inline daeFeatureID operator+(daeFeatureID a, int b)
	{
		return daeFeatureID((int)a+b);
	}	
	class daeMetaObject;
	typedef const class daeMetaElement daeMeta;
	class daeData;
	class daeProcessShare;		
	class daeCM;
	class daeAny;
	/**namespace COLLADA::XS
	 * 
	 * This namespace contains classes that correspond
	 * to elements in the XML Schema's (XSD) namespace.
	 *
	 * Classes in this namespace are used to configure
	 * the code-generation step's XML element metadata.
	 */
	struct XS
	{	
		//Holds processContents & namespace attributes.
		typedef daeAny AnyAttribute;
		 
		class SimpleType; typedef const SimpleType List;
		class Attribute; class Restriction; class Enumeration;		
		class Schema; typedef daeArray<const Schema*,8> Schemata;
		class Import;		
				
		class Any; class Element; class Group; 
		class All; class Sequence; class Choice; 
	
		enum __daeXS //XS::All is not implemented.
		{			
			ALL=-3,CHOICE,SEQUENCE, //negative for daeCM::isParentCM().
			ANY,ELEMENT,GROUP, //0~2, etc.
			SCHEMA=30,ATTRIBUTE, //miscellaneous
		};
	};
	typedef XS::__daeXS daeXS;				
	typedef const class daeDefault daeValue;
	typedef const XS::Attribute daeAttribute;
	typedef const daeArray<const XS::Element> daeTOC;	
	class daeRef;
	class daeRefRequest;
	class daeRefResolver;
	class daeRefResolverList;
	//daeURI_size<> is COLLADA::xml::base at present.
	template<int=1> 
	class daeURI_size; //GCC
	class daeURI_base;
	class daeURI_parser;
	class daeURI;
	class daeContainedObject;
	class daeDefaultURIResolver;
	class daeDefaultIDREFResolver;
	class daeDefaultSIDREFResolver;
	class daeQuietErrorHandler;
	class daeStandardErrorHandler;		
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("daeDOM")
	typedef daeDOM DAE;
	COLLADA_DEPRECATED("daeAttribute")
	typedef daeAttribute daeMetaAttribute;
	COLLADA_DEPRECATED("const daeCM")
	typedef const daeCM daeMetaCMPolicy;
	COLLADA_DEPRECATED("const XS::Any")
	typedef const XS::Any daeMetaAny;
	COLLADA_DEPRECATED("const XS::Choice")
	typedef const XS::Choice daeMetaChoice;
	COLLADA_DEPRECATED("const XS::Element")
	typedef const XS::Element daeMetaElementAttribute;
	COLLADA_DEPRECATED("const XS::Group")
	typedef const XS::Group daeMetaGroup;
	COLLADA_DEPRECATED("const XS::Sequence")
	typedef const XS::Sequence daeMetaSequence;
	//daeAtomicType has a new meaning.
	//COLLADA_DEPRECATED("daeTypewriter")
	//typedef daeTypewriter daeAtomicType;
	COLLADA_DEPRECATED("daeRefResolver")
	typedef daeRefResolver daeURIResolver;
	COLLADA_DEPRECATED("daeRefResolverList")
	typedef daeRefResolverList daeURIResolverList;	
	COLLADA_DEPRECATED("daeDefaultURIResolver")
	typedef daeDefaultURIResolver daeStandardURIResolver;
	COLLADA_DEPRECATED("daeDefaultIDREFResolver")
	typedef daeDefaultIDREFResolver daeDefaultIDRefResolver;
	COLLADA_DEPRECATED("daeQuietErrorHandler")
	typedef daeQuietErrorHandler quietErrorHandler;
	COLLADA_DEPRECATED("daeStandardErrorHandler")
	typedef daeStandardErrorHandler stdErrPlugin;
	#endif //COLLADA_NODEPRECATED

	//This default is a limited shorthand only.
	template<class=daeObject> class daeSmartRef;	
	template<class=daeElement> class daeChildRef;	
	typedef daeSmartRef<daeObject> daeObjectRef;
	typedef daeSmartRef<daeElement> daeElementRef;
	typedef daeSmartRef<daeURI> daeURIRef;
	typedef daeSmartRef<daeDoc> daeDocRef;
	typedef daeSmartRef<daeDOM> daeDOMRef;
	typedef daeSmartRef<daeImage> daeImageRef;
	typedef daeSmartRef<daeAtlas> daeAtlasRef;
	typedef daeSmartRef<daeArchive> daeArchiveRef;
	typedef daeSmartRef<daeDocument> daeDocumentRef;
	typedef daeSmartRef<daeRef> daeRefRef;	
	typedef class daeSmartTag_base const_daeSmartTag;
	typedef daeSmartRef<const daeURI> const_daeURIRef;
	typedef daeSmartRef<const daeDOM> const_daeDOMRef;
	typedef daeSmartRef<const daeDoc> const_daeDocRef;
	typedef daeSmartRef<const daeObject> const_daeObjectRef;
	typedef daeSmartRef<const daeElement> const_daeElementRef;
	typedef daeSmartRef<const daeArchive> const_daeArchiveRef;
	typedef daeSmartRef<const daeDocument> const_daeDocumentRef;

	template<class=const daeObject> struct daeNullable;

	//NOTE This grew out of a need/realization
	//but it's actually pretty neat and useful.
	struct INCOMPLETE_base
	{	
		//COLLADA:: is to silence GCC warnings.
		typedef COLLADA::daeName daeName;
		typedef COLLADA::daeMeta daeMeta;
		typedef COLLADA::daeModel daeModel;		
		typedef COLLADA::daeDoc daeDoc;
		typedef COLLADA::daeDOM daeDOM;
		typedef COLLADA::daeArchive daeArchive;
		typedef COLLADA::daeElement daeElement;
		typedef COLLADA::daeDocument daeDocument;
		typedef COLLADA::daeObject daeObject;
		typedef COLLADA::daeNullable<> daeNullable;
		typedef COLLADA::daeURI_base daeURI_base;	
		typedef COLLADA::daeURI_parser daeURI_parser;
		typedef COLLADA::daeContents_base daeContents_base;	
		typedef COLLADA::daeContainedObject daeContainedObject;
		//daeDB	(Clang)
		typedef COLLADA::daeAlloc<> daeAlloc;

		struct XS
		{
			//daeMetaElement::addAttribute.
			typedef COLLADA::XS::List List;
		};

		struct DAEP
		{
			typedef COLLADA::DAEP::Change Change;
		};

		typedef COLLADA::daeDocRef daeDocRef;
	};
	/**GCC/C++ QUICK-FIX
	 * This is used to let templates look into
	 * the future so that the definition order
	 * doesn't matter, and code can stay close
	 * together around a class definition.
	 *
	 * The @c typename keyword must be used to
	 * access it, indirectly, via another type.
	 * Usually @c DAEP::Object, since it's the
	 * top-most type.
	 */
	template<class> struct INCOMPLETE : INCOMPLETE_base{};
	/**
	 * This macro is used to convert a strange
	 * notation into a kind of usage directive.
	 */
	#define COLLADA_INCOMPLETE(x) typename ::COLLADA::INCOMPLETE<x>::

	//COLLADA_INCOMPLETE(daeFig<N>) would work.
	/**
	 * This is for when there is not a type to
	 * work with but there is an int parameter.
	 */
	template<int> struct NCOMPLETE : INCOMPLETE_base{};
	/**
	 * This is analogous to COLLADA_INCOMPLETE
	 * but uses numerical constants in a pinch.
	 */
	#define COLLADA_NCOMPLETE(x) typename ::COLLADA::NCOMPLETE<x>::

	#ifndef COLLADA_NOLEGACY
	//these are legacy for the most part
	typedef daeArray<daeInt> daeIntArray; //xsIntegerArray
	typedef daeArray<daeUInt> daeUIntArray;
	typedef daeArray<daeFloat> daeFloatArray; //xsFloatArray
	typedef daeArray<daeEnumeration> daeEnumerationArray;
	typedef daeArray<daeString> daeStringArray; //NOT StringRef
	typedef daeArray<daeByte> daeByteArray; //xsHexBinaryArray
	typedef daeArray<daeBoolean> daeBooleanArray; //xsBooleanArray
	typedef daeArray<daeDouble> daeDoubleArray; //xsDoubleArray
	typedef daeArray<daeLong> daeLongArray; //xsLongArray
	typedef daeArray<daeShort> daeShortArray; //xsShortArray
	typedef daeArray<daeStringRef> daeStringRefArray; //xsNameArray 
	typedef daeArray<daeElementRef> daeElementRefArray;
	//THESE are thought to be ill-formed.
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("daeByte")
	typedef daeByte daeChar; 
	COLLADA_DEPRECATED("daeUByte")
	typedef daeUByte daeUChar;
	COLLADA_DEPRECATED("daeBoolean")
	typedef daeBoolean daeBool; 
	COLLADA_DEPRECATED("daeEnumeration")
	typedef daeEnumeration daeEnum; 
	COLLADA_DEPRECATED("daeArray<daeEnumeration>")
	typedef daeArray<daeEnumeration> daeEnumArray;
	COLLADA_DEPRECATED("daeArray<daeByte>")
	typedef daeArray<daeByte> daeCharArray;
	#endif //COLLADA_NODEPRECATED
	#endif //COLLADA_NOLEGACY	   
	class domAny; //built-in element class
	typedef daeSmartRef<domAny> domAnyRef;		
	typedef daeSmartRef<const domAny> const_domAnyRef;		
	#ifndef COLLADA_NODEPRECATED	
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Use daeArray<domAnyRef> instead if you must.\n\
	(domAny_Array is rightly dae_Array<domAny>.)")
	//Post-2.5: END-OF-SUPPORT
	//Use daeArray<domAnyRef> instead if you must.
	//(domAny_Array is rightly dae_Array<domAny>.)	
	typedef void domAny_Array;
	#endif //COLLADA_NODEPRECATED
	//This is used to catch client element classes 
	//that were generated with a different generator
	//than the one the DOM implementation is expecting.
	//IT'S ALSO HERE BESIDE domAny TO REMIND MAINTAINERS
	//TO SEE IF domAny.h/cpp REFLECT THE GENERATED CLASSES. 
	#define COLLADA_DOM_GENERATION 1	
}

COLLADA_(namespace) //maybe a separate support header someday
{//-.
//<-'

/**AGGREGATE
 * Toggles bit.
 * @see @c daeElement::getAttributeMask()
 * SEE-ALSO: @c DAEP::Value::write_mask()
 */
struct daeBit
{		 	
	daeOffset *first,second; 

	//NOTE: Compilers can reasonably deduce
	//empty status if sources don't produce
	//empty objects.

	inline operator bool()const
	{
		return empty()?false:get();
	}
	inline bool operator=(bool cp)const
	{
		if(!empty())
		if(cp) set(); else unset(); return cp;
	}

	inline void clear()
	{
		second = 0;
	}
	inline bool empty()const
	{
		return second==0; 
	}
	inline void set()const
	{
		assert(!empty()); *first|=second; 
	}
	inline void unset()const
	{
		assert(!empty()); *first&=~second; 
	}	
	inline bool get()const
	{
		assert(!empty()); return 0!=(*first&second); 
	}		
	 	
	inline void operator++(int)
	{
		assert(!empty());
		second<<=1; 
		if(empty()){ second = 1; first++; }
	}
	inline void operator--(int)
	{
		assert(!empty());
		second>>=1; //COLLADA_SUPPRESS_C(4554)
		if(empty()){ second = daeOffset(1)<<COLLADA_PTR_BIT-1; first--; }
	}
};

/**NULLABLE
 * Added for @c daeURI_base::isSameDocumentReference(). 
 */
struct dae3ool
{
	enum State{ is_not=-1,is_neither,is }state;	
	
	dae3ool(State state):state(state){}
	dae3ool(bool l):state(l?is:is_not){}
	inline operator bool(){ return state==is; }	
	inline bool operator!()const{ return state==is_not; }	
	inline bool operator==(dae3ool cmp)const{ return state==cmp.state; }
	inline bool operator!=(dae3ool cmp)const{ return state!=cmp.state; }	
};

template<int bits_on_stack=1>
/**
 * @tparam bits_on_stack bits accessed with @c daeBit.
 */
struct daeBit_bits
{		
	/**
	 * Converts to the first bit. 
	 */
	inline operator const daeBit()
	{
		daeBit o = { _bits,1 }; return o;
	}

	/**
	 * Gets bit at subscript @a i.
	 */
	inline const daeBit operator[](daeSize i)
	{
		daeBit o={_bits+i/COLLADA_PTR_BIT};
		return o.second=1<<i%COLLADA_PTR_BIT,o;
	}
	
	daeOffset _bits[COLLADA_BIT_PTR(bits_on_stack)];

	daeBit_bits(dae3ool fill=dae3ool::is_neither)
	{
		if(fill.state!=0) memset(_bits,fill?0xFF:0x00,sizeof(_bits));
	}
};	

/**
 * Formerly @c daeIO::Range, this class as @c daePartition
 * is used by @c daeAnyAttribute::getAnyExtraPart().
 */
struct daePartition
{
	size_t first,second; 
	inline size_t limit_to_size(size_t s)
	{
		if(first>s) first = s;
		if(second>s) second = s; return s;
	}		
	inline size_t size()const{ return second-first; }
	inline bool empty()const{ return first==second; }

	inline void offset(daeOffset os){ first+=os; second+=os; }
};

//// metaprogramming //// metaprogramming //// metaprogramming

/**WORKAROUND
 * "error: explicit specialization in non-namespace scope."
 * Visual Studio has long allowed method specializations in
 * the class body but C++ says no. 
 * This creates an overloadable type. It's called fig since
 * it is functionally a fig leaf and it looks like a figure.
 */
template<int> struct daeFig{};

/**
 * This was used lightly until GCC's enforcement of
 * "explicit specialization in non-namespace scope."
 */
template<int C4305, class A, class B> struct daeTypic
{
	typedef A type; 
};
template<class A, class B> struct daeTypic<false,A,B>
{
	typedef B type; 
};

template<class,unsigned long long> 
class daeElemental;
/**
 * This is pretty crazy. This is introduced after @c daeElement
 * was made to not inherit @c DAEP::Element. Still it's desired
 * to retain both @c DAEP::Elemental and @c daeElemental. To do
 * this, a "Legacy" paramter is added. The element class is not
 * yet defined initially, so @c daeElemental specifies "Legacy."
 * The "Legacy" parameter cannot be mismatched and so it should
 * be allowed to default to @c daeLegacy<T>::type going forward. 
 */
template<class T> struct daeLegacyOf
{
	typedef char No, (&Yes)[2];
	static No DAEP(daeElement*); static Yes DAEP(...);
	//error: explicit specialization in non-namespace scope.
	enum{ is_legacy=sizeof(No)==sizeof(DAEP((T*)nullptr)) };
	typedef typename daeTypic<is_legacy,daeElement,DAEP::Element>::type type;
};

template<class T>
/** Detect presence/support for operator[] like semantics. */
struct daeAtomOf
{
	//Currently only explicit __COLLADA__Atom is implemented.
	typedef char Yes, (&No)[2];
	template<typename S> static Yes Array(typename S::__COLLADA__Atom*);
	template<typename S> static No Array(...);
	template<class,int>
	struct Atom{ typedef class undefined type; };
	template<class S>
	struct Atom<S,sizeof(Yes)>{ typedef typename S::__COLLADA__Atom type; };
	typedef typename Atom<T,sizeof(Array<T>(nullptr))>::type type;
};
template<class T> struct daeAtomOf<T*>{ typedef T type; };
template<class T> struct daeAtomOf<const T*>{ typedef T type; };
template<class T, int N> struct daeAtomOf<T[N]>{ typedef T type; };
template<class T, int N> struct daeAtomOf<const T[N]>{ typedef T type; };

/** Copies the constness of @a S. This is used by "WRT" APIs. */
template<class S, class T> struct daeConstOf{ typedef T type; };
template<class S, class T> struct daeConstOf<const S,T>{ typedef const T type; };
template<class S, class T> struct daeConstOf<const S*,T>{ typedef const T type; };
//EXPERIMENTAL These are required to go the other direction.
template<class S, class T> struct daeConstOf<S,const T>{ typedef T type; };
template<class S, class T> struct daeConstOf<const S,const T>{ typedef const T type; };

//std containers were being passed cross-module
//COLLADA-DOM 2.5 uses these forms with LINKAGE
struct daeBoundaryStringIn
{
	inline operator daeString()const{ return c_str; }
	const daeString c_str;	
	daeBoundaryStringIn(daeString c_str) //nullptr?
	:c_str(c_str){}
	daeBoundaryStringIn(int o):c_str(0){ assert(o==0); }	
	template<int N>
	daeBoundaryStringIn(const daeArray<daeStringCP,N> &a)
	:c_str(a.data()){}	
	template<class A, class B, class C>
	daeBoundaryStringIn(const std::basic_string<A,B,C> &str)
	:c_str((daeString)str.c_str())
	{
		daeCTC<sizeof(A)==sizeof(daeStringCP)>();
	}	
	template<class T> //See WORKAROUND
	daeBoundaryStringIn(const T &operator_char_)
	:c_str(operator_char_){}	
};
//This form is used when daeHashString is desired.
template<class T> struct daeBoundaryString2
{	
	//This had assumed anything larger than a pointer
	//has a size packed into it, but that misses types
	//that refer to a pointer, especially DAEP::value's.	
	typedef typename daeTypic<daeArrayAware
	<T,T>::is_class,daeHashString,daeString>::type type;	
};//NOTE: These degrade to char.
//The non (&) forms are just for completeness sake.
template<class T, int N> struct daeBoundaryString2<T[N]>
{ typedef daeHashString type; };
template<class T, int N> struct daeBoundaryString2<T (&)[N]>
{ typedef daeHashString type; };
template<class T, int N> struct daeBoundaryString2<const T[N]>
{ typedef daeHashString type; };
template<class T, int N> struct daeBoundaryString2<const T (&)[N]>
{ typedef daeHashString type; };

//ColladaDOM.inl templates
template<class> inline daeMeta &daeGetMeta();
template<class> inline const daeModel &daeGetModel();
template<class S,class T> inline 
typename daeConstOf<T,typename S::__COLLADA__T>::type *daeSafeCast(T*);
template<class> inline bool daeUnsafe(const daeElement*);
//PHASING OUT? THESE WERE BYPASSING VIRTUAL-INHERITANCE. BUT IT'S NOT USED.
//These workaround that C++ requires virtual-base-classes use dynamic_cast.
//Note: dynamic_cast cannot be of use here, as the types may be incomplete.
 /**WARNING @warning CONST-CAST. */
inline daeModel *dae(const DAEP::Model *p){ return (daeModel*)p; }
 /**WARNING @warning CONST-CAST. */
inline daeModel &dae(const DAEP::Model &r){ return (daeModel&)r; }
 /**WARNING @warning CONST-CAST. */
inline daeObject *dae(const DAEP::Object *p){ return (daeObject*)p; }
 /**WARNING @warning CONST-CAST. */
inline daeObject &dae(const DAEP::Object &r){ return (daeObject&)r; }
//These 2 are since daeElement is no longer based on DAEP::Element.
 /**WARNING @warning CONST-CAST. */
inline daeElement *dae(const daeElement *p){ return (daeElement*)p; }
 /**WARNING @warning CONST-CAST. */
inline daeElement &dae(const daeElement &r){ return (daeElement&)r; }
 /**WARNING @warning CONST-CAST. */
inline daeElement *dae(const DAEP::Element *p){ return (daeElement*)p; }
 /**WARNING @warning CONST-CAST. */
inline daeElement &dae(const DAEP::Element &r){ return (daeElement&)r; }
template<class T>
/**WARNING @warning CONST-CAST. */
inline daeElement *dae(const daeSmartRef<T> &r){ T *p = r; return dae(p); }
/**WARNING @warning CONST-CAST. */
inline daeObject *dae(const daeObjectRef &r){ return (daeObject*&)r; }
/**WARNING @warning CONST-CAST. */
inline daeObject *dae(const const_daeObjectRef &r){ return (daeObject*&)r; }
template<class T>
/**WARNING @warning CONST-CAST. NOP */
inline daeDocRoot<T> &dae(const daeDocRoot<T> &NOP){ return (daeDocRoot<T>&)NOP; }
	
//---.
}//<-'
   
/**PREPROCESSOR MACRO
 * COLLADA_DOM_OBJECT_OPERATORS serves as a compatibility layer
 * for pre-2.5 APIs that inconsistently produced pointers and/or
 * C++ references. Post-2.5 APIs seem inconsistent as well, in so
 * far as they return mixed types, depending on relationships; But
 * the relationships are consistent:
 * - Local (lockable) pointers are returned by reference.
 * - Non-local (far) pointers are returned via @a daeSmartRef.
 * - Local, Nullable pointers are returned via pointer.
 *
 * These conversions are not considered a legacy feature, because it
 * is thought convenient that all returns should "decay" to a pointer.
 * It also means references can do "obj->a<daeX>()" v. "obj.a<daeX>()."
 *
 * REMINDER
 * @c COLLADA_DOM_OBJECT_OPERATORS is not limited to @c daeObject based
 * classes. It's used also by classes in/related to the @c XS namespace.
 */
#define COLLADA_DOM_OBJECT_OPERATORS(daeObj) \
/** COLLADA_DOM_OBJECT_OPERATORS Common interface, -> operator. */\
inline daeObj *operator->(){ return (daeObj*)this; }\
/** COLLADA_DOM_OBJECT_OPERATORS Common interface, const-> operator. */\
inline const daeObj *operator->()const{ return (daeObj*)this; }\
template<class _>\
/** COLLADA_DOM_OBJECT_OPERATORS Weak conversion to pointer. */\
inline operator _*(){ _ *upcast = (daeObj*)nullptr; return (_*)this; (void)upcast; }\
template<class _>\
/** COLLADA_DOM_OBJECT_OPERATORS Weak conversion to const-pointer. */\
inline operator const _*()const{ _ *upcast = (daeObj*)nullptr; return (_*)this; (void)upcast;  }

#endif //__COLLADA_DOM__DAE_TYPES_H__
/*C1071*/
