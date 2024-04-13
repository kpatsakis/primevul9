string_free_split_command (char **split_command)
{
    int i;

    if (split_command)
    {
        for (i = 0; split_command[i]; i++)
            free (split_command[i]);
        free (split_command);
    }
}