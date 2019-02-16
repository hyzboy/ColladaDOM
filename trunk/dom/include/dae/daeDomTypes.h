/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_DOM_TYPES__
#define __COLLADA_DOM__DAE_DOM_TYPES__

#include "daeURI.h"
#include "daeIDRef.h"
#include "daeSIDResolver.h"

/**C-PREPROCESSOR MACRO
 * This is a C++ "rule of 3" wrapper for ColladaDOM 3 C++ types.
 */
#define COLLADA_DOM_3(x,y,...) COLLADA_DOM_3__##y##__##__VA_ARGS__(x)
#ifndef COLLADA_DOM_3__enum__
#define COLLADA_DOM_3__enum__(x)\
operator __enum__()const{ return __value__; }\
template<class T> __enum__ operator=(const T &v){ return __value__ = v; }\
x(__enum__ v=(__enum__)0):__value__(v){}
#endif
#ifdef NDEBUG
#error I worry omitting operator= may add redundant ref-count code.
#endif
#ifndef COLLADA_DOM_3__struct__daeSmartRef
#define COLLADA_DOM_3__struct__daeSmartRef(x) \
template<class T> x(T &cp):daeSmartRef(cp){}\
template<class T> x(const T &cp):daeSmartRef(cp){} x(){}
#endif

/**C-PREPROCESSOR MACRO
 * This just cuts out a lot of repetitive code in the generator output. 
 * @c __NB__ and @c __NS__ are just how the first generator to use this
 * happened to have been written. This is just to shape generator output.
 */
#define COLLADA_DOM_NOTE(N,context_concern,...) \
template<> struct __NS__<N>:DAEP::Note<>\
{ typedef __NB__::context_concern concern; __VA_ARGS__ };

