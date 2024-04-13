png_error_callback (png_structp png_ptr,
		    png_const_charp error_msg)
{
	char **msg;

	msg = png_get_error_ptr (png_ptr);
	*msg = g_strdup (error_msg);
	longjmp (png_jmpbuf (png_ptr), 1);
}