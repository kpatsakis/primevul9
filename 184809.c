gdPngWriteData (png_structp png_ptr, png_bytep data, png_size_t length)
{
	gdPutBuf (data, length, (gdIOCtx *) png_get_io_ptr (png_ptr));
}