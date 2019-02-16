/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_DOCUMENT_H__
#define __COLLADA_DOM__DAE_DOCUMENT_H__

#include "daeElement.h"
#include "daeIOPlugin.h"

COLLADA_(namespace)
{//-.
//<-'

/**SCOPED-ENUM
 * @c HARDLINK and @c ARCHIVE are implemented.
 */
struct daeDocType
{
	/**EXPERIMENTAL
	 * Class @c daeDoc is one of the following:
	 * - A @c daeDocument.
	 * - A hard-link to a @c daeDocument.
	 * - A symbolic-link (symlink) to a @c daeDoc.
	 * - A @c daeArchive.
	 * - A @c daeDOM, which is also a @c daeArchive.
	 * @see @c daeDoc Doxygentation's details.
	 * 
	 * @c ORPHANAGE is a proposal so free elements can
	 * reliably report a document. It could be a DOM's
	 * @c _link member; though that could be a problem.
	 * Probably @c daeDOM::_closedDocs._link is better.
	 */
	enum{ ORPHANAGE=0,HARDLINK,SYMLINK,ARCHIVE };	
};

#include "../LINKAGE.HPP" //#define LINKAGE

/**
 * Class @c daeDoc is one of the following:
 * - A @c daeDocument.
 * - A hard-link to a @c daeDocument.
 * - A symbolic-link (symlink) to a @c daeDoc.
 * - A @c daeArchive.
 * - A @c daeDOM, which is also a @c daeArchive.
 *
 * @remarks Note that a DOM is not your system's file system.
 * A hard-link means if the document is closed, the hard link keeps it open.
 * A symbolic link means, if the document is closed, it's closed, and the symlink retains
 * a link to an empty document (unless it is reopened.)
 */
class daeDoc : public daeObject
{
	friend class daeDOM;
	friend class daeArchive;
	friend class daeDocument;		

COLLADA_(protected) //DATA-MEMBERS
	//SUB-OBJECT
	/**
	 * URI of the document, archive, or DOM working directory. 
	 *
	 * @note Embedding the URI in the document would conflate
	 * their reference-counters. To even consider it requires
	 * a generic way to enumerate embedded objects whenever a
	 * reference counter becomes 0.
	 *
	 * ATTENTION
	 * @c _uri has special meaning to @c daeDOM. It is always
	 * empty. This is especially important to @c daeStringRef.
	 * Its prototype-constructor is relying on the visibility
	 * of @c _uri.
	 * @see @c daeDOM::getEmptyURI() Doxygentation.
	 */
	daeURI_size<0> _uri;
	/**ZAE 
	 * If ARCHIVE, this holds the ZAE's root DAE. 
	 */
	daeDocRef _link;
	/**ZAE 
	 * If @c _archive==this, then this is the @c daeDOM.
	 */
	daeArchive *_archive; 
		
COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeDoc)

COLLADA_(public) //ABSTRACT INTERFACE

	/**ABSTRACT-INTERFACE
	 * Initiates the "teardown" sequence.
	 * Doing this as @c virtual makes it easier to
	 * call, and looks forward to possible plugin extension.
	 * ABSTRACT?
	 * @c assert(0) is for C2259 when creating @c daeDOM on 
	 * the stack. Its constructor is exported, so it should
	 * assign the correct "vptr" and not this one.
	 */
	virtual void __daeDoc__v1__atomize(){ assert(0); /* = 0;*/ }
	/** Pro-forma. Should never be called. */
	inline void __COLLADA__atomize(){ return __daeDoc__v1__atomize(); }

COLLADA_(public/*protected*/)
	/** 
	 * Implements @c write() and @c writeTo(). 
	 * @see @c daeDocument::_write().
	 */
	inline daeError _write(const daeURI &URI, daeIOPlugin *O)const
	{
		const_daeDocRef _ = this;
		return _write2(_,daeIORequest(_archive,nullptr,&getDocURI(),&URI),O);
	}

	/** 
	 * Implements "write" analogue of @c daeArchive::_read2().
	 *
	 * ZAE
	 * @param IO is to supply a privately arranged I/O channel for a
	 * plugin to use. Perhaps a pass-through, or to fill up a buffer.
	 * It is not closed.
	 * @see @c daeIORequest::fulfillRequestO().
	 */
	LINKAGE static daeError _write2(const_daeDocRef&,const daeIORequest&,daeIOPlugin*,daeIO*IO=nullptr);

COLLADA_(public)
	/**
	 * @return Returns HARDLINK, SYMLINK, or ARCHIVE. 
	 */
	inline int getDocType()const{ return 3&getObjectTags(); } 
	
	/**
	 * @return Returns @c true if this @c daeDoc is a @c daeDocument.
	 */
	inline bool isDocument()const
	{
		//At some point, _link==nullptr should be sufficient.
		return _link==nullptr&&getDocType()==daeDocType::HARDLINK; 
	}
	/**
	 * @return Returns @c true if this @c daeDoc is a @c daeArchive,
	 * -excluding the global pseudo-archive, that may or may not be a
	 * @c daeDOM. (It is until a DOM-as-an-archive mode is ever added.)
	 */
	inline bool isArchive()const
	{
		return getDocType()==daeDocType::ARCHIVE&&_archive!=(void*)this; 
	}
	/**
	 * @return Returns @c true if this @c daeDoc is in @a a.
	 */
	inline bool inArchive(const daeArchive *a)const
	{
		const daeArchive *p = _archive;
		while(p!=((daeDoc*)p)->_archive&&a!=p)
		p = ((daeDoc*)p)->_archive; return a==p;
	}

	/**EXPERIMENTAL	 
	 * @return Returns @c true if the doc has URI semantics.
	 * Docs made by @c daeElement::getProvisionalDocument() 
	 * and @c daeDOM::getWorkingMemory() lack URI semantics.
	 * "URI semantics" means when a URI is resolved it will
	 * find the document if @c hasOwnURI(). Likewise if the
	 * URI is changed to one of another document's, it will
	 * replace that document.
	 */
	inline bool hasOwnURI()const
	{
		//Note: Should be consistent in nested archive scenarios.
		return !_uri.isUnparentedObject(); 
	}

	/**NOT-THREAD-SAFE
	 * Closes the doc, unloading all memory used by the document-or-archive.
	 * With the exception of @c daeDOM, @c daePlatform::closeURI() can veto
	 * the close. Details/override procedures follow:
	 *
	 * @return Returns @c DAE_NOT_NOW if the platform is deciding
	 * to close or not on a different thread from the calling one.
	 * (This doesn't mean the library is thread safe--the application must be.)
	 *
	 * @note Permission is gotten from @c daePlatform::closeURI().
	 * This can be circumvented by getDocURI().clear(), but this is discouraged,
	 * -because it's A) not nice, and B) makes the URI unusable for diagnostics.
	 */
	LINKAGE daeOK close();

	/**
	 * Tells that this doc is surely housed in @c daeDOM::_closedDocs.
	 * @note It seems that docs are temporarily parked when their URI 
	 * is in the process of changing. It looks like @c isClosed() may
	 * appear to be so both during this change, and if the URI is not
	 * set after creating the doc, e.g. the @c daeDocumentRef factory
	 * constructor, or @c daeIOPlugin::addDoc(). Set URIs immediately.
	 */
	inline bool isClosed()const
	{
		union{ void *p; daeDoc *P; }; //daeArchive is derived from daeDoc.
		for(p=_archive;p!=P->_archive;)p=P->_archive; return !P->_isDOM(); //-Wempty-body
	}

	/**LEGACY-SUPPORT
	 * Writes doc to its archive/document's URI.
	 * If @c this is a @c daeDOM its docs are written. The library does this unless
	 * @a O implements @c daeIOPlugin::writeRequest() or built-in I/O plugins do so.
	 */
	inline daeOK write(daeIOPlugin *O=nullptr)const{ return _write(_uri,O); }
	/**WARNING, LEGACY-SUPPORT
	 * @warning If @c this is a @c daeDOM, this API behaves differently if a plugin
	 * does not takeover. In which case, @c daeURI::transitsURI(URI) is called with
	 * each document's URI, in order to filter out documents. In other words, it is
	 * a partial write operation, and the existing documents are overwritten.
	 * 
	 * Writes doc to @a URI.
	 * @param URI If @c this is a @c daeDOM then @a URI is taken to be its base-URI,
	 * -or only its base is considered. A DOM's natural base is the empty URI.
	 */
	inline daeOK writeTo(const daeURI &URI, daeIOPlugin *O=nullptr)const{ return _write(URI,O); }								
	
	/**WARNING
	 * Gets the doc's URI that is used to locate it within the DOM's doc-tree. 
	 * @warning If a doc is created via @c daeSmartRef's @c daeDOM base constructors,
	 * -then the doc uses @c daeDOM::getEmptyURI(). 
	 * The URIs are doc-aware; meaning altering a doc's URI, relocates it in the DOM.
	 */
	inline daeURI &getDocURI(){ return _uri; }
	/**WARNING, CONST-FORM 
	 * Gets the doc's URI that is used to locate it within the DOM's doc-tree. 
	 * @warning If a doc is created via @c daeSmartRef's @c daeDOM base constructors,
	 * -then the doc uses @c daeDOM::getEmptyURI(). 
	 * The URIs are doc-aware; meaning altering a doc's URI, relocates it in the DOM.
	 */
	inline const daeURI &getDocURI()const{ return _uri; }
		
	//NOTE: Using 'noalias' even though _xmlBase is modified, because
	//it's not a global state, or is fully encapsulated. Only the CRT
	//is involved.
	COLLADA_NOALIAS
	/**XML-BASE
	 * Gets an xml:base like base-URI relative to @a c where it is an
	 * element; if an element, it's assumed @c this==dae(c)->getDoc()
	 * and so, @c true==isDocument().
	 * @see daeElement::getBaseURI()
	 * SEE-ALSO @c daeURI::isSameDocumentReference()
	 *
	 * @note It's not a misnomer to call this a "document URI" as the
	 * concept of "same document references" applies to xml:base just
	 * as to the actual document.
	 *
	 *   http://lists.xml.org/archives/xml-dev/201705/msg00017.html
	 *
	 * @return Returns @c &getDocURI() if xml:base attributes are not
	 * present.
	 */
	LINKAGE const_daeURIRef getBaseURI(const DAEP::Object *c)const;

	/** 
	 * Gets the links' archive. 
	 * @return Should not return @c nullptr; although sometimes it is @c nullptr.
	 * (Mid-transfer.)
	 * @remarks Post-2.5 returns @c const types for upstream "getter" APIs. 
	 */
	inline const daeArchive &getArchive()const{ return *_archive; }

	/** 
	 * Gets the links' real document.
	 * "getDocument" is interchangeable with @c daeElement's.
	 * @c this can be @c nullptr. (It's often convenient to blindly convert doc-to-document.)
	 */
	inline daeDocumentRef getDocument()
	{
		return _getDocument(); 
	}
	/**CONST-PROPOGATING-FORM
	 * Gets the links' real document. 	 
	 * "getDocument" is interchangeable with @c daeElement's.
	 * @c this can be @c nullptr. (It's a common scenario, and the algorithm is there.)
	 */
	inline const_daeDocumentRef getDocument()const
	{
		return _getDocument(); 
	}
	/** Implements @c getDocument(). */
	inline daeDocument *_getDocument()const
	{
		const daeDoc *p = this; if(p!=nullptr) while(p->_link!=nullptr) p = p->_link;
		//GCC/C++ want a defined outside the class body, so it can't be instantiated.
		//return const_cast<daeDoc*>(p)->a<daeDocument>();
		return p==nullptr||!p->isDocument()?nullptr:(daeDocument*)p;
	}

	/**LEGACY, CIRCULAR-DEPENDENCY
	 * Accessor to get the database associated with this document.
	 * @return Returns the database associated with this document.
	 */
	inline daeDatabase &getDatabase()const;

#ifdef BUILDING_COLLADA_DOM

COLLADA_(public)
	/**
	 * Constructor
	 */
	explicit daeDoc(daeDOM *a, int dt);
	/**
	 * Constructor of getProvisionalDocument
	 */
	explicit daeDoc(daeArchive *a, int dt);
	/**
	 * Virtual Destructor
	 */
	virtual ~daeDoc(){}

COLLADA_(public) //DAEP::Object methods

	/**PURE-OVERRIDE */
	virtual DAEP::Model &__DAEP__Object__v1__model()const;

COLLADA_(public) //Just a little more typesafe.
	/**
	 * These concern @c _doOperation().
	 * They were @c enum types with specialized structures
	 * but GCC/C++ can't reasonably do explicit-specialization.
	 */
	struct _no_op
	{
		enum{ value=0 }; typedef void type;
	};
	/** The bool is set to true if closeURI returns DAE_OK. */
	struct _close_op
	{
		enum{ value=1 }; typedef bool type;
	};
	/** This is the same string that is passed to setURI. */
	struct _setURI_op
	{
		enum{ value=2 }; typedef const daeStringCP type;
	};

	/**WORKAROUND
	 * @c close() uses this to permit reentry by daePlatform::closeURI().
	 * @c daeURI_base::setURI() uses it to resolve the URIs of documents.
	 */
	int _current_operation,_operation_thread_id;

	template<class op>
	/** 
	 * An "operation" here means that an API needs to be recursive on
	 * the object, and must return @c DAE_NOT_NOW when an other thread
	 * does @c this->_doOperation().
	 * (IT DOESN'T USE INTERLOCKED/ATOMIC OPERATIONS AS-IS, AND PROBABLY
	 * CANNOT ABIDE BY OVERLAPPING OPERATIONS.)
	 *
	 * @param DOM is the doc's DOM. 
	 * @return Returned code is tailored to @a op, and is very sensitive.
	 */
	daeOK _doOperation(const const_daeDOMRef &DOM, typename op::type *arg0=nullptr)
	{
		return _doOperation(op::value,DOM,arg0);
	}

COLLADA_(private) 

	/** Implements the safer template form. */
	daeOK _doOperation(int op, const const_daeDOMRef &DOM, const void *arg0=nullptr);

#endif //BUILDING_COLLADA_DOM

COLLADA_(public) //daeSafeCast() SHORTHANDS (Continued after class.)
	/**
	 * Follows style of daeElement::a().
	 */
	template<class T> inline T *a()
	{
		return daeObject::a<T>();
	}
	/**CONST-FORM
	 * Following style of daeElement::a(). 
	 */
	template<class T> inline const T *a()const
	{
		return const_cast<daeDoc*>(this)->a<T>();
	}
	/***GCC/C++ WANT THE FOLLOWING SPECIALIZATION IN THE NAMESPACE****/
};
/** Pass-Through; Follows style of daeElement::a(). */
template<> inline daeDoc *daeDoc::a<daeDoc>()
{
	return this;
}
/**
 * Follows style of daeElement::a(): @c This->a<daeDocument>(). 
 */
