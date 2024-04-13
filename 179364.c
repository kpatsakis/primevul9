_tiff_push_handlers (void)
{
	orig_error_handler = TIFFSetErrorHandler (_tiff_error_handler);
	orig_warning_handler = TIFFSetWarningHandler (NULL);
}