static int mongo_bson_valid( mongo *conn, const bson *bson, int write ) {
    int size;

    if( ! bson->finished ) {
        conn->err = MONGO_BSON_NOT_FINISHED;
        return MONGO_ERROR;
    }

    size = bson_size( bson );
    if( size > conn->max_bson_size ) {
        conn->err = MONGO_BSON_TOO_LARGE;
        return MONGO_ERROR;
    }

    if( bson->err & BSON_NOT_UTF8 ) {
        conn->err = MONGO_BSON_INVALID;
        return MONGO_ERROR;
    }

    if( write ) {
        if( ( bson->err & BSON_FIELD_HAS_DOT ) ||
                ( bson->err & BSON_FIELD_INIT_DOLLAR ) ) {

            conn->err = MONGO_BSON_INVALID;
            return MONGO_ERROR;

        }
    }

    conn->err = 0;

    return MONGO_OK;
}