template<> inline daeDocument *daeDoc::a<daeDocument>()
{
	if(daeObject::_this__nullptr(this)||!isDocument())
	return nullptr; return (daeDocument*)this;
}
/**
 * Follows style of daeElement::a(): @c This->a<daeArchive>(). 
 */
template<> inline daeArchive *daeDoc::a<daeArchive>()
{
	if(daeObject::_this__nullptr(this)||!isArchive())
	return nullptr; return (daeArchive*)this;
}

extern bool daeDocument_typeLookup_called;
/**
 * The @c daeDocument class implements a COLLADA document.
 */
class daeDocument : public daeDoc
{	
#ifdef _DEBUG
	daeContents *__Natvis_content;
#endif

	friend class daeDOM;
	friend class daeDoc;
	/**
	 * Disabling @c new operator for clients.
	 * @see @c daeDOM::_addDocument().
	 */
	void *operator new(size_t n)
	{
		return ::operator new(n); //NOP
	}	

COLLADA_(public) //OPERATORS
	/** 
	 * Placement-new wants this on MSVC2015. 
	 * There's no reason for this to be @c public,
	 * -but neither is there reason for it to not be so.
	 */
	void *operator new(size_t, void *p){ return p; }

	COLLADA_DOM_OBJECT_OPERATORS(daeDocument)

COLLADA_(public) //DAEP::Object methods

