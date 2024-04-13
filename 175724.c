static plist_t parse_bin_node_at_index(struct bplist_data *bplist, uint32_t node_index)
{
    int i = 0;
    const char* ptr = NULL;
    plist_t plist = NULL;
    const char* idx_ptr = NULL;

    if (node_index >= bplist->num_objects) {
        PLIST_BIN_ERR("node index (%u) must be smaller than the number of objects (%" PRIu64 ")\n", node_index, bplist->num_objects);
        return NULL;
    }

    idx_ptr = bplist->offset_table + node_index * bplist->offset_size;
    if (idx_ptr < bplist->offset_table ||
        idx_ptr >= bplist->offset_table + bplist->num_objects * bplist->offset_size) {
        PLIST_BIN_ERR("node index %u points outside of valid range\n", node_index);
        return NULL;
    }

    ptr = bplist->data + UINT_TO_HOST(idx_ptr, bplist->offset_size);
    /* make sure the node offset is in a sane range */
    if ((ptr < bplist->data) || (ptr >= bplist->offset_table)) {
        PLIST_BIN_ERR("offset for node index %u points outside of valid range\n", node_index);
        return NULL;
    }

    /* store node_index for current recursion level */
    if (plist_array_get_size(bplist->used_indexes) < bplist->level+1) {
        while (plist_array_get_size(bplist->used_indexes) < bplist->level+1) {
            plist_array_append_item(bplist->used_indexes, plist_new_uint(node_index));
        }
    } else {
        plist_array_set_item(bplist->used_indexes, plist_new_uint(node_index), bplist->level);
    }

    /* recursion check */
    if (bplist->level > 0) {
        for (i = bplist->level-1; i >= 0; i--) {
            plist_t node_i = plist_array_get_item(bplist->used_indexes, i);
            plist_t node_level = plist_array_get_item(bplist->used_indexes, bplist->level);
            if (plist_compare_node_value(node_i, node_level)) {
                PLIST_BIN_ERR("recursion detected in binary plist\n");
                return NULL;
            }
        }
    }

    /* finally parse node */
    bplist->level++;
    plist = parse_bin_node(bplist, &ptr);
    bplist->level--;
    return plist;
}