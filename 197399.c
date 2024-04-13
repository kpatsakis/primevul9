void usbredirparser_free_packet_data(struct usbredirparser *parser,
    uint8_t *data)
{
    free(data);
}