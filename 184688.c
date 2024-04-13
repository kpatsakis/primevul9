int qemu_ram_foreach_block(RAMBlockIterFunc func, void *opaque)
{
    RAMBlock *block;
    int ret = 0;

    rcu_read_lock();
    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        ret = func(block->idstr, block->host, block->offset,
                   block->used_length, opaque);
        if (ret) {
            break;
        }
    }
    rcu_read_unlock();
    return ret;
}