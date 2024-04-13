MONGO_EXPORT int mongo_set_op_timeout( mongo *conn, int millis ) {
    conn->op_timeout_ms = millis;
    if( conn->sock && conn->connected )
        mongo_env_set_socket_op_timeout( conn, millis );

    return MONGO_OK;
}