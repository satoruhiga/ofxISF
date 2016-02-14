/*{
    "CREDIT": "by tado",
    "CATEGORIES" : [
        "noise1"
    ],
    "INPUTS" : [
        {
            "NAME": "inputImage",
            "TYPE": "image"
        },

  ]
}
*/

vec3 sqr( vec3 x ) { return x*x; }
void main()
{
    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / RENDERSIZE.xy;
    float a = atan(p.y,p.x);
    float r = sqrt(dot(p,p));
    float s = r * (1.0+0.5*cos(TIME*0.5));

    vec2 uv = 0.02*p;
    uv.x +=                  .03*cos(-TIME+a*4.0)/s;
    uv.y += .02*TIME +.03*sin(-TIME+a*4.0)/s;
    uv.y += r*r*0.025*sin(2.0*r);

    vec3 col = IMG_NORM_PIXEL(inputImage,mod(0.5*uv,1.0)).xyz  * vec3(1.0,0.8,0.6);
    col += sqr(IMG_NORM_PIXEL(inputImage,mod(1.0*uv,1.0)).xxx) * vec3(0.7,1.0,1.0);

    float w = 2.0*r;
	w *= 0.5 + 0.5*pow(clamp(1.0-0.75*r,0.0,1.0),0.5);

	gl_FragColor = vec4(col*w,1.0);
}
