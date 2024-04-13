string_split_shell (const char *string)
{
    int temp_len, num_args, add_char_to_temp, add_temp_to_args, quoted;
    char *string2, *temp, **args, **args2, state, escapedstate;
    char *ptr_string, *ptr_next, saved_char;

    if (!string)
        return NULL;

    string2 = strdup (string);
    if (!string2)
        return NULL;

    /*
     * prepare "args" with one pointer to NULL, the "args" will be reallocated
     * later, each time a new argument is added
     */
    num_args = 0;
    args = malloc ((num_args + 1) * sizeof (args[0]));
    if (!args)
    {
        free (string2);
        return NULL;
    }
    args[0] = NULL;

    /* prepare a temp string for working (adding chars one by one) */
    temp = malloc ((2 * strlen (string)) + 1);
    if (!temp)
    {
        free (string2);
        free (args);
        return NULL;
    }
    temp[0] = '\0';
    temp_len = 0;

    state = ' ';
    escapedstate = ' ';
    quoted = 0;
    ptr_string = string2;
    while (ptr_string[0])
    {
        add_char_to_temp = 0;
        add_temp_to_args = 0;
        ptr_next = utf8_next_char (ptr_string);
        saved_char = ptr_next[0];
        ptr_next[0] = '\0';
        if (state == ' ')
        {
            if ((ptr_string[0] == ' ') || (ptr_string[0] == '\t')
                || (ptr_string[0] == '\r') || (ptr_string[0] == '\n'))
            {
                if (temp[0] || quoted)
                    add_temp_to_args = 1;
            }
            else if (ptr_string[0] == '\\')
            {
                escapedstate = 'a';
                state = ptr_string[0];
            }
            else if ((ptr_string[0] == '\'') || (ptr_string[0] == '"'))
            {
                state = ptr_string[0];
            }
            else
            {
                add_char_to_temp = 1;
                state = 'a';
            }
        }
        else if ((state == '\'') || (state == '"'))
        {
            quoted = 1;
            if (ptr_string[0] == state)
            {
                state = 'a';
            }
            else if ((state == '"') && (ptr_string[0] == '\\'))
            {
                escapedstate = state;
                state = ptr_string[0];
            }
            else
            {
                add_char_to_temp = 1;
            }
        }
        else if (state == '\\')
        {
            if (((escapedstate == '\'') || (escapedstate == '"'))
                && (ptr_string[0] != state) && (ptr_string[0] != escapedstate))
            {
                temp[temp_len] = state;
                temp_len++;
                temp[temp_len] = '\0';
            }
            add_char_to_temp = 1;
            state = escapedstate;
        }
        else if (state == 'a')
        {
            if ((ptr_string[0] == ' ') || (ptr_string[0] == '\t')
                || (ptr_string[0] == '\r') || (ptr_string[0] == '\n'))
            {
                state = ' ';
                if (temp[0] || quoted)
                    add_temp_to_args = 1;
            }
            else if (ptr_string[0] == '\\')
            {
                escapedstate = 'a';
                state = ptr_string[0];
            }
            else if ((ptr_string[0] == '\'') || (ptr_string[0] == '"'))
            {
                state = ptr_string[0];
            }
            else
            {
                add_char_to_temp = 1;
            }
        }
        if (add_char_to_temp)
        {
            memcpy (temp + temp_len, ptr_string, ptr_next - ptr_string);
            temp_len += (ptr_next - ptr_string);
            temp[temp_len] = '\0';
        }
        if (add_temp_to_args)
        {
            num_args++;
            args2 = realloc (args, (num_args + 1) * sizeof (args[0]));
            if (!args2)
            {
                free (string2);
                free (temp);
                return args;
            }
            args = args2;
            args[num_args - 1] = strdup (temp);
            args[num_args] = NULL;
            temp[0] = '\0';
            temp_len = 0;
            escapedstate = ' ';
            quoted = 0;
        }
        ptr_next[0] = saved_char;
        ptr_string = ptr_next;
    }

    if (temp[0] || (state != ' '))
    {
        num_args++;
        args2 = realloc (args, (num_args + 1) * sizeof (args[0]));
        if (!args2)
        {
            free (string2);
            free (temp);
            return args;
        }
        args = args2;
        args[num_args - 1] = strdup (temp);
        args[num_args] = NULL;
        temp[0] = '\0';
        temp_len = 0;
    }

    free (string2);
    free (temp);

    return args;
}