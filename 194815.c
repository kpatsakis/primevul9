string_has_highlight_regex (const char *string, const char *regex)
{
    regex_t reg;
    int rc;

    if (!string || !regex || !regex[0])
        return 0;

    if (string_regcomp (&reg, regex, REG_EXTENDED | REG_ICASE) != 0)
        return 0;

    rc = string_has_highlight_regex_compiled (string, &reg);

    regfree (&reg);

    return rc;
}