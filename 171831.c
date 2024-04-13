MONGO_EXPORT int mongo_get_socket(mongo* conn) {
    mongo* conn_ = (mongo*)conn;
    return conn_->sock;
}