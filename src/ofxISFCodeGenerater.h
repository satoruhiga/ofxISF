#pragma once

#include "Poco/RegularExpression.h"

#include "ofxISFConstants.h"
#include "ofxISFParams.h"

OFX_ISF_BEGIN_NAMESPACE

#define _S(src) # src

class ImageDecl
{
public:

	ImageDecl() {}
	ImageDecl(const Ref_<ImageParam> &param) : param(param) {}

	string getImgThisPixelString() const
	{
		string s;
		if (param->isRectangleTexture())
			s = "IMG_THIS_PIXEL_RECT($IMAGE$, _$IMAGE$_pct)";
		else
			s = "IMG_THIS_PIXEL_2D($IMAGE$, _$IMAGE$_pct)";
		ofStringReplace(s, "$IMAGE$", param->getName());
		return s;
	}

	string getImgThisNormPixelString() const
	{
		string s;
		if (param->isRectangleTexture())
			s = "IMG_THIS_NORM_PIXEL_RECT($IMAGE$, _$IMAGE$_pct)";
		else
			s = "IMG_THIS_NORM_PIXEL_2D($IMAGE$, _$IMAGE$_pct)";
		ofStringReplace(s, "$IMAGE$", param->getName());
		return s;
	}

	string getImgPixlString() const
	{
		string s;
		if (param->isRectangleTexture())
			s = "IMG_PIXEL_RECT($IMAGE$, _$IMAGE$_pct,";
		else
			s = "IMG_PIXEL_2D($IMAGE$, _$IMAGE$_pct,";
		ofStringReplace(s, "$IMAGE$", param->getName());
		return s;
	}

	string getImgNormPixelString() const
	{
		string s;
		if (param->isRectangleTexture())
			s = "IMG_NORM_PIXEL_RECT($IMAGE$, _$IMAGE$_pct,";
		else
			s = "IMG_NORM_PIXEL_2D($IMAGE$, _$IMAGE$_pct,";
		ofStringReplace(s, "$IMAGE$", param->getName());
		return s;
	}

protected:

	Ref_<ImageParam> param;
};

class CodeGenerator
{
public:

	CodeGenerator(Params &params) : params(params) {}

	bool generate(const string& isf_glsl_code)
	{
		if (!generate_shader(isf_glsl_code)) return false;

		return true;
	}

	const string& getVertexShader() const { return vert; }
	const string& getFragmentShader() const { return frag; }
	
	void dumpShader() const
	{
		cout << "vert: " << endl;
		cout << vert << endl;
		cout << "===" << endl;
		
		cout << "frag: " << endl;
		cout << frag << endl;
		cout << "===" << endl;
	}

protected:

	Params &params;

	string vert;
	string frag;

protected:

	bool generate_shader(const string& isf_glsl_code)
	{
		const vector<Ref_<ImageParam> >& images = params.getImageParams();
		map<string, ImageDecl> image_decls;

		for (int i = 0; i < images.size(); i++)
		{
			const Ref_<ImageParam> &param = images[i];
			image_decls[param->getName()] = ImageDecl(param);
		}

		string isf_source = isf_glsl_code;
		if (!process_lookup_macro(isf_source, image_decls)) return false;

		string uniforms;
		for (int i = 0; i < params.size(); i++)
		{
			Param::Ref o = params.getParam(i);
			uniforms += o->getUniform() + "\n";
		}

		{
			vert = _S(
				uniform int PASSINDEX;
				uniform vec2 RENDERSIZE;
				varying vec2 vv_FragNormCoord;

				void vv_vertShaderInit(void)
				{
					gl_Position = ftransform();
					vv_FragNormCoord = vec2(gl_MultiTexCoord0.x, gl_MultiTexCoord0.y);
				}

				$UNIFORMS$

				void main(void)
				{
					vv_vertShaderInit();
				}
			);

			ofStringReplace(vert, "$UNIFORMS$", uniforms);
		}

		{
			frag = _S(
				uniform int PASSINDEX;
				uniform vec2 RENDERSIZE;
				varying vec2 vv_FragNormCoord;
				uniform float TIME;

				$UNIFORMS$

				vec4 IMG_NORM_PIXEL_2D(sampler2D sampler, vec2 pct, vec2 normLoc)
				{
					vec2 coord = normLoc;
					coord.y = 1. - coord.y;
					return texture2D(sampler, coord * pct);
				}
				vec4 IMG_PIXEL_2D(sampler2D sampler, vec2 pct, vec2 loc)
				{
					return IMG_NORM_PIXEL_2D(sampler, pct, loc / RENDERSIZE);
				}
				vec4 IMG_THIS_NORM_PIXEL_2D(sampler2D sampler, vec2 pct)
				{
					vec2 coord = vv_FragNormCoord;
					coord.y = 1. - coord.y;
					return texture2D(sampler, coord * pct);
				}
				vec4 IMG_THIS_PIXEL_2D(sampler2D sampler, vec2 pct)
				{
					return IMG_THIS_NORM_PIXEL_2D(sampler, pct);
				}
				vec4 IMG_NORM_PIXEL_RECT(sampler2DRect sampler, vec2 pct, vec2 normLoc)
				{
					vec2 coord = normLoc;
					coord.y = 1. - coord.y;
					return texture2DRect(sampler, coord * RENDERSIZE);
				}
				vec4 IMG_PIXEL_RECT(sampler2DRect sampler, vec2 pct, vec2 loc)
				{
					return IMG_NORM_PIXEL_RECT(sampler, pct, loc / RENDERSIZE);
				}
				vec4 IMG_THIS_NORM_PIXEL_RECT(sampler2DRect sampler, vec2 pct)
				{
					vec2 coord = vv_FragNormCoord;
					coord.y = 1. - coord.y;
					return texture2DRect(sampler, coord * RENDERSIZE);
				}
				vec4 IMG_THIS_PIXEL_RECT(sampler2DRect sampler, vec2 pct)
				{
					return IMG_THIS_NORM_PIXEL_RECT(sampler, pct);
				}

				$ISF_SOURCE$
			);

			ofStringReplace(frag, "$UNIFORMS$", uniforms);
			ofStringReplace(frag, "$ISF_SOURCE$", isf_source);
		}

		return true;
	}

