string_has_highlight (const char *string, const char *highlight_words)
{
    char *msg, *highlight, *match, *match_pre, *match_post, *msg_pos;
    char *pos, *pos_end, *ptr_str, *ptr_string_ref;
    int end, length, startswith, endswith, wildcard_start, wildcard_end, flags;

    if (!string || !string[0] || !highlight_words || !highlight_words[0])
        return 0;

    msg = strdup (string);
    if (!msg)
        return 0;
    string_tolower (msg);
    highlight = strdup (highlight_words);
    if (!highlight)
    {
        free (msg);
        return 0;
    }
    string_tolower (highlight);

    pos = highlight;
    end = 0;
    while (!end)
    {
        ptr_string_ref = (char *)string;
        flags = 0;
        pos = (char *)string_regex_flags (pos, REG_ICASE, &flags);

        pos_end = strchr (pos, ',');
        if (!pos_end)
        {
            pos_end = strchr (pos, '\0');
            end = 1;
        }
        /* error parsing string! */
        if (!pos_end)
        {
            free (msg);
            free (highlight);
            return 0;
        }

        if (flags & REG_ICASE)
        {
            for (ptr_str = pos; ptr_str < pos_end; ptr_str++)
            {
                if ((ptr_str[0] >= 'A') && (ptr_str[0] <= 'Z'))
                    ptr_str[0] += ('a' - 'A');
            }
            ptr_string_ref = msg;
        }

        length = pos_end - pos;
        pos_end[0] = '\0';
        if (length > 0)
        {
            if ((wildcard_start = (pos[0] == '*')))
            {
                pos++;
                length--;
            }
            if ((wildcard_end = (*(pos_end - 1) == '*')))
            {
                *(pos_end - 1) = '\0';
                length--;
            }
        }

        if (length > 0)
        {
            msg_pos = ptr_string_ref;
            /* highlight found! */
            while ((match = strstr (msg_pos, pos)) != NULL)
            {
                match_pre = utf8_prev_char (ptr_string_ref, match);
                if (!match_pre)
                    match_pre = match - 1;
                match_post = match + length;
                startswith = ((match == ptr_string_ref) || (!string_is_word_char (match_pre)));
                endswith = ((!match_post[0]) || (!string_is_word_char (match_post)));
                if ((wildcard_start && wildcard_end) ||
                    (!wildcard_start && !wildcard_end &&
                     startswith && endswith) ||
                    (wildcard_start && endswith) ||
                    (wildcard_end && startswith))
                {
                    free (msg);
                    free (highlight);
                    return 1;
                }
                msg_pos = match_post;
            }
        }

        if (!end)
            pos = pos_end + 1;
    }

    free (msg);
    free (highlight);

    /* no highlight found */
    return 0;
}