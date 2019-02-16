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

static size_t daeMetaElement_colon(daeName &q)
{
	for(size_t o=1;o<q.extent;o++) if(':'==q[o])
	{
		//NOTE: Returning position beyond colon.
		//SEE-ALSO: _findChild3
		o++; q.string+=o; q.extent-=o; return o; 
	}
	return 0;
}

daeSA daeMeta::_allocator()const{ return &_schema->_A; }

void daeMeta::_cloneTOC(daeElement *e, const daeElement *cp)const
{
	daeOffset from = _clearTOC_offset;
	daeOffset to = _content_offset;
	assert(to>=from&&to<(daeOffset)_sizeof&&from>=(int)sizeof(DAEP::Element));
	memcpy((char*)e+from,(char*)cp+from,to-from);
}
void daeMeta::_clearTOC(daeElement *e)const
{
	_cloneTOC(e,_prototype);
	//_content_thunk.offset is not used above because it cannot get
	//pseudo-elements, as they are the prototype. So not to branch:
	{
		//SINCE cursor() must now be set, just doing it manually:
		//enum{ sz=sizeof(daeContents)-sizeof(daeContents_base) };
		//daeChildID::POD cp[1+sz/sizeof(daeChildID)] = {0x10000};
		//assert(0==sz%sizeof(daeChildID)&&daeChildID(1,0)==cp[0]);
		//memcpy((char*)e+to,cp,sizeof(cp));
		daeContents &c = getContentsWRT(e);
		(daeCounter&)c.unnamed() = 0x10000; c.cursor() = c.data();
	}
}

//Formerly daeMetaElement::_construct_new.
extern const daeStringCP _daePseudoElement_[];
daeElement *daeElement::_addElement(daeMeta &me)
{
	daePseudoElement *c = this;
	daeDOM *DOM = const_cast<daeDOM*>(c->getDOM());
	#ifdef NDEBUG
	#error Try to factor into introduceToDocument() somehow.
	#endif
	//Hack? Because the calling APIs bypass _reparent_element_to_element()
	//as an optimization; its pseudo-parent logic is being reproduced here.
	if(1==c->__DAEP__Element__data.is_document_daePseudoElement)
	{
		(const daeObject*&)c = &c->getParentObject();		
		assert((void*)c==c->getDoc()&&(void*)c==((daeDoc*)c)->_getDocument());
	}
	else assert(_daePseudoElement_!=c->getNCName().string);

	return me._construct(c,DOM);
}	   
daeElementRef daeMeta::create(daeObject *c)
{
	daeDOM *DOM; switch(int got=c->getObjectType())
	{	
	case daeObjectType::DOM: DOM = static_cast<daeDOM*>(c); 
							 break;
	case daeObjectType::DOC: if(static_cast<daeDoc*>(c)->isDocument())
							 goto DOM;
	//NOTE: Falling-through, and pseudo-elements should return nullptr.
	default: if(got<daeObjectType::ANY) return nullptr;
							 DOM: DOM = const_cast<daeDOM*>(c->getDOM());
	}	
	return _construct(c,DOM);
}
daeElement *daeMeta::_construct(DAEP::Object *parent, daeDOM *DOM)const
{
	//_prototype is passed to the database so it can have a
	//little bit of information with which to initialize its
	//extended data structure.	
	daeElement *o = _prototype;
	DOM->_database->_new(_sizeof,o,DOM->_databaseRejoinder);	
	((daeObject*)memcpy(o,_prototype,_sizeof))->_parent(*parent); 	
	(daeDOM*&)o->__DAEP__Element__data.DOM = DOM; 
	_constructor(o); return o;
} 

//SCHEDULED FOR REMOVAL/REFACTOR
inline daeMeta *daeMeta::_findChild3(daeName &permanent)const
{
	//This might be helpful with reoccurrences???
	//(_elem_names_cache is not designed for this.)
	XS::Element *thread_safe_cache = _elem_names_cache;
	const daeName &cmp = thread_safe_cache->getNCName();
	if(*(short*)permanent.string
	 ==*(short*)cmp.string&&cmp==permanent)
	{
		permanent.string = cmp.string;
		return thread_safe_cache->getChild();
	}

	_elem_names_iterator it;	
	it = _elem_names.find(permanent); if(it==_elem_names.end())
	{
		daeMeta *me; if(getAllowsAny()) 
		{
			/*Subroutine::createWRT must do this with XMLNS.
			//VERY NAIVE xs:any SUPPORT
			//NOTE: COLLADA 1.5.0 cannot benefit from this as
			//its type names have "_type" suffixes... although
			//it will work when suffixed.
			me = _schema->findChild(permanent);
			if(me!=nullptr)
			{
				permanent.string = me->getName().string;
				return me;
			}*/
			me = &domAny::_master.meta;
		}
		else me = nullptr;
		//Hack? Detect presence of ':' in name, in case
		//it's why no match is had. Otherwise returns 0.
		//permanent.extent = 0;
		while(0!=permanent.extent)
		if(':'==permanent.string[--permanent.extent])
		{
			//NOTE: Returning position beyond colon.
			//SEE-ALSO: daeMetaElement_colon
			permanent.extent++; break;
		}
		return me;
	}
	//Save the name for create/placeWRT().
	thread_safe_cache = _elem0+it->second;
	_elem_names_cache = thread_safe_cache;
	permanent.string = thread_safe_cache->getNCName().string; 
	return jumpIntoTOC(it->second).getChild();
}
daeMeta *daeMeta::_findChild2(daeString pseudonym)const
{
	daeName nc = pseudonym; return _findChild2(nc);
}
daeMeta *daeMeta::_findChild2(const daePseudonym &pseudonym)const
{
	daeName nc = pseudonym; return _findChild3(nc);
}

//ORDER-IS-IMPORTANT
static XS::SimpleType daeMetaElement_typeless;
COLLADA_(extern) daeAST::Data daeAA_void_terminator(nullptr); 
COLLADA_(extern) daeAlloc<XS::Attribute*,1> daeAA_any_thunk(1);
static XS::Attribute daeMetaElement_valueless(0); 
XS::Attribute::Attribute(int) 
{
	//HACK: Piggybacking on module initialization.
	(daeAA_any_thunk._varray[0] = (XS::Attribute*)
	&daeAA_void_terminator)->_type.writer = &daeAtomicType_void;

	memset(this,0x00,sizeof(*this)); 
	setIsProhibited();
	XS::SimpleType *st = &daeMetaElement_typeless;
	_type.datatype = st;
	st->_itemType = st;
	st->_value_typewriter = 
	_type.writer = &daeAtomicType_void;
	st->_name =
	_attribute_name_string = (char*)&_offset;
	st->_restriction = nullptr;
}

