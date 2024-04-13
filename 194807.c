string_iconv (int from_utf8, const char *from_code, const char *to_code,
              const char *string)
{
    char *outbuf;

#ifdef HAVE_ICONV
    iconv_t cd;
    char *inbuf, *ptr_inbuf, *ptr_outbuf, *next_char;
    char *ptr_inbuf_shift;
    int done;
    size_t err, inbytesleft, outbytesleft;

    if (from_code && from_code[0] && to_code && to_code[0]
        && (string_strcasecmp(from_code, to_code) != 0))
    {
        cd = iconv_open (to_code, from_code);
        if (cd == (iconv_t)(-1))
            outbuf = strdup (string);
        else
        {
            inbuf = strdup (string);
            if (!inbuf)
                return NULL;
            ptr_inbuf = inbuf;
            inbytesleft = strlen (inbuf);
            outbytesleft = inbytesleft * 4;
            outbuf = malloc (outbytesleft + 2);
            if (!outbuf)
                return inbuf;
            ptr_outbuf = outbuf;
            ptr_inbuf_shift = NULL;
            done = 0;
            while (!done)
            {
                err = iconv (cd, (ICONV_CONST char **)(&ptr_inbuf), &inbytesleft,
                             &ptr_outbuf, &outbytesleft);
                if (err == (size_t)(-1))
                {
                    switch (errno)
                    {
                        case EINVAL:
                            done = 1;
                            break;
                        case E2BIG:
                            done = 1;
                            break;
                        case EILSEQ:
                            if (from_utf8)
                            {
                                next_char = utf8_next_char (ptr_inbuf);
                                if (next_char)
                                {
                                    inbytesleft -= next_char - ptr_inbuf;
                                    ptr_inbuf = next_char;
                                }
                                else
                                {
                                    inbytesleft--;
                                    ptr_inbuf++;
                                }
                            }
                            else
                            {
                                ptr_inbuf++;
                                inbytesleft--;
                            }
                            ptr_outbuf[0] = '?';
                            ptr_outbuf++;
                            outbytesleft--;
                            break;
                    }
                }
                else
                {
                    if (!ptr_inbuf_shift)
                    {
                        ptr_inbuf_shift = ptr_inbuf;
                        ptr_inbuf = NULL;
                        inbytesleft = 0;
                    }
                    else
                        done = 1;
                }
            }
            if (ptr_inbuf_shift)
                ptr_inbuf = ptr_inbuf_shift;
            ptr_outbuf[0] = '\0';
            free (inbuf);
            iconv_close (cd);
        }
    }
    else
        outbuf = strdup (string);
#else
    /* make C compiler happy */
    (void) from_utf8;
    (void) from_code;
    (void) to_code;
    outbuf = strdup (string);
#endif /* HAVE_ICONV */

    return outbuf;
}