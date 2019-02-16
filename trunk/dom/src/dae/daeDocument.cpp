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

daeOK daeDocument::setMeta(daeMeta &rm)
{
	if(getContents().empty()) 
	{					
		XS::Element &cm = 
		const_cast<XS::Element&>(_pseudo->jumpIntoTOC(1));
		assert(&cm==&_pseudo->getCMEntree());
		cm._element.name = rm.getName();
		cm._element.child = &rm; 	
		//HACK: This is to silence a warning. Really
		//the content-model should be modified to use
		//an XS::Any entry-point.
		//This changes the ordinal from 0 to the root.
		if(_pseudo->getAllowsAny()!=rm.getAllowsAny())		
		{
			//DAEP::Schema<>::__DAEP__Schema__g1__allows_any;
			_pseudo->_DAEP_Schema^=32;
		}	 		
	}
	else return DAE_ERR_INVALID_CALL; return DAE_OK;
}
namespace DAEP //GCC refuses to disable this (erroneous) warning
{
	template<> inline DAEP::Model &DAEP::Elemental
	<daeDocument::_PseudoElement>::__DAEP__Object__v1__model()const
	{
		return dae(this)->getMeta().getModel();
	}
}

extern daeAlloc<XS::Attribute*,1> daeAA_any_thunk;
extern const daeStringCP _daePseudoElement_[] = ":daePseudoElement:";
inline void daeDocument::_init_pseudo()
{
	memset(&_pseudo,0x00,sizeof(_pseudo));
	daeModel &model = (daeModel&)_pseudo.model;
	daeMetaElement &meta = (daeMetaElement&)_pseudo.meta;
	
	//Do like XS::Scema::addElement().
	//(BY ANY MEANS NECESSARY)
	meta._name.string = _daePseudoElement_;	
	(daeMeta*&)model.__DAEP__Model__meta = &meta;
	//UNION: _schema and _processShare are unions.
	meta._processShare = domAny::_master.meta._processShare;	
	assert(meta._schema!=nullptr);
	meta._sizeof = sizeof(_PseudoElement);		
	meta._DAEP_Schema = _PseudoElement::__DAEP__Schema;
	meta._finalFeatureID = daeFeatureID(-3);
	model[meta._feature_completeID=daeFeatureID(-1)]
	._localthunk = * //YUCK
	(meta._attribs().getAU() = daeAA_any_thunk.unit());	
	_PseudoElement *_; DAEP::Parameters ee(_);
	model.setObjectType(1); //HACK: Get around assert.
	meta._continue_XS_Schema_addElement<_PseudoElement>(ee);
	model.setObjectType(0); //HACK: Got around assert.
	//Prevent construction.
	meta._constructor = nullptr;	

	daePseudoElement &e = getPseudoElement();	
	DAEP::Element &daep = e; //FRIENDS
	daep.__DAEP__Object__parent = this; //HACK
	//HACK: it's too late to call __DAEP__Object__embed_subobject().
	daep.__DAEP__Object__refs = _uri.__DAEP__Object__refs;
	(const daeDOM*&)daep.__DAEP__Element__data.DOM = getDOM();
	
	//Just for the hell of it, mark root node as required.
	const_cast<daeCounter&>(_pseudo->jumpIntoTOC(1)._minOccurs) = 1;

	assert(!e._isElement());
	assert(e.getElementTags().empty());
	assert(e.getNCName().string==_daePseudoElement_);
	daep.__DAEP__Element__data.is_document_daePseudoElement = 1; 		
	//NOTE: 2 diverges from meta._schema._elementTags.
	//It should not be data, but if 0 it might look like one of
	//the more special internal classes; so using 2 reduces the
	//chances of getting it mixed up.
	//HACK: Gaining "protected" access via daeDocument.
	((daeDocument*)&e)->_getObjectTags().moduleTag = 2;
	assert(1==e._getPShare()&&!e._isData());

	#ifdef _DEBUG
	//This is the document's top-level content for NatvisFile.natvis.
	__Natvis_content = ((_PseudoElement*)&e)->content.operator->();
	#endif
}
daeDocument::daeDocument(daeDOM *DOM):daeDoc(DOM,daeDocType::HARDLINK),_fragment(*DOM)
{
	_init_pseudo();
}
daeDocument::daeDocument(daeArchive *a):daeDoc(a,daeDocType::HARDLINK),_fragment(*a->getDOM())
{
	_init_pseudo();
}
daeDocument::~daeDocument() //VERY SKETCHY
{
	//Trying to avoid _constructor, but let go of the CM graph.
	_pseudo->~daeMetaElement(); 
	
	assert(getContents().empty());
	daeNullable<> na = {this}; na.deAlloc(getContents().getAU());

	for(size_t i=0;i<_schemata.size();i++) _schemata[i].second->_users--;
}

