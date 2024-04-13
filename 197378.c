int usbredirparser_have_cap(struct usbredirparser *parser_pub, int cap)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    return usbredirparser_caps_get_cap(parser, parser->our_caps, cap);
}