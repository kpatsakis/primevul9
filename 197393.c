void usbredirparser_free_write_buffer(struct usbredirparser *parser,
    uint8_t *data)
{
    free(data);
}