daeOK daeDoc::close()
{
	//Indicates daePlatform::closeURI() closed it itself.
	bool closed = false;
	daeOK OK = _doOperation<_close_op>(getDOM(),&closed);
	if(!closed&&OK) _archive->_closedDoc(this);
	return OK;
}
void daeDocument::__daeDoc__v1__atomize()
{	
	_xmlBase.clear();
	//Clearing these ahead avoids individual erasure of their entries.
	_typeMap.clear(); _idMap.clear(); _sidMap.clear();
	//This should do it for the root, etc.
	getContents().clear();
}
daeOK daeDoc::_doOperation(int op, const const_daeDOMRef &DOM, const void *arg0)
{
	daePlatform &pf = DOM->getPlatform();

	daeOK OK;
	int id = pf.threadID();
	if(_current_operation==_no_op::value)
	{	
		_current_operation = op; _operation_thread_id = id;

		switch(op)
		{
		case _close_op::value: //Called by daeDoc::close().
		{
			//closeURI() isn't given empty URIs. They will be closed
			//unconditionally. Users can clear the URI to force this.
			if(_uri.empty()) break;			
			//Just-in-case, don't bother closeURI() with closed docs.
			if(isClosed()){ assert(0); break; }

			daeURIRef URI = (daeURI*)_uri;				
			//DAE_OK is ambiguous. (_doOperation also returns DAE_OK.)
			*(_close_op::type*)arg0 = OK = pf.closeURI(*URI);
			assert(!OK||isClosed());
			//Indicates the daePlatform does not implement closeURI(). 
			if(OK==DAE_ERR_NOT_IMPLEMENTED) 
			OK = DAE_OK;
			break;
		}
		case _setURI_op::value: //Called by daeURI_base::_setURI().
		{	
			if(_uri==&DOM->getEmptyURI()) 
			{
				//assert in case it SHOULD be non-empty.
				assert(0); return DAE_ERR_INVALID_CALL;
			}

			//setURI() is recursive.
			OK = _uri.setURI((_setURI_op::type*)arg0);
			if(OK)
			{	
				//resolve() is recursive.
				_uri.resolve(*DOM); 

				//Signals to _setURI to finish up on its end.
				OK = DAE_ERR_NOT_IMPLEMENTED;
			}
			else assert(OK!=DAE_ERR_NOT_IMPLEMENTED);
			break;
		}}

		_current_operation = _no_op::value;
	}
	else if(id!=_operation_thread_id)
	{
		//Multi-thread applications may take advantage of this--although
		//notice that this API is not doing any special synchronizations.
		return DAE_NOT_NOW;
	}
	else //Overlapping operations will likely produce unexpected results.
	{
		//Presently this might mean, an attempt to set the URI mid-close.
		assert(op==_current_operation);
	}
	return OK;
}

