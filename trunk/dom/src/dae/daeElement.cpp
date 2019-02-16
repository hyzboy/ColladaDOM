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

const_daeURIRef daeElement::getBaseURI()const
{
	const daeDocument *d = getDocument();
	COLLADA_ASSUME(_isElement())
	return d==nullptr?nullptr:d->getBaseURI(this);
}

extern daeAST::Data daeAA_void_terminator;
extern daeAlloc<XS::Attribute*,1> daeAA_any_thunk;

  //// daeAnyAttribute //// daeAnyAttribute //// daeAnyAttribute ////
  //// daeAnyAttribute //// daeAnyAttribute //// daeAnyAttribute ////
  //// daeAnyAttribute //// daeAnyAttribute //// daeAnyAttribute ////

inline daeAlloc<> *daeAnyAttribute::_head_AU()
{
	//Commenting, since a lot of this is counterintuitive.

	//If there are only static attributes there's not yet
	//a head of the AU linked-list.
	if((daeSize)_au->getOffset()+1==getCapacity())
	return nullptr;
	
	//Get the first nonstatic attribute?
	daeData **d = _au->_varray+_au->getOffset();
	//If first happens to be the VOID attribute, then the 
	//first is actually the next attribute.	
	//if(!hasAnyExtra()) d++; 
	if(*d==&daeAA_void_terminator)
	{
		assert(!hasAnyExtra()); d++;
	}
	else assert(hasAnyExtra());
	
	//This converts the first subscript to its AU's address.
	return (daeAlloc<>*)&daeAlloc<daeData>::dataptr_cast(*d);
}
inline daeAlloc<> *daeAnyAttribute::_next_AU(daeAlloc<> *node)
{
	//here _offset is repurposed as linked-list pointer.
	return (daeAlloc<>*)((daeAllocThunk*)node)->_offset;
}
inline void daeAnyAttribute::_llist_AU(daeAlloc<> *head, void *node)
{
	if(head==nullptr) return; //Nothing to link?
	daeOffset &next = ((daeAllocThunk*)head)->_offset;
	((daeAllocThunk*)node)->_offset = next; next = daeOffset(node);
}
void daeAnyAttribute::_grow(size_t to)
{
	const size_t cp = getCapacity(); 
	to++; assert(to>cp);

	//This is to squeeze Data objects into _grow(++i+6) in
	//_extern_push_back() starting with a domAny.
	if(to%2==1) to++;
	
	daeAlloc<> *headAU = _head_AU();
	daeElement &el = const_cast<daeElement&>(getElement());
	if(headAU!=nullptr)
	{
		//TODO? Might want to ensure to-cp is large enough 
		//to justify an allocation overhead.
		//(Reminder: these are extra/any attributes.)
	}
	else //Set the copy-on-write bit first time only.
	{
		(*(DAEP::Element*)&el) //FRIENDS
		.__DAEP__Element__data.daeAnyAttribute_copy_on_write = 1;
	}

	union //UNION
	{
		daeAlloc<daeData*> *prevAU;
		daeAlloc<daeAST::Data> *availAU;
	};prevAU = _au;

	daeDatabase &db = el.getDatabase();
	daeNullable<> na = {&el};
	na._reAlloc<1,1>(daeOpaque(&getAU()),to,nullptr,&db);
	daeData **va = _au->_varray;
	memcpy(va,prevAU->_varray,cp*sizeof(daeData*));

	//Note, the current AU is not part of this linked-list.
	//The first AU will not have a head, and so the metadata
	//thunk will not end up in this list.
	_llist_AU(headAU,prevAU);

	//NOTE: When this was designed Data was much more svelte.
	//Now the ratio is way off. domAny (having 0 attributes) 
	//doesn't do so well, however an element with attributes
	//will fair somewhat better.
	//If the subscripts were a little larger the ratio would
	//be better. Data can do without the flags to be smaller
	//but 32-bit builds wouldn't change because of alignment.
	size_t extra = to-cp;
	size_t avail = cp*sizeof(void*)/sizeof(daeAST::Data);	
	daeCTC<sizeof(daeAST::Data)%sizeof(void*)==0>();
	if(extra>avail)
	{
		size_t dummyN = extra-avail;
		daeAlloc<daeAST::Data,0> dummy;
		daeAlloc<daeAST::Data> *dummyAU = dummy.unit();
		na._reAlloc<1,1>(daeOpaque(&dummyAU),dummyN,nullptr,&db);

		_llist_AU(headAU,dummyAU);				

		for(size_t i=cp+avail,j=0;j<dummyN;)
		va[i++] = dummyAU->_varray+j++;
	}
	else //In this case there may be excess...
	{
		//TODO? Increase the new capacity to make the most of
		//this? Or setInternalCounter so to try to reuse next.
		avail = extra;
	}
	for(size_t i=cp,j=0;j<avail;)
	va[i++] = availAU->_varray+j++;

	//Bulk initialize strings to owner DOM's empty string-ref.
	//daeString e = el.getDOM()->getEmptyURI().data();
	for(size_t i=cp;i<to;i++)
	new(va[i]) daeAST::Data(&el/*,e*/);
}

