int usbredirparser_have_peer_caps(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;

    return parser->have_peer_caps;
}