static plist_t parse_uid_node(const char **bnode, uint8_t size)
{
    plist_data_t data = plist_new_plist_data();
    size = size + 1;
    data->intval = UINT_TO_HOST(*bnode, size);
    if (data->intval > UINT32_MAX) {
        PLIST_BIN_ERR("%s: value %" PRIu64 " too large for UID node (must be <= %u)\n", __func__, (uint64_t)data->intval, UINT32_MAX);
        free(data);
        return NULL;
    }

    (*bnode) += size;
    data->type = PLIST_UID;
    data->length = sizeof(uint64_t);

    return node_create(NULL, data);
}