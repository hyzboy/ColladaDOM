
/*THIS CODE IS PROCEDURALLY-GENERATED. PLEASE EDIT THE GENERATOR; NOT THIS CODE.
 *
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

#ifndef __COLLADA_h__http_www_collada_org_2008_03_COLLADASchema__ColladaDOM_g1__
#define __COLLADA_h__http_www_collada_org_2008_03_COLLADASchema__ColladaDOM_g1__

#include "http_www_collada_org_2008_03_COLLADASchema.h"
COLLADA_H(__FILE__)
COLLADA_(namespace)
{
    COLLADA_(http_www_collada_org_2008_03_COLLADASchema,namespace)
    {//-.
//<-----'
COLLADA_DOM_NOTE(6532,COLLADA::xmlns)
COLLADA_DOM_NOTE(6533,COLLADA::version)
COLLADA_DOM_NOTE(6534,COLLADA::xml_base)
COLLADA_DOM_NOTE(6535,COLLADA::library_animations)
COLLADA_DOM_NOTE(6536,COLLADA::library_animation_clips)
COLLADA_DOM_NOTE(6537,COLLADA::library_cameras)
COLLADA_DOM_NOTE(6538,COLLADA::library_controllers)
COLLADA_DOM_NOTE(6539,COLLADA::library_geometries)
COLLADA_DOM_NOTE(6540,COLLADA::library_effects)
COLLADA_DOM_NOTE(6541,COLLADA::library_force_fields)
COLLADA_DOM_NOTE(6542,COLLADA::library_images)
COLLADA_DOM_NOTE(6543,COLLADA::library_lights)
COLLADA_DOM_NOTE(6544,COLLADA::library_materials)
COLLADA_DOM_NOTE(6545,COLLADA::library_nodes)
COLLADA_DOM_NOTE(6546,COLLADA::library_physics_materials)
COLLADA_DOM_NOTE(6547,COLLADA::library_physics_models)
COLLADA_DOM_NOTE(6548,COLLADA::library_physics_scenes)
COLLADA_DOM_NOTE(6549,COLLADA::library_visual_scenes)
COLLADA_DOM_NOTE(6550,COLLADA::library_joints)
COLLADA_DOM_NOTE(6551,COLLADA::library_kinematics_models)
COLLADA_DOM_NOTE(6552,COLLADA::library_articulated_systems)
COLLADA_DOM_NOTE(6553,COLLADA::library_kinematics_scenes)
COLLADA_DOM_NOTE(6554,COLLADA::library_formulas)
COLLADA_DOM_NOTE(6555,COLLADA::extra)
COLLADA_DOM_NOTE(6560,COLLADA::asset)
COLLADA_DOM_NOTE(6561,COLLADA::scene)
/**
 * The COLLADA element declares the root of the document that comprises some
 * of the content in the COLLADA schema.
 */
