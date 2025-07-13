#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "common.h"
#include "texture.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Texture<u8> LoadTexture(char const* path)
{
	s32 width, height, channels;
	u8* data = stbi_load(path, &width, &height, &channels, 4);
	if (!data)	{ DebugLog(ASSET_FAILED, path); return Texture<u8>(width, height, data); }
	else		{ DebugLog(ASSET_LOADED, path); return Texture<u8>(width, height, data); }
}

/// Load texture using a directory and file name (including extension)
Texture<unsigned char> LoadTexture(char const* name, std::string const& dir)
{
	return LoadTexture((dir + name).c_str());
}

void Save(Texture<u8> const& t, char const* path)
{
	if (stbi_write_png(path, t.width, t.height, 1, t.data, t.width))
		DebugLog(ASSET_SAVED, path);
	else
		DebugLog(ASSET_FAILED, path);
}

void Save(vector<Texture<u8>> const& layers, char const* name)
{
	for (int i = 0; i < layers.size(); i++)
	{
		std::string exclExtension = std::string(name) + std::to_string(i);
		Save(layers[i], (exclExtension + ".png").c_str());
	}
}
