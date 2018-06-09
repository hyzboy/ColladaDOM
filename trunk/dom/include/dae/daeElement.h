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
typedef daeAnyAttribute xsAnyAttribute;
typedef daeAST::TypedUnion xsAnySimpleType;
		
#ifndef COLLADA_NODEPRECATED
//Just for getDocumentURI() API.
template<int> class daeURI_size;
typedef daeSmartRef<const daeURI_size<260>> const_daeURIRef;
#endif //COLLADA_NODEPRECATED

template<int size_on_stack>
/**EXPERIMENTAL 
 * Trying to add string-literal comparison operators to
 * a @c daeArray.
 */
class daeCharData_size
:
public daeArray<daeStringCP,size_on_stack> 
,
public daeString_support<daeCharData_size<size_on_stack>>
{
	typedef daeArray<daeStringCP,size_on_stack> _1;
	typedef daeString_support<daeCharData_size<size_on_stack>> _2;

COLLADA_(public)

	using _2::operator==;
	using _2::operator!=;

	daeCharData_size()
	{
		_1::clear_and_0_terminate();
	}
	daeCharData_size(const daeHashString &cp)
	{
		operator=(cp);
	}
	daeCharData_size &operator=(const daeHashString &cp)
	{
		_1::assign_and_0_terminate(cp.data(),cp.size()); 
		return *this;
	}

	/**EXPERIMENTAL
	 * Facilitating @c daeBoundaryStringIn construction.
	 */
	inline operator daeString()const
	{
		assert(_1::back()=='\0');
		return _1::data(); 
	}
};
/**EXPERIMENTAL 
 * Trying to add string-literal comparison operators to
 * a @c daeArray.
 */
typedef daeCharData_size<32> daeCharData;

/**LEGACY-SUPPORT
 * @c daeAttributes needs this defined outside of its body.
 */
struct daeAttribute2
{
	typedef void __COLLADA__atomize;
	daeName name; daeCharData value; 
};
template<int size_on_stack>
/**LEGACY-SUPPORT
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
/**LEGACY-SUPPORT
 * @see string pairs version of @c daeElement::getAttributes().
 */
typedef daeAttributes_size<10> daeAttributes; 

/**PLAIN-OLD-DATA
 * 32-bit structure that overlays with @c daeObject::_tags.
 *
 * If two elements have equal tags, then you can assume several
 * things about them:
 *
 * A) They belong to the same modular XML namespace.
 * B) They have the same : based XML prefix: Eg. <xs:element>.
 * C) The virtual-table layout is identical. This one's for librarians.
 * D) They hail from the same client module; That is hosting the space.
 *
 * IMPORTANT!
 * None of this holds true unless the entire 32-bit value is identical.
 * In other words, do not compare individual tags. That is meaningless.
 */
struct daeElementTags
{
	inline operator daeInt()const{ return (daeInt&)*this; }

	/**ABCD
	 * interfaceTag corresponds to daeObjectLayout::_vtab. 
	 * moduleTag is always odd if owned by a daeDatabase, &
	 * moduleTag is always 0 if not owned by a daeDatabase.
	 * nameTag will lookup a global XML colonized prefix, but
	 * namespaceTag must be provided to the lookup procedure, in
	 * case it's necessary to expand the table beyond 255 entries.
	 * When nameTag is 0, namespaceTag is the the default namespace.
	 * (@c domAny has issues because it's in the 0th process-share.)
	 * When namespaceTag is 0, the element is not in a contents-array. 
	 * (This is to work w/ @c daeMetaElement::createWRT()->placeWRT().)
	 * When namespaceTag is 1, it is an undefined, document namespace. 
	 */
	const daeTag namespaceTag,nameTag,interfaceTag,moduleTag;
};

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

