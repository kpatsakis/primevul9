void *item_lru_bump_buf_create(void) {
    lru_bump_buf *b = calloc(1, sizeof(lru_bump_buf));
    if (b == NULL) {
        return NULL;
    }

    b->buf = bipbuf_new(sizeof(lru_bump_entry) * LRU_BUMP_BUF_SIZE);
    if (b->buf == NULL) {
        free(b);
        return NULL;
    }

    pthread_mutex_init(&b->mutex, NULL);

    lru_bump_buf_link_q(b);
    return b;
}