inline void daeAnyAttribute::_extern_free(const void *metaAU)
{
	if(_au==metaAU) return

	//Assuming ~daeAnyAttribute() in the second condition.
	//(Leaving open the possiblity of resetting the array.)
	assert(metaAU!=nullptr||hasAnyExtra());
	
	for(size_t i=_au->getOffset();i<getCapacity();i++)
	{
		if(i==size())
		assert(_au->_varray[i]==&daeAA_void_terminator);
		else
		((daeAST::Data*)_au->_varray[i])->~Data();
	}

	daeAlloc<> *q,*p = _head_AU(); if(p!=nullptr)
	{
		daeElement &el = const_cast<daeElement&>(getElement());
		daeDatabase &db = el.getDatabase();

		do{ q = _next_AU(p); db._delete(*p,el); p = q; }
		while(p!=nullptr);
	
		//The current daeData* AU is not included in the linked-list
		//since that avoids special handling of the metadata's thunk
		//and in theory ~daeAnyAttribute() should dispose of it. 
		#ifdef NDEBUG
		#error What about DAEP::Value<daeContents> for that matter?
		#endif
		db._delete(*_au,el); 
	}							   
	(const void*&)_au = metaAU;			
}

  //// daeElement //// daeElement //// daeElement //// daeElement ////
  //// daeElement //// daeElement //// daeElement //// daeElement ////
  //// daeElement //// daeElement //// daeElement //// daeElement ////


//daeAnySimpleType.cpp
COLLADA_(const,extern)(daeAllocThunk daeElement_thunk0) = 0;

void daeElement::__0()
{
	//__DAEP__Object__0 did this, but it seems
	//like it atomizes objects twice except in
	//unusual circumstances.	
	//__atomize2(getMeta());
	
	//The containers destructors use COLLADA_ASSUME.
	//Assuming databases have no use for attributes
	//prior to deletions.	
	//TODO? C++11 can delete the default destructor.
	{	
		daeAA &aa = getAttributes();
		if(aa.hasAnyExtra()) aa._extern_free(nullptr);
		*(const daeAllocThunk**)&aa = &daeElement_thunk0;
	}
	//Trying to do likewise for daeContents_size<0>.
	{	  
		daeContents &c = getContents();
		if(c.getCapacity()>1)
		getDatabase()._delete(*c.getAU(),*this);
		(const daeAllocThunk*&)c.getAU() = &daeElement_thunk0;
	}
}

