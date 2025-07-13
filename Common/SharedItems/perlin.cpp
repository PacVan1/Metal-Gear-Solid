#include "common.h"
#include "perlin.h"

#define TWO_PI					2 * PI
#define MAX_UNSIGNED			~0u						// 0x00000000 >flipped> 0xffffffff
#define MIN_SIGNED				~(MAX_UNSIGNED >> 1)	// 0xffffffff >shifted> 0x7fffffff >flipped> 0x80000000
#define PI_OVER_MIN_SIGNED		PI / MIN_SIGNED	

void ComputeHelperValues(PerlinData& d)
{
	d.cellsX	= d.width	/ d.cellSize;
	d.cellsY	= d.height	/ d.cellSize;
	d.cellsZ	= d.depth	/ d.cellSize;
	d.stepSizeX = static_cast<f32>(d.cellsX) / static_cast<f32>(d.width);
	d.stepSizeY = static_cast<f32>(d.cellsY) / static_cast<f32>(d.height);
	d.stepSizeZ = static_cast<f32>(d.cellsZ) / static_cast<f32>(d.depth);
}

static inline int Loop(int const num, int const min, int const max)
{
	return num <= min ? max : num;
}

static float Ease(float const a, float const b, float const t)
{
	return (b - a) * (3.0f - t * 2.0f) * t * t + a;
}

static vec3 RandomGradient(int const x, int const y, int const z, int const seed)
{
#pragma warning(push)
#pragma warning(disable : 4305) // warning C4305: '*=': truncation from '__int64' to 'unsigned int'
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = x * seed;
	unsigned b = y * seed;
	signed	 c = z * seed;

	a *= 3284157443;
	b ^= (a << s) | (a >> (w - s));
	b *= 1911520717;
	a ^= (b << s) | (b >> (w - s));
	a *= 2048419325;
	c ^= (a << s) | (a >> (w - s));
	c *= 1284432903;
	b ^= (c << s) | (c >> (w - s));
	b *= 5769034271;
	a ^= (c << s) | (c >> (w - s));
	a *= 4383098259;

	float phi	= a * PI_OVER_MIN_SIGNED;	// in range [0, 2PI]    
	float theta = c * PI_OVER_MIN_SIGNED;	// in range [0, PI]
	return { sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta) };
#pragma warning(pop) 
}

static float CellWeight(int const xi, int const yi, int const zi, float const x, float const y, float const z, int const seed)
{
    vec3 grad = RandomGradient(xi, yi, zi, seed);
    float dx = x - (float)xi;
    float dy = y - (float)yi;
    float dz = z - (float)zi;
    return (dx * grad.x + dy * grad.y + dz * grad.z);
}

float Perlin(float const x, float const y, float const z, int const seed)
{
	int x0 = (int)x, y0 = (int)y, z0 = (int)z;
	int x1 = x0 + 1, y1 = y0 + 1, z1 = z0 + 1;
	float sx = x - (float)x0, sy = y - (float)y0, sz = z - (float)z0;
	float w0		= CellWeight(x0, y0, z0, x, y, z, seed);
	float w1		= CellWeight(x1, y0, z0, x, y, z, seed);
	float w2		= CellWeight(x0, y0, z1, x, y, z, seed);
	float w3		= CellWeight(x1, y0, z1, x, y, z, seed);
	float avgTop	= Ease(Ease(w0, w1, sx), Ease(w2, w3, sx), sz);
	float w4		= CellWeight(x0, y1, z0, x, y, z, seed);
	float w5		= CellWeight(x1, y1, z0, x, y, z, seed);
	float w6		= CellWeight(x0, y1, z1, x, y, z, seed);
	float w7		= CellWeight(x1, y1, z1, x, y, z, seed);
	float avgBottom = Ease(Ease(w4, w5, sx), Ease(w6, w7, sx), sz);
	return Ease(avgTop, avgBottom, sy);
}

static float CellWeightLooped(int const xi, int const yi, int const zi, float const x, float const y, float const z, int const loopedX, int const loopedY, int const loopedZ, int const seed)
{
    glm::vec3 grad = RandomGradient(loopedX, loopedY, loopedZ, seed);
    float dx = x - (float)xi;
    float dy = y - (float)yi;
    float dz = z - (float)zi;
    return (dx * grad.x + dy * grad.y + dz * grad.z);
}

