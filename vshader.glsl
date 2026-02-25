#version 120

attribute vec4 vPosition;
attribute vec2 vTexCoord;
varying vec2 texCoord;
attribute vec4 vNormal;

uniform mat4 ctm;
uniform mat4 model_view;
uniform mat4 projection_matrix;
uniform vec4 light_position;
uniform int light_source;

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

void main()
{
	gl_Position = projection_matrix * model_view * ctm * vPosition;

	N = normalize(model_view * ctm * vNormal);

	vec4 L_temp;

	if(light_source == 0){
		L_temp = model_view * (light_position - ctm * vPosition);
	}

	else{
		//L_temp = light_position - (model_view * ctm * vPosition);
		L_temp = vec4(0.0, 0.0, 0.0, 1.0) - (model_view * ctm * vPosition);
	}


	//L_temp = model_view * (light_position - ctm * vPosition);
	//L_temp = light_position - (model_view * ctm * vPosition);
	L = normalize(L_temp);

	vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
	V = normalize(eye_point - (model_view * ctm * vPosition));

	distance = length(L_temp);

	texCoord = vTexCoord;
}
