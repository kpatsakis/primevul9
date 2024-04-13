static PHP_GINIT_FUNCTION(iconv)
{
	iconv_globals->input_encoding = NULL;
	iconv_globals->output_encoding = NULL;
	iconv_globals->internal_encoding = NULL;
}