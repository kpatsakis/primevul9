readdir_checkitem(void *context, void *item)
{
    char_u	*name = (char_u *)item;

    return checkitem_common(context, name, NULL);
}