MONGO_EXPORT int mongo_create_index( mongo *conn, const char *ns, const bson *key, int options, bson *out ) {
    bson b;
    bson_iterator it;
    char name[255] = {'_'};
    int i = 1;
    char idxns[1024];

    bson_iterator_init( &it, key );
    while( i < 255 && bson_iterator_next( &it ) ) {
        strncpy( name + i, bson_iterator_key( &it ), 255 - i );
        i += strlen( bson_iterator_key( &it ) );
    }
    name[254] = '\0';

    bson_init( &b );
    bson_append_bson( &b, "key", key );
    bson_append_string( &b, "ns", ns );
    bson_append_string( &b, "name", name );
    if ( options & MONGO_INDEX_UNIQUE )
        bson_append_bool( &b, "unique", 1 );
    if ( options & MONGO_INDEX_DROP_DUPS )
        bson_append_bool( &b, "dropDups", 1 );
    if ( options & MONGO_INDEX_BACKGROUND )
        bson_append_bool( &b, "background", 1 );
    if ( options & MONGO_INDEX_SPARSE )
        bson_append_bool( &b, "sparse", 1 );
    bson_finish( &b );

    strncpy( idxns, ns, 1024-16 );
    strcpy( strchr( idxns, '.' ), ".system.indexes" );
    mongo_insert( conn, idxns, &b, NULL );
    bson_destroy( &b );

    *strchr( idxns, '.' ) = '\0'; /* just db not ns */
    return mongo_cmd_get_last_error( conn, idxns, out );
}