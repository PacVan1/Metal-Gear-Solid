#ifdef STATIC
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main()                  
{ 					
	fragPos		= vec3(model * vec4(aPosition, 1.0));
	normal		= normalize(mat3(transpose(inverse(model))) * aNormal); 
	texCoord	= aTexCoord; 
#ifdef UI_ELEMENT
	gl_Position = projection * model * vec4(aPosition, 1.0);
#else
	gl_Position = VP * model * vec4(aPosition, 1.0);
#endif
}
#endif
#ifdef SKELETAL
layout (location = 0) in vec3	aPosition;
layout (location = 1) in vec3	aNormal;
layout (location = 2) in vec2	aTexCoord;
layout (location = 3) in ivec4	aBoneIDs;
layout (location = 4) in vec4	aWeights;

uniform mat4		finalBoneMatrices[100];
uniform mat4		model;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

// shader has incorrect computation of normals after getting transformed by rig
void main()                  
{ 				
	vec4 totalPosition	= vec4(0.0);
	vec4 totalNormal	= vec4(0.0);
	for (int i = 0; i < 4; i++)
	{
		if (aBoneIDs[i] == -1) continue;
	
		if (aBoneIDs[i] >= 100)
		{
			totalPosition = vec4(aPosition, 1.0);
			totalNormal = vec4(aNormal, 0.0);
			break;
		}
		vec4 localPosition = finalBoneMatrices[aBoneIDs[i]] * vec4(aPosition, 1.0);
		totalPosition += localPosition * aWeights[i];
		mat3 normalMatrix = mat3(finalBoneMatrices[aBoneIDs[i]]);
		vec3 localNormal = normalize(normalMatrix * aNormal);
		totalNormal += vec4(localNormal * aWeights[i], 0.0);
	}

	fragPos		= vec3(model * totalPosition);
	normal		= normalize(transpose(inverse(mat3(model))) * vec3(totalNormal)); 
	texCoord	= aTexCoord; 
	gl_Position = VP * model * totalPosition;
}
#endif