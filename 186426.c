static void *php_libxml_streams_IO_open_write_wrapper(const char *filename)
{
	return php_libxml_streams_IO_open_wrapper(filename, "wb", 0);
}