static plist_t parse_data_node(const char **bnode, uint64_t size)
{
    plist_data_t data = plist_new_plist_data();

    data->type = PLIST_DATA;
    data->length = size;
    data->buff = (uint8_t *) malloc(sizeof(uint8_t) * size);
    if (!data->strval) {
        plist_free_data(data);
        PLIST_BIN_ERR("%s: Could not allocate %" PRIu64 " bytes\n", __func__, sizeof(uint8_t) * size);
        return NULL;
    }
    memcpy(data->buff, *bnode, sizeof(uint8_t) * size);

    return node_create(NULL, data);
}