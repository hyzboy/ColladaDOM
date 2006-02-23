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
 
#include "intInputGlobal.h"
#include "intInputLocal.h"
#include "intInputLocalOffset.h"
#include "intInstanceWithExtra.h"
#include "intTargetableFloat.h"
#include "intTargetableFloat3.h"
#include "intFx_surface_common.h"
#include "intFx_sampler1D_common.h"
#include "intFx_sampler2D_common.h"
#include "intFx_sampler3D_common.h"
#include "intFx_samplerCUBE_common.h"
#include "intFx_samplerRECT_common.h"
#include "intFx_samplerDEPTH_common.h"
#include "intFx_colortarget_common.h"
#include "intFx_depthtarget_common.h"
#include "intFx_stenciltarget_common.h"
#include "intFx_clearcolor_common.h"
#include "intFx_cleardepth_common.h"
#include "intFx_clearstencil_common.h"
#include "intFx_annotate_common.h"
#include "intFx_include_common.h"
#include "intFx_newparam_common.h"
#include "intFx_setparam_common.h"
#include "intFx_code_profile.h"
#include "intGl_sampler1D.h"
#include "intGl_sampler2D.h"
#include "intGl_sampler3D.h"
#include "intGl_samplerCUBE.h"
#include "intGl_samplerRECT.h"
#include "intGl_samplerDEPTH.h"
#include "intGlsl_newarray_type.h"
#include "intGlsl_setarray_type.h"
#include "intGlsl_surface_type.h"
#include "intGlsl_newparam.h"
#include "intGlsl_setparam_simple.h"
#include "intGlsl_setparam.h"
#include "intCommon_float_or_param_type.h"
#include "intCommon_color_or_texture_type.h"
#include "intCommon_newparam_type.h"
#include "intCg_sampler1D.h"
#include "intCg_sampler2D.h"
#include "intCg_sampler3D.h"
#include "intCg_samplerCUBE.h"
#include "intCg_samplerRECT.h"
#include "intCg_samplerDEPTH.h"
#include "intCg_connect_param.h"
#include "intCg_newarray_type.h"
#include "intCg_setarray_type.h"
#include "intCg_setuser_type.h"
#include "intCg_surface_type.h"
#include "intCg_newparam.h"
#include "intCg_setparam_simple.h"
#include "intCg_setparam.h"
#include "intGles_texture_constant_type.h"
#include "intGles_texenv_command_type.h"
#include "intGles_texcombiner_argumentRGB_type.h"
#include "intGles_texcombiner_argumentAlpha_type.h"
#include "intGles_texcombiner_commandRGB_type.h"
#include "intGles_texcombiner_commandAlpha_type.h"
#include "intGles_texcombiner_command_type.h"
#include "intGles_texture_pipeline.h"
#include "intGles_texture_unit.h"
#include "intGles_sampler_state.h"
#include "intGles_newparam.h"
#include "intFx_annotate_type_common.h"
#include "intFx_basic_type_common.h"
#include "intGl_pipeline_settings.h"
#include "intGlsl_param_type.h"
#include "intCg_param_type.h"
#include "intGles_pipeline_settings.h"
#include "intGles_basic_type_common.h"
#include "intCOLLADA.h"
#include "intIDREF_array.h"
#include "intName_array.h"
#include "intBool_array.h"
#include "intFloat_array.h"
#include "intInt_array.h"
#include "intAccessor.h"
#include "intParam.h"
#include "intSource.h"
#include "intGeometry.h"
#include "intMesh.h"
#include "intSpline.h"
#include "intP.h"
#include "intLines.h"
#include "intLinestrips.h"
#include "intPolygons.h"
#include "intPolylist.h"
#include "intTriangles.h"
#include "intTrifans.h"
#include "intTristrips.h"
#include "intVertices.h"
#include "intLookat.h"
#include "intMatrix.h"
#include "intRotate.h"
#include "intScale.h"
#include "intSkew.h"
#include "intTranslate.h"
#include "intImage.h"
#include "intLight.h"
#include "intMaterial.h"
#include "intCamera.h"
#include "intAnimation.h"
#include "intAnimation_clip.h"
#include "intChannel.h"
#include "intSampler.h"
#include "intController.h"
#include "intSkin.h"
#include "intMorph.h"
#include "intAsset.h"
#include "intExtra.h"
#include "intTechnique.h"
#include "intNode.h"
#include "intVisual_scene.h"
#include "intBind_material.h"
#include "intInstance_camera.h"
#include "intInstance_controller.h"
#include "intInstance_effect.h"
#include "intInstance_force_field.h"
#include "intInstance_geometry.h"
#include "intInstance_light.h"
#include "intInstance_material.h"
#include "intInstance_node.h"
#include "intInstance_physics_material.h"
#include "intInstance_physics_model.h"
#include "intInstance_rigid_body.h"
#include "intInstance_rigid_constraint.h"
#include "intLibrary_animations.h"
#include "intLibrary_animation_clips.h"
#include "intLibrary_cameras.h"
#include "intLibrary_controllers.h"
#include "intLibrary_geometries.h"
#include "intLibrary_effects.h"
#include "intLibrary_force_fields.h"
#include "intLibrary_images.h"
#include "intLibrary_lights.h"
#include "intLibrary_materials.h"
#include "intLibrary_nodes.h"
#include "intLibrary_physics_materials.h"
#include "intLibrary_physics_models.h"
#include "intLibrary_physics_scenes.h"
#include "intLibrary_visual_scenes.h"
#include "intFx_profile_abstract.h"
#include "intEffect.h"
#include "intGl_hook_abstract.h"
#include "intProfile_GLSL.h"
#include "intProfile_COMMON.h"
#include "intProfile_CG.h"
#include "intProfile_GLES.h"
#include "intBox.h"
#include "intPlane.h"
#include "intSphere.h"
#include "intEllipsoid.h"
#include "intCylinder.h"
#include "intTapered_cylinder.h"
#include "intCapsule.h"
#include "intTapered_capsule.h"
#include "intConvex_mesh.h"
#include "intForce_field.h"
#include "intPhysics_material.h"
#include "intPhysics_scene.h"
#include "intRigid_body.h"
#include "intRigid_constraint.h"
#include "intPhysics_model.h"
