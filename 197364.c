int usbredirparser_has_data_to_write(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    return parser->write_buf_count;
}