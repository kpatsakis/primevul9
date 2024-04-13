MONGO_EXPORT int mongo_is_connected(mongo* conn) {
    return conn->connected != 0;
}