/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the SCEA Shared Source License, Version 1.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at:
 * http://research.scea.com/scea_shared_source_license.html
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License 
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
 * implied. See the License for the specific language governing permissions and limitations under the 
 * License. 
 */

#include <dae/daeDom.h>
#include <dom/domCOLLADA.h>
#include <dae/daeMetaCMPolicy.h>
#include <dae/daeMetaSequence.h>
#include <dae/daeMetaChoice.h>
#include <dae/daeMetaGroup.h>
#include <dae/daeMetaAny.h>
#include <dae/daeMetaElementAttribute.h>

extern daeString COLLADA_VERSION;
extern daeString COLLADA_NAMESPACE;

daeElementRef
domCOLLADA::create(daeInt bytes)
{
	domCOLLADARef ref = new(bytes) domCOLLADA;
	ref->attrXmlns.setContainer( (domCOLLADA*)ref );
	ref->setAttribute("version", COLLADA_VERSION );
	ref->setAttribute("xmlns", COLLADA_NAMESPACE );
	return ref;
}


daeMetaElement *
domCOLLADA::registerElement()
{
    if ( _Meta != NULL ) return _Meta;
    
    _Meta = new daeMetaElement;
    _Meta->setName( "COLLADA" );
	_Meta->registerClass(domCOLLADA::create, &_Meta);

	daeMetaCMPolicy *cm = NULL;
	daeMetaElementAttribute *mea = NULL;
	cm = new daeMetaSequence( _Meta, cm, 0, 1, 1 );

	mea = new daeMetaElementAttribute( _Meta, cm, 0, 1, 1 );
	mea->setName( "asset" );
	mea->setOffset( daeOffsetOf(domCOLLADA,elemAsset) );
	mea->setElementType( domAsset::registerElement() );
	cm->appendChild( mea );
	
	mea = new daeMetaElementArrayAttribute( _Meta, cm, 1, 0, -1 );
	mea->setName( "library" );
	mea->setOffset( daeOffsetOf(domCOLLADA,elemLibrary_array) );
	mea->setElementType( domLibrary::registerElement() );
	cm->appendChild( mea );
	
	mea = new daeMetaElementAttribute( _Meta, cm, 2, 0, 1 );
	mea->setName( "scene" );
	mea->setOffset( daeOffsetOf(domCOLLADA,elemScene) );
	mea->setElementType( domScene::registerElement() );
	cm->appendChild( mea );
	
	cm->setMaxOrdinal( 2 );
	_Meta->setCMRoot( cm );	
    //	Add attribute: xmlns
    {
		daeMetaAttribute* ma = new daeMetaAttribute;
		ma->setName( "xmlns" );
		ma->setType( daeAtomicType::get("xsAnyURI"));
		ma->setOffset( daeOffsetOf( domCOLLADA , attrXmlns ));
		ma->setContainer( _Meta );
		//ma->setIsRequired( true );
		_Meta->appendAttribute(ma);
	}
    
	//	Add attribute: version
 	{
		daeMetaAttribute *ma = new daeMetaAttribute;
		ma->setName( "version" );
		ma->setType( daeAtomicType::get("xsString"));
		ma->setOffset( daeOffsetOf( domCOLLADA , attrVersion ));
		ma->setContainer( _Meta );
		ma->setIsRequired( true );
	
		_Meta->appendAttribute(ma);
	}
	
	
	_Meta->setElementSize(sizeof(domCOLLADA));
	_Meta->validate();

	return _Meta;
}


daeMetaElement * domCOLLADA::_Meta = NULL;

