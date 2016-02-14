/*{
	"CREDIT": "by zoidberg",
	"CATEGORIES": [
		"Halftone Effect"
	],
	"INPUTS": [
		{
			"NAME": "inputImage",
			"TYPE": "image"
		},
		{
			"NAME": "gridSize",
			"TYPE": "float",
			"MIN": 1.0,
			"MAX": 256.0,
			"DEFAULT": 45.0
		},
		{
			"NAME": "dotSize",
			"TYPE": "float",
			"MIN": 0.0,
			"MAX": 1.2,
			"DEFAULT": 1.0
		},
		{
			"NAME": "smoothing",
			"TYPE": "float",
			"MIN": 0.0,
			"MAX": 1.0,
			"DEFAULT": 0.15
		}
	]
}*/

vec4		gridRot = vec4(15.0, 45.0, 75.0, 0.0);
//vec4		gridRot = vec4(15.0, 75.0, 0.0, 45.0);
//vec4		gridRot = vec4(0.0, 0.0, 0.0, 0.0);

vec2		originOffsets[4];

void main() {
	originOffsets[0] = vec2(-1.0, 0.0);
	originOffsets[1] = vec2(0.0, 1.0);
	originOffsets[2] = vec2(1.0, 0.0);
	originOffsets[3] = vec2(0.0, -1.0);

	vec3 rgbAmounts = vec3(0.0);
	int i;
	int j;

	for (i=0; i<3; ++i)	{
		float rotRad = radians(gridRot[i]);

		mat2 ccTrans = mat2(vec2(cos(rotRad), sin(rotRad)), vec2(-1.0*sin(rotRad), cos(rotRad)));
		mat2 cTrans = mat2(vec2(cos(rotRad), -1.0*sin(rotRad)), vec2(sin(rotRad), cos(rotRad)));

		vec2 gridFragLoc = cTrans * gl_FragCoord.xy;
		vec2 gridOriginLoc = vec2(floor(gridFragLoc.x/gridSize), floor(gridFragLoc.y/gridSize));

		vec2 tmpGridCoords = gridFragLoc/vec2(gridSize);
		bool fragAtTopOfGrid = ((tmpGridCoords.y-floor(tmpGridCoords.y)) > (gridSize/2.0)) ? true : false;
		bool fragAtRightOfGrid = ((tmpGridCoords.x-floor(tmpGridCoords.x)) > (gridSize/2.0)) ? true : false;
		if (fragAtTopOfGrid)
			gridOriginLoc.y = gridOriginLoc.y + 1.0;
		if (fragAtRightOfGrid)
			gridOriginLoc.x = gridOriginLoc.x + 1.0;

		vec2 gridDotLoc = vec2(gridOriginLoc.x*gridSize, gridOriginLoc.y*gridSize) + vec2(gridSize/2.0);
		vec2 renderDotLoc = ccTrans * gridDotLoc;
		vec4 renderDotImageColorRGB = IMG_PIXEL(inputImage, renderDotLoc);

		float imageChannelAmount = renderDotImageColorRGB[i];
		//	the size of the dot is determined by the value of the channel
		float		dotRadius = imageChannelAmount * (gridSize*1.50/2.0) * dotSize;
		float		fragDistanceToDotCenter = distance(gl_FragCoord.xy, renderDotLoc);
		if (fragDistanceToDotCenter < dotRadius)	{
			rgbAmounts[i] += smoothstep(dotRadius, dotRadius-(dotRadius*smoothing), fragDistanceToDotCenter);
		}

		//	calcluate the size of the dots abov/below/to the left/right to see if they're overlapping
		for (j=0; j<4; ++j)	{
			gridDotLoc = vec2((gridOriginLoc.x+originOffsets[j].x)*gridSize, (gridOriginLoc.y+originOffsets[j].y)*gridSize) + vec2(gridSize/2.0);
			renderDotLoc = ccTrans * gridDotLoc;
			renderDotImageColorRGB = IMG_PIXEL(inputImage, renderDotLoc);

			imageChannelAmount = renderDotImageColorRGB[i];
			dotRadius = imageChannelAmount * (gridSize * 1.50/2.0);
			fragDistanceToDotCenter = distance(gl_FragCoord.xy, renderDotLoc);
			if (fragDistanceToDotCenter < dotRadius)	{
				rgbAmounts[i] += smoothstep(dotRadius, dotRadius-(dotRadius*smoothing), fragDistanceToDotCenter);
			}
		}
	}

	gl_FragColor = vec4(rgbAmounts[0], rgbAmounts[1], rgbAmounts[2], 1.0);
}