float PerlinLooped(float const x, float const y, float const z, float const cellsX, float const cellsY, float const cellsZ, int const seed)
{
	int x0 = (int)x, y0 = (int)y, z0 = (int)z;
	int x1 = x0 + 1, y1 = y0 + 1, z1 = z0 + 1; 
	float sx = x - (float)x0, sy = y - (float)y0, sz = z - (float)z0;
	int loopedX0	= Loop(x0, 0, (int)cellsX), loopedY0 = Loop(y0, 0, (int)cellsY), loopedZ0 = Loop(z0, 0, (int)cellsZ);
	float w0		= CellWeightLooped(x0, y0, z0, x, y, z, loopedX0, loopedY0, loopedZ0, seed);
	float w1		= CellWeightLooped(x1, y0, z0, x, y, z, x1,		  loopedY0, loopedZ0, seed);
	float w2		= CellWeightLooped(x0, y0, z1, x, y, z, loopedX0, loopedY0, z1,		  seed);
	float w3		= CellWeightLooped(x1, y0, z1, x, y, z, x1,		  loopedY0, z1,		  seed);
	float avgTop	= Ease(Ease(w0, w1, sx), Ease(w2, w3, sx), sz);
	float w4		= CellWeightLooped(x0, y1, z0, x, y, z, loopedX0, y1, loopedZ0, seed);
	float w5		= CellWeightLooped(x1, y1, z0, x, y, z, x1,		  y1, loopedZ0, seed);
	float w6		= CellWeightLooped(x0, y1, z1, x, y, z, loopedX0, y1, z1,	    seed);
	float w7		= CellWeightLooped(x1, y1, z1, x, y, z, x1,		  y1, z1,		seed);
	float avgBottom = Ease(Ease(w4, w5, sx), Ease(w6, w7, sx), sz);
	return Ease(avgTop, avgBottom, sy);
}

void GenPerlinTexture(PerlinData const& d, u32 const depth, Texture<u8>& t)
{
	u8* data = new u8[d.width * d.height];

	int addr = 0;
	for (u32 yy = 0; yy < d.height; yy++)
	{
		for (u32 xx = 0; xx < d.width; xx++)
		{
			float amplitude = 1.0f;
			float noise = 0.0f;

			float fCellSize		= (float)d.cellSize;
			float fCellSizeZ	= (float)d.cellSizeZ;

			for (u32 oo = 0; oo < d.octaves; oo++)
			{
				noise += PerlinLooped(
					xx / fCellSize,
					yy / fCellSize,
					depth / fCellSizeZ,
					static_cast<float>(d.width) / fCellSize,
					static_cast<float>(d.height) / fCellSize,
					static_cast<float>(d.depth) / fCellSizeZ, 
					d.seed) * amplitude;

				fCellSize	/= 2.0f;
				fCellSizeZ	/= 2.0f;
				amplitude	/= 2.0f;
			}

				 if (noise > 1.0f)  noise = 1.0f;
			else if (noise < -1.0f) noise = -1.0f;

			noise += 1.0f;
			noise /= 2.0f;

			data[addr++] = (u8)(noise * 255.0f);
		}
	}

	t = Texture<u8>(d.width, d.height, data);
}

void GenPerlinTextureArray(PerlinData const& d, vector<Texture<u8>>& layers)
{
	layers.reserve(d.depth);
	layers.resize(d.depth);
	for (u32 i = 0; i < d.depth; i++)
	{
		GenPerlinTexture(d, i, layers[i]);
	}
	DebugLog(ASSET_GENERATED, "Perlin noise layer generated!");
}

void GenPerlinTextureArrayMT(PerlinData const& d, vector<Texture<u8>>& layers)
{
	layers.reserve(d.depth);
	layers.resize(d.depth);
	vector<std::thread> threads;
	for (u32 i = 0; i < d.depth; i++)
	{
		threads.emplace_back(&GenPerlinTexture, std::cref(d), i, std::ref(layers[i]));
	}
	for (auto& t : threads) t.join();
	DebugLog(ASSET_GENERATED, "Perlin noise layer generated!");
}
