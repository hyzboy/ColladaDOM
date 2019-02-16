/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_ATLAS_H__
#define __COLLADA_DOM__DAE_ATLAS_H__

#include "daeDocument.h" 
#include "daeIOPlugin.h"
  
COLLADA_(namespace)
{//-.
//<-'

/**ZAE
 * @c daeImage is a virtual file; such as an uncompressed
 * part of a ZIP archive that is modified in memory until
 * the entire archive is ready to be written out together.
 *
 * @note Users are largely responsible for creating these
 * objects. It can be implemented with @c std::deque or a+
 * @c FILE or @c std::vector or combination of other ways.
 * +DON'T use deque since _DEQUESIZ on MSVC is a disgrace.
 */
class daeImage : public daeContainedObject
{
COLLADA_(protected)
	/**
	 * @c _name is set for life. 
	 * @c const is not used in case implementations would
	 * like to recycle them.
	 */
	daeClientString _name;

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeImage)

COLLADA_(public)
	/**
	 * Constructor
	 */
	daeImage(daeClientString name):_name(name)
	{}
	/**
	 * Alternative Constructor with parentage. 
	 */
	daeImage(daeClientString name, const DAEP::Object *c)
	:daeContainedObject(c),_name(name)
	{}

	/**WARNING
	 * This is a raw string identifiying this image that 
	 * doubles as a numeric (pointer) association with a
	 * real image.
	 *
	 * @warning This string is normally owned by atlases.
	 * It's garbage if the image lingers after the atlas.
	 */
	inline daeClientString getName()const{ return _name; }	
		
	/**
	 * "Deletes" this image. To clarify: When an archive
	 * is opened it's not possible to remove its entries.
	 * Because they refer to the original. By setting it
	 * to be "deleted" when the archive is rewritten the
	 * corresponding image will no longer be part of the
	 * original or derivative archive.
	 * @see @c getIsDeleted().
	 *
	 * @note @c clear() does not delete an entry from an
	 * an archive, just as there can be 0 sized files on
	 * a file-system. It must be marked deleted. Also to
	 * "delete" does not make the image 0 sized. Nor can
	 * it delete underlying files that might be writable.
	 */
	inline void setIsDeleted(bool y=true)
	{
		if(y!=getIsDeleted()) _getObjectTags().classTag^=1;
	}
	/**
	 * Tells if @c this image's data is "Deleted."
	 * @see @c setIsDeleted() doxygentation.
	 */
	inline bool getIsDeleted()const{ return (getObjectTags()&1)!=0; }

	/**
	 * Tells the atlas that @c this image should be kept
	 * even if its source is changed. A compressed image
	 * that is stored decompressed, etc. is NOT modified.
	 * @see @c getIsModified().
	 */
	inline void setIsModified(bool y=true)
	{
		if(y!=getIsModified()) _getObjectTags().classTag^=2;
	}
	/**
	 * Tells if @c this image should be kept even if its
	 * source has changed.
	 * Should indicate @c daeAtlasValue::TIME is changed.
	 */
	inline bool getIsModified()const{ return (getObjectTags()&2)!=0; }

	/**
	 * Tells if @c this image's data has been compressed.
	 * It's likely uncompressed, modified & recompressed.
	 * "Compression" should match @c daeAtlasValue::CODE.
	 */
	inline bool getIsCompressed()const{ return (getObjectTags()&4)!=0; }

	/**
	 * Tells if @c this image's data is a memory pointer.
	 */
	inline bool getIsContiguous()const{ return (getObjectTags()&8)!=0; }

COLLADA_(public) //daeImage interfaces
	/**MUTABLE
	 * This interface is responsible for most operations.
	 *
	 * @a in is either a buffer to draw from or to fill.
	 * If @c in==nullptr insertion mode fills with junk.
	 *
	 * Insertion is done if @a range1 is provided.
	 * Erasing is done if only @a range2 is provided.	 
	 * Erasing+Insertion is done if @a range1 & @a range2.
	 * Reading is done if @a in and @a range2 is provided.
	 *
	 * @a range1 and @a range2 should be set to the affective
	 * range of @c char data. Or should be emptied on error.
	 * 
	 * @return Returns the modified size of @c this image.
	 */
	virtual size_t data(const void *in, daeIO::Range *range1, daeIO::Range *range2=nullptr)const = 0;
	
	/**MUTABLE
	 * @return Returns @c nullptr if a pointer cannot be facilitated
	 * or locking is not-implemented.
	 *
	 * @a mode is 0 to unlock; 'r' to get a shared read lock; and 'w' 
	 * to get an unshared write lock. Partial locks are not possible.
	 * @note Such locks simply ward off changes to the memory and it
	 * is implied that a 'w' lock changes the memory and 'r' doesn't.
	 *
	 * @a mode 0 may still return a pointer to memory that is useful.
	 * But it should only be used with private images. Writing to it
	 * is undefined-behavior.
	 */
	virtual char *lock(int mode)const{ return nullptr; }
	/**MUTABLE
	 * Called to unlock after using @c lock(). 
	 *
	 * @return Returns @c lock(0) which can be a pointer without any
	 * guarantees.
	 */
	inline char *unlock()const{ return lock(0); }	

COLLADA_(public) //daeContainedObject methods
	/**PURE-OVERRIDE
	 * Implements nonstandard @c atomize() & @c __COLLADA__atomize()
	 * logic. 
	 * @note BY DEFAULT nothing is done. By "nonstandard" it's meant
	 * that normally "atomizing" tears down a @c daeObject tree, but
	 * it is assumed that images do not require such trees and so it
	 * is desirable to repurpose the method similar to cache-objects.
	 */
	virtual void __daeContainedObject__v1__atomize(){}
	/**HINT
	 * If a @c daeImage is a proxy to a file, for example, this is a
	 * hint to close that file. @c daeImage is a low-level mechanism.
	 * So, atlas code should strive to always call upon @c atomize().
	 */
	inline void atomize()const
	{
		const_cast<daeImage*>(this)->__daeContainedObject__v1__atomize();
	}

COLLADA_(public) //HELPERS

  //TODO? These can use C++ template parameters if there's demand.

	/**	
	 * Reads a range of @c char sized memory.
	 */
	inline size_t copy(size_t begin, size_t end, void *cp)const
	{
		daeIO::Range r = {begin,end}; 
		return data(cp,nullptr,&r);
	}
	/**	
	 * Erase a range of @c char sized memory.
	 */
	inline size_t erase(size_t begin, size_t end)
	{
		daeIO::Range r = {begin,end}; 
		return data(nullptr,nullptr,&r);
	}
	/**	
	 * Inserts a range of @c char sized memory.
	 */
	inline size_t insert(size_t begin, size_t end, const void *cp)
	{
		daeIO::Range r = {begin,end}; 
		return data(cp,&r,nullptr);
	}
	/**	
	 * Writes a range of @c char sized memory.
	 */
	inline size_t replace(size_t begin, size_t end, const void *cp)
	{
		daeIO::Range r = {begin,end}; 
		return data(cp,&r,&r);
	}
	/**	
	 * Replaces a range of @c char sized memory.
	 */
	inline size_t replace(size_t begin, size_t end, const void *cp, size_t len)
	{
		daeIO::Range r = {begin,end}; 
		daeIO::Range r2 = {begin,begin+len}; 
		return data(cp,&r2,&r); 
	}
	/**	
	 * Replaces a range of @c char sized memory.
	 */
	inline size_t assign(const void *cp, size_t len)
	{
		daeIO::Range r = {0,size_t(-1)};
		daeIO::Range r2 = {0,len};
		return data(cp,&r2,&r);
	}
	/**	
	 * Replaces a range of @c char sized memory.
	 */
	inline size_t append(const void *cp, size_t len)
	{
		size_t n = size();
		daeIO::Range r = {n,n+len};
		return data(cp,&r,nullptr);
	}
	/**	
	 * Resizes, padding with undefined memory.
	 */
	inline size_t resize(size_t len)
	{
		size_t n = size();
		daeIO::Range r = {len,size_t(-1)};
		daeIO::Range r2 = {n,len}; 
		return data(nullptr,n<len?&r2:nullptr,&r);
	}

COLLADA_(public) //quasi Standard Library compatibility layer	

	inline size_t size()const{ return data(nullptr,nullptr); }

	inline size_t clear(){ return erase(0,size_t(-1)); }

	inline bool empty()const{ return size()==0; }

COLLADA_(public) //UTILITIES

	static bool less(const daeImageRef &a, daeString b)
	{
		return strcmp(a->_name,b)<0;
	}
};