	/**PURE-OVERRIDE */
	virtual void __daeDoc__v1__atomize();

COLLADA_(public) //INTERNALS	
	
	friend class daeAST::TypedUnion;
	/**LOW-LEVEL
	 * Gets @c getContents().getElement(). The attributes of
	 * this element are not read/written by I/O plugins, and
	 * so can be freely used by client code to manage states.
	 * @c getPseudoElement().hasAttribute_xml_base() relates
	 * to the presence of xml:base attributes or any kind of
	 * base at any point within @c this document. The pseudo
	 * element is a part of the document and a @c daeElement.
	 *
	 * @remark Databases must implement element-migration in
	 * which the element graph is traversed in response to a
	 * change-notice that moves an element from one document
	 * or from no-document, to another document. In order to
	 * use xml:base @c getPseudoElement().getMigrationByte()
	 * must be used to flip-on @c daeMigrationByte::XML_BASE.
	 */
	NOALIAS_LINKAGE daePseudoElement &getPseudoElement()
	SNIPPET( return *_pseudo.element(); )
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */
	inline daePseudoElement &getPseudoElement()const
	{
		return const_cast<daeDocument*>(this)->getPseudoElement();
	}

#if defined(BUILDING_COLLADA_DOM) || defined(__INTELLISENSE__)
													
COLLADA_(private) //INTERNALS	

