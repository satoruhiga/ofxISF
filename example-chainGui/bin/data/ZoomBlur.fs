/*{
	"DESCRIPTION": "Zoom Blur",
	"CREDIT": "by satoruhiga",
	"CATEGORIES": [
		"GLSL FX"
	],
	"INPUTS": [
		{
			"NAME": "inputImage",
			"TYPE": "image"
		},
		{
			"NAME": "zoom",
			"TYPE": "float",
			"DEFAULT": 3,
			"MIN": -50,
			"MAX": 50
		},
		{
			"NAME": "feedback",
			"TYPE": "float",
			"DEFAULT": 0.95,
			"MIN": 0,
			"MAX": 1
		},
		{
			"NAME": "dry",
			"TYPE": "float",
			"DEFAULT": 0.0,
			"MIN": 0,
			"MAX": 1
		}
	],
	"PERSISTENT_BUFFERS": [
		"accum"
	],
	"PASSES": [
		{
			"TARGET":"accum"
		},
		{
		}
	]
}*/

void main()
{
	vec4 A = IMG_THIS_PIXEL(inputImage);
	
	if (PASSINDEX == 0)
	{
		vec2 this_pixel = vv_FragNormCoord * RENDERSIZE;
		vec2 center = RENDERSIZE / 2.;
		vec2 vec = normalize(center - this_pixel);
		
		vec4 B = IMG_PIXEL(accum, this_pixel + vec * zoom);

		gl_FragColor.rgb = (A.rgb * A.a * (1. - feedback)) + (B.rgb * B.a * feedback);
		gl_FragColor.a = 1.0;
	}
	else if (PASSINDEX == 1)
	{
		vec4 B = IMG_THIS_PIXEL(accum);

		gl_FragColor.rgb = (A.rgb * dry) + (B.rgb);
		gl_FragColor.a = 1.0;
	}
}
