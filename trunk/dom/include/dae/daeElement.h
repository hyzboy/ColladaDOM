/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_ELEMENT_H__
#define __COLLADA_DOM__DAE_ELEMENT_H__

#include "daeMetaElement.h"

COLLADA_(namespace)
{//-.
//<-'

//#include daeDomTypes.h
//domAny and daeDocument
//want these definitions
typedef daeElement xsAny;
typedef daeURI_size<> xmlBase; 
typedef daeAnyAttribute xsAnyAttribute;
typedef daeAST::TypedUnion xsAnySimpleType;
									 							 
template<int size_on_stack>
/**UTILITY
 * Trying to add string-literal comparison operators to
 * a @c daeArray.
 * @remark It turns out to be useful for distinguishing
 * xs:byte arrays apart from string arrays, as both use
 * @c char.
 * @see @c daeCharData
 */
class daeCharData_size
:
public daeArray<daeStringCP,size_on_stack>  //FIRST
,
//Must include size_on_stack this time.
//public daeString_support<daeCharData>
public daeString_support<daeCharData_size<size_on_stack>>
{
	typedef daeArray<daeStringCP,size_on_stack> _1;
	typedef daeString_support<daeCharData_size<size_on_stack>> _2;

COLLADA_(public)

	using _1::size;
	using _1::data;	
	using _1::begin;
	using _1::end;	
	using _2::_assign;
	using _2::operator==;
	using _2::operator!=;	
	
	template<class T>
	daeCharData_size(const T &cp){ operator=(cp); }
	daeCharData_size(){ _1::clear_and_0_terminate(); } 	
	template<class T>
	inline daeCharData_size &operator=(const T &cp)
	{
		_assign(cp,nullptr); return *this; 
	}
	inline void _assign(const daeHashString &cp,...)
	{
		_1::assign_and_0_terminate(cp.data(),cp.size()); 
	}

	/**EXPERIMENTAL
	 * Facilitating @c daeBoundaryStringIn construction.
	 */
	inline operator daeString()const
	{
		assert(_1::back()=='\0'); return _1::data(); 
	}
};

/**UTILITY, LEGACY-SUPPORT
 * @c daeAttributes needs this defined outside of its body.
 */
struct daeAttribute2
{
	typedef void __COLLADA__atomize;
	daeName name; daeCharData value; 
};
template<int size_on_stack>
/**UTILITY, LEGACY-SUPPORT
 * @see string pairs version of @c daeElement::getAttributes().
 */
struct daeAttributes_size : daeArray<daeAttribute2,size_on_stack>
{
	typedef daeArray<daeAttribute2,size_on_stack> _base;

	daeAttributes_size(daeAnyAttribute &aa)
	{
		*this = aa;
	}
	daeAttributes_size &operator=(daeAnyAttribute &aa)
	{
		size_t iN = aa.size();
		_base::resize(iN); //Or?
		const daeElement *e = &aa.getElement();
		for(size_t i=0;i<iN;i++)
		{			
			daeAttribute2 &a = _base::operator[](i);
			a.name = aa[i]->getName();
			aa[i]->memoryToStringWRT(e,a.value);
		}
		return *this;
	}
};
/**UTILITY, LEGACY-SUPPORT
 * @see string pairs version of @c daeElement::getAttributes().
 */
typedef daeAttributes_size<10> daeAttributes; 

#include "../LINKAGE.HPP" //#define LINKAGE
  
template<class T, unsigned long long ULL>
/**
 * COLLADA C++ class that implements an instance of @c daeElement.
 *
 * Generators have the option of basing classes off @c daeElement
 * (COLLADA-DOM 2.x) or @c DAEP::Element (ColladaDOM 3) depending
 * on if @c daeElemental is inherited from, or @c DAEP::Elemental
 * is inherited from. 
 * @see @c DAEP::Elemental template in the DAEP.h header.
 * @note Generators must implement @c __DAEP__Object__v1__model()
 * for all classes based on @c daeElemental.
 *
 * @remarks There are significant wrinkles introduced by the Legacy
 * parameter. Initially the hope was that virtual-inheritance could
 * be used to share the data/virtual-layout, but it didn't work out.
 */
class daeElemental : public DAEP::Elemental<T,ULL,daeElement>
{
COLLADA_(public) //These may not work.
	/**
	 * Static version of @c daeObject::getObjectType().
	 */
	static int getObjectType(){ return getElementType(); }
	/**
	 * Static version of @c daeElement::getElementType().
	 */
	static int getElementType()
	{
		return T::elementType; 
		//This is just checking the generator's work.		
		daeCTC<T::elementType==T::__DAEP__Schema__genus>();
	}
};

/**ABSTRACT BASE-CLASS
 *
 * 2016 Changes
 * ============
 * 2.5 NOTES: Classes can only be indirectly based on @c daeElement
 * by way of @c daeElemental<T>, where "T" is the class itself.
 * Loaders should use @c daeMetaElement::pushBackWRT() to fill out
 * the element's contents.
 * 2.5 adds text-nodes (comments, etc.) but does not add APIs to this
 * legacy interface, because COLLADA itself doesn't really require it,
 * -and because @c getContents() is added and can be called on to get
 * at these new APIs; just as @c getContents().clear() can clear them.
 * End 2.5 NOTES
 * =============
 * The @c daeElement class represents an instance of a COLLADA "Element";
 * it is the main base class for the COLLADA DOM.
 * Features of this class include:
 * - Uses factory concepts defined via @c daeMetaElement.
 * - Composed of attributes, content elements and content values.
 * - Reference counted via daeSmartRef.
 * - Contains information for XML base URI, and XML containing element. 
 */
class daeElement : public daeObject
{	
	friend class daeDOM;
	friend class daeObject;
	friend class daeMetaElement;
	//DAEP Element DATA-MEMBERS & CONSTRUCTORS
	COLLADA_(protected)
	//VIRTUAL-INHERITANCE DOESN'T LINE UP WITH 
	//DAEP::Element, AND COULD ENTAIL OVERHEAD.
	DAEP::Element::__DAEP__Element__Data __DAEP__Element__data;
	char _reserved[daeSizeOf(DAEP::Element,__DAEP__Element__data_reserved)];
	//REPLICATING @c DAEP::Element CONSTRUCTOR
	daeElement()
	{ /*NOP*/ }	
	//REPLICATING @c DAEP::Element CONSTRUCTOR
	explicit daeElement(enum dae_clear):daeObject(dae_clear)
	{ /*NOP*/ }
	friend class DAEP::Object;
	friend class DAEP::Element;
	/** These ferry the document downstream (for @c daeDocument::typeLookup().) */
	void __atomize2(daeMeta&),__0();
	void __clear2(daeContents&,daeMeta&,const daeDocument*);
	COLLADA_(public)
	/**CIRCULAR-DEPENDENCY @c DAEP::Element::__COLLADA__atomize() is exported. */
	inline void __COLLADA__atomize(){ ((DAEP::Element*)this)->__COLLADA__atomize(); }

COLLADA_(public) //PORTABLE?

	template<class F, class G>
	/**EXPERIMENTAL
	 * Implements pseudo breadth-wise traversal.
	 */
	inline void for_each(F &f, G &g)
	{
		f(*this); getContents().for_each(f,g);
		g(*this); 
	}
	template<class F, class G>
	/**EXPERIMENTAL, CONST-PROPOGATING-FORM
	 * Implements pseudo breadth-wise traversal.
	 */
	inline void for_each(F &f, G &g)const
	{
		f(*this); getContents().for_each(f,g);
		g(*this); 
	}	

COLLADA_(public) //FACTORY
	/**
	 * Formerly "daeMeta::_construct_new."
	 * Works similarly to @c daeDOM::_addElement().
	 */
	LINKAGE daeElement *_addElement(daeMeta&);

COLLADA_(public) //non-SIDREF SID lookup

	#ifdef NDEBUG
	#error Probably a daeArray<daeSmartRef<T>> form of this is in order.
	#endif
	template<class S, class T> //T is DAEP::Element or daeElement based
	/**WARNING, LEGACY-SUPPORT
	 * @warning THIS IS LEGACY FUNCTIONALITY. SEE THE BODY OF @c daeDocument 
	 * FOR HOW TO ENABLE IT, AND HOW IT CAN FAIL IF id IS NOT A SMALL STRING.
	 *
	 * Looks up a breadth-first match for @a sid located under @c this element.
	 * @remark The 1.4.1 and 1.5.0 COLLADA specifications are inconsistent WRT
	 * to SID lookup. For example, <texture texture> isn't a SIDRE at all, and
	 * instead is simply the NCName of the "sid" attribute in <profile_COMMON>.
	 *
	 * @param sid The SID to match on.
	 * @return Returns @a match. Currently @c this element may or may not be
	 * able to be returned via @a match. If COLLADA or users require this it
	 * can be arranged. For now callers must not assume one way or the other.
	 */
	inline daeSmartRef<T> &sidLookup(const S &sid, daeSmartRef<T> &match, enum dae_clear clear=dae_clear)const
	{
		return sidLookup<S,T>(sid,match,const_daeDocRef(getDoc()),clear);
	}	
	template<class S, class T> //T is DAEP::Element or daeElement based
	/**WARNING, LEGACY-SUPPORT
	 * @warning See the fewer arguments form of this method's Doxygentation.
	 * This form lets the user avoid the other form's @c getDoc() procedure.
	 * @return Returns @c nullptr via @a match if @a doc==nullptr.
	 */
	inline daeSmartRef<T> &sidLookup(const S &sid, daeSmartRef<T> &match, const daeDoc *doc, enum dae_clear clear=dae_clear)const
	{
		(void)static_cast<const daeElement*>(dae((T*)nullptr)); //up/downcast
		if(clear!=dae_default) match = nullptr;
		_sidLookup(daeBoundaryStringRef(*this,sid),(daeElementRef&)match,doc); 		
		if(nullptr==dae(match)->template a<T>()) match = nullptr; return match;
	}
	template<class S, class T> //S is DAEP::Element or daeElement based
	/**WARNING, LEGACY-SUPPORT
	 * @warning See the 2 or 3 arguments form of this method's Doxygentation.
	 * This is a single-argument shorthand with @a S being explicitly stated.	 
	 */
	inline daeSmartRef<typename S::__COLLADA__T> &sidLookup(const T &sid,
	class undefined*_=0,const daeSmartRef<typename S::__COLLADA__T>&def=nullptr)const
	{
		return sidLookup(sid,const_cast<daeSmartRef<typename S::__COLLADA__T>&>(def));
	}
	/** Implements @c sidLookup(). @a ref is a @c daeStringRef. */
	LINKAGE void _sidLookup(daeString ref, daeElementRef&, const daeDoc*)const;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeElement)
	//SCHEDULED FOR REMOVAL?
	/** These sometimes comes in handy since no longer inheriting. */
	inline operator DAEP::Element*(){ return (DAEP::Element*)this; }	
	/** These sometimes comes in handy since no longer inheriting. */
	operator const DAEP::Element*()const{ return (DAEP::Element*)this; }
	/** These sometimes comes in handy since no longer inheriting. */
	inline operator DAEP::Element&(){ return *(DAEP::Element*)this; }	
	/** These sometimes comes in handy since no longer inheriting. */
	operator const DAEP::Element&()const{ return *(DAEP::Element*)this; }

COLLADA_(public) //2.5 daeSafeCast-like shorthand

	/**EXPERIMENTAL 
	 * @c daeSmartRef uses this to convert to @c xs::any.
	 * 24 is version 2.4, but really it just has to be 1
	 * less than @c DAEP::Element::__DAEP__Legacy.
	 */
	enum{ __DAEP__Legacy=24 };

	/**WORKAROUND
	 * This is now needed by all standard @c daeSmartRef classes since
	 * many templates use it to let smart-refs stand in for their type.	 
	 * (That allows consistent use of shorter WYSIWYG smart-ref names.)
	 */
	typedef daeElement __COLLADA__T;
	 
	template<class T> 
	/** Implements @c a() and @c an(). */
	inline typename T::__COLLADA__T *_a()const
	{
		if(daeObject::_this__nullptr(this)||daeUnsafe<T>(*this))
		return nullptr; return (typename T::__COLLADA__T*)this; 
	}

	template<class T> 
	/**WARNING, UNDEFINED-BEHAVIOR
	 * @warning 2.5 adds an @c assert() to @c daeSafeCast(). This was
	 * originally to be a shorter alternative, but eventually similar
	 * APIs were added to the other objects, which are designed to do
	 * filtering. For consistency the @c assert() function is removed.
	 *
	 * Here is an example; below "->" is pronounced "to."
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.C++}
	 * domCOLLADA *COLLADA = p->a<domCOLLADA>();
	 */	
	inline typename T::__COLLADA__T *a(){ return _a<T>(); }
	template<class T> 
	/**CONST-FORM 
	 * @see non-const overload's Doxygentation. 
	 */
	const typename T::__COLLADA__T *a()const{ return _a<T>(); }

	template<class T> 
	/**ALTERNATIVE-SPELLING
	 * Only because some words use "an" instead of "a" in English. 
	 * @see @c a().
	 */
	inline typename T::__COLLADA__T *an(){ return _a<T>(); }
	template<class T> 
	/**CONST-FORM, ALTERNATIVE-SPELLING
	 * Only because some words use "an" instead of "a" in English. 
	 * @see @c a().
	 */
	const typename T::__COLLADA__T *an()const{ return _a<T>(); }

COLLADA_(public) //BASIC PROPERTY ACCESSORS
	/**LEGACY
	 * @remarks This method predates 2.5, and so is repurposed.
	 * @return Returns the daeObjectType class's typeless enum.
	 */
	inline int getElementType()const{ return getMeta().getElementType(); }
	/**LEGACY
	 * Gets the COLLADA schema type name corresponding to @c getElementType().
	 */
	inline const daePseudonym &getTypeName()const{ return getMeta().getName(); }

	/**LEGACY
	 * Gets the associated Meta information for this element. This
	 * Meta also acts as a factory. See @c daeMetaElement Doxygentation for more
	 * information.
	 * @return Returns the associated meta information.
	 */
	inline daeMeta &getMeta()const{ return *__DAEP__Element__data.meta; }
	
	//Exposing to client on daeStringRef prototype-constructor's grounds.
	//Reminder: it's not just for efficiency. Since the prototypes don't
	//have a DOM parent, they only have DOM set to daeStringRef_protoDOM.
	//#ifdef BUILDING_COLLADA_DOM //INVISIBLE
	/**NEVER-NULL
	 * @see @c daeObject::getDOM().
	 */
	inline const daeDOM *getDOM()const{ return __DAEP__Element__data.DOM; }	
	//#endif
	/**CIRCULAR-DEPENDENCY
	 * This is newly added in case it's decided to store a copy of @c daeDOM::_database in
	 * every element, alongside a copy of the @c daeMetaElement pointer. They are constant
	 * for the entirety of the element's life.
	 *
	 * NOTE: BECAUSE OF _databaseRejoinder, IT SEEMS MORE LIKE A DOM POINTER WOULD BE USED.
	 * ALSO: A _DOM WOULD HAVE TO WORK TRANSPARENTLY VIS-A-VIS daeDocument::_PseudoElement.
	 */
	inline daeDatabase &getDatabase()const;	
	//{
	//	return getDOM()->getDatabase(); 
	//}

	/**LOW-LEVEL
	 * If @c 0==getMigrationByte() a database's change-notice handler
	 * is likely to be uniterested in this element.
	 * @c const_cast can be used to set the bit masks associated with
	 * this @c char sized byte. @c daeDocument::getPseudoElement() is
	 * the only instance in which doing so makes sense.
	 * @see @c daeDocument::getPseudoElement()
	 * SEE-ALSO @c daeMigrationByte
	 * SEE-ALSO @c getMigrationNibble_xml_attribute_masks()
	 * Bit #1 is @c getMigrationBit_definitely_is_childless() 
	 * Bit #2 is @c getMigrationBit_has_static_ID_attribute() 
	 * Bit #3 is @c getMigrationBit_has_nonstatic_attribute()
	 * Bit #5 is @c hasAttribute_xmlns()
	 * Bit #6 is @c hasAttribute_xml_base()
	 * Bit #7 is @c hasAttribute_xml_lang()
	 * Bit #8 is @c hasAttribute_xml_space_et_cetera()
	 */
	inline const unsigned char &getMigrationByte()const
	{
		return __DAEP__Element__data.element_migration_byte();
	}  
	/**LOW-LEVEL
	 * @see @c daeMigrationByte::CONTENT_ALLOCATION_UNIT
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 *
	 * As its wording suggests, it means the element cannot have content,
	 * -or that it cannot be a "graph." It doesn't mean that it is a graph.
	 * By knowing that it's not a graph, code can avoid accessing a metadata
	 * record, in order to access a contents-array, in order to determine that
	 * the array is empty or not.
	 *
	 * Very often elements are inserted into the document, and then their content
	 * is added afterward. So on the optimum route, this returns @c false. In isn't
	 * really important in the grand scheme of things.
	 */
	inline bool getMigrationBit_definitely_is_childless()const
	{
		return 0==__DAEP__Element__data.daeContents_capacity_is_nonzero;
	}
	/**LOW-LEVEL
	 * @see @c daeMigrationByte::HAS_STATIC_ID_ATTRIBUTE
	 * @return Returns @c true if the schema describes attributes that
	 * are marked by @c XS::Schema::getIDs().
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 */
	inline bool getMigrationBit_has_static_ID_attribute()const
	{
		return 1==__DAEP__Element__data.getMeta_getFirstID_is_nonzero;
	}
	/**LOW-LEVEL
	 * @see @c daeMigrationByte::HAS_NONSTATIC_ATTRIBUTE
	 * @return Returns @c true if @c getAttributes().grow() is entered.
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 */
	inline bool getMigrationBit_has_nonstatic_attribute()const
	{
		return 1==__DAEP__Element__data.daeAnyAttribute_copy_on_write;
	}
		
COLLADA_(public) //QNAME PROPERTIES

	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Try getNCName, getQNamePrefix, and/or setNamespace respectively.")
	typedef void getElementName,setElementName;
	#endif

	/**
	 * Gets @c getObjectTags() as @c daeElementTags type. Both
	 * of the first two tags are used to store and compare the
	 * XML namespace value and prefix.
	 */ 
	inline const daeTags getElementTags()const
	{
		return (daeTags&)_getObjectTags(); 
	};

	/**
	 * Tells if this element has a colonized QName prefix.
	 * @note Equal if @c a->getElementTags()==b->getElementTags() at system
	 * level (regardless of DOM membership.) 
	 */
	inline bool hasQName()const{ return 1!=getElementTags().nameTag; }

	/**
	 * Gets this element's colonized QName prefix.
	 * @note Equal if @c a->getElementTags()==b->getElementTags() at system
	 * level (regardless of DOM membership.) 
	 */
	inline daeStringRef_base getQNamePrefix()const{ return getElementTags().second(); }
				   	
	template<class I>
	/**WARNING
	 * @warning @c a.getNamespace()==b.getNamespace() is a safer comparison
	 * in cases of prefix overflow (Overflowing not currently implemented.)
	 * 
	 * Tells if @c cmp.namespaceTag is equal to @c this element's namespace.
	 */
	inline bool hasNamespaceTag(I cmp)const
	{
		return getElementTags().namespaceTag==(0xFF&cmp);
	}

	/**
	 * Get this element's namespace value.
	 * @return Returns a value that can be compared to determine namespace
	 * equality. The string is the value of the xmlns attributes.
	 * @note Equal if @c a->getElementTags()==b->getElementTags() at system
	 * level (regardless of DOM membership.) 
	 * Further note that @c daeElementTags::namespaceTag may differ even for
	 * matching vaules. String comparison is unnecessary.
	 */
	inline daeStringRef_base getNamespace()const{ return getElementTags().first(); }

	/**WARNING
	 * Sets the XMLNS namespace and QName prefix of this element.
	 * It's valid to assign any element's tags to any other tags.
	 * However, a well-formed document must have a corresponding
	 * xmlns attribute within the element's attributes or parent
	 * elements.
	 * @see @c migrateNamespace_for_each()
	 *
	 * @param gotETags is an @c int type underneath its annoying
	 * class requirement. It's disasterous to pass an unassigned
	 * value. Proceed with caution.
	 *
	 * @warning @c true==gotETags.found() is required when using
	 * @c findNamespace_xmlns() to construct @a gotETags. An XML
	 * prefix must appear in the element's attributes or parents
	 * or else the document is malformed. It can get complicated.
	 * Which is why it's not made as simple as assigning a value.
	 * There are many ways to approach namespace management, but
	 * most require situational knowledge of documents' contents.
	 * @see @c daeElementTags::assign() 
	 */
	inline void setNamespace(daeTags gotETags)
	{
	  //REMINDER: daeSmartTag can also set the namespace, with
	  //identical code. There's a BIG PROBLEM for schema-based
	  //elements if the namespaceTag does not match the schema.
	  //It may be that this is just a bad design. An assert on
	  //that case doesn't do much to help, since it can happen
	  //anywhere, in algorithms that might not be fully tested.
		
		//Note: 0 indicates findNamespace_xmlns mismatch.
		assert(gotETags.found());
		//Note: In x86 ideally this copies a 16-bit word.
		_getObjectTags().classTag = gotETags.namespaceTag;
		_getObjectTags().objectTag = gotETags.nameTag;		
	}

	/**
	 * Sets the namespace, and try to inject xmlns attributes.
	 * If attributes are required, @c resolveNamespace() will
	 * be used to scan the rest of the elements below @c this
	 * element subject to the same rules applied to insertion.
	 * @see @c resolveNamespace()
	 */
	inline bool setNamespace_and_resolve(daeTags gotETags)
	{
		setNamespace(gotETags);
		if(!migrateNamespace()) return false;
		else resolveNamespace_g(*this); return true;
	}
	
	/**MUTABLE
	 * Updates namespace using pseudo breadth-first recursion.
	 * @c daePlatform::resolveXMLNS needs to update one shell
	 * at a time.
	 * MORE SPECIFICALLY this recurses over the element graph
	 * repairing any issues with namespace (XMLNS) assignment.
	 *
	 * @c setNamespace_and_resolve() conditions resolution on
	 * its single reassignment, whereas @c resolveNamespace()
	 * is more like a deferred clean up operation in contrast.
	 */
	inline void resolveNamespace()const 
	{
		for_each(resolveNamespace_f,resolveNamespace_g);
	}
	/**FUNCTOR @see resolveNamespace() implementation. */
	static void resolveNamespace_f(const daeElement &ea)
	{
		ea.migrateNamespace();
	}
	/**FUNCTOR @see resolveNamespace() implementation. */
	static void resolveNamespace_g(const daeElement &ea)
	{
		if(!ea.getMigrationBit_definitely_is_childless())
		ea.getContents().for_each(resolveNamespace_f,resolveNamespace_g);
	}
	
	/**WARNING, MUTABLE
	 * @warning This procedure injects XMLNS attributes into a
	 * document that are not strictly local to @c this element.
	 * It's @c const because it's just repairing a document to
	 * better reflect its state and to simplify change-notices.
	 *
	 * Databases must call this method in response to elements
	 * change-notices. It looks for xmlns attributes to assign
	 * to @c this element, and creates xmlns attributes on the
	 * fly as required. If databases do not do this then users
	 * cannot rely on @c daePlatform::introduceToDocument() to
	 * work for inserted elements, since it's not performed in 
	 * advance so it's not double-duty.
	 *
	 * @param cond If @c true the exported algorithm is called.
	 * If @c false the @c inline code is a no-op. If "neither"
	 * it's a no-op if @c this element's tags match its parent.
	 * In that case an xmlns attribute is assumed to exist for
	 * the parent's matching namespace.
	 *
	 * ALGORITHM
	 * When @a cond enters the exported code, the algorithm is
	 * as follows:
	 * 1) Code very similar to @c findQNamePrefix_xmlns() will 
	 * change the prefix if possible. 
	 * 2) In the event that no XMLNS exists then the prefix is
	 * is used as the basis for a new XMLNS. The DAEP operator
	 * based insertion elects the namespace used by the schema.
	 * Other methods require users provide the prefixe to them.
	 * @c daePlatform::resolveXMLNS() involves the user in the
	 * process, and @c daePlatform::introduceToDocument() adds
	 * new schemata to the document.
	 *
	 * IMPORTANT
	 * @return Returns @c true if an xmlns is added. Migration
	 * should ignore this. @c resolveNamespace() ignores it as
	 * well, whereas @c setNamespace_and_resolve() resolves if
	 * @c true.
	 */
	inline bool migrateNamespace(dae3ool cond=dae3ool::is_neither)const
	{
		if(cond.state==cond.is||cond.state==cond.is_neither
		&&getObjectTags()!=getParentObject().getObjectTags())
		return _migrateNamespace2(); return false;
	}
	//RISKY
	//__declspec(noalias) doesn't compile at the local call site.
	//Technically this isn't no-alias, but I think it's unlikely
	//code that uses it (indirectly) will also access attributes.
	COLLADA_NOALIAS
	/** Implements @c migrateNamespace(). */
	LINKAGE bool _migrateNamespace2()const;	

	/**WARNING
	 * Find the namespace/prefix according to xmlns naming rules.
	 * @return Use @c daeElementTags::found() to check for if an
	 * attribute matching @a prefix was found. If @c false it is
	 * important to not assign the value using @c setNamespace().
	 *
	 * @warning On the face this method looks almost useless, as
	 * it returns precious little details, and allows no control
	 * over the search. It's mainly for reading documents, since
	 * an element cannot be instantiated without looking for the
	 * xmlns element among its attributes to determine its class.
	 * Instead of taking additional parameters, etc. users might
	 * better implement their own case-specific search procedure.
	 * THOUGH EXPORTED, THIS METHOD DOESN'T DO ANYTHING ORIGINAL.
	 */
	LINKAGE daeTags findNamespace_xmlns(const daeStringRef &prefix=daeStringRef())const;

	/**WARNING
	 * @warning Don't use this as a substitute for finding xmlns
	 * attributes. It stops searching if it hits an element with
	 * @c io.namespaceTag. That element mightn be @c this element.
	 *
	 * Does reverse of @c findNamespace_xmlns().
	 * @return Returns @c true if @c io.nameTag is returned with
	 * a prefix corresponding to the inputted @c io.namespaceTag.
	 */
	LINKAGE bool findQNamePrefix_xmlns(daeTags &io)const;
	 
	/**WARNING, LEGACY-SUPPORT
	 * Gets this element's NCName.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some child names are NCNames with a colon, or a pseudo QName.
	 * @return Returns the @c daePseudonym for the NCName.
	 * @remarks @c getTypeName() gets the COLLADA schema type name
	 * @warning A STRING-LITERAL OR PERMA-STRING MUST BE USED TO SET THIS.
	 */
	inline daeName &getNCName(){ return __DAEP__Element__data.NCName; }
	/**CONST-FORM, LEGACY-SUPPORT
	 * Gets this element's NCName.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some child names are NCNames with a colon, or a pseudo QName.
	 * @return Returns the @c daePseudonym for the NCName.
	 * @remarks @c getTypeName() gets the COLLADA schema type name
	 * Use caution when using this function since you can easily create invalid documents.
	 */
	inline const daeName &getNCName()const{ return __DAEP__Element__data.NCName; }	
	 
COLLADA_(public) //LEGACY GRAPH CONSTRUCTION METHODS

	template<class U> //See daeContents::__cursorize().
	/**WARNING, LEGACY, OVERLOAD 
	 * Adds @a child to @c this element.
	 *
	 * @remarks This API had a cursor-like argument before. It wasn't
	 * really appropriate, since it only applied to the old contents-
	 * arrays, which not every element had.	In any case, it's removed.
	 *
	 * @warning @a child CANNOT BE A NULLPTR-PLACEHOLDER NOR NULLPTR-POINTER.
	 */
	inline daeElement *add(const U &child)
	{
		return getMeta().placeWRT(this,child)>=DAE_OK?child:nullptr; 
	}
	/**LEGACY, OVERLOAD
	 * @param list_of_names is a space separated list of nested child 
	 * names. For example: @c COLLADA->add("asset contributor");
	 * @note The library doesn't (yet) support QNames properly.
	 * Some children names are NCNames with a colon, or a pseudo QName.
	 *
	 * @note This is the pre-2.5 way. It's not C++ style, because a
	 * string is weakly typed. It's better to assign directly to the 
	 * arrays when possible. The "ColladaDOM 3" generator will output
	 * prefix/suffix free member names.
	 * ColladaDOM 3 alternative: @c +COLLADA->asset->contributor.
	 *
	 * @remarks This API had a cursor-like argument before. It wasn't
	 * really appropriate, since it only applied to the old contents-
	 * arrays, which not every element had.	In any case, it's removed.
	 */
	LINKAGE daeElement *add(daeString list_of_names);	

	template<class S, class T, class U> //See daeContents::__cursorize().
	/**WARNING, LEGACY-SUPPORT
	 *
	 * Adds @a child to @c this element, not before @a after, nor after @c before.
	 *
	 * @warning Care must be taken with @a before and @a after so not
	 * to shift the contents-array as a side-effect of obtaining them.
	 * @warning <xs:all> returns @c DAE_ORDER_IS_NOT_PRESERVED, which 
	 * appears to be a success as viewed by this API.
	 *
	 * @warning @a child CANNOT BE A NULLPTR-PLACEHOLDER NOR NULLPTR-POINTER.
	 */
	inline daeElement *addBetween(const S &after, const U &child, const T &before)
	{
		return getMeta().placeBetweenWRT(this,after,child,before)>=DAE_OK?child:nullptr; 
	}

	template<class T, class U> //See daeContents::__cursorize().
	/**WARNING, LEGACY 
	 *
	 * IF IMPLEMENTING A LOADER, 2.5 ADDS @daeMeta::pushBackWRT().
	 *
	 * Adds @a child to @c this element, not before @a after.
	 *
	 * @warning <xs:all> returns @c DAE_ORDER_IS_NOT_PRESERVED, which 
	 * appears to be a success as viewed by this API.	 
	 *
	 * @warning @a child CANNOT BE A NULLPTR-PLACEHOLDER NOR NULLPTR-POINTER. 
	 */
	inline daeElement *addAfter(const U &child, const T &after)
	{
		return getMeta().placeAfterWRT(after,this,child)>=DAE_OK?child:nullptr;
	}

	template<class T, class U> //See daeContents::__cursorize().
	/**WARNING, LEGACY
	 * Adds @a child to @c this element, not after @a before.
	 *
	 * @warning <xs:all> returns @c DAE_ORDER_IS_NOT_PRESERVED, which 
	 * appears to be a success as viewed by this API.
	 *
	 * @warning @a child CANNOT BE A NULLPTR-PLACEHOLDER NOR NULLPTR-POINTER. 
	 */
	inline daeElement *addBefore(const U &child, const T &before)
	{
		return getMeta().placeBeforeWRT(before,this,child)>=DAE_OK?child:nullptr;
	}

	template<class U> //See daeContents::__cursorize().
	/**WARNING, LEGACY
	 * Removes the @a element from its parent, the @c this element.
	 * @param element Element to be removed in the @c this container.
	 *
	 * @return Returns @c DAE_ORDER_IS_NOT_PRESERVED or @c DAE_OK if successful.
	 * @warning THIS API WILL ACCEPT NULLPTR-PLACEHOLDERS, AND REMOVE THEM FROM
	 * THE CONTENTS-ARRAY; STILL THIS PRACTICE IS DISCOURAGED FOR ITS ASYMMETRY.
	 *
	 * @remarks The library does not currently envision constrained removals of
	 * <xs:choice> based children. This API is unconstrained in any case. Where
	 * code expects a @c bool return type, it will report @c false if reordered.
	 */
	inline daeOK removeChildElement(const U &element)
	{
		return getMeta().removeWRT(this,element);
	}

	template<class U> //See daeContents::__cursorize().
	/**LEGACY
	 * Removes @a element element from its parent element.
	 * The API finds the parent itself, and is a @c static method,
	 * since removing the element from its parent may result in its deletion.
	 * (IT'S NOT CLEAR WHAT'S DELETED. Pre-2.5 childless groups were deleted.)
	 * If the element has no parent, nothing is done.
	 *
	 * @return Returns @c DAE_ORDER_IS_NOT_PRESERVED or @c DAE_OK if successful.
	 *
	 * @remarks The library does not currently envision constrained removals of
	 * <xs:choice> based children. This API is unconstrained in any case. Where
	 * code expects a @c bool return type, it will report @c false if reordered.
	 */
	static daeOK removeFromParent(const U &element)
	{
		if(const daeElement*p=element->getParentElement()) 
		return const_cast<daeElement*>(p)->removeChildElement(element); 
		return DAE_ERROR;
	}

COLLADA_(public) //NEW "blind faith" ACCESSORS	
	/**WARNING
	 * @warning The return string does not partake in reference count
	 * tracking, and so should not be held onto to for long. Use this
	 * at your own risk.
	 *
	 * Gets this element's value IF-AND-ONLY-IF its underlying binary
	 * representation matches that of xs:string.
	 * @return Returns an empty string if the value is a number or an
	 * other kind of non-string data.
	 * @note This avoids copying the string into a new buffer if code
	 * is highly confident about the value's underlying type, or does
	 * not care if it is not a string.
	 */
	inline daeHashString string()const
	{
		return _string(getCharData());
	}
	template<class I>
	/**WARNING
	 * @warning The return string does not partake in reference count
	 * tracking, and so should not be held onto to for long. Use this
	 * at your own risk.
	 *
	 * Gets an attribute's value IF-AND-ONLY-IF its underlying binary
	 * representation matches that of xs:string.
	 * @return Returns an empty string if the value is a number or an
	 * other kind of non-string data.
	 * @note This avoids copying the string into a new buffer if code
	 * is highly confident about the value's underlying type, or does
	 * not care if it is not a string.
	 */
	inline daeHashString string(const I &i_or_Name)const
	{
		return _string(getAttribute(i_or_Name));
	}
	/** Implements @c string(). */
	inline daeHashString _string(const daeData &d)const
	{
		if(d->getTypeWRT(this).hasStringType())
		return (const daeStringRef&)d.getWRT(this); 		
		return daeHashString(empty_daeString1,0);
	}	
	
COLLADA_(public) //TEMPORARY "glimpse" (red flag) ACCESSORS
	
	/**WARNING, LEGACY-SUPPORT, OVERLOAD
	 * Formerly "getCharData."
	 * Gets simple-content in the form of a TEMPORARY C++ object.
	 * @warning THE ARRAY IS DESTRUCTED AT THE END OF THE CALL'S SCOPE!!!
	 * @see @c string().
	 */
	inline daeCharData &glimpse(const daeCharData&def=daeCharData())const
	{
		return getCharData(const_cast<daeCharData&>(def));
	}
	template<int N>
	/**WARNING, LEGACY-SUPPORT, OVERLOAD
	 * Gets simple-content in the form of a TEMPORARY C++ object.
	 * @warning THE ARRAY IS DESTRUCTED AT THE END OF THE CALL'S SCOPE!!!	 
	 * @see @c string().
	 */
	inline daeCharData &glimpse(const daeCharData_size<N>&def=daeCharData_size<N>())const
	{
		return getCharData(const_cast<daeCharData&>(def));
	}
	template<class I>
	/**WARNING, LEGACY-SUPPORT, OVERLOAD
	 * Formerly "getAttribute."
	 * Finds attribute value in the form of a TEMPORARY C++ object.
	 * @warning THE ARRAY IS DESTRUCTED AT THE END OF THE CALL'S SCOPE!!!
	 * @see @c string().
	 */
	inline daeCharData &glimpse(const I &i_or_Name, const daeCharData&def=daeCharData())const
	{
		return getAttribute<I>(i_or_Name,const_cast<daeCharData&>(def));
	}
	template<int N, class I>
	/**WARNING, LEGACY-SUPPORT, OVERLOAD
	 * Finds attribute value in the form of a TEMPORARY C++ object.
	 * @warning THE ARRAY IS DESTRUCTED AT THE END OF THE CALL'S SCOPE!!!
	 * @see @c string().
	 */
	inline daeCharData &glimpse(const I &i_or_Name, const daeCharData_size<N>&def=daeCharData_size<N>())const
	{
		return getAttribute<I>(i_or_Name,const_cast<daeCharData&>(def));
	}

COLLADA_(public) //NON-STATIC (careful) ATTRIBUTE ACCESSORS
	
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	These APIs were unclear about their purpose WRT domAny\n\
	Try getAttribute, findAttribute, and getCharData respectively.")
	typedef void getAttributeObject,hasAttribute,getCharDataObject;
	#endif

	template<class I>
	/**WARNING, OVERLOAD
	 * Formerly "GetAttributeObject."	 
	 * @warning Creates an attribute if one does not already
	 * exist. 
	 * Creates an attribute if one does not already
	 * exist. 
	 * @return Returns created attribute. 
	 * @note All elements can create any attribute, whether it
	 * is in their schema or not. So be careful. ALSO NOTE THAT
	 * THIS IS BY DESIGN.
	 * @see 2-argument form of @c getAttribute(). It will not
	 * make attributes, but nothing suggests that it would.
	 * @see @c findAttribute().
	 */
	inline daeData &getAttribute(const I &i_or_Name)
	{
		return *getAttributes()[i_or_Name];
	}
	template<class I>
	/**CONST-FORM, OVERLOAD
	 * Formerly "GetAttributeObject."
	 * Gets a @c daeAtomicType::VOID attribute if
	 * the attribute does not exist.
	 * @return Returns a @c daeAtomicType::VOID attribute if
	 * the attribute does not exist.
	 * @warning Returns a @c daeAtomicType::VOID attribute if
	 * the attribute does not exist.
	 *
	 * @remark This API is a special const-form that supposes
	 * if @c this element is @c const, that there's no desire
	 * to add attributes where they do not exist. Instead the
	 * returned attribute is a read-only data-type that won't
	 * have a name matching @a i_or_Name.
	 */
	inline daeData &getAttribute(const I &i_or_Name)const
	{
		return *getAttributes()._maybe_get(i_or_Name);
	}

	template<class I>
	/**WARNING, LEGACY-SUPPORT
	 * @warning Creates an attribute if one does not already
	 * exist. 
	 * Creates an attribute, returning its position in the attributes
	 * array.	 
	 * @see 2-argument form of @c findAttribute() if creation
	 * is undesired.
	 *
	 * @remark The const-form of this API does not create an attribute
	 * but instead returns an index equal to the size of the attribute
	 * array, where a @c daeAtomicType::VOID attribute lives.
	 */
	inline size_t getAttributeIndex(const I &i_or_Name)
	{
		return &getAttributes()[i_or_Name]-getAttributes().cbegin();
	}
	template<class I>
	/**CONST-FORM, LEGACY-SUPPORT
	 * Returns the attribute-array index matching @c i_or_Name, unless
	 * the attribute does not exist, in which case a special read-only
	 * attribute index that is equal to attribute-array's @c size().
	 * @see @c findAttribute()
	 */
	inline size_t getAttributeIndex(const I &i_or_Name)const
	{
		return &getAttributes()._maybe_get(i_or_Name)-getAttributes().cbegin();
	}

	template<class I>
	/**
	 * Sets attribute to @c "" and clears write-mask equvialent to
	 * @c getAttributeMask()=false;
	 */
	inline daeOK removeAttribute(const I &i_or_Name)
	{
		//REMINDER: nullptr sets nonstatics to daeAnySimpleType_void.
		size_t i = getAttributeIndex(i_or_Name);
		getAttributes().mask()[i] = false;
		return getAttributes()[i]->stringToMemoryWRT(this,nullptr,0);
	}	

	template<class I, class T>
	/**WARNING, LEGACY, OVERLOAD
	 * @warning @c nullptr==value will accidentally remove nonstatic 
	 * attributes. The write-mask will not be incorrect as nonstatic
	 * attributes do not have one.
	 *
	 * Sets the attribute to the specified value.
	 * Sets the attribute's mask.
	 * @param i_or_Name Name or index of the set attribute.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some attribute names are NCNames with a colon, or a pseudo QName.
	 * @param value Value to apply to the attribute.
	 * @return Returns true if the attribute was found and the value was set, false otherwise.
	 */
	inline daeOK setAttribute(const I &i_or_Name, const T &value)
	{
		size_t i = getAttributeIndex(i_or_Name);
		getAttributes().mask()[i] = true;
		return getAttributes()[i]->stringToMemoryWRT(this,value);
	}
	template<class I, class T, class U>
	/**WARNING, LEGACY-SUPPORT, OVERLOAD
	 * @warning @c nullptr==value will accidentally remove nonstatic 
	 * attributes. The write-mask will not be incorrect as nonstatic
	 * attributes do not have one.
	 *
	 * Sets the attribute to the specified value.
	 * Sets the attribute's mask.
	 * @param i_or_Name Name or index of the set attribute.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some attribute names are NCNames with a colon, or a pseudo QName.
	 * @param value Value to apply to the attribute.
	 * @return Returns true if the attribute was found and the value was set, false otherwise.
	 */
	inline daeOK setAttribute(const I &i_or_Name, const T &value, U len_or_end)
	{
		size_t i = getAttributeIndex(i_or_Name);
		getAttributes().mask()[i] = true;
		return getAttributes()[i]->stringToMemoryWRT(this,value,len_or_end);
	}	

	/**OPTIMIZATION
	 * Sets/unsets an XMLNS attribute.
	 * @param value if @c value==nullptr the attribute is removed.
	 * As this is a special API a "removeAttribute_xmlns" doesn't
	 * exist to complement this behavior.
	 */
	LINKAGE daeOK setAttribute_xmlns(const daeName &prefix, const daeName &value=nullptr);
	/**LOW-LEVEL OPTIMIZATION
	 * Sets/unsets an XMLNS attribute.
	 * @param value if @c value==dae_clear the attribute is removed.
	 * As this is a special API a "removeAttribute_xmlns" doesn't
	 * exist to complement this behavior.
	 */
	LINKAGE daeOK setAttribute_xmlns(daeStringRef_base prefix, daeStringRef_base value=dae_clear);

COLLADA_(public) //STATIC ATTRIBUTE ACCESSORS  
	/**OVERLOAD
	 * Gets <xs:anyAttributes> map, including static attributes.
	 * All elements may have any-like attributes.
	 * @note This works on a copy-on-write basis.
	 */
	inline daeAnyAttribute &getAttributes()const
	{
		return *(daeAnyAttribute*)&this[1]; 
	}
	/**LEGACY-SUPPORT, OVERLOAD
	 * Lets users know @c daeAttributes is availabe to them.
	 * @remarks Identical to (@a attrs=this->getAttributes()).
	 * @c getAttributes() in the form of key/value string pairs. 
	 */
	inline daeAttributes &getAttributes(daeAttributes &attrs)const
	{
		return attrs = getAttributes();
	}	

	/**LEGACY-SUPPORT
	 * Formerly "getAttributeCount."
	 * Returns the number of attributes in this element.
	 * @return The number of attributes this element has.
	 * @remarks Returns the highest getAttributeIndex + 1.
	 */
	inline size_t getAttributesCount()const{ return getAttributes().size(); }
	
	template<class I>
	/**WARNING, MUTABLE
	 * Formerly "isAttributeSet."	 
	 * Gets a hint optionally set by I/O plugins that tells if an 
	 * attribute is defined by a document, as opposed to defaults.
	 * Historically the built-in plugins do not set bits whenever
	 * schemata supply default values. This removes defaults from
	 * a document on round-trip. 2.5 CHANGES THIS BEHAVIOR by way
	 * of "option_to_not_retain_default_attributes" which is able
	 * to be controlled with @c daePlatform::getLegacy().
	 *
	 * @remarks The mask controls what attributes are written out
	 * with regard to I/O operations. A set bit forces attributes
	 * to be written, unless empty. Ultimately it is up to an I/O
	 * plugin to interpret the mask, and/or set the mask up as it
	 * loads documents into its DOM.

	 * @warning This feature is subject to compile-time disabling.
	 * In which case the result is always @c false.
	 * @see @c daeAnyAttribute::Mask Doxygentation.
	 */
	inline const daeBit getAttributeMask(const I &i_or_Name)const
	{
		return getAttributes().mask()[getAttributeIndex(i_or_Name)];
	}	

	/**OVERLOAD
	 * Finds the attribute without adding it to the attributes array.
	 * @return Returns @c nullptr if @c name is not found.
	 */
	inline daeData *findAttribute(const daeName &name)const
	{
		daeAnyAttribute &aa = getAttributes();
		for(size_t i=0;i<aa.size();i++)
		if(name==aa[i]->getName()) return aa[i]; return nullptr;
	}
	/**OVERLOAD
	 * Finds the attribute without adding it to the attributes array.
	 * @return Returns a @c bool value to be more orthodox/avoid the
	 * need to do @c nullptr checks, since @a i returns an index and
	 * the other overload returns a @c daeData pointer. For instance
	 * this overload is for use with @c getAttribute().
	 */
	inline bool findAttribute(const daeName &name, size_t &i)const
	{
		daeAnyAttribute &aa = getAttributes();
		for(i=0;i<aa.size();i++) 
		if(name==aa[i]->getName()) return true; return false;
	}	
	
	/**LEGACY
	 * Returns the name of the attribute at the specified position.
	 * @param i The index of the attribute whose name should be retrieved.
	 * @return Returns the name of the attribute, or "" if the index is out of range.
	 */
	inline daePseudonym getAttributeName(size_t i)const
	{
		daeAnyAttribute &aa = getAttributes();
		return aa.size()<=i?empty_daeString1:aa[i]->getName();
	}
		
	/**LEGACY-SUPPORT
	 * Formerly "getID."
	 * Gets the element "id" if it exists.
	 * @return Returns @c nullptr if the "id" attribute does not exist.
	 * @remarks A pointer is returned (versus a view) because often ID
	 * strings are compared by the value of the pointer instead of the
	 * content of the string (and printing C strings is pretty trivial.)
	 */
	NOALIAS_LINKAGE daeString getID_id()const
	SNIPPET
	(
		daeAttribute *a = getMeta().getID_id();
		return a==nullptr?nullptr:(const daeString&)a->getWRT(&*this); 
	)
	
	template<class I>
	/**LEGACY-SUPPORT, OVERLOAD
	 * @return Returns @a val as @c daeCharData C++ reference.
	 * @param i_or_Name Attribute's name or position to get.
	 * @note Positions must not be out-of-bounds. Names don't
	 * have to exist. In which case, they yield empty strings.
	 *
	 * @note The library doesn't (yet) support QNames properly.
	 * Some attribute names are NCNames with a colon, or a pseudo QName.
	 * @param A string in which to store the value of the attribute. 
	 * This will be set to "" if this element doesn't have the specified attribute.
	 *
	 * @see @c glimpse() for a simpler, yet dangerous alternative.
	 * @see 0-argument form of @c getAttribute() for a completely
	 * different kind of API from this one.
	 */
	inline daeCharData &getAttribute(const I &i_or_Name, daeArray<> &val)const
	{
		getAttributes()._maybe_get(i_or_Name)->memoryToStringWRT(this,val);
		return *(daeCharData*)&val;
	}
	
COLLADA_(public) //xml/xmlns lookup/methods (WORK-IN-PROGRESS)
	/**
	 * @see @c daeMigrationByte::XMLNS
	 * @return Returns @c true if xmlns attributes are present and/or
	 * have been defined. If a user defined prefix is defined it must
	 * be a nonstatic attribute, that may not count until it is first
	 * assigned a value. This behavior is an optimization. 
	 *
	 * @remark This method checks a flag. It's provided for optimally
	 * backtracing parents to better locate scoped namespace prefixes.
	 */
	inline bool hasAttribute_xmlns()const
	{
		return 0!=(1&__DAEP__Element__data.xml_qualified_attribute_masks);
	}
	/**TODO
	 * Tells if this element holds a base attribute/value. It isn't
	 * necessarily an xml:base attribute. 
	 * @see @c daeMigrationByte::XML_BASE
	 * @return Returns @c true if the xml:base attribute is present.
	 *
	 * @todo Add callback to @c daePlatform to retrieve a base when
	 * xml:base is not present. Databases must raise this flag, and
	 * the platform must interpret the element.
	 */
	inline bool hasAttribute_xml_base()const
	{
		return 0!=(2&__DAEP__Element__data.xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_LANG
	 * @return Returns @c true if the xml:lang attribute is present.
	 */
	inline bool hasAttribute_xml_lang()const
	{
		return 0!=(4&__DAEP__Element__data.xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_ATTRIBUTE_UNCOUNTED
	 * @return Returns @c true if there is an xml qualified attribute 
	 * present that is neither xml:base nor xml:lang. xml:space maybe.
	 */
	inline bool hasAttribute_xml_space_et_cetera()const
	{
		return 0!=(8&__DAEP__Element__data.xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_ATTRIBUTE_NIBBLE
	 * @return Returns the internal 4-bitfield corresponding to flags
	 * for xml qualified attributes.
	 */
	inline unsigned int getMigrationNibble_xml_attribute_masks()const
	{
		return __DAEP__Element__data.xml_qualified_attribute_masks;
	}

COLLADA_(public) //CONTENT ACCESSORS	
	/**LEGACY
	 * @return Returns @c true if this element is supposed to have character data.
	 * @note Since 2.5 elements that shouldn't have data are allowed to, in order
	 * to facilitate lossless processing in the face of errors or unorthodox use.
	 * @note A mixed-text (with inline elements) content-model says NOT @c true.
	 */
	inline bool hasCharData()const
	{
		return daeContentModel::SIMPLE==getMeta().getContentModel(); 
	}

	/**OVERLOAD
	 * Formerly "getCharDataObject."
	 * (Changed the name since "getAttributeObject" is removed, and "object" suggests
	 * that it is somehow related to @c daeObject.)
	 *
	 * Returns the @c daeData object corresponding to the character data for this element.
	 * @return No longer returns @c nullptr. The valueless type is @c daeAtomicType::VOID.
	 * @see hasCharData().
	 */
	inline daeData &getCharData()const
	{
		//NOTE: Can't return daeValue because daeData is a base class, and is not
		//const, and so loses-qualifiers as base conversions cannot be overloaded.
		return const_cast<daeDefault&>(getMeta().getValue()); 
	}
	/**LEGACY-SUPPORT, OVERLOAD
	 * @return Returns @a value.
	 * @param data The string to be filled with this element's character content. The
	 * string is set to an empty string if this element can't have character data.
	 */
	inline daeCharData &getCharData(daeArray<> &value)const
	{
		getCharData().memoryToStringWRT(this,value); return *(daeCharData*)&value;
	}

	template<class T>
	/**LEGACY
	 * Sets this element's character data.
	 * @param value The new character data of this element.
	 * @return Returns true if this element can have character data and the character data
	 * was successfully changed, false otherwise.
	 */
	inline daeOK setCharData(const T &value)
	{
		//TODO: Should work even if !hasCharData().
		return getCharData().stringToMemoryWRT(this,value);
	}
	template<class T, class U>
	/**LEGACY-SUPPORT
	 * Sets this element's character data.
	 * @param value The new character data of this element.
	 * @return Returns true if this element can have character data and the character data
	 * was successfully changed, false otherwise.
	 */
	inline daeOK setCharData(const T &value, U len_or_end)
	{
		//TODO: Should work even if !hasCharData().
		return getCharData().stringToMemoryWRT(this,value,len_or_end);
	}

	/**LEGACY
	 * Gets the container element for @c this element.
	 * If @c createAndPlace() was used to create the element, its parent is the the caller of @c createAndPlace().
	 * @return Returns the parent element, if @c this is not the top level element.
	 * @remarks Post-2.5 returns @c const types for upstream "getter" APIs. 
	 */
	inline const daeElement *getParentElement()const 
	{
		const daeElement &c = static_cast<const daeElement&>(getParentObject());		
		return c._isDoc_or_DOM()?nullptr:&c; 
	}	
	
	/**INTERNAL
	 * Implements @c getContents().grow(). It's needed to adjust the iterator.
	 */
	LINKAGE void __grow(daeArray<daeContent> &contents_of_this, size_t minCapacity);
	/**INTERNAL
	 * Implements @c getContents().clear().
	 * @remarks "e->__clear(e->getContents())" is technically
	 * more efficent, but stylistically, getContents().clear() is recommended.	 
	 */
	LINKAGE void __clear(daeArray<daeContent> &contents_of_this)
	SNIPPET( __clear2(static_cast<daeContents&>(contents_of_this),getMeta(),getDocument()); )
	
	/**LEGACY-SUPPORT
	 * Gets the contents-array.
	 * @remarks Pre-2.5 not all elements had contents-arrays.
	 */
	inline daeContents &getContents(){ return getMeta().getContentsWRT(this); }
	/**CONST-FORM, LEGACY-SUPPORT
	 * Gets the contents-array.
	 * @remarks Pre-2.5 not all elements had contents-arrays.
	 */
	inline const daeContents &getContents()const{ return getMeta().getContentsWRT(this); }		

	/**LEGACY-SUPPORT
	 * Tells if the element is part of a document, as opposed to simply having 
	 * a @c daeDocument object parent.
	 */
	inline bool isContent()const{ return 1==__DAEP__Element__data.is_content; }

	/**WARNING, LEGACY-SUPPORT
	 * Gets the container document for @c this element; or @c nullptr.
	 * @see @c getProvisionalDocument()
	 * @return Returns a @c const pointer. Post-2.5 upstream pointers are @c const.
	 * @warning This is not @c getDoc(). If @c isContent()==false, it's @c nullptr.
	 * @remarks I think going up the chain is better if a smart-ref is keeping the
	 * element alive until its document is closed or it's removed from the picture.
	 */
	inline const daeDocument *getDocument()const
	{
		const daeElement *p = this; return _getDocument2(p);
	}
	/**EXPERIMENTAL
	 * Unlike @c getDocument() this method will not return @c nullptr.
	 * If @c this element is not part of a document, in this case one
	 * will be created for it in the form of an inaccessible document.
	 *
	 * @remark Added to support @c daePlatform::introduceToDocument().
	 * It is also providing a never-null solution to @c getDocument().
	 * @c daeDoc::hasOwnURI() and @c daeDOM::getWorkingMemory() offer
	 * tools for working with documents that don't have URI semantics.
	 */
	inline const daeDocument &getProvisionalDocument()const
	{
		const daeElement *p = this; 
		const daeDocument *d = _getDocument2(p);
		if(d==nullptr) d = &p->_provisionalDocument(); return *d;
	}
	/**
	 * Implements @c getProvisionalDocument(). 
	 * @note Always makes document unless @c this is a pseudo-element.
	 * @see @c daeDOM::getWorkingMemory_new().
	 */
	LINKAGE const daeDocument &_provisionalDocument()const;
	/**
	 * Implements @c getDocument() & @c getProvisionalDocument(). 
	 */
	static const daeDocument *_getDocument2(const daeElement* &p)
	{
		for(;;) if(p->isContent())
		{		
			//Covers built-in daeDoc based object types.
			const daeObject *o = &p->getParentObject();
			if(o->_isDoc_or_DOM())
			{
				//NOT THREAD-SAFE
				//isContent() should preclude DOM.
				assert(o->_isDoc());
				return (const daeDocument*)o;
			}			
			(const daeObject*&)p = o;
		}
		else return nullptr;
	}	
			
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Use getBaseURI to implement xml:base, or getDocument()->getDocURI().")
	void getDocumentURI()const;
	#endif
	/**
	 * Gets the xml:base like URI of the virtual document that is the
	 * URI of @c this element's base-URI.
	 *
	 * @note It's not a misnomer to call this a "document URI" as the
	 * concept of "same document references" applies to xml:base just
	 * as to the actual document.
	 *
	 *   http://lists.xml.org/archives/xml-dev/201705/msg00017.html
	 *
	 * @return Returns @c nullptr if @c this element's parent isn't a
	 * document.
	 */
	NOALIAS_LINKAGE const_daeURIRef getBaseURI()const;
		
COLLADA_(public) //LEGACY CLONE API

	/**LEGACY-SUPPORT */
	struct clone_Suffix : daeArray<daeStringCP,96>
	{
		clone_Suffix():id(nullptr,0),name(nullptr,0){}
		daeName id,name; COLLADA_NOINLINE void _suffix(daeElement*);		
	};

	//TODO? THESE AREN'T ISSUING ELEMENT-LEVEL
	//CHANGE-NOTICES. THE RETURNED CLONE ISN'T
	//IN A DOCUMENT. ONE IS ISSUED IF INSERTED.	
	/**
	 * Implements @c clone() family of overloads. 
	 */
	inline void _clone2(daeElement&,clone_Suffix*)const;
	/**LEGACY-SUPPORT
	 * Clones copies @c this @c daeElement and all of its descendents.
	 * @note daeMeta::create() creates a new element of the same type.
	 * @return Returns a @c daeElementRef of the copy of this element.
	 * @param idSuffix A string to append to the copied element's ID, if one exists.
	 * @param nameSuffix A string to append to the copied element's name, if one exists.
	 */
	inline daeElementRef clone(clone_Suffix *suffix=nullptr)const
	{
		return clone(const_cast<daeDOM*>(getDOM()),suffix);
	}	
	/**
	 * Clones copies @c this @c daeElement and all of its descendents
	 * into @a DOM using it as the parent and its DOM as a target DOM.
	 * @note daeMeta::create() creates a new element of the same type.
	 * @return Returns a @c daeElementRef of the copy of this element.
	 * @param idSuffix A string to append to the copied element's ID, if one exists.
	 * @param nameSuffix A string to append to the copied element's name, if one exists.
	 */
	NOALIAS_LINKAGE daeElementRef clone(daeDOM *DOM, clone_Suffix *suffix=nullptr)const;
	/**WARNING
	 * @c warning This is the recusive version. The clone still needs
	 * to be placed into the parent's contents-array.
	 *
	 * Clones copies @c this @c daeElement and all of its descendents
	 * into @a DOM using it as the parent and its DOM as a target DOM.
	 * @note daeMeta::create() creates a new element of the same type.
	 * @return Returns a @c daeElementRef of the copy of this element.
	 * @param idSuffix A string to append to the copied element's ID, if one exists.
	 * @param nameSuffix A string to append to the copied element's name, if one exists.
	 */
	NOALIAS_LINKAGE daeElementRef clone(daeElement *DOM, clone_Suffix *suffix=nullptr)const;	

COLLADA_(public) //LEGACY QUERY API

	//OLD CLUNKY STUFF
	//These are helper structures to let the XML hierarchy search functions know when we've
	//found a match. You can implement a custom matcher by inheriting from this structure,
	//just like matchName and matchType.
	struct matchElement
	{
		virtual ~matchElement(){}
		virtual bool operator()(const daeElement&)const = 0;		
	};	
	/**LEGACY-SUPPORT
	 * Matches an element by its schema metadata.
	 */
	struct matchMeta : public matchElement
	{
		daeMeta *meta;
		matchMeta(daeMeta *m):meta(m){}		
		virtual bool operator()(const daeElement &e)const{ return meta==&e.getMeta(); }
	};
	/**LEGACY
	 * Matches an element by NCName.
	 */
	struct matchName : public matchElement
	{
		daeName name; //std::string
		matchName(const daeName &n):name(n){}
		virtual bool operator()(const daeElement &e)const{ return name==e.getNCName(); }		
	};	
	/**
	 * Matches an element by namespace.
	 */
	struct matchNamespace : public matchElement
	{
		daeElementTags tags; 
		matchNamespace(const daeElementTags &ns):tags(ns){}
		virtual bool operator()(const daeElement &e)const
		{
			daeElementTags cmp = e.getElementTags();
			if(cmp.namespaceTag==tags.namespaceTag)			
			return !tags.found()||cmp.nameTag==tags.nameTag;
			return false;
		}
	};	
	/**
	 * Matches an element by QName prefix.
	 */
	struct matchQNamePrefix : public matchElement
	{
		daeStringRef_base q; 
		matchQNamePrefix(const daeElementTags &ns):q(ns.second()){}
		virtual bool operator()(const daeElement &e)const
		{
			return q==e.getQNamePrefix();
		}
	};	
	/**WARNING, LEGACY
	 * Matches an element by schema type.
	 * @warning mismatched namespaces can result in false positives
	 */
	struct matchType : public matchElement
	{
		int DAEP_genus;
		matchType(int g):DAEP_genus(g){}		
		virtual bool operator()(const daeElement &e)const{ return DAEP_genus==e.getElementType(); }
	};
	/**WARNING, LEGACY-SUPPORT
	 * Matches an element by SID type.
	 * @warning It's recommended to only use this with @c getChild().
	 * Although @c getDescendant() does a correct SIDREF-like search,
	 * -for some kinds of searches that can mean the entire document.
	 * @see @c sidLookup() and @c daeSIDResolver.
	 */
	struct matchSID : public matchElement
	{
		const daeStringRef &sid;
		template<class T>
		matchSID(const T &SID, daeElement *e=nullptr)
		:sid(daeBoundaryStringRef(*e,SID)){}		
		virtual bool operator()(const daeElement &e)const
		{
			for(daeAttribute *ID=e.getMeta().getFirstID();ID!=nullptr;ID=ID->getNextID())
			if(sid==(const daeStringRef&)ID->getWRT(&e)&&"sid"==ID->getName()) 
			return true; return false; 
		}
	};
	/**LEGACY
	 * Returns a matching child element. By "child", I mean one hierarchy level beneath the
	 * current element. This function is basically the same as getDescendant, except that it
	 * only goes one level deep.
	 */
	NOALIAS_LINKAGE daeElement *getChild(const matchElement &matcher);
	/**LEGACY
	 * Returns a matching child element. By "child", I mean one hierarchy level beneath the
	 * current element. This function is basically the same as getDescendant, except that it
	 * only goes one level deep.
	 */
	inline daeElement *getChild(const daePseudonym &named)
	{
		return getChild(matchName(named)); 
	}
	/**CONST-PROPOGATING-FORM, LEGACY
	 * Returns a matching child element. By "child", I mean one hierarchy level beneath the
	 * current element. This function is basically the same as getDescendant, except that it
	 * only goes one level deep.
	 */
	inline const daeElement *getChild(const matchElement &matcher)const
	{
		return const_cast<daeElement*>(this)->getChild(matcher); 
	}
	/**CONST-PROPOGATING-FORM, LEGACY
	 * Returns a matching child element. By "child", I mean one hierarchy level beneath the
	 * current element. This function is basically the same as getDescendant, except that it
	 * only goes one level deep.
	 */
	inline const daeElement *getChild(const daePseudonym &named)const
	{
		return getChild(matchName(named)); 
	}
	/**WARNING, LEGACY
	 * @warning Strict "breadth-first" is very inefficient. It's unclear if it can be better
	 * implemented top-down.
	 *
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	NOALIAS_LINKAGE daeElementRef getDescendant(const matchElement &matcher);									
	/**WARNING, LEGACY
	 * @warning Strict "breadth-first" is very inefficient. It's unclear if it can be better
	 * implemented top-down.
	 *
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	inline daeElementRef getDescendant(const daePseudonym &named)
	{
		return getDescendant(matchName(named)); 
	}	
	/**WARNING, CONST-PROPOGATING-FORM, LEGACY
	 * @warning Strict "breadth-first" is very inefficient. It's unclear if it can be better
	 * implemented top-down.
	 *
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	inline const daeElementRef getDescendant(const matchElement &matcher)const
	{
		return const_cast<daeElement*>(this)->getDescendant(matcher);
	}
	/**WARNING, CONST-PROPOGATING-FORM, LEGACY
	 * @warning Strict "breadth-first" is very inefficient. It's unclear if it can be better
	 * implemented top-down.
	 *
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	inline const_daeElementRef getDescendant(const daePseudonym &named)const
	{
		return getDescendant(matchName(named)); 
	}	
	/**LEGACY
	 * Searches up through the XML hiearchy and returns a matching element.
	 * @remarks Post-2.5 returns @c const types for upstream "getter" APIs. 
	 */
	NOALIAS_LINKAGE const_daeElementRef getAncestor(const matchElement &matcher)const;
	/**LEGACY
	 * Searches up through the XML hiearchy and returns a matching element.
	 * @remarks Post-2.5 returns @c const types for upstream "getter" APIs. 
	 */
	inline const_daeElementRef getAncestor(const daePseudonym &named)const
	{
		return getAncestor(matchName(named)); 
	}
	template<class T>
	/**LEGACY SUPPORT 
	 * Calls @c getAncestor() vis-a-vis @c daeGetMeta(). 
	 */
	inline daeSmartRef<const typename T::__COLLADA__T> getAncestor()const
	{
		const const_daeElementRef &o = getAncestor(matchMeta(daeGetMeta<T>()));
		return *(daeSmartRef<const typename T::__COLLADA__T>*)&o;
	}
	/**LEGACY
	 * @remarks "getParent" isn't in conflict with @c getParentElement()
	 * because they belong to two different families of APIs. 
	 * Technically @c getParent() is a query API; it's just functionally 
	 * the exact same thing.
	 *
	 * Returns the parent element.
	 * @remarks Post-2.5 returns @c const types for upstream "getter" APIs. 
	 */
	inline const daeElement *getParent()const
	{
		return getParentElement(); 
	}
	template<class T>
	/**LEGACY SUPPORT 
	 * Calls @c getParent() vis-a-vis @c daeGetMeta(). 
	 */
	inline const typename T::__COLLADA__T *getParent()const
	{
		return getParentElement()->a<T>(); 
	}

	/**
	 * @return Returns what @c getChildren().size() would be.
	 */
	NOALIAS_LINKAGE size_t getChildrenCount()const
	SNIPPET( return getMeta().getChildrenCountWRT(this); )

	/**WARNING, LEGACY 
	 * @warning historically, does NOT clear the input array!
	 * BREAKING-CHANGES: children nodes are added to the results
	 * whether-or-not they appear more than once. Logically this should not be!
	 * And where there are duplicates--if ever--this change preserves them/will not obscure them.
	 *
	 * @param array The return value. An elementref array to append this element's children to.
	 * @return Returns @a result.
	 */
	LINKAGE daeArray<daeElementRef> &getChildren(daeArray<daeElementRef> &result)
	SNIPPET( return getMeta().getChildrenWRT(this,result); )
	/**CONST-FORM */
	inline daeArray<const_daeElementRef> &getChildren(daeArray<const_daeElementRef> &result)const
	{
		const_cast<daeElement*>(this)->getChildren((daeArray<daeElementRef>&)result); return result;
	}
	
	
	template<int recursive, class vec>
	/**C++98/03-SUPPORT
	 * Implements @c getChildrenByType().	
	 */
	struct _getChildrenBy_matchElement_f
	{	 
		vec &mc; const matchElement &match;		
		//Reminder: GCC/C++ can't reasonably do explicit-specialization.
		template<int r> void op(daeElement &ch){ r==1?op_1(ch):op_0(ch); }
		inline void op_0(daeElement &ch){ if(match(ch)) mc.push_back(&ch); }
		inline void op_1(daeElement &ch){ op_0(ch);ch.getContents().for_each_child(*this); }
		void operator()(const daeElement &ch){ op<recursive>(const_cast<daeElement&>(ch)); }
	};		
	template<int op, typename vec>
	/** Implements @c getChildrenBy(), etc. */	
	inline void _getChildrenBy_op(const matchElement &match, vec &got)const
	{
		_getChildrenBy_matchElement_f<op,vec> f = { got,match };		
		const_cast<daeElement*>(this)->getContents().for_each_child(f); 
	}	

	template<template<class> class V, typename T>
	/**WARNING, LEGACY, NOT-RECOMMENDED
	 * @warning historically, clears the input array!
	 * @warning mismatched namespaces can result in false positives
	 * @return Returns @a matchingChildren.
	 */	
	inline V<T> &getChildrenByType(V<T> &matchingChildren, enum dae_clear clear=dae_clear)
	{
		V<daeElementRef> &mc = *(V<daeElementRef>*)&matchingChildren;
		daeElement &cc = *T(); (void)cc; matchMeta m(daeGetMeta<T>());
		if(clear) matchingChildren.clear(); _getChildrenBy_op<0>(m,mc); return matchingChildren;
	}	
	template<template<class> class V, typename T>
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */		
	inline V<T> &getChildrenByType(V<T> &matchingChildren, enum dae_clear clear=dae_clear)const
	{
		const daeElement &cc = *T(); 
		return const_cast<daeElement*>(this)->getChildrenByType(matchingChildren,clear);
	}	

	template<template<class> class V, typename T>
	/**LEGACY-SUPPORT 
	 * Use this if you really want to. It's adapted from @c daeDocument::getElementsByType().
	 * @return Returns a recursive version of @c getChildrenByType() in an unspecified order.
	 */	
	inline V<T> &getDescendantsByType(V<T> &matchingDescendants, enum dae_clear clear=dae_clear)
	{
		daeElement &cc = *T(); (void)cc; matchMeta m(daeGetMeta<T>());
		V<daeElementRef> &md = *(V<daeElementRef>*)&matchingDescendants;
		if(clear) matchingDescendants.clear(); _getChildrenBy_op<1>(m,md); return matchingDescendants;
	}	
	template<template<class> class V, typename T>
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */		
	inline V<T> &getDescendantsByType(V<T> &matchingDescendants, enum dae_clear clear=dae_clear)const
	{
		const daeElement &cc = *T(); 
		return const_cast<daeElement*>(this)->getDescendantsByType(matchingDescendants,clear);
	}	

	template<typename V>
	/**
	 * Use this if you really want to. It's adapted from @c daeDocument::getElementsByType().
	 * @return Returns a recursive version of @c getChildrenByType() in an unspecified order.
	 */	
	inline V &getChildrenBy(const matchElement &match, V &matchingChildren, enum dae_clear clear=dae_clear)
	{
		daeElement &cc = *typename V::value_type(); (void)cc;
		if(clear) matchingChildren.clear(); _getChildrenBy_op<0>(match,matchingChildren); return matchingChildren;
	}	
	template<typename V>
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */		
	inline V &getChildrenBy(const matchElement &match, V &matchingChildren, enum dae_clear clear=dae_clear)const
	{
		const daeElement &cc = *typename V::value_type(); (void)cc;
		if(clear) matchingChildren.clear(); _getChildrenBy_op<0>(match,matchingChildren); return matchingChildren;
	}	

	template<typename V>
	/**
	 * Use this if you really want to. It's adapted from @c daeDocument::getElementsByType().
	 * @return Returns a recursive version of @c getChildrenByType() in an unspecified order.
	 */	
	inline V &getDescendantsBy(const matchElement &match, V &matchingDescendants, enum dae_clear clear=dae_clear)
	{
		daeElement &cc = *typename V::value_type(); (void)cc;
		if(clear) matchingDescendants.clear(); _getChildrenBy_op<1>(match,matchingDescendants); return matchingDescendants;
	}	
	template<typename V>
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */		
	inline V &getDescendantsBy(const matchElement &match, V &matchingDescendants, enum dae_clear clear=dae_clear)const
	{
		const daeElement &cc = *typename V::value_type(); (void)cc;
		if(clear) matchingDescendants.clear(); _getChildrenBy_op<1>(match,matchingDescendants); return matchingDescendants;
	}	

	/**
	 * Class for reporting info about element comparisons
	 */
	typedef struct compare_Result
	{
		int compareValue; //> 0 if elt1 > elt2,
		//< 0 if elt1 < elt2,
		//= 0 if elt1 = elt2
		const daeElement *elt1,*elt2;
		bool nameMismatch; //true if the names didn't match		
		daeName attrMismatch; //The name of the mismatched attribute, or "" if there was no attr mismatch
		bool charDataMismatch; //true if the char data didn't match
		bool childCountMismatch; //true if the number of children didn't match
		
		//Write to a string. Use approximately daeArray<daeStringCP,1024>.
		LINKAGE daeArray<> &format(daeArray<> &out);

	}compareResult; //LEGACY
	  
	/**LEGACY
	 * Function for doing a generic, recursive comparison of two XML elements. It
	 * also provides a full element ordering, so that you could store elements in
	 * a map or a set. Return val is > 0 if elt1 > elt2, < 0 if elt1 < elt2, and 0
	 * if elt1 == elt2.
	 */
	NOALIAS_LINKAGE static int compare(const daeElement &elt1, const daeElement &elt2, compare_Result *ret=nullptr);
	/**LEGACY-SUPPORT
	 * Same as the previous function, but returns a full compare_Result object.
	 */
	static int compare(compare_Result &ret, const daeElement &elt1, const daeElement &elt2)
	{
		return compare(elt1,elt2,&ret);
	}

	/**LEGACY
	 * Same as the previous function, but returns a full compare_Result object.
	 */
	static compare_Result compareWithFullResult(const daeElement &elt1, const daeElement &elt2)
	{
		compare_Result o; compare(elt1,elt2,&o); return o;
	}	
};

#include "../LINKAGE.HPP" //#undef LINKAGE
  				
//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ELEMENT_H__
/*C1071*/
