void usbredirparser_send_filter_filter(struct usbredirparser *parser_pub,
    const struct usbredirfilter_rule *rules, int rules_count)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    char *str;

    if (!usbredirparser_peer_has_cap(parser_pub, usb_redir_cap_filter))
        return;

    str = usbredirfilter_rules_to_string(rules, rules_count, ",", "|");
    if (!str) {
        ERROR("error creating filter string, not sending filter");
        return;
    }
    usbredirparser_queue(parser_pub, usb_redir_filter_filter, 0, NULL,
                         (uint8_t *)str, strlen(str) + 1);
    free(str);
}