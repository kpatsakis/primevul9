MONGO_EXPORT int mongo_get_host_count(mongo* conn) {
    mongo_replica_set* r = conn->replica_set;
    mongo_host_port* hp;
    int count = 0;
    if (!r) return 0;
    for (hp = r->hosts; hp; hp = hp->next)
        ++count;
    return count;
}