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

#include "intIncludes.h"

void
registerElements()
{
  intInputGlobal::registerElement();
  intInputLocal::registerElement();
  intInputLocalOffset::registerElement();
  intInstanceWithExtra::registerElement();
  intTargetableFloat::registerElement();
  intTargetableFloat3::registerElement();
  intFx_surface_format_hint_common::registerElement();
  intFx_surface_init_planar_common::registerElement();
  intFx_surface_init_volume_common::registerElement();
  intFx_surface_init_cube_common::registerElement();
  intFx_surface_init_from_common::registerElement();
  intFx_surface_common::registerElement();
  intFx_sampler1D_common::registerElement();
  intFx_sampler2D_common::registerElement();
  intFx_sampler3D_common::registerElement();
  intFx_samplerCUBE_common::registerElement();
  intFx_samplerRECT_common::registerElement();
  intFx_samplerDEPTH_common::registerElement();
  intFx_colortarget_common::registerElement();
  intFx_depthtarget_common::registerElement();
  intFx_stenciltarget_common::registerElement();
  intFx_clearcolor_common::registerElement();
  intFx_cleardepth_common::registerElement();
  intFx_clearstencil_common::registerElement();
  intFx_annotate_common::registerElement();
  intFx_include_common::registerElement();
  intFx_newparam_common::registerElement();
  intFx_code_profile::registerElement();
  intGl_sampler1D::registerElement();
  intGl_sampler2D::registerElement();
  intGl_sampler3D::registerElement();
  intGl_samplerCUBE::registerElement();
  intGl_samplerRECT::registerElement();
  intGl_samplerDEPTH::registerElement();
  intGlsl_newarray_type::registerElement();
  intGlsl_setarray_type::registerElement();
  intGlsl_surface_type::registerElement();
  intGlsl_newparam::registerElement();
  intGlsl_setparam_simple::registerElement();
  intGlsl_setparam::registerElement();
  intCommon_float_or_param_type::registerElement();
  intCommon_color_or_texture_type::registerElement();
  intCommon_transparent_type::registerElement();
  intCommon_newparam_type::registerElement();
  intCg_sampler1D::registerElement();
  intCg_sampler2D::registerElement();
  intCg_sampler3D::registerElement();
  intCg_samplerCUBE::registerElement();
  intCg_samplerRECT::registerElement();
  intCg_samplerDEPTH::registerElement();
  intCg_connect_param::registerElement();
  intCg_newarray_type::registerElement();
  intCg_setarray_type::registerElement();
  intCg_setuser_type::registerElement();
  intCg_surface_type::registerElement();
  intCg_newparam::registerElement();
  intCg_setparam_simple::registerElement();
  intCg_setparam::registerElement();
  intGles_texture_constant_type::registerElement();
  intGles_texenv_command_type::registerElement();
  intGles_texcombiner_argumentRGB_type::registerElement();
  intGles_texcombiner_argumentAlpha_type::registerElement();
  intGles_texcombiner_commandRGB_type::registerElement();
  intGles_texcombiner_commandAlpha_type::registerElement();
  intGles_texcombiner_command_type::registerElement();
  intGles_texture_pipeline::registerElement();
  intGles_texture_unit::registerElement();
  intGles_sampler_state::registerElement();
  intGles_newparam::registerElement();
  intFx_surface_init_common::registerElement();
  intFx_annotate_type_common::registerElement();
  intFx_basic_type_common::registerElement();
  intGl_pipeline_settings::registerElement();
  intGlsl_param_type::registerElement();
  intCg_param_type::registerElement();
  intGles_pipeline_settings::registerElement();
  intGles_basic_type_common::registerElement();
  intCOLLADA::registerElement();
  intIDREF_array::registerElement();
  intName_array::registerElement();
  intBool_array::registerElement();
  intFloat_array::registerElement();
  intInt_array::registerElement();
  intAccessor::registerElement();
  intParam::registerElement();
  intSource::registerElement();
  intGeometry::registerElement();
  intMesh::registerElement();
  intSpline::registerElement();
  intP::registerElement();
  intLines::registerElement();
  intLinestrips::registerElement();
  intPolygons::registerElement();
  intPolylist::registerElement();
  intTriangles::registerElement();
  intTrifans::registerElement();
  intTristrips::registerElement();
  intVertices::registerElement();
  intLookat::registerElement();
  intMatrix::registerElement();
  intRotate::registerElement();
  intScale::registerElement();
  intSkew::registerElement();
  intTranslate::registerElement();
  intImage::registerElement();
  intLight::registerElement();
  intMaterial::registerElement();
  intCamera::registerElement();
  intAnimation::registerElement();
  intAnimation_clip::registerElement();
  intChannel::registerElement();
  intSampler::registerElement();
  intController::registerElement();
  intSkin::registerElement();
  intMorph::registerElement();
  intAsset::registerElement();
  intExtra::registerElement();
  intTechnique::registerElement();
  intNode::registerElement();
  intVisual_scene::registerElement();
  intBind_material::registerElement();
  intInstance_camera::registerElement();
  intInstance_controller::registerElement();
  intInstance_effect::registerElement();
  intInstance_force_field::registerElement();
  intInstance_geometry::registerElement();
  intInstance_light::registerElement();
  intInstance_material::registerElement();
  intInstance_node::registerElement();
  intInstance_physics_material::registerElement();
  intInstance_physics_model::registerElement();
  intInstance_rigid_body::registerElement();
  intInstance_rigid_constraint::registerElement();
  intLibrary_animations::registerElement();
  intLibrary_animation_clips::registerElement();
  intLibrary_cameras::registerElement();
  intLibrary_controllers::registerElement();
  intLibrary_geometries::registerElement();
  intLibrary_effects::registerElement();
  intLibrary_force_fields::registerElement();
  intLibrary_images::registerElement();
  intLibrary_lights::registerElement();
  intLibrary_materials::registerElement();
  intLibrary_nodes::registerElement();
  intLibrary_physics_materials::registerElement();
  intLibrary_physics_models::registerElement();
  intLibrary_physics_scenes::registerElement();
  intLibrary_visual_scenes::registerElement();
  intFx_profile_abstract::registerElement();
  intEffect::registerElement();
  intGl_hook_abstract::registerElement();
  intProfile_GLSL::registerElement();
  intProfile_COMMON::registerElement();
  intProfile_CG::registerElement();
  intProfile_GLES::registerElement();
  intBox::registerElement();
  intPlane::registerElement();
  intSphere::registerElement();
  intEllipsoid::registerElement();
  intCylinder::registerElement();
  intTapered_cylinder::registerElement();
  intCapsule::registerElement();
  intTapered_capsule::registerElement();
  intConvex_mesh::registerElement();
  intForce_field::registerElement();
  intPhysics_material::registerElement();
  intPhysics_scene::registerElement();
  intRigid_body::registerElement();
  intRigid_constraint::registerElement();
  intPhysics_model::registerElement();
}