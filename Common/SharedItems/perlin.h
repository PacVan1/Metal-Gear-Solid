#pragma once

// src: 
// Understanding Perlin Noise:		https://adrianb.io/2014/08/09/perlinnoise.html
// Improving Noise - Ken Perlin:	https://mrl.cs.nyu.edu/~perlin/paper445.pdf
// Random Gradient Function:		https://pastebin.com/XwCPn0xR
// Perlin Noise Video - Zipped:		https://www.youtube.com/watch?v=kCIaHqb60Cw

struct PerlinData
{
	u32 width;
	u32 height;
	u32 depth;
	u32 octaves;
	u32 cellSize;
	u32 cellSizeZ;
	u32 seed;

	// helper:
	u32	cellsX;
	u32	cellsY;
	u32	cellsZ;
	f32	stepSizeX;
	f32	stepSizeY;
	f32	stepSizeZ;
};

void				ComputeHelperValues(PerlinData& d);
[[nodiscard]] float	Perlin(float const x, float const y, float const z, int const seed);
[[nodiscard]] float	PerlinLooped(float const x, float const y, float const z, float const cellsX, float const cellsY, float const cellsZ, int const seed);
[[nodiscard]] void 	GenPerlinTexture(PerlinData const& data, u32 const depth, Texture<u8>& texture);
[[nodiscard]] void	GenPerlinTextureArray(PerlinData const& data, vector<Texture<u8>>& layers);
[[nodiscard]] void	GenPerlinTextureArrayMT(PerlinData const& data, vector<Texture<u8>>& layers);