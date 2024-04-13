void Dispatcher::parse_params( void )
{
  if ( parsed ) {
    return;
  }

  parsed_params.clear();
  const char *str = params.c_str();
  const char *segment_begin = str;

  while ( 1 ) {
    const char *segment_end = strchr( segment_begin, ';' );
    if ( segment_end == NULL ) {
      break;
    }

    errno = 0;
    char *endptr;
    int val = strtol( segment_begin, &endptr, 10 );
    if ( endptr == segment_begin ) {
      val = -1;
    }
    if ( errno == 0 || segment_begin == endptr ) {
      parsed_params.push_back( val );
    }

    segment_begin = segment_end + 1;
  }

  /* get last param */
  errno = 0;
  char *endptr;
  int val = strtol( segment_begin, &endptr, 10 );
  if ( endptr == segment_begin ) {
    val = -1;
  }
  if ( errno == 0 || segment_begin == endptr ) {
    parsed_params.push_back( val );
  }

  parsed = true;
}