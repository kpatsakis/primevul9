MONGO_EXPORT const char* mongo_get_host(mongo* conn, int i) {
    mongo_replica_set* r = conn->replica_set;
    mongo_host_port* hp;
    int count = 0;
    if (!r) return 0;
    for (hp = r->hosts; hp; hp = hp->next) {
        if (count == i)
            return _get_host_port(hp);
        ++count;
    }
    return 0;
}