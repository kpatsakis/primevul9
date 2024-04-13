static const char* _get_host_port(mongo_host_port* hp) {
    static char _hp[sizeof(hp->host)+12];
    bson_sprintf(_hp, "%s:%d", hp->host, hp->port);
    return _hp;
}