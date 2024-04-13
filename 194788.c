string_replace_with_callback (const char *string,
                              char *(*callback)(void *data, const char *text),
                              void *callback_data,
                              int *errors)
{
    int length, length_value, index_string, index_result;
    char *result, *result2, *key, *value;
    const char *pos_end_name;

    *errors = 0;

    if (!string)
        return NULL;

    length = strlen (string) + 1;
    result = malloc (length);
    if (result)
    {
        index_string = 0;
        index_result = 0;
        while (string[index_string])
        {
            if ((string[index_string] == '\\')
                && (string[index_string + 1] == '$'))
            {
                index_string++;
                result[index_result++] = string[index_string++];
            }
            else if ((string[index_string] == '$')
                     && (string[index_string + 1] == '{'))
            {
                pos_end_name = strchr (string + index_string + 2, '}');
                if (pos_end_name)
                {
                    key = string_strndup (string + index_string + 2,
                                          pos_end_name - (string + index_string + 2));
                    if (key)
                    {
                        value = (*callback) (callback_data, key);
                        if (value)
                        {
                            length_value = strlen (value);
                            length += length_value;
                            result2 = realloc (result, length);
                            if (!result2)
                            {
                                if (result)
                                    free (result);
                                free (key);
                                free (value);
                                return NULL;
                            }
                            result = result2;
                            strcpy (result + index_result, value);
                            index_result += length_value;
                            index_string += pos_end_name - string -
                                index_string + 1;
                            free (value);
                        }
                        else
                        {
                            result[index_result++] = string[index_string++];
                            (*errors)++;
                        }

                        free (key);
                    }
                    else
                        result[index_result++] = string[index_string++];
                }
                else
                    result[index_result++] = string[index_string++];
            }
            else
                result[index_result++] = string[index_string++];
        }
        result[index_result] = '\0';
    }

    return result;
}