MONGO_EXPORT int  mongo_get_server_err(mongo* conn) {
    return conn->lasterrcode;
}