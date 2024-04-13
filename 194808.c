string_format_size (unsigned long long size)
{
    char *unit_name[] = { N_("bytes"), N_("KB"), N_("MB"), N_("GB") };
    char *unit_format[] = { "%.0f", "%.1f", "%.02f", "%.02f" };
    float unit_divide[] = { 1, 1024, 1024*1024, 1024*1024*1024 };
    char format_size[128], str_size[128];
    int num_unit;

    str_size[0] = '\0';

    if (size < 1024*10)
        num_unit = 0;
    else if (size < 1024*1024)
        num_unit = 1;
    else if (size < 1024*1024*1024)
        num_unit = 2;
    else
        num_unit = 3;

    snprintf (format_size, sizeof (format_size),
              "%s %%s",
              unit_format[num_unit]);
    snprintf (str_size, sizeof (str_size),
              format_size,
              ((float)size) / ((float)(unit_divide[num_unit])),
              (size <= 1) ? _("byte") : _(unit_name[num_unit]));

    return strdup (str_size);
}