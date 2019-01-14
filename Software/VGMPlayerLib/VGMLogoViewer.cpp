#include "VGMLogoViewer.h"
#include <png.h>

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

bool VGMLogoViewer::read_png_file(const char* filename)
{
	png_byte magic[8];
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth, color_type;
	FILE *fp = NULL;
	png_bytep *row_pointers = NULL;
	png_uint_32 w, h;
	int i;

	/* Open image file */
	fp = fopen(filename, "rb");
	if (!fp)
	{
		fprintf(stderr, "error: couldn't open \"%s\"!\n", filename);
		return NULL;
	}

	/* Read magic number */
	fread(magic, 1, sizeof(magic), fp);
	/* Check for valid magic number */
	if (!png_check_sig(magic, sizeof(magic)))
	{
		fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n", filename);
		fclose(fp);
		return NULL;
	}

	/* Create a png read struct */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return NULL;
	}
	
	/* Create a png info struct */
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}
	
	/* Create our OpenGL texture object */
	//texinfo = (gl_texture_t *)malloc(sizeof(gl_texture_t));
	/* Initialize the setjmp for returning properly after a libpng error occured */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		if (row_pointers) free(row_pointers);
		return NULL;
	}
	
	/* Setup libpng for using standard C fread() function with our FILE pointer */
	png_init_io(png_ptr, fp);
	
	/* Tell libpng that we have already read the magic number */
	png_set_sig_bytes(png_ptr, sizeof(magic));
	
	/* Read png info */
	png_read_info(png_ptr, info_ptr);
	
	/* Get some usefull information from header */
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	
	/* Convert index color images to RGB images */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	png_set_filler(png_ptr, 0xff, 0);
	
	/* Convert 1-2-4 bits grayscale images to 8 bits grayscale. */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
	
	if (bit_depth == 16) 
		png_set_strip_16(png_ptr);
	else if (bit_depth < 8) 
		png_set_packing(png_ptr);
	
	/* Update info structure to apply transformations */
	png_read_update_info(png_ptr, info_ptr);
	
	/* Retrieve updated information */
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);
	
	/* Get image format and components per pixel */
	//GetPNGtextureInfo(color_type, texinfo);

	/* We can now allocate memory for storing pixel data */
	GLubyte *texels = (GLubyte *)malloc(sizeof(GLubyte) * w * h * 4);
	
	/* Setup a pointer array. Each one points at the begening of a row. */
	row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * h);
	for (i = 0; i < h; ++i)
	{
		row_pointers[i] = (png_bytep)(texels + ((h - (i + 1)) * w * 4));
	}
	
	/* Read pixel data using row pointers */
	png_read_image(png_ptr, row_pointers);
	
	/* Finish decompression and release memory */
	png_read_end(png_ptr, NULL);
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	/* We don't need row pointers anymore */
	free(row_pointers);
	
	fclose(fp);
}

VGMLogoViewer::VGMLogoViewer(const string& name_, const string& filename_, UINT32 x_, UINT32 y_, UINT32 width_, UINT32 height_)
	: VGMDataObverser()
	, name(name_)
	, filename(filename_)
	, x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
{
}

VGMLogoViewer::~VGMLogoViewer()
{
}

void VGMLogoViewer::onNotifySomething(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMLogoViewer::onNotifyOpen(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.open(name.c_str(), x, y, width, height);
	read_png_file(filename.c_str());
}

void VGMLogoViewer::onNotifyClose(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;

	const VGMHeader& header = vgmData.getHeader();
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	videoDevice.close();
}

void VGMLogoViewer::onNotifyPlay(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMLogoViewer::onNotifyStop(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMLogoViewer::onNotifyPause(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMLogoViewer::onNotifyResume(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
}

void VGMLogoViewer::onNotifyUpdate(Obserable& observable)
{
	VGMData& vgmData = (VGMData &)observable;
	const VGMData::PlayInfo& playInfo = vgmData.getPlayInfo();
	const VGMData::BufferInfo& bufferInfo = vgmData.getBufferInfo();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPushMatrix();
	gluOrtho2D(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);

	videoDevice.makeCurrent();
	videoDevice.clear(Color(0.0, 0.0, 0.0, 0.0));
	videoDevice.flush();
	glPopMatrix();
}