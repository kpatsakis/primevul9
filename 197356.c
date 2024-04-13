static int usbredirparser_caps_get_cap(struct usbredirparser_priv *parser,
    uint32_t *caps, int cap)
{
    if (cap / 32 >= USB_REDIR_CAPS_SIZE) {
        ERROR("error request for out of bounds cap: %d", cap);
        return 0;
    }
    if (caps[cap / 32] & (1 << (cap % 32))) {
        return 1;
    } else {
        return 0;
    }
}