extern daeDOM &daeStringRef_protoDOM;
//A content-typewriter is a big side project.
//(It'd basically be an XML document "writer.")
extern daeTypewriter &daeStringRef_counterTW;
extern daeAlloc<daeCounter,0> daeStringRef_counterLT;
void *daeMeta::_continue_XS_Schema_addElement2(void ctor(DAEP::Object*), daeFeatureID union_feature, daeOffset content_offset)
{
	//NEW: Slip in daeAnyAttribute and dummy values.
	const int any_value = _value==nullptr?1:2;
	//HACK: Can XS::Schema::addElement set this up??
	if(any_value==1) _value = &daeMetaElement_valueless; 
	
	//XS::Schema::addElement has initialized a lot of the model already by this stage.	
	_bring_to_life(_elems);
	_bring_to_life(_elem_names);
	_elem_names.get_allocator() = _allocator();
	_destructor_ptr = daeMetaElement_self_destruct;
	_constructor = ctor;
	size_t features = -(int)_finalFeatureID;	
	size_t attribsN = _attribs().size();
	assert(attribsN<features);
	size_t single_elems_32 = 0;
	size_t single_elems = (int)union_feature-(int)_finalFeatureID-1;
	assert(single_elems<features&&single_elems!=1);
	size_t approx_elems = features-attribsN-any_value-1;
	assert(approx_elems<features&&approx_elems>=1);
	//Hack: this constructs, and decreases the count.
	//The positive childID count isn't truly knowable, but this should be a tight fit.	
	_elems.grow(approx_elems);			
	for(size_t i=0;i<approx_elems;i++) 
	_bring_to_life(_elems.data()[i]);_elems.getAU()->setInternalCounter(approx_elems);	
	_elem0 = _elems.data()+single_elems-1;
	XS::Element &e1 = _elem0[1];	
	{
		//In lieu of a default constructor...				
		e1._daeCM_init(XS::ELEMENT,this,nullptr,0,0,-1);		
		e1._substitute_maxOccurs = -1;
		memset(&e1._element,0x00,sizeof(e1._element));
		e1._maxOrdinals = 1;
		e1._element.name._clear_client_string();
		//CHICKEN/EGG: 0 if domAny's metdata.
		e1._element.child = domAny::_master.meta;

	  //This remaining code is just to avoid having junk data inside getTOC().

		if(single_elems>0) //Zero fill the bogus e0 void?		
		{
			single_elems_32 = single_elems/32+(single_elems%32>0?1:0);

			*_elem0 = e1; 
			//HACK: The lower bound is used to orient the _deepCM entry point.
			_elems[0]._element.childID = daeChildID(-int(single_elems-1),0);
		}
		size_t iN=approx_elems-single_elems; if(iN>1) //Zero fill in e2 and up?
		{
			//_elem0 is a hole, now only because of dae_Array's legacy 0 mode.
			//The single element bitfield is located in front of e1, which is
			//always available to permit "unnamed" elements. This means e2 is
			//a hole if there are single elements, and there may be more than
			//32 single elements, in which case the bitfield goes into e3 and
			//so on. In addition, because of 64bit alignment needs, there can
			//be an additional hole on odd numbers. So the last one is filled
			//in just in case that is the case. This is all just so that if a
			//user iterates over these elements, they won't have junk in them.
			size_t zerofill = 2+single_elems_32;
			if(zerofill%2==0) zerofill++; //One more for remainder/x64 logic?
			for(size_t i=1;i<zerofill&&i<iN;i++) _elem0[i+1] = e1;
		}
	}			
	size_t union_size = sizeof(dae_Array<>);
	daeOffset union_offset = content_offset-union_size;
	{
		//Automatically add the unnammed dae_Array, since it's always present.
		_addChild2(&e1,1,union_offset);
	}
	//HACK: This is a starting point. _addContentModel() adds plural children.
	_clearTOC_offset = union_offset-single_elems_32*sizeof(daeChildID::POD);
	//addContentModel() may never be called.
	_CMRoot = _CMEntree = _elem_names_cache = &e1; _IDs = _IDs_id = nullptr;
	//Set up the content/cursor features so
	//that generator output doesn't have to.
	{
		//HACK: Just getting this out of the way.
		//TODO: It would be better to addFeature
		//here at the same time for all features.
		//It's just not so straightforward to do.
		daeModel &mod = getModel();				
		{
			//These are the cursor-arrays. They just appear to be counters.		
			for(daeFeatureID fit=_finalFeatureID+1;fit!=_feature_completeID;fit++)
			{
				//Maybe these could be special somehow?
				mod[fit].setAllocThunk(daeStringRef_counterLT);
				mod[fit].setTypewriter(daeStringRef_counterTW);	
			}
		}
		//This is the "unnamed" cursor array.		
		mod.addFeature(union_feature,union_offset);		
		assert(mod[union_feature]->getSize()==union_size); 				
		daeFeature &content = mod.addFeature(_finalFeatureID,content_offset);		
		//Unimplemented: a contents-array typewriter
		//would effectively be an XML text<->binary job.
		content.setTypewriter(nullptr/*daeMetaElement_contentTW*/); 				
		//This is dicey. The 1 capacity is so
		//daeContent::__COLLADA__move() can not
		//branch on empty to set up the partition.
		//REMINDER: re/deAlloc won't deallocate a 1.		
		new((void*)&_content_thunk) daeAlloc<daeContent,0>(1); //-Wplacement-new
		content.setAllocThunk(&_content_thunk);		
		assert(0==content_offset%sizeof(void*));
		_content_offset = content_offset; //Computable...
		_content_thunk._offset = content_offset+daeOffsetOf(daeContents,getAU()); //_au 		
	}	
	_prototype->__DAEP__Element__data.NCName = _name;
	(daeMeta*&)_prototype->__DAEP__Element__data.meta = this;
	(daeDOM*&)_prototype->__DAEP__Element__data.DOM = (daeDOM*)&daeStringRef_protoDOM;

	//NEW: Do DAEP::Object's constructor to setup namespaceTag.
	_prototype->_getObjectTags() = daeObjectTags(_schema->_elementTags);
	//hack: Set parent to self without setting reference count.
	daePseudoElement2(_prototype).context->parent = _prototype;

	//Assign the prototype's thunk. This couldn't be more clumsy.
	daeContents &c = getContentsWRT(_prototype);
	c.getAU() = (daeAlloc<daeContent>*)&_content_thunk; c.cursor() = &_content_0_terminator;
	assert(c.cursor()==c.data());
	//This is the only time user/client headers see the prototype.
	return _prototype;
}

#ifdef NDEBUG
#error What are suitable attributes?
#endif
static const daeAny daeMetaElement_anyless;
const daeAny &daeMeta::getAnyAttribute()const
{
	return getAllowsAnyAttribute()?_any():daeMetaElement_anyless;
}

extern daeAST::AnyWriter daeAnySimpleType_void;
extern daeAST::AnyWriter daeAnySimpleType_string;
XS::Attribute &daeMeta::_addAttribute(daeFeatureID fID, daeOffset os, 
daeAlloc<>&lt(daeAllocThunk&), daeClientString2 type, daeClientString2 name)
{
	const bool value = name==nullptr;
	//Second -1 assumes xsAnyAttribute. 
	const int i = -fID-1-1; assert(i>=0);
	daeDefault &out = value?*_value:*_attribs()[i];
	
	const XS::SimpleType &st = *_schema->findType(type);
	
	//Just some generator sanity checks.
	assert(nullptr!=&st&&nullptr!=&out); 

	daeTypewriter &tw = st.getValueTypewriter();

	daeFeature &mf = getModel().addFeature(fID,os,tw.getSize());
	{
		mf.setTypewriter(tw);		
		//These thunks assigned by _addElement().
		//But setAllocThunk() is not yet done to them.
		//(It could've been, but it would be pointless too.)
		daeAllocThunk &t = const_cast<daeAllocThunk&>(mf.getAllocThunk());
		{
			lt(t); //t._prototype = &out._type;
			//Here _offset is array->object. It must be 0
			//unless the thunk is actually used by arrays.
			if(st.hasList()) t._offset = os;	
		}//HACK: set _flags.atomizing as side-effect. 
		mf.setAllocThunk(&mf.getAllocThunk()); 
		
		//If this is the last attribute, or the value
		//and the attributes are set up (allowing some
		//unnecessary flexibility to the generator) the
		//model features need to be finalized. The child
		//features are trivial, and so are not considered.		
		if(fID==_feature_completeID) _feature_complete(_feature_completeID);
	}

	//out._meta = this;
	if(tw==daeAnySimpleType_void)
	{	
		out._anySimpleType = 2;
		daeAST::TypedUnion &ast = daeOpaque(_prototype)[os];
		os+=daeOffsetOf(xs::anySimpleType,_union);		
		ast._offset = os;
		ast._type = &daeAnySimpleType_void; 
		ast._union._long = 0;
		(const void*&)ast._union.hexBinary.AU = &mf.getAllocThunk();
	}
	if(value) //if(name==nullptr) //value?
	{
		//out._is_not_attribute = 1;

		//out._attribute_name._clear_client_string();
		out._attribute_name_string = daeStringRef_empty; 
		out._attribute_name_length = 0;		
	}
	else //Not implementing truncation... assuming not.
	{
		out._attribute_name_string = name.string;	
		out._set_attribute_name_length_etc(name);
	}		
	out._offset = os; assert(os>sizeof(daeElement));
	out._type.datatype = &st;	
	out._type.writer = &tw; 
	out._type.value = daeOpaque(_prototype)[os];
	if(st.hasList())
	(const void*&)out._type.value = &mf.getAllocThunk();	
	/*REMOVING 
	if(st.getRestriction())
	{
		out._type.minLength = st.getRestriction().getMinLength();
		out._type.maxLength = st.getRestriction().getMaxLength();
	}
	else if(st.hasList())
	{
		out._type.maxLength = ~(out._type.minLength=0); 
	}
	else out._type.minLength = out._type.maxLength = 1;	
	*/
	
	if(tw.hasStringType())
	{
		(daeString&)out._type.value = daeStringRef_empty;

		_addAttribute_maybe_addID(out,_prototype); //ID?
		
		//Schema can't define prefixes on users' behalf.
		//if(q-qq==5&&"xmlns"==daeName(qq,5))
		if("xmlns"==name) 
		{	
			//ASSUMING FIRST ATTRIBUTE (findNamespace_xmlns)
			//<xs:appinfo>enable-xmlns</xs:appinfo>
			//TODO: This practice is peculiar to the COLLADA schema.
			//It should probably be disabled so it's unnecessary to
			//do this/a special test for findNamespace_xmlns.
			assert(i==0);

			out._setIsXmlns(_prototype);
		}
	} 	
	if("xml"==out.getQNamePrefix())
	{
		int b = 8;
		if("lang"==out.getNCName()) b = 4; 
		if("base"==out.getNCName()) b = 2;	
		//Deferring xml:base to optimize URI resolution.
		if(0!=out._anySimpleType)
		{
			//HACK: Using to decide to take the time to
			//find the attribute.
			_systemAttribute = 1;
		}
		else out._setIsXml(b,_prototype);
	}
	return static_cast<XS::Attribute&>(out);	
}
void daeMeta::_addAttribute_maybe_addID(daeAttribute &maybe_ID, const daeElement *proto_or_any)
{
	if(nullptr==_schema->_IDs.find(maybe_ID.getName()))
	return;	
	XS::Attribute *it = const_cast<XS::Attribute*>(_IDs); 
	XS::Attribute *ID = const_cast<XS::Attribute*>(&maybe_ID);
	if(nullptr!=it)
	{
		//Assuming attributes are unsorted for lookup.
		assert(it<ID);

		while(it->_next_attribute_is_ID) 
		it = it+it->_next_attribute_is_ID; 
		while(it<ID)
		{
			int diff = int(ID-it);
			assert(diff>0);
			it->_next_attribute_is_ID = diff; 
			if(diff!=(int)it->_next_attribute_is_ID) 
			{
				assert(0); //NOT EXPECTING
				//HACK: Determine the maximum delta
				//of a bitfield. It's not worth doing
				//this in a header.
				it->_next_attribute_is_ID = 0;
				diff = it->_next_attribute_is_ID-=1;
			}
			it+=diff;
		}; assert(it==ID);
	}else _IDs = it = ID;
	it->_this_attribute_is_ID = 1; 	
	if("id"==it->getName()) _IDs_id = it; //LEGACY		
	proto_or_any->__DAEP__Element__data.getMeta_getFirstID_is_nonzero = 1;
}
extern daeAnySimpleType daeAnySimpleType_hexBinary;
void daeDefault::_setDefaultString(daeHashString def)
{	
	//I think this can do without _meta?
	//daeElement *pt = _meta->_prototype;
	daeElement &pt = _type.value[-_offset];

	assert(pt->_isData());
	//Turn the data-bit off for the last time.
	//It's falsely indicating there is a database.
	//(So that _construct() doesn't have to flip it on.)
	//NOTE: THIS LIKELY ONLY APPLIES TO ARRAYS (xs:list)
	//(AND ONLY IF THEY GROW BEYOND THEIR THUNK AT THAT)
	//(MAYBE daeStringRef TOO, BUT I ALMOST CERTAIN NOT)
	//(daeBinary is also arrays)
	pt->_getObjectTags().moduleTag^=1; 
	assert(!pt->_isData());
	{	
		//2018: Indicate via daeData that there is cause
		//to look for the default value?
		if(0==_fixed_default)
		_fixed_default = 1; assert(!def.empty());

		daeOK OK; //DEBUGGING

		if(_type.writer==&daeAnySimpleType_void) 
		{
			//Assuming no schema would do this, but it's
			//a loop-hole. https://www.w3.org/TR/xmlbase/
			//advises against this, but does not forbid it. 
			if(4==getNCNameCP()&&"xml:base"==getName())
			{
				//Illustrating how to close this loop-hole.
				_setIsXml(2,&pt); assert(0);
			}

			//EXPERIMENTAL
			//WORTHWHILE? This is preventing hexBinary default strings
			//and also hexBinary lists in defaults, because copying them
			//in the prototype is extravagant compared to what little this
			//type offers versus a string.
			//This code could "assert(0)" if the prototype constructor handles
			//the BINARY case.
			size_t size;
			daeAST::AnyWriter *ast = ast->_type(def.string,def.extent,size);
			if(ast==daeAnySimpleType_hexBinary
			 ||ast==daeAnySimpleType_hexBinary+1)
			ast = &daeAnySimpleType_string;
			OK = ast->_unserialize2(def.string,def.extent,_type.value,size);
		}
		else //standard operating procedure
		{
			OK = _type->stringToMemory(def,_type.value); 
		}

		#ifdef _DEBUG
		//Ensure equivalent to old "_default" member.
		//assert(_default==nullptr); _default = def;
		__vizDefault = def; daeCharData check;
		defaultToString(check);
		if(def!=check) switch(getDefaultType().getAtomicType())
		{		
		case -32/CHAR_BIT: //case daeAtom::INT:

			//I think this is 64-bit however on 32-bit
			//builds daeDomTypes.h is making 64-bit to
			//be 32-bit for integers. It's possible to
			//override this on a case by case basis.
			//https://www.khronos.org/bugzilla/show_bug.cgi?id=1948
			//gl_pipeline_settings stencil_mask value
			if(check=="-1"&&def=="4294967295")
			check = def;
			break;

		case COLLADA_DOM_ATOMIC_FLOAT(float):
		case COLLADA_DOM_ATOMIC_FLOAT(double):
			check.append_and_0_terminate(".0",2);
			break;
		//case daeAtom::BOOLEAN:
		//	if("0"==check) check = "false";
		//	if("1"==check) check = "true";
		//	break;
		case daeAtom::EXTENSION:
			//Assuming arrays are correct until someone says
			//otherwise.
			check = def;
			break;
		}
		assert(OK&&check==def);
		#endif		
	}
	pt->_getObjectTags().moduleTag^=1; 
}	