void DAEP::Element::__COLLADA__atomize()
{	
	daeElement &_this = dae(*this);
	const daeDocument *doc = _this.getDocument(); 
	daeMeta &meta = _this.getMeta();			
	_this.__clear2(meta.getContentsWRT(&_this),meta,doc);
	_this.__atomize2(meta);	
}
void daeElement::__atomize2(daeMeta &meta)
{
	//If this is called, it should follow the element has been removed.
	assert(!isContent());		
	assert(!meta.hasObjectsEmbedded()); //__DAEP__Object__0 assumes so.
	if(!meta.hasFeaturesWithAtomize())
	return;
	DAEP::Model &model = meta.getModel();
	for(daeFeatureID f=meta._feature_completeID;f!=0;f++) model[f].atomizeWRT(this);	
}
void daeElement::__clear2(daeContents &content, daeMeta &meta, const daeDocument *doc)
{	
	if(content.empty()) return;

	//NOTE: This kind of feels like a "clearContentsWRT" API, but it's 
	//implemented as a daeElement method because the contents-array is
	//used to call it, or can be, and so it isn't quite the same thing.

	//This is implementing daeArray<daeContent>::clear().
	for(size_t i=0,iN=content.size();i<iN;i++) 
	{
		daeElement *ch = content[i].getChild();		
		if(nullptr==ch) 
		{
			if(content[i].hasText())
			i+=content[i].getKnownStartOfText().span()-1;
		}
		else //ORDER IS IMPORTANT HERE.
		{
			//ALGORITHM
			//FIRST __clear2() recurses through the descendants,
			//-so that they're removed at the leaves. This keeps
			//the database from having to traverse the graph for
			//every removal.
			//Then, when there are no more grandchildren, replace
			//the child with a nullptr placeholder so that the
			//cursor-stop indices don't have to be managed as the
			//contents-array is being taken down.
			//Lastly, child removed, __atomize2() tears down any
			//types among the attributes that might include refs.
			//That must be done, so that the destructor can ever
			//be called. (Because of the ref-counting, destruction
			//is always a two-pass process. That's what "atomize"
			//means: the first pass of destruction.

			daeMeta &meta = ch->getMeta(); //SHADOWING
			ch->__clear2(meta.getContentsWRT(ch),meta,doc);

			//The element must survive its vacation.
			daeElementRef compact = ch;
			{
				//_vacate_operation does this first.
				//It's done here because the document
				//is on hand, so no need to look it up.
				if(meta._daeDocument_typeLookup_enabled) //LEGACY
				{
					#ifdef NDEBUG
					#error Maybe try to bulk erase reoccurrences?
					#endif
					if(doc!=nullptr) doc->_typeLookup_self_remove(meta,*ch);
				}	

				#ifdef NDEBUG
				#error Need a mechanism for suppressing notifications.
				#endif
				bool notify = true;
				daeContents_base::_vacate_operation op(content[i]);
				if(notify) daeNoteChange(op);				
			}
			//OPTIMIZATION
			//The cryptic/hard to search for compacting operator~()
			//returns true if the object is deleted. If it is being
			//deleted then __atomize2() will have already been done.
			if(!~compact) ch->__atomize2(meta);
		}
	}
	//This resets all of the internal counters to 0.
	meta._clearTOC(this); assert(content._==content.data());

	//NEW: Pursuing a strategy where the back is all 0s.
	//(So that increasing the size needn't 0 terminate.)
	size_t zero_fill = content.size()*sizeof(daeContent);	
	//Zero/set the special end-pointer to the beginning.
	memset(content.begin(),0x00,zero_fill);
	content.getAU()->setInternalCounter(0);
}	

void daeElement::__grow(daeArray<daeContent> &contents_of_this, size_t minCapacity)
{
	daeContents &c = static_cast<daeContents&>(contents_of_this);			
	//NEW: This is the real reason this is implemented apart from
	//daeArray.h & daeContent.h. Although getDatabase() is a plus.
	daeOffset _ = c.cursor()-c.data();
	//This should be the same logic as daeArray::_grow3(). +1 for 0-terminator.
	size_t newT = minCapacity<4?4:std::max(c.getCapacity()*2,minCapacity+1);
	//NEW: This is repurposing bits and pieces of code from all over to
	//make this simpler to write than it has any right to be.
	{
		//This sets a hint to traverse the element's graph when it's migrated.
		//It's for keeping things like "id" and "sid" lookup tables up-to-date.
		__DAEP__Element__data.daeContents_capacity_is_nonzero = 1;	
		//This reAlloc() was added for loaders that tally the number of spaces
		//in a list, and so can reserve memory in order to avoid reallocations.
		daeNullable<> na = {this};
		na.reAlloc(getDatabase(),c.getAU(),newT,c._element_less_mover);
		//This makes it so insertion code doesn't generate 0-termination codes.
		memset(c.end(),0x00,sizeof(daeContent)*(c.getCapacity()-c.size()));		
	}
	//Finally repair the pointer-based cursor. (Previously daeCursor
	//was an offset, and so repair had not been necessary.)
	c.cursor() = c.data()+_;
}
	//TODO: It seems like daeAnySimpleType_xmlns should have its own dedicated
	//daeAtomicType::XMLNS in order to avoid the string table confusion and to
	//not have to lookup the string in the first place.

extern size_t daeStringRef_namespaceTag(daeStringRef_base); //emits error	
extern size_t daeStringRef_nameTag(size_t,daeStringRef_base); //emits error	
void daeTags::assign(daeStringRef_base ns, daeStringRef_base pf)
{	 
	//YUCK: memset overwrites namespaceTag...
	size_t nt = namespaceTag; memset(this,0x00,sizeof(*this));
	//HACK: Needs work... assuming nameTag less than 256.
	if(nullptr==ns) goto nt; //dae_append?
	else nt = daeStringRef_namespaceTag(ns); if(0!=nt||ns.empty()) //domAny?
	{
		namespaceTag = 0xFF&nt; nt:
		nameTag = nullptr==pf?1:0xFF&daeStringRef_nameTag(nt,pf); //dae_default?
	}
}

