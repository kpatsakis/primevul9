MONGO_EXPORT void bson_print( const bson *b ) {
    bson_print_raw( b->data , 0 );
}