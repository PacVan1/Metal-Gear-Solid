#pragma once

static string_view constexpr DEFAULT_TEXTURE_DIR = "../../../../../Common/Assets/Textures/";

template <typename T>
struct Texture
{
	T*	data;
	int	width; 
	int height;
	Texture();
	Texture(int const width, int const height);
	Texture(int const width, int const height, T* data); 
	Texture(Texture<T> const& other);
	[[nodiscard]] T& operator[](int index);
	[[nodiscard]] T const& operator[](int index) const; 
};

template <typename T>
Texture<T>::Texture() :
	data(nullptr), 
	width(0), 
	height(0)
{}

template <typename T>
Texture<T>::Texture(int const width, int const height) :
	data(static_cast<T*>(malloc(sizeof(T) * width * height))),
	width(width), 
	height(height)
{}

template<typename T>
inline Texture<T>::Texture(int const width, int const height, T* data) : 
	data(data), 
	width(width),
	height(height)
{}

template <typename T>
Texture<T>::Texture(Texture<T> const& other) : 
	data(other.data),
	width(other.width),
	height(other.height)
{}

template <typename T>
inline T& Texture<T>::operator[](int index)
{
	return data[index];
}

template <typename T> 
inline T const& Texture<T>::operator[](int index) const 
{
	return data[index];   
}

Texture<unsigned char>		LoadTexture(char const* path);
Texture<unsigned char>		LoadTexture(char const* name, std::string const& dir);
template<typename T> void	Destruct(Texture<T>& texture) { free(texture.data); }
void						Save(Texture<u8> const& texture, char const* path);
void						Save(vector<Texture<u8>> const& layers, char const* name);