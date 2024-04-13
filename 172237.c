buffer_to_bio( char *buffer, size_t buffer_len, BIO **bio )
{
	if ( buffer == NULL ) {
		return FALSE;
	}

	*bio = BIO_new( BIO_s_mem() );
	if ( *bio == NULL ) {
		return FALSE;
	}

	if ( BIO_write( *bio, buffer, buffer_len ) < (int)buffer_len ) {
		BIO_free( *bio );
		return FALSE;
	}

	return TRUE;
}