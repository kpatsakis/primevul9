string_strip (const char *string, int left, int right, const char *chars)
{
    const char *ptr_start, *ptr_end;

    if (!string)
        return NULL;

    if (!string[0])
        return strdup (string);

    ptr_start = string;
    ptr_end = string + strlen (string) - 1;

    if (left)
    {
        while (ptr_start[0] && strchr (chars, ptr_start[0]))
        {
            ptr_start++;
        }
        if (!ptr_start[0])
            return strdup (ptr_start);
    }

    if (right)
    {
        while ((ptr_end >= ptr_start) && strchr (chars, ptr_end[0]))
        {
            ptr_end--;
        }
        if (ptr_end < ptr_start)
            return strdup ("");
    }

    return string_strndup (ptr_start, ptr_end - ptr_start + 1);
}