class COLLADA
: 
public DAEP::Elemental<COLLADA,0x030c000300000002ULL>
{
public: //NESTED ELEMENTS
	
	typedef class __COLLADA__scene__
	/**
	 * The scene embodies the entire set of information that can be visualized
	 * from the contents of a COLLADA resource. The scene element declares the
	 * base of the scene hierarchy or scene graph. The scene contains elements
	 * that comprise much of the visual and transformational information content
	 * as created by the authoring tools.
	 * @see XSD @c COLLADA::local__scene
	 */
	local__scene; 

public: //Parameters

	typedef struct:Elemental
	{   COLLADA_DOM_0
	_0; DAEP::Value<sizeof(1),xs::anyURI>
	_1; DAEP::Value<sizeof(2),version_enum>
	_2; DAEP::Value<sizeof(3),xml::base>
	_3; COLLADA_WORD_ALIGN
		DAEP::Child<sizeof(24)> //library_animations_type
	_4; DAEP::Child<sizeof(23)> //library_animation_clips_type
	_5; DAEP::Child<sizeof(22)> //library_cameras_type
	_6; DAEP::Child<sizeof(21)> //library_controllers_type
	_7; DAEP::Child<sizeof(20)> //library_geometries_type
	_8; DAEP::Child<sizeof(19)> //library_effects_type
	_9; DAEP::Child<sizeof(18)> //library_force_fields_type
	_10; DAEP::Child<sizeof(17)> //library_images_type
	_11; DAEP::Child<sizeof(16)> //library_lights_type
	_12; DAEP::Child<sizeof(15)> //library_materials_type
	_13; DAEP::Child<sizeof(14)> //library_nodes_type
	_14; DAEP::Child<sizeof(13)> //library_physics_materials_type
	_15; DAEP::Child<sizeof(12)> //library_physics_models_type
	_16; DAEP::Child<sizeof(11)> //library_physics_scenes_type
	_17; DAEP::Child<sizeof(10)> //library_visual_scenes_type
	_18; DAEP::Child<sizeof(9)> //library_joints_type
	_19; DAEP::Child<sizeof(8)> //library_kinematics_models_type
	_20; DAEP::Child<sizeof(7)> //library_articulated_systems_type
	_21; DAEP::Child<sizeof(6)> //library_kinematics_scenes_type
	_22; DAEP::Child<sizeof(5)> //library_formulas_type
	_23; DAEP::Child<sizeof(4)> //extra_type
	_24; COLLADA_DOM_Z(1,2)
	DAEP::Value<sizeof(27),dae_Array<>> _Z; enum{ _No=27 };
	DAEP::Value<sizeof(31),daeContents> content; typedef __NS__<6531> notestart;
	}_;

public: //Attributes

	/**NO-NAMES
	 * These attributes are invalid according to the schema. They may be user- 
	 * defined additions and substitutes.
	 */
	DAEP::Value<0,xs::anyAttribute,_,(_::_)&_::_0> extra_schema__value;
	/**
	 * This element may specify its own xmlns.
	 * @see XSD @c xs::anyURI
	 */
	DAEP::Value<1,xs::anyURI,_,(_::_)&_::_1> xmlns;
	/**
	 * The version attribute is the COLLADA schema revision with which the instance
	 * document conforms. Required Attribute.
	 * @see XSD @c version_enum
	 */
	DAEP::Value<2,version_enum,_,(_::_)&_::_2> version;
	/**
	 * The xml:base attribute allows you to define the base URI for this COLLADA
	 * document. See http://www.w3.org/TR/xmlbase/ for more information.
	 * @see XSD @c xml::base
	 */
	DAEP::Value<3,xml::base,_,(_::_)&_::_3> xml_base;

public: //Elements
		
	COLLADA_WORD_ALIGN
	/**
	 * The COLLADA element may contain any number of library_animations elements.
	 * @see XSD @c library_animations_type
	 */
	DAEP::Child<24,library_animations_type,_,(_::_)&_::_4> library_animations;
	/**
	 * The COLLADA element may contain any number of library_animation_clips elements.
	 * @see XSD @c library_animation_clips_type
	 */
	DAEP::Child<23,library_animation_clips_type,_,(_::_)&_::_5> library_animation_clips;
	/**
	 * The COLLADA element may contain any number of library_cameras elements.
	 * @see XSD @c library_cameras_type
	 */
	DAEP::Child<22,library_cameras_type,_,(_::_)&_::_6> library_cameras;
	/**
	 * The COLLADA element may contain any number of library_controllerss elements.
	 * @see XSD @c library_controllers_type
	 */
	DAEP::Child<21,library_controllers_type,_,(_::_)&_::_7> library_controllers;
	/**
	 * The COLLADA element may contain any number of library_geometriess elements.
	 * @see XSD @c library_geometries_type
	 */
	DAEP::Child<20,library_geometries_type,_,(_::_)&_::_8> library_geometries;
	/**
	 * The COLLADA element may contain any number of library_effects elements.
	 * @see XSD @c library_effects_type
	 */
	DAEP::Child<19,library_effects_type,_,(_::_)&_::_9> library_effects;
	/**
	 * The COLLADA element may contain any number of library_force_fields elements.
	 * @see XSD @c library_force_fields_type
	 */
	DAEP::Child<18,library_force_fields_type,_,(_::_)&_::_10> library_force_fields;
	/**
	 * The COLLADA element may contain any number of library_images elements.
	 * @see XSD @c library_images_type
	 */
	DAEP::Child<17,library_images_type,_,(_::_)&_::_11> library_images;
	/**
	 * The COLLADA element may contain any number of library_lights elements.
	 * @see XSD @c library_lights_type
	 */
	DAEP::Child<16,library_lights_type,_,(_::_)&_::_12> library_lights;
	/**
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_materials_type
	 */
	DAEP::Child<15,library_materials_type,_,(_::_)&_::_13> library_materials;
	/**
	 * The COLLADA element may contain any number of library_nodes elements.
	 * @see XSD @c library_nodes_type
	 */
	DAEP::Child<14,library_nodes_type,_,(_::_)&_::_14> library_nodes;
	/**
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_physics_materials_type
	 */
	DAEP::Child<13,library_physics_materials_type,_,(_::_)&_::_15> library_physics_materials;
	/**
	 * The COLLADA element may contain any number of library_physics_models elements.
	 * @see XSD @c library_physics_models_type
	 */
	DAEP::Child<12,library_physics_models_type,_,(_::_)&_::_16> library_physics_models;
	/**
	 * The COLLADA element may contain any number of library_physics_scenes elements.
	 * @see XSD @c library_physics_scenes_type
	 */
	DAEP::Child<11,library_physics_scenes_type,_,(_::_)&_::_17> library_physics_scenes;
	/**
	 * The COLLADA element may contain any number of library_visual_scenes elements.
	 * @see XSD @c library_visual_scenes_type
	 */
	DAEP::Child<10,library_visual_scenes_type,_,(_::_)&_::_18> library_visual_scenes;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_joints_type
	 */
	DAEP::Child<9,library_joints_type,_,(_::_)&_::_19> library_joints;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_models_type
	 */
	DAEP::Child<8,library_kinematics_models_type,_,(_::_)&_::_20> library_kinematics_models;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_articulated_systems_type
	 */
	DAEP::Child<7,library_articulated_systems_type,_,(_::_)&_::_21> library_articulated_systems;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_scenes_type
	 */
	DAEP::Child<6,library_kinematics_scenes_type,_,(_::_)&_::_22> library_kinematics_scenes;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_formulas_type
	 */
	DAEP::Child<5,library_formulas_type,_,(_::_)&_::_23> library_formulas;
	/**
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	DAEP::Child<4,extra_type,_,(_::_)&_::_24> extra;

	COLLADA_DOM_Z(1,2) union //NO-NAMES & ONLY-CHILDS
	{
	/**NO-NAMES
	 * These elements are invalid according to the schema. They may be user- 
	 * defined additions and substitutes.
	 */
	DAEP::Child<1,xs::any,_,(_::_)&_::_Z> extra_schema__unnamed;
	/**
	 * The COLLADA element must contain an asset element.
	 * @see XSD @c asset_type
	 */
	DAEP::Child<-1,asset_type,_,(_::_)&_::_Z> asset;
	/**
	 * The scene embodies the entire set of information that can be visualized
	 * from the contents of a COLLADA resource. The scene element declares the
	 * base of the scene hierarchy or scene graph. The scene contains elements
	 * that comprise much of the visual and transformational information content
	 * as created by the authoring tools.
	 * @see XSD @c local__scene
	 */
	DAEP::Child<-2,local__scene,_,(_::_)&_::_Z> scene;
	};