static bool daeElement_xmlns2
(const daeElement *p,  daeData *d, daeStringRef &ns)
{
	if(d->isValue()) //<xs:appinfo>enable-xmlns</xs:appinfo>
	{
		daeAnyAttribute &aa = p->getAttributes();

		//XMLNS treats "" as a special "empty" namespace.
		if(!aa.mask().bit(0).get())
		{	
			assert(d==*aa.cbegin());

			return false;
		}

		//HACK: Opportunistically convert to system-pool string.
		if(0!=ns.pool()) ns.setRef(daeStringRef(ns.value()));
	}
	else if(ns!=nullptr) //VOID?
	{
		assert(0==ns.pool());
	}
	else return false; return true;
}
static bool daeElement_xmlns
(const daeElement *p, daeData *d, daeStringRef_base pf, daeTags &et)
{
	if(!d->getIsXmlns()||pf!=d->getNCName()) return false;

	daeStringRef &ns = d->getWRT(const_cast<daeElement*>(p)); 
	
	if(!daeElement_xmlns2(p,d,ns)) return false;

	et.assign(ns,pf); return true; //return et.found();
}
daeTags daeElement::findNamespace_xmlns(const daeStringRef &prefix)const
{	
	daeStringRef_base pf = //HACK: Ensure prefix is system-pool.
	0==prefix.pool()?prefix:daeStringRef(prefix.data(),prefix.size());
	daeTags et;
	for(const daeElement *p=this;p!=nullptr;p=p->getParentElement())
	{
		et = p->getElementTags();
		if(pf==et.second()) goto et;	
		else if(p->hasAttribute_xmlns())
		{
			daeAnyAttribute &a = p->getAttributes();

			//<xs:appinfo>enable-xmlns</xs:appinfo>
			//TODO: This practice is peculiar to the COLLADA schema.
			//It should probably be disabled so it's unnecessary to
			//do this special test.
			if(pf.data()==daeStringRef_empty
			&&daeElement_xmlns(p,a[0],pf,et)) goto et;

			//Static attributes should not include xmlns attributes.
			daePartition aep = a.getAnyExtraPart();
			while(aep.first<aep.second)
			if(daeElement_xmlns(p,a[aep.first++],pf,et)) goto et;		
		}
	}
	et.nameTag = 0; et: return et;
}

template<bool resolving>
static bool daeElement_findQNamePrefix_xmlns(const daeObject* &pIn, daeTags &et)
{
	const daeElement *p = (const daeElement*)pIn;

	daeTags cp = et;
	daeStringRef_base ns = et.first();
	if(resolving)
	goto resolving; for(;;)
	{	
		if(p->hasNamespaceTag(cp))
		{
			et.nameTag = p->getElementTags().nameTag;
			return true;
		}

		resolving:

		if(p->hasAttribute_xmlns())
		{
			daeAnyAttribute &a = p->getAttributes();

			//Static attributes should not include xmlns attributes.
			daePartition aep = a.getAnyExtraPart();

			//<xs:appinfo>enable-xmlns</xs:appinfo>
			//TODO: This practice is peculiar to the COLLADA schema.
			//It should probably be disabled so it's unnecessary to
			//do this special test.
			size_t i = 0; goto enable_xmlns;
			for(;i<aep.second;i=aep.first++) enable_xmlns:
			{
				//Similar to daeElement_xmlns.
				daeData *d = a[i]; 
				if(!d->getIsXmlns()) continue;
				//SCHEDULED FOR REMOVAL
				//Planning to switch daeAnySimpleType_xmlns over to 
				//a daeElementTags atomic-type.
				daeStringRef &ns2 = d->getWRT(const_cast<daeElement*>(p)); 
				if(daeElement_xmlns2(p,d,ns2)&&ns==ns2)
				{
					et.assign(dae_append,daeStringRef(d->getNCName()));
					return true;
				}
			}
		}

		if(resolving)
		{
			if(!p->isContent()) break;
		}

		(const daeObject*&)p = &p->getParentObject();
		if(p->_isDoc_or_DOM()) break;
	}	
	if(resolving) pIn = p; return false;
}
bool daeElement::findQNamePrefix_xmlns(daeTags &et)const
{
	const daeObject *p = this; 
	return daeElement_findQNamePrefix_xmlns<false>(p,et); 	
}

