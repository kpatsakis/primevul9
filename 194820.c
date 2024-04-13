string_regex_flags (const char *regex, int default_flags, int *flags)
{
    const char *ptr_regex, *ptr_flags;
    int set_flag, flag;
    char *pos;

    ptr_regex = regex;
    if (flags)
        *flags = default_flags;

    while (strncmp (ptr_regex, "(?", 2) == 0)
    {
        pos = strchr (ptr_regex, ')');
        if (!pos)
            break;
        if (!isalpha ((unsigned char)ptr_regex[2]) && (ptr_regex[2] != '-'))
            break;
        if (flags)
        {
            set_flag = 1;
            for (ptr_flags = ptr_regex + 2; ptr_flags < pos; ptr_flags++)
            {
                flag = 0;
                switch (ptr_flags[0])
                {
                    case '-':
                        set_flag = 0;
                        break;
                    case 'e':
                        flag = REG_EXTENDED;
                        break;
                    case 'i':
                        flag = REG_ICASE;
                        break;
                    case 'n':
                        flag = REG_NEWLINE;
                        break;
                    case 's':
                        flag = REG_NOSUB;
                        break;
                }
                if (flag > 0)
                {
                    if (set_flag)
                        *flags |= flag;
                    else
                        *flags &= ~flag;
                }
            }
        }
        ptr_regex = pos + 1;
    }

    return ptr_regex;
}