public: //Complex Content Model

	/**
	 * Children, mixed-text, comments & processing-instructions.
	 */
	DAEP::Value<31,daeContents,_,(_::_)&_::content> content;
};
COLLADA_DOM_NOTE(6564,scene::instance_physics_scene)
COLLADA_DOM_NOTE(6565,scene::instance_kinematics_scene)
COLLADA_DOM_NOTE(6566,scene::extra)
COLLADA_DOM_NOTE(6571,scene::instance_visual_scene)
/**
 * The scene embodies the entire set of information that can be visualized
 * from the contents of a COLLADA resource. The scene element declares the
 * base of the scene hierarchy or scene graph. The scene contains elements
 * that comprise much of the visual and transformational information content
 * as created by the authoring tools.
 * @see XSD @c COLLADA::local__scene
 */
class __COLLADA__scene__
: 
public DAEP::Elemental<__COLLADA__scene__,0x030d000000000006ULL>
{
public: //Parameters

	typedef struct:Elemental
	{   COLLADA_DOM_0
	_0; COLLADA_WORD_ALIGN
		DAEP::Child<sizeof(6)> //instance_with_extra_type
	_1; DAEP::Child<sizeof(5)> //instance_kinematics_scene_type
	_2; DAEP::Child<sizeof(4)> //extra_type
	_3; COLLADA_DOM_Z(1,2)
	DAEP::Value<sizeof(6),dae_Array<>> _Z; enum{ _No=6 };
	DAEP::Value<sizeof(9),daeContents> content; typedef __NS__<6563> notestart;
	}_;

public: //Attributes

	/**NO-NAMES
	 * These attributes are invalid according to the schema. They may be user- 
	 * defined additions and substitutes.
	 */
	DAEP::Value<0,xs::anyAttribute,_,(_::_)&_::_0> extra_schema__value;

public: //Elements
		
	COLLADA_WORD_ALIGN
	/**
	 * The instance_physics_scene element declares the instantiation of a COLLADA
	 * physics_scene resource. The instance_physics_scene element may appear any
	 * number of times.
	 * @see XSD @c instance_with_extra_type
	 */
	DAEP::Child<6,instance_with_extra_type,_,(_::_)&_::_1> instance_physics_scene;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c instance_kinematics_scene_type
	 */
	DAEP::Child<5,instance_kinematics_scene_type,_,(_::_)&_::_2> instance_kinematics_scene;
	/**
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	DAEP::Child<4,extra_type,_,(_::_)&_::_3> extra;

	COLLADA_DOM_Z(1,2) union //NO-NAMES & ONLY-CHILDS
	{
	/**NO-NAMES
	 * These elements are invalid according to the schema. They may be user- 
	 * defined additions and substitutes.
	 */
	DAEP::Child<1,xs::any,_,(_::_)&_::_Z> extra_schema__unnamed;
	/**
	 * The instance_visual_scene element declares the instantiation of a COLLADA
	 * visual_scene resource. The instance_visual_scene element may only appear
	 * once.
	 * @see XSD @c instance_with_extra_type
	 */
	DAEP::Child<-1,instance_with_extra_type,_,(_::_)&_::_Z> instance_visual_scene;
	};

