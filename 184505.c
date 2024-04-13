int jio_fprintf(FILE * stream, const char * format, ...) {
	va_list args;

	Trc_SC_fprintf_Entry();

	va_start( args, format );
	vfprintf(stream, format, args);
	va_end( args );

	Trc_SC_fprintf_Exit(0);

	return 0;
}