		/** @see getFragment() doxygentation. */
		mutable daeStringRef _fragment;

		friend class daeURI_base;	
		/** Implements @c daeDoc::getBaseURI(). */
		mutable daeStringMap<daeURIRef> _xmlBase;

		/** Implements @c getSchemata(), @c _get_and_or_use_xs_schema(). */
		mutable std::vector<std::pair<size_t,const XS::Schema*>> _schemata;

		//GCC (template parameter lists.)
		template<class,unsigned long long,class> 
		friend class DAEP::Elemental;

		struct _PseudoElement 
		:
		public daeElemental<_PseudoElement,32+2>
		{
		public: //COLLADA-DOM 2
			/**
			 * These were deprecated; but are kept for switch-cases.
			 */
			enum{ elementType=daeObjectType::_ };

		public: //Parameters

			typedef struct:Elemental
			{	COLLADA_DOM_0
			_0; COLLADA_WORD_ALIGN
				COLLADA_DOM_Z(0,0)
			DAEP::Value<sizeof(1),dae_Array<>> _Z; enum{ _No=1 };
			DAEP::Value<sizeof(2),daeContents> content; typedef void notestart;
			}_;

		public: //Attributes

			//FOR THE RECORD
			//Technically software could use these, and even I/O plugins
			//could interpret them. But they are not XML and so will not
			//be written out into the document. (They've nowhere to go.)

			/**NO-NAMES
			 * These attributes are invalid according to the schema. They may be user- 
			 * defined additions and substitutes.
			 */
			DAEP::Value<0,xsAnyAttribute,_,(_::_)&_::_0> userdata;

		public: //Complex Content Model

			COLLADA_WORD_ALIGN
			COLLADA_DOM_Z(0,0) 
			/**NO-NAMES
			 * These elements are invalid according to the schema. They may be user-defined 
			 * additions and substitutes.
			 */
			DAEP::Child<1,xsAny,_,(_::_)&_::_Z> unnamed;
			/**
			 * Children, mixed-text, comments & processing-instructions.
			 */
			DAEP::Value<2,daeContents,_,(_::_)&_::content> content;
		};	  
		struct _Pseudo
		{
			daeFeature features[3];
			char model[sizeof(_PseudoElement)];
			char meta[sizeof(daeMeta)+sizeof(_PseudoElement)];			
			daeElement *element()const{ return operator->()->_prototype; }
			daeMetaElement *operator->()const{ return (daeMetaElement*)meta; }			
		}; 		
		/**
		 * This is a self-contained model/metadata arrangement.
		 * It has no process-share nor schema. It's assigned the
		 * schema of its children, which must share a schema.
		 */
		_Pseudo _pseudo;				

COLLADA_(protected)
		/**
		 * Constructor
		 */
		explicit daeDocument(daeDOM*);
		/**
		 * Constructor of getProvisionalDocument
		 */
		explicit daeDocument(daeArchive*);
		/**
		 * Constructs @c _pseudo.
		 */
		inline void _init_pseudo();

COLLADA_(public/*C4624*/) 
		/**
		 * Virtual Destructor
		 */
		virtual ~daeDocument();	
#else 

//C4624: silence warning for the DBase types.
COLLADA_(public) ~daeDocument(){ assert(0); }

#endif //BUILDING_COLLADA_DOM

COLLADA_(public)	
	/**NEVER-NULL
	 * @see @c daeObject::getDOM().
	 */
	inline const daeDOM *getDOM()const
	{
		assert(getParentObject()._isDOM());
		return (daeDOM*)__DAEP__Object__parent;
	}

