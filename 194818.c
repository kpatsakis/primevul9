string_tolower (char *string)
{
    while (string && string[0])
    {
        if ((string[0] >= 'A') && (string[0] <= 'Z'))
            string[0] += ('a' - 'A');
        string = utf8_next_char (string);
    }
}