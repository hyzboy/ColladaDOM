/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH
#include <ColladaDOM.g++> //GCH

#ifdef BUILDING_IN_TINYXML /////////////////////////////////////////////

#include <tinyxml.h>

COLLADA_(namespace)
{//-.
//<-'

//TIXML_USE_STL is a mess.
struct daeTinyXMLPlugin::Value : daeHashString
{
	Value(TiXmlNode *p)
	{
		const TIXML_STRING &str = p->ValueTStr();
		string = str.c_str(); extent = str.size();
	}
};
template<class T>
T *daeTinyXMLPlugin::setValue(T *io, const daeHashString &v)
{
	const_cast<TIXML_STRING&>(io->ValueTStr()).assign(v.string,v.extent);
	return io;
}
template<class T>
T *daeTinyXMLPlugin::setQName(T *io, daeName pf, const daeName &v)
{
	const_cast<TIXML_STRING&>(io->ValueTStr()).assign(pf.string,pf.extent)
	.append(":",1).append(v.string,v.extent); return io;
}

daeTinyXMLPlugin::daeTinyXMLPlugin(int legacy):daeIOPluginCommon(legacy)
{
	//2/3 is to give US a heads up
	daeCTC<(__combined_size_on_client_stack*2/3>=sizeof(*this))>();
}

int daeTinyXMLPlugin::_errorRow(){ return _err->Row(); }

bool daeTinyXMLPlugin::_read(daeIO &IO, daeContents &content)
{
	TiXmlDocument doc; _err = &doc; 
	const daeIORequest &req = getRequest();	
	daeHashString string = req.string;	
	std::vector<daeStringCP> stringbuf; if(string.empty())
	{
		size_t size = IO.getLock(); if(size!=0)
		{
			stringbuf.resize(size+1); string = stringbuf;
			IO.readIn(&stringbuf[0],size);			
			stringbuf.back() = '\0'; string.extent--;
		}
	}
	if(!string.empty())	
	{
		//doc.Parse expects a 0-terminator.
		//doc.Parse(req.string);
		int undefined[16] = {};
		daeString q,p = string, d = p+string.extent;
		for(TiXmlNode*n;p+1<d;p=q) if(p[0]=='<')
		{
			switch(p[1])
			{
			case '/': 				
				//Theoretically this API can read a partial 
				//document, but it's not really fleshed out.				
				assert(p[1]!='/'); p = d; break;			
			case '!': if('-'==p[2]) 
				n = new TiXmlComment(); else
				n = new TiXmlUnknown(); break; //DTD
			case '?': n = new TiXmlDeclaration(); break;
			default:  n = new TiXmlElement(""); break;
			}
			q = n->Parse(p,(TiXmlParsingData*)undefined,TIXML_ENCODING_UTF8);			
			doc.LinkEndChild(n);
			if(p==q||p==nullptr) //Documentation is poor.
			goto error;
		}
		else switch(p[1])
		{
		case ' ': case '\t': case '\r': case '\n': 
				
			p++; continue; //Assuming spaces are OK.

		default: error:

			daeEH::Error<<"In daeTinyXMLPlugin::readFromMemory...";
			return false;
		}
	}/*Wanting to be rid of getReadFILE.
	else if(nullptr!=req.remoteURI)
	{
		doc.LoadFile(IO.getReadFILE()); 
		if(!doc.RootElement())
		{
			daeEH::Error<<"In daeTinyXMLPlugin::readFromFile...";
			return false;
		}
	}*/else
	{
		//daeEH::Error<<"daeTinyXMLPlugin I/O request is neither FILE, nor memory-string...";
		daeEH::Error<<"daeTinyXMLPlugin I/O request was empty...";
		return false;
	}
	
	TiXmlNode *p = doc.FirstChild();
	if(p!=nullptr&&p->Type()==TiXmlNode::TINYXML_DECLARATION)
	{
		TiXmlDeclaration *decl = (TiXmlDeclaration*)p;
		daeIOPluginCommon::_push_back_xml_decl(content,
		decl->Version(),decl->Encoding(),"yes"==daeName(decl->Standalone()));
		p = p->NextSibling();
	}
	while(p!=nullptr&&p->Type()==TiXmlNode::TINYXML_UNKNOWN)
	{
		Value v(p); if('!'==v[0])
		{
			#ifdef NDEBUG
			#error parent.getDTD().push_back(v);
			#endif	
			assert(0); p = p->NextSibling();
		}
		else break;
	}	
	_readContent2(p,content.getElement()); return true;
}

void daeTinyXMLPlugin::_readElement(TiXmlElement *tinyXmlElement, daePseudoElement &parent)
{
	daePseudoElement2 parent2 = parent;

	_err = tinyXmlElement;

	TiXmlAttribute *a;
	for(a=tinyXmlElement->FirstAttribute();a!=nullptr;a=a->Next())
	{
		daeString k = a->Name(), v = a->Value();
		daeIOPluginCommon::_attribs.push_back(_attrPair(k,v));

		//NAMESPACE LOGIC
		//Chicken-and-egg scenario: Attributes determines class.
		if(_cc.xmlnstr(k))
		{	
			//Using daeCreateContext if xmlns attributes present.
			//TODO: Should xmlTextReaderConstPrefix be used here? 
			//(Can't call xmlTextReaderMoveToNextAttribute if so.)
			parent2 = &_cc; _cc.parent = &parent;
		}
	}
	daeElement &element = 
	daeIOPluginCommon::_beginReadElement(parent2,Value(tinyXmlElement));	
	/*Post-2.5 all elements are accepted in order to prevent loss. This is not a validator.
	if(element==nullptr)
	{
		//Couldn't append the element. _beginReadElement prints an error message.
		return;
	}*/
	_readContent2(tinyXmlElement->FirstChild(),element); 
}

void daeTinyXMLPlugin::_readContent2(TiXmlNode *p, daePseudoElement &parent)
{
	for(;p!=nullptr;p=p->NextSibling())	switch(p->Type())
	{
	default: assert(0);
	case TiXmlNode::TINYXML_UNKNOWN: 
	{
		Value v(p); if('?'==v[0]&&v.extent>=2)
		{
			v.string+=1; v.extent-=2;
			parent.getContents().push_back<'?'>(v);
		}
		else assert(0); break;
	}
	case TiXmlNode::TINYXML_DECLARATION:
	
		assert(0); //Caller handles this
		break;
	
	case TiXmlNode::TINYXML_COMMENT:

		parent.getContents().push_back<'!'>(Value(p));
		break;

	case TiXmlNode::TINYXML_TEXT:

		daeIOPluginCommon::_readElementText(parent,Value(p));
		break;
		
	case TiXmlNode::TINYXML_ELEMENT:
		
		_readElement((TiXmlElement*)p,parent); 
		break;
	}
}

daeOK daeTinyXMLPlugin::writeContent(daeIO &IO, const daeContents &content)
{
	//Wanting to be rid of getWriteFILE.
	//FILE *f = IO.getWriteFILE(); 
	//if(f==nullptr) return DAE_ERR_BACKEND_IO;

	TiXmlDocument doc; _err = &doc;

	const char *version,*encoding,*standalone;
	daeIOPluginCommon::_xml_decl(content,version,encoding,standalone);	
	doc.LinkEndChild(new TiXmlDeclaration(version,encoding,standalone));

	_writeContent2(&doc,content);

	//Wanting to be rid of getWriteFILE.
	//doc.SaveFile(f); 
	TiXmlPrinter printer;
	printer.SetIndent("\t");
	doc.Accept(&printer);
	return IO.writeOut(printer.CStr(),printer.Size());	
}

void daeTinyXMLPlugin::_writeAttribute(daeData &attr, const daeElement &element)
{	
	assert(!_CD.empty()); //HACK: Comes pre-filled.

	((TiXmlElement*)_err)->SetAttribute(attr.getName(),_CD.data());
}

void daeTinyXMLPlugin::_writeElement(TiXmlNode *tinyXmlNode, const daeElement &element)
{		
	#ifdef NDEBUG
	#error This is a quick fix for QNames.
	#endif
	TiXmlElement *tiElm = new TiXmlElement("");
	if(element.hasQName())
	setQName(tiElm,element.getQNamePrefix(),element.getNCName());	
	else setValue(tiElm,element.getNCName());

	//_err may or may not work in this context. It's UNTESTED.
	//(Looks safe, going by tinyxml.cpp for _writeAttribute.)
	_err = tinyXmlNode->LinkEndChild(tiElm);
	assert(_err==tiElm);
	_err = tiElm;
	daeIOPluginCommon::_writeAttributes(element);
			   	
	//Reminder: domAny is ambiguous with regard to value/content.
	_writeContent2(tiElm,element.getContents());
	if(!element->getCharData(_CD).empty())
	tiElm->LinkEndChild(setValue(new TiXmlText(""),_CD));	
}
void daeTinyXMLPlugin::_writeContent2(TiXmlNode *tinyXmlNode, const daeContents &content)
{
	for(size_t i=0,iN=content.size();i<iN;) if(content[i].hasText())
	{
		daeText &text = content[i]; 
		enum dae_clear clear; //TinyXML is quirky. 
		clear = text.isPI_like()?dae_append:dae_clear;
		if(!clear) _CD.assign("?",1);
		if(text.getText_increment(i,_CD,clear).size()==(clear?0:1))
		continue;
		if(!clear) _CD.append_and_0_terminate("?",1);
		TiXmlNode *node; switch(text.kind())
		{			
		case daeKindOfText::COMMENT:

			node = new TiXmlComment; break;

		case daeKindOfText::PI_LIKE:

			if("xml "==daeName(_CD.data()+1,4))
			continue;
			node = new TiXmlUnknown; break;

		case daeKindOfText::MIXED:

			node = new TiXmlText(""); break; 

		default: assert(0); continue;
		}				
		_err = tinyXmlNode->LinkEndChild(setValue(node,_CD));
	}
	else _writeElement(tinyXmlNode,content[i++]);
}

//---.
}//<-'

#endif //BUILDING_IN_TINYXML///////////////////////////////////////////////

/*C1071*/