/**SCOPED-ENUM 
 * These are for querying @c daeAtlas named contents metadata.
 * The selection is very limited at the moment.
 */
struct daeAtlasValue
{
	/**ZAE	 
	 * @c CODE is an encoding method corresponding to the ZIP
	 * format. Popular inter-plugin values can be added later.	 
	 * @c SMALL is the coded size of the named-data--in bytes.
	 * @c LARGE is its decoded size. 
	 * @note @c SMALL may be larger than @c LARGE, and if not
	 * known the value should not be enumerated, but may be 0.
	 * @c TIME comes from ZIP. It's a DOS timestamp concerned
	 * with the time-of-modification only.
	 * @see @c decomposeTIME().
	 *
	 * EXPERIMENTAL
	 * @c DELETE causes I/O operations to fail as if the name
	 * does not exist.
	 * @c _ I'm thinking this (0) is a placeholder and has to
	 * be placed between values that require more than @c int.
	 */
	enum{ DELETE=-1,_,CODE,SMALL,LARGE,TIME };

	template<class T>
	/**EDUCATIONAL 
	 * @a TIME is a FAT/MS-DOS timestamp. 
	 * Months & days are 1-based (January is 1 instead of 0.)
	 */
	static void decomposeTIME
	(T&yr,T&mo,T&d, T&hr,T&min,T&sec, int TIME, int Epoch=1980)
	{
		yr = Epoch+(TIME>>25); mo = TIME>>21&0xF; d = TIME>>16&0x1F;
		hr = TIME>>11&0x1F; min = TIME>>5&0x3F; sec = 2*(TIME&0x1F);
	}
};