daeError daeDoc::_write2(const_daeDocRef &in, const daeIORequest &reqO, daeIOPlugin *O, daeIO *IO)
{
	if(nullptr==reqO.remoteURI)
	{
		if(nullptr!=reqO.localURI) //Flushing DOM?
		return DAE_ERR_INVALID_CALL; //meaningful?
	}
	else if(!reqO.isEmptyRequest()) reqO.resolve();
	else return DAE_ERR_INVALID_CALL; //meaningful?

	daeDOMRef DOM = reqO.getDOM();	
		
	if(reqO.scope!=DOM
	 &&reqO.localURI!=nullptr
	&&!reqO.localURI->transitsDoc(reqO.scope))
	{
		return DAE_ERR_QUERY_NO_MATCH; //or DAE_ERR_INVALID_CALL?
	}

	//If a doc isn't specified then one should be
	//found, but new docs should not be opened up
	//in the process. A plugin can do that though. 
	if(nullptr!=reqO.localURI&&!reqO.localURI->empty())
	{
		if(in!=nullptr)	
		{
			if(DOM!=in->getDOM()) return DAE_ERR_INVALID_CALL; 
		}
		else if(nullptr==reqO.localURI->docLookup(reqO.scope,in))
		{
			//TODO? Try _whatsupDoc? Or a soft _docHookup?
			#ifdef NDEBUG
			#error Exact match is inadequate if archive-like.		
			#endif					
		}
	}
	else if(in!=nullptr) 
	{
		if(in!=DOM) return DAE_ERR_INVALID_CALL; 		
	}
	else in = DOM; 
	
	const daeDocument *document = nullptr;
	if(in!=nullptr) if(!in->isDocument())
	{
		if(in->_link!=nullptr //RECURSIVE?
		&&daeDocType::ARCHIVE!=in->getDocType())
		{
			in = in->_link;
			daeIORequest req2 = reqO;
			req2.scope = in->_archive;
			req2.localURI = /*&*/in->_uri;
			return _write2(in,req2,O);
		}		
	}
	else document = (const daeDocument*)&*in;
		
	daeRAII::UnplugIO RAII(DOM->getPlatform());
	if(O==nullptr) 
	{
		const daeURI *plugURI = reqO.remoteURI;
		if(plugURI==nullptr) 
		{
			plugURI = DOM->getEmptyURI(); assert(in==DOM);
		}
		O = RAII = RAII.p.pluginIO(*plugURI,'w'); 

		#ifdef __COLLADA_DOM__ZAE
		if(O==nullptr)
		if(plugURI->getURI_extensionIs("zae")
		 ||plugURI->getURI_extensionIs("zip"))
		{
			static daeZAEPlugin zae; O = &zae;
		}
		#endif
	}
	daeRAII::InstantLegacyIO legacyIO; //emits error
	if(O==nullptr&&reqO.localURI!=nullptr)
	O = &legacyIO(RAII.p.getLegacyProfile(),*reqO.localURI);						
	daeRAII::Reset<const daeIORequest*> _O___request(O->_request);
	O->_request = &reqO;	
	daeIOEmpty I;
	#ifdef NDEBUG
	//Reminder: daeZAEPlugin::writeRequest has the same problem.
	#error This only partly makes sense in the output context :(
	#endif
	//Writing archives is an open question. This bit of code is
	//grasping for a way forward :(
	//daeRAII::CloseIO RAII2(RAII.p); //emits error
	//daeRAII::CloseIO RAII2(reqO.scope->getIOController()); //emits error	
	daeArchive *HACK = reqO.scope;
	if(nullptr!=document)
	{
		//No clue how this ought to work, but let's try and see.
		if(reqO.remoteURI!=nullptr
		&&!reqO.remoteURI->transitsDoc(HACK))
		{
			//UNIMPLEMENTED?
			//No archive implements write operations. It's hard
			//to say what the hell it means to write. Should an
			//archive be searched for? And opened if necessary?
			//And to what end? Writing just isn't as simplistic
			//as reading.
			
			//TODO? Give the "platform" a chance to load a best
			//matching archive. Here no matching is done so the
			//document has a chance to be saved where otherwise
			//there's little chance.
			HACK = DOM;
		}
	}
	daeRAII::CloseIO RAII2(HACK->getIOController()); //emits error		
	if(IO==nullptr)
	IO = RAII2 = RAII2.p.openIO(I,*O);
	if(IO==nullptr)
	return DAE_ERR_CLIENT_FATAL;
	//NEW: Send cancel signal in case of plugin error and return
	//write error in case where write operation did not complete.
	//return O->writeContent(*IO,getContents());	
	daeOK OK; if(nullptr==document)
	{
		//SKETCHY: Still figuring out how this is 
		//supposed to work??
		const_daeURIRef URI; if(in!=nullptr)
		{
			URI = &in->getDocURI();
		}		
		OK = O->writeRequest(*IO,URI);

		//This is a way to write a subrange of the DOM tree.
		//It's the best that can be done since it's unclear
		//how to translate global URIs to a local directory.
		if(OK==DAE_ERR_NOT_IMPLEMENTED)
		if(in==DOM)
		{
			const daeURI *wc = reqO.remoteURI;
			if(nullptr!=wc&&wc->empty()) 
			wc = nullptr;	  
			for(size_t i=0;i<DOM->_docs.size();i++)
			{
				//NOT LOOKING INSIDE ARCHIVES.
				const daeDoc *doc = DOM->_docs[i];
				if(wc==nullptr||doc->_uri.transitsURI(*wc))			
				{
					daeOK OK = doc->write();
					if(!OK) return OK;
				}
			}
			return DAE_OK;
		}
	}
	else OK = O->writeContent(*IO,document->getContents());	
	if(!OK) RAII2.OK = OK;
	RAII2.close();
	return OK?RAII2.OK:OK;
}

