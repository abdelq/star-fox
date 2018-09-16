#include "texture.h"
#include <fstream>

Texture::Texture(const char* filepath) : _id(BAD_BUFFER)
{
	std::ifstream file(filepath, std::ifstream::binary | std::ifstream::in);

	if (!file)
	{
		file.open((std::string("../") + filepath).c_str(), std::ifstream::binary | std::ifstream::in);
		if (!file)
		{
			_LOG_CRIT() << "Could not load file '" << filepath << "'";
		}
	}

	char buffer[256];

	file.read(buffer, 18);
	if (!file)
	{
		_LOG_CRIT() << "File '" << filepath << "' is not a valid TGA file.";
	}

	int size_of_image_id = buffer[0], is_colormap = buffer[1], targa_type = buffer[2];

	int colormap_origin = buffer[3] + ((int)(buffer[4]) << 8);
	unsigned int colormap_length = buffer[5] + ((int)(buffer[6]) << 8);
	int colormap_entry_size = buffer[7];

	int image_width = buffer[12] + ((unsigned)(buffer[13]) << 8);
	int image_height = buffer[14] + ((unsigned)(buffer[15]) << 8);
	int image_pixel_size = buffer[16];
	int image_descriptor = buffer[17];

	if ((targa_type != 1) && (targa_type != 2))
	{
		_LOG_CRIT() << "File '" << filepath << "' is of an invalid type.";
	}

	if ((targa_type == 1) && !is_colormap)
	{
		_LOG_CRIT() << "File '" << filepath << "' has no colormap.";
	}

	if ((targa_type == 2) && is_colormap)
	{
		_LOG_CRIT() << "File '" << filepath << "' is incompatible.";
	}

	if (size_of_image_id)
	{
		file.read(buffer, size_of_image_id);
		if (!file)
		{
			_LOG_CRIT() << "File '" << filepath << "' is incomplete.";
		}
	}

	int is_inverted = (image_descriptor & 0x10) != 0;

	if ((image_descriptor & 0xC0))
	{
		_LOG_CRIT() << "File '" << filepath << "' has interlacing, which is not supported.";
	}

	int raster_width;
	std::vector<char> colormap;

	if (is_colormap)
	{
		if ((colormap_entry_size != 24) && (colormap_entry_size != 32))
		{
			_LOG_CRIT() << "File '" << filepath << "' has an unsupported colormap size.";
		}

		colormap_entry_size >>= 3;

		colormap.resize(colormap_length * colormap_entry_size);

		file.read(&colormap[0], colormap_entry_size * colormap_length);
		if (!file)
		{
			_LOG_CRIT() << "File '" << filepath << "' is incomplete.";
		}

		image_pixel_size = (image_pixel_size + 7) >> 3;
		raster_width = image_width *colormap_entry_size;
	}
	else
	{
		if ((image_pixel_size != 24) && (image_pixel_size != 32))
		{
			_LOG_CRIT() << "File '" << filepath << "' has an unsupported depth.";
		}

		image_pixel_size >>= 3;
		raster_width = image_width *image_pixel_size;
	}

	_data.resize(raster_width * image_height);

	int i, j;

	for (int y = (is_inverted ? (image_height - 1) : 0);
		(is_inverted ? (y >= 0) : (y < (int)image_height));
		(is_inverted ? (--y) : (++y)))
	for (int x = 0; x < image_width; x++) {

		file.read(buffer, image_pixel_size);
		if (!file)
		{
			_LOG_CRIT() << "File '" << filepath << "' is incomplete.";
		}

		if (is_colormap)
		{
			/* colormapped */
			i = ((buffer[0] + ((unsigned)(buffer[1]) << 8)) - colormap_origin)
			*colormap_entry_size;
			j = (y *raster_width) + (x *colormap_entry_size);

			_data[j] = colormap[i + 2];
			_data[j + 1] = colormap[i + 1];
			_data[j + 2] = colormap[i];

			if (colormap_entry_size > 3)
				_data[j + 3] = colormap[i + 3];
		}
		else
		{
			/* non-colormapped */
			j = (y *raster_width) + (x *image_pixel_size);

			_data[j] = buffer[2];
			_data[j + 1] = buffer[1];
			_data[j + 2] = buffer[0];

			if (image_pixel_size > 3)
				_data[j + 3] = buffer[3];
		}
	}

	_width = image_width;
	_height = image_height;
	_depth = (unsigned short)image_pixel_size;

	glGenTextures(1, &_id);

	glBindTexture(GL_TEXTURE_2D, _id);

	glTexImage2D(GL_TEXTURE_2D, 0, (_depth > 3) ? (GL_RGBA8) : (GL_RGB8), _width, _height, 0, (_depth > 3) ? (GL_RGBA) : (GL_RGB), GL_UNSIGNED_BYTE, &_data[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	debugGLError();
}

Texture::~Texture()
{
	if (_id != BAD_BUFFER)
		glDeleteTextures(1, &_id);
}