/*
//SCHEDULED FOR REMOVAL
//TODO: Allocate everything with getSchema()._allocator.
//(Then just delete that.)
daeParentCM::~daeParentCM()
{
	for(size_t i=0;i<_CM.size();i++) _CM[i]->_self_delete();
	//TODO? _deepCM could be part of a variable-length daeParentCM.
	delete[](_deepCM+_meta->getTOC().front().getChildID().getName());
}
void daeCM::_self_delete()const 
{
	//NOTE: This is very picky about which classes have
	//containers like std::vector (e.g. XS::Choice). At
	//some point daeStringAllocator will be used so the
	//memory can just be left alone.
	switch(_xs)
	{	
	case XS::CHOICE:		
		delete static_cast<const XS::Choice*>(this); break;
	case XS::SEQUENCE: case XS::ALL:	
		delete static_cast<const daeParentCM*>(this); break;
	case XS::ANY: case XS::GROUP: delete this; break;
	default: assert(_xs==XS::ELEMENT);
	}
}*/

static void daeMetaElement_newGroup(daeCM* &out, daeTOC &TOC, daeSA a)
{
	size_t namesN = TOC.getCapacity(); short *jump_table;
	(void*&)out = a.allocate(sizeof(XS::Group)+namesN*sizeof(*jump_table)); //new
	jump_table = (short*)((char*)out+sizeof(XS::Group));	
	new(out) XS::Group(jump_table-TOC[0].getChildID().getName());
	//__subsetof() needs to know if the names exist or not.
	for(size_t i=0;i<namesN;i++) jump_table[i] = XS::Group::_groupNames_not_included;
}
daeCM &daeMeta::_addCM(daeXS xs, daeParentCM *parent, int (&i4)[4])
{
	daeCM *out; switch(xs) //Initialize.
	{
	case XS::ALL: out = _new<XS::All>(); break;
	case XS::ANY: out = _new<XS::Any>(); break;
	case XS::CHOICE: out = _new<XS::Choice>(); break;
	case XS::ELEMENT: //The real element is resolved by setChild().
	new(out=&_schema->_temporary_working_memory)XS::Element; break;
	case XS::GROUP:	daeMetaElement_newGroup(out,getTOC(),_allocator()); break;
	case XS::SEQUENCE: out = _new<XS::Sequence>(); break;
	default: assert(0); return *(out=nullptr); //???
	}
	out->_daeCM_init(xs,this,parent,(daeCounter)i4[0],i4[1],i4[2]);	
	assert(i4[2]!=0); //assert(maxO!=0);

	if(daeCM::isParentCM(xs)) //Allocate the off-center child ID jump-table.
	{	
		//The daeParentCM destructor undoes this.
		daeParentCM &pout = (daeParentCM&)*out;		
		pout._CM.get_allocator() = _allocator();
		pout._CM.reserve(i4[3]);
		size_t avail = _elems.size();
		if(xs==XS::CHOICE) 
		{
			avail*=2; //_proclaimCM //OVERKILL//OVERKILL//OVERKILL//OVERKILL
			((XS::Choice*)&pout)->_solutions.get_allocator() = _allocator();
		}
		//TODO: do something like daeMetaElement_newGroup() once more solid.
		pout._deepCM = _new<daeParentCM::_DeepCM_string>(avail);
		for(size_t i=0;i<avail;i++)
		pout._deepCM[i].get_allocator() = _allocator();
		pout._deepCM-=_elems[0].getChildID().getName();
	}	
	else assert(parent!=nullptr||xs==XS::GROUP); //These are XML Schema rules.

	if(nullptr!=parent) //Add the CM to its parent.
	{
		//Check the generator has passed a parent type.
		assert(parent->isParentCM()); 
		
		parent->_CM.push_back(out);		

		//This is the XS::Any equivalent of setChild().
		//Establish <xs:any> path for unnamed children.
		//(Technically, depending on how <xs:any> are setup,
		//they can be sinks for named children as well. It's not
		//clear what the library's auto-placement facility should do.)
		if(XS::ANY==xs) parent->_deepCM_push_back(1,out);
	}
	else //Assign the CM graph's root pointer.
	{
		//Check the generators are not reassigning/nothing's changed.
		assert(_CMRoot==_elem0+1); 
		//_elem0 is the default for empty/any content-models.		
		_CMRoot = out;
	}
	return *out;
}
 