	/**ZAE
	 * Gets the fragment part of the document's URL that was removed
	 * when @c this document was opened or was later associated with
	 * it. Any action that changes the document's URL can cause this
	 * fragment to change or be cleared. Users are free to change it
	 * to any value at any time, as if changing a browser's location.
	 *
	 * @note This API was added to support the <dae_root> concept of
	 * ZAE archives. @c daeZAEPlugin prefers the URL's fragment over
	 * the fragment supplied by <dae_root>.
	 */ 	
	LINKAGE daeStringRef &getFragment()const
	SNIPPET( return _fragment; )

	/**
	 * Gets the contents-array of this document.
	 * @note "getContents" is designed to match 2.x generated classes.
	 */
	inline daeContents &getContents()
	{
		return getPseudoElement().getContents(); 
	}
	/**CONST-PROPOGATING-FORM
	 * Gets the contents-array of this document.
	 * @note "getContents" is designed to match 2.x generated classes.
	 */
	inline const daeContents &getContents()const
	{
		return getPseudoElement().getContents(); 
	}

	/**
	 * Gets the "pseudo-element" metadata, describing the acceptable
	 * root elements. The CM graph is a degenerate <xs:element> root.
	 * It might be extended to support an <xs:any> like content mode.
	 * If so, this API will probably serve whatever @c setMeta() got.
	 * If @c setMeta() is never called, it will be the @c domAny one.
	 * This is here to go with @c daePlatform::personalizeDocument().
	 */
	inline daeMeta &getMeta()const
	{
		return getPseudoElement().getMeta().jumpIntoTOC(1).getChild();
	}
	/**
	 * It seems harmless to let this be set directly if the document
	 * is empty.
	 * @return This API currently returns @c DAE_ERR_INVALID_CALL if
	 * @c false==getContents().empty().
	 */
	LINKAGE daeOK setMeta(daeMeta&);

