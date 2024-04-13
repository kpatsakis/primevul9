string_toupper (char *string)
{
    while (string && string[0])
    {
        if ((string[0] >= 'a') && (string[0] <= 'z'))
            string[0] -= ('a' - 'A');
        string = utf8_next_char (string);
    }
}