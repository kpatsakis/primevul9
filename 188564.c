qmp_guest_set_memory_blocks(GuestMemoryBlockList *mem_blks, Error **errp)
{
    GuestMemoryBlockResponseList *head, **link;
    Error *local_err = NULL;

    head = NULL;
    link = &head;

    while (mem_blks != NULL) {
        GuestMemoryBlockResponse *result;
        GuestMemoryBlockResponseList *entry;
        GuestMemoryBlock *current_mem_blk = mem_blks->value;

        result = g_malloc0(sizeof(*result));
        result->phys_index = current_mem_blk->phys_index;
        transfer_memory_block(current_mem_blk, false, result, &local_err);
        if (local_err) { /* should never happen */
            goto err;
        }
        entry = g_malloc0(sizeof *entry);
        entry->value = result;

        *link = entry;
        link = &entry->next;
        mem_blks = mem_blks->next;
    }

    return head;
err:
    qapi_free_GuestMemoryBlockResponseList(head);
    error_propagate(errp, local_err);
    return NULL;
}