public: //Complex Content Model

	/**
	 * Children, mixed-text, comments & processing-instructions.
	 */
	DAEP::Value<9,daeContents,_,(_::_)&_::content> content;
};
//-------.
    }//<-'
}

#ifndef COLLADA_DOM_FLAT
#include "asset_type.h"
#include "library_animations_type.h"
#include "library_animation_clips_type.h"
#include "library_cameras_type.h"
#include "library_controllers_type.h"
#include "library_geometries_type.h"
#include "library_effects_type.h"
#include "library_force_fields_type.h"
#include "library_images_type.h"
#include "library_lights_type.h"
#include "library_materials_type.h"
#include "library_nodes_type.h"
#include "library_physics_materials_type.h"
#include "library_physics_models_type.h"
#include "library_physics_scenes_type.h"
#include "library_visual_scenes_type.h"
#include "library_joints_type.h"
#include "library_kinematics_models_type.h"
#include "library_articulated_systems_type.h"
#include "library_kinematics_scenes_type.h"
#include "library_formulas_type.h"
#include "instance_with_extra_type.h"
#include "instance_kinematics_scene_type.h"
#include "extra_type.h"
#ifndef COLLADA_DOM_LITE
COLLADA_(namespace)
{
	namespace DAEP //GCC
	{//-.
//<-----'
template<>
COLLADA_(inline) Model &Elemental
<xmlns::http_www_collada_org_2008_03_COLLADASchema::COLLADA>
::__DAEP__Object__v1__model()const
{
	static Model *om = nullptr; if(om!=nullptr) return *om;
	
	__COLLADA__Element * toc=0;
	daeMetaElement &el = xmlns::http_www_collada_org_2008_03_COLLADASchema
	::__XS__Schema__().addElement(toc,"COLLADA");

	XS::Attribute *a;
	a = el.addAttribute(toc->xmlns,"xs:anyURI","xmlns");
	a = el.addAttribute(toc->version,"version_enum","version");
	a->setIsRequired();
	a = el.addAttribute(toc->xml_base,"xml:base","xml:base");
		
	om = &el.getModel();
	daeCM *cm = nullptr;
	el.addCM<XS::Sequence>(cm,0,1,1,4);
	el.addCM<XS::Element>(cm,0,1,1).setChild(toc->asset,"asset");
	el.addCM<XS::Choice>(cm,1,0,-1,20);
		el.addCM<XS::Element>(cm,0,1,1).setChild(toc->library_animations,"library_animations");
		el.addCM<XS::Element>(cm,1,1,1).setChild(toc->library_animation_clips,"library_animation_clips");
		el.addCM<XS::Element>(cm,2,1,1).setChild(toc->library_cameras,"library_cameras");
		el.addCM<XS::Element>(cm,3,1,1).setChild(toc->library_controllers,"library_controllers");
		el.addCM<XS::Element>(cm,4,1,1).setChild(toc->library_geometries,"library_geometries");
		el.addCM<XS::Element>(cm,5,1,1).setChild(toc->library_effects,"library_effects");
		el.addCM<XS::Element>(cm,6,1,1).setChild(toc->library_force_fields,"library_force_fields");
		el.addCM<XS::Element>(cm,7,1,1).setChild(toc->library_images,"library_images");
		el.addCM<XS::Element>(cm,8,1,1).setChild(toc->library_lights,"library_lights");
		el.addCM<XS::Element>(cm,9,1,1).setChild(toc->library_materials,"library_materials");
		el.addCM<XS::Element>(cm,10,1,1).setChild(toc->library_nodes,"library_nodes");
		el.addCM<XS::Element>(cm,11,1,1).setChild(toc->library_physics_materials,"library_physics_materials");
		el.addCM<XS::Element>(cm,12,1,1).setChild(toc->library_physics_models,"library_physics_models");
		el.addCM<XS::Element>(cm,13,1,1).setChild(toc->library_physics_scenes,"library_physics_scenes");
		el.addCM<XS::Element>(cm,14,1,1).setChild(toc->library_visual_scenes,"library_visual_scenes");
		el.addCM<XS::Element>(cm,15,1,1).setChild(toc->library_joints,"library_joints");
		el.addCM<XS::Element>(cm,16,1,1).setChild(toc->library_kinematics_models,"library_kinematics_models");
		el.addCM<XS::Element>(cm,17,1,1).setChild(toc->library_articulated_systems,"library_articulated_systems");
		el.addCM<XS::Element>(cm,18,1,1).setChild(toc->library_kinematics_scenes,"library_kinematics_scenes");
		el.addCM<XS::Element>(cm,19,1,1).setChild(toc->library_formulas,"library_formulas");
		el.popCM<20,3000>(cm);
	el.addCM<XS::Element>(cm,60001,0,1).setChild(toc->scene,"scene");
	el.addCM<XS::Element>(cm,60002,0,-1).setChild(toc->extra,"extra");
	el.addContentModel<60003,1>(cm,toc); return *om;
}
template<>
COLLADA_(inline) Model &Elemental
<xmlns::http_www_collada_org_2008_03_COLLADASchema::COLLADA::local__scene>
::__DAEP__Object__v1__model()const
{
	static Model *om = nullptr; if(om!=nullptr) return *om;
	
	__COLLADA__Element * toc=0;
	daeMetaElement &el = xmlns::http_www_collada_org_2008_03_COLLADASchema
	::__XS__Schema__().addElement(toc,"scene");
		
	om = &el.getModel();
	daeCM *cm = nullptr;
	el.addCM<XS::Sequence>(cm,0,1,1,4);
	el.addCM<XS::Element>(cm,0,0,-1).setChild(toc->instance_physics_scene,"instance_physics_scene");
	el.addCM<XS::Element>(cm,1,0,1).setChild(toc->instance_visual_scene,"instance_visual_scene");
	el.addCM<XS::Element>(cm,2,0,-1).setChild(toc->instance_kinematics_scene,"instance_kinematics_scene");
	el.addCM<XS::Element>(cm,3,0,-1).setChild(toc->extra,"extra");
	el.addContentModel<4,1>(cm,toc); return *om;
}
//-------.
    }//<-'
}
#endif //!COLLADA_DOM_LITE
#endif //!COLLADA_DOM_FLAT
 
