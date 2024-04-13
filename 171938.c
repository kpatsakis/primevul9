MONGO_EXPORT int mongo_get_err(mongo* conn) {
    return conn->err;
}