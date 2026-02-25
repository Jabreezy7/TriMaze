#version 120

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;


vec4 ambient = vec4(0.0, 0.0, 0.0, 0.0);
vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);
vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);

varying vec2 texCoord;
uniform sampler2D texture;

uniform vec4 light_options;

uniform vec4 aim_direction;

uniform int light_source;

void main()
{
	vec4 color = texture2D(texture, texCoord);

	vec4 NN = normalize(N);
	vec4 LL = normalize(L);
	vec4 VV = normalize(V);
	
	if(light_options.x == 1.0){
		ambient = color * 0.3;

		if(light_source == 2){
			ambient = color * 0.05;
		}
	}

	if(light_options.y == 1.0){
		diffuse = max(dot(LL, NN), 0.0) * color;

		if(light_source == 2){
			vec4 AA = normalize(-1 * aim_direction);
			diffuse = pow(max(dot(AA, LL), 0.0), 50) * color;
		}
	}

	vec4 R = ((2 * dot(LL, NN)) * NN) - LL;
	float shininess = 100;

	if(light_options.z == 1.0){
		specular = pow(max(dot(VV, R), 0.0), shininess) * vec4(1.0, 1.0, 1.0, 1.0);

		if(light_source == 2){
			specular = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}

	float a = 1;
	float b = 0;
	float c = 0;

	float attenuation = 1 / (a + (b * distance) + (c * distance * distance));

	if(light_options.w == 1.0){
		gl_FragColor = ambient + attenuation * (diffuse + specular);
		//gl_FragColor = ambient + diffuse + specular;
		//gl_FragColor = NN;
	}

	else{
		gl_FragColor = color;
	}

}
