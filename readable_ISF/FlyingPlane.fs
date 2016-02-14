/*{
    "CREDIT": "by tado",
    "CATEGORIES" : [
        
    ],
    "INPUTS" : [
    	{
			"NAME": "planeBlur",
			"LABEL": "Plane Blur",
			"TYPE": "float",
			"MIN": 0.0,
			"MAX": 1.0,
			"DEFAULT": 0.9
		},
         {
            "NAME":"level",
            "TYPE": "float",
           "DEFAULT": 0.0,
            "MIN": -1.0,
            "MAX": 1.0
        }
  ]
}
*/
#define SPEED           (1.7)
#define WARMUP_TIME     (2.0)

// Shadertoy's sound is a bit out of sync every time you run it :(
#define SOUND_OFFSET    (-0.0)

float saturate(float x){
    return clamp(x, 0.0, 1.0);
}

float isectPlane(vec3 n, float d, vec3 org, vec3 dir){
    float t = -(dot(org, n) + d) / dot(dir, n);
    return t;
}


vec3 drawEffect(vec2 coord, float time){
    vec3 clr = vec3(0.0);
    const float far_dist = 10000.0;
    vec2 uv = coord.xy / RENDERSIZE.xy;

    vec3 org = vec3(0.0);
    vec3 dir = vec3(uv.xy * 2.0 - 1.0, 1.0);

    // Animate tilt
    //float ang = sin(time * 0.01) * 3.0;
    float ang = 3.2415/2.0;
    vec3 odir = dir;
    dir.x = cos(ang) * odir.x + sin(ang) * odir.y;
    dir.y = sin(ang) * odir.x - cos(ang) * odir.y;

    // Animate FOV and aspect ratio
    //dir.x *= 1.5 + 0.5 * sin(time * 0.125);
    //dir.y *= 1.5 + 0.5 * cos(time * 0.25 + 0.5);
    
    dir.x *= 2.0;

    // Animate view direction
    //dir.x += 0.25 * sin(time * 0.3);
    //dir.y += 0.35 * cos(time * 0.7);

    // Cycle between long blurry and short sharp particles
    //vec2 param = mix(vec2(60.0, 0.8), vec2(800.0, 3.0), pow(0.5 + 0.5 * sin(time * 0.2), 2.0));
    vec2 param = vec2(1000.0, 1.0);

    //float lt = fract(mtime / 4.0) * 4.0;
    vec2 mutes = vec2(0.0);

    for (int k = 0; k < 2; k++)
    for (int i = 0; i < 24; i++)
    {
        // Draw only few layers during prologue
        //if (mtime < 16.0 && i >= 16) break;

        vec3 pn = vec3(k > 0 ? -1.0 : 1.0, 0.0, 0.0);
        float t = isectPlane(pn, 200.0 + float(i) * 100.0, org, dir);

        if (t <= 0.0 || t >= far_dist) continue;

        vec3 p = org + dir * t;
        vec3 vdir = normalize(-p);

        // Create particle lanes by quantizing position
        vec3 pp = ceil(p / 100.0) * 100.0;

        // Pseudo-random variables
        float n = pp.y + float(i) + float(k) * 123.0;
        float q = fract(sin(n * 123.456) * 234.345);
        float q2= fract(sin(n * 234.123) * 345.234);

		float pblur = (1.0 - planeBlur) / 2000.0;
        q = sin(p.z * pblur + 1.0 * time * (0.25 + 0.75 * q2) + q * 12.0);		
		
        // Smooth particle edges out
        q = saturate(q * param.x - param.x + 1.0) * param.y;
        q *= saturate(4.0 - 8.0 * abs(-50.0 + pp.y - p.y) / 100.0);
				
        // Fade out based on distance
        q *= 1.0 - saturate(pow(t / far_dist, 5.0));
		
        // Fade out based on view angle
        float fn = 1.0 - pow(1.0 - dot(vdir, pn), 2.0);
        q *= 2.0 * smoothstep(0.0, 1.0, fn);

        //Flash fade left or right plane
        q *= 1.0 - 0.9 * (k == 0 ? mutes.x : mutes.y);
		
        // Cycle palettes
        const vec3 orange = vec3(1.0, 0.2, 0.0);
        const vec3 blue   = vec3(0.0, 0.0, 1.0);
        //clr += q * mix(orange, blue, 0.5 + 0.5 * sin(time * 0.5 + q2));
        //clr += q * mix(orange, blue, 1.0 - level);
        clr += q * mix(orange, blue, 0.1);
        //clr = orange;
    }

    //clr *= 0.8;

    // Cycle gammas
    clr.r = pow(clr.r, 0.75 + 0.35 * sin(time * 0.5));
    clr.b = pow(clr.b, 0.75 - 0.35 * sin(time * 0.5));

    // Vignette in linear space (looks better)
    clr *= clr;
    clr *= 1.4;
    clr *= 1.0 - 1.5 * dot(uv - 0.5, uv - 0.5);
    clr = sqrt(max(vec3(0.0), clr));

    return clr;
}

void main()
{
    float time = max(0.0, TIME - WARMUP_TIME);
    vec3  clr = vec3(0.0);

    clr = drawEffect(gl_FragCoord.xy, TIME);
    //clr = mix(clr, vec3(0.8, 0.9, 1.0), 0.3 * drawLogo(gl_FragCoord.xy));

    gl_FragColor = vec4(clr, 1.0);
}