	//NOT THREAD-SAFE	
	/**EXPERIMENTAL
	 * Gets XML Schema added by @c daePlatform::introduceToDocument().
	 * @note Presently there is no way to add schemas directly to the
	 * document. It is done when an element is first created/inserted.
	 * (Note that inserted means after elements created-from-thin-air
	 * are inserted into the document.)
	 * @see @c XS::Schema::getUsers().
	 */
	LINKAGE void getSchemata(XS::Schemata &o, enum dae_clear e=dae_clear)const;
	/**MUTABLE
	 * There is not an advertised API for doing this, but if you need
	 * to add schemata to @c getSchemata() then by all means use this.
	 * @return Returns @true if the input schema and output are equal.
	 * The input schema must be initialized. It is a default, and can
	 * be @c nullptr in or out. @c daePlatform::introduceToDocument() 
	 * can change it to @c nullptr.
	 */
	LINKAGE bool _get_and_or_use_xs_schema(daeTags,const XS::Schema*&)const;

	/**
	 * Accessor to get the XML-like ROOT node associated with this document.
	 * @return A @c dae_Array<> reference for the the root of this document.
	 * @remarks Technically 1 means there can be more than one root.
	 */	
	inline dae_Array<daeElement,1> &getRoot()
	{
		return getContents().unnamed(); 
	}
	/**CONST-PROPOGATING-FORM
	 * Accessor to get the XML-like ROOT node associated with this document.
	 * @return A @c dae_Array<> reference for the the root of this document.
	 * @remarks Technically 1 means there can be more than one root.
	 */	
	inline const dae_Array<daeElement,1> &getRoot()const
	{
		return getContents().unnamed(); 
	}
	
	#ifndef COLLADA_NODEPRECATED
	COLLADA_DEPRECATED("Post-2.5: END-OF-SUPPORT\n\
	Use getBaseURI to implement xml:base, or getDocURI().")
	void getDocumentURI()const;
	#endif
				
COLLADA_(public) //LEGACY: old "database" APIs

  /////////////////////////////////////////////////////////////////
  //TODO: Should COLLADA_NOLEGACY apply? Or some other directive?//
  /////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
  //THESE WON'T WORK IF THE daeDatabase INTERFACE DOESN'T FORWARD// 
  //ID/SID NOTES TO daeDocument::_carry_out_change_of_ID_or_SID()//
  //The database must also implement small string pooling, and if//
  //the id/sid strings are too large to qualify they cannot match//
  //ID lookup requires @c daeStringRef::getID_id() to be operable//
  /////////////////////////////////////////////////////////////////