bool daeElement::_migrateNamespace2()const
{		
	union //HEADS-UP
	{
		const daeDocument *d; const daeObject *o; 
	};

	const daePseudoElement *pe = 
	(const daePseudoElement*)&getParentObject();	
	daeTags &et = *(daeTags*)&_getObjectTags();
	if(pe->hasNamespaceTag(et)&&pe->_isElement())
	{
		et.nameTag = pe->getElementTags().nameTag;
		return false;
	}
	else if(!daeElement_findQNamePrefix_xmlns<true>(o=this,et))		
	{
		if(!o->_isDoc()) 
		{
			if(o==this) pe = nullptr; o = nullptr;
		}
		else if(pe==o) 
		{
			//HACK: This is testing for inserting an
			//"empty namespace" element into the top
			//of the document. It's easier to do the 
			//check here, and seems best to not have
			//an unnecessary empty xmlns declaration.
			if(et.empty()) return false;

			pe = &d->getPseudoElement();
		}
	}
	else return false;

	//NOTE: getProvisionalDocument here would change the 
	//document out from under the database change-notice.

	//daePlatform::introduceToDocument
	if(d!=nullptr) 
	{
		const XS::Schema *x = &getMeta().getSchema();
		d->_get_and_or_use_xs_schema(et,x);
	}

	//daePlatform::resolveXMLNS	
	daeElement &_this = *const_cast<daeElement*>(this);
	if(!getDOM()->getPlatform().resolveXMLNS(d,pe,_this))
	{
		daeTags xmlns = getElementTags();
		_this.setAttribute_xmlns(xmlns.second(),xmlns.first());
	}

	return true;
}
daeOK daeElement::setAttribute_xmlns(const daeName &prefix, const daeName &value)
{
	daeStringRef_base v = dae_default; //dae_clear;
	if(value.string!=nullptr) v = daeTags::_make_base(value);
	return setAttribute_xmlns(daeTags::_make_base(prefix),v);
}
daeOK daeElement::setAttribute_xmlns(daeStringRef_base prefix, daeStringRef_base value)
{
	//Static attributes should not include xmlns attributes.
	daeAnyAttribute &a = getAttributes();
	daePartition aep = a.getAnyExtraPart();
	if(hasAttribute_xmlns())
	{	
		//<xs:appinfo>enable-xmlns</xs:appinfo>
		//TODO: This practice is peculiar to the COLLADA schema.
		//It should probably be disabled so it's unnecessary to
		//do this special test.
		size_t i = 0; goto enable_xmlns;
		for(;i<aep.second;i=aep.first++) enable_xmlns:
		{
			//Similar to daeElement_xmlns.
			daeData *d = a[i]; 
			if(!d->getIsXmlns()||d->getName()!=prefix)
			continue;
			
			daeElementRef test;
			d->getKnownAnyExtra(test);
			#ifdef NDEBUG
			#error Implement daeAtom::XMLNS here.
			#endif
			if(value==nullptr) removeAttribute(i);
			if(value!=nullptr) setAttribute(i,value);

			return DAE_OK;
		}
	}
	if(value==nullptr) return DAE_OK;

	daeCharData cd = "xmlns";
	if(!prefix.empty()) cd.push_back(':'); 
	if(!prefix.empty()) cd.append(prefix); 
	
	a._extern_push_back(aep.second,cd);
	return setAttribute(aep.second,value);
}

const daeDocument &daeElement::_provisionalDocument()const
{
	daeDocument *d;
	if(1==__DAEP__Element__data.is_document_daePseudoElement)
	{
		d = (daeDocument*)&getParentObject();
		assert(this==&d->getPseudoElement());
	}
	else 
	{
		daeDOM *DOM = const_cast<daeDOM*>(getDOM());
		d = DOM->_addDocument(DOM);
		d->getRoot() = const_cast<daeElement*>(this);
	}	
	return *d;
}

daeElement *daeElement::add(daeString list_of_names)
{
	daeMeta *meta = getMeta();
	daeElement *root = nullptr, *out = nullptr;
	for(daeString p=list_of_names,q=p;*q!='\0';p=q+1)
	{
		while(*q!='\0'&&*q!=' ') q++;
		out = add(meta->createWRT(*this,daeHashString(p,q-p)));		
		if(out==nullptr) break;
		if(root==nullptr) root = out;
	}
	if(out==nullptr&&root!=nullptr) removeChildElement(root); return out;
}

