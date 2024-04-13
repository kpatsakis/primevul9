string_is_command_char (const char *string)
{
    const char *ptr_command_chars;

    if (!string)
        return 0;

    if (string[0] == '/')
        return 1;

    ptr_command_chars = CONFIG_STRING(config_look_command_chars);
    if (!ptr_command_chars || !ptr_command_chars[0])
        return 0;

    while (ptr_command_chars && ptr_command_chars[0])
    {
        if (utf8_charcmp (ptr_command_chars, string) == 0)
            return 1;
        ptr_command_chars = utf8_next_char (ptr_command_chars);
    }

    return 0;
}