/**ZAE
 * @c daeAtlas is a virtual file system; such as a compressed
 * archive. @c daeAtlas IS NOT A COPY of a file--not any more
 * than @c daeDOM is a copy of the Internet. To make an exact
 * copy of a file (in a moment in time) the easiest way is to
 * load it into memory and set @c daeIORequest::string to the
 * view of the memory until the atlas is unloaded from memory.
 * (NOTE: Technically strings are interpreted by the plugin.) 
 *
 * ON PARENTAGE
 * @note daeAtlas is a ref-counted object, however it doesn't
 * belong to a @c daeDOM or a @c daeDatabase. Usually atlases
 * are unparented so that when their @c daeArchive goes, they
 * will go with it. Usually there is only one archive, but it
 * can be that multiple archives in multiple DOMs share these.
 * FURTHERMORE @c daeContainedObject was amended so images do
 * not "strong" reference their atlas...
 *
 * ON IMAGES
 * Use @c daeContainerObject::contain() to assign @c daeImage
 * objects to @c this atlas. They must be created with a name
 * pointer (that is not just a string) that the atlas owns or
 * they will not be recognized by the atlas. NOTE THAT images
 * are a completely optional feature... or that by default an
 * atlas will not have images since the data is drawn from an
 * I/O channel until for some reason it must be cached or got
 * overridden. FURTHER NOTE THE IMAGES MUST BE SORTED. Please
 * use @c findImage() until there is an easier way to do this.
 */
