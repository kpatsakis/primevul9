MONGO_EXPORT bson_err_handler set_bson_err_handler( bson_err_handler func ) {
    bson_err_handler old = err_handler;
    err_handler = func;
    return old;
}