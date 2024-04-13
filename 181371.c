void v9fs_reset(V9fsState *s)
{
    VirtfsCoResetData data = { .pdu = { .s = s }, .done = false };
    Coroutine *co;

    while (!QLIST_EMPTY(&s->active_list)) {
        aio_poll(qemu_get_aio_context(), true);
    }

    co = qemu_coroutine_create(virtfs_co_reset, &data);
    qemu_coroutine_enter(co);

    while (!data.done) {
        aio_poll(qemu_get_aio_context(), true);
    }
}