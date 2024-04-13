string_encode_base64 (const char *from, int length, char *to)
{
    const char *ptr_from;
    char *ptr_to;

    ptr_from = from;
    ptr_to = to;

    while (length >= 3)
    {
        string_convbase64_8x3_to_6x4 (ptr_from, ptr_to);
        ptr_from += 3 * sizeof (*ptr_from);
        ptr_to += 4 * sizeof (*ptr_to);
        length -= 3;
    }

    if (length > 0)
    {
        char rest[3] = { 0, 0, 0 };
        switch (length)
        {
            case 1 :
                rest[0] = ptr_from[0];
                string_convbase64_8x3_to_6x4 (rest, ptr_to);
                ptr_to[2] = ptr_to[3] = '=';
                break;
            case 2 :
                rest[0] = ptr_from[0];
                rest[1] = ptr_from[1];
                string_convbase64_8x3_to_6x4 (rest, ptr_to);
                ptr_to[3] = '=';
                break;
        }
        ptr_to[4] = 0;
    }
    else
        ptr_to[0] = '\0';
}