string_split (const char *string, const char *separators, int keep_eol,
              int num_items_max, int *num_items)
{
    int i, j, n_items;
    char *string2, **array;
    char *ptr, *ptr1, *ptr2;

    if (num_items != NULL)
        *num_items = 0;

    if (!string || !string[0] || !separators || !separators[0])
        return NULL;

    string2 = string_strip (string, 1, (keep_eol == 2) ? 0 : 1, separators);
    if (!string2 || !string2[0])
        return NULL;

    /* calculate number of items */
    ptr = string2;
    i = 1;
    while ((ptr = strpbrk (ptr, separators)))
    {
        while (ptr[0] && (strchr (separators, ptr[0]) != NULL))
        {
            ptr++;
        }
        i++;
    }
    n_items = i;

    if ((num_items_max != 0) && (n_items > num_items_max))
        n_items = num_items_max;

    array = malloc ((n_items + 1) * sizeof (array[0]));
    if (!array)
        return NULL;

    ptr1 = string2;

    for (i = 0; i < n_items; i++)
    {
        while (ptr1[0] && (strchr (separators, ptr1[0]) != NULL))
        {
            ptr1++;
        }
        if (i == (n_items - 1))
        {
            ptr2 = strpbrk (ptr1, separators);
            if (!ptr2)
                ptr2 = strchr (ptr1, '\0');
        }
        else
        {
            if ((ptr2 = strpbrk (ptr1, separators)) == NULL)
            {
                if ((ptr2 = strchr (ptr1, '\r')) == NULL)
                {
                    if ((ptr2 = strchr (ptr1, '\n')) == NULL)
                    {
                        ptr2 = strchr (ptr1, '\0');
                    }
                }
            }
        }

        if ((ptr1 == NULL) || (ptr2 == NULL))
        {
            array[i] = NULL;
        }
        else
        {
            if (ptr2 - ptr1 > 0)
            {
                if (keep_eol)
                {
                    array[i] = strdup (ptr1);
                    if (!array[i])
                    {
                        for (j = 0; j < n_items; j++)
                        {
                            if (array[j])
                                free (array[j]);
                        }
                        free (array);
                        free (string2);
                        return NULL;
                    }
                }
                else
                {
                    array[i] = malloc (ptr2 - ptr1 + 1);
                    if (!array[i])
                    {
                        for (j = 0; j < n_items; j++)
                        {
                            if (array[j])
                                free (array[j]);
                        }
                        free (array);
                        free (string2);
                        return NULL;
                    }
                    strncpy (array[i], ptr1, ptr2 - ptr1);
                    array[i][ptr2 - ptr1] = '\0';
                }
                ptr1 = ++ptr2;
            }
            else
            {
                array[i] = NULL;
            }
        }
    }

    array[i] = NULL;
    if (num_items != NULL)
        *num_items = i;

    free (string2);

    return array;
}