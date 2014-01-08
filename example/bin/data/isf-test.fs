/*{
	"DESCRIPTION": "RGB color trail + noise",
	"CREDIT": "by satoruhiga",
	"CATEGORIES": [
		"TEST-GLSL FX"
	],
	"INPUTS": [
		{
			"NAME": "inputImage",
			"TYPE": "image"
		},
		{
			"NAME": "blurAmount",
			"TYPE": "float"
		}
	],
	"PERSISTENT_BUFFERS": [
		"bufferVariableNameA"
	],
	"PASSES": [
		{
			"TARGET":"bufferVariableNameA"
		}
	]
}*/

void main()
{
	vec4 freshPixel = IMG_THIS_PIXEL(inputImage);
	vec4 stalePixel = IMG_THIS_PIXEL(bufferVariableNameA);
	gl_FragColor = mix(freshPixel, stalePixel, blurAmount);
	gl_FragColor.rg = mix(gl_FragColor.rg, stalePixel.rg, (sin(TIME * 0.1) / 6.283) * 0.2 + 0.8);
	gl_FragColor.rb = mix(gl_FragColor.rb, stalePixel.rb, (sin(TIME * 0.13) / 6.283) * 0.2 + 0.8);
	gl_FragColor.ba = mix(gl_FragColor.ba, stalePixel.ba, (sin(TIME * 0.16) / 6.283) * 0.2 + 0.8);
}
