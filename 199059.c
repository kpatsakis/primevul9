static void lru_bump_buf_link_q(lru_bump_buf *b) {
    pthread_mutex_lock(&bump_buf_lock);
    assert(b != bump_buf_head);

    b->prev = 0;
    b->next = bump_buf_head;
    if (b->next) b->next->prev = b;
    bump_buf_head = b;
    if (bump_buf_tail == 0) bump_buf_tail = b;
    pthread_mutex_unlock(&bump_buf_lock);
    return;
}