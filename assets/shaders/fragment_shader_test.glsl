#version 330 core
layout(location = 0)out vec4 fragColor;
in vec2 outTexCoord;
in float outTexID;
in float outAlphaValue;
in float depth;
in vec3 outColor;

uniform sampler2D u_textures[32];
uniform int u_max_texture_units;

void set_fragColor(){
	switch(int(outTexID)){
		case 0: fragColor = texture2D(u_textures[0], outTexCoord); break;
		case 1: fragColor = texture2D(u_textures[1], outTexCoord); break;
		case 2: fragColor = texture2D(u_textures[2], outTexCoord); break;
		case 3: fragColor = texture2D(u_textures[3], outTexCoord); break;
		case 4: fragColor = texture2D(u_textures[4], outTexCoord); break;
		case 5: fragColor = texture2D(u_textures[5], outTexCoord); break;
		case 6: fragColor = texture2D(u_textures[6], outTexCoord); break;
		case 7: fragColor = texture2D(u_textures[7], outTexCoord); break;
		case 8: fragColor = texture2D(u_textures[8], outTexCoord); break;
		case 9: fragColor = texture2D(u_textures[9], outTexCoord); break;
		case 10: fragColor = texture2D(u_textures[10], outTexCoord); break;
		case 11: fragColor = texture2D(u_textures[11], outTexCoord); break;
		case 12: fragColor = texture2D(u_textures[12], outTexCoord); break;
		case 13: fragColor = texture2D(u_textures[13], outTexCoord); break;
		case 14: fragColor = texture2D(u_textures[14], outTexCoord); break;
		case 15: fragColor = texture2D(u_textures[15], outTexCoord); break;
		case 16: fragColor = texture2D(u_textures[16], outTexCoord); break;
		case 17: fragColor = texture2D(u_textures[17], outTexCoord); break;
		case 18: fragColor = texture2D(u_textures[18], outTexCoord); break;
		case 19: fragColor = texture2D(u_textures[19], outTexCoord); break;
		case 20: fragColor = texture2D(u_textures[20], outTexCoord); break;
		case 21: fragColor = texture2D(u_textures[21], outTexCoord); break;
		case 22: fragColor = texture2D(u_textures[22], outTexCoord); break;
		case 23: fragColor = texture2D(u_textures[23], outTexCoord); break;
		case 24: fragColor = texture2D(u_textures[24], outTexCoord); break;
		case 25: fragColor = texture2D(u_textures[25], outTexCoord); break;
		case 26: fragColor = texture2D(u_textures[26], outTexCoord); break;
		case 27: fragColor = texture2D(u_textures[27], outTexCoord); break;
		case 28: fragColor = texture2D(u_textures[28], outTexCoord); break;
		case 29: fragColor = texture2D(u_textures[29], outTexCoord); break;
		case 30: fragColor = texture2D(u_textures[30], outTexCoord); break;
		case 31: fragColor = texture2D(u_textures[31], outTexCoord); break;
		default: fragColor = vec4(outColor, 1.0f);
	}
}

void main()
{
	set_fragColor();
	float average = (fragColor.x + fragColor.y + fragColor.z) / 3.0f;
	
	fragColor = vec4(average,average,average, fragColor.w);		
};