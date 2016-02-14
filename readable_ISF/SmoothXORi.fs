/*{
	"CREDIT": "tado",

	"CATEGORIES": [
		""
	],
	"DESCRIPTION": "",
  "INPUTS": [
			{
            "NAME": "rate",
            "TYPE": "float",
           "DEFAULT": 4.8,
            "MIN": 0.5,
            "MAX": 10.0
        },
          {
            "NAME": "intensity",
            "TYPE": "float",
           "DEFAULT": 0.86,
            "MIN": 0.1,
            "MAX": 0.99
        },
                          {
            "NAME": "seed1",
            "TYPE": "float",
           "DEFAULT": 0.0087,
            "MIN": 0.0005,
            "MAX": 0.0050
        },
                  {
            "NAME": "seed2",
            "TYPE": "float",
           "DEFAULT": 0.33,
            "MIN": 0.01,
            "MAX": 0.99
        },
          {
            "NAME": "zoom",
            "TYPE": "float",
           "DEFAULT": 3,
            "MIN": 1,
            "MAX": 20
        },
        			{
            "NAME": "tint",
            "TYPE": "float",
           "DEFAULT": 0.35,
            "MIN": 0.1,
            "MAX": 0.6
        }
  ]
}
*/

void main()
{
    vec2 pos = (RENDERSIZE.y/zoom)*gl_FragCoord.xy/RENDERSIZE.x - (TIME*rate-zoom);

    vec3 col = vec3(0.0);
    for( int i=0; i<6; i++ )
    {
        vec2 a = floor(pos);
        vec2 b = fract(pos);

        vec4 w = fract((sin(a.x*7.0+31.0*a.y + 0.01*TIME)+vec4(seed2,0.01,0.0,0.7))*pow(139.74812,seed1)*123.2352456); // randoms

        col += w.xyz *                                   // color
               smoothstep(intensity*sqrt(intensity),intensity,w.w) *               // intensity
               sqrt(16.0*b.x*b.y*(1.0-b.x)*(1.0-b.y) ); // pattern

        pos /= 2.0; // lacunarity
        col /= (2.5-intensity); // attenuate high frequencies
    }

    col = pow( 1.5*col, vec3(tint+0.2,1.0,0.9-tint) );    // contrast and color shape

    gl_FragColor = vec4( col, 1.0 );
}
