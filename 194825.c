string_convert_hex_chars (const char *string)
{
    char *output, hex_str[8], *error;
    int pos_output;
    long number;

    output = malloc (strlen (string) + 1);
    if (output)
    {
        pos_output = 0;
        while (string && string[0])
        {
            if (string[0] == '\\')
            {
                string++;
                switch (string[0])
                {
                    case '\\':
                        output[pos_output++] = '\\';
                        string++;
                        break;
                    case 'x':
                    case 'X':
                        if (isxdigit ((unsigned char)string[1])
                            && isxdigit ((unsigned char)string[2]))
                        {
                            snprintf (hex_str, sizeof (hex_str),
                                      "0x%c%c", string[1], string[2]);
                            number = strtol (hex_str, &error, 16);
                            if (error && !error[0])
                            {
                                output[pos_output++] = number;
                                string += 3;
                            }
                            else
                            {
                                output[pos_output++] = '\\';
                                output[pos_output++] = string[0];
                                string++;
                            }
                        }
                        else
                        {
                            output[pos_output++] = string[0];
                            string++;
                        }
                        break;
                    default:
                        output[pos_output++] = '\\';
                        output[pos_output++] = string[0];
                        string++;
                        break;
                }
            }
            else
            {
                output[pos_output++] = string[0];
                string++;
            }
        }
        output[pos_output] = '\0';
    }

    return output;
}