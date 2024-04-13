MONGO_EXPORT void mongo_clear_errors( mongo *conn ) {
    conn->err = 0;
    conn->errcode = 0;
    conn->lasterrcode = 0;
    memset( conn->errstr, 0, MONGO_ERR_LEN );
    memset( conn->lasterrstr, 0, MONGO_ERR_LEN );
}