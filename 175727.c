static plist_t parse_unicode_node(const char **bnode, uint64_t size)
{
    plist_data_t data = plist_new_plist_data();
    uint64_t i = 0;
    uint16_t *unicodestr = NULL;
    char *tmpstr = NULL;
    long items_read = 0;
    long items_written = 0;

    data->type = PLIST_STRING;
    unicodestr = (uint16_t*) malloc(sizeof(uint16_t) * size);
    if (!unicodestr) {
        plist_free_data(data);
        PLIST_BIN_ERR("%s: Could not allocate %" PRIu64 " bytes\n", __func__, sizeof(uint16_t) * size);
        return NULL;
    }
    for (i = 0; i < size; i++)
        unicodestr[i] = be16toh(((uint16_t*)*bnode)[i]);

    tmpstr = plist_utf16_to_utf8(unicodestr, size, &items_read, &items_written);
    free(unicodestr);
    if (!tmpstr) {
        plist_free_data(data);
        return NULL;
    }
    tmpstr[items_written] = '\0';

    data->type = PLIST_STRING;
    data->strval = realloc(tmpstr, items_written+1);
    if (!data->strval)
        data->strval = tmpstr;
    data->length = items_written;
    return node_create(NULL, data);
}