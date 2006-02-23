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

#ifndef __intSkin_h__
#define __intSkin_h__

#include <dae/daeIntegrationObject.h>
#include <dae/daeMetaElement.h>

/** The skin element contains vertex and primitive information sufficient to describe 
			blend-weight skinning. The skinning information is stored outside of the affected geometry, 
			under the skin element. This decoupling allows skeletons to 'skin' multiple geometries and 
			different instances of a geometry element to be modified by different skeletons. 
			* Bind-shape: represented with a source, the same way as mesh vertex positions. 
			* Influences / joints: a combiner represents the variable-length influence lists for each 
			vertex. 
			* Bind-pose: to avoid inverting matrices on import, COLLADA stores the inverse of the 
			bind-matrices. The matrices are per-node (they are the same for each vertex that is 
			influenced by a certain node). This is represented by two matching arrays (one for the 
			nodes and one for the matrices) and combined by the joints element. Note that any node can 
			be an influence, not only joints. The name of the joints element was chosen for familiarity 
			reasons. There must be a one-to-one correspondence between the vertices in skin and the mesh 
			vertices. Naturally, if a mesh is exported in its bind-shape (the skeleton was not moved 
			after binding), the bindshape positions in the skin will be the same as the vertex positions 
			under the targeted mesh. Also, the skeleton's transforms will result in matrices that, after 
			inverting, will match the inverse-bind matrices in the skin.
*/class intSkin;

typedef daeSmartRef<intSkin> intSkinRef;
typedef daeTArray<intSkinRef> intSkinArray;

class intSkin : public daeIntegrationObject
{
public: // Constuctor and Destructor
	/**
	 * Default Constructor.
	 */
	intSkin();
	 
	/**
	 * Default Destructor.
	 */
	~intSkin();
	 
protected: // VIRTUAL INTEGRATION INTERFACE

// IMPORT FROM COLLADA	
	// This Method is used to create tool/runtime object
	virtual void createFrom(daeElementRef element);

	// This method translate from COLLADA to tool/runtime object
	virtual void fromCOLLADA();

	// This method is used as a second pass post process on tool/runtime objs
	virtual void fromCOLLADAPostProcess();

// EXPORT TO COLLADA	
	// This Method is used to create COLLADA objects
	virtual void createTo(void* userData);

	// This method translate to COLLADA from tool/runtime object
	virtual void toCOLLADA();

	// This method is used as a second pass post process on COLLADA dom
	virtual void toCOLLADAPostProcess();

public: // STATIC METHODS
	static daeElementRef create(daeInt bytes);
	static daeMetaElement* registerElement();
	
public: // STATIC MEMBERS
	static daeMetaElement* _Meta;

};

#endif