class daeAtlas 
:
public daeContainerObject<0,daeImage>, public daeIOController
{
COLLADA_(protected)
	/**
	 * These are used in order to back the original source of
	 * the atlas. These are implementation dependent. The ZAE
	 * plugin copies the input request with the understanding 
	 * that the string or URI is tied to the atlas's lifetime.
	 * @c daeAtlas::daeAtlas() is available to help set these.
	 */
	daeURI_size<0> _srcURI; daeHashString _srcString;

COLLADA_(public)	
	/**WARNING
	 * Gets source of the atlas via a @c daeIORequest object.
	 * @param req is designed for forwarding to I/O requests.
	 * @remark These should be locked in the beginning of an 
	 * atlas's life. A URI should be unparented and resolved.
	 * @warning Do not reference the URI beyond the duration
	 * of an I/O request. (Also note that this is geared for
	 * read (I) requests.)
	 */
	inline void getSource(daeIORequest &req)const
	{
		if(!_srcURI.empty()) req.remoteURI = _srcURI;
		if(!_srcString.empty()) req.string = _srcString;
	}

	/**
	 * Get @i for use with @c daeContainerObject::getContained() 
	 * and @c daeContainerObject::contain().
	 */
	inline daeOK findImage(daeClientString name, size_t &i)const
	{
		i = std::lower_bound
		(_contained.begin(),_contained.end(),name,daeImage::less)
		-_contained.begin();
		if(i<_contained.size()&&name==_contained[i]->getName())
		return DAE_OK; return DAE_ERR_QUERY_NO_MATCH;
	}

	/**
	 * Iterates an index @a i over the stored images according
	 * to the @a name of the prior iteration. To begin @a name
	 * should be set to @c nullptr and @a i should be set to 0.
	 */
	inline daeClientString nextImage(daeClientString name, size_t &i)const
	{
		daeClientString o = nullptr;
		while(i<_contained.size()&&name==(o=_contained[i]->getName()))
		i++;
		return o;
	}

	/**
	 * Translates a URI's substring into @c this atlas's namespace.
	 * @remark This is needed to write an archive without guessing
	 * about how a URI translates into the atlas's name convention.
	 */
	inline daeClientString name(const daeName &URI_substring)const
	{
		daeArray<daeClientString,1> o; 
		if(DAE_OK!=getNames(o,URI_substring)) return nullptr;
		else assert(1==o.size()); return o[0];
	}
	/**WARNING
	 * Translates a doc (URI-archive) into @c this atlas's namespace.
	 * @warning Docs added to an archive don't necessarily appear in
	 * the archive's atlas.
	 */
	inline daeClientString name(const daeDoc *doc)const
	{
		const daeArchive &a = doc->getArchive(); //CIRCULAR-DEPENDENCY
		return name(doc->getDocURI()-((const daeDoc*)&a)->getDocURI());
	}

COLLADA_(public) //daeAtlas interfaces 
	/**WARNING
	 * Gets a sorted list of raw region "names."
	 * @warning In practice this API should be implemented, since
	 * plugins use it to write/format archives.
	 *
	 * @return Returns an accessory error code which is secondary
	 * to if @a o receives names. When @c wc!=nullptr a @c DAE_OK
	 * code indicates that @wc exactly matches a name returned by
	 * @c o.back() and @c DAE_ERR_QUERY_NO_MATCH must be returned
	 * unless some other code is returned.
	 * If @c wc==nullptr @c DAE_OK indicates normal working order.
	 * @see @c name().
	 *
	 * @note "raw" here means that an atlas can store names as it
	 * likes. Conversion from raw names to URI-friendly string if
	 * necessary will be done with a "getString" API that is also
	 * to be used for text fields (comments, users, etc.)
	 *
	 * @note @c daeImage objects should be added to the container
	 * array in the same order as the names.
	 *
	 * @param o is NOT cleared.
	 * @param wc is a substring to match against. All matches are
	 * outputted. A name is a match if it starts with @c wc. This
	 * is like specifying a directory. Normally a filename cannot
	 * also be a directory; in which case @a wc can translate any
	 * @c daeString into its "interned" pointer/string equivalent.
	 * @warning TECHNICALLY @c wc must be URI-like, and the names
	 * may use a different syntax. They are most likely identical.
	 * @see @c name().
	 */
	virtual daeError getNames(daeArray<daeClientString>&o, const daeName &wc=nullptr)const = 0;
	
	/**WARNING
	 * Gets metadata belonging to names returned by @c getNames().	 
	 * @warning In practice this API should be implemented, since
	 * plugins use it to write/format archives.
	 *
	 * ON IMAGES
	 * Metadata of @c daeImage assigned names such as @c LARGE or
	 * @c SMALL are not up-to-date with respect to the image. The
	 * caller is responsible for iterating over @c getContained()
	 * @see @c daeContainerObject::getContained().
	 *
	 * @param values retrieves metadata about @c named. Care should
	 * be taken when requesting "all available" values since values
	 * may require an I/O operation, that can mean downloading, etc.
	 * If @a values is empty it is filled with all available values.
	 * In this case even elements are @c daeAtlasValue codes.
	 * If @a values is nonempty it is @c daeAtlasValue keys to fill
	 * punctuated-and-terminated by default values.+ (Note that the
	 * reason for this is so the array does not change and so it is
	 * easier to fill out and read back results. +Atlases shouldn't
	 * set a value some times and not other times so default values
	 * don't have to be reset in order to reuse @c values.)
	 *
	 * @param named MUST be either a valid name (pointer) or it may
	 * be @c nullptr to attempt to get metadata of the atlas itself.
	 * @see @c getNames().
	 *
	 * @return Returns a mixed list of @c daeAtlasValue @c enum key
	 * values and their corresponding value-values which might span
	 * multiple @c int items according to the key-value's semantics.
	 *
	 * REMINDER (string values)
	 * This API cannot return strings. Another (getString) API will
	 * be added if there is need or a demand for one. That API will
	 * have to convert metdata fields to strings, and also the name
	 * may not be URI-friendly, in which case it will convert names.
	 */
	virtual daeError getValues(daeArray<int> &values, daeClientString named=nullptr)const = 0;

	/**
	 * @note Here you are editing an archive for some application.
	 *
	 * When an atlas is created it typically already has many names
	 * that're derived from a ZIP-like archive that was just opened.
	 * This API adds all new subimages. 
	 *
	 * @return Returns @c nullptr if a new region cannot be created.
	 * @param error retrieves an error code if it is not @c nullptr.
	 * The returned pointer can be used to create a @c daeImage and
	 * add it to @c this atlas via @c daeContainerObject::contain().
	 *
	 * @remark There is no way to remove names. This is because the
	 * original source of the archive would have to remove its name.
	 * To remove the name from future write operations, @c daeImage
	 * has a @c setIsDeleted() method. A "property" framework might
	 * allow the name to be stricken without creating a @c daeImage.
	 */
	virtual daeClientString addName(const daeName&, daeError *error=nullptr) 
	{
		if(error!=nullptr) *error = DAE_ERR_NOT_IMPLEMENTED; return nullptr;
	}

	/**WARNING
	 * Sets metadata belonging to names returned by @c getNames().	 
	 * @note These changes do not modify the source but are applied
	 * when an archive is written by some I/O operation. 
	 *
	 * @param value has @a valueN elements. @c value[0] selects the
	 * @c daeAtlasValue @c enum to set. The remaining elements must
	 * be either the length of the value to set, or if greater then
	 * it must be a multi-int value with 0 placeholders as required
	 * by @c getValues--OR--if there is none, the value is reverted
	 * to the source's value (a la @c getSource().)
	 *
	 * @param valueN if 1 causes the value to be reverted. Refer to
	 * @a value for 1 and other values of @c valueN.
	 *
	 * @param named MUST be either a valid name (pointer) or it may
	 * be @c nullptr to attempt to set metadata of the atlas itself.
	 * @see @c getNames().
	 *
	 * @warning Setting values faciliates creating/editing archives.
	 * Plugins might not implement this ability, and it is possible
	 * that if an editor does not take great care the metadata will
	 * become out-of-step with the source and/or @c daeImage images.
	 * Metadata such as @c SMALL and @c LARGE are only hints at the
	 * the number of bytes involved. Ultimately @c daeIO::getLock()
	 * returns a size figure, and @c daeImage::size() is the better
	 * way to have the size of images. An atlas may become confused
	 * if it relies on this metadata and affords the ability to set
	 * them and they become set to something different than is best.
	 *
	 * ON DOCS
	 * An atlas is a source of data. A @c daeArchive is a node in a
	 * doc tree. The archive draws from an atlas to populate itself.
	 * @c daeIOPlugin::writeRequest() can in theory work on atlases
	 * however it is designed to work with archives, and so what it
	 * is writing is first the archive's "docs" and second whatever
	 * remains in its atlas. The separation has more to do with who
	 * owns what: DOMs share atlases. DOMs are subjected to scripts.
	 * Atlases emulate static file systems. Atlas data is dumb/flat.
	 */
	virtual daeError setValue(const int *value, size_t valueN, daeClientString named=nullptr)
	{
		return DAE_ERR_NOT_IMPLEMENTED;
	}
	
COLLADA_(public) //HELPERS (DELETIONS)
	/**WARNING
	 * @return Returns @c DAE_OK if the atlas marks its entries for
	 * deletion in this way. 
	 * @warning Names are not removed from @c getNames() and images
	 * take precedence over this.
	 * @see @c setImageDeleted().
	 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.C++}
	 * //Ensure name is absolutely deleted (where supported.)
	 * if(atlas.setNameDeleted(name)) while(atlas.setImageDeleted(name));
	 */
	inline daeOK setNameDeleted(daeClientString name, bool y=true)
	{
		int v[2] = {daeAtlasValue::DELETE,y?1:0}; 
		return setValue(v,2,name);
	}
	/**
	 * Complements @c setNameDeleted().
	 * @return Returns @c DAE_OK if an image is marked for deletion.
	 * Repeated calls will mark all so-named.
	 */
	inline daeOK setImageDeleted(daeClientString name, bool y=true)
	{
		size_t i;
		if(daeError error=findImage(name,i)) 
		return error;
		do if(y!=_contained[i]->getIsDeleted())
		{
			_contained[i]->setIsDeleted(y); 
			return DAE_OK;
		}while(i<_contained.size()&&name==_contained[i]->getName());
		return DAE_ERR_NAME_CLASH;
	}

COLLADA_(public) //OPERATORS

	COLLADA_DOM_OBJECT_OPERATORS(daeAtlas)

COLLADA_(protected)
	/**
	 * Default Constructor
	 */
	daeAtlas(){}
	/**EDUCATIONAL Constructor
	 * Copies the string & URI members of an I/O request originated 
	 * atlas. 
	 * @see @c getSource().
	 * @remarks In most instances the local URI and remote URI will
	 * be identical, however if an archive is moved in the DOM tree
	 * and is then written, there may be incomplete parts that will
	 * have to be filled in with a read I/O channel to the original.
	 */
	daeAtlas(const daeIORequest &req):_srcString(req.string)
	{
		const daeURI *URI = req.remoteURI;
		if(URI==nullptr) URI = req.localURI; if(URI!=nullptr)		
		{
			_srcURI.setURI(URI->data());						
			_srcURI.setIsResolved(); //absolute/resolved
			_srcURI.setIsAttached(); //unparented
			_srcURI.setIsDirectoryLike(); //archive-like
		}
	}
};

//---.
}//<-'

#endif //__COLLADA_DOM__DAE_ATLAS_H__
/*C1071*/
