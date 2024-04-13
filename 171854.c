MONGO_EXPORT int mongo_get_op_timeout(mongo* conn) {
    return conn->op_timeout_ms;
}