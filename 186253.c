static void process_stat_settings(ADD_STAT add_stats, void *c) {
    assert(add_stats);
    APPEND_STAT("maxbytes", "%llu", (unsigned long long)settings.maxbytes);
    APPEND_STAT("maxconns", "%d", settings.maxconns);
    APPEND_STAT("tcpport", "%d", settings.port);
    APPEND_STAT("udpport", "%d", settings.udpport);
    APPEND_STAT("inter", "%s", settings.inter ? settings.inter : "NULL");
    APPEND_STAT("verbosity", "%d", settings.verbose);
    APPEND_STAT("oldest", "%lu", (unsigned long)settings.oldest_live);
    APPEND_STAT("evictions", "%s", settings.evict_to_free ? "on" : "off");
    APPEND_STAT("domain_socket", "%s",
                settings.socketpath ? settings.socketpath : "NULL");
    APPEND_STAT("umask", "%o", settings.access);
    APPEND_STAT("growth_factor", "%.2f", settings.factor);
    APPEND_STAT("chunk_size", "%d", settings.chunk_size);
    APPEND_STAT("num_threads", "%d", settings.num_threads);
    APPEND_STAT("num_threads_per_udp", "%d", settings.num_threads_per_udp);
    APPEND_STAT("stat_key_prefix", "%c", settings.prefix_delimiter);
    APPEND_STAT("detail_enabled", "%s",
                settings.detail_enabled ? "yes" : "no");
    APPEND_STAT("reqs_per_event", "%d", settings.reqs_per_event);
    APPEND_STAT("cas_enabled", "%s", settings.use_cas ? "yes" : "no");
    APPEND_STAT("tcp_backlog", "%d", settings.backlog);
    APPEND_STAT("binding_protocol", "%s",
                prot_text(settings.binding_protocol));
    APPEND_STAT("auth_enabled_sasl", "%s", settings.sasl ? "yes" : "no");
    APPEND_STAT("item_size_max", "%d", settings.item_size_max);
    APPEND_STAT("maxconns_fast", "%s", settings.maxconns_fast ? "yes" : "no");
    APPEND_STAT("hashpower_init", "%d", settings.hashpower_init);
    APPEND_STAT("slab_reassign", "%s", settings.slab_reassign ? "yes" : "no");
    APPEND_STAT("slab_automove", "%d", settings.slab_automove);
}