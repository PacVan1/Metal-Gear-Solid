layout (location = 0) in vec3	aPosition;
layout (location = 1) in vec3	aNormal;
layout (location = 2) in vec2	aTexCoord;
layout (location = 3) in ivec4	aBoneIDs;
layout (location = 4) in vec4	aWeights;

uniform mat4		MVP;
uniform mat4		finalBoneMatrices[100];

out vec2 texCoord;

void main()                  
{ 				
	vec4 totalPosition = vec4(0.0);
	for (int i = 0; i < 4; i++)
	{
		if (aBoneIDs[i] == -1) continue;
	
		if (aBoneIDs[i] >= 100)
		{
			totalPosition = vec4(aPosition, 1.0);
			break;
		}
		vec4 localPosition = finalBoneMatrices[aBoneIDs[i]] * vec4(aPosition, 1.0);
		totalPosition += localPosition * aWeights[i];
	}

	texCoord	= aTexCoord; 
	gl_Position = MVP * totalPosition;
	//gl_Position = MVP * vec4(aPosition, 1.0);
}