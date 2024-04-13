static int mongo_choose_write_concern( mongo *conn,
                                       mongo_write_concern *custom_write_concern,
                                       mongo_write_concern **write_concern ) {

    if( custom_write_concern ) {
        *write_concern = custom_write_concern;
    }
    else if( conn->write_concern ) {
        *write_concern = conn->write_concern;
    }
    if ( *write_concern && (*write_concern)->w < 1 ) {
        *write_concern = 0; /* do not generate getLastError request */
    }
    if( *write_concern && !((*write_concern)->cmd) ) {
        __mongo_set_error( conn, MONGO_WRITE_CONCERN_INVALID,
                           "Must call mongo_write_concern_finish() before using *write_concern.", 0 );
        return MONGO_ERROR;
    }
    else
        return MONGO_OK;
}