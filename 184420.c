static void parse_sample_time_index (struct sample_time_index *dst, char *src)
{
    int colons = 0;
    double temp;

    memset (dst, 0, sizeof (*dst));

    if (*src == '+' || *src == '-')
        dst->value_is_relative = (*src++ == '+') ? 1 : -1;

    while (*src)
        if (*src == ':') {
            if (++colons == 3)
                return;

            src++;
            dst->value_is_time = 1;
            dst->value *= 60.0;
            continue;
        }
        else if (*src == '.' || isdigit (*src)) {
            temp = strtod (src, &src);

            if (temp < 0.0 || (dst->value_is_time && temp >= 60.0) ||
                (!dst->value_is_time && temp != floor (temp)))
                    return;

            dst->value += temp;
        }
        else
            return;

    dst->value_is_valid = 1;
}