/*{
    "INPUTS" : [
        {
			"NAME": "tiling",
			"TYPE": "float",
			"MIN": 0.0,
			"MAX": 8.0,
			"DEFAULT": 1.0
		},
		{
			"NAME": "distort",
			"TYPE": "float",
			"MIN": 0.5,
			"MAX": 1.5,
			"DEFAULT": 1.0
		}
  ]
}
*/


#define TAU 6.28318530718
#define MAX_ITER 8

void main()
{
	float time = TIME * .5 + 23.0;
	vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;
    
	vec2 p = mod(uv * TAU * tiling, TAU) - 250.0;
	vec2 i = vec2(p);
	float c = 1.0;
	float inten = .005;

	for (int n = 0; n < MAX_ITER; n++){
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) * distort + sin(t + i.y) * distort, sin(t - i.y) * distort + cos(t + i.x) * distort);
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(MAX_ITER);
	c = 1.17 - pow(c, 1.4);
	vec3 colour = vec3(pow(abs(c), 14.0));
    colour = clamp(colour + vec3(0.2, 0.5, 0.7), 0.0, 1.0);
    
	vec2 pixel = 2.0 / RENDERSIZE.xy;
	uv *= 2.0;

	float f = floor(mod(TIME*.5, 2.0));
	vec2 first = step(pixel, uv) * f;
	uv  = step(fract(uv), pixel);

	gl_FragColor = vec4(colour, 1.0);
}