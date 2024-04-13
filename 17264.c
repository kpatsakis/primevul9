readdirex_checkitem(void *context, void *item)
{
    dict_T	*dict = (dict_T*)item;

    return checkitem_common(context, NULL, dict);
}