COLLADA_(namespace)
{
	namespace DAEP
	{
		COLLADA_(http_www_collada_org_2008_03_COLLADASchema,namespace)
		{//-.
//<---------'
/**WYSIWYG
 * The COLLADA element declares the root of the document that comprises some
 * of the content in the COLLADA schema.
 * @see XSD @c COLLADA
 */
struct COLLADA
:
daeSmartRef<__::COLLADA>
{
	COLLADA_DOM_3(COLLADA,struct,daeSmartRef)
	/**
	 * This element may specify its own xmlns.
	 * @see XSD @c xs::anyURI
	 */
	typedef xs::anyURI xmlns_anyURI;
	/**
	 * The version attribute is the COLLADA schema revision with which the instance
	 * document conforms. Required Attribute.
	 * @see XSD @c version_enum
	 */
	typedef __::version_enum version_enum;
	/**
	 * The xml:base attribute allows you to define the base URI for this COLLADA
	 * document. See http://www.w3.org/TR/xmlbase/ for more information.
	 * @see XSD @c xml::base
	 */
	typedef xml::base xml_base;
	/**
	 * The COLLADA element must contain an asset element.
	 * @see XSD @c asset_type
	 */
	typedef struct asset asset;
	/**
	 * The COLLADA element may contain any number of library_animations elements.
	 * @see XSD @c library_animations_type
	 */
	typedef struct library_animations library_animations;
	/**
	 * The COLLADA element may contain any number of library_animation_clips elements.
	 * @see XSD @c library_animation_clips_type
	 */
	typedef struct library_animation_clips library_animation_clips;
	/**
	 * The COLLADA element may contain any number of library_cameras elements.
	 * @see XSD @c library_cameras_type
	 */
	typedef struct library_cameras library_cameras;
	/**
	 * The COLLADA element may contain any number of library_controllerss elements.
	 * @see XSD @c library_controllers_type
	 */
	typedef struct library_controllers library_controllers;
	/**
	 * The COLLADA element may contain any number of library_geometriess elements.
	 * @see XSD @c library_geometries_type
	 */
	typedef struct library_geometries library_geometries;
	/**
	 * The COLLADA element may contain any number of library_effects elements.
	 * @see XSD @c library_effects_type
	 */
	typedef struct library_effects library_effects;
	/**
	 * The COLLADA element may contain any number of library_force_fields elements.
	 * @see XSD @c library_force_fields_type
	 */
	typedef struct library_force_fields library_force_fields;
	/**
	 * The COLLADA element may contain any number of library_images elements.
	 * @see XSD @c library_images_type
	 */
	typedef struct library_images library_images;
	/**
	 * The COLLADA element may contain any number of library_lights elements.
	 * @see XSD @c library_lights_type
	 */
	typedef struct library_lights library_lights;
	/**
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_materials_type
	 */
	typedef struct library_materials library_materials;
	/**
	 * The COLLADA element may contain any number of library_nodes elements.
	 * @see XSD @c library_nodes_type
	 */
	typedef struct library_nodes library_nodes;
	/**
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_physics_materials_type
	 */
	typedef struct library_physics_materials library_physics_materials;
	/**
	 * The COLLADA element may contain any number of library_physics_models elements.
	 * @see XSD @c library_physics_models_type
	 */
	typedef struct library_physics_models library_physics_models;
	/**
	 * The COLLADA element may contain any number of library_physics_scenes elements.
	 * @see XSD @c library_physics_scenes_type
	 */
	typedef struct library_physics_scenes library_physics_scenes;
	/**
	 * The COLLADA element may contain any number of library_visual_scenes elements.
	 * @see XSD @c library_visual_scenes_type
	 */
	typedef struct library_visual_scenes library_visual_scenes;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_joints_type
	 */
	typedef struct library_joints library_joints;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_models_type
	 */
	typedef struct library_kinematics_models library_kinematics_models;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_articulated_systems_type
	 */
	typedef struct library_articulated_systems library_articulated_systems;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_scenes_type
	 */
	typedef struct library_kinematics_scenes library_kinematics_scenes;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_formulas_type
	 */
	typedef struct library_formulas library_formulas;
	/**
	 * The scene embodies the entire set of information that can be visualized
	 * from the contents of a COLLADA resource. The scene element declares the
	 * base of the scene hierarchy or scene graph. The scene contains elements
	 * that comprise much of the visual and transformational information content
	 * as created by the authoring tools.
	 * @see XSD @c local__scene
	 */
	typedef struct scene scene;
	/**
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	typedef struct extra extra;
};
/**WYSIWYG, CONST-FORM
 * The COLLADA element declares the root of the document that comprises some
 * of the content in the COLLADA schema.
 * @see XSD @c COLLADA
 */
struct const_COLLADA
:
daeSmartRef<const __::COLLADA>
{
	COLLADA_DOM_3(const_COLLADA,struct,daeSmartRef)
	/**CONST-FORM
	 * This element may specify its own xmlns.
	 * @see XSD @c xs::anyURI
	 */
	typedef const xs::anyURI xmlns_anyURI;
	/**CONST-FORM
	 * The version attribute is the COLLADA schema revision with which the instance
	 * document conforms. Required Attribute.
	 * @see XSD @c version_enum
	 */
	typedef const __::version_enum version_enum;
	/**CONST-FORM
	 * The xml:base attribute allows you to define the base URI for this COLLADA
	 * document. See http://www.w3.org/TR/xmlbase/ for more information.
	 * @see XSD @c xml::base
	 */
	typedef const xml::base xml_base;
	/**CONST-FORM
	 * The COLLADA element must contain an asset element.
	 * @see XSD @c asset_type
	 */
	typedef struct const_asset asset;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_animations elements.
	 * @see XSD @c library_animations_type
	 */
	typedef struct const_library_animations library_animations;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_animation_clips elements.
	 * @see XSD @c library_animation_clips_type
	 */
	typedef struct const_library_animation_clips library_animation_clips;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_cameras elements.
	 * @see XSD @c library_cameras_type
	 */
	typedef struct const_library_cameras library_cameras;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_controllerss elements.
	 * @see XSD @c library_controllers_type
	 */
	typedef struct const_library_controllers library_controllers;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_geometriess elements.
	 * @see XSD @c library_geometries_type
	 */
	typedef struct const_library_geometries library_geometries;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_effects elements.
	 * @see XSD @c library_effects_type
	 */
	typedef struct const_library_effects library_effects;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_force_fields elements.
	 * @see XSD @c library_force_fields_type
	 */
	typedef struct const_library_force_fields library_force_fields;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_images elements.
	 * @see XSD @c library_images_type
	 */
	typedef struct const_library_images library_images;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_lights elements.
	 * @see XSD @c library_lights_type
	 */
	typedef struct const_library_lights library_lights;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_materials_type
	 */
	typedef struct const_library_materials library_materials;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_nodes elements.
	 * @see XSD @c library_nodes_type
	 */
	typedef struct const_library_nodes library_nodes;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_materials elements.
	 * @see XSD @c library_physics_materials_type
	 */
	typedef struct const_library_physics_materials library_physics_materials;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_physics_models elements.
	 * @see XSD @c library_physics_models_type
	 */
	typedef struct const_library_physics_models library_physics_models;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_physics_scenes elements.
	 * @see XSD @c library_physics_scenes_type
	 */
	typedef struct const_library_physics_scenes library_physics_scenes;
	/**CONST-FORM
	 * The COLLADA element may contain any number of library_visual_scenes elements.
	 * @see XSD @c library_visual_scenes_type
	 */
	typedef struct const_library_visual_scenes library_visual_scenes;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_joints_type
	 */
	typedef struct const_library_joints library_joints;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_models_type
	 */
	typedef struct const_library_kinematics_models library_kinematics_models;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_articulated_systems_type
	 */
	typedef struct const_library_articulated_systems library_articulated_systems;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_kinematics_scenes_type
	 */
	typedef struct const_library_kinematics_scenes library_kinematics_scenes;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c library_formulas_type
	 */
	typedef struct const_library_formulas library_formulas;
	/**CONST-FORM
	 * The scene embodies the entire set of information that can be visualized
	 * from the contents of a COLLADA resource. The scene element declares the
	 * base of the scene hierarchy or scene graph. The scene contains elements
	 * that comprise much of the visual and transformational information content
	 * as created by the authoring tools.
	 * @see XSD @c local__scene
	 */
	typedef struct const_scene scene;
	/**CONST-FORM
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	typedef struct const_extra extra;
};
/**WYSIWYG
 * The scene embodies the entire set of information that can be visualized
 * from the contents of a COLLADA resource. The scene element declares the
 * base of the scene hierarchy or scene graph. The scene contains elements
 * that comprise much of the visual and transformational information content
 * as created by the authoring tools.
 * @see XSD @c COLLADA::local__scene
 */
struct scene
:
daeSmartRef<__::COLLADA::local__scene>
{
	COLLADA_DOM_3(scene,struct,daeSmartRef)
	/**
	 * The instance_physics_scene element declares the instantiation of a COLLADA
	 * physics_scene resource. The instance_physics_scene element may appear any
	 * number of times.
	 * @see XSD @c instance_with_extra_type
	 */
	typedef struct __XSD__instance_with_extra_type__ instance_physics_scene;
	/**
	 * The instance_visual_scene element declares the instantiation of a COLLADA
	 * visual_scene resource. The instance_visual_scene element may only appear
	 * once.
	 * @see XSD @c instance_with_extra_type
	 */
	typedef struct __XSD__instance_with_extra_type__ instance_visual_scene;
	/**UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c instance_kinematics_scene_type
	 */
	typedef struct instance_kinematics_scene instance_kinematics_scene;
	/**
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	typedef struct extra extra;
};
/**WYSIWYG, CONST-FORM
 * The scene embodies the entire set of information that can be visualized
 * from the contents of a COLLADA resource. The scene element declares the
 * base of the scene hierarchy or scene graph. The scene contains elements
 * that comprise much of the visual and transformational information content
 * as created by the authoring tools.
 * @see XSD @c COLLADA::local__scene
 */
struct const_scene
:
daeSmartRef<const __::COLLADA::local__scene>
{
	COLLADA_DOM_3(const_scene,struct,daeSmartRef)
	/**CONST-FORM
	 * The instance_physics_scene element declares the instantiation of a COLLADA
	 * physics_scene resource. The instance_physics_scene element may appear any
	 * number of times.
	 * @see XSD @c instance_with_extra_type
	 */
	typedef struct __XSD__const_instance_with_extra_type__ instance_physics_scene;
	/**CONST-FORM
	 * The instance_visual_scene element declares the instantiation of a COLLADA
	 * visual_scene resource. The instance_visual_scene element may only appear
	 * once.
	 * @see XSD @c instance_with_extra_type
	 */
	typedef struct __XSD__const_instance_with_extra_type__ instance_visual_scene;
	/**CONST-FORM, UNDOCUMENTED
	 * The XSD schema does not provide documentation in this case.
	 * @see XSD @c instance_kinematics_scene_type
	 */
	typedef struct const_instance_kinematics_scene instance_kinematics_scene;
	/**CONST-FORM
	 * The extra element may appear any number of times.
	 * @see XSD @c extra_type
	 */
	typedef struct const_extra extra;
};
//-----------.
        }//<-'
    }	
}
	
#endif //__COLLADA_h__http_www_collada_org_2008_03_COLLADASchema__ColladaDOM_g1__
/*C1071*/