extern XS::Schema domAny_xs;
void daeDocument::getSchemata(XS::Schemata &o, enum dae_clear e)const	
{									   
	if(e==dae_clear) o.clear();		
	bool any = false;
	for(size_t i=0;i<_schemata.size();i++)
	{
		const XS::Schema *xs = _schemata[i].second;
		if(xs==&domAny_xs)
		any = true;
		else if(xs!=nullptr)
		o.push_back(xs);
	}
	if(any) o.push_back(&domAny_xs);
}
bool daeDocument::_get_and_or_use_xs_schema(daeTags et, const XS::Schema* &io)const
{	
	size_t nt = et.namespaceTag;
	size_t i; const XS::Schema *xs;
	for(i=0;i<_schemata.size();i++) if(nt==_schemata[i].first)
	{
		xs = _schemata[i].second; goto xs;
	}	
	
	xs = io;
	getDOM()->getPlatform().introduceToDocument(*this,et,xs);
	if(xs==nullptr) goto io;
	else if(nt!=xs->_elementTags.namespaceTag)
	{
		if(xs==&domAny_xs)
		{
			/*introduceToDocument can implement this.
			//NOTE: It might want to do it recursive
			//or to not do it at all.
			if(io==nullptr||io==&domAny_xs)
			{
				//If the schema imports another schema
				//it stores its pointer in the CM data.
				//But if it first appears as an xs:any
				//child, it might end up with domAny's
				//schema.
				for(i=0;i<_schemata.size();i++) 				
				{
					xs = &_schemata[i];
					if(xs->getXMLNS().find(et,i))
					{
						xs = xs->getXMLNS()[i].getSchema();
						if(xs==nullptr) break;
						else _get_and_or_use_xs_schema(et,xs);
						goto o;
					}
				}
				xs = &domAny_xs;
			}*/
		}
		else //PROGRAMMER ERROR
		{	
			assert(et.namespaceTag==xs->_elementTags.namespaceTag);			
			xs = nullptr; goto io;
		}
	}

	xs->_users++;
	
io:	_schemata.push_back(std::make_pair(nt,xs));

	//Helpful? Maybe if locking AU?		
xs:	if(i!=0) std::swap(_schemata[0],_schemata[i]);	

	bool o = io==xs; io = xs; return o;
}

