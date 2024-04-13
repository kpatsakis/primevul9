jio_vfprintf(FILE * stream, const char * format, va_list args)
{

	Trc_SC_vfprintf_Entry(stream, format);

	vfprintf(stream, format, args);

	Trc_SC_vfprintf_Exit(0);

	return 0;
}