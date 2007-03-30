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

#ifndef __intSource_h__
#define __intSource_h__

#include <dae/daeIntegrationObject.h>
#include <dae/daeMetaElement.h>

class intSource;

typedef daeSmartRef<intSource> intSourceRef;
typedef daeTArray<intSourceRef> intSourceArray;

/**
 * The source element declares a data repository that provides values according
 * to the  semantics of an input element that refers to it. A data source
 * is a well-known source of information that can be accessed through an 
 * established communication channel. The data source provides access methods
 * to the information.  These access methods implement various techniques
 * according to the representation of the  information. The information may
 * be stored locally as an array of data or a program that  generates the
 * data. One of the array elements (array, bool_array, float_array, int_array,
 * or Name_array) may  occur zero or more times.
 */
class intSource : public daeIntegrationObject
{
public: // Constuctor and Destructor
	/**
	 * Default Constructor.
	 */
	intSource();
	 
	/**
	 * Default Destructor.
	 */
	~intSource();
	 
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

public:
class intTechnique;

typedef daeSmartRef<intTechnique> intTechniqueRef;
typedef daeTArray<intTechniqueRef> intTechniqueArray;

/**
 * The technique element must occur one or more times. Within the technique
 * element,  the common profile's accessor elements describe the output of
 * the source element.  The output is assembled from the accessor elements
 * in the order they are specified.
 */
class intTechnique : public daeIntegrationObject
{
public: // Constuctor and Destructor
	/**
	 * Default Constructor.
	 */
	intTechnique();
	 
	/**
	 * Default Destructor.
	 */
	~intTechnique();
	 
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

};

#endif