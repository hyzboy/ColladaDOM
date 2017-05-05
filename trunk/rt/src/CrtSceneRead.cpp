/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <RT.pch.h> //PCH

#include "CrtNode.h"
#include "CrtScene.h"
#include "CrtRender.h"
#include "CrtTexture.h"
#include "CrtLight.h"
#include "CrtCamera.h"
#include "CrtEffect.h"
#include "CrtGeometry.h"
#include "CrtController.h"
#include "CrtAnimation.h"
					
COLLADA_(namespace)
{	
	namespace RT
	{//-.
//<-----'

//Adding this so these values can
//be hooked up if needed later on.
struct RT::DBase::LoadTargetable_of
{ 
	template<class S>
	LoadTargetable_of(S &in, float &v)
	{
		if(b=!in.empty()) v = in->value;
	}
	template<class S>
	LoadTargetable_of(S &in, double &v)
	{
		if(b=!in.empty()) v = in->value;
	}
	template<class S>
	LoadTargetable_of(S &in, FX::Float3 &v)
	{
		if(b=!in.empty()) in->value->get3at(0,v.x,v.y,v.z);
	}
	template<class S>
	LoadTargetable_of(S &in, FX::Float4 &v)
	{
		if(b=!in.empty()) in->value->get4at(0,v.x,v.y,v.z,v.w);
	}
	template<class S>
	LoadTargetable_of(S &in, RT::Matrix &v)
	{
		//This is tortured to avoid loading an identity matrix.
		const S::XSD::type *e;
		if(b=!in.empty()) e = in;
		if(!b||e->value->size()<16) RT::MatrixLoadIdentity(v); 
		if(b) e->value->get4at(0,v[M00],v[M10],v[M20],v[M30]);
		if(b) e->value->get4at(4,v[M01],v[M11],v[M21],v[M31]);
		if(b) e->value->get4at(8,v[M02],v[M12],v[M22],v[M32]);
		if(b) e->value->get4at(12,v[M03],v[M13],v[M23],v[M33]);
		
	}
	operator bool(){ return b; } bool b;
};

struct RT::DBase::LoadLight_technique_common
{
	RT::Light *light;
	LoadLight_technique_common(RT::Light *l, 
	Collada05::const_light::technique_common in):light(l)
	{		
		if(in==nullptr) return;

		if(!in->ambient.empty())
		{
			light->Type = RT::Light_Type::AMBIENT;
			LoadTargetable_of(in->ambient->color,light->Color);
		}
		else if(!in->directional.empty())
		{
			light->Type = RT::Light_Type::DIRECTIONAL;
			LoadTargetable_of(in->directional->color,light->Color);
		}
		else if(!in->point.empty())
		{
			light->Type = RT::Light_Type::POINT;
			LoadTargetable_of(in->point->color,light->Color);
			Load_attenuation(*in->point);
		}
		else if(!in->spot.empty())
		{
			light->Type = RT::Light_Type::POINT;
			Collada05::const_spot spot = in->spot;			
			LoadTargetable_of(spot->color,light->Color);
			Load_attenuation(*spot);
			LoadTargetable_of(spot->falloff_angle,light->FalloffAngle);
			LoadTargetable_of(spot->falloff_exponent,light->FalloffExponent);
		}
	}
	template<class T> void Load_attenuation(T &in)
	{
		LoadTargetable_of(in.constant_attenuation,light->ConstantAttenuation);
		LoadTargetable_of(in.linear_attenuation,light->LinearAttenuation);
		LoadTargetable_of(in.quadratic_attenuation,light->QuadraticAttenuation);
	}
};
RT::Light *RT::DBase::LoadLight(Collada05::const_light in)
{
	if(in==nullptr) return nullptr;
	RT::Light *out = GetLight(in);
	if(out!=nullptr) return out; //light was instanced
				   
	daeEH::Verbose<<"Adding new light "<<in->id;
						 
	out = COLLADA_RT_new(RT::Light);	
	out->Id = in->id; out->DocURI = RT::DocURI(in);
	
	LoadLight_technique_common(out,in->technique_common);

	Lights.push_back(out); return out;
}

struct RT::DBase::LoadCamera_technique_common
{
	RT::Camera *out;
	LoadCamera_technique_common(RT::Camera *cm,
	Collada05::const_optics::technique_common in):out(cm)
	{
		if(in!=nullptr)
		if(!in->orthographic.empty()) Load_shape(*in->orthographic);
		else if(!in->perspective.empty()) Load_shape(*in->perspective);		
		out->Refresh();
	}
	template<class T> void Load_shape(T &in)
	{
		Load_shape_2(in);
		LoadTargetable_of(in.aspect_ratio,out->Aspect);
		if(LoadTargetable_of(in.znear,out->ZNear)) 
		out->ZNear*=RT::Asset.Meter;
		if(LoadTargetable_of(in.zfar,out->ZFar)) 
		out->ZFar*=RT::Asset.Meter;
	}
	void Load_shape_2(const DAEP::Schematic<Collada05::orthographic>::type &in)
	{
		#ifdef NDEBUG
		#error It should be easy to fix this since X/Ymag shouldn't be 0.
		#endif
		daeEH::Warning<<"Support for orthographic cameras is rudimentary.";
		LoadTargetable_of(in.xmag,out->Xmag); out->Xfov = 0;
		LoadTargetable_of(in.ymag,out->Ymag); out->Yfov = 0;
	}
	void Load_shape_2(const DAEP::Schematic<Collada05::perspective>::type &in)
	{
		LoadTargetable_of(in.xfov,out->Xfov); out->Xmag = 0;
		LoadTargetable_of(in.yfov,out->Yfov); out->Ymag = 0;
	}
};
RT::Camera *RT::DBase::LoadCamera(Collada05::const_camera in)
{
	if(in==nullptr) return nullptr;	
	RT::Camera *out = GetCamera(in);
	if(out!=nullptr) return out; //camera was instanced

	daeEH::Verbose<<"Adding new Camera "<<in->id;

	out = COLLADA_RT_new(RT::Camera);
	out->Id = in->id; out->DocURI = RT::DocURI(in);

	LoadCamera_technique_common(out,in->optics->technique_common);

	Cameras.push_back(out); return out;
}

struct RT::DBase::LoadEffect_profile_COMMON
{
	RT::Effect *out;
	RT::DBase *dbase; 	
	Collada05::const_profile_COMMON profile_COMMON; 
	const daeDocument *doc;
	LoadEffect_profile_COMMON(RT::DBase *db, RT::Effect *ef, Collada05::const_effect &ef2)
	:dbase(db),out(ef),doc(dae(ef2)->getDocument())
	{
		#ifdef NDEBUG
		#error This only accesses the first profile_COMMON/technique.
		#endif
		profile_COMMON = ef2->profile_COMMON;
		if(profile_COMMON==nullptr)
		return;

		for(size_t i=0;i<profile_COMMON->image.size();i++)				
		dbase->LoadImage(profile_COMMON->image[i]);

		Collada05::const_profile_COMMON::technique technique;
		technique = profile_COMMON->technique;
		if(technique!=nullptr)
		{
			for(size_t i=0;i<technique->image.size();i++)
			dbase->LoadImage(technique->image[i]);

			//This support is really basic, since the shader models don't descend from a common type
			//we have to handle each one individually.  There can only be one in the technique.
			//All of them assume the texture is in the diffuse component for now.		
			if(!technique->constant.empty())
			Load_constant(*technique->constant);			
			else if(!technique->lambert.empty())
			Load_lambert(*technique->lambert);			
			else if(!technique->phong.empty())
			Load_phong(*technique->phong);
			else if(!technique->blinn.empty())
			Load_blinn(*technique->blinn);	
		}
	}
	template<class blinn> void Load_blinn(blinn &blinn)
	{
		Load_phong(blinn);
	}	
	template<class phong_base> void Load_phong(phong_base &phong)
	{
		Load_lambert(phong);		
		Load_color_or_texture(phong.specular,out->Specular);			
		Load_float_or_param(phong.shininess,out->Shininess);
	}	
	template<class lambert_base> void Load_lambert(lambert_base &lambert)
	{
		Load_constant(lambert);
		Load_color_or_texture(lambert.ambient,out->Ambient);	
		Load_color_or_texture(lambert.diffuse,out->Diffuse);	
	}
	template<class constant_base> void Load_constant(constant_base &constant)
	{
		Load_color_or_texture(constant.emission,out->Emission);	
		Load_color_or_texture(constant.reflective,out->Reflective);	
		Load_float_or_param(constant.reflectivity,out->Reflectivity);			
		Load_color_or_texture(constant.transparent,out->Transparent);			
		Load_float_or_param(constant.transparency,out->Transparency);			
		Load_float_or_param(constant.index_of_refraction,out->RefractiveIndex);
	}
	void Load_float_or_param(const Collada05_XSD::common_float_or_param_type *in, float &o)
	{
		if(in!=nullptr&&!LoadTargetable_of(in->float__alias,o)&&!in->param.empty())
		{
			Collada05::const_profile_COMMON::newparam newparam;
			dae(profile_COMMON)->sidLookup(in->param->ref,newparam,doc);
			if(newparam!=nullptr) LoadTargetable_of(newparam->float__alias,o);
		}
	}
	template<class T> void Load_color_or_texture(T &child, FX::Float4 &o)
	{	
		if(child.empty()) return; 
		
		const typename T::XSD::type &in = *child;				
		if(LoadTargetable_of(in.color,o)) return;
	
		typedef Collada05::const_profile_COMMON::newparam Lnewparam;
		Lnewparam newparam;
		if(!in.texture.empty())
		{
			//1.4.1 IS RIDICULOUS?!?!
			//The texture image is three levels of indirection away!						
			Lnewparam::sampler2D::source source = //1
			dae(profile_COMMON)->sidLookup(in.texture->texture,newparam,doc)->sampler2D->source; 
			if(source!=nullptr)
			{
				Lnewparam::surface surface = //2
				dae(profile_COMMON)->sidLookup(source->value,newparam,doc)->surface;
				if(surface!=nullptr&&!surface->init_from.empty())
				{
					//REMINDER: init_from IS AN ARRAY.
					assert(1==surface->init_from.size());
					RT::Image *texture = dbase->LoadImage //3
					(doc->idLookup<Collada05::image>(surface->init_from->value));
					if(texture!=nullptr) 
					out->Textures.push_back(texture); //OK!
				}
			}
			//The COMMON profile doesn't modulate textures.
			o = FX::Float4(1);
		}
		else if(!in.param.empty())
		{
			#ifdef NDEBUG
			#error collada_spec_1_4.pdf About Parameters has guidelines on casting.
			#endif
			dae(profile_COMMON)->sidLookup(in.param->ref,newparam,doc);
			if(newparam!=nullptr)
			{
				//TODO: "RT::Newparam::cast" via database integration.
				if(!LoadTargetable_of(newparam->float4,o))
				if(!LoadTargetable_of(newparam->float3,o))
				daeEH::Error<<"Failed to cast color_or_texture_type param.";
			}			
		}
	}
};
RT::Effect *RT::DBase::LoadEffect(Collada05::const_effect in)
{
	if(in==nullptr) return nullptr;
	RT::Effect *out = GetEffect(in);
	if(out!=nullptr) return out; //effect was instanced
	
	daeEH::Verbose<<"Adding new Effect "<<in->id;

	for(size_t i=0;i<in->image.size();i++)		
	LoadImage(in->image[i]);
	
	out = COLLADA_RT_new(RT::Effect);
	out->Id = in->id; out->DocURI = RT::DocURI(in);
	if(RT::Main.LoadEffects)
	out->COLLADA_FX = RT::Main.COLLADA_FX.Load(in);

	RT::DBase::LoadEffect_profile_COMMON(this,out,in);	

	Effects.push_back(out); return out;
}

RT::Material *RT::DBase::LoadMaterial(Collada05::const_material in)
{
	if(in==nullptr) return nullptr;	
	RT::Material *out = GetMaterial(in);
	if(out!=nullptr) return out; //material was instanced	

	const daeDocument *doc = dae(in)->getDocument();
	
	Collada05::const_effect effect = 
	xs::anyURI(in->instance_effect->url->*"",doc).get<Collada05::effect>();
	if(effect==nullptr) return nullptr;

	daeEH::Verbose<<"Adding new Material "<<in->id<<"...\n"
	"Attaching effect "<<effect->id;

	out = COLLADA_RT_new(RT::Material);
	out->Id = in->id; out->DocURI = RT::DocURI(in);

	//find the effect that the material is refering too 
	out->Effect = LoadEffect(effect);	
	doc = dae(effect)->getDocument();
	if(RT::Main.LoadEffects)
	out->COLLADA_FX = RT::Main.COLLADA_FX.Load(in,out->Effect->COLLADA_FX,doc); 		

	//NEW: Adding this, just for correctness.
	#ifdef NDEBUG
	#error What about <setparam> and <technique_hint>? Material has unused values.
	#endif	
	*out = *out->Effect; //Copy RT::Material_Data.

	Materials.push_back(out); return out;	
}

struct RT::DBase::LoadGeometry_technique_common
{
	enum //These are feature test placeholders.
	{
		Load_spline=0 
	};
	template<class mesh_or_convex_mesh>
	void Load_mesh(mesh_or_convex_mesh &in)
	{
		Collada05::const_vertices vertices = in.vertices;
		if(vertices!=nullptr)
		{
			inputs = Inputs(vertices->input,doc);			
			out->Vertices = inputs.position.count;
			out->Positions = inputs.position.source;
			out->Normals = inputs.normal.source;
			out->TexCoords = inputs.texture0.source;
		}	
		//These parameters are humble triangulation codes.
		//0 for fans that pivot around 0.
		//1 for non-fan that repeat every 1.
		//2 for lines: repeating every 2.
		//3 for triangles: repeating every 3.
		//- implicates line primitives.		
		Load_p<0>(in.polylist); Load_p<3>(in.triangles);
		Load_p<0>(in.polygons); Load_p<1>(in.tristrips);		
		Load_p<0>(in.trifans);			
		Load_p<-2>(in.lines); Load_p<-1>(in.linestrips);	
	}
	RT::Geometry *out; const daeDocument *doc;		
	LoadGeometry_technique_common(RT::Geometry *out, Collada05::const_geometry &in)
	:out(out),doc(dae(in)->getDocument())
	{		
		if(!in->mesh.empty())
		{
			Load_mesh(*in->mesh);		
		}
		else if(!in->convex_mesh.empty())
		{
			Load_mesh(*in->convex_mesh);
			//LoadGeometry_technique_common will not do this.
			assert(in->convex_mesh->convex_hull_of->empty());
		}
		//This sorts in order of material, format, then mode.
		std::sort(out->Elements.begin(),out->Elements.end());

		//TODO: ONCE THIS PACKAGE IS MATURE, HERE, MERGE THE
		//ELEMENT PACKS WHERE POSSIBLE, AND SEE ABOUT ADDING
		//A STRIPPER/FANNER TO THE MIX THAT ISN'T INTERESTED
		//IN CACHE-LOCALITY TO TRANSMIT LESS DATA TO SERVERS.
		//LESS DATA IS ALSO LESS WORK FOR THE CLIENT INDEXER.
	}

	struct Inputs //SINGLETON
	{	
		Inputs(){} //compiler
	
		struct Input
		{	
			unsigned offset, count;

			Input():offset(),count(){}

			RT::Geometry_Semantic source;

		}position,normal,texture0; int max_offset;
		
		template<class input>
		Inputs(input &in, const daeDocument *doc):max_offset()
		{
			RT::Accessor05 array(doc);
			for(size_t i=0;i<in.size();i++)
			{
				Input *set = _SetOffset(*in[i]);
				if(set==nullptr) continue;
				
				array.bind(in[i]);
				set->source = array;
				if(array==nullptr) continue;

				set->count = array.accessor->count;
				set->source.Offset = array.accessor->offset;
				set->source.Stride = array.accessor->stride;
			}
		}
		Input *_SetOffset(const Collada05_XSD::InputLocal &in_i)
		{
			if("POSITION"==in_i.semantic) return &position;			
			if("NORMAL"==in_i.semantic) return &normal;			
			return "TEXCOORD"==in_i.semantic?&texture0:nullptr;
		}
		Input *_SetOffset(const Collada05_XSD::InputLocalOffset &in_i)
		{
			int os = (int)in_i.offset;
			if(max_offset<=os) max_offset = os+1; 		
			if("VERTEX"==in_i.semantic)
			{
				//The VERTEX input refers to <vertices> 
				//as opposed to a proper <source> ref.
				position.offset = os; return nullptr; //&position;
			}
			else if("NORMAL"==in_i.semantic)
			{
				normal.offset = os;	return &normal;
			}
			else if("TEXCOORD"==in_i.semantic)
			{
				if(in_i.set==0)
				{
					texture0.offset = os; return &texture0;
				}
				else daeEH::Error<<"Unsupported TEXCOORD set "<<+in_i.set;
			}
			return nullptr;
		}

	}inputs; enum{ triangulate_offsetN=3 };
		
	int triangulate_offset[triangulate_offsetN];
			
	int push_back_Elements(xs::string material, GLenum mode)
	{	
		int max, VERTEX_index = inputs.position.offset;		

		RT::Geometry_Elements e;
		e.Material_Instance_Symbol = material; e.Mode = mode;
		e.Normals = inputs.normal.source;		
		e.TexCoords = inputs.texture0.source;	
		//.Index is used as inputs below.
		e.Normals.Index = inputs.normal.offset;
		e.TexCoords.Index = inputs.texture0.offset;
		//e._Collapse_Index(inputs.position.offset);		
		{
		  //This is designed to make the VERTEX data
		  //come first, since it's stored implicitly.
		  //AND to remove vertex attributes from the
		  //inputs that RT::Geometry isn't ready for. 

			RT::Geometry_Semantic *it; 
			for(max=0,it=e.Begin();it<e.End();it++)		
			if(it->Index!=VERTEX_index&&*it!=nullptr)
			{
				if(0!=VERTEX_index) it->Index++;
				max = std::max<int>(max,it->Index);
			}
			else it->Index = 0;
			for(int i=1;i<=max;i++)
			{
				bool keep = false;
				for(it=e.Begin();it<e.End();it++)
				{
					if(i==it->Index) keep = true;
				}
				if(keep) continue; i--; max--;
				for(it=e.Begin();it<e.End();it++)
				if(i<(int)it->Index) it->Index--;
			}

			//Build a map from the Index values to the original offset.
			triangulate_offset[e.Normals.Index] = inputs.normal.offset;
			triangulate_offset[e.TexCoords.Index] = inputs.texture0.offset;
			triangulate_offset[0] = VERTEX_index; //Important this is last.
		}		
		e.Region = out->ElementBuffer.size(); assert(max<triangulate_offsetN); 
		
		out->Elements.push_back(e); return max+1; daeCTC<3==triangulate_offsetN>(); 
	}
	template<int M, class T> void Load_p(T &in)
	{
		for(size_t i=0;i<in.size();i++)
		{
			const typename T::XSD::type &in_i = *in[i];	
			if(in_i.p.empty()) continue;

			inputs = Inputs(in_i.input,doc);
			int indexes = //Unhandled vertex-attributes are discarded.
			push_back_Elements(in_i.material,i<0?GL_LINES:GL_TRIANGLES);	
			for(int i=0;i<indexes;i++)
			{
				triangulate_offset[0] = triangulate_offset[i];
				if(M*M>=2) Load_p_1<M>(in_i); 
				if(M*M<=1) Load_p_N<M>(in_i); 
				if(i==0) //Capture the number of triangulated indices.
				out->Elements.back().Width = 
				out->ElementBuffer.size()-out->Elements.back().Region;
			}	
			if(0==out->Elements.back().Width) out->Elements.pop_back();
		}
	}
	template<int M, class T> void Load_p_1(T &in_i)
	{									   
		Collada05::const_p p = in_i.p;
		Triangulate<M>(p,p->value->size()/inputs.max_offset);
	}	
	template<int M, class T> void Load_p_N(T &in_i)
	{
		for(size_t i=0;i<in_i.p.size();i++)
		{
			Collada05::const_p p = in_i.p[i];			
			Triangulate<M>(p,p->value->size()/inputs.max_offset);
		}
	}	
	template<> void Load_p_N<0>(const Collada05_XSD::polylist &in_i)
	{	
		GLuint restart = 0;
		Collada05::const_p p = in_i.p;		
		Collada05::const_vcount vcount = in_i.vcount;
		//<vcount> is not required, but the manual doesn't say what to do without it.
		if(vcount==nullptr) 
		{	
			GLuint restartN = p->value->size()/inputs.max_offset;
			GLuint vcount_i = restartN/in_i.count;
			if(p->value->size()!=vcount_i*in_i.count*inputs.max_offset)			
			daeEH::Warning<<"<polylist><vcount> is absent and <polylist count> does\n"
			"not agree with the <p> data stride calculated by <input offset>.\n";			
			while(restart<restartN)
			{
				Triangulate<0>(p,vcount_i,restart); restart+=vcount_i;
			}
		}
		else for(size_t i=0;i<vcount->value->size();i++)
		{
			Triangulate<0>(p,vcount->value[i],restart); restart+=vcount->value[i];
		}
	}
	template<int M> void Triangulate(Collada05::const_p &p, size_t count, GLuint restart=0)
	{
		if(0==count) return;	

		enum{ nN = M<0?-M:M==0?1:M, tristrip=M==1 };

		std::vector<GLuint> &buf = out->ElementBuffer; 
		GLuint stride = inputs.max_offset;
		GLuint i = triangulate_offset[0]+restart*stride;
		GLuint iN = i+stride*(GLuint)count;
		GLuint index[nN], index0 = 0, index1 = 0;
		if(0!=p->value->size()%stride)
		{
			//Don't want trouble with RT::Geometry_Elements::Width.
			daeEH::Error<<"<p> size is not a multiple of <input> offsets.";
			return;
		}		

		assert(!tristrip);

		if(M==0||tristrip) //0 is triangle fans.				
		{
			p->value->get1at(i,index0); i+=stride;
		}
		if(M==0||M==-1||tristrip) //- is lines. 1 is linestrip.
		{
			p->value->get1at(i,index1); i+=stride;
		}
		for(;i<iN;index1=index[nN-1])
		{
			for(int n=0;n<nN;n++,i+=stride)
			if(1!=p->value->get1at(i,index[n]))
			daeEH::Error<<"Index into <p> is out-of-range.\n"
			"(Is <vcount> to blame?)";

			#ifdef NDEBUG
			#error Get visuals. Do tristrips alternate?
			#endif
			if(M==0||tristrip) buf.push_back(index0);
			if(M==0||M==-1||tristrip) buf.push_back(index1);				
			for(int n=0;n<nN;n++) buf.push_back(index[n]);	
			if(tristrip) index0 = index1;
		}
	}		
};
RT::Geometry *RT::DBase::LoadGeometry(Collada05::const_geometry in)
{
	if(in==nullptr||!RT::Main.LoadGeometry)
	return nullptr;
	RT::Geometry *out = GetGeometry(in);
	if(out!=nullptr) return out; //geometry was instanced
	
	if(!in->spline.empty())
	{
		if(!LoadGeometry_technique_common::Load_spline)
		{
			daeEH::Warning<<"Not visualising <spline> of geometry "<<in->id;
			return nullptr;
		}
	}
	else if(!in->convex_mesh.empty())
	{
		if(!in->convex_mesh->convex_hull_of->empty())
		{
			//Physics processes convex_hull_of. The physics library is computing the
			//hull. In the unlikely event this is entered, it will just fail to show.
			daeEH::Warning<<"Not visualizing \"convex_hull_of\" geometry "<<in->id;
			return nullptr;
		}
	}

	daeEH::Verbose<<"Adding new Geometry "<<in->id;

	//Assuming <source><asset> doesn't specify up/meter.
	RT::Main_Asset _RAII(in);

	out = COLLADA_RT_new(RT::Geometry);
	out->Id = in->id; out->DocURI = RT::DocURI(in);

	LoadGeometry_technique_common(out,in);	
		 
	out->SetRange(out->Positions,out->Vertices);

	Geometries.push_back(out); return out;
}

RT::Image *RT::DBase::LoadImage(Collada05::const_image in)
{
	if(in==nullptr||!RT::Main.LoadImages)
	return nullptr;
	RT::Image *out = GetImage(in);
	if(out!=nullptr) return out; //image was instanced

	daeEH::Verbose<<"Adding new image "<<in->id;

	out = COLLADA_RT_new(RT::Image);
	out->Id = in->id; out->DocURI = RT::DocURI(in);

	//FYI: This is a relative URI (relative to DocURI.)
	out->URL = in->init_from->value;

	//load the actual image passing in the current file name 
	//to first look relative to the current <file>_Textures
	//for the textures 
	out->Refresh(); 
	
	//NOTE: If the image failed to load, there's no sense in
	//continually trying to reload it.
	Images.push_back(out); return out;
}

struct RT::DBase::LoadAnimation_channel
{	
	RT::Animation *out; const daeDocument *doc;
	LoadAnimation_channel(RT::Animation *out, Collada05::const_animation &in)
	:out(out),doc(dae(in)->getDocument()) //...
	,INPUT(doc),OUTPUT(doc),IN_TANGENT(doc),OUT_TANGENT(doc),INTERPOLATION(doc)		
	{
		assert(&doc<(void*)&INPUT); //C++
		for(size_t i=0;i<in->channel.size();i++) 
		Load(in->channel[i]);			
		if(out->Channels.empty()) daeEH::Warning<<"No <channel> remains/exists.";
	}		
	daeSIDREF SIDREF;
	RT::Accessor05 INPUT,OUTPUT,IN_TANGENT,OUT_TANGENT;		
	RT::Accessor<Collada05::const_Name_array> INTERPOLATION;
	void SetSamplerInput(Collada05::const_sampler::input in)
	{						  
		if(in->semantic=="INPUT") INPUT.bind(in);
		else if(in->semantic=="OUTPUT") OUTPUT.bind(in);
		else if(in->semantic=="IN_TANGENT") IN_TANGENT.bind(in);
		else if(in->semantic=="OUT_TANGENT") OUT_TANGENT.bind(in);
		else if(in->semantic=="INTERPOLATION") INTERPOLATION.bind(in);
		else daeEH::Warning<<"Unrecognized <sampler> semantic: "<<in->semantic;
	}			
	void Load(Collada05::const_channel in)
	{	
		daeRefRequest req;
		SIDREF = in->target;
		if(!SIDREF.get(req)||!req.isAtomicType())
		{
			#ifdef NDEBUG
			#error If there is an element but no data, then use the value's capacity.
			#endif
			daeEH::Error<<"Animation target "<<in->target<<" is not understood.";
			return;
		}

		//This check is needed, because if a selection is not applied, the returned
		//length is the length of the string itself, which will treat each codepoint
		//as a keyframe parameter.
		switch(req.type->writer->getAtomicType())
		{
		case daeAtomicType::STRING: case daeAtomicType::TOKEN:

			daeEH::Error<<"Animation target is an untyped string "<<in->target<<"\n"<<
			"(Is the target an extensions?)";
			return;
		}
				
		//Each channel's <sampler> is almost required to be unique.		
		INPUT.bind(); IN_TANGENT.bind();
		OUTPUT.bind(); OUT_TANGENT.bind(); INTERPOLATION.bind();
		{
			Collada05::const_sampler sampler;			
			if(doc->idLookup(in->source,sampler)!=nullptr)
			for(size_t i=0;i<sampler->input.size();i++)
			SetSamplerInput(sampler->input[i]);
		}
		if(INPUT==nullptr||OUTPUT==nullptr)
		{	
			daeEH::Error<<"No <channel> INPUT and/or OUTPUT data for sampler "<<in->source;
			return;
		}		

		RT::Animation_Channel ch = in->target;
		RT::Target t(req.rangeMin,req.rangeMax);
		t.Fragment = req->a<xs::any>();
		ch.Target = t;
		size_t iN = INPUT.accessor->count;
		size_t jN = t.GetSize();			

		bool transpose = false;
		//2017: ItemTargeted was added so not to encode
		//so much information for no purpose. But there
		//is still this problem of transposing <matrix>.
		if(!RT::Matrix_is_COLLADA_order
		&&nullptr!=req->a<Collada05::matrix>()) if(jN==1) 
		{
			ch.Target.Min =
			ch.Target.Max = ch.Target.Min%4*4+ch.Target.Min/4;
		}
		else if(jN!=16) //SID member selection is all-or-1.
		{
			daeEH::Error<<
			"Failed to transpose animated <matrix> with "<<jN<<" params/channels.\n"<<
			"(1 or 16 expected.)";
			return;
		}
		else transpose = true;
		
		ch.Points = (short)iN;
		ch.Parameters = (short)(1+jN);
		ch.PointSize = (short)1+ch.Parameters; //LINEAR
		ch.SamplePointsMin = (short)out->SamplePoints.size();
		ch.Algorithms = 0;
		if(iN==0) return; //C++98/03 support

		//Copying variable length units is dicey.
		size_t o_s = OUTPUT.accessor->stride;	
		size_t o_o = OUTPUT.accessor->offset;
		if(o_o+o_s*OUTPUT.accessor->count>OUTPUT->value.size())
		{
			OUTPUT._out_of_range(); return;
		}		
		const RT::Float *o_1 = &OUTPUT->value[o_o]-1;

		//POINT-OF-NO-RETURN//
		out->SamplePoints.resize(ch.SamplePointsMin+iN*ch.PointSize);
		RT::Spline_Point *p = &out->SamplePoints[ch.SamplePointsMin];

		RT::Float t0; INPUT.get1at(0,t0);
		out->TimeMin = std::min(out->TimeMin,t0);
		for(size_t i=0;i<iN;i++,p+=ch.PointSize)
		{
			//Reminder: don't set to BEZIER/HERMITE if there are not
			//IN_TANGENT and OUT_TANGENT data included in PointsSize.
			p->Algorithm = RT::Spline_Algorithm::LINEAR;
			p->LinearSteps = 0; 			
			ch.Algorithms|=p->Algorithm;
			RT::Float *params = p->GetParameters();
			INPUT.get1at(i,params[0]);
			for(size_t j=1;j<=jN;j++) params[j] = o_1[j]; o_1+=o_s;
		}
		if(transpose)
		{
			p-=iN*ch.PointSize;
			for(size_t i=0;i<iN;i++,p+=ch.PointSize)			
			RT::Matrix4x4Transpose(p->GetParameters()+1);
		}
		out->Channels.push_back(ch);				
		out->TimeMax = std::max(out->TimeMax,p[-ch.PointSize].GetParameters()[0]);
	}
};
void RT::DBase::LoadAnimation(Collada05::const_animation in)
{						
	assert(RT::Main.LoadAnimations);

	daeEH::Verbose<<"Adding new animation "<<in->id;

	RT::Animation *out = COLLADA_RT_new(RT::Animation);	
	out->Id = in->id; out->DocURI = RT::DocURI(in);

	LoadAnimation_channel(out,in);

	//also get it's last key time and first key time
	RT::Asset.TimeMin = std::min(RT::Asset.TimeMin,out->TimeMin);
	RT::Asset.TimeMax = std::max(RT::Asset.TimeMax,out->TimeMax);

	//RECURSIVE
	Animations.push_back(out);
	for(size_t i=0;i<in->animation.size();i++) LoadAnimation(in->animation[i]);	
}

struct RT::DBase::LoadController_skin
{
	////LoadGeometry_technique_common() in miniature.
	int inputs_max_offset, JOINT_offset, WEIGHT_offset;
	
	RT::Skin *out; 
	LoadController_skin(daeName &src,
	RT::Controller *con, Collada05::const_skin in)
	:out(dynamic_cast<RT::Skin*>(con))
	,JOINT_offset(-1)
	,WEIGHT_offset(-1),WEIGHT_array(in),inputs_max_offset()
	{
		src = in->source__ATTRIBUTE;

		LoadTargetable_of(in->bind_shape_matrix,out->BindShapeMatrix);		
	
		RT::Accessor<> array(in);
		Collada05::const_Name_array SIDs;
		Collada05::const_IDREF_array IDREFs;		
		Collada05::const_joints joints = in->joints;						
		if(joints!=nullptr)
		for(size_t i=0;i<joints->input.size();i++)
		{
			daeName semantic = array.bind(joints->input[i]);

			if("INV_BIND_MATRIX"==semantic)
			{
				//Maybe these should just be pointers, so
				//there's the one std::vector/less memory.
				array.get16(out->Joints_INV_BIND_MATRIX);
			}
			else if("JOINT"==semantic)
			{
				SIDs = array->a<Collada05::Name_array>();
				IDREFs = array->a<Collada05::IDREF_array>();
				out->Using_IDs_to_FindJointNode = IDREFs!=nullptr;
				if(out->Using_IDs_to_FindJointNode||SIDs!=nullptr)	
				array.get(out->Joints,SIDs->value->*IDREFs->value);
			}
		}
		//<skin> is done is Y_UP/1 space.		
		{
			if(RT::Asset.Up!=RT::Up::Y_UP)
			{
				RT::Matrix up;
				RT::MatrixLoadAsset(up,RT::Asset.Up);				
				//Two multiplies translates into a coordinate system
				//producing a mutant identity matrix with two -1s if
				//necessary that cancel each other out.
				RT::MatrixMult(up,out->BindShapeMatrix);
				RT::MatrixMult(out->BindShapeMatrix,up,out->BindShapeMatrix);
				for(size_t i=0;i<out->Joints_INV_BIND_MATRIX.size();i++)
				{
					RT::MatrixMult(up,out->Joints_INV_BIND_MATRIX[i]);
					RT::MatrixMult(out->Joints_INV_BIND_MATRIX[i],up,out->Joints_INV_BIND_MATRIX[i]);
				}
			}
			if(RT::Asset.Meter!=1)
			{
				out->BindShapeMatrix[M30]*=RT::Asset.Meter;
				out->BindShapeMatrix[M31]*=RT::Asset.Meter;
				out->BindShapeMatrix[M32]*=RT::Asset.Meter;
				for(size_t i=0;i<out->Joints_INV_BIND_MATRIX.size();i++)
				{
					out->Joints_INV_BIND_MATRIX[i][M30]*=RT::Asset.Meter;
					out->Joints_INV_BIND_MATRIX[i][M31]*=RT::Asset.Meter;
					out->Joints_INV_BIND_MATRIX[i][M32]*=RT::Asset.Meter;
				}
			}
		}

		Collada05::const_vertex_weights
		vertex_weights = in->vertex_weights;				
		if(vertex_weights!=nullptr)
		{
			Collada05::const_vertex_weights::input input;
			for(size_t i=0;i<vertex_weights->input.size();i++)
			{
				input = vertex_weights->input[i];

				inputs_max_offset = std::max<int>(inputs_max_offset,input->offset);

				if(input->set!=0)
				{
					//This will require some work to rectify.
					#ifdef NDEBUG
					#error In theory sets can reuse sources; but seem to be purposeless.
					#endif
					daeEH::Error<<"Unsupported <vertex_weights> set "<<+input->set;
					continue;
				}

				int *offset;
				if("JOINT"==input->semantic)
				{
					offset = &JOINT_offset;
				}
				else if("WEIGHT"==input->semantic)
				{
					offset = &WEIGHT_offset; WEIGHT_array.bind(input);
				}
				else continue;

				if(-1!=*offset)
				if(*offset==input->offset)
				{
					daeEH::Warning<<"Duplicate "<<input->semantic<<" offset in set 0.\n"<<
					"(Assuming JOINT/WEIGHT sources are simply concatenated in order.)";
				}
				else daeEH::Error<<"Two or more "<<input->semantic<<" offsets in set 0.";
				else *offset = input->offset;
			}
		}
		if(0==inputs_max_offset||JOINT_offset<0||WEIGHT_offset<0||WEIGHT_array==nullptr)
		return;
		inputs_max_offset+=1; 
	  //template<> void Load_p_N<0>(const Collada05_XSD::polylist &in_i)
	  //{	
			GLuint restart = 0;
			Collada05::const_v v = vertex_weights->v;	
			Collada05::const_vcount vcount = vertex_weights->vcount;
			if(v==nullptr||0!=v->value->size()%inputs_max_offset)
			{
				daeEH::Error<<"<v> size is not a multiple of <input> offsets.";
				return;
			}		
			//<vcount> is not required, but the manual doesn't say what to do without it.
			if(vcount==nullptr) 
			{	
				GLuint restartN = v->value->size()/inputs_max_offset;
				GLuint vcount_i = restartN/vertex_weights->count;
				if(v->value->size()!=vcount_i*vertex_weights->count*inputs_max_offset)			
				daeEH::Warning<<"<vertex_weights><vcount> is absent and <vertex_weights count> does\n"
				"not agree with the <v> data stride calculated by <input offset>.\n";			
				while(restart<restartN)
				{
					Weight(v,vcount_i,restart); restart+=vcount_i;
				}
			}
			else for(size_t i=0;i<vcount->value->size();i++)
			{
				Weight(v,vcount->value[i],restart); restart+=vcount->value[i];
			}
	  //}
	}
	RT::Accessor05 WEIGHT_array;
	void Weight(Collada05::const_v &v, GLuint count, GLuint restart=0)
	{
		std::vector<RT::Skin_Weight> &buf = out->Weights; 
		
		GLuint stride = inputs_max_offset;		
		assert(stride==2);
		GLuint j = JOINT_offset+restart*stride;
		GLuint i = WEIGHT_offset+restart*stride; 
		GLuint iN = i+count*stride, sum_n = 0;				
		RT::Skin_Weight w; float sum = 0, rcp;
		float epsilon_10 = 10*std::numeric_limits<float>::epsilon();
		for(int x;i<iN;i+=stride,j+=stride)
		{
			if(1!=v->value->get1at(j,w.Joint)
			 ||1!=v->value->get1at(i,x))
			{
				daeEH::Error<<"Index into <v> is out-of-range.\n"
				"(Is <vcount> to blame?)";
				continue;
			}
			else if(1!=WEIGHT_array->value->get1at(x,w.Weight.x))
			{
				daeEH::Error<<"Index into WEIGHT <source> is out-of-range.\n";
				continue;
			}
			if(!/*NaN?*/(w.Weight.x>epsilon_10))
			{
				daeEH::Warning<<"Not rendering small vertex-weight "<<w.Weight.x;
				continue;
			}
			buf.push_back(w); sum+=w.Weight.x; sum_n++;
		}

		//COLLADA says <vertex_weights> are to be normalized if they are to be displayed.		
		if(0==sum_n)
		{
			//The RT::Controller algorithm needs a sum of 1 per vertex without omissions.
			w.Joint = -1; w.Weight.x = 1; buf.push_back(w);
		}
		else if(sum!=1)
		{
			//Assuming floating-point determinism.
			rcp = 1/sum; sum = 0;
			for(size_t i=buf.size()-count;i<buf.size();i++)
			sum+=buf[i].Weight.x*=rcp;
			if(sum>1) buf.back().Weight.x-=(sum-1)*2;
		}
	}
};
struct RT::DBase::LoadController_morph
{
	RT::Morph *out;
	LoadController_morph(daeName &src, 
	RT::Controller *con, RT::DBase *db, Collada05::const_morph in)	
	:out(dynamic_cast<RT::Morph*>(con))
	{
		src = in->source__ATTRIBUTE;
															
		//Will this shorthand work?
		//if(Collada05::morph::method_MorphMethodType::RELATIVE==in->method)
		if(in->method==in->method->RELATIVE)
		out->Using_RELATIVE_method = true;

		//COLLADA never actually supported morph animation???
		//https://www.khronos.org/collada/wiki/Morph_weights_KHR_extension		
		Collada05::const_float_array array_target;
		Collada05::const_morph::source source_target;
		
		RT::Accessor<> array(in); 
		size_t weightsN = 0, targetsN = 0;
		Collada05::const_targets targets = in->targets;		
		if(targets!=nullptr)
		for(size_t i=0;i<targets->input.size();i++)
		{
			daeName semantic = array.bind(targets->input[i]);
			
			size_t *counter;
			if("MORPH_TARGET"==semantic)
			counter = &targetsN;			
			else if("MORPH_WEIGHT"==semantic)
			counter = &weightsN;			
			else
			{
				if(!semantic.empty())
				daeEH::Error<<"Morph semantic "<<semantic<<" not supported.";
				continue;
			}

			size_t j,jN = array.accessor->count;
			if(out->MorphTargets.size()<*counter+jN)
			{
				//Reminder: there can be any number of inputs.
				out->MorphTargets.resize(*counter+jN);
			}
			RT::Morph_Target *j0 =
			&out->MorphTargets[*counter]; *counter+=jN;			
			if(&weightsN==counter)
			{					
				//NOTE: hack_morph_weight_id makes more sense. But,
				//-its support is poor, because it relies on SID-less SIDREFs,
				//-and the ID must be in the same document as the <animation><channel>.				
				array_target = array->a<Collada05::float_array>();
				source_target = //Removing RT::Accessor::source.
				dae(array)->getAncestor<Collada05::morph::source>();
				if(array_target!=nullptr) 
				for(j=0;j<jN;j++) 
				if(1!=array.get1at(j,j0[j].Weight,array_target->value))
				break;
			}
			else if(&targetsN==counter)
			{
				Collada05::const_IDREF_array 
				IDREF_array = array->a<Collada05::IDREF_array>();
				if(IDREF_array!=nullptr)
				{
					const void *id; //Avoiding daeStringRef construction.
					//Assuming the IDs are local to the containing array.
					//COLLADA is haphazardly laid out, but IDREFs aren't.
					const daeDocument *doc2 = dae(IDREF_array)->getDocument();
					for(j=0;j<jN&&1==array.get1at(j,id,IDREF_array->value);j++)
					{
						j0[j].Vertices = 
						db->LoadGeometry(doc2->idLookup<Collada05::geometry>(id));
						if(nullptr==j0[j].Vertices)
						daeEH::Error<<"Missed <morph> target. Are <controller> valid targets?";						
					}
				}
			}
			else assert(0); 
		}
		
		//SCHEDULED FOR REMOVAL (EXPONENTIAL LOOK UP)
		//check if there are animation on morph weight
		//REMINDER: ANIMATIONS SHOULD HAVE BEEN PRE-LOADED.
		RT::Animator a; size_t &j = a.Channel;
		for(size_t i=0;i<db->Animations.size();i++)					
		for(a.Animation=db->Animations[i],j=0;j<a.Animation->Channels.size();j++)
		{
			RT::Target &t = a.Animation->Channels[j].Target;
			if(source_target==t.Fragment||array_target==t.Fragment)
			out->Animators.push_back(a);
		}
	}
};
RT::Controller *RT::DBase::LoadController(Collada05::const_controller in)
{
	if(in==nullptr) return nullptr;
	RT::Controller *out = GetController(in);
	if(out!=nullptr) return out; //controller was instanced

	daeEH::Verbose<<"Adding new controller "<<in->id;
							   
	RT::Main_Asset _RAII(in);

	daeName src = nullptr; RT::Skin *skin = nullptr;
	if(!in->skin.empty())
	LoadController_skin(src,out=skin=COLLADA_RT_new(RT::Skin),in->skin);
	else if(!in->morph.empty()) 
	LoadController_morph(src,out=COLLADA_RT_new(RT::Morph),this,in->morph);	
	else return nullptr;
	
	out->Id = in->id; out->DocURI = RT::DocURI(in);
	
	xs::const_any source = xs::anyURI(src,in)->get<xs::any>();
	out->Geometry = LoadGeometry(source->a<Collada05::geometry>());
	out->Source = LoadController(source->a<Collada05::controller>());	
	if(out->Geometry==nullptr) 
	if(out->Source==nullptr)
	{
		RT::Assert("Controller source not found.");
		COLLADA_RT_delete(out);	return nullptr;
	}
	else out->Geometry = out->Source->Geometry;

	//The first skin transforms into Y_UP/1 space.
	//<morph> on <skin> is too exotic to consider.
	if(skin!=nullptr&&0!=out->Geometry->Asset
	&&dynamic_cast<RT::Skin*>(out->Source)==nullptr)
	{
		RT::Matrix m; RT::Up_Meter um = 
		RT::GetUp_Meter(out->Geometry->Asset);
		RT::MatrixLoadAsset(m,um.first,um.second);
		RT::MatrixMult(m,skin->BindShapeMatrix);
	}

	Controllers.push_back(out); return out;
}

struct RT::DBase::LoadScene_Physics
{
	//These implement COLLADA's ridiculous constraint
	//attachment binding semantics.
	int bodies_model, bodies_body, bodies_constraint;

	//This is a hierarchical body instance identifier.
	int body(){ return bodies_model<<16|bodies_body; }

	xs::anyURI URI; daeSIDREF SIDREF;
	RT::DBase *dbase;	
	Collada05::const_visual_scene visual_scene;
	Collada05::const_physics_model physics_model;
	std::vector<std::pair<int,RT::PhysicsModel*>> bodies;
	LoadScene_Physics(RT::DBase *db, Collada05::const_scene &in)
	:dbase(db),SIDREF("",in),have_bodies()
	,URI(in->instance_visual_scene->url->*"",in)		
	{		
		visual_scene = URI.get<Collada05::visual_scene>();
	}
	void Load(Collada05::const_scene &in)
	{
		if(!RT::Physics::OK||in->instance_physics_scene.empty()) 
		return;

		RT::Main.Physics.Init(); 

		Collada05::const_physics_scene physics_scene;
		for(size_t i=0;i<in->instance_physics_scene.size();i++)
		{	
			URI = in->instance_physics_scene[i]->url;
			physics_scene = URI.get<Collada05::physics_scene>();			 
			if(physics_scene==nullptr) continue;

			RT::Main_Asset _RAII(physics_scene/*->asset*/);
			#ifdef NDEBUG
			#error This is per scene gravity
			#endif
			Collada05::const_gravity gravity = physics_scene->technique_common->gravity;
			if(gravity!=nullptr)
			{
				RT::Float x,y,z; if(3==gravity->value->get3at(0,x,y,z))
				{	
					RT::Asset.Mult(x,y,z); RT::Main.Physics.SetGravity(x,y,z);	
				}
			}

			//Collision geometry depends on LoadGeometry().
			bool lg = RT::Main.LoadGeometry;
			if(!lg) RT::Main.LoadGeometry = true;
			for(size_t i=0;i<physics_scene->instance_physics_model.size();i++)
			{
				LoadInstancePhysicsModel(physics_scene->instance_physics_model[i]);
			}
			if(!lg) RT::Main.LoadGeometry = false;
		}
	}
	RT::PhysicsModel *LoadPhysicsModel(Collada05::const_physics_model in)
	{
		if(in==nullptr) return nullptr;
		RT::PhysicsModel *out = dbase->GetPhysicsModel(in);
		if(out!=nullptr) return out; //physics model was instanced
	
		RT::Main_Asset _RAII(in);
		out = new RT::PhysicsModel();
		out->Id = in->id; out->DocURI = RT::DocURI(in); 
		
		out->RigidBodies.reserve(in->rigid_body.size());
		{
			for(size_t i=0;i<in->rigid_body.size();i++)	
			{
				out->RigidBodies.push_back(RT::RigidBody());
				out->RigidBodies.back().OverrideWith(*in->rigid_body[i],URI);
			}
		}
		//Reminder: These only exist so Find_NCName/Find_non_NCName
		//can be repurposed to double for rigid_constraint look-ups.
		out->RigidConstraints.reserve(in->rigid_constraint.size());
		{
			for(size_t i=0;i<in->rigid_constraint.size();i++)	
			{
				RT::RigidConstraint c(in->rigid_constraint[i]);
				if(nullptr!=c.rigid_constraint)
				out->RigidConstraints.push_back(c);
			}
		}

		dbase->PhysicsModels.push_back(out); return out;
	}
	void LoadInstancePhysicsModel(Collada05::const_instance_physics_model in)
	{
		URI = in->url;  //Collada05::const_physics_model
		physics_model = URI.get<Collada05::physics_model>();
		RT::PhysicsModel *model =
		LoadPhysicsModel(physics_model);
		if(model==nullptr) return;

		RT::Main_Asset _RAII(model->Asset);
		RT::RigidBody *lv1; 
		Collada05::const_instance_rigid_body lv2;		
		bodies_constraint = -1;
		for(size_t i=0;i<in->instance_rigid_body.size();i++)
		{
			//body is NCName, so this should be impossible.
			//But COLLADA is a mess, especially Physics, and
			//seems to not realize it or the type was mistaken.
			//This also supports 1.5.0 and user-modified schemas.
			lv2 = in->instance_rigid_body[i];			
			lv1 = FindBody(lv2->body,model);
			if(lv1!=nullptr)
			{	
				RT::RigidBody ib = *lv1; 
				if(ib.OverrideWith(*lv2,URI)) 
				{
					if(BindNode(lv2->target,ib,lv2)) 
					continue;
				}
				else daeEH::Error<<"No collision shape for physics body.";
			}
			daeEH::Error<<"Failed to instance physics body "<<lv2->body;
		}	

		//Assuming any constraints refer to bodies internal to the
		//the same model that defines them, or to an animated node.
		for(size_t i=0;i<in->instance_rigid_constraint.size();i++)
		{
			//<instance_rigid_constraint> is almost pointless. Presumably
			//it's required, and selectively enables/disables constraints.
			//It has a "sid" but it seems useless outside of <extra> data.
			daeName constraint = in->instance_rigid_constraint[i]->constraint;
			bodies_constraint = -1;
			RT::RigidConstraint *p;
			if(constraint.getID_slashed()) //FindConstraint(constraint,model);
			p = Find_non_NCName(&RT::PhysicsModel::RigidConstraints,constraint,model);
			else p = Find_NCName(&RT::PhysicsModel::RigidConstraints,constraint,model);			
			if(p==nullptr) 
			{
				daeEH::Error<<"Failed to instantiate Physics constraint "<<constraint;
				continue;
			}

			//Not sure what <asset> space applies to <rigid_constraint>. 
			//1) Could be the body/node on either attachment (complicated.)
			//2) Here using the <asset> of the contraint's <physics_model>.
			RT::Main_Asset _RAII2((bodies_model<0?model:bodies[bodies_model].second)->Asset);

			//UNTESTED
			//Because <instance_rigid_constraint> cannot bind to anything
			//this is supposed to limit searches for bodies to containing
			//physics models.
			bodies_constraint = bodies_model;			
			#ifdef NDEBUG
			#error Implement <node> attachments. 
			#endif
			if(nullptr!=FindBody(p->rigid_constraint->ref_attachment->rigid_body->getID_id(),model))
			{
				int body1 = body();
				if(nullptr!=FindBody(p->rigid_constraint->attachment->rigid_body->getID_id(),model))
				{
					RT::Main.Physics.Bind_instance_rigid_constraint(body1,body(),p->rigid_constraint);	
				}
			}
		}

	//From here down, "bodies" holds subordinate physics models, which are not
	//looked for until a body cannot be found in the top-level's physics model.

		if(have_bodies) bodies.clear(); have_bodies = false;
	}
	bool have_bodies; void CacheBodies()
	{
		have_bodies = true; assert(bodies.empty());
		
		//bodies is sorted by depth to meet the SIDREF-like "beadth-first" requirement.
		//Note, that the scenarios in which it is used are not necessarily SIDREF-like.
		CacheBodies_recursive(physics_model,1); std::sort(bodies.begin(),bodies.end());
	}
	RT::PhysicsModel *CacheBodies_recursive(Collada05::const_physics_model in, int depth, int parent=0)
	{
		if(in!=nullptr) for(size_t i=0;i<in->instance_physics_model.size();i++)
		{
			RT::PhysicsModel *p; URI = in->instance_physics_model[i]->url; 
			p = CacheBodies_recursive(URI.get<Collada05::physics_model>(),depth+1,i);
			if(p!=nullptr) bodies.push_back(std::make_pair(depth<<16|parent,p));
		}
		return LoadPhysicsModel(in);
	}
	RT::RigidBody *FindBody(const daeName &body, RT::PhysicsModel *p)
	{
		if(!body.getID_slashed())
		return Find_NCName(&RT::PhysicsModel::RigidBodies,body,p);
		return Find_non_NCName(&RT::PhysicsModel::RigidBodies,body,p);
	}
	template<class T> T *Find_NCName
	(std::vector<T> RT::PhysicsModel::*p2m, const daeName &body, RT::PhysicsModel *p)
	{
		size_t j = bodies_constraint;
		if(-1==bodies_constraint) goto p0; 
		for(;j<bodies.size();j++)
		{
			p = bodies[j].second; p0: //p0
			for(size_t i=0;i<(p->*p2m).size();i++)
			if(body.string==(p->*p2m)[i].Sid)
			{
				bodies_model = (int)j; bodies_body = (int)i;
				return &(p->*p2m)[i];
			}
			if(!have_bodies) CacheBodies();
		}
		return nullptr;
	}
	template<class T> T *Find_non_NCName
	(std::vector<T> RT::PhysicsModel::*p2m, const daeName &body, RT::PhysicsModel *pp)
	{
		if(!have_bodies) CacheBodies(); SIDREF.setSIDREF(body);
		if('.'==body[0]) SIDREF.setParentObject(physics_model);

		daeSIDREFRequest req;
		if(!SIDREF.get(req)||nullptr==req->a<typename T::type>())
		return nullptr;
		
		for(size_t i=req.SID_by_SID.size()-1;i-->0;)
		switch(req.SID_by_SID[i]->getElementType())
		{
		case DAEP::Schematic<Collada05::instance_physics_model>::genus:
		req.SID_by_SID[i] = req.SID_by_SID[i+1]->getAncestor<Collada05::physics_model>();		
		case DAEP::Schematic<Collada05::physics_model>::genus: break;
		default: req.SID_by_SID[i] = nullptr;
		}
		req.SID_by_SID.pop_back();
		
		RT::PhysicsModel *p = pp; int depth = 0, parent = 0;
		for(int sid=0,j=bodies_constraint;j<(int)bodies.size();j++)
		{
			if(sid==req.SID_by_SID.size())
			{
				for(size_t i=0;i<(p->*p2m).size();i++)
				if(body.string==(p->*p2m)[i].Sid)
				{
					bodies_model = p==pp?-1:(int)j; bodies_body = (int)i;
					return &(p->*p2m)[i];
				}
				return nullptr;
			}

			//SCHEDULED FOR REMOVAL
			//This merely converts a DOM element into an RT handle.
			RT::PhysicsModel *q =
			LoadPhysicsModel(req.SID_by_SID[sid++]->a<Collada05::physics_model>());
			if(q==p||q==nullptr) continue;

			//0xFFFF tells the compiler to use two-byte addressing.
			for(size_t jj=j;j<(int)bodies.size()&&bodies[j].first>>16==(depth&0xFFFF);j++)
			if(p==bodies[j].second&&(depth<<16)+parent==bodies[j].first)
			{
				parent = int(j-jj); break;
			}							
			depth++;
		}
		assert(0); return nullptr;
	}	 

	//COLLADA 1.4.1/1.5 make this ridiculous.
	bool BindNode(const daeStringRef &target,
	RT::RigidBody &ib, Collada05::const_instance_rigid_body &iv)
	{
		const daeStringRef &id = target.getID_id();
		daeName fragment = id;
		const daeDocument *doc,*doc2 =
		dae(visual_scene)->getDocument();
		if('#'!=target[0])
		{
			URI.setURI_and_resolve(target);
			fragment = URI.erase_fragment();			
			doc = URI.isSameDocumentReference()
			?doc2:RT::Main.DOM.getDoc(URI)->getDocument();			
		}
		else doc = doc2;
		if(doc==nullptr) return false;
		daeSIDREFRequest req;
		Collada05::const_node node;				
		if('#'!=target[0]||target.getID_slashed())
		{
			SIDREF.setSIDREF(fragment);
			SIDREF.setParentObject(doc==doc2?
			(const DAEP::Object*)visual_scene:doc);			
			if(!SIDREF.get(req)) return false;
			for(size_t i=0;i<req.SID_by_SID.size();i++)		
			switch(req.SID_by_SID[i]->getElementType())
			{
			case DAEP::Schematic<Collada05::instance_node>::genus:
			req.SID_by_SID[i] = req.SID_by_SID[i]->getParentElement();			
			case DAEP::Schematic<Collada05::node>::genus: break;
			default: req.SID_by_SID[i] = nullptr;
			}
			node = req->a<Collada05::node>();
			req.SID_by_SID.pop_back();
		}
		else if(doc->idLookup(id,node)==nullptr) //optimizing
		{
			dae(visual_scene)->sidLookup(id,node);
		}
		#ifdef NDEBUG
		#error Does <instance_node> make sense?
		#endif
		if(node==nullptr) return nullptr;
		
		bool initial_velocity = 
		!iv->technique_common.empty()
		&&(!iv->technique_common->velocity.empty()
		||!iv->technique_common->angular_velocity.empty());

		int o = 0;
		#ifdef NDEBUG
		#error This is an exponential lookup.
		#endif
		for(size_t i=0;i<RT::Main.Stack.Data.size();i++)
		{
			//Fragment could be removed if daeDatabse
			//is set up to integrate these RT classes.
			RT::Stack_Data &d = RT::Main.Stack.Data[i];			
			if(node!=d.Node->Fragment) 
			continue;
			
			//Eliminate the candidate if any SIDs are 
			//not among its parent nodes in the stack.
			RT::Stack_Data *p = d.Parent;
			for(size_t i=req.SID_by_SID.size();i-->0;)
			if(nullptr!=req.SID_by_SID[i])
			while(p!=nullptr&&p->Node->Fragment!=req.SID_by_SID[i])
			p = p->Parent;
			if(p==nullptr) continue;
			
			if(o++!=0)
			daeEH::Warning<<"Binding physics body to many instanced nodes "<<target;
			RT::Main.Physics.Bind_instance_rigid_body(body(),ib,d);
			if(initial_velocity)
			RT::Main.Physics.Init_velocity(d,iv);
		}
		return o!=0;
	}
};
void RT::DBase::LoadScene(Collada05::const_scene scene)
{	
	//Worried about initializing daeRef with a nullptr.
	if(scene==nullptr) return;
	//Just using the URI and Physics needs the visual_scene.
	LoadScene_Physics Physics(this,scene); 	
	Collada05::const_visual_scene &Visuals = Physics.visual_scene;
	if(nullptr==Visuals) return;

	//get the scene name 
	RT::Main.Scene.Name = Visuals->name;
	//Probably shouldn't.
	RT::Main.Scene.Id = Visuals->id;
	
	daeEH::Verbose<<"Loading Collada Scene "<<Visuals->name;

	RT::Main_Asset _RAII(Visuals->asset);
	//recurse through the scene, read and add nodes 
	for(size_t i=0;i<Visuals->node.size();i++)
	LoadNode(Visuals->node[i],-1,&RT::Main.Scene);		
	
	//Physics depends on this step.
	//The "stack" holds the scene-graph nodes-
	//which are distinct from the DOM's nodes.	
	RT::Main.Stack.Select(&RT::Main.Scene);
	{
		if(RT::Main.UsePhysics) Physics.Load(scene);	
	}
}

struct RT::DBase::LoadInstances_of
{
	RT::Node *out; RT::DBase *dbase;
	RT::Geometry *geom;
	LoadInstances_of(RT::DBase *db, RT::Node *out, Collada05::const_node in)	
	:dbase(db),out(out),geom()
	{
		//Process <instance_*> children.
		Load(out->Geometries,in->instance_geometry);
		Load(out->Controllers,in->instance_controller);
		Load(out->Lights,in->instance_light);
		Load(out->Cameras,in->instance_camera);
	}
	template<class S, class T> void Load(std::vector<S*> &iv, T &in)
	{
		for(size_t i=0;i<in.size();i++)
		{
			S *ii = Load_i(*in[i]); if(ii!=nullptr) iv.push_back(ii);
		}
	}
	RT::Light *Load_i(const Collada05_XSD::instance_light &in)
	{
		return dbase->LoadLight(xs::anyURI(in.url)->get<Collada05::light>());
	}
	RT::Camera *Load_i(const Collada05_XSD::instance_camera &in)
	{
		return dbase->LoadCamera(xs::anyURI(in.url)->get<Collada05::camera>());
	}	
	RT::Geometry_Instance *Load_i(const Collada05_XSD::instance_geometry &in)
	{
		geom = dbase->
		LoadGeometry(xs::anyURI(in.url).get<Collada05::geometry>());
		if(geom==nullptr) 
		return nullptr;

		RT::Geometry_Instance *out = COLLADA_RT_new(RT::Geometry_Instance);
		out->Geometry = geom;

		Collada05::const_bind_material::technique_common
		technique_common = in.bind_material->technique_common;
		if(technique_common!=nullptr)
		for(size_t i=0;i<technique_common->instance_material.size();i++)
		Load_i_material(out->Materials,technique_common->instance_material[i]);

		return out;
	}
	RT::Controller_Instance *Load_i(const Collada05_XSD::instance_controller &in)
	{
		xs::anyURI URI = in.url;
		RT::Controller *con = dbase->
		LoadController(URI.get<Collada05::controller>());
		if(con==nullptr) //controller not found
		return nullptr;

		daeEH::Verbose<<"Instancing Controller "<<in.name;

		RT::Controller_Instance *out = COLLADA_RT_new(RT::Controller_Instance);
		out->Controller = con; geom = con->Geometry;
	
		Collada05::const_bind_material::technique_common 
		technique_common = in.bind_material->technique_common;
		if(technique_common!=nullptr)
		for(size_t j=0;j<technique_common->instance_material.size();j++)
		Load_i_material(out->Materials,technique_common->instance_material[j]);

		#ifdef NDEBUG
		#error Support #SIDREF skeletons. And track <instance_node>?
		#endif	
		out->Skeletons = in.skeleton.size();
		out->Joints.resize(out->Skeletons);
		for(size_t i=0;i<out->Skeletons;i++)
		{
			URI = in.skeleton[i]->value;
			Collada05::const_node skel = URI.get<Collada05::node>();
			//This is cleaned up after.
			if(skel==nullptr) continue;

			out->Joints[i] = dbase->LoadNode(skel);

			Collada05::const_node joint;
			const daeDocument *doc = dae(skel)->getDocument();

		//TODO? Warn if joints are bound more than once.
		//TODO? Warn if a skeleton has no joints inside.

			size_t jN = out->Skeletons;
			for(RT::Controller *p=con;p!=nullptr;p=p->Source)
			{	
				RT::Skin *skin = dynamic_cast<RT::Skin*>(p);
				if(skin==nullptr)
				continue;

				//+1 and 1+ account for a <skeleton> entry.
				size_t j = jN; jN+=skin->Joints.size();
				if(jN>out->Joints.size()) 
				out->Joints.resize(jN);				
				for(size_t i=0;i<skin->Joints.size();i++)
				{
					//sidLookup does not follow <instance_node> joints.
					//These SIDs are xs:Name, so they cannot be SIDREFs.
					const void *ref = skin->Joints[i];
					RT::Node *found = dbase->
					LoadNode(skin->Using_IDs_to_FindJointNode?
					doc->idLookup(ref,joint):dae(skel)->sidLookup(ref,joint,doc));
					if(found==nullptr) continue;
					out->Joints[j+i] = found;				
					daeEH::Verbose<<"Skin Controller "<<skin->Id<<" joint binding made to node "<<skin->Joints[i];
				}

				for(size_t i=0;i<skin->Joints.size();i++)
				if(nullptr==out->Joints[j+i])
				daeEH::Warning<<"Failed to make joint binding for Controller "<<skin->Id<<" for Joint "<<skin->Joints[i];	
			}
		}
		for(size_t i=0;i<out->Skeletons;i++) if(nullptr==out->Joints[i])		
		{									
			out->Joints.erase(out->Joints.begin()+i); out->Skeletons--; i--;
		}
		return out;
	}
	void Load_i_material(std::vector<RT::Material_Instance> &out, Collada05::const_instance_material in)
	{
		RT::Material_Instance im;
		im.Material = dbase->LoadMaterial(xs::anyURI(in->target).get<Collada05::material>());				
		if(im.Material==nullptr) return;
		
		im.Symbol = in->symbol; out.push_back(im);

		//Adding this to help debug demo.dae.
		for(size_t i=0;i<geom->Elements.size();i++)			
		if(im.Symbol==geom->Elements[i].Material_Instance_Symbol)
		return;
		daeEH::Warning<<"Did not find material symbol "<<im.Symbol<<" in geometry "<<geom->Id;
	}
};
struct RT::DBase::LoadTransforms_of
{
	RT::DBase *dbase; RT::Node *out;
	LoadTransforms_of(RT::DBase *db, RT::Node *out, Collada05::const_node in)
	:dbase(db),out(out)
	{
		//This preserves the transforms' order of appearance and application.
		in->content->for_each_child(*this);

		//SCHEDULED FOR REMOVAL (EXPONENTIAL LOOK UP)
		//check if there are animation on the transform
		//REMINDER: ANIMATIONS SHOULD HAVE BEEN PRE-LOADED.
		RT::Animator a; size_t &j = a.Channel;		
		for(size_t i=0;i<dbase->Animations.size();i++)					
		for(a.Animation=dbase->Animations[i],j=0;j<a.Animation->Channels.size();j++)
		{
			RT::Target &t = a.Animation->Channels[j].Target;
			if(dae(in)!=&dae(t.Fragment)->getParentObject()) 
			continue;		
			//This is mapping the animations directly to the
			//packed transform data so the animation code can
			//ignore the transforms.
			short tf = 0;
			for(size_t i=0;i<out->Transforms.size();i++)			
			if(out->Transforms[i].Fragment==t.Fragment)
			{
				t.Min+=tf; 
				t.Max+=tf;
				out->Animators.push_back(a); break;
			}
			else tf+=out->Transforms[i].Size;
		}
	}
	void operator()(const xs::const_any &e)
	{
		switch(e->getElementType()) //ORDER-IS-IMPORTANT
		{
		#define _(x) \
		case DAEP::Schematic<Collada05::x>::genus:\
			out->Transforms.push_back\
			(RT::Transform_##x(e->a<Collada05::const_##x>()));\
			break;
		_(rotate)_(translate)_(scale)_(lookat)_(skew)_(matrix)
		#undef _
		}			
	}
};
RT::Node *RT::DBase::LoadNode(Collada05::const_node in, int i, RT::Node *node)
{
	if(in==nullptr) return nullptr;

	//REMINDER: GetNode is broken insofar as it is not document aware.
	RT::Node *out = nullptr;
	if(!in->id->empty())
	{
		out = GetNode(in); if(out!=nullptr)
		{
			//2017: A <skeleton> reference can cause a <node> to be loaded
			//before its root node will be. The <skeleton> may also come from
			//an outside document.
			if(node!=nullptr)
			node->Nodes.push_back(out);
			return out;
		}
	}

	RT::Main_Asset _RAII(in->asset);
	//If <instance_node> recursively retrieve
	//the <asset> <up_axis> and <unit> values.
	if(-1!=i) RT::Asset = in;	

	daeEH::Verbose<<"Loading Scene Node "<<in->id;	

	out = COLLADA_RT_new(RT::Node);	
	out->Name = in->name;
	out->Id = in->id;
	out->DocURI = RT::DocURI(in); 
	out->Sid = in->sid;	
	//SCHEDULED FOR REMOVAL
	out->Fragment = in;
	LoadTransforms_of(this,out,in);
	LoadInstances_of(this,out,in);

	//add to parent 
	if(node!=nullptr)
	node->Nodes.push_back(out);

	//read children 
	for(size_t i=0;i<in->node.size();i++)
	{
		LoadNode(in->node[i],-1,out);	
	}
	xs::anyURI URI; 
	for(size_t i=0;i<in->instance_node.size();i++)
	{
		URI = in->instance_node[i]->url;			
		LoadNode(URI.get<Collada05::node>(),(int)i,node);	
	}

	Nodes.push_back(out); return out;
}

//-------.
	}//<-'
}

/*C1071*/