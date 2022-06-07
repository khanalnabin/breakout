#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management
class Texture2D {
  public:
	unsigned int ID;
	unsigned int Width, Height;

	// texture format
	unsigned int Internal_Format;
	unsigned int Image_Format;

	// texure configuration
	unsigned int Wrap_S;
	unsigned int Wrap_T;
	unsigned int Filter_Min;
	unsigned int Filter_Max;

	// constructor (sets default texture modes)
	Texture2D();

	// generates texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char *data);

	// binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};
#endif // TEXTURE_H_