static void daeDocument_xmlBase //RECURSIVE
(daeStringMap<daeURIRef> &c, std::pair<daeName,daeURI*> &io)
{
	std::pair<daeStringMap<daeURIRef>::iterator,bool> ins;
	ins = c.insert(io);
	if(!ins.second){ io.second = ins.first->second; return; }

	size_t len = strlen(io.first)+1; 
	if(len!=io.first.extent)			
	{
		io.first.string+=len; io.first.extent-=len;
		daeDocument_xmlBase(c,io);
		io.first.string-=len; io.first.extent+=len;
	}

	//The key is stored behind the URI member
	//data. It may also become the URI string.
	len = io.first.extent*sizeof(daeStringCP);
	xmlBase *p = (xmlBase*)operator new(sizeof(*p)+len);	
	daeCTC<sizeof(xmlBase)==sizeof(daeURI_size<>)>();
	new(p) xmlBase(dae_default,&io.second->getParentObject());
	p->setIsAttached();
	p->__DAEP__Object__unembed();		
	//1) Copy key; 2) Construct URI; 3) Copy/return an xml:base URI.
	(void*&)ins.first->first.string = 
	memcpy((char*)p+sizeof(*p),io.first,len);				 		
	p->setURI_and_resolve_client_string(ins.first->first,io.second);
	ins.first->second = io.second = *p; 
}
static bool daeDocument_xmlBase_relativistic(const daeName &URI)
{
	//NOTE: This is just a heuristic/can afford get it wrong.
	if(URI.size()>=8) for(size_t i=7;i>0;i--) if(URI[i]==':') 
	return false; return true;
}
static bool daeDocument_next_up(daeObject &d, const daeElement* &e)
{
	//NOTE: This can be implemented with the below logic
	//if callers can be trusted to supply e->getDoc() to
	//getBaseURI. An infinite-loop is worse than crashes.
	#ifdef COLLADA_DOM_STABLE
	return &d!=((const void*&)e=&e->getParentObject());
	#endif
	const daeObject &p = e->getParentObject();
	if(p._isDoc_or_DOM())
	{
		assert(&d==&p); return false;
	}
	else assert(e!=&p); (const void*&)e = p;
	assert(e->_isElement()&&e->isContent()); return true;
}
static daeName daeDocument_base(const daeElement *e)
{
	//HACK: How to support other kinds of bases?
	//Databases would need to flag the elements.
	//A callback could obtain attributes.
	daeName base = e->string("xml:base");
	//if(base.empty()) base = e->string("href"); 
	return base;
}
const_daeURIRef daeDoc::getBaseURI(const DAEP::Object *c)const
{
	daeDocument &d = *(daeDocument*)this; assert(c!=nullptr); 
	const daeElement *e = *(daeElement*)c;
	assert(isDocument()||!e->_isElement());
	if(!e->_isElement()||!d.getPseudoElement().hasAttribute_xml_base())
	return &d._uri;	
	else do if(e->hasAttribute_xml_base())
	{	
		//TODO: Might want to search getExtraAnyPart first, since
		//xml:base is likely to be a nonstatic attribute; or very
		//low in the static attribute array.
		std::pair<daeName,daeURI*> top(daeDocument_base(e),d._uri);
		if(top.first.empty()) continue;

		//daeDocument_next_up is implemented to not infinite-loop
		//if "this" is not a parent of "e" as provided by callers.
		//NOTE that if its check fails d._getPseudoElement() will
		//possibly be an access-violation/segmentation-fault. The
		//checks only "assert" in cases that should be impossible.
		//while(this!=((const void*&)e=&e->getParentObject()))
		if(daeDocument_xmlBase_relativistic(top.first))
		while(daeDocument_next_up(d,e)) 
		{
			if(e->hasAttribute_xml_base())
			{
				daeName base = daeDocument_base(e);
				if(base.empty()) continue;

				daeArray<daeStringCP,COLLADA_MAX_PATH> deep;
				deep.assign(top.first);
				deep.push_back('\0'); deep.append(base);
				deep.push_back('\0');					
				if(daeDocument_xmlBase_relativistic(base))
				while(daeDocument_next_up(d,e))				
				if(e->hasAttribute_xml_base())
				{
					base = daeDocument_base(e);
					if(base.empty()) continue;
					
					deep.append(base); deep.push_back('\0');
					if(!daeDocument_xmlBase_relativistic(base))
					break;
				}
				top.first = deep;
				daeDocument_xmlBase(d._xmlBase,top); return top.second;
			}
		}
		top.first.extent+=1;
		daeDocument_xmlBase(d._xmlBase,top); return top.second;
			
	}while(daeDocument_next_up(d,e)); return &d._uri; 
}

