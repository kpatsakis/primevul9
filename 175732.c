static plist_t parse_real_node(const char **bnode, uint8_t size)
{
    plist_data_t data = plist_new_plist_data();
    uint8_t buf[8];

    size = 1 << size;			// make length less misleading
    switch (size)
    {
    case sizeof(uint32_t):
        *(uint32_t*)buf = float_bswap32(*(uint32_t*)*bnode);
        data->realval = *(float *) buf;
        break;
    case sizeof(uint64_t):
        *(uint64_t*)buf = float_bswap64(*(uint64_t*)*bnode);
        data->realval = *(double *) buf;
        break;
    default:
        free(data);
        PLIST_BIN_ERR("%s: Invalid byte size for real node\n", __func__);
        return NULL;
    }
    data->type = PLIST_REAL;
    data->length = sizeof(double);

    return node_create(NULL, data);
}