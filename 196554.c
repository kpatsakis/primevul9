progress( int c )
{
    if ( progress_cb )
	progress_cb ( progress_cb_data, c );
    else
	fputc( c, stderr );
}