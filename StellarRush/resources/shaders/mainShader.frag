#version 400 core

in vec3 vColour;				// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;				// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec4 eyePosition;			// Position in eye coordinates
in vec3 eyeNormal;				// Normal in eye coordinates
in vec3 worldPosition;

out vec4 vOutputColour;			// The output colour

uniform int pass;
uniform sampler2D sampler0;		

uniform samplerCube CubeMapTex;
uniform bool renderSkybox;

uniform bool showFog;
float rho = 0.005f;
float fogStart = 200.0f;		// Start distance of the fog
float fogEnd = 600.0f;			// End distance of the fog
vec3 fogColour = vec3(0.0f);	// Colour of the fog

uniform bool isAsteroid;
uniform bool isTrack;
uniform bool isShip;
uniform bool isImmune;
uniform bool blur;

uniform float t;				// Value used to move the noise
uniform vec2 resolution;		// Resolution for the noise

uniform int width, height;
uniform bool renderScreenQuad;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
#define MAX_LIGHTS 50
uniform int spotlightsAmount;

uniform LightInfo directionalLight;
uniform LightInfo directionalLight2;
uniform LightInfo spotlights[MAX_LIGHTS]; 
uniform MaterialInfo material; 

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(LightInfo light, vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(directionalLight.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light.La * material.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light.Ld * material.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light.Ls * material.Ms * pow(max(dot(r, v), 0.0f), material.shininess + eps);
	
	return ambient + diffuse + specular;
}

vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material.Ma;
	float d = distance(light.position.xyz, p.xyz);
	if (angle < cutoff && d < 50) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = light.Ls * material.Ms * pow(max(dot(h, n), 0.0), material.shininess);
		return ambient + spotFactor * (diffuse + specular);
	} else 
		return ambient;
}

vec4 Blur()
{
	float dx = 1.0 / float(width);
	float dy = 1.0 / float(height);

	int halfSize = 6;
	vec4 avg = vec4(0);

	for(int y = -halfSize; y <= halfSize; y++) 
	{
		for(int x = -halfSize; x <= halfSize; x++) 
		{
			avg += texture(sampler0, vTexCoord + vec2(x * dx, y * dy));
		}
	}
	int N = (2 * halfSize + 1);
	avg /= N * N;
	return avg;
}

// Simplex noise and hash function from https://www.shadertoy.com/view/Msf3WH
vec2 hash( vec2 p )
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}

void main()
{
	if (renderSkybox) 
	{
		vOutputColour = texture(CubeMapTex, worldPosition);
		vOutputColour = vOutputColour * 2.0f;
	}
	else if (renderScreenQuad)
	{
		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		// Don't use lighting when rendering quad
		vOutputColour = vTexColour;

		// Create blur effect and make colours darker
		if (pass == 1 && blur)
		{
			vOutputColour = Blur();
			vOutputColour.rgb = vOutputColour.rgb * 0.7f;
		}
	}
	else 
	{
		// Apply the Phong model to compute the fragment colour
		vec3 vColour = PhongModel(directionalLight, eyePosition, eyeNormal) + PhongModel(directionalLight2, eyePosition, eyeNormal) ;

		if (!isAsteroid)
		{
			for(int i = 0; i < 0; i++)
				vColour += BlinnPhongSpotlightModel(spotlights[i], eyePosition, normalize(eyeNormal));
		}

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		vOutputColour = vTexColour * vec4(vColour, vTexColour.a);	// Combine object colour and texture 

		if (isShip)
		{
			// Create effect using noise when ship is immune
			if (isImmune)
			{
				vec2 p = eyePosition.xz / resolution.xy;

				vec2 uv = p * vec2(resolution.x / resolution.y, 1.0) + t * 0.001f;
			
				// Scale the noise differently to get 3 different values
				float f1 = noise(15.0f * uv);
				float f2 = noise(10.0f * uv);
				float f3 = noise(5.0f * uv);

				// Increase the noise values to make them brighter
				f1 = 0.8f + 0.5f * f1;
				f2 = 0.8f + 0.5f * f2;
				f3 = 0.8f + 0.5f * f3;
	
				// Use the noise values to create the colour
				vec4 colour = vec4(f1, f2, f3, 1.0);

				// Make the texture brighter and combine it with the colour
				vOutputColour = (vTexColour * 3) * colour;
			}
		}

		if (isTrack)
		{
			vOutputColour = vOutputColour * 1.5f;
		}

		// Create fog using alpha value to make objects further away invisible
		// Don't use fog for asteroids
		if (showFog && !isAsteroid)
		{
			float d = length(eyePosition.xyz);
			float w;
			if (d < fogEnd)
				w = (fogEnd - d) / (fogEnd - fogStart);
			else
				w = 0;
			vOutputColour.a = mix(0.0f, vOutputColour.a, w);
		}

		vOutputColour = vOutputColour * 2.0f;
	}
}
