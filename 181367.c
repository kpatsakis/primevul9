static void coroutine_fn virtfs_co_reset(void *opaque)
{
    VirtfsCoResetData *data = opaque;

    virtfs_reset(&data->pdu);
    data->done = true;
}