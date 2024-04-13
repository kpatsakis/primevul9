gdPngReadData (png_structp png_ptr, png_bytep data, png_size_t length)
{
	int check;
	check = gdGetBuf (data, length, (gdIOCtx *) png_get_io_ptr (png_ptr));
	if (check != (int)length) {
		png_error(png_ptr, "Read Error: truncated data");
	}
}