COLLADA_(namespace)
{	
	/**EXPERIMENTAL
	 * Gives @c operator->*(xs::QName)
	 * access to namespace/prefix part.
	 */
	typedef daeXMLNS xmlns;

	//Refs default to the "ref" attribute.
	namespace xml
	{
		/**
		 * Users should prefer @c xml::baseURI 
		 * if possible. The 2.4 code generator
		 * may need to be modified.
		 */
		typedef daeURI_size<> base; //xml:base

		typedef daeURI baseURI; //auto-complete
	}
	namespace xs
	{
		//These are defined last below.
		//DAEP::xs using's COLLADA::xs.
	};
	namespace math
	{
		/////////////////////////////////////////////
		//REMINDER: domAny IS COLLADA-DOM 2.x STYLE//
		/////////////////////////////////////////////
		typedef domAny math; //math:math default type
	}
	namespace DAEP
	{
		/**
		 * @c DAEP::__DAEP__Object__v1__model() is using
		 * this to look nice/not redefine C-preprocessor 
		 * macros.
		 */
		namespace xmlns = COLLADA;

		namespace xml
		{
			typedef daeURI baseURI; //auto-complete

			/**
			 * @c COLLADA::DAEP::xml::base takes the place 
			 * of COLLADA::xml::base.
			 */
			struct base : const_daeURIRef
			{	
				template<class T>
				base &operator=(const T &cp)
				{
					_assign(base(cp)); return *this; 
				}
				template<int N>
				base(const daeURI_size<N> *p):daeSmartRef(p)
				{}
				base(const daeDoc &doc, const DAEP::Object *c)
				{
					new(this) const_daeURIRef(doc.getBaseURI(c)); 
				}
				base(const DAEP::Object *c=nullptr)
				{
					const daeDoc *d = 
					c==nullptr?nullptr:((daeObject*)c)->getDoc();
					if(d!=nullptr) new(this) base(*d,c);
				}
				template<int ID, class CC, typename CC::_ PtoM>
				base(const DAEP::Value<ID,xmlBase,CC,PtoM> &xml_base)
				{
					new(this) base(&xml_base.object());
				}
			};
		}

		namespace xs //xs:any abstract type
		{
			using namespace ::COLLADA::xs;
			typedef daeElementRef any;
			typedef const_daeElementRef const_any;
		}
		
		namespace math //math:math default type
		{
			typedef domAnyRef math; 
			typedef const_domAnyRef const_math; 
		}
		
		template<class Note, class Unsigned>
		/**TEMPLATE-SPECIALIZATION, BREAKING-CHANGE
		 * Don't know if this is helpful or not. It breaks client code, 
		 * -but seems like the logical progression of SID support.
		 */
		struct Cognate<daeString,Note,typename Note::schema::sidref_type,Unsigned>
		{
			typedef daeSIDREF type; 
		};
	
		template<class Note, class Unsigned>
		/**TEMPLATE-SPECIALIZATION, LEGACY-SUPPORT
		 * Historically the library treats urifragment_type as a URI.
		 * @note URIFragmentType is the name taken from the 1.4.1 COLLADA schema.
		 */
		struct Cognate<daeString,Note,typename Note::schema::URIFragmentType,Unsigned>
		{
			typedef daeURI type; 
		};
		template<class Note, class Unsigned>
		/**TEMPLATE-SPECIALIZATION, LEGACY-SUPPORT
		 * Historically the library treats urifragment_type as a URI.
		 * @note urifragment_type is the name taken from the 1.5.0 COLLADA schema.
		 */
		struct Cognate<daeString,Note,typename Note::schema::urifragment_type,Unsigned>
		{
			typedef daeURI type; 
		};

		//TODO? Known issues:
		//https://www.khronos.org/bugzilla/show_bug.cgi?id=1948
		//
		//	*gl_pipeline_settings stencil_mask value:
		//
		//		64-bit defaults to 4294967295 (becomes -1 as 32-bit value.)
		//
		#if COLLADA_UPTR_MAX==0xFFFFFFFF || defined(COLLADA_DOM_INT32_long_long)
		template<class Note, class Unsigned>
		/**TEMPLATE-SPECIALIZATION, LEGACY-SUPPORT
		 * 64 bit values should not be used on 32 bit systems. If a value requires
		 * 64 bits this specialization can still be overriden.
		 * @note 64 bit @c size_t is standard on 64 bit systems, for better or worse.
		 */
		struct Cognate<signed long long,Note,Note,Unsigned>
		{
			typedef signed COLLADA_DOM_INT32 type; 
		};
		template<class Note, class Unsigned>
		/**TEMPLATE-SPECIALIZATION, LEGACY-SUPPORT
		 * 64 bit values should not be used on 32 bit systems. If a value requires
		 * 64 bits this specialization can still be overriden.
		 * @note 64 bit @c size_t is standard on 64 bit systems, for better or worse.
		 */
		struct Cognate<unsigned long long,Note,Note,Unsigned>
		{
			typedef unsigned COLLADA_DOM_INT32 type; 
		};				
		#else
		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * COLLADA is ridiculous for using 64-bit indices. There's never a reason
		 * to do that, as datasets can always be partitioned if they truly needed
		 * BILLIONS of datapoints!
		 */
		struct Container<long long,Unsigned>
		{
			typedef daeArray<signed COLLADA_DOM_INT32> type; 
		};
		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * COLLADA is ridiculous for using 64-bit indices. There's never a reason
		 * to do that, as datasets can always be partitioned if they truly needed
		 * BILLIONS of datapoints!
		 */
		struct Container<unsigned long long,Unsigned>
		{
			typedef daeArray<unsigned COLLADA_DOM_INT32> type; 
		};
		#endif

		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * @c daeBoolean is the storage class for @c bool (@c xs::boolean).
		 * It remembers if XML had used a 0/1 presenation of an xs:boolean
		 * or a true/false presentation. It defaults to 0/1 in the xs:list
		 * context, and the more friendly true/false in an attribute/value
		 * context.
		 */
		struct Container<bool,Unsigned>
		{
			typedef daeArray<daeBoolean> type; 
		};		

		template<>
		/**
		 * Not sure how to deal with this, or if it's valid, however, with
		 * @c xsAnySimpleType the implementation can't distinguish between
		 * @c daeAST::Data and @c daeArray<daeAST::TypedUnion> because the
		 * offsets can't be demonstrated to be inside the container object.
		 */
		struct Container<xsAnySimpleType,signed>
		{
			//TODO: xs::anyAtomicType could be put here. It could not have 
			//the same semantics as xsAnySimpleType. There's not much need
			//to go to this length however, since its use would be limited
			//to schemata that actually use this pattern... which might be 
			//none.
			typedef xsAnySimpleType type; 
		};		

		template<int N, class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * For some misbegotten reason COLLADA 1.4.1 and 1.5.0 have spaces
		 * in their "list_of_hex_binary_type" types. They really shouldn't
		 * have been list-based, but the spefication is what it is for now.
		 * It's possible the schema could be retroactively changed, making
		 * documents with spaces invalid, but either way, this specializes
		 * a plain xs:hexBinary list so that the first item is an embedded
		 * allocation-unit. If there are more than one, the memory is lost.
		 * Because hexBinary is an array, this way the first dimension can
		 * not be dynamically allocated. Since it's binary data, if it has
		 * units, that should be stored in a header at the top of the data.
		 * @tparam N applies to base64Binary also, though COLLADA has none.
		 *
		 * @remark "sizeof(void*)" is to avoid allocating dummy allocation
		 * units if a multi-dimensional array is indeed used. 1 would work
		 * as well, but it may as well be rounded up to the structure size.
		 */
		struct Container<daeBinary<N>,Unsigned>
		{
			typedef daeArray<daeBinary<N,sizeof(void*)>,1> type; 
		};		

		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * Post-2.5 references are based on @c DAEP::Object, and so cannot
		 * be housed in @c daeArray since smart-refs cannot form permanent
		 * pointers into the array. There are other reasons, but this is a
		 * strong rationale.
		 */
		struct Container<daeURI,Unsigned>
		{
			typedef daeArray<daeTokenRef> type; 
		};
		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * @see @c daeURI specialization's Doxygentation.
		 */
		struct Container<daeIDREF,Unsigned>
		{
			typedef daeArray<daeTokenRef> type; 
		};
		template<class Unsigned>
		/**PARTIAL-TEMPLATE-SPECIALIAZTION 
		 * @see @c daeURI specialization's Doxygentation.
		 */
		struct Container<daeSIDREF,Unsigned>
		{
			typedef daeArray<daeTokenRef> type; 
		};
		
		template<int ID, class CC, typename CC::_ PtoM>
		/**TEMPLATE-SPECIALIZATION
		 * @see the @c daeBoolean specialization of DAEP Container Doxygentation.
		 */
		class Value<ID,bool,CC,PtoM> 
		:
		public InnerValue<ID,bool,CC,PtoM,daeBoolean>
		{
			COLLADA_DOM_INNER_OPERATORS //C2679

			/**
			 * I don't think this is needed.
			 */
			//inline operator bool()const{ return this->value; }
			/**
			 * I know this will be required. I'm not sure what else. @c daeBoolean
			 * is not as C-like as a raw @c bool.
			 */
			inline bool operator!()const{ return !this->value; }
		};

		template<int ID, class CC, typename CC::_ PtoM>
		/**TEMPLATE-SPECIALIZATION
		 * @see the @c daeURI specialization of DAEP Container Doxygentation.
		 */
		class Value<ID,daeURI,CC,PtoM> : public InnerValue<ID,daeURI,CC,PtoM,daeStringRef>
		{
			COLLADA_DOM_INNER_OPERATORS //C2679
		};
		template<int ID, class CC, typename CC::_ PtoM>
		/**TEMPLATE-SPECIALIZATION
		 * @see the @c daeURI specialization of DAEP Container Doxygentation.
		 */
		class Value<ID,daeIDREF,CC,PtoM> : public InnerValue<ID,daeIDREF,CC,PtoM,daeTokenRef>
		{
			COLLADA_DOM_INNER_OPERATORS //C2679
		};
		template<int ID, class CC, typename CC::_ PtoM>
		/**TEMPLATE-SPECIALIZATION
		 * @see the @c daeURI specialization of DAEP Container Doxygentation.
		 */
		class Value<ID,daeSIDREF,CC,PtoM> : public InnerValue<ID,daeSIDREF,CC,PtoM,daeTokenRef>
		{
			COLLADA_DOM_INNER_OPERATORS //C2679
		};		

		//HACK: xml::base is daeURI_size<1> using xs::anySimpleType in place
		//of daeStringRef.
		template<int ID, class CC, typename CC::_ PtoM>
		/**TEMPLATE-SPECIALIZATION
		 * @see the @c daeURI specialization of DAEP Container Doxygentation.
		 */
		class Value<ID,xmlBase,CC,PtoM> : public InnerValue<ID,xmlBase,CC,PtoM,xsAnySimpleType>
		{
			COLLADA_DOM_INNER_OPERATORS //C2679
		};
	}
	//IT SEEMS USING DAEP::Container BELOW MAKES THESE DEPENDENT ON THE ABOVE SPECIALIZATIONS
	namespace xs
	{
	//EXPERIMENTAL
	/**
	 * @c any changes similar to how generated 
	 * classes change from object to smart-ref.
	 */
	typedef daeElement any; 	
	typedef any element_type;
	typedef element_type any_type;
	typedef dae_Array<any,1> any_name;	
	typedef daeContents complexContent;
	#if ! COLLADA_(using)
	typedef daeChildRef<> element;
	typedef dae_Array<> element_name;	
	#elif COLLADA_(using)
	template<class T=daeElement>
	using element = daeChildRef<T>;
	template<class T=daeElement>
	using element_name = dae_Array<T>;	
	template<class T, int size_on_stack=0>
	using list = daeArray<T,size_on_stack>; //xs:list
	#endif
	typedef daeSmartTag complexType;
	typedef daeSmartTag_base const_complexType;

	typedef domAny anyType;
	typedef daeAnyAttribute anyAttribute;
	typedef daeAST::TypedUnion anySimpleType;
	typedef daeURI anyURI;	
	typedef daeString language;
	typedef daeString duration;
	typedef daeString dateTime;	
	typedef daeString fate, time;	
	typedef daeString gYear, gMonth, gDay;	
	typedef daeString gYearMonth, gMonthDay;	
	typedef daeBinary<16> hexBinary;
	typedef daeBinary<64> base64Binary;	
	typedef daeString QName;	
	typedef daeString NOTATION;	
	typedef daeString ID;	
	typedef daeIDREF IDREF;
	typedef daeArray<DAEP::Container<daeIDREF>::type::value_type> IDREFS;
	typedef daeString ENTITY;
	typedef daeArray<DAEP::Container<ENTITY>::type::value_type> ENTITIES;
	typedef daeString NCName;	
	typedef daeString NMTOKEN;
	typedef daeArray<DAEP::Container<NMTOKEN>::type::value_type> NMTOKENS;
	typedef daeString Name;	
	typedef daeString token;	
	typedef daeString string;	
	typedef daeHashString string_view;
	typedef daeString normalizedString;
	typedef bool boolean; //daeBoolean
	typedef daeFloat float__alias;	
	typedef daeDouble double__alias;	
	typedef daeDouble decimal;	
	typedef daeLong integer;	
	typedef daeULong nonNegativeInteger;
	typedef daeULong positiveInteger;
	typedef daeLong nonPositiveInteger;
	typedef daeLong negativeInteger;	
	typedef daeLong long__alias;	
	typedef daeULong unsignedLong;	
	typedef daeInt int__alias;
	typedef daeUInt unsignedInt;	
	typedef daeShort short__alias;	
	typedef daeUShort unsignedShort;	
	typedef daeByte byte;	
	typedef daeUByte unsignedByte;
	}

#if 2 == COLLADA_DOM || !defined(COLLADA_NOLEGACY)

	 //RESERVED
	//This is reserved for a type that sets its length
	//to 0 instead of calling strlen() in order to not
	//inject strlen calls into user code. It's used to
	//fix the 2.x setX() mutator APIs so setting a URI
	//is not ambiguous and doesn't trigger conversions
	//on top of conversions--involving buffers/exports.
	//daeStringRef would then call strlen() if it must.
	//THE QUESTION IS, IS LEGACY-SUPPORT EVEN WORTH IT?
	typedef daeHashString daeHashString2;

	//2.5 refs default to the "ref" attribute.
	typedef daeURI_size<> xmlBase; //xml:base
	typedef domAny mathMath; //math:math default type
	typedef daeSmartRef<domAny> mathMathRef;

	//use this if you want to
	typedef daeElement domElement;

	//legacy: seems harmless
	#define daeTSmartRef daeSmartRef
	//2.5: same deal as above
	#define daeTArray daeArray

	//EXPERIMENTAL
	typedef daeElement xsAny;
	typedef daeElementRef xsAnyRef;
	typedef const_daeElementRef const_xsAnyRef;
	typedef daeArray<daeElementRef> xsAny_Array;
	typedef daeArray<const_daeElementRef> const_xsAny_Array;

	typedef domAny xsAnyType;
	typedef daeAnyAttribute xsAnyAttribute;
	typedef daeAST::TypedUnion xsAnySimpleType;
	typedef daeURI xsAnyURI;
	typedef daeIDREF xsIDREF;
	typedef daeString xsString,
	xsName, xsToken, xsNormalizedString, 
	xsDuration, xsDateTime, xsDate, xsTime, 
	xsGYear, xsGMonth, xsGDay, xsGYearMonth, xsGMonthDay,
	xsQName, xsNOTATION, xsENTITY, xsNCName, xsNMTOKEN, 
	xsID, xsLanguage;
	typedef daeArray<daeTokenRef> xsStringArray, 
	xsNameArray, xsNMTOKENS, xsNMTOKENArray, xsNormalizedStringArray, 
	xsDurationArray, xsDateTimeArray, xsDateArray, xsTimeArray,
	xsGYearArray, xsGMonthArray, xsGDayArray, xsGYearMonthArray, xsGMonthDayArray, 	
	xsQNameArray, xsNOTATIONArray, xsENTITIES, xsENTITYArray, xsNCNameArray, xsTokenArray, 
	xsIDArray, xsIDREFS, xsIDREFArray, xsAnyURIArray;
	typedef daeBinary<16> xsHexBinary;	
	typedef daeArray<xsHexBinary> xsHexBinaryArray; //A		
	typedef daeBinary<64> xsBase64Binary;	
	typedef daeArray<xsBase64Binary> xsBase64BinaryArray; //A
	typedef bool xsBoolean; //daeBoolean
	typedef daeArray<daeBoolean> xsBooleanArray; //A
	typedef daeFloat xsFloat;
	typedef daeArray<daeFloat> xsFloatArray; //A
	typedef daeDouble xsDouble;
	typedef daeArray<daeDouble> xsDoubleArray; //A
	typedef daeDouble xsDecimal;
	typedef daeArray<daeDouble> xsDecimalArray; //A
	typedef daeLong xsInteger;
	typedef daeArray<daeLong> xsIntegerArray; //A
	typedef daeULong xsNonNegativeInteger;
	typedef daeArray<daeULong> xsNonNegativeIntegerArray; //A
	typedef daeULong xsPositiveInteger;
	typedef daeArray<daeULong> xsPositiveIntegerArray; //A
	typedef daeLong xsNonPositiveInteger;
	typedef daeArray<daeLong> xsNonPositiveIntegerArray; //A
	typedef daeLong xsNegativeInteger;
	typedef daeArray<daeLong> xsNegativeIntegerArray; //A
	typedef daeLong xsLong;
	typedef daeArray<daeLong> xsLongArray; //A
	typedef daeULong xsUnsignedLong;
	typedef daeArray<daeULong> xsUnsignedLongArray; //A
	typedef daeInt xsInt;
	typedef daeArray<daeInt> xsIntArray; //A
	typedef daeUInt xsUnsignedInt;
	typedef daeArray<daeUInt> xsUnsignedIntArray; //A
	typedef daeShort xsShort;
	typedef daeArray<daeShort> xsShortArray; //A
	typedef daeUShort xsUnsignedShort;
	typedef daeArray<daeUShort> xsUnsignedShortArray; //A
	typedef daeByte xsByte;
	typedef daeArray<daeByte> xsByteArray; //A
	typedef daeUByte xsUnsignedByte;
	typedef daeArray<daeUByte> xsUnsignedByteArray; //A

#endif
}

#endif //__COLLADA_DOM__DAE_DOM_TYPES__
/*C1071*/