#include "../LINKAGE.HPP" //#define LINKAGE

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
	friend class domAny;
	friend class daeObject;
	friend class daeDocument;
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
	explicit daeElement(const DAEP::Proto<> &pt):daeObject(pt)
	{ /*NOP*/ }
	//REPLICATING @c DAEP::Element CONSTRUCTOR
	explicit daeElement(const DAEP::Object *_this):daeObject(_this)
	{ __DAEP__Object__unembed(1); }
	friend class DAEP::Object;
	friend class DAEP::Element;
	/** These ferry the document downstream (for @c daeDocument::typeLookup().) */
	void __atomize2(daeMeta&);
	void __clear2(daeContents&,daeMeta&,const daeDocument*);
	COLLADA_(public)
	/**CIRCULAR-DEPENDENCY @c DAEP::Element::__COLLADA__atomize() is exported. */
	inline void __COLLADA__atomize(){ ((DAEP::Element*)this)->__COLLADA__atomize(); }

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
		assert(isContent()); return sidLookup<S,T>(sid,match,const_daeDocRef(getDoc()),clear);
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

COLLADA_(public) //2.5 daeSafeCast-like shorthand

	COLLADA_DOM_OBJECT_OPERATORS(daeElement)
	/** These sometimes comes in handy since no longer inheriting. */
	inline operator DAEP::Element*(){ return (DAEP::Element*)this; }	
	/** These sometimes comes in handy since no longer inheriting. */
	operator const DAEP::Element*()const{ return (DAEP::Element*)this; }
	/** These sometimes comes in handy since no longer inheriting. */
	inline operator DAEP::Element&(){ return *(DAEP::Element*)this; }	
	/** These sometimes comes in handy since no longer inheriting. */
	operator const DAEP::Element&()const{ return *(DAEP::Element*)this; }

	/**WORKAROUND
	 * This is now needed by all standard @c daeSmartRef classes since
	 * many templates use it to let smart-refs stand in for their type.	 
	 * (That allows consistent use of shorter WYSIWYG smart-ref names.)
	 */
	typedef daeElement __COLLADA__T;

	template<class T> 
	/**WARNING
	 * @warning 2.5 adds an @c assert() to @c daeSafeCast(). This was
	 * originally to be a shorter alternative, but eventually similar
	 * APIs were added to the other objects, which are designed to do
	 * filtering. For consistency the @c assert() function is removed.
	 *
	 * Here is an example; below "->" is pronounced "to."
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.C++}
	 * domCOLLADA *COLLADA = p->a<domCOLLADA>();
	 */	
	inline typename T::__COLLADA__T *a()
	{
		return daeUnsafe<T>(this)?nullptr:(typename T::__COLLADA__T*)this; 
	}		
	template<class T> 
	/**CONST-FORM 
	 * @see non-const Doxygentation. 
	 */
	const typename T::__COLLADA__T *a()const
	{
		return daeUnsafe<T>(this)?nullptr:(typename T::__COLLADA__T*)this; 
	}	
	template<class T> 
	/**ALT-SPELLING
	 * Just because some words use "an" instead of "a" in English. 
	 * @see @c a().
	 */
	inline typename T::__COLLADA__T *an()
	{
		return daeUnsafe<T>(this)?nullptr:(typename T::__COLLADA__T*)this; 
	}		
	template<class T> 
	/**CONST-FORM, ALT-SPELLING
	 * @see @c a().
	 */
	const typename T::__COLLADA__T *an()const
	{
		return daeUnsafe<T>(this)?nullptr:(typename T::__COLLADA__T*)this; 
	}	

