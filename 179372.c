_tiff_pop_handlers (void)
{
	TIFFSetErrorHandler (orig_error_handler);
	TIFFSetWarningHandler (orig_warning_handler);
}