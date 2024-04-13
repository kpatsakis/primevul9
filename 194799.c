string_input_for_buffer (const char *string)
{
    char *pos_slash, *pos_space, *next_char;

    /* special case for C comments pasted in input line */
    if (strncmp (string, "/*", 2) == 0)
        return string;

    /*
     * special case if string starts with '/': to allow to paste a path line
     * "/path/to/file.txt", we check if next '/' is after a space or not
     */
    if (string[0] == '/')
    {
        pos_slash = strchr (string + 1, '/');
        pos_space = strchr (string + 1, ' ');

        /*
         * if there's no other '/' of if '/' is after first space,
         * then it is a command, and return NULL
         */
        if (!pos_slash || (pos_space && pos_slash > pos_space))
            return NULL;

        return (string[1] == '/') ? string + 1 : string;
    }

    /* if string does not start with a command char, then it's not command */
    if (!string_is_command_char (string))
        return string;

    /* check if first char is doubled: if yes, then it's not a command */
    next_char = utf8_next_char (string);
    if (!next_char || !next_char[0])
        return string;
    if (utf8_charcmp (string, next_char) == 0)
        return next_char;

    /* string is a command */
    return NULL;
}