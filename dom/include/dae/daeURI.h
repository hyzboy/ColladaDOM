/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_URI_H__
#define __COLLADA_DOM__DAE_URI_H__

#include "daeRefResolver.h"
						
COLLADA_(namespace)
{//-.
//<-'

#ifdef NDEBUG
#error Does the new constructor regime require this?
#endif
/**INTERNAL C2335 */
class daeURI_parser_view;

template<int size_on_stack> 
/**VARIABLE-LENGTH
 * @note @c daeURI_size<> or @c daeURI_size<1> is reserved for
 * xml:base @c DAEP::Value members. It's also being sacrificed
 * to IntelliSense, which fails to auto-complete it after it's
 * used by @c deaURI_base.
 */
class daeURI_size 
: 
//public daeURI_base
//Visual Studio has "typename ignored in base class".
//public COLLADA_NCOMPLETE(size_on_stack) daeURI_base
public COLLADA::NCOMPLETE<size_on_stack>::daeURI_base
{	
	friend class daeDOM;
	friend class daeDoc;
	friend class daeURI_base;
	template<int> friend class daeURI_size;
	
	//This way GCC/C++ can delay evaluation of some types.
	typedef COLLADA::NCOMPLETE<size_on_stack> _incomplete;

COLLADA_(protected) //DATA-MEMBER
	/** 
	 * Multi-representational string. 
	 */
	daeRefString<size_on_stack> _refString;

COLLADA_(public) //NON-STRING CONSTRUCTORS
	/**
	 * Default Constructor
	 */
	daeURI_size(enum dae_clear clr=dae_clear)
	{
		if(clr==dae_clear) this->_00(); 
	}
	/**
	 * Non-clearing Constructor
	 */
	daeURI_size(enum dae_clear clr, const DAEP::Object *c)
	:daeURI_base(c)
	{
		if(clr==dae_clear) this->_00(); 
	}

	template<class C_str>
	/**LEGACY-SUPPORT
	 * Constructor having @c this contained by @a c.
	 */
	daeURI_size(const C_str &URI, const DAEP::Object *c):daeURI_base(c)
	{
		this->_setURI(daeBoundaryStringIn(URI).c_str);
	}
	template<class C_str>
	/**LEGACY-SUPPORT
	 * Constructor having @c this contained by @a c.
	 *
	 * @remark @c xml::base construction needs this
	 * to not be ambiguous with older constructions.
	 */
	daeURI_size(const C_str &URI, const daeSmartRef_base &c)
	:daeURI_base(*(DAEP::Object**)&c)
	{
		this->_setURI(daeBoundaryStringIn(URI).c_str);
	}
	template<class C_str>
	/**LEGACY-SUPPORT
	 * Constructor having @c this contained by @a c.
	 *
	 * @remark May as well allow @c daeSmartTag too.
	 */
	daeURI_size(const C_str &URI, const daeSmartTag_base &c)
	:daeURI_base(*(DAEP::Object**)&c)
	{
		this->_setURI(daeBoundaryStringIn(URI).c_str);
	}

	template<class Base> 
	/**LEGACY-SUPPORT
	 * @warning This is not as flexible as @c setURI() since
	 * it cannot use a string as a base, because it needs to
	 * have an object associated with it--to be used--though
	 * arguably the parent/container object is of little use.
	 *
	 * Constructs a @c daeURI out of a @a baseURI & a @a URI. 
	 * Calls @c setURI(URI,(const daeURI*)baseURI).
	 *
	 * @param baseURI Base URI to resolve against. Its parent object
	 * will be used as the new URI's parent, like a copy-constructor.
	 *
	 * @param URI The URI string to be combined with @a baseURI when
	 * it is a relative URI; or the URI if @a URI is an absolute URI.
	 */
	daeURI_size(const Base &baseURI, daeBoundaryStringIn URI)	
	:daeURI_base(((const daeObject*)baseURI)->getParentObject())
	{
		const daeURI *base = baseURI; this->_setURI(URI.c_str,base);
	}
	 									
COLLADA_(protected) //PROTECTED daeURI_parser CONSTRUCTOR
	/**
	 * This is used by @c daeURI_parser to avoid calling @c _00().
	 */
	daeURI_size(const daeURI_parser_view &URI, const DAEP::Object *c)
	//Call the view-constructor on dummy class daeURI_parser_view.
	:daeURI_base(c),_refString(&(daeStringCP&)URI)
	{
		this->_setURI(&(daeStringCP&)URI,nullptr); //Won't overwrite itself.
	}

COLLADA_(public) //STRING CONSTRUCTORS
	
	template<class T>
	/**C++98/03 SUPPORT
	 * If the standard copy-constructor is not available, the compiler
	 * generates one. Same goes for the assignment operator. Therefore
	 * the constructors are defined in terms of @c operator=().
	 * @c remarks Under C++11 these could be deleted and this wouldn't
	 * be necessary as these would default to @c daeURI_base's.
	 *
	 * UPDATE: I don't know if the default agument is reasonably C++98
	 * anymore. But it's required with GCC and probably most compilers.
	 * The only other way is to define all constructors.
	 */
	daeURI_size(const T &cp, typename DAEP::NoValue<T>::type SFINAE=0)
	{
		this->daeURI_base::operator=(cp); (void)SFINAE;
	}
	/** C++ Non-Default Copy Constructor */
	daeURI_size(const daeURI_size &cp){ this->daeURI_base::operator=(cp); }

	template<class T, int ID, class CC, typename CC::_ PtoM>
	/**
	 * Because SFINAE is used above to prevent implicit conversion, so
	 * the DAEP Value's conversion operator can take precedence, it is
	 * necessary to allow explicit construction.
	 */
	explicit daeURI_size(const DAEP::Value<ID,T,CC,PtoM> &cp)
	{
		this->daeURI_base::operator=(cp);
	}

	template<class T>
	COLLADA_SUPPRESS_C(4522)
	/** Pass-through Assignment Operator */
	daeURI &operator=(const T &cp){ return this->daeURI_base::operator=(cp); }
	COLLADA_SUPPRESS_C(4522)
	/** C++ Non-Default Assignment Operator */
	daeURI &operator=(const daeURI_size &cp){ return this->daeURI_base::operator=(cp); }

COLLADA_(public) //LAZY EVALUATION 
	/**
	 * @return Returns @c true if @c this URI references @a doc. 	 
	 */
	inline bool referencesDoc(const daeDoc *doc)const
	{	
		const typename _incomplete::daeDoc *i = doc;
		assert(i!=nullptr);
		return this->referencesURI(i->getDocURI());
	}
	/**XML-BASE
	 * @return Returns @c true if @c true==referencesDoc(doc) or
	 * else falls back on xml:base like logic.
	 * @see @c isSameDocumentReference().
	 */
	inline bool referencesDoc2(const daeDoc *doc)const
	{	
		//NOTE: If the URI refers to the xml:base URI then it's a 
		//same-document-refrence, but I'm not 100% certain if the
		//same is true if there is not a fragment. 
		//Then it follows that if the URI is the documents actual
		//address, then it is also referring to the same document.
		//Therefore two comparisons are needed, unless the URI is
		//the same.
		//It seems best to do the quick check first, since it can
		//skip the next check if the URI is internal and resolved.
		if(referencesDoc(doc)) return true;
		const typename _incomplete::daeDoc *i = doc;
		const_daeURIRef xml_base = i->getBaseURI(&this->getParentObject());
		if(xml_base!=&i->getDocURI())
		return this->referencesURI(*xml_base); return false;
	}
	/**
	 * @c daeDoc is an incomplete type.
	 * @return Returns @c true if @c this URI is based on @a arc. 
	 */	
	inline bool transitsDoc(const daeDoc *doc)const
	{	
		const typename _incomplete::daeDoc *i = doc;
		assert(i!=nullptr);
		return this->transitsURI(i->getDocURI());
	}

	/**LEGACY
	 * @return Returns the document that this URI references, 
	 * -or @c nullptr if the document hasn't been loaded yet.
	 * @note Cannot be a @c daeURI_base method because of @c daeDoc::getDocument().
	 */
	inline daeDocumentRef getReferencedDocument()const
	{
		const daeDOM *DOM = this->getDOM();
		const typename _incomplete::daeDocRef doc;
		if(DOM!=nullptr) this->docLookup2(*DOM,doc); return doc->getDocument();
	}
};

#include "../LINKAGE.HPP" //#define LINKAGE

/**NOT A BASE-URI. 
 * This class is not for clients to use. 
 * @see @c daeURI_size, where it is the base-class.
 * @note ALL OF @c daeURI's VIRTUAL OVERRIDES BELONG IN THIS CLASS.
 */
class daeURI_base : public daeRef_support<daeURI>
{														 
COLLADA_(protected) 	

  //////////////////////////////////////////////////////
  //REMINDER: don't try to implement constructors here//
  //that operator= will take as an implicit conversion//
  //////////////////////////////////////////////////////

	/**
	 * Default Constructor
	 */
	daeURI_base():COLLADA_SUPPRESS_C(4355)daeRef_support(this){}
	/**
	 * Constructor
	 */
	explicit daeURI_base(const DAEP::Object *c):daeRef_support(c){}

COLLADA_(public) //daeRef methods
										
	/** Statically shadows @c daeRef::getRefType(). */
	static int getRefType(){ return daeRefType::URI; }

	/**PURE-OVERRIDE
	 * One of @c daeRefType, or negative if registered as a client type.
	 */
	virtual int __daeRef__v1__type()const{ return daeRefType::URI; }

	/**PURE-OVERRIDE
	 * Gets this URI like reference's text--refreshed; cannot return @c nullptr.
	 */
	virtual daeString __daeRef__v1__text()const{ refresh(); return data(); }

COLLADA_(public) //DAEP::Object methods

	/**PURE-OVERRIDE */
	virtual DAEP::Model &__DAEP__Object__v1__model()const
	{
		//daeRefString must be able to switch to dynamic-storage.		
		static DAEP::Model *om = nullptr; if(om!=nullptr) return *om;
		static daeAlloc<daeStringCP,0> t; daeModel &m = 
		getDefaultProcessShare().addModel<1>((daeURI_size<>*)this,"COLLADA::daeURI_base");				
		m.addFeature_variant<1>(this,_refString().getInternalStorage()
		,"VARIANT //daeRefString::_varray").setAllocThunk_offset(t);
		m.addFeatureComplete();
		m.setObjectType(daeObjectType::REF); om = &m; return *om; 	
	}

COLLADA_(private) //DATA-MEMBERS

	template<int> friend class daeURI_size;
  
  ////WARNING, STOP!/////////////////////////////////
  //Don't change or rearrange any of these members,//
  //without carefully looking over daeURI.cpp.///////
  ///////////////////////////////////////////////////

	/**
	 * This had been @c daeUShort, but that type is meant to
	 * configure xs:unsignedShort.
	 */
	typedef unsigned short CP;

	/**DELICATE MACHINERY
	 * This is the codepoint where the relative-part of the URL
	 * begins, for purposes of reconstructing the relative URL,
	 * given a non-relative URL, based on a base URL. 
	 * @note Relativity is not strictly path-based, owing to the
	 * modern advent of "protocol-relative" URLs.
	 * @see @c _rel_backtracks.
	 */
	short _rel_half;	
	/**DELICATE MACHINERY
	 * ALIGNED behind @c _rel_backtracks 
	 * (@c _rel_backtracks counts the ../ directives.)
	 * authority component
	 * @note The "scheme" is implicitly at position 0.
	 * @note Non-URL schemes are not readily supported 
	 * by this scheme. They will appear to be empty, in
	 * which case, the : must be scanned for manually by
	 * the resolver.
	 * (To try to distinguish between URIs and URLs, the 
	 * scheme is being called "protocol".)
	 */
	char _rel_backtracks, _authority;
	/**DELICATE MACHINERY
	 * authority component dissected 
	 * @note The "user" is implicitly at position @c _authority.	 
	 */
	short _authority_password, _authority_host, _authority_port;
	/**DELICATE MACHINERY
	 * path component 
	 * @see @c _rel_backtracks Doxygentation pertaining to @c _rel_half. 
	 */
	short _path, _path_filename, _path_extension;	
	/**DELICATE MACHINERY
	 * query component */
	short _query;
	/**DELICATE MACHINERY
	 * fragment component */
	short _fragment;
	/**DELICATE MACHINERY
	 * code-point count, including the 0 terminating code-point. */
	short _size;
	/** UNUSED, ALIGNED
	 * Must be 0. Maybe it will be a scheme-based enum? Or @c _size 
	 * might be expanded to 32, or even size_t bits if possible. */
	short _reserved; inline void _00()
	{
		memset(&_rel_half,0x00,intptr_t(&_size)-intptr_t(&_rel_half));
		_size = 1; _reserved = 0;
	}	
	inline daeRefString<> &_refString()const
	{
		//NOTE: 1 is equivalent to 0 but used as a dummy because
		//IntelliSense cannot display whichever value is used by
		//daeURI.h.
		return ((daeURI_size<>*)this)->_refString; 
	}

COLLADA_(private) //OBJECT MEMBERS

	struct _Flags
	{
		#ifdef NDEBUG
		#error Add string-ref support via a ref-release flag.
		#endif
		unsigned int resolved:1,any:1,attached:1,directory_like:1; 

		_Flags(){ (unsigned&)*this = 0; }
	};
	inline _Flags &_getFlags()const{ return *(_Flags*)&_getObjectTags(); } 
	
COLLADA_(public) //OPERATORS
		
	COLLADA_DOM_OBJECT_OPERATORS(daeURI)
		
	/**
	 * Cast to any @c daeURI, as the @c size_on_stack is irrelevant.
	 */
	inline operator daeURI&(){ return *(daeURI*)this; }
	/**CONST-FORM
	 * Cast to any @c daeURI, as the @c size_on_stack is irrelevant.
	 */
	inline operator const daeURI&()const{ return *(daeURI*)this; }

	/**ZAE
	 * Extract relative path given a URI that is known to be a base.
	 * @param baseURI has a path that is a substring of @c this URI.
	 */
	inline daeName operator-(const daeURI_base &baseURI)const
	{
		assert(transitsURI(baseURI));		
		size_t base = baseURI.getURI_uptoCP<'?'>()+1;		
		size_t size = getURI_uptoCP<'?'>(); //Assuming ? is nonviable!!		
		return daeName(data()+base,size-base);
	}

	/**WARNING, LEGACY
	 * @c std::string Assignment Operator. 
	 * @warning Pre-2.5 this would have resolved the URI. Post-2.5
	 * it would logically be good if operator= had identical semantics
	 * to the constructor. Or would it? But the deciding factor for
	 * a conditional resolve is the @c daeDoc URIs: They don't require
	 * resolution--they are automatically resolevd by @c setURI(). 
	 * That means that sometimes the URI is resolved, like it or not!
	 * Therefore, a conditional resolve is done, so that the behavior
	 * can at least be predictable.
	 */
	inline daeURI &operator=(const daeBoundaryStringIn &cp)
	{
		bool resolved = getIsResolved();
		_setURI(cp.c_str); if(resolved!=getIsResolved()) resolve(); return *this;
	}		
	/**DEFAULT-ASSIGNMENT OPERATOR
	 * Luckily @c daeURI_base doesn't have any string converting 
	 * constructors. Otherwise this would convert string-like arguments
	 * int @c daeURI.
	 */
	inline daeURI &operator=(const daeURI_base &cp)
	{
		if(!getIsAttached()) _reparent(cp.getParentObject());
		daeArray<daeStringCP,COLLADA_MAX_PATH> rel; 
		return operator=(cp.getURI_baseless(rel).data());		
	}

	template<class T, int ID, class CC, typename CC::_ PtoM>
	/**
	 * This seems unavoidable in order to not write versions for every kind
	 * of string-like object. Restricting it to @c daeURI could be a useful
	 * way to introduce type-safety above and beyond what's normal. 
	 * @remarks C++ prefers this constructor over the object-aware cast due
	 * to the template based @c daeURI_size::daeURI_size().
	 */
	inline daeURI &operator=(const DAEP::Value<ID,T,CC,PtoM> &cp)
	{
		const daeObject *o = &dae(cp.object()); _xml_base(o,(T*)nullptr);
		if(!getIsAttached()) _reparent(*o); return operator=(cp->data());		
	}
	static void _xml_base(const daeObject* &o, xmlBase *p)
	{
		o = &o->getParentObject(); daeURI_size<> *check = p; (void)check;
	}
	static void _xml_base(...){}
	
COLLADA_(public) //daeSafeCast() SHORTHANDS

	/** Pass-Through; Follows style of daeElement::a(). */
	template<class T> T *a(){ return (daeURI*)this; }
	/**CONST-FORM Following style of daeElement::a(). */
	template<class T> const T *a()const{ return (daeURI*)this; }

COLLADA_(public) //ACCESSORS & MUTATORS
	/** 
	 * Gets the internal @c resolved flag. A resolved URI is considered
	 * to be in its canonical form. 
	 * @see @c daePlatform::resolveURI().
	 */
	inline bool getIsResolved()const{ return _getFlags().resolved!=0; }
	/** 
	 * Sets the internal @c resolved flag. A resolved URI is considered
	 * to be in its canonical form. 
	 * @see @c daePlatform::resolveURI().
	 */
	inline void setIsResolved(bool y=true){ _getFlags().resolved = y?1:0; }

	/** 
	 * Gets a flag that tells @c daePlatform::openURI() that it
	 * can resort to opening a pure @c domAny based document.
	 */
	inline bool getAllowsAny()const{ return _getFlags().any!=0; }
	/** 
	 * Sets a flag that tells @c daePlatform::openURI() that it
	 * can resort to opening a pure @c domAny based document.
	 */
	inline void setAllowsAny(bool y=true){ _getFlags().any = y?1:0; }

	/** 
	 * Gets a flag that makes @c this URI permanently attached to its
	 * parent object. (@c daeDoc URIs rely on this.)
	 */
	inline bool getIsAttached()const{ return _getFlags().attached!=0; }
	/** 
	 * Sets a flag that makes @c this URI permanently attached to its
	 * parent object. (@c daeDoc URIs rely on this.)
	 */
	inline void setIsAttached(){ _getFlags().attached = true; }

	/**ZAE
	 * Gets a flag that tells if @c this URI needs a / if it's a base.
	 */
	inline bool getIsDirectoryLike()const{ return _getFlags().directory_like!=0; }
	/**ZAE
	 * Sets a flag that tells if @c this URI needs a / if it's a base.
	 */
	inline void setIsDirectoryLike(){ _getFlags().directory_like = true; }
	
	/**
	 * Sets the pointer to the @c daeObject that contains this ref.
	 * @param c the containing @c daeObject.
	 */
	inline daeOK setParentObject(const DAEP::Object *c)
	{
		if(getIsAttached())
		return DAE_ERR_INVALID_CALL; return _reparent(dae(*c));
	}
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("setParentObject")
	/**LEGACY
	 * Sets the pointer to the @c daeElement that contains this ref.
	 * @param c Pointer to the containing @c daeElmement.
	 */
	inline void setContainer(const DAEP::Object *c)
	{
		if(setParentObject(c==nullptr?*this:(daeObject&)*c)!=DAE_OK) assert(0);
	}
	#endif //COLLADA_NODEPRECATED	

	/**
	 * This helps to parse backslashes. 
	 * #define COLLADA_DOM_TOSLASH to change its behavior.
	 * @see @c resolve_RFC3986::toslash.
	 */
	NOALIAS_LINKAGE static daeStringCP toslash(daeStringCP CP)
	SNIPPET( return COLLADA_DOM_TOSLASH(CP)?'/':CP; )

	/**
	 * @c refresh() replaces "setContainer(getContainer())". 
	 * @note setContainer is not always a valid operation.
	 *
	 * What @c refresh() does is @c setResolved(false) and 
	 * have the URI resolved again, in case its base changes.
	 * It's not expected to change the URI appreciably, hence
	 * it is a @c const expression.
	 *
	 * @return Returns @c daePlatform::resolveURI() or if 
	 * if no DOM is available: @c DAE_ERR_NOT_IMPLEMENTED. 
	 */
	LINKAGE daeOK refresh()const;	

	/**
	 * Similar to refresh, but @c getIsResolved() is consulted
	 * and the URI is resolved against @a DOM, versus @c getDOM().
	 * @return Returns @c DAE_ERR_INVALID_CALL to indicate no change.
	 * @note @resolve() does an assertion check if the flag is not set.
	 * @param DOM @c getDOM() is used if it is @c nullptr or unspecified. 
	 */
	LINKAGE daeOK resolve(const daeArchive *DOM_or_ZAE=nullptr)const;	
			 
	/**SCHEDULED FOR REMOVAL?
	 * Pre-2.5 this was "reset" and marked as internal, 
	 * -but it cannot hurt to expose a std::string like method.
	 */
	inline void clear(){ _setURI(empty_daeString1); }

	/** 
	 * Removes the fragment by changing its '#' to '\0'.
	 * @return Returns the fragment without '#' as a view.
	 * @see @c erase()
	 */
	inline daeName erase_fragment(){ return erase(getURI_fragmentCP()); }
	/** 
	 * @param CP is a codepoint to become the new 0-terminator.	 
	 * @return Returns the erased section starting after @a CP.
	 * (Often @a CP is a separator. If not, ignore the output.)
	 * @see @c erase_fragment()
	 */
	inline daeName erase(size_t CP)
	{
		short zt = (short)CP; if(zt<_size-1)
		{
			if(zt<_authority) _authority = (char)zt;
			for(short *it=&_authority_password;it<&_size;it++)
			if(*it>zt) *it = zt;
			//This copies up to _size so the returned view is intact.
			daeRefString<> &rs = _refString();
			if(rs.isView()) rs.setString(*this,rs.getString(),_size);			
			const_cast<daeStringCP&>(data()[zt]) = '\0';
			//This compenstates for the branch. The branch is needed
			//in order to ensure the return is 0-terminated, even if
			//CP is the 0-terminator itself--it's not bound checking.
			zt--; 
		}
		else zt = _size-2;		
		daeName erased(data()+zt+1,_size-zt-2);
		_size = zt+2; return erased;
	}

	/**Standard Library support 
	 * Tells if @c size() is 0.
	 * If @c clear() exists, then it follows that @c empty() should. 
	 */
	inline bool empty()const{ return _size<=1; }
	/**Standard Library support 
	 * Gets the size minus, excluding the 0 terminator.
	 * If @c clear() exists, then it follows that @c size() should. 
	 */
	inline size_t size()const{ return _size-1; }
	/**Standard Library support 
	 * This is an alternative to writing "getURI().view," which is
	 * admittedly, kind of a pain in the ass.
	 */
	inline daeString data()const{ return _refString()._varray; }

	/**
	 * @return Returns @c true if the unresolved URI is absolute. 
	 */
	inline bool isAbsoluteURI()const{ return _rel_half==0&&_path!=0; }	
	/** 
	 * @return Returns @c true if the unresolved URI is relative. 
	 */
	inline bool isRelativeURI()const{ return _rel_half!=0||_path==0; }	
	/** 
	 * @return Returns @c true if the unresolved URI is a # only.
	 */
	inline bool isFragmentURI()const{ return _rel_half==_fragment-1; }	
	/**
	 * @return Returns @c true if the unresolved URI ends in a /.
	 */
	inline bool isDirectoryURI()const
	{
		daeString pp = data(), p = pp+_query; 
		if(p!=pp&&'?'==p[-1]) p--;
		if(p!=pp&&'/'==toslash(p[-1])) return true; return false;
	}

	template<class C_str>
	/**WARNING
	 * @warning The base URL cannot alter the parent/container object.
	 *
	 * Setter function for setting the URI, noting the URL separators.
	 * The URI is not altered from @a URI. It is marked as unresolved.
	 *
	 * LEGACY 
	 * This overload permits a base-URL, that is permitted to be @c nullptr.	 
	 * @baseURL is described in the other @c setURI() overload's Doxygentation.
	 * @return Returns DAE_ERR_INVALID_CALL if @a baseURL cannot work as the base,
	 * -where a base is required; or if @c getIsClosed()==true.
	 */
	inline daeOK setURI(const C_str &URI, const daeURI *baseURL=nullptr)
	{
		return _setURI(daeBoundaryStringIn(URI).c_str,baseURL);
	}	
	template<class Base>
	/**WARNING
	 * @warning The base URL cannot alter the parent/container object.
	 *
	 * Setter function for setting/concatenating the URL.
	 * If @c URL is relative, the full URI is @a baseURL's directory, with @a URL 
	 * appended to the back. Any ../ directives between the two are stripped away
	 * along with the corresponding directories of @a baseURL. 
	 * The relative URL can be reconstructed via @c getURI_baseless(), however it
	 * is not possible to get @a baseURL back, because A) it may've had a file part,
	 * and B) the URL may've had ../ parts. The reason for this, is to be able to
	 * move the URL to a new base/like set of documents.
	 * 
	 * @a baseURL should either be resolved, or a strictly correct URL string; As 
	 * if constructed from a string, then there is no context by which to resolve.
	 * @return Returns @c DAE_ERR_INVALID_CALL if @a URL won't base on @a baseURL.
	 */
	inline daeOK setURI(const Base &baseURL, daeBoundaryStringIn URL)
	{
		return _setURI_base(baseURL,URL.c_str,nullptr);
	}
	template<class Base>
	daeOK _setURI_base(const Base &base, daeString rel, typename Base::__COLLADA__T*)
	{
		const daeURI *baseURL = base; return _setURI(rel,baseURL);
	}
	template<class Base>
	daeOK _setURI_base(const Base &base, daeString rel,...)
	{
		COLLADA_INCOMPLETE(Base) daeURI_parser baseURL(base); 
		return _setURI(rel,baseURL);
	}	

	template<class C_str>
	/**OVERLOAD
	 * This uses some trickery to avoid doing a double copy. It relies on
	 * that resolution will normally make a duplicate of the URI in order
	 * to resolve it. Afterward the duplicate will be its own string, and
	 * the input string won't be copied. If that's not what happens, then
	 * @c setURI_and_resolve() takes it upon itself to make the only copy.
	 *
	 * @param baseURL is for symmetry. If it's not empty then there'll be
	 * two copies as normal. There's a little extra overhead on the front,
	 * -but using this API also avoids calling @c setURI() & @c resolve()
	 * back-to-back exported APIs.
	 *
	 * @return Returns @c void. Returning something is open to discussion. 
	 */
	inline void setURI_and_resolve(const C_str &URI, const daeURI *baseURL=nullptr)
	{
		_setURI_and_resolve(daeBoundaryStringIn(URI).c_str,baseURL);		
	}
	template<class Base>
	/**OVERLOAD
	 * Does @c setURI_and_resolve() with a base-URI.
	 */
	inline void setURI_and_resolve(const Base &baseURL, daeBoundaryStringIn URL)
	{
		return _setURI_base_and_resolve(baseURL,URL.c_str,nullptr);
	}
	template<class Base>
	daeOK _setURI_base_and_resolve(const Base &base, daeString rel, typename Base::__COLLADA__T*)
	{
		const daeURI *baseURL = base; return _setURI_and_resolve(rel,baseURL);
	}
	template<class Base>
	daeOK _setURI_base_and_resolve(const Base &base, daeString rel,...)
	{
		COLLADA_INCOMPLETE(Base) daeURI_parser baseURL(base); 
		return _setURI_and_resolve(rel,baseURL);
	}
	/** Implements template forms of @c setURI_and_resolve(). */
	LINKAGE void _setURI_and_resolve(daeString URI, const daeURI *baseURL)
	SNIPPET
	(		setURI_and_resolve_client_string(URI,baseURL);			
			if(URI==data()) _refString().setString(*this,URI);
	)		
	/**
	 * @param URI will not be buffered. @c refresh() is called, which may
	 * or may not buffer a resolved URI. @c daeURI_size<0> makes sense to
	 * use with this API since it doesn't reserve any space for buffering.
	 * @note @c daeDoc::getBaseURI() uses this to avoid buffering if able.
	 * @see @c daeURI_parser
	 */
	inline void setURI_and_resolve_client_string(daeClientString URI, const daeURI *baseURL=nullptr)
	{
		_refString().setView(URI); _setURI(URI,baseURL); 
		//refresh is desirable as A) resolved==false B) don't want assert().
		refresh(); //resolve(); 
	}

	template<class T> 
	/**
	 * Gets the entire URI to a @a T. 
	 * @see no-argument form's Doxygentation.
	 */
	inline T &getURI(T &io)const
	{
		return _getT(io,_refString().getString(),_size); 
	}	
	/**LEGACY-SUPPORT
	 * Gets the URI. 
	 * Use @c getURI_baseless() to get the relative form.
	 * Use @c getURI_terminatedCP() to get the terminated size.
	 * Use @c getURI_terminatorCP() to get the unterminated size.
	 * (Prefer @c size().)
	 *
	 * @return Returns @c daeName to be symmetric with the 
	 * "getURI" family of APIs and for an expanded feature set.
	 */
	inline daeName getURI()const
	{
		return daeName(_refString().getString(),_size-1);
	} 		  	

COLLADA_(public) //COMPONENT ACCESSORS & MUTATORS
				  
  //////////////////////////////////////////////////////////////////
  //This section used to have many getters and setters, however in//
  //2.5 it proved impractical to continue to maintain them all. It//
  //was not an easily come to decision. It seemed the best course.//
  //(Originally, many of these had dedicated std::string buffers.)//
  //////////////////////////////////////////////////////////////////

	/**LOW-LEVEL
	 * Gets authority component codepoint after :// or the path codepoint. */
	inline CP getURI_authorityCP()const{ return _authority; }
	/**LOW-LEVEL
	 * Gets password subcomponent codepoint after : or the host's @. */
	inline CP getURI_passwordCP()const{ return _authority_password;  }
	/**LOW-LEVEL
	 * Gets host subcomponent codepoint after @ or the port's :. */
	inline CP getURI_hostCP()const{ return _authority_host;  }
	/**LOW-LEVEL
	 * Gets port subcomponent codepoint after : or the path codepoint. */
	inline CP getURI_portCP()const{ return _authority_port;  }
	/**LOW-LEVEL
	 * Gets path component codepoint before / or the query's ?. */
	inline CP getURI_pathCP()const{ return _path;  } 
	/**LOW-LEVEL
	 * Gets filename subcomponent codepoint after / or the query's ?. */
	inline CP getURI_filenameCP()const{ return _path_filename; }
	/**LOW-LEVEL
	 * Gets extension subcomponent codepoint after . or the query's ?. */
	inline CP getURI_extensionCP()const{ return _path_extension; }	
	/**LOW-LEVEL
	 * Gets query component codepoint after ?, or the fragment's #. */
	inline CP getURI_queryCP()const{ return _query; }
	/**LOW-LEVEL
	 * Gets fragment component codepoint after #, or the @c '\0' codepoint. */
	inline CP getURI_fragmentCP()const{ return _fragment; }
	/**LOW-LEVEL
	 * Gets the size of the URI+0 in units of @c daeStringCP codepoints. */
	inline CP getURI_terminatedCP()const{ return _size; }
	/**LOW-LEVEL
	 * Prefer @c size().
	 * Gets the length of the URI in units of @c daeStringCP codepoints. 
	 */
	inline CP getURI_terminatorCP()const{ return _size-1; }

	template<int X> //X can be '://', '@', ':', '/', '.', '?', or '#'.
	/**LOW-LEVEL
	 * @see getURI_upto(), which this parallels.
	 * @note / is equivalent to @c getURI_pathCP().
	 */
	inline CP getURI_uptoCP()const
	{
		daeString URI = data();		
		switch(X)
		{	
		#define _(X,c) case X: return c-(c>0&&URI[c-1]==X?1:0);
		_('@',_authority_host)_(':',_authority_port)_('?',_query)_('#',_fragment)
		_('.',_path_extension)
		#undef _
		case '/': return _path; 
		//worth it? will generate multi-char compiler warnings
		case '://': return _authority>2?_authority-3:0;
		}assert(0); return 0;
		daeCTC<X=='@'||X==':'||X=='/'||X=='.'||X=='?'||X=='#'||X=='://'>();
	}	
	
	//This macro is added for no-argument views. 
	#define _(f) \
	daeName f()const{daeName o;return f(o);}template<class T>T&f(T&io)const
	/**WARNING
	 * Gets the protocol component to a @a T. 
	 * @warning This is setup to support URL like URIs.
	 * It will fail if there is not a :// in the URI, and
	 * just because it is empty, it doesn't mean the URI is
	 * invalid. This is by design. This is not a naming issue.
	 * FYI, THE NAME HERE IS "protocol", SINCE IT'S NOT A SCHEME.
	 *
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_protocol){ return _getURI(io,0,':',std::max(_authority-2,0)); }	
	/** 
	 * Gets the authority component to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_authority){ return _getURI(io,_authority,'\0',_path); }		
	/** 
	 * Gets the username subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_username){ return _getURI(io,_authority,':',_authority_password); }	
	/** 
	 * Gets the password subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_password){ return _getURI(io,_authority_password,'@',_authority_host); }		
	/** 
	 * Gets the getURI_credentials subgroup to a @a T.
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_credentials){ return _getURI(io,_authority,'@',_authority_host); }		
	/** 
	 * Gets the host subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_host){ return _getURI(io,_authority_host,':',_authority_port); }		
	/** 
	 * Gets the port subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_port){ return _getURI(io,_authority_port,'\0',_path); }		
	/** 
	 * Gets the path component to a @a T.
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_path){ return _getURI(io,_path,'?',_query); }		
	/** 
	 * Gets the directory subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_directory){ return _getURI(io,_path,'\0',_path_filename); }		
	/** 
	 * Gets the filename subgroup to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_filename){ return _getURI(io,_path_filename,'?',_query); }		
	/** 
	 * Gets the "base-name" subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_filebase){ return _getURI(io,_path_filename,'.',_path_extension); }		
	/** 
	 * Gets the extension subcomponent to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_extension){ return _getURI(io,_path_extension,'?',_query); }
	/**CASE-INSENSITIVE 
	 * @return Returns @c true if *a ext matches the extension. 
	 */
	inline bool getURI_extensionIs(daeBoundaryStringIn ext)const
	{
		return _ieq(getURI_extensionCP(),ext)==getURI_uptoCP<'?'>();
	}	
	/** 
	 * Gets the query component to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	_(getURI_query){ return _getURI(io,_query,'#',_fragment); }		
	/** 
	 * Gets the fragment component to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	daeName getURI_fragment()const
	{
		daeName o; return getURI_fragment(o); 
	}	
	template<class T>
	/** 
	 * Gets the fragment component to a @a T. 
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 */
	T &getURI_fragment(T &io)const
	{
		return _getURI(io,_fragment,'\0',_size); 
	}
	#undef _
	
	template<class T> //daeArray, std::string
	/**WARNING
	 * @warning This one doesn't work on @c daeName.
	 * Gets the path-relative or protocol relative URL.
	 * If the URL was not constructed from a base URL, then
	 * the result is simply to copy the URI in its entirety.
	 */
	inline T &getURI_baseless(T &io, enum dae_clear clear=dae_clear)const
	{
		if(clear) io.clear();
		for(int i=_rel_backtracks;i-->0;io.append("../",3));
		daeRef::_getT<dae_append>(io,data()+_rel_half,_size-_rel_half); 
		return io;
	};

	template<int X, class T>
	/**
	 * Gets ill-defined component groups to a @a T. 
	 * @tparam X can be '@', ':', '/', '.', '?', '#', or '://'...
	 * - '@' is the host part of the authority.
	 * - ':' is the port part of the authority.
	 * - '/' is the path part.
	 * - '.' is the extension part of the path.
	 * - '?' is the query part.
	 * - '#' is the fragment part.
	 * - '://' is equivalent to @c getURI_protocol(). (It is "Multichar.")
	 * @tparam T can be daeName, daeArray, std::string, etc.
	 * Here ':' is the port part. It's somewhat cryptic, but there are not
	 * names for these things! 
	 * These go "upto" (up to) the separator (whether it exists in the URI
	 * or not) and so, for example, '/' can be read as ':', but with ports.
	 * @param from lets you strip off the front. It will be optimized away.
	 */
	inline T &getURI_upto(T &io, size_t from=0)const
	{
		CP cp = (CP)from; //C4267
		switch(X) //CP should be unsigned. So from=0 can be optimized away.
		{
		case '@': return _getURI(io,cp,'@',std::max<CP>(cp,_authority_host));
		case ':': return _getURI(io,cp,':',std::max<CP>(cp,_authority_port));
		case '/': return _getURI(io,cp,'\0',std::max<CP>(cp,_path));
		case '.': return _getURI(io,cp,'.',std::max<CP>(cp,_path_extension));
		case '?': return _getURI(io,cp,'?',std::max<CP>(cp,_query)); 
		case '#': return _getURI(io,cp,'#',std::max<CP>(cp,_fragment)); 			
		//Just for parity with getURI_uptoCP and Doxygentation.
		//worth it? will generate multi-char compiler warnings.
		case '://': assert(0==from); return getURI_protocol(io);
		}
		assert(0); return io; 
		daeCTC<X=='://'||X=='@'||X==':'||X=='/'||X=='.'||X=='?'||X=='#'>();
	}	
	template<char X> 
	/**OVERLOAD
	 * Gets a temporary @c daeName of @c getURI_upto<X>().
	 * @see two-argument overloads's Doxygentation.
	 */
	daeName getURI_upto()const{ daeName o; return getURI_upto<X>(o); }
	
COLLADA_(public) //UTILITIES

	template<class T> //T is daeDocRef or const_daeDocRef
	/**
	 * @note @c daeDOM is based on @c daeArchive.
	 * Looks up a documents, matching this URI, sans-fragment.
	 * Lookup is confined to @a archive, and may store in @a matchingDoc,
	 * -one of @c nullptr; @a archive itself; one of @a archive's docs, 
	 * -or one of any archives housed inside of @a archive, or any
	 * of such archives's docs.
	 * @return Returns @a matchingDoc. 
	 *
	 * IMPLEMENTATION NOTE
	 * ===================
	 * This API sets up @c daeArchive::_whatsupDoc so that the library's
	 * classes can retrace the lookup. This is less so to accelerate the
	 * library, and more so to avoid reimplementing the lookup procedure.
	 *
	 * AUTOMATIC-RESOLUTION
	 * ====================
	 * There had been a docLookup2 API that tried unresolved and
	 * then tried again resolved, but it's clumsy and so if that
	 * is what the user wants they can use setIsResolved instead.
	 */
	inline T &docLookup(const daeArchive *archive, T &matchingDoc, enum dae_clear clear=dae_clear)const
	{
		if(clear!=dae_default) matchingDoc = nullptr;
		const daeDoc *upcast = matchingDoc; (void)upcast;		
		_docLookup(*archive,(daeDocRef&)matchingDoc); return matchingDoc;
	}
	template<class T> //T is daeDocRef or const_daeDocRef
	/**
	 * Does a lookup, and failing that, resolves & retries.
	 * @see docLookup().
	 */
	inline T &docLookup2(const daeArchive *archive, T &matchingDoc)const
	{
		docLookup(archive,matchingDoc);
		if(matchingDoc==nullptr&&!getIsResolved())
		{
			//daeArchive is undefined here.
			if(DAE_OK==resolve((daeArchive*)((daeObject*)archive)->getDOM()))
			docLookup(archive,matchingDoc,dae_default);
		}
		return matchingDoc;
	}

	/**SCOPED-ENUM 
	 * Options for use with @c resolve_RFC3986().
	 *
	 * @c toslash Uses @c daeURI::toslash() to convert the baseless URI.
	 *
	 * @c lower Takes ASCII in the scheme and hostname parts, converting
	 * it to lowercase; unless the ASCII is a percent-encoded hexadecimal 
	 * digit, in which case RFC3986 calls for uppercase A, B, C, D, E & F.
	 *
	 * @c normalize Removes ./.. directives from the path, and may do more.
	 *
	 * @c decode Converts percent-encodings into non-ASCII/UTF8 code points.
	 * THIS IS KEEPING WITH THE LIBRARY'S PENCHANT FOR BINARY REPRESENTATION.
	 *
	 * @c rebase bases the URI on @c baseLookup(). Without this, the resulting
	 * URI is relative (if not an absolute-URL) in which case the caller should
	 * do this themself after @c resolve_RFC3986() returns.
	 *
	 * @c ALL should NOT be ~0, so that callers won't get more than was asked for.
	 * (It's a default parameter of @c resolve_RFC3986().)
	 */
	struct RFC3986{ enum{ toslash=1, lower=2, normalize=4, decode=8, rebase=16, ALL=0x1F }; };
	/**HELPER 
	 * @see @c daePlatform::resolveURI().
	 * @see https://tools.ietf.org/html/rfc3986
	 * @see @c daeURI_base::RFC3986 Doxygentation.
	 * This does RFC3986's resolution/canonicalization recommendations.
	 * @remarks This does not resolve to an element. @c daeRef::get() does.
	 * @param RFC3986_ops are bitwise options, taken from @c daeURI_base::RFC3986.
	 * @return Returns the result of @c setURI().
	 */
	LINKAGE daeOK resolve_RFC3986(const daeArchive &DOM_or_ZAE, int RFC3986_ops=RFC3986::ALL);

	/**HELPER
	 * @c baseLookup() depends on these conditions to produce a correct URI.
	 * It uses @c getDoc() to get the base, or defaults to default-base-URLs.
	 * @see @c baseLookup()
	 * @see @c resolve_RFC3986() for an example of how to use this to resolve
	 * a URI while satifying the constness guarantees of the @c setURI() APIs.
	 */
	inline bool is_baseLookup_friendly()const
	{
		return _path==0||_rel_half==_authority||isRelativeURI()&&nullptr!=getDoc();
	}
	/**WARNING, HELPER
	 * This returns the inputted @a base if @c getDoc()==nullptr and the default
	 * URLs would produce an incorrect base.
	 * 
	 * This implements some rather convoluted logic for locating the natural base
	 * of a URL, based on A) its parent object, or B) via @a DOM's default base URL. 
	 * It's not that tricky, except for the @c this==base cases. It needs to work for
	 * all URLs.
	 *
	 * @remarks If the URL is an absolute-URL, then the base is not generally required.
	 * @c daeDoc::getBaseURI() is now used to incorporate xml:base attributes.
	 * @c daePlatform::resolveURI() needs to implement something like this, so this is 
	 * to ease development of coding a resolver. 
	 * @see resolve_RFC3986().
	 *
	 * @param base will not be @c nullptr upon return.
	 *
	 * @return Returns @a base unaltered if @c false==is_baseLookup_friendly().
	 */
	LINKAGE const_daeURIRef &baseLookup(const daeArchive &DOM_or_ZAE, const_daeURIRef &base)const;

	/**WARNING
	 * @warning Doesn't say the URIs are resolved!!
	 * @return Returns @c true if @c this URI references @a cmp. 
	 */
	inline bool referencesURI(const daeURI &cmp)const
	{	
		return getURI_upto<'#'>()==((daeURI_base*)&cmp)->getURI_upto<'#'>();
	}
	/**WARNING
	 * @warning Doesn't say the URIs are resolved!!
	 * @return Returns @c true if @c this URI is based on @a cmp AND not equal.
	 */	
	inline bool transitsURI(const daeURI &cmp)const
	{	
		daeName a = getURI_upto<'#'>();
		daeName b = ((daeURI_base*)&cmp)->getURI_upto<'#'>();

		if(b.extent<a.extent) a.extent = b.extent; 

		return a==b&&(0==b.extent||'/'==b[b.extent-1]||'/'==a[b.extent]);
	}

	/**XML-BASE
	 * @warning Doesn't say the URI is resolved!! ALSO this now looks for
	 * xml:base or whatever strategy is appropriate to elements/documents.
	 * Basically if not a #fragment URI it's assumed the caller knows the
	 * URI is resolved as-is if unresolved; e.g. @c resolve().
	 *
	 * Tells if the URI refers to part of the document that it is part of.
	 * This is actually very tricky. See the following dicussion for some
	 * background. Bottom-line, if @c true, @c getDoc() is the fragment's
	 * document, or if fragmentless, the URI is the document's, I believe.
	 * I don't have a source for how to interpret self-referring xml:base.
	 *
	 *   http://lists.xml.org/archives/xml-dev/201705/msg00017.html
	 *
	 * @remarks This does the inverse of deprecated "isExternalReference".
	 */
	inline dae3ool isSameDocumentReference()const
	{	
		const_daeDocRef doc = getDoc();
		if(doc!=nullptr)
		return isFragmentURI()?true:((daeURI_size<>*)this)->referencesDoc2(doc);
		return dae3ool::is_neither;
	}
	#ifndef COLLADA_NODEPRECATED
	/**LEGACY, UNUSED (INTERNALLY)
	 * Tells if the URI refers to a document other than its own.
	 * @warning Doesn't guarantee a document is a parent object.
	 */
	inline bool isExternalReference()const{ return !isSameDocumentReference(); }
	#endif //COLLADA_NODEPRECATED

COLLADA_(private) //INTERNAL SUBROUTINES

	/** Implements @c setURI() via C-style string. */
	LINKAGE daeOK _setURI(daeString URI, const daeURI *baseURI=nullptr);	
	COLLADA_NOINLINE //PRETENDING INVISIBLE
	/** Implements concatenation phase of @c _setURI(). */
	void _setURI_concat(const daeURI&, size_t, daeString);	
	/** Efficiently/quietly rewrites any nested URIS. */
	void _setURI_rebaseNest(size_t,daeArray<>&);

	template<class T> //T can be daeArray or std::string or daeName.
	/** Implements many daeURI methods, with std::string compatible code. */
	inline T &_getURI(T &str, short pos, daeStringCP sep, short end)const
	{
		daeString URI = data(); if(end==0||URI[end-1]!=sep) end++;
		daeRef::_getT<dae_clear>(str,URI+pos,end-pos); return str;
	}

	/** Does case-insensitive comparisons. */
	inline short _ieq(short i, daeString cmp)const
	{
		for(daeString URI=data();*cmp!='\0';)
		if(tolower(URI[i++])!=tolower(*cmp++)) return -1; return i;
	}
	
	template<int docLookup>
	/**
	 * Implements @c docLookup() and doc insertion internally.
	 * @param reinsert If @c nullptr, it's a matching/lookup procedure.
	 * @tparam docLookup was added so @c docLookup can work with @c dae_default.
	 */
	inline void _docHookup(daeArchive&,daeDocRef &reinsert)const;
	/** Implements @c docLookup() */
	LINKAGE void _docLookup(const daeArchive&,daeDocRef&)const;
													   		
COLLADA_(public) //daeRef_support traits

	using daeRef_support::_eq;
	/** Implements @c operator==() without conversion. */
	inline bool _eq(const daeURI_base *cmp)const
	{
		assert(getIsResolved()&&cmp->getIsResolved());
		return _size==cmp->_size&&0==memcmp(data(),cmp->data(),_size);
	}
	/** Implements @c operator>() without conversion. */
	inline bool _gt(const daeURI_base *cmp)const
	{
		assert(getIsResolved()&&cmp->getIsResolved());
		return strcmp(data(),cmp->data())>0;
	}
	/** Implements @c operator>() without conversion. */
	inline bool _lt(const daeURI_base *cmp)const
	{
		assert(getIsResolved()&&cmp->getIsResolved());
		return strcmp(data(),cmp->data())<0;
	}	
};

#include "../LINKAGE.HPP" //#undef LINKAGE

/**HELPER
* This is a clever way to use a @c daeURI to parse @a URL.
* It works by assigning the C-string to the @c daeURI as a 
* @c daeStringRef, even if it is not actually one. It won't
* matter. @c _setURI() will not reset the string, since it's
* the current string, but it will parse it.
*
* Such a URI cannot be part of a @c daeDatabase, and so it's
* safe to use, as long as the C-string pointer remains valid.
*
* It's tempting to pass this class to APIs that expect a URI.
* However, if it is resolved, while it will work, it is small,
* -and so will almost surely touch the heap.
*/
class daeURI_parser : public daeURI_size<0>
{	
COLLADA_(public)
	/**
	 * Default Constructor
	 */
	daeURI_parser(enum dae_clear clr=dae_clear):daeURI_size(clr){}

	template<class T>
	/**
	 * Constructor, optimized to not call @c clear() unnecessarily. 
	 */
	daeURI_parser(const T &URL)
	//daeURI_parser_view is a dummy class.
	COLLADA_SUPPRESS_C(4355)
	:daeURI_size(*(class daeURI_parser_view*)daeBoundaryStringIn(URL).c_str,this){}

	template<class T>
	/**
	 * Constructor, optimized to not call @c clear() unnecessarily. 
	 */
	daeURI_parser(const T &URL, const DAEP::Object *c)
	//daeURI_parser_view is a dummy class.
	:daeURI_size(*(class daeURI_parser_view*)daeBoundaryStringIn(URL).c_str,c){}
};


/**WARNING
 *
 * COLLADA C++ URI that initially lives on the stack, until it outgrows it. 
 *
 * @warning While it is correct--or intended--for @c daeURI to have URI in
 * its class name, and it is technically a URI, its features cater to URLs.
 * It can represent a URI, that can even be resolved into a URL. Resolvers
 * must manually parse non-URL URIs. A URL-like URI uses an authority-part
 * that begins with a // sequence. The scheme part of a URL is referred to
 * as a "protocol", in order to avoid confusion. A URI has a "protocol" if
 * :// separates its scheme and authority-parts. Note the authority can be
 * empty. Usually an empty authority resolves to the system's "localhost."
 */
class daeURI : public daeURI_size<COLLADA_MAX_PATH>
{
	//NOTE: This is its own class first-and-foremost because Visual Studio
	//IntelliSense fails to parse daeURI_size<COLLADA_MAX_PATH> or however
	//daeURI appears in daeURI_base/daeURI_size. daeURI_size<1> is made to
	//be the sacrifical template instance, as it's identical to 0, since a
	//daeRefString<0> cannot have a 0 sized array. Another benefit of this
	//is not having the template parameter in exported function signatures.
	//The down side is forwarding the constructors and assignment operator.
	//NOTE: daeIDREF and daeSIDREF avoid the IntelliSense problem by using
	//a template for their base class.

COLLADA_(public)
	/**FOWARDING CONTRUCTOR
	 * @see daeURI_size::daeURI_size
	 * @note This pattern is for IntelliSense and better export signatures.
	 */
	daeURI(){}

	template<class S, class T>
	/**FOWARDING CONTRUCTOR
	 * @see daeURI_size::daeURI_size
	 * @note This pattern is for IntelliSense and better export signatures.
	 */
	daeURI(const S &cp1, const T &cp2):daeURI_size(cp1,cp2){}

	template<class T>
	/**FOWARDING CONTRUCTOR
	 * @see daeURI_size::daeURI_size
	 * @note This pattern is for IntelliSense and better export signatures.
	 */
	daeURI(const T &cp, typename DAEP::NoValue<T>::type SFINAE=0):daeURI_size(cp){}
	
	template<class T> 
	/**FOWARDING ASSIGNMENT-OPERATOR
	 * @see daeURI_size::daeURI_size
	 * @note This pattern is for IntelliSense and better export signatures.
	 */
	daeURI &operator=(const T &cp){ return daeURI_base::operator=(cp); }
		
	/**C++
	 * Assignment operator
	 */
	daeURI &operator=(const daeURI &cp){ return daeURI_base::operator=(cp); }
};	

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_URI_H__
/*C1071*/
