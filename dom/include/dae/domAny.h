/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#ifndef __COLLADA_DOM__DOM_ANY_H__
#define __COLLADA_DOM__DOM_ANY_H__

#include "daeElement.h"
  
COLLADA_(namespace)
{//-.
//<-'

	/////////////////////////////////////////////
	//REMINDER: domAny IS COLLADA-DOM 2.x STYLE//
	/////////////////////////////////////////////

/**LEGACY
 * The @c domAny class is used as a placeholder where there is not a schema
 * that describes an element. This usually takes the form of pure @c domAny 
 * documents, or sometimes imported elements do not have schema's; e.g. the
 * case of COLLADA 1.5.0's MathML element without including MathML's schema.
 * Alternatively an <xs:any> part of a schema will be filled with @c domAny
 * elements as a last resort.
 *
 * @note The content-model is technically MIXED, however in this case it is
 * ambiguous. In which case, the SIMPLE model is preferred, because it lets
 * there be a value, and mixed-content will still go in the contents-arrays.
 */
class domAny : public daeElemental<domAny,64+32+1>
{
public: //COLLADA-DOM 2
	/**
	 * These were deprecated; but are kept for switch-cases.
	 */
	enum{ elementType=daeObjectType::ANY };

public: //Parameters

	typedef struct:Elemental
	{	COLLADA_DOM_0
	_0; DAEP::Value<sizeof(1),xsAnySimpleType> value;
		COLLADA_WORD_ALIGN
		COLLADA_DOM_Z(0,0)
	DAEP::Value<sizeof(2),dae_Array<>> _Z; enum{ _No=2 };
	DAEP::Value<sizeof(3),daeContents> content; typedef void notestart;
	}_;

public: //Attributes

	/**NO-NAMES
	 * These attributes are invalid according to the schema. They may be user- 
	 * defined additions and substitutes.
	 *	
	 * ANY, DOCUMENTATION-NOT-SUPPORTED
	 * This member is a placeholder for an <xs:anyAttribute> element in the schema.
	 * Feature support for <xs:anyAttribute> is limited. 
	 */
	DAEP::Value<0,xsAnyAttribute,_,(_::_)&_::_0> attrAny_et_cetera__value;

public: //Simple Content Model

	/**WARNING
	 * The @c xsAnySimpleType value of the text data of this element. 
	 * @warning Technically "domAny" should be using a mixed content
	 * model. This is subject to change. But for now it advertises a
	 * simple content-model, since handling of mixed-content is rare
	 * and mixed-content is accepted even with simple-content models.
	 */
	DAEP::Value<1,xsAnySimpleType,_,(_::_)&_::value> value;

	COLLADA_WORD_ALIGN
	COLLADA_DOM_Z(0,0) 
	/**NO-NAMES
	 * These elements are invalid according to the schema. They may be user-
	 * defined additions and substitutes.
	 *	
	 * ANY, DOCUMENTATION-NOT-SUPPORTED
	 * This member is a placeholder for an <xs:any> element in the schema.
	 * Feature support for <xs:any> is limited. 
	 */
	DAEP::Child<1,xsAny,_,(_::_)&_::_Z> elemAny_et_cetera__unnamed;
	/**
	 * Children, mixed-text, comments & processing-instructions.
	 */
	DAEP::Value<3,daeContents,_,(_::_)&_::content> content;

public: //COLLADA-DOM 2
	/**
	 * Gets the contents-array.
	 * @return Returns a reference to the contents-array.
	 */
	daeContents &getContents(){ return content; }
	/**
	 * Gets the contents-array.
	 * @return Returns a constant reference to the contents-array.
	 */
	const daeContents &getContents()const{ return content; }

		////WARNING/////////////////////////////////////
		//                                            //
		// xsAnySimpleType is bare-bones at this time //
		// xsAny should be used to access domAny data //
		// xsAnySimpleType will see more methods soon //
		//                                            //
		////////////////////////////////////////////////

	/**
	 * Gets the value of this element.
	 * @return Returns a reference to the @c xsAnySimpleType of the value.
	 */
	xsAnySimpleType &getValue(){ return value; }
	/**
	 * Gets the value of this element.
	 * @return Returns a constant reference to the @c xsAnySimpleType of the value.
	 */
	const xsAnySimpleType &getValue()const{ return value; }
	/**WARNING
	 * Sets the value of this element.
	 * @param val The new value for this element.	 
	 */
	void setValue(const xsAnySimpleType &val){ value = val; }
 	   
  ///////////// END OF GENERATION (1) OUTPUT /////////////
												
#ifdef BUILDING_COLLADA_DOM

COLLADA_(public) //INVISIBLE
		/**
		 * Implements @c domAny::_master.
		 */
		struct _Master
		{
			_Master(XS::Schema&); domAny *toc;

			daeMeta &meta; DAEP::Model &model;
		};
		/**
		 * This is an empty "meta" record, that is used to bridge
		 * the difference between regular static metadata and the 
		 * manifold @c domAny metadata. 
		 * @c XS::Any::findChild() use it for processContents="lax".
		 * @see daeStringRef.cpp
		 */
		static _Master _master;

#endif //BUILDING_COLLADA_DOM

COLLADA_(private)

	friend class DAEP::Elemental<domAny,64+32+1>;
	/**
	 * Reminder: This lets "math:math" default to @c domAny.
	 */
	COLLADA_DOM_LINKAGE static DAEP::Model &_master_model()
	COLLADA_DOM_SNIPPET( return _master.model; )
};

namespace DAEP //GCC
{
	template<>
	/**
	 * Gets a @c domAny model for the purpose of making a new domAny/model.
	 * @remarks This was added so @c DAEP::Generic<domAny> is able to work.
	 */
	inline DAEP::Model &DAEP::Elemental<domAny>::__DAEP__Object__v1__model()const
	{
		return domAny::_master_model();
	}
}

//---.
}//<-'

#endif //__COLLADA_DOM__DOM_ANY_H__
/*C1071*/