void XS::Group::_setGroup(daeMeta &group)
{
	_groupCM = group.getCMEntree(); //Or getCMRoot()?

	/*The generator would not be able to do this unless it
	//has both schema's on hand. It would be a big project.
	//NOTE: Groups aren't given proper names or namespaces
	//however these can be reconstructed from the metadata
	//name and XS::Schema::_XMLNSes, preferring the prefix 
	//of the latter.
	if(isImportedGroup()) getSchema().useXMLNS(group.getSchema());
	*/assert(!__isImportedGroup());
}
XS::Element *XS::Group::_addChild(daeFeatureID fid, daeOffset os, daeClientString2 name)
{
	daeMeta &group = getGroup();
	daeMeta::_elem_names_iterator 
	itt = group._elem_names.end(),
	it = group._elem_names.find(name); if(it==itt) missing2:
	{
		//<gles2_value_group><usertype><setparam><array> requires this.
		XS::Schema::_MissingChild c = { &group,_meta,this,fid,os,name };
	
		//HACK: If name lookup fails, it may be a QName. It's deferred
		//since the full name needs to be forwarded, and also unlikely.
		if(it==itt&&0!=daeMetaElement_colon(c.name))
		if((it=group._elem_names.find(c.name))!=itt) 
		{
			goto q; //QName
		}
		else c.name = name;

		_meta->getSchema()._missingChildren.push_back(c); return nullptr;
	}
q:	int ii = it->second;
	const XS::Element &cp = group.jumpIntoTOC(ii);
	//In this case _setChild2 is waiting on _setChild3 to return.
	if(cp._element.child==nullptr) goto missing2;
	XS::Element &real = 
	const_cast<daeMetaElement*>(_meta)->_addChild(cp,fid,os,name);		
	int i = real.getChildID().getName(); _groupNames[i] = ii; 
	if(_parentCM!=nullptr) _parentCM->_deepCM_push_back(i,this); return &real;
}
void daeMeta::_addContentModel(const daeCM *generator_root)
{
		//INDENTED CODE CONSTITUTES THE NORMAL FUNCTIONING OF THIS API.
		assert(generator_root==_CMRoot); 
	
	std::vector<XS::Schema::_MissingChild> 
	&missing = getSchema()._missingChildren;
	for(size_t i=0;i<missing.size();i++)
	{
		if(this==missing[i].meta) //There's a ciruclar transclusion situation?
		return;
	}
		//INDENTED CODE CONSTITUTES THE NORMAL FUNCTIONING OF THIS API.
		_CMEntree = const_cast<daeCM*>(_CMRoot)->_prepareContentModel();

	for(size_t j,i=0;i<missing.size();) 
	{	
		if(this==missing[i].ref) //There's a ciruclar transclusion resolution?
		{
			XS::Schema::_MissingChild &e = missing[i];
			if(nullptr==e.CM_graph_node->_addChild(e.fid,e.os,e.name)) 
			assert(0);
			daeMeta *swap = missing[i].meta;
			//Could do in bulk with erase(remove_if) but C++98 is not worth it.
			missing.erase(missing.begin()+i);
			for(j=i;j<missing.size();j++) if(swap==missing[j].meta)
			goto erased;
			const_cast<daeMetaElement*>(swap)->_addContentModel(swap->_CMRoot);
			i = 0; //just start over in case _addContentModel removed elements. 
		}
		else i++; erased:;
	}
		//HACK: it will be less if there are only non-plural children.
		_clearTOC_offset = std::min<size_t>(_clearTOC_offset,_elems.back().getOffset());

	//REMINDER: DO AFTER _missingChildren ADDITIONS 
	//XMLNS: Cannot do this in _addChild because it
	//doesn't get the metadata pointer.
	XS::Schema &xs = *_schema;
	for(size_t i=0;i<_elems.size();i++)	
	for(XS::Element*p=&_elems[i];p!=nullptr;p=p->_element.namefellows)
	{	
		const XS::Schema &cmp = p->getChild().getSchema();
		daeTags et = xs.useXMLNS(cmp);
		p->_element.namespaceTag = et.namespaceTag;
		p->_element.nameTag = et.nameTag;
		//Setting to 0 in case isGrouplicated is in an interwed configuration.
		p->_element.child_is_import = xs!=cmp?1:0;
	}
}
XS::Element &XS::Element::_setChild3(daeFeatureID fid, daeOffset os, daeClientString2 name)const
{	
	assert(this==&_meta->getSchema()._temporary_working_memory);
	return const_cast<daeMetaElement*>(_meta)->_addChild(*this,fid,os,name);
}
XS::Element &daeMeta::_addChild(const XS::Element &cp, daeFeatureID fid, daeOffset os, daeClientString2 QName)
{	
	assert(os>=(int)sizeof(daeElement));

	daeName NCName = QName;
	size_t colon = daeMetaElement_colon(NCName);

	//This could be easier if _addElement() had more members.
	daeOffset pivot = _elem0-_elems.begin();
	daeOffset i,toc = _content_offset-os;
	XS::Element *out;
	if(toc>(int)sizeof(daeCounter))
	{
		//positive dae_Array
		i = toc/sizeof(daeCounter);
		assert(i>=2&&0==toc%sizeof(daeCounter));
	}
	else //negative dae_Array
	{
		i = -pivot-1+(fid-_finalFeatureID);		
		assert(i<0&&toc==(int)sizeof(daeCounter));
	}
	//This assert should indicate the content model is likely
	//importing a child that shares a name with another child
	//that is either imported or not.
	//If this happens createWRT and pushBackWRT will have use
	//the QName prefix to choose from among the "namefellows".
	//Failing that, the CM graph would just have to be probed.
	//_elem_names[NCName] = (int)i;
	int &ins = _elem_names[NCName];
	assert(ins==0||ins==i);
	ins = (int)i;
	out = &_jumpIntoTOC(i); //This includes a bounds check.
	bool first_instance_of_name = 0==out->_element.offset;

	//TODO: This can reasonably be done in bulk by addElement().
	//But it should be double-checked here if/when it's done so.
	if(first_instance_of_name) getModel().addFeature(fid,os);

	//Determine if this is the first <xs:element> with this name,
	//-and if not, link it into the namefellow singly linked list.
	if(!first_instance_of_name)
	{	
		while(out->_element.namefellows!=nullptr) 
		out = out->_element.namefellows;
		out = out->_element.namefellows = _new<XS::Element>();
	}	
	out->_element.namefellows = nullptr; //0 terminate their list.
	//Copy the addCM() information.
	memcpy(out,&cp,sizeof(daeCM));
	
	if(cp._meta!=this) //Transcluding?
	{
		//Group elements retain the parent of their content-model, and
		//if namefellows are preset, they're deleted in _self_destruct.
		out->_meta = this;
		out->_element.child = cp._element.child;
		out->_substitute_maxOccurs = cp._substitute_maxOccurs;
	}
	else //Repair the CM graph and fill out jump-tables.
	{	
		out->_element.child = nullptr; //For XS::Group::_addChild.
		daeParentCM *p = out->_parentCM;
		assert(p!=nullptr&&p->_CM.back()==cp);		
		p->_CM.back() = out; p->_deepCM_push_back((size_t)i,out);
	}
	//Finish up.
	assert(NCName!=nullptr);
	out->_element.name = NCName; 
	out->_element.qualified_name_offset = colon;
	out->_element.namefellows_demote = 0; return _addChild2(out,(int)i,os);	
}
XS::Element &daeMeta::_addChild2(XS::Element *out, int i, daeOffset os)
{
	out->_element.offset = os;
	//There is ambiguity about where the only-child (i<0) kind are located.
	//This is the understood address because they share pointer-to-members.
	out->_element.content_offset = (i>0?i:1)*-daeOffset(sizeof(daeCounter));
	out->_element.childID = daeChildID(i,0);
	//Set up the dynamic dae_Array<T,0> ID. The counter would be zero either way.
	if(i>0) (daeChildID&)out->getWRT(_prototype) = out->_element.childID; return *out;
}

struct daeMeta::Subroutine //Trying to declutter daeMetaElement.h.
{	
	//NOTE: Anything that can go here should, since it communicates
	//that it's not required to be visible outside of daeElement.cpp.

	/**LEGACY-SUPPORT
	 * This tries to determine whether to take the typeLookup() path.
	 * If the element has content it's generally assumed that the graph
	 * must be traversed, unless the source/destination documents are the
	 * same, or a global flag that says typeLookup() has never been called.
	 */
	static bool typeLookup_unless(daeElement &e)
	{	
		//Unfortunately, this requires deep inspection.
		//This bit is set if the contents-array's been touched.
		if(!e.getMigrationBit_definitely_is_childless())
		{
			//This is to avoid deep inspection if users
			//aren't calling daeDocument::typeLookup() at all.
			//extern bool daeDocument_typeLookup_called;
			return daeDocument_typeLookup_called;
		}
		//In practice, this can't work in the prototype.
		//Should daeDocument_typeLookup_called be checked first?
		//daeDocument_typeLookup_called is off in some other part of memory.
		return e.getMeta()._daeDocument_typeLookup_enabled;
	}

