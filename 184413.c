static int dump_tag_item_to_file (WavpackContext *wpc, const char *tag_item, FILE *dst, char *fname)
{
    const char *sanitized_tag_item = filespec_name ((char *) tag_item) ? filespec_name ((char *) tag_item) : tag_item;

    if (WavpackGetMode (wpc) & MODE_VALID_TAG) {
        if (WavpackGetTagItem (wpc, tag_item, NULL, 0)) {
            int value_len = WavpackGetTagItem (wpc, tag_item, NULL, 0);
            char *value;

            if (fname) {
                snprintf (fname, 256, "%s.txt", sanitized_tag_item);
                fname [255] = 0;
            }

            if (!value_len || !dst)
                return value_len;

#if defined(_WIN32)
            _setmode (_fileno (dst), O_TEXT);
#endif
#if defined(__OS2__)
            setmode (fileno (dst), O_TEXT);
#endif
            value = malloc (value_len * 2 + 1);
            WavpackGetTagItem (wpc, tag_item, value, value_len + 1);
            dump_UTF8_string (value, dst);
            free (value);
            return value_len;
        }
        else if (WavpackGetBinaryTagItem (wpc, tag_item, NULL, 0)) {
            int value_len = WavpackGetBinaryTagItem (wpc, tag_item, NULL, 0), res = 0, i;
            uint32_t bcount = 0;
            char *value;

            value = malloc (value_len);
            WavpackGetBinaryTagItem (wpc, tag_item, value, value_len);

            for (i = 0; i < value_len; ++i)
                if (!value [i]) {

                    if (dst) {
#if defined(_WIN32)
                        _setmode (_fileno (dst), O_BINARY);
#endif
#if defined(__OS2__)
                        setmode (fileno (dst), O_BINARY);
#endif
                        res = DoWriteFile (dst, (unsigned char *) value + i + 1, value_len - i - 1, &bcount);
                    }

                    if (fname) {
                        char *sanitized_tag_value = filespec_name (value) ? filespec_name (value) : value;

                        if (strlen (sanitized_tag_value) < 256)
                            strcpy (fname, sanitized_tag_value);
                        else {
                            snprintf (fname, 256, "%s.bin", sanitized_tag_item);
                            fname [255] = 0;
                        }
                    }

                    break;
                }

            free (value);

            if (i == value_len)
                return 0;

            if (dst && (!res || bcount != value_len - i - 1))
                return 0;

            return value_len - i - 1;
        }
        else
            return 0;
    }
    else
        return 0;
}