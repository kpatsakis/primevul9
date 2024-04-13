static void item_link_q_warm(item *it) {
    pthread_mutex_lock(&lru_locks[it->slabs_clsid]);
    do_item_link_q(it);
    itemstats[it->slabs_clsid].moves_to_warm++;
    pthread_mutex_unlock(&lru_locks[it->slabs_clsid]);
}