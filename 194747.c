string_iconv_fprintf (FILE *file, const char *data, ...)
{
    char *buf2;
    int rc, num_written;

    rc = 0;
    weechat_va_format (data);
    if (vbuffer)
    {
        buf2 = string_iconv_from_internal (NULL, vbuffer);
        num_written = fprintf (file, "%s", (buf2) ? buf2 : vbuffer);
        rc = (num_written == (int)strlen ((buf2) ? buf2 : vbuffer)) ? 1 : 0;
        if (buf2)
            free (buf2);
        free (vbuffer);
    }

    return rc;
}