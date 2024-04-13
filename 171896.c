MONGO_EXPORT void bson_oid_gen( bson_oid_t *oid ) {
    static int incr = 0;
    static int fuzz = 0;
    int i;
    time_t t = time( NULL );

    if( oid_inc_func )
        i = oid_inc_func();
    else
        i = incr++;

    if ( !fuzz ) {
        if ( oid_fuzz_func )
            fuzz = oid_fuzz_func();
        else {
            srand( ( int )t );
            fuzz = rand();
        }
    }

    bson_big_endian32( &oid->ints[0], &t );
    oid->ints[1] = fuzz;
    bson_big_endian32( &oid->ints[2], &i );
}