	template<bool any, bool add>
	/**
	 * Common section of @c daeMeta::createWRT().
	 */
	static daeElement *createWRT(daeMeta *me, 
	daeElement *p, daeTags xmlns, const daeName &NCName)
	{
		#ifdef NDEBUG
		#error setNamespace needs thought.
		#endif
		//TODO? The parent's namespace can in theory be changed
		//by setNamespace, so to not correspond to its metadata.
		//domAny's metadata doesn't have a namespace of its own.

		//Going ahead and testing both tags
		//in order to detect !xmlns.found().
		daeTags cmp = p->getElementTags();

		//If optimized this is no different.
		//if(!p->hasNamespaceTag(xmlns))
		if(xmlns.namespaceTag!=cmp.namespaceTag
		||(xmlns.nameTag!=cmp.nameTag&&!add))
		{	
			const XS::Schema *x = &me->getSchema();

			if(add) //_addWID
			{
				//This is ambiguous; prefering the parent XMLNS 
				//only so an "xmlns" attribute is not generated.
				if(xmlns.empty())
				{
					xmlns = cmp; goto add;
				}

				//If not found, then the prefix defaults to the
				//schema's reference. It should be added by the
				//database during its migration.				
				p->findQNamePrefix_xmlns(xmlns);
			}
			else if(0==xmlns.namespaceTag)
			{
				assert(me==&domAny::_master.meta);

				if(!xmlns.found())
				{
					//This is a problem, but it's important to keep
					//this sentinel namespace value out of elements.

					assert(xmlns.found()); 

					xmlns.clear(); //goto any; //Why was this goto?
				}

				if(any) //pushBackWRT?
				{					
					//How is <xs:any> interpreted? I don't know. But
					//files generated by Maya add nonschema elements
					//to <technique> without any xmlns/prefix change.
					if(cmp.second()==xmlns.second()) xmlns = cmp;
				}
			}
			
			if(!p->getProvisionalDocument()._get_and_or_use_xs_schema(xmlns,x))		
			{
				if(x==nullptr) 
				{
					if(any) goto any; 
					else return nullptr;
				}
				if(!add||me==&domAny::_master.meta)
				{
					//_addWID can't tolerate different metadata from the one
					//that its ID is bound to.
					me = x->findChild(NCName);
				}
				else assert(!add);
				if(me==nullptr) any:
				{
					if(any) me = &domAny::_master.meta;
					else return nullptr;
				}
			}
		}
		else if(add) xmlns.nameTag = cmp.nameTag; add:

		daeElement *o = p->_addElement(*me);
	
		o->setNamespace(xmlns); o->getNCName() = NCName; return o;	
	}

	/**
	 * Implements daeMetaElement_self_destruct(). 
	 */
	static void self_destruct(daeMetaElement &self) 
	{
		//VERY DICEY PROTOTYPE DESTRUCTION PROCEDURE
		{
			daeElement *pt = self._prototype;

			 //DEFECT REPORT
			//For this to work, the DAEP::Proto constructors
			//must not copy on an "empty" value, in order to
			//not copy the prototype into itself/leak memory.

			assert(pt->_isData());
			//Turn the data-bit off for the last time.
			//It's falsely indicating there is a database.
			//(So that _construct() doesn't have to flip it on.)
			pt->_getObjectTags().moduleTag^=1; 
			assert(!pt->_isData());

			//It's not clear that this would work in general, but
			//it's too late at this stage to be deleting elements.
			//Since their metadata may've already been destructed.
			assert(self.getContentsWRT(pt).empty());
			const daeArray<daeAttribute*> &attribs = self.getAttributes();
			daeAttribute*const *a = attribs.data();
			for(size_t i=0,iN=attribs.size();i<iN;i++)		
			if(nullptr!=a[i]->_destructor)
			{
				//These should be equivalent.
				//a[i]->_destructor(&a[i]->getWRT(pt));
				a[i]->_destructor(&a[i]->_type.value);
			}
		}
	
		if(self._value!=&daeMetaElement_valueless)
		{
			//This likely does nothing, but you never know.
			self._value->~daeDefault(); 
			//This is the only real opportune time to check 
			//that generators set up daeContentModel::SIMPLE.
			assert(&self._value->getSimpleType()!=nullptr);
		}

		//The _offset holds the number of static attributes.
		(*self._attribs().getAU())->_offset = 0;

		//TODO: Implement schema wide daeStringAllocator.
		//Destructs/deletes CM-graph, except elements go 
		//down with _elems. ~Element deletes namefellows.
		//self._CMRoot->_self_delete();
		
		self.~daeMetaElement();
	}
};
extern void daeMetaElement_self_destruct(daeMetaObject *self) 
{
	daeMeta::Subroutine::self_destruct(*static_cast<daeMetaElement*>(self));
}

daeElementRef daeMeta::createWRT(daePseudoElement2 pseudo2, const daePseudonym &pseudonym)const
{	
	//Can't assume pseudonym is permastring/0-terminated.
	daeName permanent = pseudonym; 

	//NEW XMLNS MACHINERY
	//TODO: Largely untested!!!
	//OPTIMIZATION? Since QNames are rare, a lot of this
	//code is designed to avoid looking for the : inside
	//the names. Likewise, it's not possible to pass the
	//position of the colon, except for daeCreateContext.
	daeMeta *ME;
	daeTags et;
	size_t colon;
	daeStringRef pf; //Initially empty.		
	daePseudoElement *parent;
	if(0==pseudo2.context->_zero) //Is daeCreateContext?
	{
		daeCreateContext &cc = *pseudo2.context;
		parent = cc.parent;
		 		
		//Callers can leave the prefix blank, or opt not
		//to strip it out of the QName.
		if(cc.prefix_string==nullptr)
		{
			//ORDER-IS-IMPORTANT
			cc.prefix_string = permanent.string;
			cc.prefix_extent = UINT_MAX&daeMetaElement_colon(permanent);
		}
		else if(cc.prefix_extent<permanent.extent
		&&':'==permanent.string[cc.prefix_extent])
		{
			permanent.string+=cc.prefix_extent+1;
			permanent.extent-=cc.prefix_extent+1;
		}
		//xmlns is a plugin/user-provided callback that will
		//return either et or xmlns. Whichever is convenient.
		daeName xmlns; *(unsigned int*)&et = cc.xmlns(xmlns);
		if(!et.found())
		{
			if(0==cc.prefix_extent)
			{
				//TODO? Would handle form="unqualified" here.
				//Note: This is looking up the default xmlns.
			}
			else pf.setString(cc.prefix_string,cc.prefix_extent);
			if(!xmlns.empty()) et.assign(et._make_base(xmlns),pf);
			else goto pfind; goto limit;
		}
	}
	else //Incorrect if child has own xmlns attribute/prefix.
	{
		parent = pseudo2.element;
		et = parent->getElementTags();
		if(1!=et.nameTag) //Is parent QNamed?
		{
			//If so, it seems better to go ahead and scan it.
			colon = daeMetaElement_colon(permanent);
			if(0==colon)
			{
				//TODO? Would handle form="unqualified" here.
				//Note: This is looking up the default xmlns.
				goto pfind;
			}
			else if(parent->getQNamePrefix()!=daeName(pseudonym,colon-1))
			{	
	colon:		pf.setString(pseudonym,colon-1);
	pfind:		et = parent->findNamespace_xmlns(pf);
	limit:		if(!et.found())
				{
					//NOTE: Includes overflowing 8-bit limit.

					/*Letting pass.. not sure if findNamespace_xmlns
					//is premature.					
					return nullptr;
					*/

					//Should get straightened out down below.
					et.clear();
				}
			}			
		}	
	}
	assert(et.found());
	
	//YUCK: Relying on _elem_names_cache
	//as its namespace may not be ME's if
	//it is imported via the xs::any class.
	ME = _findChild3(permanent); 
								   	
	if(permanent.extent!=pseudonym.extent) //Is nontrivial?
	{	
		assert(ME!=nullptr);

		if(0==permanent.extent) //Is domAny?
		{
			//TODO? If a name is empty ("") it will end up here.
			//I can't think of a rationale for how to handle it
			//either way. This is not the best API for making a
			//domAny. A branch for domAny sake isn't a big deal.
			assert(permanent.empty()&&ME==domAny::_master.meta); 

			//Must generate perma-string. extent is adjusted in
			//case pseudonym is prefixed.
			//NOTE: Going ahead and doing this, even though the
			//type is not yet fully determined.
			permanent.extent = pseudonym.extent;
			permanent.extent-=permanent.string-pseudonym.string;
			permanent.string = et._make_base(permanent);		
		}
		else if(permanent.string==pseudonym.string) //Is QName?
		{
			colon = permanent.extent;
			permanent.string = pseudonym.string+colon;
			permanent.extent = pseudonym.extent-colon;
			goto colon;
		}
		else assert(0); //A multi-colonized name?
	}
	else if(ME==nullptr)
	{
		//Reminder: Historically this API returns nullptr.
		//(It could otherwise create an "unnamed" domAny.)	
		return nullptr;
	}
	else if(!_elem_names_cache->hasNamespaceTag(et)) //_findChild3
	{
		//HACK: Going by 1-name-rule, create the element and let
		//the xmlns attribute be added for it when it's inserted.		
		et = _elem_names_cache->getLocalNameTags();
		//NOTE: Could default to the xs:element prefix, but then
		//it would be ambiguous what to do with a default prefix.		
		if(pf.empty())
		{
			//If a prefix is not provided, then try to find one?
			/*I don't know about this. It seems cleaner to reuse
			//a match upon insertion, and not create one when so.
			//If users want a prefix they can provide one. If an
			//xmlns is desired, they have the ability to do that.
			if(!parent->findQNamePrefix_xmlns(et))*/
			{
				et.nameTag = 1; 
			}
		}
		else et.prefix(pf);
	}	 	

	return Subroutine::createWRT<0,0>(ME,parent,et,permanent);	
}
daeElementRef daeMeta::createWRT(daePseudoElement *parent, daeTags et, const daeName &NCName)const
{	
	daeName permanent = NCName; 
	daeMeta *ME = _findChild3(permanent); 		
	if(ME==nullptr) return nullptr;

	if(permanent.empty()) //domAny?
	{
		permanent.extent = NCName.extent;
		permanent.string = et._make_base(permanent);
	}
	else if(!_elem_names_cache->hasNamespaceTag(et)) //_findChild3
	{
		/*This is an option, but it would differ from
		//its overload.
		if(!et.found()) 
		{
			et = _elem_names_cache->getLocalNameTags();
		}*/

		//REMINDER: This API is unforgiving. But it's
		//not strictly illegal to reassign namespaces.
		return nullptr;
	}

	return Subroutine::createWRT<0,0>(ME,parent,et,permanent);
}

