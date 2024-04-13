tiff_set_handlers (void)
{
        TIFFSetErrorHandler (tiff_warning_handler);
        TIFFSetWarningHandler (tiff_warning_handler);
}