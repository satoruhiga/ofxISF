/*{
    "CREDIT": "by tado",
    "CATEGORIES" : [
        "water",
        "noise"
    ],
    "INPUTS" : [
        {
            "NAME": "inputImage",
            "TYPE": "image"
        },
		{
			"NAME": "tiling",
			"LABEL": "Ripple Speed",
			"TYPE": "float",
			"MIN": 0.0,
			"MAX": 3.0,
			"DEFAULT": 1.0
		},
		{
			"NAME": "distort",
			"LABEL": "Ripple Speed",
			"TYPE": "float",
			"MIN": 0.5,
			"MAX": 1.5,
			"DEFAULT": 1.0
		}
  ]
}
*/

#define TAU 6.28318530718
#define MAX_ITER 6

void main()
{
	float time = TIME * .5 + 23.0;
	vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;

	vec2 p = mod(uv * TAU * tiling, TAU) - 250.0;
	vec2 i = vec2(p);
	vec3 col;
	float c = 1.0;
	float inten = .005;

	for (int n = 0; n < MAX_ITER; n++){
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) * distort + sin(t + i.y) * distort, sin(t - i.y) * distort + cos(t + i.x) * distort);
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(MAX_ITER);
	c = 1.17 - pow(c, 1.4);
	vec3 colour;
	colour.r = pow(abs(c), 14.0);

	for (int n = 0; n < MAX_ITER; n++){
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) * distort + sin(t + i.y) * distort, sin(t - i.y) * distort + cos(t + i.x) * distort);
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(MAX_ITER);
	c = 1.17 - pow(c, 1.4);
	colour.g = pow(abs(c), 14.0);

	for (int n = 0; n < MAX_ITER; n++){
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) * distort + sin(t + i.y) * distort, sin(t - i.y) * distort + cos(t + i.x) * distort);
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(MAX_ITER);
	c = 1.17 - pow(c, 1.4);
	colour.b = pow(abs(c), 14.0);

	gl_FragColor = vec4(colour, 1.0);
}