daeError daeMeta::_placeWRT(daeElement &parent, daeElement &child)const
{
	_place_operation OK; 
	OK.prolog(*this,parent,child); if(!OK) return OK;
	daeError e = _CMEntree->_placeElement(OK); if(e<DAE_OK) return e;
	OK.default_insertion_point_to_back(); OK.epilog(child); return OK?e:OK;
}
daeError daeMeta::_placeWRT(daeElement &parent, daeContent &content)const
{
	_place_operation OK;
	OK.prolog(*this,parent,*content.getKnownChild()); if(!OK) return OK;
	daeError e = _CMEntree->_placeElement(OK); if(e<DAE_OK) return e;
	OK.default_insertion_point_to_back(); OK.epilog(content); return OK?e:OK;
}
daeError daeMeta::_placeBetweenWRT(daeElement &parent, void *a, daeElement &child, void *b)const
{
	_place_operation OK;
	OK.prolog(*this,parent,child); if(!OK) return OK;
	OK.uncursorize(a,b);
	daeError e = _CMEntree->_placeElementBetween(OK.a,OK,OK.b); if(e<DAE_OK) return e;
	OK.reach_insertion_point(); OK.epilog(child); return OK?e:OK;
}
daeError daeMeta::_placeBetweenWRT(daeElement &parent, void *a, daeContent &content, void *b)const
{
	_place_operation OK;
	OK.prolog(*this,parent,*content.getKnownChild()); if(!OK) return OK;							   
	OK.uncursorize(a,b);
	daeError e = _CMEntree->_placeElementBetween(OK.a,OK,OK.b); if(e<DAE_OK) return e;
	OK.reach_insertion_point(); OK.epilog(content); return OK?e:OK;
}
daeError daeMeta::_removeWRT(daeElement &parent, daeElement &child)const
{	
	assert(this==parent->getMeta()); //LEGACY	
	daeContents &rc = getContentsWRT(&parent);
	//TODO: check the cursor given a child-ID index.
	size_t found; if(!rc.find(&child,found))
	{
		//Historically this API has worked this way.
		return DAE_ERR_INVALID_CALL;
	}	
	return _removeWRT(parent,rc[found]);
}
daeError daeMeta::_removeWRT(daeElement &parent, daeContent &content)const
{
	assert(this==parent.getMeta()); //LEGACY
		 
	daeContents &c = getContentsWRT(&parent);
	daeCM_Demotion<> d(*this,c,content);

	//Passing placeholders through this API seems safe.
	//Still, it seems wrong to encourage this practice.
	if(nullptr!=content._child.ref)
	{
		d.count = c.__decrement_internal_counter(d.name).getIndex();
		c._remove_child(&content);
		d.maybe_demote();
	}
	else
	{
		//It must remain viable through daeNoteChange().
		daeElementRef RAII = content._child.ref;
		daeContents::_remove_operation op(d);
		//SOMEHOW DO THIS ALL AT ONCE
		if(Subroutine::typeLookup_unless(*content._child.ref))	
		op.typeLookup = true;
		daeNoteChange(op); 		
	}
	return d.maybe_demote_ORDER_IS_NOT_PRESERVED; 
}

void daeMeta::_place_operation::prolog(daeMeta &m, daeElement &p, daeElement &c)
{
	//WARNING: content IS INITIALIZED LAST THING.

	assert(m==p.getMeta()); //LEGACY
	assert(&p!=nullptr); if(&c==nullptr)
	{
		//There's not enough information to extract the parent
		//in this case, even if there is a content placeholder.
		error = DAE_ERR_INVALID_CALL; 
		return;
	}

	XS::Element *thread_safe_cache = m._elem_names_cache;
	//_elem_names_cache is set by _findChild2() via createWRT().
	//It may be counter-productive from a caching perspective, but
	//its purpose is to avoid doing a lookup for placeWRT(), which is
	//designed to follow createWRT().
	daeCTC<!c.__DAEP__Element__data._have_child_ID_onboard>();
	if(thread_safe_cache->getNCName().string!=c.getNCName().string)
	{	
		_elem_names_iterator it;	
		it = m._elem_names.find(c.getNCName());
		//IT'S NOT SAFE TO SEND A NAME TO daeCM THAT DOESN'T EXIST.
		//CHECKING getAllowsAny() MAY NOT BE THE BEST WAY TO DO IT.
		//IDEALLY 1 WILL ALWAYS HAVE AN OUTLET, BUT IT DOESN'T YET.
		//daeParentCM::_deepCM's COMMENTS HAVE SOME THOUGHTS ON IT.
		if(it==m._elem_names.end()) if(!m.getAllowsAny())
		{
			//NOTE: EVEN IF THE ABOVE RECOMMENDATIONS ARE REALIZED
			//THESE APIS HAVE HISTORICALLY ERRORED OUT IN THIS WAY.
			error = DAE_ERR_QUERY_SYNTAX; 
			//This is a good indicator that the element isn't from
			//createWRT() and requires it's name to be changed via
			//getNCName() to be accepted. This is programmer error.
			assert(it!=m._elem_names.end()); 
			return;
		}
		else name = 1; else name = it->second; 
	}
	else name = thread_safe_cache->getChildID().getName();
	//WARNING: content IS NOT YET INITIALIZED.
	new(this) daeCM_Placement<>(name,count,m.getContentsWRT(&p)); 
	count = content.__plural_or_non_size(name);
}
void daeMeta::_place_operation::uncursorize(void *after, void *before)
{
	daeCursor itt = content.end();
	daeCursor iit = content.data();	
	_a = content.__uncursorize(after); if(_a>=itt)
	{
		a = daeOrdinals; _a = iit-1;
	}
	else //Could ignore text if _a is a known to be a child.
	{	
		daeCursor rit = _a, ritext;
		ritext = (daeCursor)rit->getText(); 
		if(ritext!=nullptr)
		{
			//SHOULD THE TEXT BE SPLIT IN 2 IN THIS CASE???
			_a = ritext+ritext->getKnownText().span()-1;

			for(rit=ritext-1;rit->hasText()&&rit>iit;)
			rit-=rit->getKnownEndOfText().counterspan();
			a = rit>=iit?iit->_child.ordinal:daeOrdinals;
		}		
		else a = rit->_child.ordinal; 
	}
	_b = content.__uncursorize(before); if(_b>=itt) 
	{
		b = 0; _b = itt;
	}
	else //Could ignore text if _b is a known to be a child.
	{	
		daeCursor it = _b, itext;
		itext = (daeCursor)it->getText(); 
		if(itext!=nullptr)
		{	
			//SHOULD THE TEXT BE SPLIT IN 2 IN THIS CASE???
			_b = itext;

			it = itext+itext->getKnownStartOfText().span();
			while(it->hasText()) it+=it->getKnownStartOfText().span();
			assert(it<content.end()); 
		}		
		b = it->_child.ordinal; 
	}
}
void daeMeta::_place_operation::reach_insertion_point()
{
	daeCursor iit = _a+1; if(a!=ordinal)
	{
		daeCursor itt = _b; if(b!=ordinal)
		{
			daeCursor it = iit;
			while(it->hasText()) it+=it->getKnownStartOfText().span();
			if(ordinal>=it->_child.ordinal)
			{
				//THIS IS NOT NECESSARILY THE MOST EFFICIENT MOVE AT
				//THIS STAGE, BUT IT SHOULD WORK.
				while(ordinal<it->_magic_ordinal) 
				it++;
				insertion_point = it; assert(it<=itt);
			}
			else insertion_point = iit;
		}
		else insertion_point = itt;
	}
	else insertion_point = iit;
}
void daeMeta::_place_operation::epilog(daeElement &child)
{
	if(!child.isContent())
	{
		daeContents::_insert_operation op(*this,child);
		//SOMEHOW DO THIS ALL AT ONCE
		if(Subroutine::typeLookup_unless(child))	
		op.typeLookup = true;
		daeNoteChange(op); error = op;		
	}
	else //TODO: check the cursor given a child-ID index.
	{
		assert(nullptr!=child.getParentElement());
		daeElement &parent2 = (daeElement&)child.getParentObject();		
		daeContents &rc = parent2.getContents();		
		size_t found; if(!rc.find(&child,found))
		{
			//This shouldn't be. Release builds should assume success.
			assert(0); error = DAE_ERR_INVALID_CALL; return;
		}
		daeCM_Demotion<> d(parent2.getMeta(),rc,rc[found]);
		daeContents::_insert_move_operation op(d,*this);
		//SOMEHOW DO THIS ALL AT ONCE
		if(Subroutine::typeLookup_unless(child))	
		op.typeLookup = parent2.getDocument()!=content.getElement().getDocument();
		daeNoteChange(op); error = op;
	}
}
void daeMeta::_place_operation::epilog(daeContent &content)
{
	daeElement &child = *content.getKnownChild();
	assert(nullptr!=child.getParentElement());
	daeElement &parent2 = (daeElement&)child.getParentObject();		
	daeCM_Demotion<> d(parent2.getMeta(),parent2.getContents(),content);
	daeContents::_insert_move_operation op(d,*this);
	//SOMEHOW DO THIS ALL AT ONCE
	if(Subroutine::typeLookup_unless(child))	
	op.typeLookup = parent2.getDocument()!=content._child.ref->getDocument();
	daeNoteChange(op); error = op;
}

