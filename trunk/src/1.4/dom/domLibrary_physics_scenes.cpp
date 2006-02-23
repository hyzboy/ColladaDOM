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
#include <dom/domLibrary_physics_scenes.h>

daeElementRef
domLibrary_physics_scenes::create(daeInt bytes)
{
	domLibrary_physics_scenesRef ref = new(bytes) domLibrary_physics_scenes;
	return ref;
}


daeMetaElement *
domLibrary_physics_scenes::registerElement()
{
    if ( _Meta != NULL ) return _Meta;
    
    _Meta = new daeMetaElement;
    _Meta->setName( "library_physics_scenes" );
	_Meta->setStaticPointerAddress(&domLibrary_physics_scenes::_Meta);
	_Meta->registerConstructor(domLibrary_physics_scenes::create);

	// Add elements: asset, physics_scene, extra
    _Meta->appendElement(domAsset::registerElement(),daeOffsetOf(domLibrary_physics_scenes,elemAsset));
    _Meta->appendArrayElement(domPhysics_scene::registerElement(),daeOffsetOf(domLibrary_physics_scenes,elemPhysics_scene_array));
    _Meta->appendArrayElement(domExtra::registerElement(),daeOffsetOf(domLibrary_physics_scenes,elemExtra_array));

	//	Add attribute: id
 	{
		daeMetaAttribute *ma = new daeMetaAttribute;
		ma->setName( "id" );
		ma->setType( daeAtomicType::get("xsID"));
		ma->setOffset( daeOffsetOf( domLibrary_physics_scenes , attrId ));
		ma->setContainer( _Meta );
	
		_Meta->appendAttribute(ma);
	}

	//	Add attribute: name
 	{
		daeMetaAttribute *ma = new daeMetaAttribute;
		ma->setName( "name" );
		ma->setType( daeAtomicType::get("xsNCName"));
		ma->setOffset( daeOffsetOf( domLibrary_physics_scenes , attrName ));
		ma->setContainer( _Meta );
	
		_Meta->appendAttribute(ma);
	}
	
	
	_Meta->setElementSize(sizeof(domLibrary_physics_scenes));
	_Meta->validate();

	return _Meta;
}


daeMetaElement * domLibrary_physics_scenes::_Meta = NULL;


