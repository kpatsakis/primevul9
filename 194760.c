string_free_split (char **split_string)
{
    int i;

    if (split_string)
    {
        for (i = 0; split_string[i]; i++)
            free (split_string[i]);
        free (split_string);
    }
}