static void add_keywords(strarray_t *flags, json_t *set_keywords, int add)
{
    int i;

    for (i = 0; i < strarray_size(flags); i++) {
        const char *flag = strarray_nth(flags, i);

        if (!strcasecmp(flag, "\\Seen")) flag = "$Seen";
        else if (!strcasecmp(flag, "\\Flagged")) flag = "$Flagged";
        else if (!strcasecmp(flag, "\\Answered")) flag = "$Answered";
        else if (!strcasecmp(flag, "\\Draft")) flag = "$Draft";

        json_object_set_new(set_keywords, flag,
                            add ? json_true() : json_false());
    }
}