	template<class S, class T> //T is DAEP::Element or daeElement based
	/**WARNING, LEGACY
	 * @warning THIS IS LEGACY FUNCTIONALITY. SEE THE BODY OF @c daeDocument 
	 * FOR HOW TO ENABLE IT, AND HOW IT CAN FAIL IF id IS NOT A SMALL STRING.
	 *
	 * Looks up elements by "id" attribute.
	 * @param id The ID to match on. A # is allowed by removing the first bit
	 * of the string-ref's pointer address. Databases that enable @c idLookup
	 * are responsible for this.
	 * @return Returns @a match.
	 */
	inline daeSmartRef<T> &idLookup(const S &id, daeSmartRef<T> &match, enum dae_clear clear=dae_clear)const
	{
		(void)static_cast<const daeElement*>(dae((T*)nullptr)); //up/downcast
		if(clear!=dae_default) match = nullptr;
		_idLookup(daeBoundaryStringRef(*this,id),(daeElementRef&)match); 		
		if(nullptr==dae(match)->template a<T>()) match = nullptr; return match;
	}
	template<class S, class T> //S is DAEP::Element or daeElement based
	/**WARNING, LEGACY
	 * @warning THIS IS LEGACY FUNCTIONALITY. SEE THE BODY OF @c daeDocument 
	 * FOR HOW TO ENABLE IT, AND HOW IT CAN FAIL IF id IS NOT A SMALL STRING.
	 *
	 * This is a single-argument shorthand with @a S being explicitly stated.
	 * Looks up elements by "id" attribute.
	 * @param id The ID to match on. A # is allowed by removing the first bit
	 * of the string-ref's pointer address. Databases that enable @c idLookup
	 * are responsible for this.
	 * @return Returns @a def.
	 */
	inline daeSmartRef<typename S::__COLLADA__T> &idLookup(const T &id,
	class undefined*_=0,const daeSmartRef<typename S::__COLLADA__T>&def=nullptr)const
	{
		return idLookup(id,const_cast<daeSmartRef<typename S::__COLLADA__T>&>(def));
	}

	template<class S>
	/**WARNING, LEGACY
	 * @warning THIS IS LEGACY FUNCTIONALITY. SEE THE BODY OF @c daeDocument 
	 * FOR HOW TO ENABLE IT, AND HOW IT CAN FAIL IF sid IS NOT A SMALL STRING.
	 *
	 * @note This is not very useful on its own. SIDs are not unique. 
	 * @see @c daeElement::sidLookup().
	 * @see @c daeSIDResolver.
	 * @remark Historically @a matchingElements is cleared.
	 * @param sid The COLLADA Scoped Identifier (SID) to match on.
	 * @param matchingElements The array of matching elements.
	 * @return Returns @a matchingElements.
	 */
	inline daeArray<daeElementRef> &sidLookup(const S &sid, daeArray<daeElementRef> &matchingElements, enum dae_clear clear=dae_clear)const
	{
		if(clear) matchingElements.clear(); 
		_sidLookup(daeBoundaryStringRef(*this,sid),matchingElements); return matchingElements;
	}
	
  /////////////////////////////////////////////////////////////////
  //typeLookup is a difficult part of the old library to support!//
  //It's orthogonal to the change-notice system, so that it won't//
  //become entangled with it. Users may want it disabled, because//
  //it complicates a lot of basic procedures. It seems like there//
  //could be a more general tracking facility in it, but it's not//
  //clear what it'd look like, or be, or mean to database writers//
  /////////////////////////////////////////////////////////////////

	template<typename T>
	/**LEGACY-SUPPORT
	 * This can be used to "prime the pump." 
	 * @see @c daePlatform::personalizeDocument() Doxygentation. 
	 */
	inline void typeLookup_enable()const{ daeArray<daeSmartRef<T>,1> dummy; typeLookup(dummy); }

	template<typename T> //T is DAEP::Element based, domAny will silently fail.
	/**WARNING, LEGACY-SUPPORT
	 * @warning T WON'T WORK WITH @c domAny. IT WON'T FIND ANYTHING. THERE AREN'T GOOD REASONS
	 * TO LOOK FOR @c domAny. ONLY THE MASTER META DATA WILL BE USED, SO IT'S NOT CATASTROPHIC.
	 *
	 * Looks up elements by C++ type.
	 * @remark Historically @a matchingElements is cleared.
	 * @param matchingElements The array of matching elements.
	 * @return Returns @a matchingElements.
	 */	
	inline daeArray<daeSmartRef<T>> &typeLookup(daeArray<daeSmartRef<T>> &matchingElements, enum dae_clear clear=dae_clear)const
	{
		if(clear) matchingElements.clear();
		//T won't pass daeGetMeta if not an element.
		_typeLookup(daeGetMeta<T>(),(daeArray<daeElementRef>&)matchingElements); return matchingElements;
	}
	template<class T> //daeElement or const daeElement (The template crap captures const-ness.)	
	/**LEGACY-SUPPORT */
	inline daeArray<daeSmartRef<typename daeConstOf<T,daeElement>::type>> &typeLookup
	(daeMeta &same, daeArray<daeSmartRef<T>> &matchingElements, enum dae_clear clear=dae_clear)const
	{
		if(clear) matchingElements.clear(); 
		_typeLookup(same,(daeArray<daeElementRef>&)matchingElements); return matchingElements;
	}	

