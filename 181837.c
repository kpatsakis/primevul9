check_number(const char *str_num, const char **num_end, LY_DATA_TYPE base)
{
    if (!isdigit(str_num[0]) && (str_num[0] != '-') && (str_num[0] != '+')) {
        *num_end = str_num;
        return;
    }

    if ((str_num[0] == '-') || (str_num[0] == '+')) {
        ++str_num;
    }

    while (isdigit(str_num[0])) {
        ++str_num;
    }

    if ((base != LY_TYPE_DEC64) || (str_num[0] != '.') || !isdigit(str_num[1])) {
        *num_end = str_num;
        return;
    }

    ++str_num;
    while (isdigit(str_num[0])) {
        ++str_num;
    }

    *num_end = str_num;
}