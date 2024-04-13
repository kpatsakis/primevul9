static unsigned int temp_lru_size(int slabs_clsid) {
    int id = CLEAR_LRU(slabs_clsid);
    id |= TEMP_LRU;
    unsigned int ret;
    pthread_mutex_lock(&lru_locks[id]);
    ret = sizes_bytes[id];
    pthread_mutex_unlock(&lru_locks[id]);
    return ret;
}