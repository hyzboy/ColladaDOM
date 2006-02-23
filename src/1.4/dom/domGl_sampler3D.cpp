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
#include <dom/domGl_sampler3D.h>

daeElementRef
domGl_sampler3D::create(daeInt bytes)
{
	domGl_sampler3DRef ref = new(bytes) domGl_sampler3D;
	return ref;
}


daeMetaElement *
domGl_sampler3D::registerElement()
{
    if ( _Meta != NULL ) return _Meta;
    
    _Meta = new daeMetaElement;
    _Meta->setName( "gl_sampler3D" );
	_Meta->setStaticPointerAddress(&domGl_sampler3D::_Meta);
	_Meta->registerConstructor(domGl_sampler3D::create);

	// Add elements: source, wrap_s, wrap_t, wrap_p, minfilter, magfilter, mipfilter, border_color, mipmap_maxlevel, mipmap_bias
    _Meta->appendElement(domSource::registerElement(),daeOffsetOf(domGl_sampler3D,elemSource));
    _Meta->appendElement(domWrap_s::registerElement(),daeOffsetOf(domGl_sampler3D,elemWrap_s));
    _Meta->appendElement(domWrap_t::registerElement(),daeOffsetOf(domGl_sampler3D,elemWrap_t));
    _Meta->appendElement(domWrap_p::registerElement(),daeOffsetOf(domGl_sampler3D,elemWrap_p));
    _Meta->appendElement(domMinfilter::registerElement(),daeOffsetOf(domGl_sampler3D,elemMinfilter));
    _Meta->appendElement(domMagfilter::registerElement(),daeOffsetOf(domGl_sampler3D,elemMagfilter));
    _Meta->appendElement(domMipfilter::registerElement(),daeOffsetOf(domGl_sampler3D,elemMipfilter));
    _Meta->appendElement(domBorder_color::registerElement(),daeOffsetOf(domGl_sampler3D,elemBorder_color));
    _Meta->appendElement(domMipmap_maxlevel::registerElement(),daeOffsetOf(domGl_sampler3D,elemMipmap_maxlevel));
    _Meta->appendElement(domMipmap_bias::registerElement(),daeOffsetOf(domGl_sampler3D,elemMipmap_bias));
	
	
	_Meta->setElementSize(sizeof(domGl_sampler3D));
	_Meta->validate();

	return _Meta;
}


daeMetaElement * domGl_sampler3D::_Meta = NULL;


