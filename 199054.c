static bool lru_maintainer_bumps(void) {
    lru_bump_buf *b;
    lru_bump_entry *be;
    unsigned int size;
    unsigned int todo;
    bool bumped = false;
    pthread_mutex_lock(&bump_buf_lock);
    for (b = bump_buf_head; b != NULL; b=b->next) {
        pthread_mutex_lock(&b->mutex);
        be = (lru_bump_entry *) bipbuf_peek_all(b->buf, &size);
        pthread_mutex_unlock(&b->mutex);

        if (be == NULL) {
            continue;
        }
        todo = size;
        bumped = true;

        while (todo) {
            item_lock(be->hv);
            do_item_update(be->it);
            do_item_remove(be->it);
            item_unlock(be->hv);
            be++;
            todo -= sizeof(lru_bump_entry);
        }

        pthread_mutex_lock(&b->mutex);
        be = (lru_bump_entry *) bipbuf_poll(b->buf, size);
        pthread_mutex_unlock(&b->mutex);
    }
    pthread_mutex_unlock(&bump_buf_lock);
    return bumped;
}