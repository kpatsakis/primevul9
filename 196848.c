static void conn_release_items(conn *c) {
    assert(c != NULL);

    if (c->item) {
        item_remove(c->item);
        c->item = 0;
    }

    while (c->ileft > 0) {
        item *it = *(c->icurr);
        assert((it->it_flags & ITEM_SLABBED) == 0);
        item_remove(it);
        c->icurr++;
        c->ileft--;
    }

    if (c->suffixleft != 0) {
        for (; c->suffixleft > 0; c->suffixleft--, c->suffixcurr++) {
            do_cache_free(c->thread->suffix_cache, *(c->suffixcurr));
        }
    }

    c->icurr = c->ilist;
    c->suffixcurr = c->suffixlist;
}