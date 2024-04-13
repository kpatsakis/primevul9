MONGO_EXPORT const char* mongo_get_primary(mongo* conn) {
    mongo* conn_ = (mongo*)conn;
    if( !(conn_->connected) || (conn_->primary->host[0] == '\0') )
        return NULL;
    return _get_host_port(conn_->primary);
}