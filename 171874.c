void bson_numstr( char *str, int i ) {
    if( i < 1000 )
        memcpy( str, bson_numstrs[i], 4 );
    else
        bson_sprintf( str,"%d", i );
}