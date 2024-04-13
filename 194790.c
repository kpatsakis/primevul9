string_has_highlight_regex_compiled (const char *string, regex_t *regex)
{
    int rc, startswith, endswith;
    regmatch_t regex_match;
    const char *match_pre;

    if (!string || !regex)
        return 0;

    while (string && string[0])
    {
        rc = regexec (regex, string,  1, &regex_match, 0);
        if ((rc != 0) || (regex_match.rm_so < 0) || (regex_match.rm_eo < 0))
            break;

        startswith = (regex_match.rm_so == 0);
        if (!startswith)
        {
            match_pre = utf8_prev_char (string, string + regex_match.rm_so);
            startswith = !string_is_word_char (match_pre);
        }
        endswith = 0;
        if (startswith)
        {
            endswith = ((regex_match.rm_eo == (int)strlen (string))
                        || !string_is_word_char (string + regex_match.rm_eo));
        }
        if (startswith && endswith)
            return 1;

        string += regex_match.rm_eo;
    }

    /* no highlight found */
    return 0;
}