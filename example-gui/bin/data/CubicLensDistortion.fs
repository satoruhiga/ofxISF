/*{
	"DESCRIPTION": "Cubic Lens Distortion",
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
			"NAME": "kcube",
			"TYPE": "float",
			"DEFAULT": 1.25,
			"MIN": 0.5,
			"MAX": 2.5
		},
		{
			"NAME": "k",
			"TYPE": "float",
			"DEFAULT": -0.15,
			"MIN": -1,
			"MAX": 3
		}
	]
}*/

float aspect2 = pow(RENDERSIZE.x / RENDERSIZE.y, 2.);
vec2 half_norm = vec2(0.5, 0.5);

vec2 distort(vec2 uv)
{
	float r2 = aspect2 * uv.x*uv.x + uv.y*uv.y;
	float f = 1. + r2 * (k + kcube * sqrt(r2));
	return f * uv;
}

void main()
{

	vec2 uv = distort(vv_FragNormCoord - half_norm);
	vec2 uv0 = distort(-half_norm);

	float s = length(half_norm) / length(uv0);

	gl_FragColor = IMG_PIXEL(inputImage, (s * uv + half_norm) * RENDERSIZE);
}
