string_mask_to_regex (const char *mask)
{
    char *result;
    const char *ptr_mask;
    int index_result;
    char *regex_special_char = ".[]{}()?+";

    if (!mask)
        return NULL;

    result = malloc ((strlen (mask) * 2) + 1);
    if (!result)
        return NULL;

    result[0] = '\0';
    index_result = 0;
    ptr_mask = mask;
    while (ptr_mask[0])
    {
        /* '*' in string ? then replace by '.*' */
        if (ptr_mask[0] == '*')
        {
            result[index_result++] = '.';
            result[index_result++] = '*';
        }
        /* special regex char in string ? escape it with '\' */
        else if (strchr (regex_special_char, ptr_mask[0]))
        {
            result[index_result++] = '\\';
            result[index_result++] = ptr_mask[0];
        }
        /* standard char, just copy it */
        else
            result[index_result++] = ptr_mask[0];

        ptr_mask++;
    }

    /* add final '\0' */
    result[index_result] = '\0';

    return result;
}