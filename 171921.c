MONGO_EXPORT const char*  mongo_get_server_err_string(mongo* conn) {
    return conn->lasterrstr;
}