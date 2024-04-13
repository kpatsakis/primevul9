string_decode_base64 (const char *from, char *to)
{
    const char *ptr_from;
    int length, to_length, i;
    char *ptr_to;
    unsigned char c, in[4], out[3];
    unsigned char base64_table[]="|$$$}rstuvwxyz{$$$$$$$>?"
        "@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

    ptr_from = from;
    ptr_to = to;

    ptr_to[0] = '\0';
    to_length = 0;

    while (ptr_from && ptr_from[0])
    {
        length = 0;
        for (i = 0; i < 4; i++)
        {
            c = 0;
            while (ptr_from[0] && (c == 0))
            {
                c = (unsigned char) ptr_from[0];
                ptr_from++;
                c = ((c < 43) || (c > 122)) ? 0 : base64_table[c - 43];
                if (c)
                    c = (c == '$') ? 0 : c - 61;
            }
            if (ptr_from[0])
            {
                length++;
                if (c)
                    in[i] = c - 1;
            }
            else
            {
                in[i] = '\0';
                break;
            }
        }
        if (length)
        {
            string_convbase64_6x4_to_8x3 (in, out);
            for (i = 0; i < length - 1; i++)
            {
                ptr_to[0] = out[i];
                ptr_to++;
                to_length++;
            }
        }
    }

    ptr_to[0] = '\0';

    return to_length;
}