	bool process_lookup_macro(string& isf_source, map<string, ImageDecl> &image_decls)
	{
		{
			string pattern = "(IMG_THIS_PIXEL|IMG_THIS_NORM_PIXEL)\\s*\\(\\s*(.*?)\\s*\\)";

			Poco::RegularExpression re(pattern, Poco::RegularExpression::RE_NEWLINE_ANY);
			Poco::RegularExpression::Match m;
			m.offset = 0;

			while (0 != re.match(isf_source, m.offset, m))
			{
				string found(isf_source, m.offset, m.length);

				string lookup_name = found;
				re.subst(lookup_name, "$1");

				string image_name = found;
				re.subst(image_name, "$2");

				map<string, ImageDecl>::iterator it = image_decls.find(image_name);
				if (it == image_decls.end())
				{
					ofLogError("ofxISF::CodeGenerator") << "image name mismatch: " << image_name;
					return false;
				}

				ImageDecl &image_decl = it->second;
				string replace_string;

				if (lookup_name == "IMG_THIS_PIXEL")
				{
					replace_string = image_decl.getImgThisPixelString();
				}
				else if (lookup_name == "IMG_THIS_NORM_PIXEL")
				{
					replace_string = image_decl.getImgThisNormPixelString();
				}
				else
				{
					throw "unknown error";
				}

				isf_source.replace(m.offset, m.length, replace_string);

				m.offset += replace_string.size();
			}
		}

		{
			string pattern = "(IMG_PIXEL|IMG_NORM_PIXEL)\\s*\\(\\s*(.*?)\\s?,";

			Poco::RegularExpression re(pattern, Poco::RegularExpression::RE_NEWLINE_ANY);
			Poco::RegularExpression::Match m;
			m.offset = 0;

			while (0 != re.match(isf_source, m.offset, m))
			{
				string found(isf_source, m.offset, m.length);

				string lookup_name = found;
				re.subst(lookup_name, "$1");

				string image_name = found;
				re.subst(image_name, "$2");

				map<string, ImageDecl>::iterator it = image_decls.find(image_name);
				if (it == image_decls.end())
				{
					ofLogError("ofxISF::CodeGenerator") << "image name mismatch: " << image_name;
					return false;
				}

				ImageDecl &image_decl = it->second;
				string replace_string;

				if (lookup_name == "IMG_PIXEL")
				{
					replace_string = image_decl.getImgPixlString();
				}
				else if (lookup_name == "IMG_NORM_PIXEL")
				{
					replace_string = image_decl.getImgNormPixelString();
				}
				else
				{
					throw "unknown error";
				}

				isf_source.replace(m.offset, m.length, replace_string);

				m.offset += replace_string.size();
			}
		}
		
		return true;
	}
};

#undef _S

OFX_ISF_END_NAMESPACE