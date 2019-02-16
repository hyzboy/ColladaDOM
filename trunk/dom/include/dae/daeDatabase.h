/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_DATABASE_H__
#define __COLLADA_DOM__DAE_DATABASE_H__

#include "daeTypes.h"

COLLADA_(namespace)
{//-.
//<-'

/**SCOPED-ENUM
 * These values map to @c daeElement::getMigratioByte().
 */
struct daeMigrationByte
{
	enum //SCOPED-ENUM
	{
	CONTENT_ALLOCATION_UNIT=1,
	HAS_STATIC_ID_ATTRIBUTE=2,
	HAS_NONSTATIC_ATTRIBUTE=4,
	XML_ATTRIBUTE_NIBBLE=0xf0,
	XMLNS=16,
	XML_BASE=32,
	XML_LANG=64,
	XML_ATTRIBUTE_UNCOUNTED=128,
	};
	
	inline operator unsigned int&()
	{
		return *(unsigned int*)this;
	}

	daeMigrationByte(unsigned int i=0)
	{
		*(unsigned int*)this = i; 
	}

  //__DAEP__Element__Data bits //__DAEP__Element__Data bits

	/**BITFIELD, FIRST-MEMBER
	 * @see @c daeMigrationByte::CONTENT_ALLOCATION_UNIT
	 *
	 * This is for migration handling. Traversing a graph can
	 * be painful. Typically elements are inserted without any
	 * content in tow. Testing this flag can avoid digging deep
	 * into the metadata, e.g. calling daeElement::getContents().		 
	 *
	 * @note THIS IS ONLY A HINT, AND DOESN'T CONTEMPLATE EMBEDDED
	 * CONTENTS-ARRAYS. (As of now, embedding is not implemented.)
	 */
	unsigned int daeContents_capacity_is_nonzero:1;		
	/**BITFIELD-CONTINUED
	 * @see @c daeMigrationByte::HAS_STATIC_ID_ATTRIBUTE
	 *
	 * This means the element metadata contains an "id" like attribute.
	 * These are registered with @c XS::Schema::getIDs().
	 */
	unsigned int getMeta_getFirstID_is_nonzero:1;		
	/**BITFIELD-CONTINUED
	 * @see @c daeMigrationByte::HAS_NONSTATIC_ATTRIBUTE
	 *
	 * @c daeAnyAttribute nonstatic attributes copy-on-write allocated.
	 */
	unsigned int daeAnyAttribute_copy_on_write:1;		
	/**BITFIELD-CONTINUED
	 */
	unsigned int _reserved_migration_bit_:1;
	/**BITFIELD-CONTINUED
	 * @see @c daeMigrationByte::XML_ATTRIBUTE_NIBBLE
	 *
	 * 1: Set upon creation of xmlns qualified attributes; Never unset.
	 * 2: xml:base
	 * 4: xml:lang
	 * 8: other (xml:space)
	 */
	unsigned int xml_qualified_attribute_masks:4;  

  //daeElement methods //daeElement methods //daeElement methods

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
	inline bool definitely_is_childless()const
	{
		return 0==daeContents_capacity_is_nonzero;
	}
	/**LOW-LEVEL
	 * @see @c daeMigrationByte::HAS_STATIC_ID_ATTRIBUTE
	 * @return Returns @c true if the schema describes attributes that
	 * are marked by @c XS::Schema::getIDs().
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 */
	inline bool has_static_ID_attribute()const
	{
		return 1==getMeta_getFirstID_is_nonzero;
	}
	/**LOW-LEVEL
	 * @see @c daeMigrationByte::HAS_NONSTATIC_ATTRIBUTE
	 * @return Returns @c true if @c getAttributes().grow() is entered.
	 * This is an optimization mainly for DAEP::ELEMENT change-notices.
	 */
	inline bool has_nonstatic_attribute()const
	{
		return 1==daeAnyAttribute_copy_on_write;
	}

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
		return 0!=(1&xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_BASE
	 * @return Returns @c true if the xml:base attribute is present.
	 */
	inline bool hasAttribute_xml_base()const
	{
		return 0!=(2&xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_LANG
	 * @return Returns @c true if the xml:lang attribute is present.
	 */
	inline bool hasAttribute_xml_lang()const
	{
		return 0!=(4&xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_ATTRIBUTE_UNCOUNTED
	 * @return Returns @c true if there is an xml qualified attribute 
	 * present that is neither xml:base nor xml:lang. xml:space maybe.
	 */
	inline bool hasAttribute_xml_space_et_cetera()const
	{
		return 0!=(8&xml_qualified_attribute_masks);
	}
	/**
	 * @see @c daeMigrationByte::XML_ATTRIBUTE_NIBBLE
	 * @return Returns the internal 4-bitfield corresponding to flags
	 * for xml qualified attributes.
	 */
	inline unsigned int getMigrationNibble_xml_attribute_masks()const
	{
		return xml_qualified_attribute_masks;
	}
};

template<class Change>
/**
 * @c daeDatabase @c friend.
 * Sends a note-of-change to the database.
 * @see @c DAEP::Change.
 * OLD "daeValue::noteChangeWRT" DOXYGENTATION
 * This does very little but unpack the architecture.
 * This version doesn't check if this value/attribute is subject
 * to notifications. This form is mainly if the caller
 * "just knows" that notifications are enabled.
 */
inline Change &daeNoteChange(Change &note, daeData *attrib=nullptr)
{
	dae(note.element_of_change)->getDatabase()._v1_note(note,attrib); return note;
}

namespace DAEP{ class Change; }

/**ABSTRACT, NEVER-CONST
 * The @c daeDatabase class defines the COLLADA runtime database interface.
 * Pre-2.5 the database had access to the DOM, and controlled the documents.
 * Conceptually the database shouldn't be positioned to alter data/structure.
 *
 * @remarks @c const should not be used with @c daeDatabase, as it is closed.
 */
class daeDatabase_base
{	
COLLADA_(private) //VIRTUAL METHOD TABLE
	/**
	 * Virtual Destructor
	 * Put at top, in case it affects the vtable layout.
	 */
	virtual ~daeDatabase_base(){}

	friend class daeDOM; 
	friend class daeObject;
	friend class daeElement;
	friend class daeDatabase;
	friend class DAEP::Object;
	friend class daeAnyAttribute;
	template<class> friend class daeDB;	
	template<class T> friend struct daeNullable;
	 									   
	inline void **_userptrptr(const daeObject &obj)
	{
		/*if(_vN_>=0)*/ return _v1_userptrptr(obj); 
	}

	inline void _ciao(daeDOM &dom, void* &rejoinder)
	{
		/*if(_vN_>=0)*/ return _v1_ciao(dom,rejoinder); 
	}

	friend class daeStringRef;
	inline daeString _ref(daeString string, size_t extent, void *&rejoinder)
	{
		/*if(_vN_>=0)*/ return _v1_ref(string,extent,rejoinder); 
	}	
	inline void _prototype(/*const*/daeStringRef &prototyped, void *&rejoinder)
	{
		/*if(_vN_>=0)*/ return _v1_prototype(prototyped,rejoinder); 
	}

	friend class daeMetaElement;
	template<class T> inline void _new(size_t chars, T* &obj, void* &rejoinder)
	{ 
		/*if(_vN_>=0)*/ return _v1_new(chars,obj,rejoinder); 
	}
	template<class T> inline void _delete(const T &obj)
	{ 
		/*if(_vN_>=0)*/ return _v1_delete(obj); 
	}
	
	template<class T>
	inline daeAlloc<T> &_new(size_t &newT, const daeAlloc<T> &AU, const daeObject &obj)
	{
		/*if(_vN_>=0)*/ return (daeAlloc<T>&)_v1_new(newT,AU,obj);
	}
	inline void _delete(const daeAlloc<> &AU, const daeObject &obj)
	{
		/*if(_vN_>=0)*/ return _v1_delete(AU,obj);
	}	
	
	template<class Change> //DAEP::Change
	friend Change &daeNoteChange(Change&,daeData*);

	//ADD to _vN_ if introducing new interfaces//
	//AND check against _vN_ before calling it.//
	virtual void **_v1_userptrptr(const daeObject&) = 0;
	virtual void _v1_ciao(daeDOM&,void*&) = 0;
	virtual daeString _v1_ref(daeString,size_t,void*&) = 0;	
	virtual void _v1_prototype(const daeStringRef&,void*&) = 0;	
	virtual void _v1_new(size_t, daeObject*&,void*&) = 0;	
	virtual void _v1_delete(const daeObject&) = 0;
	virtual void _v1_new(size_t, daeElement*&,void*&) = 0;	
	virtual void _v1_delete(const daeElement&) = 0;
	virtual void _v1_new(size_t, daeDocument*&,void*&) = 0; 
	virtual void _v1_delete(const daeDocument&) = 0; 
	virtual	daeAlloc<> &_v1_new(size_t &newT, const daeAlloc<>&, const daeObject&) = 0;
	virtual void _v1_delete(const daeAlloc<>&, const daeObject&) = 0;	
	virtual void _v1_note(const DAEP::Change&, daeData*) = 0;	
	virtual bool _v1_atomize_on_note(daeContainedObject&) = 0;	
	/**
	 * The current interface version is _v1_.
	 * New interfaces must be added after the past ones.
	 * New interfaces after 2.5 will begin with _v1_ and must be checked
	 * when called by their inline counterparts.
	 */
	const int _vN_;
	/**
	 * Default Constructor
	 */
	daeDatabase_base():_vN_(0){}

COLLADA_(public) //OPERATORS
	
	inline operator daeDatabase*(){ return (daeDatabase*)this; }
	inline operator daeDatabase&(){ return *(daeDatabase*)this; }		
	inline daeDatabase *operator->(){ return (daeDatabase*)this; }
	
	template<class T>
	/**WORKAROUND
	 * "error: invalid use of incomplete type."
	 * @see @c daeDB.
	 */
	static void _destruct(T &inclomplete_type){ inclomplete_type.~T(); }
	
	template<class T>
	/**WORKAROUND
	 * "error: invalid use of incomplete type."
	 * @see @c daeDB.
	 */
	static daeElement &_element_of_change(T &note)
	{
		return dae(*note.element_of_change);
	}
};

/**NEVER-CONST
 * @c daeDatabase_base has been separated out to give the user/client
 * control over their database class's look-and-feel.
 */
class daeDatabase : public daeDatabase_base
{
COLLADA_(public)
	/**wARNING, LEGACY-SUPPORT
	 * This lets resolver callers dirty the database's cache.
	 * If the resolver's cache-object isn't already contained by
	 * the database, it will be added if the database supports this.
	 *
	 * @note This isn't necessarily limited to resolvers, but it's
	 * not easy to come up with a generic wording for this funciton.
	 * @warning Post-2.5 the caches are cleared by @c daeNoteChange().
	 * IF NOTICES-OF-CHANGE ARE SUPPRESSED, the caches can produce
	 * unexpected results. Users/clients just have to figure this out.
	 */
	inline bool cacheResolverResult(daeContainedObject &cache_object)
	{
		return _v1_atomize_on_note(cache_object);
	}
};
  
#ifdef NDEBUG
#error Should there be a field that describes character classes?
#error For example, checking for whitespace could beat scanning.
#error One field can mark the presence of <![CDATA[]]> sections.
#endif
/**NEVER-CONST
 * A @c daeStringRef is really a pointer into a @c daeDBaseString.
 */
class daeDBaseString
{
COLLADA_(public)	
	/**
	 * Database plugins must acquire one or more string-allocator
	 * slots. A slot can be shared by databases that share a pool.
	 */
	unsigned short pool;
	/**
	 * If @c _allocator is equal, @c _refs is increased/decreased.
	 * If @c _refs%N==0 the the exported allocator API comes into
	 * play. N is an arbitrarily large number.
	 */
	unsigned short refs;
	/**
	 * This is the @c daeStringCP length, including @c fragmentCP,
	 * -excluding the 0 terminator.
	 */
	unsigned int fragmentN;	
	union
	{
	/**
	 * Because strings are very often "id" attributes, and these
	 * are very often paired, the @c daeStringRef class is setup
	 * to recognize strings that begin on a +1 address as a pair.
	 *
	 * As an optimization, at a later date, pairing logic may be
	 * limited to "id" strings. Allocators can choose to pair in
	 * this fashion, or choose to only pair for a valid "id." In
	 * other words @c fragmentCP is expected to be '#' for pairs.
	 */
	const daeStringCP fragmentCP;	
	/**VARIABLE-LENGTH
	 * This is to visualize the corresponding @c daeStringRef in
	 * a debugger.
	 */
	daeStringCP fragment[32];
	};

COLLADA_(public)

	inline daeUShort ref(){ return ++refs; }

	inline daeUShort release(){ return --refs; }
		   	
	/**
	 * Gets the next address in memory suitable for a @c daeDBaseString.
	 */
	void *next_pointer_boundary()
	{
		//size_t mask = sizeof(void*)-1;
		//return fragment+((fragmentN+1+mask)&~mask);
		return (char*)fragment+daeBoundaryOffset(fragmentN+1);
	}

	/**
	 * Constructor
	 */
	daeDBaseString(size_t _pool, int refs, daeString fragmentless, size_t len)
	:pool((unsigned short)_pool)
	,refs((unsigned short)refs),fragmentN((unsigned int)len+1),fragmentCP('#')
	{
		assert(0==daeOffset(fragment)%2); assert(pool==_pool);
		memcpy(fragment+1,fragmentless,len*sizeof(daeStringCP));
		fragment[fragmentN] = '\0';
	}
	/**
	 * Append constructor
	 */
	daeDBaseString(const daeDBaseString &cp, daeString fragmentless, size_t len)
	:pool(cp.pool),refs(1),fragmentN(cp.fragmentN),fragmentCP('#')
	{
		assert(0==daeOffset(fragment)%2);
		memcpy(fragment,cp.fragment,fragmentN*sizeof(daeStringCP));		
		memcpy(fragment+fragmentN,fragmentless,len*sizeof(daeStringCP));
		fragmentN+=(unsigned int)len;
		fragment[fragmentN] = '\0';		
	}
								 
	/**
	 * Gets the @c daeStringRef::_string pointer, including #.
	 * Do +1 to get behind @c _fragment.
	 */
	inline operator daeStringCP*(){ return fragment; }
};

/**
 * Helper for implementing @c daeDB, below.
 */
template<
class S=daeDocument, 
class T=daeElement, 
class U=daeAlloc<>, 
class V=daeObject,
class W=daeDOM> struct daeDBaseTraits
{
	typedef S Document; typedef T Element;
	typedef U Alloc;
	typedef V Object; typedef W Rejoinder; 	 	
};

/**C-PREPROCESSOR MACRO
 * This is in the same school as @c daeOffsetOf and @c daeSizeOf.
 * It sees that a user/client data-structure meets the requirements
 * of @c daeDB. It can fail if the compiler adds to structures, or if
 * a user does so on accident. (DEBUG BUILDS ONLY)
 * @see daeLegacyDBase.
 */
#define daeDBaseOK(UserObject,object) \
assert(daeOffsetOf(UserObject,object)==sizeof(UserObject)-daeSizeOf(UserObject,object));
/**NEVER-CONST
 * @c daeDB must be used to instantiate a @c daeDatabase, as its members are all private.
 * @tparam DBase is a class with a member: @c typedef @c daeDBaseTraits<> @c _DBaseTraits.
 * From there, you must implement each of the @a DBase:: methods, until the class compiles.
 */
template<class DBase> class daeDB : public daeDatabase_base, public DBase
{	
COLLADA_(private)

	typedef typename DBase::_DBaseTraits::Alloc _Alloc;
	typedef typename DBase::_DBaseTraits::Object _Object;
	typedef typename DBase::_DBaseTraits::Element _Element;
	typedef typename DBase::_DBaseTraits::Document _Document;
	typedef typename DBase::_DBaseTraits::Rejoinder _Rejoinder;

	//Please don't replace _ with cast() functions.
	//#define _(x,y) (_##x&)static_cast<const _##x&>(y)
	/**
	 * Unfortunately C++ doesn't define the layout of base
	 * classes, and so inheritance cannot be used to do this.
	 * Just doing sizeof is dicey, but needs no infrastructure.
	 * @see @c daeDBaseOK.
	 */
	#define _(x,y) (*(_##x*)((char*)&y-sizeof(_##x)+sizeof(COLLADA_INCOMPLETE(DBase) dae##x))) //sizeof(y)
	/**TODO?
	 * @todo Inheritance had to be abandoned. This does no checks
	 * to guarantee that the object is the last member in the data
	 * structure. It could conveivably fail on a compiler that adds
	 * to the end of the data structure; or users mistakingly do so.
	 * @see @c daeDBaseOK.
	 */
	template<class S, class T> inline size_t _v1_new_prolog(T *obj)
	{
		#ifdef _DEBUG
		obj = nullptr;
		#endif
		return sizeof(S)-sizeof(T); daeCTC<(sizeof(S)>=sizeof(T))>();
	}
	
	virtual ~daeDB(){ /*NOP*/ }
	
	virtual void **_v1_userptrptr(const daeObject &obj)
	{
		//Here the DB can supply a user-data pointer.
		return DBase::_userptrptr(_(Object,obj));
	}
	virtual void _v1_ciao(daeDOM &DOM, void* &rejoinder)
	{
		//Here the DB says hello AND goodbye (ciao!)
		//rejoinder will be equal to nullptr at first.
		//&rejoinder is in DOM. ciao can save the delta.
		//Note. DOM has this DB for life. The DB may have
		//multiple DOMs at a time; their lives overlapping.
		DBase::_ciao(DOM,(_Rejoinder*&)rejoinder); assert(rejoinder!=nullptr);
	}
	virtual daeString _v1_ref(daeString string, size_t extent, void* &rejoinder)
	{
		//Here the DB must return a daeDBaseString that
		//matches string, and is 0-terminated at string[extent].
		//IMPORTANTLY, DBase must also add a ref to the string-ref's 
		//ref-counter. Unfortunately it's impossible to do that for DBase.
		return DBase::_ref(string,extent,(_Rejoinder*&)rejoinder); 
	}
	virtual void _v1_prototype(const daeStringRef &prototyped, void* &rejoinder)
	{
		//This is like _ref() except DBase is encouraged
		//to build an index of prototype string-pointers that
		//maps to a matching pooled string-ref. This is because a
		//prototype string is forever, and likely a "default" or "fixed"
		//attribute from the XML Schema, and therefor there is likely overlap.
		(daeString&)prototyped = DBase::_prototype(prototyped,(_Rejoinder*&)rejoinder);
	}
	virtual void _v1_new(size_t chars, daeObject* &obj, void* &rejoinder)
	{
		//Here the DB allocates a chars-size memory block.
		//It can add/construct memory on the front and back.
		size_t diff = _v1_new_prolog<_Object>(obj);		
		DBase::_constructing(chars+diff,(_Object*&)obj,(_Rejoinder*&)rejoinder); 
		(char*&)obj+=diff; assert(obj!=nullptr);
	}
	virtual void _v1_delete(const daeObject &obj)
	{
		//Here the DB destructs its parts of its memory block.
		DBase::_destructing(_(Object,obj)); _destruct(obj); //obj.~daeObject();
		//Here the DB deallocates its memory block from before.
		DBase::_destructed(_(Object,obj)); 
	}
	virtual void _v1_new(size_t chars, daeElement* &obj, void* &rejoinder)
	{
		//This can be used to glean something about the
		//element, in case there's something about element 
		//that needs to be filled out. The other _contructing
		//interfaces do not have prototypes. (they're just junk.)
		//Do _constructing(x,y,z,...) if a template form is desired.
		const daeElement &prototype = *obj;

		//See the instructions of the daeObject form.
		size_t diff =_v1_new_prolog<_Element>(obj);		
		DBase::_constructing(chars+diff,(_Element*&)obj,(_Rejoinder*&)rejoinder,prototype); 		
		(char*&)obj+=diff; assert(obj!=nullptr);
	}
	virtual void _v1_delete(const daeElement &elem)
	{
		//See the instructions of the daeObject form.
		DBase::_destructing(_(Element,elem)); _destruct(elem); //elem.~daeElement();
		DBase::_destructed(_(Element,elem)); 
	}
	virtual void _v1_new(size_t chars, daeDocument* &obj, void* &rejoinder)
	{
		//See the instructions of the daeObject form.
		size_t diff = _v1_new_prolog<_Document>(obj);		
		DBase::_constructing(chars+diff,(_Document*&)obj,(_Rejoinder*&)rejoinder); 
		(char*&)obj+=diff; assert(obj!=nullptr);
	}
	virtual void _v1_delete(const daeDocument &doc)
	{
		//See the instructions of the daeObject form.
		DBase::_destructing(_(Document,doc)); _destruct(doc); //doc.~daeDocument();
		DBase::_destructed(_(Document,doc));
	}	
	virtual	daeAlloc<> &_v1_new(size_t &newT, const daeAlloc<> &AU, const daeObject &obj)
	{
		//Here the DB uses AU to allocate/return 
		//a newT-or-larger-sized AU of the same type.
		//Do something like AU.getRaw(newT)-daeOpaque(&AU).
		return DBase::_reallocating(newT,_(Alloc,AU),obj);
	}
	virtual void _v1_delete(const daeAlloc<> &AU, const daeObject &obj)
	{
		//Here the DB deallocates its memory block from before.
		return DBase::_deallocating(_(Alloc,AU),_(Object,obj));
	}
	virtual void _v1_note(const DAEP::Change &note, daeData *attrib)
	{
		const COLLADA_INCOMPLETE(DBase) DAEP::Change &i = note;
		//Don't forget to call note.carry_out_change().		
		//Assume attrib==nullptr if note.kind_of_change!=DAEP::ATTRIBUTE.
		//(It may actually be a @c daeValue pointer, to avoid branching.)
		//note must be forwarded to elem.getDocument() to use the legacy indexing APIs.
		return DBase::_noting(_(Element,*i.element_of_change),note,attrib);
	}
	virtual bool _v1_atomize_on_note(daeContainedObject &cache_object)
	{
		//To not manage a cache, @c return false.
		//This is to support the legacy resolver cache framework.
		//If @c _noting() is called since @c _atomize_on_noting() was last
		//called, then these objects should have their @c __COLLADA__atomize() called.
		return DBase::_atomize_on_noting(cache_object);
	}
	#undef _
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_DATABASE_H__
/*C1071*/
