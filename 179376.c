_read_png (png_structp png_ptr,
	   png_bytep data,
	   png_size_t len)
{
	GInputStream *stream;

	stream = png_get_io_ptr (png_ptr);
	g_input_stream_read (stream, data, len, NULL, NULL);
}