//// sid/idLookup //// sid/idLookup //// sid/idLookup //// sid/idLookup //// sid/idLookup 

static daeString daeDocument_ID_id(daeString ref)
{
	return (daeString)(((daeOffset&)ref)&~daeOffset(1));
}
void daeDocument::_idLookup(daeString ref, daeElementRef &match)const
{
	ref = daeDocument_ID_id(ref); //Permit # string-refs for <source> and just convenience.

	_idMapIter cit = _idMap.find(ref); if(cit!=_idMap.end()) match = cit->second;
} 
void daeDocument::_sidLookup(daeString ref, daeArray<daeElementRef> &matchingElements)const
{
	//REMINDER: daeElement::_sidLookup() accesses these directly.
	_sidMapRange range = _sidMap.equal_range(ref);
	for(_sidMapIter cit=range.first;cit!=range.second;cit++)	
	matchingElements.push_back(cit->second);
}
void daeDocument::_carry_out_change_of_ID_or_SID(const DAEP::Change &c, const daeData *a)const
{	
	const daeElement *e = dae(c.element_of_change);
			
	bool id = a->getName().string[0]=='i';
	assert(daeName(id?"id":"sid")==a->getName());

	daeTypewriter *tw = a->getTypeWRT(e);
	daeString got = (const daeString&)a->getWRT(e); 
	if(!tw->hasStringType())
	{
		//Should be an empty string.
		assert(tw->isAnySimpleType());
	}
	else if(got[0]!='\0')
	{
		if(!id) //sid?
		{
			_sidMapRange range = _sidMap.equal_range(got);
			for(_sidMapIter cit=range.first;cit!=range.second;cit++)	
			{
				if(cit->second==e)
				{
					_sidMap.erase(cit); break;
				}
			}		
		}
		else _idMap.erase(daeDocument_ID_id(got)); 
	}
	c.carry_out_change();
	got = (const daeString&)a->getWRT(e); 
	if(!tw->hasStringType())
	{
		//Should be an empty string.
		assert(tw->isAnySimpleType());
	}
	else if(got[0]!='\0') 
	{
		if(id)
		{
			daeElement* &uniqueID = _idMap[daeDocument_ID_id(got)];
			#ifdef NDEBUG
			#error This is probably worth outputting something to the error logs.
			#error What if this is a proposed "orphans" document?
			#endif
			if(nullptr!=uniqueID)			
			daeEH::Error<<"Non-unique id=\""<<got<<"\" added to daeDocument map.";
			uniqueID = const_cast<daeElement*>(e);
		}
		else _sidMap.insert(std::make_pair(got,const_cast<daeElement*>(e)));
	}
}
void daeDocument::_migrate_ID_or_SID(const daeDocument *destination, const daeElement *e, const daeData *a)const
{
	//"this" is the source, just to make things simpler.
	const daeDocument *source = this;
	assert(source!=destination); //Callers should avoid this.

	daeTypewriter *tw = a->getTypeWRT(e);
	assert(tw->hasStringType()); 
	daeString got = (const daeString&)a->getWRT(e);
	if(!tw->hasStringType()||got[0]=='\0') 
	return; 

	bool id = a->getName().string[0]=='i';
	assert(daeName(id?"id":"sid")==a->getName());		

	//These are both similar to _carry_out_change_of_ID_or_SID().
	if(nullptr!=source) 
	{
		if(!id) //sid?
		{
			_sidMapRange range = source->_sidMap.equal_range(got);
			for(_sidMapIter cit=range.first;cit!=range.second;cit++)	
			{
				if(cit->second==e)
				{
					_sidMap.erase(cit); break;
				}
			}		
		}
		else source->_idMap.erase(daeDocument_ID_id(got)); 
	}
	if(nullptr!=destination) 
	{
		if(id)
		{
			daeElement* &uniqueID = destination->_idMap[daeDocument_ID_id(got)];
			#ifdef NDEBUG
			#error This is probably worth outputting something to the error logs.
			#error What if this is a proposed "orphans" document?
			#endif
			if(nullptr!=uniqueID)			
			daeEH::Error<<"Non-unique id=\""<<got<<"\" added to daeDocument map.";
			uniqueID = const_cast<daeElement*>(e);
		}
		else destination->_sidMap.insert(std::make_pair(got,const_cast<daeElement*>(e)));
	}	
}

