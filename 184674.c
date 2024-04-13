static RAMBlock *find_ram_block(ram_addr_t addr)
{
    RAMBlock *block;

    QLIST_FOREACH_RCU(block, &ram_list.blocks, next) {
        if (block->offset == addr) {
            return block;
        }
    }

    return NULL;
}