void daeElement::clone_Suffix::_suffix(daeElement *clone)
{		
	#define _(x) if(x!=nullptr){\
	if(!clone->getAttribute(#x,*this).empty())\
	clone->setAttribute(#x,append_and_0_terminate(x,x.extent)); }
	_(id)_(name)
	#undef _
}
void daeElement::_clone2(daeElement &ret, clone_Suffix *suffix)const
{
	//NOTE: setNamespace() works too, but this has simpler assignment.
	ret._getObjectTags() = daeObjectTags(getElementTags());
	ret.getNCName() = getNCName();
	
	//NOTE: Copying attributes this way is suboptimal for equal types.
	//ret.getAttributes() = getAttributes();	
	ret.getAttributes()._assign<true,true>(getAttributes());
	//NOTE: Assigning id/name twice.
	if(suffix!=nullptr) suffix->_suffix(&ret); 
	//NOTE: Check is unnecessary, but probably beats blind assignment.
	if(hasCharData()) getCharData().getKnownValue().copyWRT(&ret,this);

	//HACK: This is as close to a copy implementation that is on hand.
	const daeContents &src = getContents(); if(!src.empty())
	{	 
		getMeta()._cloneTOC(&ret,this);
		daeContents &dst = ret.getContents();
		dst.grow(src.getCapacity()); 
		dst.getAU()->setInternalCounter(src.size());
		//NOTE: If I recall, the extra capacity is expected to be 0-filled.
		memcpy(dst.data(),src.data(),src.getCapacity()*sizeof(daeContent));	
		for(size_t i=0,iN=dst.size();i<iN;i++)
		{
			daeElement *cp = dst[i].getChild();
			if(cp!=nullptr)
			{
				cp = *new(&dst[i]._child.ref) daeElementRef(cp->clone(&ret,suffix));
				cp->__DAEP__Element__data.is_content = 1;
			}
			else if(dst[i].hasText())
			{
				i+=dst[i].getKnownStartOfText().span()-1;
			}
		}
		ret.__DAEP__Element__data.daeContents_capacity_is_nonzero = 1;
	}
}
template<class T> //daeDOM/Element
static daeElement *daeElement_clone
(T *DOM, daeElement::clone_Suffix *suffix, const daeElement &_this)
{	
	//TODO? _addElement uses the prototype... if it were possible to
	//use _this instead, _clone2 would have less work to do... still
	//the code would be trickier, because it will create a minefield.
	daeElement *ret = DOM->_addElement(_this.getMeta()); 	

	//TODO? Determine/document if change-notices are not required in
	//this case. isContent() will be false for the top element. That
	//means it can't be inside a document. In which case, changes to
	//its tree can't be very consequential.
	_this._clone2(*ret,suffix); return ret;
}
daeElementRef daeElement::clone(daeElement *DOM, clone_Suffix *suffix)const
{		
	return daeElement_clone(DOM,suffix,*this);
}
daeElementRef daeElement::clone(daeDOM *DOM, clone_Suffix *suffix)const
{		
	return daeElement_clone(DOM,suffix,*this);
}

static std::ostream &formatColumns(std::ostream &os, daeString c1, daeString c2)
{
	if(c1==nullptr) c1 = empty_daeString1; 
	if(c2==nullptr) c2 = empty_daeString1;
	int w = (int)os.width();
c2:	int n = (int)strnlen(c1,w);
	if(n>w) //append ellipses
	{
		os.width(0);
		w-=3;
		for(int i=0;i<w;i++) os.put(*c1++);		
		os<<"...";
	}
	else os << c1; 
	if(c2!=nullptr){ c1 = c2; c2 = nullptr; goto c2; }	
	return os;
}
daeArray<> &daeElement::compare_Result::format(daeArray<> &out)
{
	out.clear_and_0_terminate();
	if(elt1==nullptr||elt2==nullptr) return out;
	daeArray<daeStringCP,256> tmp1,tmp2;
	daeOStringBuf<1024> buf(out);	
	daeString ns1 = elt1->getNamespace();
	daeString ns2 = elt2->getNamespace();
	if(ns1==ns2) ns1 = ""; 
	std::ostream msg(&buf); msg 
	<<std::setw(13)<<std::left<<"Namespace(s)"<<std::endl
	<<std::setw(50*2)<<std::left<<ns1<<std::endl	
	<<std::setw(50*2)<<std::left<<ns2<<std::endl	
	<<std::setw(13)<<std::left<<""
	<<std::setw(50)<<std::left<<"Element 1"<<"Element 2"<<std::endl
	<<std::setw(13)<<std::left<<""
	<<std::setw(50)<<std::left<<"---------"<<"---------"<<std::endl
	<<std::setw(13)<<std::left<<"Name"
	<<std::setw(50)<<std::left,formatColumns(msg,elt1->getNCName(),elt2->getNCName())<<std::endl
	<<std::setw(13)<<std::left<<"Type"
	<<std::setw(50)<<std::left,formatColumns(msg,elt1->getTypeName(),elt2->getTypeName())<<std::endl
	<<std::setw(13)<<std::left<<"id"
	<<std::setw(50)<<std::left,formatColumns(msg,elt1->getID_id(),elt2->getID_id())<<std::endl
	<<std::setw(13)<<std::left<<"Attr name"
	<<std::setw(50)<<std::left,formatColumns(msg,attrMismatch,attrMismatch)<<std::endl
	<<std::setw(13)<<std::left<<"Attr value"
	<<std::setw(50)<<std::left,formatColumns
	(msg,elt1->getAttribute(attrMismatch,tmp1).data()
	,elt2->getAttribute(attrMismatch,tmp2).data())<<std::endl
	<<std::setw(13)<<std::left<<"Char data"
	<<std::setw(50)<<std::left,formatColumns
	(msg,elt1->getCharData(tmp1).data(),elt2->getCharData(tmp2).data())<<std::endl
	<<std::setw(13)<<std::left<<"Child count"
	<<std::setw(50)<<std::left<<elt1->getChildrenCount()<<elt2->getChildrenCount();
	return out;
}

struct daeElement_compare : daeElement::compare_Result //LEGACY/REFACTOR
{
	daeArray<daeStringCP,COLLADA_MAX_PATH> value1,value2;
		
	daeElement_compare()
	{
		memset(this,0x00,sizeof(compare_Result)); attrMismatch.string = empty_daeString1; 
	}

	void compareMatch()
	{
		assert(compareValue==0); //compareValue = 0; 
	}
	void nameMismatched(const daeElement &e1, const daeElement &e2)
	{
		elt1 = &e1; elt2 = &e2;
		compareValue = strcmp(e1.getNCName(),e2.getNCName());
		nameMismatch = true;
	}
	void attrMismatched(const daeElement &e1, const daeElement &e2, const daeName &name)
	{
		elt1 = &e1; elt2 = &e2;
		compareValue = strcmp(e1.glimpse(name).data(),e2.glimpse(name).data());
		attrMismatch = name; 
	}
	void charDataMismatched(const daeElement &e1, const daeElement &e2)
	{
		elt1 = &e1; elt2 = &e2;
		compareValue = strcmp(e1.glimpse().data(),e2.glimpse().data());
		charDataMismatch = true;
	}
	void childCountMismatched(size_t a, size_t b, const daeElement &e1, const daeElement &e2)
	{
		elt1 = &e1; elt2 = &e2;		
		compareValue = (int)a-(int)b;
		childCountMismatch = true; 
	}
	
	void compareAttributes(const daeElement &e1, const daeElement &e2)
	{
		//Compare attributes. Be careful because each element could have a
		//different number of attributes.		
		if(e1.getAttributesCount()>e2.getAttributesCount())
		return attrMismatched(e1,e2,e1.getAttributeName(e2.getAttributesCount()));
		if(e2.getAttributesCount()>e1.getAttributesCount())
		return attrMismatched(e1,e2,e2.getAttributeName(e1.getAttributesCount()));
		size_t i = 0, iN = e1.getAttributesCount();
		if(e1.getMeta()==e2.getMeta())
		i = compareAttributesSameType(e1,e2);	
		if(0==compareValue) 
		compareAttributesDifferentTypes(i,iN,e1,e2);
	}
	size_t compareAttributesSameType(const daeElement &e1, const daeElement &e2)
	{
		//Compare attributes 
		daeAnyAttribute &attribs = e1.getAttributes();
		size_t i,iN = attribs.getAnyExtraPart().first;
		for(i=0;i<iN;i++)
		if(0!=attribs[i]->getKnownValue().compareWRT(&e1,&e2))
		{
			attrMismatched(e1,e2,attribs[i]->getName());
			break;
		}
		return i;
	}
	void compareAttributesDifferentTypes(size_t i, size_t iN, const daeElement &e1, const daeElement &e2)
	{	
		for(;i<iN;i++)
		{
			e1.getAttribute(i,value1);
			e2.getAttribute(e1.getAttributeName(i),value2);
			if(value1!=value2)
			return attrMismatched(e1,e2,e1.getAttributeName(i));
		}
	}

	void compareWithFullResult(const daeElement &e1, const daeElement &e2)
	{
		//Check the element name
		if(e1.getNCName()!=e2.getNCName()
		||!e1.hasNamespaceTag(e2.getElementTags()))
		return nameMismatched(e1,e2);

		//Compare character data
		bool same = e1.getMeta()==e2.getMeta();
		if(same&&0!=e1.getCharData().getKnownValue().compareWRT(&e1,&e2)
		||!same&&e1.getCharData(value1)!=e2.getCharData(value2))
		return charDataMismatched(e1,e2);

		//Compare attributes
		compareAttributes(e1,e2);
		if(0!=compareValue) return;

		//RECURSIVE
		#ifdef NDEBUG
		#error try to iterate over contents-arrays, unbuffered
		#endif		
		compareElementsChildren(e1,e2); 
	}
	void compareElementsChildren(const daeElement &e1, const daeElement &e2)
	{
		//RECURSIVE
		#ifdef NDEBUG
		#error try to iterate over contents-arrays, unbuffered
		#endif		
		daeArray<const_daeElementRef,64> children1, children2;

		if(e1.getChildren(children1).size()
		 !=e2.getChildren(children2).size())
		return childCountMismatched(children1.size(),children2.size(),e1,e2);
		for(size_t i=0;i<children1.size();i++)
		{
			compareWithFullResult(*children1[i],*children2[i]);
			if(0!=compareValue) return;
		}

		return compareMatch();
	}
};
int daeElement::compare(const daeElement &elt1, const daeElement &elt2, compare_Result *ret)
{
	daeElement_compare cmp; cmp.compareWithFullResult(elt1,elt2); 	

	if(ret!=nullptr) *ret = cmp; return cmp.compareValue;
}
 
daeElement *daeElement::getChild(const matchElement &matcher)
{
	daeContents &ca = getContents();
	daeContents::iterator it = ca.begin(), itt = ca.end(); 
	while(it!=itt) if(it->hasChild())
	{
		if(*it!=nullptr&&matcher(*it)) return it->_child.ref;
		else it++;
	}
	else it+=it->getKnownStartOfText().span(); return nullptr;
}

daeElementRef daeElement::getDescendant(const matchElement &matcher)
{
	//daeContents::for_each_child() isn't up to this.
	//And even if it could, the refs/locks are multi-level.
	daeArray<daeElementRef,64> elts; getChildren(elts);
	for(size_t j,i=0;i<elts.size();i++)
	{
		//NEW: Process one level before adding the next to
		//the array, to not add to the array unnecessarily.
		for(j=i;j<elts.size();j++)
		{
			//Check the current element for a match.
			if(matcher(*elts[j])) return elts[j];
		}

		//Append the element's children to the queue.
		while(i<j) elts[i++]->getChildren(elts);
	}		  
	return nullptr;
}

const_daeElementRef daeElement::getAncestor(const matchElement &matcher)const
{
	const_daeElementRef elt = getParent(); while(elt!=nullptr)
	{
		if(matcher(*elt)) return elt; elt = elt->getParent();
	}
	return nullptr;
}

void daeElement::_sidLookup(daeString ref, daeElementRef &match, const daeDoc *docIn)const
{
	const daeStringRef &known_ref = *(daeStringRef*)&ref;

	//This is copy/pasted from daeSIDResolver.cpp.
	//This is designed to get constructions of the type
	//<rotate sid="rotateX"> where there may be several
	//SIDs to eliminate by the regular bottom-up search.
	if(30<known_ref.getSID_frequency())
	{
		daeElement::matchSID matcher(known_ref);
		const daeElement *e = getChild(matcher);
		if(e!=nullptr)
		{
			match = const_cast<daeElement*>(e);
			return; 
		}
	}

	const daeDocument *doc = (daeDocument*)docIn;
	if(doc==nullptr) return; assert(doc==docIn->getDocument());

	//This is a simplified form of daeSIDResolver.cpp's _find() and
	//_computeDistance() procedures.
	unsigned minDistance = UINT_MAX;
	const daeElement *closestElement = nullptr;
	daeDocument::_sidMapRange range = doc->_sidMap.equal_range(ref);
	for(daeDocument::_sidMapIter it=range.first;it!=range.second;it++)	
	{
		unsigned distance = 0; 
		const daeElement *p = it->second; do
		{
			if(this==p) 
			{
				if(distance<minDistance)
				{
					minDistance = distance;	closestElement = it->second;					
				}
				break;
			}
			else distance++;

		}while(nullptr!=(p=p->getParentElement()));		
	}
	if(closestElement!=nullptr)
	match = const_cast<daeElement*>(closestElement);
}

//---.
}//<-'

/*C1071*/
