static void usbredirparser_verify_caps(struct usbredirparser_priv *parser,
    uint32_t *caps, const char *desc)
{
    if (usbredirparser_caps_get_cap(parser, caps,
                                    usb_redir_cap_bulk_streams) &&
        !usbredirparser_caps_get_cap(parser, caps,
                                     usb_redir_cap_ep_info_max_packet_size)) {
        ERROR("error %s caps contains cap_bulk_streams without"
              "cap_ep_info_max_packet_size", desc);
        caps[0] &= ~(1 << usb_redir_cap_bulk_streams);
    }
}