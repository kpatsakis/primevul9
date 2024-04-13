bio_to_buffer( BIO *bio, char **buffer, size_t *buffer_len )
{
	if ( bio == NULL ) {
		return FALSE;
	}

	*buffer_len = BIO_pending( bio );

	*buffer = (char *)malloc( *buffer_len );
	if ( *buffer == NULL ) {
		return FALSE;
	}

	if ( BIO_read( bio, *buffer, *buffer_len ) < (int)*buffer_len ) {
		free( *buffer );
		return FALSE;
	}

	return TRUE;
}