//This is neither of the create/placeWRT family nor of the "WID" family.
daeChildRef<> &daeMeta::pushBackWRT(daePseudoElement2 pseudo2, const daePseudonym &pseudonym)const
{
	//NOTE: Unlike with createWRT this version is forwarding to the
	//overload. It's just expedient, and would require a big review
	//of all of pushBackWRT's parts. 

	daeName NCName = pseudonym;

	daeTags et;
	size_t colon;
	daeStringRef pf; //Initially empty.		
	daePseudoElement *parent;
	if(0==pseudo2.context->_zero) //Is daeCreateContext?
	{
		daeCreateContext &cc = *pseudo2.context;
		parent = cc.parent;
		 		
		//Callers can leave the prefix blank, or opt not
		//to strip it out of the QName.
		if(cc.prefix_string==nullptr)
		{
			//ORDER-IS-IMPORTANT
			cc.prefix_string = NCName.string;
			cc.prefix_extent = UINT_MAX&daeMetaElement_colon(NCName);
		}
		else if(cc.prefix_extent<NCName.extent
		&&':'==NCName.string[cc.prefix_extent])
		{
			NCName.string+=cc.prefix_extent+1;
			NCName.extent-=cc.prefix_extent+1;
		}
		//xmlns is a plugin/user-provided callback that will
		//return either et or xmlns. Whichever is convenient.
		daeName xmlns; *(unsigned int*)&et = cc.xmlns(xmlns);
		if(!et.found())
		{
			if(0==cc.prefix_extent)
			{
				//TODO? Would handle form="unqualified" here.
				//Note: This is looking up the default xmlns.
			}
			else pf.setString(cc.prefix_string,cc.prefix_extent);
			if(!xmlns.empty()) et.assign(et._make_base(xmlns),pf);
			else goto pfind; 
		}
	}
	else //Incorrect if child has own xmlns attribute/prefix.
	{

//		HACK: Scanning for colon in name always because it's
//		just too complicated to implement a fallback on name
//		lookup at the moment.


		parent = pseudo2.element;
		et = parent->getElementTags();
//		if(1!=et.nameTag) //Is parent QNamed?
//		{
			//If so, it seems better to go ahead and scan it.
			colon = daeMetaElement_colon(NCName);
			if(0==colon)
			{
				//TODO? Would handle form="unqualified" here.
				//Note: This is looking up the default xmlns.
				//goto pfind;
if(1!=et.nameTag)
goto pfind;				
			}
			else 
if(1==et.nameTag
			||parent->getQNamePrefix()!=daeName(pseudonym,colon-1))
			{	
				pf.setString(pseudonym,colon-1);
	pfind:		et = parent->findNamespace_xmlns(pf);	
			}
//		}	
	}

	return pushBackWRT(parent,et,NCName);
}
daeChildRef<> &daeMeta::pushBackWRT
(daePseudoElement *parent, daeTags et, const daeName &NCName)const
{
	daeError e;
	daeContents &c = getContentsWRT(parent);
	daeCM_Placement<> p(c); 
	daeContent* &p_insertion_point = //nonconst
	const_cast<daeContent*&>(p.insertion_point);

	//Locate the back-most child whose 
	//ordinal is ordered: or is nonzero.
	//Starting at c.end() would be bad if
	//the array is significantly unordered.
	const daeContent *iit = c.begin();
	const daeContent *rit = c.cursor();
	assert(!rit->hasText());
	if(0==rit->_child.ordinal)
	{
		//This is not normal. 
		//It does occur when text-nodes are pushed-back.
		if(rit--!=iit)
		{
			//Rewind to the first non-0/ordered ordinal.
			while(0==rit->_child.ordinal&&rit>=iit)
			rit--;
			//Try to avoid this rewind next time around.
			c.cursor() = rit+1;
			if(c.cursor()->hasText())
			c.cursor()++;
		}
	}
	else //Normally elements will do this everytime.
	{
		//Normally this advances once testing twice.
		while(0!=rit[1]._child.ordinal) rit++;
	}
	const daeContent *back = rit+1;	

	daeMeta *ME; daeName permanent; 
	_elem_names_iterator jt;	
	XS::Element *thread_safe_cache = _elem_names_cache;
	//This might be helpful with reoccurrences???
	//(_elem_names_cache is not designed for this.)
	if(thread_safe_cache->getNCName()!=NCName)
	{	
		jt = _elem_names.find(NCName);
		if(jt!=_elem_names.end()) 
		{
			p.name = jt->second;			
			_elem_names_cache = 
			thread_safe_cache = _elem0+p.name;			
		}
		else 
		{
			//Note: This is just permitting passing 0 for et
			//to have a slightly different behavior.
			if(!et.found()) et.clear();
					 
			permanent = daeName(et._make_base(NCName),NCName.extent);
			unnamed: 
			ME = domAny::_master.meta;
			if(!getAllowsAny())
			{
				//NOTE: IF THIS ISN'T HANDLED LIKE THIS THEN 0s WILL
				//HAVE TO BE DEALT WITH AFTER _placeElementBetween().
				p.count = c.unnamed().size();
				goto unordered;
			}				
			p.name = 1; goto any; 
		}		
	}
	else p.name = thread_safe_cache->getChildID().getName();
	ME = &jumpIntoTOC(p.name).getChild();	
	if(!et.found()||!thread_safe_cache->hasNamespaceTag(et))
	{		  
		//Note: This is just permitting passing 0 for et
		//to have a slightly different behavior.
		if(et.found()
		||!parent->findQNamePrefix_xmlns(et=thread_safe_cache->getLocalNameTags()))			
		{
			goto unnamed;
		}
	}
	permanent = thread_safe_cache->getNCName();
	any:
	p.count = c.__plural_or_non_size(p.name);

	//Place after the last nonzero ordinal. Text-nodes may get in the way.
	while(rit->hasText()&&rit>iit) 
	rit-=rit->getKnownEndOfText().counterspan();
	e = _CMEntree->_placeElementBetween
	(rit<iit?daeOrdinals:rit->_child.ordinal,p,daeOrdinal0); 

	//If this is hit the below code needs to be revised if 0 is legitimate.
	assert(e<DAE_OK||0!=p.ordinal);

	if(e!=DAE_OK)
	{
		if(e==DAE_ORDER_IS_NOT_PRESERVED) //<xs:all>?
		{		
			size_t grow = back-rit; if(grow>1) //There's grow-1 text nodes?
			{
				//There is text in front of "back," so it must be moved
				//to the insertion point. This is to preserve <!--comments
				//mainly. It pulls in mixed-text & processing-instructions
				//also. This is the default behavior of <xs:all>.

				//REMINDER: a DAEP::Change(DAEP::CONTENT) is not being done.
				//DAE_ORDER_IS_NOT_PRESERVED is more of a technicality. If 
				//a schema needs to preserve order, then there needs to be 
				//a way to communicate that on an individual <xs:all> basis.

				#ifdef NDEBUG
				#error The below code has not been vetted.				
				#endif
				assert(0);

				c.grow(c.size()+grow); //It's the back-reserve that's grown.
				c._insert_child<+1>(back);
				void *swapbuf = c.end();
				size_t swapmem = sizeof(daeContent)*grow;
				size_t movemem = sizeof(daeContent)*c.size()-swapmem;				
				memcpy(swapbuf,&c[rit-iit+1],swapmem);
				//The child sits behind the text-nodes.
				p.insertion_point = &c[p.insertion_point-iit];
				memmove(p_insertion_point+grow,p.insertion_point,movemem);
				memcpy(p_insertion_point,swapbuf,swapmem);								
				//CONVENTION: keep the back-memory 0 filled so regular
				//high-frequency code doesn't have to.
				memset(swapbuf,0x00,swapmem);
				//By definition there must be a child. _insert_child(back)
				//should have set the cursor on back, and the child that is
				//causing this move should slide into the position of back.
				assert(c.cursor()->hasChild());
			}
			else c._insert_child<+1>(p.insertion_point);
		}
		else unordered:
		{
			//NEW: This forces the element into the extra-schema group.
			//FYI: The other APIs don't do this due to either legacy or 
			//stability requirements.
			p.name = 1; 
				
			p.ordinal = 0; goto unordered2;			
		}
	}
	else if(back==c.end()) 
	{
		c.cursor() = c.end(); unordered2:

		p.insertion_point = c._append_child<+1>();
	}
	else p.insertion_point = c._insert_child<+1>(back);

	c.__increment_internal_counter(p.name);
	new(p_insertion_point) daeContent(p.ordinal,daeChildID(p.name,p.count),nullptr);
	
	daeElement *child = Subroutine::createWRT<1,0>(ME,parent,et,permanent);	
							
	daeContents::_populate_new_operation op(*p_insertion_point,*child);
	//SOMEHOW DO THIS ALL AT ONCE
	if(Subroutine::typeLookup_unless(*child))
	op.typeLookup = true;
	daeNoteChange(op);

	return p_insertion_point->_child_ref();
}