COLLADA_(public)	
	/**LEGACY
	 * @remarks This method predates 2.5, and so is repurposed.
	 * @return Returns the daeObjectType class's typeless enum.
	 */
	inline int getElementType()const{ return getMeta().getElementType(); }

	/**WARNING, LEGACY-SUPPORT
	 * Gets this element's NCName.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some child names are NCNames with a colon, or a pseudo QName.
	 * @return Returns the @c daePseudonym for the NCName.
	 * @remarks @c getTypeName() gets the COLLADA schema type name
	 * @warning A STRING-LITERAL OR PERMA-STRING MUST BE USED TO SET THIS.
	 */
	inline daePseudonym &getNCName(){ return __DAEP__Element__data.NCName; }
	/**CONST-FORM, LEGACY-SUPPORT
	 * Gets this element's NCName.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some child names are NCNames with a colon, or a pseudo QName.
	 * @return Returns the @c daePseudonym for the NCName.
	 * @remarks @c getTypeName() gets the COLLADA schema type name
	 * Use caution when using this function since you can easily create invalid documents.
	 */
	inline const daePseudonym &getNCName()const{ return __DAEP__Element__data.NCName; }	

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
	/**INVISIBLE, SHADOWING @c daeObject::getDOM().
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

	/**MICRO-OPTIMIZATION
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 */
	inline bool getOptimizationBit_has_registered_ID_attribute()const
	{
		return 1==__DAEP__Element__data.getMeta_getFirstID_is_nonzero;
	}
	//NOTE: THIS MAY BE USEFUL IF NOTICES ARE DISABLED, BECAUSE
	//IF THERE'S A GRAPH, THE NOTICE IS PROBABLY UNAVOIDABLE, AS
	//THERE SHOULD ONLY BE ONE GRAPH-TRAVERSER: I.E. THE DATABASE.
	/**MICRO-OPTIMIZATION
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
	inline bool getOptimizationBit_is_definitely_not_a_graph()const
	{
		return 0==__DAEP__Element__data.daeContents_capacity_is_nonzero;
	}

	/**STATIC OVERRIDE
	 * Tags provide a mechanism for extensibility. 
	 * @remarks At some point getTags might be added to daeObject; Override it here if so.
	 * @return Returns @c (daeElementTags&)daeObject::_tags by value.
	 */ 
	inline daeElementTags getTags()const
	{
		daeCTC<_sizeof__DAEP__Object__tags==sizeof(daeElementTags)>(); 
		return (daeElementTags&)_getClassTag(); 
	}
	/** @see getTags--Just explicit--and just in case of name-clash. */
	inline daeElementTags getElementTags()const
	{
		daeCTC<_sizeof__DAEP__Object__tags==sizeof(daeElementTags)>();
		return (daeElementTags&)_getClassTag(); 
	}

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
	inline daeHashString _string(daeData &d)const
	{
		if(d->getTypeWRT(this).hasStringType())
		return (const daeStringRef&)d.getWRT(this); 		
		return daeHashString("",0);
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
		return getAttributes()._maybe_get(i_or_Name);
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
		return &getAttributes()[i_or_Name]-getAttributes().begin();
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
		return &getAttributes()._maybe_get(i_or_Name)-getAttributes().begin();
	}

	template<class I, class T>
	/**LEGACY, OVERLOAD
	 * Sets the attribute to the specified value.
	 * @param i_or_Name Name or index of the set attribute.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some attribute names are NCNames with a colon, or a pseudo QName.
	 * @param value Value to apply to the attribute.
	 * @return Returns true if the attribute was found and the value was set, false otherwise.
	 */
	inline daeOK setAttribute(const I &i_or_Name, const T &value)
	{
		return getAttribute(i_or_Name).stringToMemoryWRT(this,value);
	}
	template<class I, class T, class U>
	/**LEGACY-SUPPORT, OVERLOAD
	 * Sets the attribute to the specified value.
	 * @param i_or_Name Name or index of the set attribute.
	 * @note The library doesn't (yet) support QNames properly.
	 * Some attribute names are NCNames with a colon, or a pseudo QName.
	 * @param value Value to apply to the attribute.
	 * @return Returns true if the attribute was found and the value was set, false otherwise.
	 */
	inline daeOK setAttribute(const I &i_or_Name, const T &value, U len_or_end)
	{
		return getAttribute(i_or_Name).stringToMemoryWRT(this,value,len_or_end);
	}	

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
		daeAnyAttribute &aa = getAttributes(); return aa.size()<=i?"":aa[i]->getName();
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
	inline daeCharData &getAttribute(const I &i_or_Name, daeArray<daeStringCP> &val)const
	{
		getAttributes()._maybe_get(i_or_Name).memoryToStringWRT(this,val);
		return *(daeCharData*)&val;
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

	/**
	 * Formerly "getCharDataObject."
	 * (Changed the name since "getAttributeObject" is removed, and "object" suggests
	 * that it is somehow related to @c daeObject.)
	 *
	 * Returns the @c daeData object corresponding to the character data for this element.
	 * @return No longer returns @c nullptr. If it returns a bad reference, it's because 
	 * @c daeMetaElement::getValue() is not fully implemented.
	 * @see hasCharData().
	 */
	inline daeData &getCharData()const
	{
		//TODO: Should work even if !hasCharData().
		return const_cast<daeDefault&>(getMeta().getValue()); 
	}
	/**LEGACY-SUPPORT
	 * @return Returns @a val.
	 * @param data The string to be filled with this element's character content. The
	 * string is set to an empty string if this element can't have character data.
	 */
	inline daeCharData &getCharData(daeArray<daeStringCP> &val)const
	{
		getCharData().memoryToStringWRT(this,val); return *(daeCharData*)&val;
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

	/**LEGACY-SUPPORT
	 * Tells if the element is part of a document, as opposed to simply having a
	 * @c daeDocument object parent.
	 */
	inline bool isContent()const{ return 0!=getElementTags().namespaceTag; }
	/**WARNING, LEGACY
	 * Gets the container document for @c this element. 
	 * @return Returns a @c const pointer. Post-2.5 upstream pointers are @c const.
	 * @warning This is not @c getDoc(). If @c isContent()==false, it's @c nullptr.
	 */
	inline const daeDocument *getDocument()const
	{
		#ifdef NDEBUG
		#error Is isContent() sufficient? Can false==getParentElement()->isContent()?
		#endif
		//The isContent() check is really required for routines that try to determine
		//if an element was removed from a contents-array, because more often than not
		//that does not change the element's parent object.
		return !isContent()?nullptr:(daeDocument*)getDoc(); 
	}

	#ifndef COLLADA_NODEPRECATED
	/**LEGACY, DEPRECATED
	 * (@c getDocument()->getBaseURI() should do.)
	 * Gets the URI of the document containing this element, note that this is NOT the URI of the element.
	 * @return Returns a pointer to the daeURI of the document containing this element.
	 */
	NOALIAS_LINKAGE const_daeURIRef getDocumentURI()const;
	#endif //COLLADA_NODEPRECATED
				
	//OLD CLUNKY STUFF
	//These are helper structures to let the XML hierarchy search functions know when we've
	//found a match. You can implement a custom matcher by inheriting from this structure,
	//just like matchName and matchType.
	struct matchElement
	{
		virtual ~matchElement(){}
		virtual bool operator()(const daeElement*)const = 0;		
	};	
	/**LEGACY-SUPPORT
	 * Matches an element by its schema metadata.
	 */
	struct matchMeta : public matchElement
	{
		daeMeta *meta;
		matchMeta(daeMeta *m):meta(m){}		
		virtual bool operator()(const daeElement *e)const{ return meta==e->getMeta(); }
	};
	/**LEGACY
	 * Matches an element by name.
	 */
	struct matchName : public matchElement
	{
		daePseudonym name; //std::string
		matchName(const daePseudonym &n):name(n){}
		virtual bool operator()(const daeElement *e)const{ return name==e->getElementName(); }		
	};	
	/**WARNING, LEGACY
	 * Matches an element by schema type.
	 * @warning mismatched namespaces can result in false positives
	 */
	struct matchType : public matchElement
	{
		int DAEP_genus;
		matchType(int g):DAEP_genus(g){}		
		virtual bool operator()(const daeElement *e)const{ return DAEP_genus==e->getElementType(); }
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
		virtual bool operator()(const daeElement *e)const
		{
			for(daeAttribute *ID=e->getMeta().getFirstID();ID!=nullptr;ID=ID->getNextID())
			if(sid==(const daeStringRef&)ID->getWRT(e)&&"sid"==ID->getName()) 
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
	/**LEGACY
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	NOALIAS_LINKAGE daeElementRef getDescendant(const matchElement &matcher);									
	/**LEGACY
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	inline daeElementRef getDescendant(const daePseudonym &named)
	{
		return getDescendant(matchName(named)); 
	}	
	/**CONST-PROPOGATING-FORM, LEGACY
	 * Performs a breadth-first search and returns a matching descendant element. A "descendant
	 * element" is an element beneath the current element in the xml hierarchy.
	 */
	inline const daeElementRef getDescendant(const matchElement &matcher)const
	{
		return const_cast<daeElement*>(this)->getDescendant(matcher);
	}
	/**CONST-PROPOGATING-FORM, LEGACY
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

	/**WARNING, LEGACY
	 * SOFT-DEPRECATED: Prefer @c getNCName().	 
	 * @warning: QNames are not properly implemented. (They are NCNames with colons.)
	 * Gets this element's QName.
	 * @return Returns the string for the name.
	 * @remarks @c getTypeName() gets the COLLADA schema type name
	 */
	inline const daePseudonym &getElementName()const
	{
		return __DAEP__Element__data.NCName; 
	}		
	template<class T>
	/**WARNING, LEGACY
	 * SOFT-DEPRECATED: Prefer @c getNCName().
	 * @warning: QNames are not properly implemented. (They are NCNames with colons.)
	 * Sets this element's QName.
	 * @param QName Specifies the string to use as the element's name.
	 * @remarks This legacy API adds you string to the system string-table to be safe.
	 * Use caution when using this function since you can easily create invalid documents.
	 */
	inline void setElementName(const T &QName)
	{
		//TODO: QNAMES ARE NOT ACTUALLY IMPLEMENTED. COLONS SHOULD BE LOOKED FOR HERE.
		daeStringRef ref(QName); __DAEP__Element__data.NCName = ref;
		//IF LARGER STRINGS ARE REQUIRED (UNICODE MAYBE?) MANAGE THEM BY ANOTHER MEANS.
		assert(ref.size()<64);
	}	
	/**LEGACY
	 * Gets the COLLADA schema type name.
	 */
	inline const daePseudonym &getTypeName()const{ return getMeta().getName(); }
	
	/**KISS
	 * Implements @c getContents().grow(). It's needed to adjust the iterator.
	 */
	LINKAGE void __grow(daeArray<daeContent> &contents_of_this, size_t minCapacity);
	/**KISS
	 * Implements @c getContents().clear().
	 * @remarks "e->__clear(e->getContents())" is technically
	 * more efficent, but stylistically, getContents().clear() is recommended.
	 * @note @c this==nullptr is accepted, in case of freestanding contents-arrays.	 
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
	
	template<int recursive=0, class T=daeArray<daeElementRef>>
	/**C++98/03-SUPPORT
	 * Implements @c getChildrenByType().	
	 */
	struct _getChildrenByType_f
	{	
		T &mc; daeMeta *meta;		
		//Reminder: GCC/C++ can't reasonably do explicit-specialization.
		template<int r> void op(daeElement *ch){ r==1?op_1(ch):op_0(ch); }
		inline void op_0(daeElement *ch){ if(ch->getMeta()==meta) mc.push_back(ch); }
		inline void op_1(daeElement *ch){ op_0(ch); ch->getContents().for_each_child(*this); }
		void operator()(daeElement *ch){ op<recursive>(ch); }
	};	
	template<int op, typename T>
	/** Implements @c getChildrenByType(), etc. */	
	inline void _getChildrenByType_op(daeArray<daeSmartRef<T>> &got)const
	{
		_getChildrenByType_f<op> f = { (daeArray<daeElementRef>&)got,daeGetMeta<T>() };
		const_cast<daeElement*>(this)->getContents().for_each_child(f); 
	}	
	template<typename T>
	/**WARNING, LEGACY, NOT-RECOMMENDED
	 * @warning historically, clears the input array!
	 * @warning mismatched namespaces can result in false positives
	 * @return Returns @a matchingChildren.
	 */	
	inline daeArray<daeSmartRef<T>> &getChildrenByType(daeArray<daeSmartRef<T>> &matchingChildren, enum dae_clear clear=dae_clear)
	{
		if(clear) matchingChildren.clear(); _getChildrenByType_op<0>(matchingChildren); return matchingChildren;
	}	
	template<typename T>
	/**LEGACY-SUPPORT 
	 * Use this if you really want to. It's adapted from @c daeDocument::getElementsByType().
	 * @return Returns a recursive version of @c getChildrenByType() in an unspecified order.
	 */	
	inline daeArray<daeSmartRef<T>> &getDescendantsByType(daeArray<daeSmartRef<T>> &matchingDescendants, enum dae_clear clear=dae_clear)const
	{
		if(clear) matchingDescendants.clear(); _getChildrenByType_op<1>(matchingDescendants); return matchingDescendants;
	}	
	
	/**LEGACY-SUPPORT */
	struct clone_Suffix : daeArray<daeStringCP,96>
	{
		clone_Suffix():id(nullptr,0),name(nullptr,0){}
		daeHashString id,name; COLLADA_NOINLINE void _suffix(daeElement*);		
	};
	/**LEGACY
	 * Clones copies @c this @c daeElement and all of its descendents.
	 * @param DOM performs cross-DOM copy. Cross-copy is no different.
	 * @return Returns a @c daeElementRef of the copy of this element.
	 * @param idSuffix A string to append to the copied element's ID, if one exists.
	 * @param nameSuffix A string to append to the copied element's name, if one exists.
	 */
	NOALIAS_LINKAGE daeElementRef clone(daeDOM &DOM, clone_Suffix *suffix=nullptr)const;
	/**LEGACY-SUPPORT
	 * Clones copies @c this @c daeElement and all of its descendents.
	 * @return Returns a @c daeElementRef of the copy of this element.
	 * @param idSuffix A string to append to the copied element's ID, if one exists.
	 * @param nameSuffix A string to append to the copied element's name, if one exists.
	 */
	inline daeElementRef clone(clone_Suffix *suffix=nullptr)const
	{
		return clone(*const_cast<daeDOM*>(getDOM()),suffix);
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
		//2.5: std::string was unsafe
		daePseudonym attrMismatch; //The name of the mismatched attribute, or "" if there was no attr mismatch
		bool charDataMismatch; //true if the char data didn't match
		bool childCountMismatch; //true if the number of children didn't match

		compare_Result()
		:compareValue(),elt1(),elt2(),nameMismatch(),attrMismatch(""),
		charDataMismatch(),childCountMismatch(){}

		//Write to a string. Use approximately daeArray<daeStringCP,1024>.
		LINKAGE daeArray<daeStringCP> &format(daeArray<daeStringCP> &out);

	}compareResult; //LEGACY
	  
	/**LEGACY
	 * Function for doing a generic, recursive comparison of two XML elements. It
	 * also provides a full element ordering, so that you could store elements in
	 * a map or a set. Return val is > 0 if elt1 > elt2, < 0 if elt1 < elt2, and 0
	 * if elt1 == elt2.
	 */
	static int compare(const daeElement &elt1, const daeElement &elt2)
	{
		return compareWithFullResult(elt1,elt2).compareValue;
	} 
	/**LEGACY
	 * Same as the previous function, but returns a full compare_Result object.
	 */
	NOALIAS_LINKAGE static compare_Result compareWithFullResult(const daeElement &elt1, const daeElement &elt2);
};

#include "../LINKAGE.HPP" //#undef LINKAGE

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ELEMENT_H__
/*C1071*/
