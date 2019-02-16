/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH
#include <ColladaDOM.g++> //GCH

COLLADA_(namespace)
{//-.
//<-'

extern daeTypewriter &daeStringRef_xs_string;
extern daeTypewriter *daeStringRef_xs_(daeName);
extern daeTypewriter *daeStringRef_sys_typewrit(intptr_t);

extern size_t daeStringRef_namespaceTag(daeStringRef_base); //emits error	
void XS::Schema::__XS__Schema__construct
(bool invis, 
int philosophy, //COLLADA_DOM_PHILOSOPHY
int production, //COLLADA_DOM_PRODUCTION
int generation, //COLLADA_DOM_GENERATION
daeClientString2 ns, daeClientString2 version)
{	
	if(invis) //This is a pseudo constructor.
	new(static_cast<__Schema__invisible*>(this)) __Schema__invisible;
				
	_targetNamespace = ns; _version = version; _agent = nullptr;

	_error = DAE_OK; _users = 0;

	size_t nt = daeStringRef_namespaceTag(daeStringRef(ns));
	if(0==nt&&!ns.empty())
	_error = DAE_ERR_NOT_IMPLEMENTED; //overflow 	
	_elementTags = daeTags(_ptr->tags); 	
	_elementTags.namespaceTag = 0xFF&nt;
	_elementTags.nameTag = 1;
	_elementTags.moduleTag|=1; //_isData

	_IDs.push_back("id"); _IDs.push_back("sid");

	//There could be wiggle room here, but for now, require that all 3 agree.
	assert(generation==COLLADA_DOM_GENERATION
	&&philosophy==COLLADA_DOM_PHILOSOPHY&&production==COLLADA_DOM_PRODUCTION);

	//*3/2 is to give US a heads up
	daeCTC<__size_on_client_stack*1/2>=sizeof(*this)>();
}
void XS::Schema::__XS__Schema__destruct(bool invis)
{
	daeStringMap<XS::SimpleType*>::const_iterator it;	
	for(it=_simpleTypes.begin();it!=_simpleTypes.end();it++)	
	{
		it->second->~SimpleType(); //delete it->second;	
	}

	for(size_t i=1;i<_XMLNS.size();i++)	
	if(_XMLNS[i].hasSchema()) 
	_XMLNS[i].getSchema()->_users--;
	//DEBUGGING
	assert(0==_users); //REMOVE ME

	if(invis) //This is a pseudo destructor.
	static_cast<__Schema__invisible*>(this)->~__Schema__invisible();

}//CIRCULAR-DEPENDENCY
XS::SimpleType::~SimpleType(){ getRestriction().~Restriction(); };

DAEP::Model &daeProcessShare::getOpaqueModel()
{
	if(_opaqueModel==nullptr)
	_opaqueModel = &addModel<0>((daeObject*)nullptr,empty_daeString1);			
	return *_opaqueModel;
}static int daeMetaSchema_bogus_family_counter = 0;
daeModel &daeProcessShare_base::_addModel(int size, daeFeatureID finalID, int ext_chars)
{
	//TODO: +size is a feature-map that in practice doesn't see much use, and especially
	//not in the case of elements, which are the vast majority of objects.
	size_t features = (int)finalID*-int(sizeof(daeFeature));
	size_t fullsize = features+size+sizeof(daeMetaObject)+ext_chars;
	daeOpaque mem = memset(__DAEP__Make__v1__operator_new(fullsize),0x00,fullsize);
	daeModel &mod = (daeModel&)mem[features];	
	daeMetaObject &out = (daeMetaObject&)daeOpaque(&mod)[size];
	const_cast<daeMetaObject*&>(mod.__DAEP__Model__meta) = &out;

	//UNIMPLEMENTED
	//This is supposed to be an ID that if equal means the elements are binary equivalent.
	//This is so daeSafeCast can work with identical classes coming from different modules.
	const_cast<daeInt&>(mod.__DAEP__Model__family) = --daeMetaSchema_bogus_family_counter;
	
	out._processShare = static_cast<daeProcessShare*>(this);
	out._sizeof = size;
	out._finalFeatureID = finalID;	
	if(0!=finalID)
	mod[daeFeatureID(-1)]._flags.feature_1 = 1;
	out._deleteList = _deleteList; _deleteList = &out; return mod;
}
extern daeAlloc<XS::Attribute*,1> daeAA_any_thunk;
daeMetaElement &XS::Schema::_addElement(long long schema, int size, daeFeatureID finalID, daeClientString2 name)
{
	assert((int)finalID<=-3);	
	//The calling method checks these formulas.
	int genus = 1+int(schema>>48);
	int attribsN = 0x3F&int(schema>>32);
	//FYI: Thunks are not being assigned to the DAEP::Child features.
	//domAny has a "value" member even though it's saying it's MIXED.
	int thunks = 1+attribsN, addvalue = 0;
	int addAny = schema&64?sizeof(daeAny):0; //This is checked below.
	if((schema&3)==daeContentModel::SIMPLE) 
	{
		thunks++; addvalue = sizeof(daeDefault);
	}	
	else assert(genus!=daeObjectType::ANY); //mixed?
	int diff = sizeof(daeMetaElement)-sizeof(daeObject);
	int back = addAny+addvalue+thunks*sizeof(daeAllocThunk);
	back+=attribsN*sizeof(XS::Attribute);
	daeModel &mod = _addModel(size,finalID,diff+size+back);
	mod.setObjectType(genus);

	daeMetaElement &out = 
	const_cast<daeMetaElement&>(mod.__DAEP__Model__elements_meta());	
	{
		//_addModel() set up the daeMetaObject ones.
		out._name = name;
		out._feature_completeID = daeFeatureID(-thunks);
		out._DAEP_Schema = schema;
		assert((0==addAny)==!out.getAllowsAnyAttribute());
	}	
	
	union //HEADS UP!
	{
		char *behind_prototype;
		daeAllocThunk *thunkp; 
		XS::Attribute *attrib;
	};	
	daeFeatureID fit = out._feature_completeID;
	behind_prototype = &daeOpaque(out._prototype)+size;
	if(0!=addAny)
	behind_prototype+=sizeof(out._bring_to_life(out._any()));
	
	memset(attrib,0x00,sizeof(XS::Attribute)*attribsN);
	daeArray<XS::Attribute*> &attribs = out._attribs();	
	if(0!=attribsN)
	{
		//HACK: Allocate an exact amount since single pointers
		//have a minimum capacity of 16 at the time of writing.
		//out._bring_to_life(attribs).grow(attribsN+1);		
		out._bring_to_life(attribs);
		attribs.getObject().reAlloc(attribs.getAU(),attribsN+1);
		//The _offset holds the number of static attributes.
		(*attribs.getAU())->_offset = attribsN; 
		for(int i=0;i<attribsN;i++) attribs.push_back(attrib++);
		attribs.push_back(daeAA_any_thunk._varray[0]);
		attribs.getAU()->setInternalCounter(attribsN);
	}
	else attribs.getAU() = daeAA_any_thunk.unit();	
	if(0!=addvalue) 
	{
		out._value = (daeDefault*)behind_prototype; 
		behind_prototype+=sizeof(daeDefault);

		//daeAST::AnyWriter::_reset_type was using this.
		assert(attribsN==0||out._value==attribs.back()+1);

		//HACK: setAllocThunk() expects initialized memory.
		mod[fit++]._localthunk = thunkp++; //setAllocThunk();
	}
	while(fit) mod[fit++]._localthunk = thunkp++; //setAllocThunk();
		
	if(!out.getIsLocal())
	{
		//_globalTypes lookup (findChild) is typically done
		//with NCNames.
		assert(nullptr==strchr(name.string,':'));
		_globalTypes[name] = &out; 
	}

	return out;
}
 
static void daeMetaSchema_deleteAllocThunk(daeMetaObject *self) 
{
	daeModel &mod = self->getModel();
	for(daeFeatureID it=self->rbeginFeatureID();it;it++)
	{
		daeFeature &f = mod[it];
		if(1==f.getFlags().new_thunk) f.deleteAllocThunk();
	}
}
void daeMetaObject::_feature_complete(daeFeatureID fID)
{
	for(const daeFeature *f=&getModel()[fID];fID;fID++,f++)
	{	
		if(!f->getFlags()) continue;

		if(1==f->_flags.subobject)
		{
			int i = -(int)fID-1; 
			assert(i<(int)sizeof(_subobjectsMask)*CHAR_BIT);
			_subobjectsMask|=1<<i;			
		}
		if(1==f->_flags.atomizing) _features_atomize = 1;
		if(1==f->_flags.new_thunk)
		{
			_deleteAllocThunk = 1;
			if(_destructor_ptr==nullptr)
			_destructor_ptr = daeMetaSchema_deleteAllocThunk; 
		}
	}	
}

//HACK! NOT-THREAD-SAFE _addList() sets this up in advance.
static const XS::SimpleType *daeMetaSchema_base = nullptr;
XS::SimpleType &XS::Schema::_addAtom(daeTypewriter *writ, const daeName *base, const daeName *name)
{	
	assert(name!=nullptr);

	bool itemType; if(name->string==nullptr)
	{
		assert(base!=nullptr);

		itemType = false; name = base; base = nullptr;
	}
	else itemType = base==nullptr; //_addList?

	assert(!itemType||daeMetaSchema_base!=nullptr);

	//There's room for some optimization here.
	XS::SimpleType* &out = _simpleTypes[*name];
	if(out!=nullptr)
	{
		daeMetaSchema_base = nullptr;
		assert(out==nullptr); return *out; //unacceptable
	}
	size_t size = sizeof(XS::SimpleType);
	if(base!=nullptr) 
	size+=sizeof(XS::Restriction)-daeSizeOf(XS::SimpleType,_restriction);
	//(void*&)out = operator new(size);
	(void*&)out = _a().allocate(size);
		
	if(base!=nullptr)	
	if(daeMetaSchema_base!=nullptr)
	{
		//HACK: NOT THREAD-SAFE
		out->_restriction = daeMetaSchema_base;
	}
	else out->_restriction = findType(*base);
	else out->_restriction = nullptr;

	XS::Restriction &r = out->getRestriction();	
	if(r)	
	{
		//String-pooling must be in play.
		assert(base!=nullptr);
		assert(base->string==r.getBase().getName().string); 
		//_minmax is setup by the calling/inline APIs.
		r._length[0] = 0; r._length[1] = ~r._length[0];	
		r._compare[0] = INT_MIN;  r._compare[1] = INT_MAX;
		r._enumeration = 0; 
	}
	else assert(base==nullptr);
	if(r) //hack?
	{
		//Normally the library does not inherit anything from the base.
		//That's the generators' job. This is an edge-case however. It's
		//intended to catch a length restricted list, that is implemented
		//in terms of a fixed array, deriving from an unresticted daeArray.
		out->_itemType = r.getBase().hasList()?r.getBase()._itemType:out;
	}
	else out->_itemType = out; out->_name = *name; 
	
	//Here a "writ" is a partial typewriter. 
	intptr_t system_type = (intptr_t)writ;
	if(std::abs(system_type)<=64) //daeAtomicType?
	{
		daeTypewriter *xs; if(r) //Derived?
		{
			xs = r.getBase().getValueTypewriter();	
		}
		else if(itemType) //Hack: itemType?
		{
			//Prefer built-in typewriter. hexBinary requires
			//this as long as BINARY doesn't encode the base.
			xs = daeMetaSchema_base->getValueTypewriter();
		}
		else xs = daeStringRef_xs_(*name);
		int t = 0; //HACK: Assuming string->token is alright.
		if(xs==nullptr||((t=xs->getAtomicType())!=system_type)
		&&t<daeAtom::STRING
		&&0!=system_type) //HACK: xs:anySimpleType?
		xs = daeStringRef_sys_typewrit(system_type);		
		out->_value_typewriter = xs;	
	}
	else out->_value_typewriter = writ; //Should be a real typewriter.
		
	return *out;
}
XS::SimpleType &XS::Schema::_addList(daeTypewriter *writ, daeClientString2 base_or_item, daeClientString2 name)
{
	//This is a little convoluted so that it
	//can be implemented in terms of _addAtom().
	const XS::SimpleType *base = findType(base_or_item);
	assert(base!=nullptr);
	//String-pooling must be in play.
	assert(base->getName().string==base_or_item.string);
	daeMetaSchema_base = base;
	const daeName *list = base->hasList()?&base_or_item:nullptr;
	XS::SimpleType &out = _addAtom(writ,list,&name);
	daeMetaSchema_base = nullptr;
	//Note, _addAtom() inherits "itemType" if <xs:list> based.
	if(list==nullptr) out._itemType = base;
	assert(out.hasList()||out.getRestriction()); 
		
		//This problem will have to be dealt with one day.
		assert(!out._value_typewriter->isAnySimpleType());

	out._value_typewriter = out._value_typewriter->where<daeArray>(); return out;
}

XS::SimpleType &XS::Schema::_addEnum
(int N, const daeClientEnum *e, const daeClientEnum *re, daeClientString2 base, daeClientString2 name, int ext)
{	
	assert(name!=nullptr&&base!=nullptr);
	//There's room for some optimization here.
	XS::SimpleType* &out = _simpleTypes[name];
	if(out!=nullptr)
	{
		assert(out==nullptr); return *out; //unacceptable
	}
	size_t size = sizeof(XS::SimpleType);
	size+=sizeof(XS::Restriction)-daeSizeOf(XS::SimpleType,_restriction);
	size+=sizeof(XS::Enumeration)-daeSizeOf(XS::Restriction,_enumeration);
	size+=ext>0?ext:sizeof(daeEnumType);
	//(void*&)out = operator new(size);
	(void*&)out = _a().allocate(size);
	
	out->_name = name;
	out->_itemType = out; 	
	//There's room for some optimization here.				
	out->_restriction = findType(base);
	XS::Restriction &r = out->getRestriction(); assert(r);	
	{
		//String-pooling must be in play.
		assert(base.string==r.getBase().getName().string);
		r._length[0] = 0; r._length[1] = ~r._length[0];			
		r._compare[0] = INT_MIN;  r._compare[1] = INT_MAX;
		r._setMinMax<daeEnumeration>();
		r._enumeration = N; 
	}	
	XS::Enumeration &xe = r.getEnumeration(); assert(xe);
	xe._enum = e; xe._enumEnd = e+N;
	xe.string_table._begin = re; xe.string_table._end = re+N;
	out->_value_typewriter = *new(&xe+1)daeEnumType(xe); return *out;
}

void XS::Schema::addXMLNS(daeClientString2 key_or_value, daeClientString2 value)
{
	daeTags et; size_t i;
	if(value==nullptr) et = daeXMLNS(key_or_value);
	if(value!=nullptr) et = daeXMLNS(key_or_value,value);
	if(_XMLNS.find(et,i))
	{
		//Generators can't assign more than one prefix
		//to the same XMLNS. They should merge them if
		//necessary. It's not really an error, but the
		//client should be aware of this.
		assert(et.nameTag==_XMLNS[i]._xmlns.nameTag); 
	}
	else
	{
		_XMLNS.push_back(et);	
		//Generators must call addXMLNS once, with the 
		//schema's XMLNS first.
		if(1==_XMLNS.size()) _XMLNS[0]._schema = this;
	}
}
daeTags XS::Schema::useXMLNS(const XS::Schema &xs)
{
	if(&xs==this)
	{
		//If this fails the generator needs to reorder addXMLNS 
		//so the schema is first added.
		assert(_XMLNS[0]._schema==this);
		return _XMLNS[0]._xmlns;
	}
	bool ins = true;
	XS::Import o(xs._elementTags.namespaceTag,&xs);	
	size_t i; if(_XMLNS.find(o._xmlns.namespaceTag,i))
	{
		XS::Import &xsi = _XMLNS[i];
		if(ins=&xs!=xsi._schema)
		{
			if(xsi.hasSchema()) 
			{
				while(++i<_XMLNS.size())
				if(&xs==_XMLNS[i]._schema)
				{
					assert(!_XMLNS[i].hasAttribute_xmlns());
					return _XMLNS[i]._xmlns;
				}
				goto o;
			}
			else xsi._schema = &xs;
		}		
		o._xmlns = xsi._xmlns;
	}
	else o: _XMLNS.push_back(o);

	if(ins) xs._users++; return o._xmlns;
}

//---.
}//<-'

/*C1071*/