namespace
{
	//This ended up getting less use than anticipated.
	struct daeMetaElement_unpack_WID
	{
		int name; daeCounter index;
		daeElement &parent; daeMeta &meta;
		daeMetaElement_unpack_WID(daeChildID ID, daeContents &c)
		:name(ID.getName()),index(ID.getIndex()),parent(c.getElement()),meta(parent.getMeta())
		{}
	};	
	template<class T> 
	struct daeMetaElement_setWID2_a //C++98/03-SUPPORT (C2918)
	{
		T _child; daeElement *child(){ return _child; }
		daeContent &content(daeContents&){ return _child; }
	};template<>
	daeContent &daeMetaElement_setWID2_a<daeElement*>::content(daeContents &c)
	{
		size_t found; if(DAE_OK!=c.find(_child,found)) assert(0);
		return c[found];
	}
}
daeChildRef<> &daeMeta::_addWID(daeChildID ID, daeContents &c, daeContent *it)
{
	daeMetaElement_unpack_WID up(ID,c);	
	daeTOC::Atom &el = up.meta.jumpIntoTOC(up.name);	
	daeElement *child = Subroutine::createWRT<1,1>
	(el.getChild(),up.parent,el.getLocalNameTags(),el.getNCName());
	
	if(it==nullptr)
	{
		//Under current guidelines it should be the case
		//that if it==nullptr then the index is the count.
		assert(up.index==c.__plural_or_non_size(up.name));
		daeCM_Placement<> p(up.name,up.index,c); 				
		if(DAE_OK>up.meta._CMEntree->_placeElement(p))
		{	
			//Failure is not an option.
			p.set_insertion_point_et_cetera_to_extra();
		}
		else p.default_insertion_point_to_back();
		//HACK/RAII: it is receiving p.insertion_point.
		{
			daeContents::_insert_new_operation op(p,*child);
			//SOMEHOW DO THIS ALL AT ONCE
			if(Subroutine::typeLookup_unless(*child))
			op.typeLookup = true;
			daeNoteChange(op);
		}
		it = const_cast<daeContent*>(p.insertion_point);
	}				
	else
	{
		daeContents::_populate_new_operation op(*it,*child);
		//SOMEHOW DO THIS ALL AT ONCE
		if(Subroutine::typeLookup_unless(*child))
		op.typeLookup = true;
		daeNoteChange(op);
	}
	assert(it->_child.ref->isContent()); return it->_child_ref();
}
template<class S> 
inline void daeMeta::_setWID3(daeChildID ID, S &source, daeContents &c, daeContent* &it)
{
	daeMetaElement_unpack_WID up(ID,c);
	daeElement *child = source.child();
	
	if(it==nullptr)
	{
		//Under current guidelines it should be the case
		//that if it==nullptr then the index is the count.
		assert(up.index==c.__plural_or_non_size(up.name));
		daeCM_Placement<> p(up.name,up.index,c); 	
		if(DAE_OK>up.meta._CMEntree->_placeElement(p))
		{	
			//Failure is not an option.
			p.set_insertion_point_et_cetera_to_extra();
		}
		else p.default_insertion_point_to_back();

		if(child==nullptr) //Placeholder?
		{
			c.__increment_internal_counter(up.name);
			new(c._insert_child<+1>(p.insertion_point)) daeContent(p.ordinal,ID,nullptr);
		}
		else 
		{
			const daeName &rename = 
			//HACK: child->name=child->name is easier than maintaining two paths.
			up.name==1?child->getNCName():up.meta.jumpIntoTOC(up.name).getNCName();

			if(child->isContent())
			{		
				//THIS IS THE EXACT SAME LOGIC AS BELOW.
				assert(nullptr!=child->getParentElement());
				daeElement &parent2 = (daeElement&)child->getParentObject();		
				daeContents &rc = parent2.getContents();
				daeContent &rm = source.content(rc);
				daeCM_Demotion<> d(parent2.getMeta(),rc,rm);				
				daeContents::_insert_move_rename_operation op(d,p,rename);
				//SOMEHOW DO THIS ALL AT ONCE
				if(Subroutine::typeLookup_unless(*child))
				op.typeLookup = parent2.getDocument()!=up.parent.getDocument();
				daeNoteChange(op); 
			}
			else
			{					
				daeContents::_insert_rename_operation op(p,*child,rename);
				//SOMEHOW DO THIS ALL AT ONCE
				if(Subroutine::typeLookup_unless(*child))
				op.typeLookup = true;
				daeNoteChange(op);
			}
		}
		it = const_cast<daeContent*>(p.insertion_point);
	}				
	else 
	{
		if(it->_child.ref!=nullptr) //Vacate?
		{
			//_populate_move_operation() can't 
			//handle self-reassignment. vacate()
			//releases its ref. This seems for the
			//best. Note, there is no requirement to
			//do with change-notices.
			if(child==it->_child.ref) //Assigning to self?
			{
				return; //NOP
			}

			//It must remain viable through daeNoteChange().
			daeElementRef RAII = it->_child.ref;

			#ifdef NDEBUG
			#error Need a mechanism for suppressing notifications.
			#endif
			bool notify2 = true;
			daeContents::_vacate_operation op(*it);
			//SOMEHOW DO THIS ALL AT ONCE			
			if(Subroutine::typeLookup_unless(*it->_child.ref))
			op.typeLookup = true;
			if(notify2) daeNoteChange(op);
		}

		if(child==nullptr) //Placeholder?
		{
			//There's nothing more to do.
		}
		else 
		{
			const daeName &rename = 
			//HACK: child->name=child->name is easier than maintaining two paths.			
			up.name==1?child->getNCName():up.meta.jumpIntoTOC(up.name).getNCName();

			if(child->isContent())
			{
				//THIS IS THE EXACT SAME LOGIC AS ABOVE.
				assert(nullptr!=child->getParentElement());
				daeElement &parent2 = (daeElement&)child->getParentObject();		
				daeContents &rc = parent2.getContents();
				daeContent &rm = source.content(rc);
				daeCM_Demotion<> d(parent2.getMeta(),rc,rm);
				daeContents::_populate_move_rename_operation op(d,*it,up.parent,rename);
				//SOMEHOW DO THIS ALL AT ONCE				
				if(Subroutine::typeLookup_unless(*child))
				op.typeLookup = parent2.getDocument()!=up.parent.getDocument();
				daeNoteChange(op); 
			}
			else 
			{
				daeContents::_populate_rename_operation op(*it,up.parent,*child,rename);
				//SOMEHOW DO THIS ALL AT ONCE
				if(Subroutine::typeLookup_unless(*child)) op.typeLookup = true;
				daeNoteChange(op);
			}
		}
	}
}
daeChildRef<> &daeMeta::_setWID2(daeChildID ID, daeElement *child, daeContents &c, daeContent *it)
{
	daeMetaElement_setWID2_a<daeElement*> //C++98/03 (C2918) 
	adapter = { child };
	_setWID3(ID,adapter,c,it); return it->_child_ref();
}
daeChildRef<> &daeMeta::_setWID2(daeChildID ID, daeContent &child, daeContents &c, daeContent *it)
{
	daeMetaElement_setWID2_a<daeContent&> //C++98/03 (C2918)
	adapter = { child };
	_setWID3(ID,adapter,c,it); return it->_child_ref();
}
void daeMeta::_removeWID(daeContents &c, daeContent *it)
{
	if(nullptr==it) return;

	daeCM_Demotion<> d(c.getElement().getMeta(),c,*it);

	if(nullptr==it->_child.ref)
	{	
		d.count = c.__decrement_internal_counter(d.name).getIndex();
		c._remove_child(it);
		d.maybe_demote();
	}
	else
	{
		//It must remain viable through daeNoteChange().
		daeElementRef RAII = it->_child.ref;
		daeContents::_remove_operation op(d); 
		//SOMEHOW DO THIS ALL AT ONCE
		if(Subroutine::typeLookup_unless(*it->_child.ref))
		op.typeLookup = true;
		daeNoteChange(op); 
	}	
}
void daeMeta::_resizeWID2(daeContents &c, daeChildID::POD ID, daeChildID::POD ID_size)
{
	//_setWID2 (in a loop) especially is not the most brilliant 
	//implementation, but it can suffice for the time being. If
	//the compiler optimizes aggressively, then it's not so bad
	//as long as the "loops" aren't great or it isn't a case of
	//an element with maxOccurs describing array-like semantics.
	while(ID<ID_size) _removeWID(c,c._get<int(0)>(ID++));
	while(ID_size<ID) _setWID2(ID_size++,nullptr,c,nullptr);
}

//---.
}//<-'

/*C1071*/
