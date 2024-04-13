set_error_string( const char *message )
{
	if ( _globus_error_message ) {
		free( const_cast<char *>(_globus_error_message) );
	}
	_globus_error_message = strdup( message );
}