//// typeLookup //// typeLookup //// typeLookup //// typeLookup //// typeLookup //// typeLookup 

	//Too much goes into fulfilling this legacy feature.

//This is used to avoid graph-traversals 
//if typeLookup has never once been called. 
COLLADA_(extern) bool daeDocument_typeLookup_called = false;
void daeDocument::_typeLookup(daeMeta &meta, daeArray<daeElementRef> &matchingElements)const
{
	//USERS MAY WANT TO DISABLE TYPELOOKUP BY MACRO.
	daeDocument_typeLookup_called = true;

	//The inline APIs are doing this.
	assert(matchingElements.empty());	
	std::pair<_typeMapIter,bool> ins = 
	_typeMap.insert(std::make_pair((daeString)&meta,_typeVec()));
	_typeVec &vec = ins.first->second; if(ins.second)
	{
		meta._daeDocument_typeLookup_enabled = true;
		getPseudoElement().getDescendantsBy(daeElement::matchMeta(meta),vec);
	}//vec.data() is C++11.
	matchingElements.append(vec.empty()?nullptr:&vec[0],vec.size()); 
}		  
template<int op> 
void daeDocument::_typeLookup_migrate_operation<op>::operator()(const daeElement &ch)
{
	daeMeta &meta = ch.getMeta();
				   	
	_typeMapIter cit = doc->_typeMap.find((daeString)&meta); 		
		
	if(cit!=doc->_typeMap.end()) 
	{
		std::vector<daeElement*> &v = cit->second;
		//ALGORITHM
		//These lists aren't expected to be large, and this should only
		//be entered if the document is not being closed, yet still the
		//element is being removed (i.e. an editing/scripting context.)
		//It can't do much better, except to try to erase them en masse.
		if(op==1) v.erase(std::find(v.begin(),v.end(),&ch));	
		if(op==0) v.push_back(const_cast<daeElement*>(&ch));	
	}
	if(!ch.getMigrationBit_definitely_is_childless()) //Recurse?
	{
		meta.getContentsWRT(&ch).for_each_child(*this);
	}
}//CIRCULAR-DEPENDENCY
void daeContents_base::_operation::typeLookup_insert(daeElement &e)
{
	daeDocument::_typeLookup_migrate_operation<0> op = { e.getDocument() };
	if(!op.doc->_typeMap.empty()) op(e);
}//CIRCULAR-DEPENDENCY
void daeContents_base::_operation::typeLookup_remove(daeElement &e)
{
	daeDocument::_typeLookup_migrate_operation<1> op = { e.getDocument() };
	if(!op.doc->_typeMap.empty()) op(e);
}
daeDocument::_typeVec *daeDocument::_typeLookup_vector(daeMeta &m)const
{
	if(!_typeMap.empty())
	{	
		_typeMapIter cit = _typeMap.find((daeString)&m); 		
		if(cit!=_typeMap.end()) 
		return &cit->second;
	}
	return nullptr;
}
#ifdef NDEBUG
#error Implement _typeLookup_bulk_remove()?
#endif
void daeDocument::_typeLookup_self_remove(daeMeta &meta, daeElement &e)const
{
	std::vector<daeElement*> *v = _typeLookup_vector(meta);
	if(v!=nullptr) v->erase(std::find(v->begin(),v->end(),&e));	
}
	
