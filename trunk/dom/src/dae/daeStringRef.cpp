/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */											 
#include <ColladaDOM.inl> //PCH

COLLADA_(namespace)
{//-.
//<-'

#ifdef EXPORTING_COLLADA_DOM
//This is conceptually the first thing
//that is initialized. The rest follow.
COLLADA_(extern) daePShare DOM_process_share = 0;
#else
//Static linked modules own DOM_process_share.
static daeOK _1st_things_1st = DOM_process_share.grant();
#endif

static struct daeStringRef_dummyAtlas : daeAtlas
{
virtual daeIO *openIO(daeIOPlugin&,daeIOPlugin&)
{ return nullptr; }
virtual daeOK closeIO(daeIO*,daeOK)
{ return DAE_ERR_CLIENT_FATAL; }
virtual daeError getNames(daeArray<daeClientString>&,daeName)const
{ return DAE_ERR_NOT_IMPLEMENTED; }
virtual daeError getValues(daeArray<int>&,daeClientString)const
{ return DAE_ERR_NOT_IMPLEMENTED; }
}_daeStringRef_dummyAtlas;
extern daeAtlas &daeStringRef_dummyAtlas = _daeStringRef_dummyAtlas;

/**128-1-1
 * Arbitrary figure, divisible by 8, 
 * minus the #, and minus the 0-terminator.
 * @todo "COLLADA_DOM_LARGE_SYSTEM_STRING"?
 */
enum{ daeStringRef_large=126 }; 

static daeSmallStringTable<> daeStringRef_table(0);

COLLADA_(extern) daeString daeStringRef_empty = daeStringRef_table.allocString("",0);

daeString daeStringRef_system(daeString cp, size_t extent)
{
	if(extent==0) return daeStringRef_empty;

	daeString out;
	bool frag = '#'==cp[0]; 
	if(frag){ cp++; extent--; }
	if(extent>=daeStringRef_large-1) //1 is added to len.
	{
		void *large = operator new
		(daeOffsetOf(daeDBaseString,fragment[1+extent+1]));
		out = *new(large)daeDBaseString(0,1,cp,extent)+1;		
	}
	else out = daeStringRef_table.allocString(cp,extent); 
	return out+(frag?-1:0);
}

//pool registration
static std::vector<daeStringPool<>*> daeStringRef_pools;
static std::vector<unsigned short> daeStringRef_poolstack;
static struct daeStringRef_0 : daeStringPool<> //system pool
{	
	//daeStringPool methods
	virtual void release(String string)
	{
		if(string->fragmentN>=daeStringRef_large) delete string;
	}
	virtual daeString ref(daeString cp, size_t extent)
	{
		return daeStringRef_system(cp,extent);
	}
	virtual void rollover(String &string, String &source, int factor)
	{
		//Just handle the unlikely case that a large-string has 32768 refs.
		if(0==string->refs&&string->fragmentN<daeStringRef_large)
		{
			#ifdef NDEBUG
			#error string.string should be added to a list with a link to
			#error its rollover string, so its rollover can be reassigned.
			#error It may also be a good idea to emit a graphical warning.
			#endif
			string->refs-=2; 
			string.string = daeStringRef_system(string.string,string.size);
			string->refs+=1;
			source.string = string.string; 			
		}
	}
}daeStringRef_0; //The constructor registers it...
daeStringPool_pool::
daeStringPool_pool():pool(daeStringRef_poolstack.empty()?
daeStringRef_pools.size():daeStringRef_poolstack.back())
{
	if(pool<daeStringRef_pools.size())
	{
		daeStringRef_poolstack.pop_back();
		daeStringRef_pools[pool] = (daeStringPool<>*)this;	
	}
	else daeStringRef_pools.push_back((daeStringPool<>*)this);
}
daeStringPool_pool::~daeStringPool_pool()
{
	daeStringRef_pools[pool] = nullptr;
	daeStringRef_poolstack.push_back((unsigned short)pool);
}

//pool API
void daeStringRef::_release() 
{
	daeStringRef_pools[_ptr->pool]->release(_string);
}
void daeStringRef::_ref_and_release(daeString cp)
{
	_ref_and_release(cp,strlen(cp));
}
void daeStringRef::_ref_and_release(const daeStringRef &cp)
{
	_ref_and_release(cp,cp.size());
}
void daeStringRef::_ref_and_release(daeString cp, size_t extent)
{
	daeStringPool<> *pool = daeStringRef_pools[_ptr->pool];
	daeString string = _string; _string = pool->ref(cp,extent);	
	pool->release(string);
}
void daeStringRef::_ref_and_release(const daeHashString &cp)
{
	_ref_and_release(cp.string,cp.extent);
}
void daeStringRef::_rollover(daeStringRef &source, int factor) 
{
	daeStringRef_pools[_ptr->pool]->rollover
	((daeStringPool<>::String&)_string
	,(daeStringPool<>::String&)source,factor);
}

//prototype-constructor
void daeStringRef::_ref(const daeDOM &DOM)
{
	DOM._database->_prototype(*this,DOM._databaseRejoinder);
}

//array-emplacement
void daeStringRef::_ref(const daeObject *c, daeString cp) 
{
	_ref(c,cp,strlen(cp));
}
void daeStringRef::_ref(const daeObject *c, daeString cp, size_t len) 
{
	assert(cp!=nullptr);
	const daeDOM *DOM = c==nullptr?nullptr:c->getDOM();
	if(DOM!=nullptr) _ref(*DOM,cp,len); else _ref(cp,len);
}
void daeStringRef::_ref(const daeObject *c, const daeStringRef &cp) 
{
	const daeDOM *DOM = c==nullptr?nullptr:c->getDOM();
	if(DOM!=nullptr) _ref(*DOM,cp); else _ref(cp);
}
void daeStringRef::_ref(const daeObject *c, const daeHashString &cp)
{
	_ref(c,cp.string,cp.extent);
}

//constructor
void daeStringRef::_ref(const DAEP::Object &c, daeString cp) 
{
	_ref(c,cp,strlen(cp));
}
void daeStringRef::_ref(const DAEP::Object &c, daeString cp, size_t len) 
{
	assert(cp!=nullptr);
	const daeDOM *DOM = dae(c).getDOM();
	if(DOM!=nullptr) _ref(*DOM,cp,len); else _ref(cp,len);
}
void daeStringRef::_ref(const DAEP::Object &c, const daeHashString &cp)
{
	_ref(c,cp.string,cp.extent);
}
//constructor/subroutine
void daeStringRef::_ref(const daeDOM &DOM, daeString cp, size_t len) 
{
	_string = DOM._database->_ref(cp,len,DOM._databaseRejoinder);
}
void daeStringRef::_ref(const daeDOM &DOM, const daeHashString &cp) 
{
	_ref(DOM,cp.string,cp.extent);
}
//system-pool/subroutine
void daeStringRef::_ref(const daeString cp)
{
	_string = daeStringRef_system(cp,strlen(cp));
}
void daeStringRef::_ref(const daeString cp, size_t len)
{
	_string = daeStringRef_system(cp,len);
}
void daeStringRef::_ref(const daeHashString &cp) 
{
	_string = daeStringRef_system(cp.string,cp.extent);
}

//constructor
void daeStringRef::_ref(const DAEP::Object &c, const daeStringRef &cp) 
{
	const daeDOM *DOM = dae(c).getDOM();
	if(DOM!=nullptr) _ref(*DOM,cp); else _ref(cp);
}
//constructor/subroutine
void daeStringRef::_ref(const daeDOM &DOM, const daeStringRef &cp) 
{
	//paranoia: Shouldn't happen. Not even for a prototype.
	assert(&DOM!=nullptr); 

	daeString empty = DOM.getEmptyURI().data();
	if(cp._ptr->pool!=((daeStringRef&)empty)._ptr->pool)
	_string = DOM._database->_ref(cp,cp.size(),DOM._databaseRejoinder);
	else new(this) daeStringRef(cp);
}
//system-pool/subroutine
void daeStringRef::_ref(const daeStringRef &cp)
{
	_string = 0==cp._ptr->pool?cp:daeStringRef_system(cp,cp.size());
}
				 
//system-pool
void daeStringRef::_ref(){ _string = daeStringRef_empty; }


////////////////////////////////////////////////////////////////////////
// These are globals that depend on the system string pool having been//
// initialized. (daeStringRef.cpp may or may not be initialized first)//
////////////////////////////////////////////////////////////////////////

template<class T>
static daeType<T> &xs_best_fit()
{
	static daeType<T> out; return out; 
}
daeCTC<sizeof(long long)==8> daeStringRef_long_long_check;
COLLADA_(extern) daeAnySimpleType
daeAnySimpleType_string = daeAnySimpleType(xs_best_fit<daeStringRef>()), 
daeAnySimpleType_hexBinary = daeAnySimpleType(xs_best_fit<daeBinary<16>>()),
daeAnySimpleType_boolean = daeAnySimpleType(xs_best_fit<daeBoolean>()),
daeAnySimpleType_double = daeAnySimpleType(xs_best_fit<double>()),
daeAnySimpleType_long = daeAnySimpleType(xs_best_fit<long long>()),
daeAnySimpleType_int = daeAnySimpleType(xs_best_fit<int>()),
daeAnySimpleType_ulong = daeAnySimpleType(xs_best_fit<unsigned long long>()),
daeAnySimpleType_uint = daeAnySimpleType(xs_best_fit<unsigned int>());
void daeAnySimpleTypewriter::_assert_daeAnySimpleTypewriter()
{
	//This may misfire if the globals are not contiguous in memory.
	assert(this>=daeAnySimpleType_string&&this<=daeAnySimpleType_uint+3);
}
COLLADA_(extern) daeTypewriter daeMetaElement_voidwriter,
&daeStringRef_xs_anySimpleType = *daeAnySimpleType_string,
//THESE MUST AGREE WITH daeDomTypes.h
//* technically should be string
#define xs_(x,y) &daeStringRef_xs_##y = xs_best_fit<x>()//;
xs_(daeStringRef,string),
xs_(daeStringRef,normalizedString),
xs_(daeStringRef,duration),
xs_(daeStringRef,dateTime),
xs_(daeStringRef,date),
xs_(daeStringRef,time),
xs_(daeStringRef,gYear),
xs_(daeStringRef,gYearMonth),
xs_(daeStringRef,gMonth),
xs_(daeStringRef,gMonthDay),
xs_(daeStringRef,gDay),
xs_(daeBinary<16>,hexBinary),
xs_(daeBinary<64>,base64Binary),
xs_(daeStringRef,anyURI),
xs_(daeTokenRef,QName),
xs_(daeTokenRef,NOTATION),
xs_(daeBoolean,boolean),
xs_(daeFloat,float),
xs_(daeDouble,double),
xs_(daeDouble,decimal), //*
xs_(daeLong,long),
xs_(daeLong,integer), //*		
xs_(daeLong,nonPositiveInteger), //*
xs_(daeLong,negativeInteger), //*	
xs_(daeInt,int),
xs_(daeShort,short),
xs_(daeByte,byte),
xs_(daeULong,unsignedLong),
xs_(daeULong,nonNegativeInteger), //*
xs_(daeULong,positiveInteger), //*
xs_(daeUInt,unsignedInt),
xs_(daeUShort,unsignedShort),
xs_(daeUByte,unsignedByte),
xs_(daeTokenRef,token),
xs_(daeTokenRef,language),
xs_(daeTokenRef,Name),
xs_(daeTokenRef,NCName),
xs_(daeTokenRef,NMTOKEN),&
daeStringRef_xs_NMTOKENS = daeStringRef_xs_NMTOKEN,
xs_(daeTokenRef,ID),
xs_(daeTokenRef,IDREF),&
daeStringRef_xs_IDREFS = daeStringRef_xs_IDREF,
xs_(daeTokenRef,ENTITY),&
daeStringRef_xs_ENTITIES = daeStringRef_xs_ENTITY;
#undef xs_

//A content-typewriter is a big side project.
//(It'd basically be an XML document "writer.")
COLLADA_(extern) daeAlloc<daeCounter,0> daeStringRef_counterLT(0);
COLLADA_(extern) daeTypewriter &daeStringRef_counterTW = xs_best_fit<daeCounter>();
static XS::Schema domAny_xs(0); XS::Schema::Schema(int ps)
{
	__XS__Schema__construct(false, //__invisible,
	COLLADA_DOM_PHILOSOPHY,COLLADA_DOM_PRODUCTION,COLLADA_DOM_GENERATION,"","");
	//HACK: Assign the special 0 process-share, or: why this constructor exists.
	_ps = ps; assert(ps==0&&this==&domAny_xs);
}
domAny::_Master domAny::_master(domAny_xs);
//PREVIOUSLY of domAny.cpp (R.I.P.)
//These are currently at the bottom
//of daeStringRef.cpp to ensure that
//daeStringRef_anySimpleType is handy.
//static XS::Schema domAny_xs("","");
//domAny::_Master domAny::_master(domAny_xs);
domAny::_Master::_Master(XS::Schema &xs)
:meta(xs.addElement(toc,"")),model(meta.getModel())
{
	//CHICKEN/EGG: Normally addElement (above) does
	//this; but it was just used to initialize meta.
	const_cast<daeMetaElement&>(meta)
	._jumpIntoTOC(1)._element.child = &meta;

	xs.addType<xs::anySimpleType>("xs:anySimpleType");

	daeMetaElement &el = const_cast<daeMetaElement&>(meta);

	el.addAnyAttribute().setProcessContents("lax");

	el.addValue(toc->value,"xs:anySimpleType");

	#ifdef NDEBUG
	#error This can be removed. What is a domAny?
	#error Using a nonzero ordinal may be unwise.
	#endif
	daeCM *cm = nullptr;
	el.addCM<XS::Sequence>(cm,0,1,1);
	el.addCM<XS::Any>(cm,0,0,-1).setProcessContents("lax");
	el.addContentModel<1,1>(cm,toc);

	//This may be 2 since changing from 0 to 1 being unnamed.
	//Work on _continue_XS_Schema_addElement2 until it's not.
	assert(1==el.getTOC().size());
	
	domAny &pt = daeOpaque(el._prototype);
	
	//HACK: This assigns domAny to a special 0 process-share.
	(&pt._getClassTag())[3] = 1;
	//HACK: _isAny() now needs this to tell it's not Doc/DOM.
	(&pt._getClassTag())[1] = 0x80;
	assert(0==pt._getPShare()&&pt._isData()&&!daeUnsafe<domAny>(pt));
}
//This must be destructed absolutely last.
static struct daeStringRef_protoDOM : daeDoc
{
	daeStringRef_protoDOM():COLLADA_SUPPRESS_C(4355)daeDoc((daeDOM*)this,daeDocType::ARCHIVE)
	{
		(&_getClassTag())[1] = 2; assert(_isDOM()); 
	}
}protoDOM;
//This struct is just needed for an empty URI for daeStringRef.
//It could be a daeDOM, but that has a rather larger footprint.
COLLADA_(extern) daeDOM &daeStringRef_protoDOM = *(daeDOM*)&protoDOM;

extern daeTypewriter *daeStringRef_xs_(daeName name)
{
	if(name[0]!='x'||name[1]!='s'||name[2]!=':')
	return nullptr;
	name.string+=3; name.extent-=3;
	#define _(x) if(#x==name) return &daeStringRef_xs_##x//;
	switch(name[0])
	{
	case 'a':_(anyURI);_(anySimpleType);
	break;
	case 'b':_(boolean);_(base64Binary);_(byte);
	break;
	case 'd':_(double);_(dateTime);_(date);_(decimal);_(duration);
	break;
	case 'E':_(ENTITY);_(ENTITIES).where<daeArray>();
	break;
	case 'f':_(float);
	break;
	case 'g':_(gDay);_(gYear);_(gMonth);_(gYearMonth);_(gMonthDay);
	break;
	case 'h':_(hexBinary); 
	break;
	case 'i':_(int);_(integer);
	break; 
	case 'I':_(ID);_(IDREF);_(IDREFS).where<daeArray>();
	break;
	case 'l':_(long);_(language); 
	break;
	case 'n':_(normalizedString);_(negativeInteger);_(nonNegativeInteger);_(nonPositiveInteger);
	break;
	case 'N':_(NCName);_(Name);_(NMTOKEN);_(NMTOKENS).where<daeArray>();_(NOTATION); 
	break;
	case 'p':_(positiveInteger); 
	break;
	case 'Q':_(QName); 
	break;
	case 's':_(string);_(short);
	break;
	case 't':_(token);_(time);
	break;
	case 'u':_(unsignedLong);_(unsignedInt);_(unsignedShort);_(unsignedByte); 
	break;
	}return nullptr;
	#undef _
}
extern daeTypewriter *daeStringRef_sys_typewrit(intptr_t st)
{
	//These are broken up in case the values are the same
	//(on exotic systems.)
	switch(st) 
	{
	case daeAtomicType::INT: return xs_best_fit<daeInt>();
	case daeAtomicType::UINT: return xs_best_fit<daeUInt>();
	case daeAtomicType::FLOAT: return xs_best_fit<daeFloat>();		
	}		
	switch(st)
	{
	case daeAtomicType::LONG: return xs_best_fit<daeLong>();
	case daeAtomicType::ULONG: return xs_best_fit<daeULong>();
	case daeAtomicType::DOUBLE: return xs_best_fit<daeDouble>();
	case daeAtomicType::BOOLEAN: return xs_best_fit<daeBoolean>();	
	case daeAtomicType::TOKEN: return xs_best_fit<daeTokenRef>();
	case daeAtomicType::STRING: return xs_best_fit<daeStringRef>();	
	}
	switch(st)
	{
	case daeAtomicType::BYTE: return xs_best_fit<daeByte>();
	case daeAtomicType::UBYTE: return xs_best_fit<daeUByte>();
	}
	switch(st)
	{
	case daeAtomicType::SHORT: return xs_best_fit<daeShort>();
	case daeAtomicType::USHORT: return xs_best_fit<daeUShort>();
	//To avoid this assert, derive a user type from @c daeBinary.
	//A BINARY must be named "xs:hexBinary" or "xs:base64Binary."
	case daeAtomicType::BINARY: assert(st!=daeAtomicType::BINARY);
	}
	assert(0); return nullptr; //this can't end well
}

//---.
}//<-'

/*C1071*/
