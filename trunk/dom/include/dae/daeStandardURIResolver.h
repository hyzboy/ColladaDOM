/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_STANDARD_URI_RESOLVER_H__
#define __COLLADA_DOM__DAE_STANDARD_URI_RESOLVER_H__

#include "../dae.h"
#include "daeIOPlugin.h"

COLLADA_(namespace)
{//-.
//<-'

/**LEGACY
 * The @c daeDefaultURIResolver class derives from @c daeRefResolver and implements
 * the default XML backend resolver.
 */
class daeDefaultURIResolver : public daeRefResolver
{			
COLLADA_(protected) //daeContainedObject methods

	/**PURE-OVERRIDE*/
	virtual void __daeContainedObject__v1__atomize(){ /*NOP*/}

COLLADA_(public) //public daeRefResolver methods
	
	/** Used by @c daeRefResolver::is(). */
	static daeClientString getRefResolverNameString()
	{
		return "Default URI Resolver (daeDefaultURIResolver)"; 
	}
	/**PURE-OVERRIDE */
	virtual daeClientString getName()const{ return getRefResolverNameString(); }

COLLADA_(public)
	/**
	 * Default Constructor
	 */
	daeDefaultURIResolver():daeRefResolver(daeRefType::URI){} 

	/** 
	 * This is making use of the resolver, independently of 
	 * @c RefResolverList. (It uses @c _resolve() instead.)
	 * The refresh is mandatory. If you think it's too much
	 * then study @c _resolve() to write your own resolver.
	 */
	inline daeOK resolve(const daeURI &URI, daeRefRequest &req)const
	{
		/*URI.refresh();*/ return _resolve(URI,req);
	}

	/**LEGACY */
	inline daeElementRef resolveElement(const daeURI &uri)const
	{
		daeRefRequest req;
		if(DAE_OK==_resolve(uri,req)&&req.object->_isElement())
		return (daeElementRef&)req.object;
		return nullptr;
	}

COLLADA_(protected) //daeRefResolver::_resolve
	/**PURE-OVERRIDE
	 * Implements @c daeRefResolver::_resolve() for @c daeRefResolveList.
	 */
	virtual daeError _resolve(const daeRef &ref, daeRefRequest &req)const
	{
		const daeURI &URI = static_cast<const daeURI&>(ref);

		//If there are additional arguments, assume they
		//are intended for another resolver.
		if(!req.empty()) return DAE_ERR_NOT_IMPLEMENTED;
				
		const_daeDOMRef dom; daeError err = DAE_OK; 
		if(req.object!=nullptr) dom = req.object->getDOM();
		if(dom==nullptr) dom = ref.getDOM();
		if(dom!=nullptr) 
		{
			const_daeDocRef doc = URI.getDoc();			
			if(doc!=nullptr)
			{
				if(URI.isFragmentURI()) goto fragment;
				if(!URI.getIsResolved()) URI.resolve();
				if(URI.referencesDoc2(doc)) goto fragment;
				
				doc = nullptr; goto document;
			}
			else document:
			{			
				if(URI.docLookup2(*dom,doc)==nullptr)
				{	
					doc = const_cast<daeDOM&>(*dom).openDoc<void>(URI);
					if(doc==nullptr)
					err = DAE_ERR_QUERY_NO_MATCH;
				}
			}
			if(doc!=nullptr) fragment:
			{
				daeName id = URI.getURI_fragment();
				if(!id.empty())
				{
					daeObjectRef obj;
					if(nullptr==doc->getDocument()->idLookup(id,obj))
					err = DAE_ERR_QUERY_NO_MATCH;
					else req.object = obj;
				}
				else req.object = &const_cast<daeDoc&>(*doc);
				if(err==DAE_OK)
				req.typeInstance = nullptr;
			}
		}
		else err = DAE_ERR_INVALID_CALL; 
		
		if(err==DAE_OK) URI.resolve();
		if(err!=DAE_OK) _printError(err,URI.getURI()); return err;
	}
	static void _printError(daeError err, const daeName &uri)	
	{
		daeEH::Warning<<"daeDefaultURIResolver - Failed to resolve:\n"<<uri;
	}
};

/**LEGACY, NOT RECOMMENDED
 * @see daeRawResolver::cache.
 * "A simple class to make speed up the process of resolving a .raw URI.
 * The result of the resolve is cached for future use.
 * This is meant for DOM internal use only."
 */
class daeRawRefCache : public daeContainedObject
{
COLLADA_(protected) //daeContainedObject method

	/**PURE-OVERIDE
	 * Initiates "teardown" sequence. 
	 */
	virtual void __daeContainedObject__v1__atomize(){ clear(); }

COLLADA_(public)

	inline const daeElementRef lookup(const daeURI &uri) 
	{
		Table::iterator it = _lookupTable.find(uri.data());
		if(it==_lookupTable.end()) return nullptr; //miss				
		return (daeElementRef&)it->second; //hit		
	}

	inline void add(const daeURI &URI, const daeObjectRef &elt)
	{		
		daeDatabase *db = elt->_getDBase();
		if(db!=nullptr&&db->cacheResolverResult(*this))
		_lookupTable[URI.data()] = elt; 
	}

	inline void clear(){ _lookupTable.clear(); }

COLLADA_(private) //DATA-MEMBER

	struct Less // : std::less<std::string>
	{
		typedef int is_transparent; //C++14: apparently this is not done by default [WG21 N3657]

		//why is this not inherited from std::less???
		inline bool operator()(const std::string &a, const std::string &b)const{ return a<b; }
		//just to be safe, define in both directions.
		inline bool operator()(const std::string &a, daeString b)const{ return strcmp(a.c_str(),b)<0; }
		inline bool operator()(daeString a, const std::string &b)const{ return strcmp(a,b.c_str())<0; }
	};

	typedef std::map<std::string,daeObjectRef,Less> Table;

	Table _lookupTable;
};

/**LEGACY, NOT RECOMMENDED
 * This resolver edits documents on the fly, like an animated webpage!
 * It's not recommended, as this is counterintuitive, and far outside
 * the role that is played by the DOM. 
 *
 * (It turns <source> URLs to RAW resources into <x_array> elements.)
 *
 * The counterpart to this resolver is @c daeLibXMLPlugin::write; as 
 * it's required, to reverse the changes made by @c daeRawResolver on
 * write. @c option_to_write_COLLADA_array_values_to_RAW_file_resource 
 * is required to be enabled to do so.
 *
 * This is an old part of the library, which would be better performed
 * by a RAW plugin proxy, if at all (on both read in, and write out.)
 * It's hard to make the call to roll it into the daeLibXMLPlugin. It's
 * hard to know if this functionality is relied upon. It's a speed 
 * optimization, on because it makes parsing the text unnecessary. That
 * said, if COLLADA has to be high performance--you're doing it wrong!
 */
class daeRawResolver : public daeRefResolver
{
COLLADA_(protected) //daeContainedObject method

	/**PURE-OVERRIDE */
	virtual void __daeContainedObject__v1__atomize(){ _cache.clear(); }

COLLADA_(public) //public daeRefResolver methods

	/** Used by @c daeRefResolver::is(). */
	static daeClientString getRefResolverNameString()
	{
		return "Raw Resolver (daeRawResolver)"; 
	}
	/**PURE-OVERRIDE */
	virtual daeClientString getName()const{ return getRefResolverNameString(); }

COLLADA_(public) 	
	/**
	 * Default Constructor
	 */
	daeRawResolver():daeRefResolver(daeRefType::URI)
	{
		_cache.__DAEP__Object__embed_subobject(this);
	} 

	/** 
	 * This is making use of the resolver, independently of 
	 * @c RefResolverList. (It uses @c _resolve() instead.)
	 */
	inline daeOK resolve(const daeURI &URI, daeRefRequest &req)const
	{
		URI.refresh(); return _resolve(URI,req);
	}

	/**LEGACY */
	inline daeElementRef resolveElement(const daeURI &uri)const
	{
		daeRefRequest req;
		if(DAE_OK==_resolve(uri,req)) return (daeElementRef&)req.object;
		return nullptr;
	}

COLLADA_(protected) //daeRefResolver::_resolve
	/**PURE-OVERRIDE
	 * Implements @c daeRefResolver::_resolve() for @c daeRefResolveList.
	 */
	virtual daeError _resolve(const daeRef &ref, daeRefRequest &req)const
	{
		const daeURI &uri = static_cast<const daeURI&>(ref);
		daeElementRef hit = _cache.lookup(uri);
		daeError err = _resolve_exported(hit,uri,req);
		if(err==DAE_OK&&hit!=req.object) _cache.add(uri,req.object); return err;
	}
	COLLADA_DOM_LINKAGE
	/**
	 * The exported implementation of _resolve(). 
	 * @note It's important that the cache is not visible to @c _resolve_exported(),
	 * -because the STL makes no ABI guarantees.
	 * @see daeURI.cpp TU's implementation.
	 */
	daeOK _resolve_exported(const daeElementRef &hit, const daeURI &uri, daeRefRequest &req)const;

COLLADA_(private) //DATA-MEMBER
	//SUB-OBJECT
	/** 
	 * Transplant from daeDOM/previously DAE. 
	 */
	mutable daeRawRefCache _cache; 
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_STANDARD_URI_RESOLVER_H__
/*C1071*/