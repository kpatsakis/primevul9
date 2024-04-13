BOOL INSTAPI SQLWriteFileDSNW(LPCWSTR  lpszFileName,
                              LPCWSTR  lpszAppName,
                              LPCWSTR  lpszKeyName,
                              LPCWSTR  lpszString)
{
	BOOL ret;
	char *file;
	char *app;
	char *key;
	char *str;

	file = lpszFileName ? _single_string_alloc_and_copy( lpszFileName ) : (char*)NULL;
	app = lpszAppName ? _single_string_alloc_and_copy( lpszAppName ) : (char*)NULL;
	key = lpszKeyName ? _single_string_alloc_and_copy( lpszKeyName ) : (char*)NULL;
	str = lpszString ? _single_string_alloc_and_copy( lpszString ) : (char*)NULL;

	ret = SQLWriteFileDSN( file, app, key, str );

	if ( file )
		free( file );
	if ( app )
		free( app );
	if ( key )
		free( key );
	if ( str )
		free( str );

	return ret;
}