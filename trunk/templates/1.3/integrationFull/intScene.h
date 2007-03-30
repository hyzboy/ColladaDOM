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

#ifndef __intScene_h__
#define __intScene_h__

#include <dae/daeIntegrationObject.h>
#include <dae/daeMetaElement.h>

class intScene;

typedef daeSmartRef<intScene> intSceneRef;
typedef daeTArray<intSceneRef> intSceneArray;

/**
 * The scene embodies the entire set of information that can be visualized
 * from the contents of  a COLLADA resource. The scene element declares the
 * base of the scene hierarchy or scene graph.  The scene contains elements
 * that comprise much of the visual and transformational information  content
 * as created by the authoring tools. The hierarchical structure of the scene
 * is  organized into a scene graph. A scene graph is a directed acyclic graph
 * (DAG) or tree data  structure that contains nodes of visual information
 * and related data. The structure of the  scene graph contributes to optimal
 * processing and rendering of the data and is therefore widely used in the
 * computer graphics domain.
 */
class intScene : public daeIntegrationObject
{
public: // Constuctor and Destructor
	/**
	 * Default Constructor.
	 */
	intScene();
	 
	/**
	 * Default Destructor.
	 */
	~intScene();
	 
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