	template<template<class> class V, typename T>
	/**LEGACY-SUPPORT
	 * Similar to @c daeElement::getDescendantsByType(), but with root included. Nothing special.
	 */	
	inline V<T> &getElementsByType(V<T> &matchingElements, enum dae_clear clear=dae_clear)
	{
		return getPseudoElement().getDescendantsByType(matchingElements,clear);
	}	
	template<template<class> class V, typename T>
	/**CONST-FORM
	 * @see overload Doxygentation.
	 */	
	inline V<T> &getElementsByType(V<T> &matchingElements, enum dae_clear clear=dae_clear)const
	{
		return getPseudoElement().getDescendantsByType(matchingElements,clear);
	}	
	
COLLADA_(private) //exported indexing methods

	/** Implements @c idLookup(). @a ref is a @c daeStringRef. */
	LINKAGE void _idLookup(daeString ref, daeElementRef&)const;	
	/** Implements @c typeLookup(). */
	LINKAGE void _typeLookup(daeMeta&, daeArray<daeElementRef>&)const;	
	/** Implements @c sidLookup(). @a ref is a @c daeStringRef. */
	LINKAGE void _sidLookup(daeString ref, daeArray<daeElementRef>&)const;

COLLADA_(public) //LEGACY-SUPPORT
	/**LEGACY
	 * This API is the mechanism by which the ID and SID maps are populated. 
	 * @c daeDatabase implementations are responsible for calling it, in response
	 * to @c daeDatabase::_v1_note(). Databases are not obliged to do so. Notification
	 * is required to go to the database. It's this way so that there aren't duplicate paths.
	 */
	LINKAGE void _carry_out_change_of_ID_or_SID(const DAEP::Change&, const daeData*)const;
	/**LEGACY
	 * Not only is it necessary to capture changes of the attributes, whenever an
	 * element having an "id" or "sid" attribute is inserted or removed from a document
	 * it's necessary to manage the document's id/sid lookup tables. It's up to the database
	 * to walk the element graph, since that's heavy lifting, and there might be other things it
	 * needs to do.
	 * @param destination can be @c nullptr. Both @c this document and it need to "compare notes." 
	 * @c this can also be @c nullptr. The API is not @c static "just because." E.g:
	 * @c source->_migrate_ID_or_SID(destination,...);
	 */
	LINKAGE void _migrate_ID_or_SID(const daeDocument *destination, const daeElement*, const daeData*)const;

#ifdef BUILDING_COLLADA_DOM

COLLADA_(private) //LEGACY: old "database" API stuff

		mutable daeStringRefMap<daeElement*> _idMap;
		typedef daeStringRefMap<daeElement*>::const_iterator _idMapIter;

		mutable daeStringRefMultiMap<daeElement*> _sidMap;	
		typedef daeStringRefMultiMap<daeElement*>::const_iterator _sidMapIter;
		typedef std::pair<_sidMapIter,_sidMapIter> _sidMapRange;

		friend daeMetaElement;
		mutable daeStringRefMap<std::vector<daeElement*>> _typeMap; 
		typedef std::vector<daeElement*> _typeVec;		
		typedef daeStringRefMap<std::vector<daeElement*>>::iterator _typeMapIter;
		friend daeContents_base;		
		template<int op> struct _typeLookup_migrate_operation
		{
			const daeDocument *doc;	void operator()(const daeElement&);
		};
		friend daeElement;		
		_typeVec *_typeLookup_vector(daeMeta&)const;
		void _typeLookup_self_remove(daeMeta&,daeElement&)const;
		void _typeLookup_bulk_remove(daeMeta&/*,UNIMPLEMENTED*/)const;
		
#endif //BUILDING_COLLADA_DOM
};

#include "../LINKAGE.HPP" //#undef LINKAGE
	
//---.
}//<-'

#endif //__COLLADA_DOM__DAE_DOCUMENT_H__
/*C1071*/