inline daeCursor daeContents_base::__insert(int KoT, daeString s, size_t e, const void *before)
{
	#if _DEBUG
	for(size_t i=0;i<e;i++) assert(s[i]>='\t');
	#endif

	daeContents &c = _this();

	const int text_off = daeOffsetOf(daeText,_text);
	const int span_max = sizeof(daeContent)*255-text_off-1;

	//This just seems more readible with a loop.
	int nodes,i;
	for(nodes=0,i=(int)e;i>span_max;i-=span_max)
	nodes+=255;
	nodes+=(text_off+i+1)/sizeof(*_);
	nodes+=(text_off+i+1)%sizeof(*_)!=0?1:0;
	__uncursorize(before);
	cursor() = __uncursorize(before);
	c.grow(c.size()+nodes); //ROOM FOR OPTIMIZATION
	{
		typedef void before;

		union{ daeContent *p; daeText *t; }; p = _;

		//This is not the best, but it's consistent.
		while(p->hasText())
		{
			t = p->getText(); if(!t->isMoreText()) break;
		}

		memmove(p+nodes,p,(c.end()-p)*sizeof(*p));
		c.getAU()->setInternalCounter(c.size()+nodes);
		for(i=(int)e;i>0;i-=span_max)
		{	
			int extent = std::min(i,span_max);
			int span = i>=span_max?255:nodes%255;
			assert(span!=0);
			daeEOText *eot = (daeEOText*)(p+span-1);
			memset(eot,'\n',sizeof(*p));
			eot->_textview[eot->_cspan] = span;	
			t->_.hole = KoT;
			t->_.extent = extent;
			t->_.span = span;
			t->_.continued = i!=(int)e;
			t->_.continues = i>span_max;
			t->_.reserved = 0;
			memcpy(t->_text,s,extent*sizeof(daeStringCP)); s+=extent; p+=span;
			assert(span==eot->_textview[eot->_cspan]);	
		}

		//Note: _/cursor() isn't permitted to remain on text. 
		//It's more safe to do this than to require the user to clear it after.
		_ = p; while(_->hasText()) _+=_->getKnownText().span(); return p-nodes;
	}
}
daeCursor daeContents_base::_comment(daeString s, size_t e, const void *_)
{
	if(s[0]=='<'&&e>=7&&s[1]=='!'&&s[2]=='-'&&s[3]=='-')
	{
		s+=4; e-=3; assert(s[e]=='-'&&s[e+1]=='-'&&s[e+1]=='>');
	}
	return __insert(daeKindOfText::COMMENT,s,e,_); 
}
daeCursor daeContents_base::_mix(daeString s, size_t e, const void *_)
{
	return __insert(daeKindOfText::MIXED,s,e,_);
}
daeCursor daeContents_base::_instruct(daeString s, size_t e, const void *_)
{
	if(s[0]=='<'&&e>=4&&s[1]=='?')
	{
		s+=2; e-=4; assert(s[e]=='?'&&s[e+1]=='>');
	}
	return __insert(daeKindOfText::PI_LIKE,s,e,_);
}

//---.
}//<-'

/*C1071*/
