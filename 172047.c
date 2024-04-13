MONGO_EXPORT bson_type bson_find( bson_iterator *it, const bson *obj, const char *name ) {
    bson_iterator_init( it, (bson *)obj );
    while( bson_iterator_next( it ) ) {
        if ( strcmp( name, bson_iterator_key( it ) ) == 0 )
            break;
    }
    return bson_iterator_type( it );
}