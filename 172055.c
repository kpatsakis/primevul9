static int mongo_cursor_bson_valid( mongo_cursor *cursor, const bson *bson ) {
    if( ! bson->finished ) {
        cursor->err = MONGO_CURSOR_BSON_ERROR;
        cursor->conn->err = MONGO_BSON_NOT_FINISHED;
        return MONGO_ERROR;
    }

    if( bson->err & BSON_NOT_UTF8 ) {
        cursor->err = MONGO_CURSOR_BSON_ERROR;
        cursor->conn->err = MONGO_BSON_INVALID;
        return MONGO_ERROR;
    }

    return MONGO_OK;
}