int bson_check_string( bson *b, const char *string,
                       const size_t length ) {

    return bson_validate_string( b, ( const unsigned char * )string, length, 1, 0, 0 );
}