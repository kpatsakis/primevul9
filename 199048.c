static bool lru_bump_async(lru_bump_buf *b, item *it, uint32_t hv) {
    bool ret = true;
    refcount_incr(it);
    pthread_mutex_lock(&b->mutex);
    lru_bump_entry *be = (lru_bump_entry *) bipbuf_request(b->buf, sizeof(lru_bump_entry));
    if (be != NULL) {
        be->it = it;
        be->hv = hv;
        if (bipbuf_push(b->buf, sizeof(lru_bump_entry)) == 0) {
            ret = false;
            b->dropped++;
        }
    } else {
        ret = false;
        b->dropped++;
    }
    pthread_mutex_unlock(&b->mutex);
    return ret;
}