/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH
#include <ColladaDOM.g++> //GCH

#if defined(BUILDING_IN_LIBXML)\
 || defined(BUILDING_IN_TINYXML)

COLLADA_(namespace)
{//-.
//<-'

void daeIOPluginCommon::__xstruct(int x, int legacy)
{			  
	switch(x) //Visual Studio workaround
	{
	#ifdef BUILDING_IN_LIBXML
	case daePlatform::LEGACY_LIBXML:
	new(this) daeLibXMLPlugin(legacy); return;
	case ~daePlatform::LEGACY_LIBXML:
	((daeLibXMLPlugin*)this)->daeLibXMLPlugin::~daeLibXMLPlugin(); return;
	#endif
	#ifdef BUILDING_IN_TINYXML
	case 0: new(this) daeTinyXMLPlugin(legacy); return;
	case ~0:
	((daeTinyXMLPlugin*)this)->daeTinyXMLPlugin::~daeTinyXMLPlugin(); return;
	#endif
	}
	assert(0); //Are libraries mismatched?
}

daeIOPluginCommon::daeIOPluginCommon(int legacy)
:_readFlags(),_encoder(),_decoder()
,_legacyAttributes(0==(legacy&daePlatform::LEGACY_ATTRIBUTE_MASKS))
{
	//1/3 is to give US a heads up
	daeCTC<(__combined_size_on_client_stack*1/3>=sizeof(*this))>();
}

daeOK daeIOPluginCommon::addDoc(daeDocRef &out, daeMeta*)
{		
	out = daeDocumentRef(*getRequest().getDOM()); return DAE_OK;
}

daeOK daeIOPluginCommon::readContent(daeIO &IO, daeContents &content)
{
	_readFlags = 0; if(!_read(IO,content))
	{
		const daeURI *URI = getRequest().remoteURI;
		daeEH::Error<<"Failed to load "<<(URI!=nullptr?URI->data():"XML document from memory");
		return DAE_ERR_BACKEND_IO;
	}	
	return DAE_OK;
}
		
bool daeIOPluginCommon::_beginReadContext::xmlnstr(daeString k)
{
	return k[0]=='x'&&k[1]=='m'&&k[2]=='l'&&k[3]=='n'&&k[4]=='s';
}
unsigned int daeIOPluginCommon::_beginReadContext::xmlns(daeName &o)
{
	//Basic implementation involving no namespace stack logic.	
	daeIOPluginCommon &oo = daeObjectOf(this,daeIOPluginCommon,_cc);
	_attrPair *itt,*it = &oo._attribs[0];
	for(itt=it+oo._attribs.size();it<itt;it++) 
	if(xmlnstr(it->first)) switch(it->first[5])
	{
	case '\0': if(0==prefix_extent) goto o; break;
	case ':': if(0==strncmp(it->first+6,prefix_string,prefix_extent))
	         if('\0'==it->first[6+prefix_extent]) goto o; break;
	}
	if(0) o: o = it->second; 	
	prefix_string = nullptr; return 0; //todo? _encoder
}
daeElement &daeIOPluginCommon::_beginReadElement(daePseudoElement2 parent2, const daeName &elementName)
{		
	daeChildRef<> &child = parent2->getMeta().pushBackWRT(parent2,elementName);
	//This is sub-optimal.
	if(0==child.ordinal())
	{
		_readFlags|=_readFlag_unordered;

		daeEH::Warning<<
		"Appended an unordered element named "<<
		elementName<<" at line "<<_errorRow()<<".\n"
		"(Could be a schema violation OR ordinal-space overflow.)";
	}

	//Process the attributes	
	daeAnyAttribute &aa = child->getAttributes();	
	for(size_t i=0;i<_attribs.size();i++)
	{
		daeName &name = _attribs[i].first, value = _attribs[i].second;				
		daeAA::Atom &attr = aa[name];
		//This is an old feature request.		
		//It isn't said why the document isn't just written with a Latin declaration.
		//NOTE THAT MAKING THE NAMES LATIN WOULDN'T WORK IF THE METADATA ISN'T LATIN.
		if(_maybeExtendedASCII(*attr,*child))
		{
			value = _encoder(value,_CD);
		}
		if(daeOK OK=attr->stringToMemoryWRT(child,value))
		{
			if(_legacyAttributes&&attr->isMasked()||attr->isValue()) 
			{
				aa.mask().bit(&attr-aa.cbegin()).set();
			}
		}
		else if(!value.empty()) //Some files had bad attributes.
		{
			//Expecting daeAnyAttribute to pick up any attribute.
			//_readFlags|=_readFlag_unattributed;
			_readFlags|=_readFlag_data_loss;
			daeEH::Error<<"DATA LOSS: "<<daeErrorString(OK)<<"\n"
			//"Could not create an attribute "<<name<<"=\""<<value<<"\""
			"Could not store an attribute "<<name<<"=\""<<value<<"\""
			" at line "<<_errorRow()<<".\n"
			"(Should be a schema/ABI violation.)\n";
		}
	}
	_attribs.clear(); return *child;
}

void daeIOPluginCommon::_readElementText(daeElement &el, const daeHashString &textIn)
{
	daeHashString text = textIn;
	if(el.hasCharData())
	{
		daeData &CD = el.getCharData();
		if(_maybeExtendedASCII(CD,el))
		{
			text = _encoder(text,_CD);
		}
		#ifdef NDEBUG
		#error And comment/PI peppered text?
		#endif
		if(CD.stringToMemoryWRT(&el,text))
		return;

		//It's tempting to default to mixed text, but
		//for A) there's not parity with attributes.
		//(Maybe add an xs:anyAttribute?)
		//And B) if later set, it's impossible to say
		//if it's a default or unset or what.
	}
	else //NEW: assuming mixed-text/author knows best.
	{
		el.getContents().insert<' '>(_encode(text,_CD));
		return;
	}

	_readFlags|=_readFlag_unmixed;

	daeEH::Warning<<
	"The DOM was unable to set a value for element of type "<<
	el.getTypeName()<<" at line "<<_errorRow()<<".\n"
	"Probably a schema violation.\n"
	"(2.5 policy is to add the value as a mixed-text text-node.)";
}

void daeIOPluginCommon::_writeAttributes(const daeElement &element)
{
	daeAA::iterator iit,it,aep,itt;
	daeAA &aa = element.getAttributes();
	iit = aa.cbegin(); 
	it = iit;
	aep = it+aa.getAnyExtraPart().first;
	itt = it+aa.size();
	for(;it!=aep;it++)
	{
		daeData &attr = **it;

		if(!attr.getKnownValue().compareIsDefaultWRT(attr,&element))
		{
			/**This makes me uncomfortable, but might make sense as an
			//option. It's more work to do. In this mode it would make
			//more sense to check the mask first to immediately reject.
			if(!_legacyAttributes)
			{			
				if(!aa.mask().bit(it-iit))
				{
					#ifdef NDEBUG
					#error Do something like _readFlags to signal this.
					#endif
					assert(mask.get());
					goto mask_override;
				}
			}*/
		}
		else if(_legacyAttributes&&!attr.isMasked())
		{
			if(attr.hasDefault()) //mask_override:
			{
				if(!attr.getIsRequired()&&!attr.getIsFixed()) 
				{
					continue;
				}				
			}
		}				
		else if(!aa.mask().bit(it-iit))
		{
			if(!_legacyAttributes) continue;

			//REMINDER: It seems that in this case
			//the value should normally be emptied
			//or 0-initialized. But this is seeing
			//that the write-mask agrees with this
			//and emitting the value if it doesn't.
			//It doesn't support nonstandard types.
						
			//LEGACY: Be paranoid about prototypes
			//without any schema specified default. 
			daeOpaque a = attr.getWRT(&element);
			daeTypewriter &dt = attr.getDefaultType();			
			switch(dt.getAtomicType())
			{
			default:

				if(dt.isPOD())
				{
					//Assuming zero-initialized if zero.
					if(attr.compareIsZeroedWRT(&element))
					continue;
				}
				else if(dt.hasStringType())
				{
					const daeStringRef &s = a;
					if(s.empty()) continue;
				}
				break;
			
			case daeAtom::EXTENSION: 
				
				//Should be empty if not masked.
				if(dt.hasListType())
				{
					if(0) case daeAtom::BINARY: 
					{
						a = a[daeOffsetOf(daeBinary<>,getAU())];
					}
					const daeAlloc<>* &au = a;
					if(0==au->getInternalCounter())
					continue;					
				}
				break;
			}
		}

		daeOK OK = attr.memoryToStringWRT(&element,_CD);

		#ifdef NDEBUG
		#error What about daeTypewriter errors?
		#endif
	ok:	assert(OK);

		if(_CD.empty())
		{
			//NEW: Empty/required attribute is probably illegal. 
			//So try to fill it out?
			if(attr.getIsRequired()||attr.getIsFixed())
			{
				//REMINDER: Maybe empty because of error (!OK.)
				OK = attr.getKnownValue().defaultToString(_CD);
				if(!_CD.empty()) goto ok;
			}
			else if(attr.hasDefault()) //continue;
			{
				//NEW: XML actually allows for "" attributes. 
				//If there is a default, then empty can't be
				//a valid value. So assuming it is identical
				//to the default. NOTE: "" is only practical
				//for xs:string based types. Officially it's
				//said to only be supported for string types.
				continue;
			}
			else //???
			{
				//Maybe there is more that needs to be done?
				assert(true==aa.mask().bit(it-iit));
			}
		}		

		_writeAttribute(attr,element); //Passing _CD.
	}
	for(;it!=itt;it++) //Assuming non-empty.
	{
		daeData &attr = **it;

		if(daeOK OK=attr.memoryToStringWRT(&element,_CD))
		{
			if(_CD.empty()) //continue;
			{
				//NEW: XML actually allows for "" attributes. 
				if(daeAtom::VOID==attr.getDefaultType().getAtomicType())
				continue;
			}
				
			_writeAttribute(attr,element); //Passing _CD.
		}
		else assert(OK); //UNFINISHED
	}
}

bool daeIOPluginCommon::_maybeExtendedASCII(daeData &v, const daeElement &e)
{	
	if(nullptr!=daeIOPluginCommon::_encoder)	
	switch(int at=v.getTypeWRT(e).where<daeAtom>().getAtomicType())
	{
	default: if(at<daeAtom::STRING) break;

	case daeAtom::ENUMERATION: //UBOOLEAN?

		//An enum is probably a short string.
		//With getSimpleType something like the following
		//could be done. But an enum is very likely to be an xs:string.
		//return _maybeExtendedASCII(st.getRestriction().getBase());

	case daeAtom::EXTENSION: //Could be anything?

		return true;
	}
	return false;
}

void daeIOPluginCommon::_push_back_xml_decl(daeContents &content, daeName version, daeName encoding, bool standalone)
{
	//The built-in plugins only output UTF-8. 
	//They are old/unrecommended/unworth time investments.
	//This is to let users know (in a test environment) they're recoding.
	assert("UTF-8"==encoding||"utf-8"==encoding||encoding.empty());
	encoding = "UTF-8"; 

	_CD.assign(daeName("xml version=\"")).append(version).push_back('"');
	if(!encoding.empty())
	_CD.append(daeName(" encoding=\"")).append(encoding).push_back('"');
	if(standalone)
	_CD.append(daeName(" standalone=\"yes\"")); content.insert<'?'>(_CD,0);
}
void daeIOPluginCommon::_xml_decl(const daeContents &content, daeString &version, daeString &encoding, daeString &standalone)
{
	version = "1.0"; encoding = "UTF-8"; standalone = "no";	
	if(!content.data()->hasText()) 
	return;
	daeText &xml_decl = content[0].getKnownStartOfText();
	if(!xml_decl.isPI_like()
	 ||"xml "!=daeName(xml_decl.data(),4))
	return;
	xml_decl.getText(_CD);
	for(size_t i=0;i<_CD.size();i++) if(_CD[i]=='"'&&_CD[i-1]!='=')
	{
		_CD[i] = '\0';
		size_t j = i-1; while(j>0&&_CD[j]!='=') j--;
		if(j>=11&&_CD[j+1]=='"')
		{
			char *value = &_CD[j+2]; value[-2] = '\0';
			#define _(x) \
			if(0==strcmp(value-1-sizeof(#x),#x)) x = value; else assert(0); break;
			switch(value[-3])
			{
			case 'n': _(version) case 'g': _(encoding) case 'e': _(standalone) break;
			default: assert(0);
			}
			#undef _
		}
		else assert(0);
	}
}

//---.
}//<-'

#endif //BUILDING_IN_LIBXML || BUILDING_IN_TINYXML

/*C1071*/
