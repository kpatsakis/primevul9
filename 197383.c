struct usbredirparser *usbredirparser_create(void)
{
    return calloc(1, sizeof(struct usbredirparser_priv));
}