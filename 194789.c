hook_print_exec (struct t_gui_buffer *buffer, struct t_gui_line *line)
{
    struct t_hook *ptr_hook, *next_hook;
    char *prefix_no_color, *message_no_color;
    int tags_match, tag_found, i, j;

    if (!line->data->message || !line->data->message[0])
        return;

    prefix_no_color = (line->data->prefix) ?
        gui_color_decode (line->data->prefix, NULL) : NULL;

    message_no_color = gui_color_decode (line->data->message, NULL);
    if (!message_no_color)
    {
        if (prefix_no_color)
            free (prefix_no_color);
        return;
    }

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_PRINT];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (!HOOK_PRINT(ptr_hook, buffer)
                || (buffer == HOOK_PRINT(ptr_hook, buffer)))
            && (!HOOK_PRINT(ptr_hook, message)
                || !HOOK_PRINT(ptr_hook, message)[0]
                || string_strcasestr (prefix_no_color, HOOK_PRINT(ptr_hook, message))
                || string_strcasestr (message_no_color, HOOK_PRINT(ptr_hook, message))))
        {
            /* check if tags match */
            if (HOOK_PRINT(ptr_hook, tags_array))
            {
                /* if there are tags in message printed */
                if (line->data->tags_array)
                {
                    tags_match = 1;
                    for (i = 0; i < HOOK_PRINT(ptr_hook, tags_count); i++)
                    {
                        /* search for tag in message */
                        tag_found = 0;
                        for (j = 0; j < line->data->tags_count; j++)
                        {
                            if (string_strcasecmp (HOOK_PRINT(ptr_hook, tags_array)[i],
                                                   line->data->tags_array[j]) == 0)
                            {
                                tag_found = 1;
                                break;
                            }
                        }
                        /* tag was asked by hook but not found in message? */
                        if (!tag_found)
                        {
                            tags_match = 0;
                            break;
                        }
                    }
                }
                else
                    tags_match = 0;
            }
            else
                tags_match = 1;

            /* run callback */
            if (tags_match)
            {
                ptr_hook->running = 1;
                (void) (HOOK_PRINT(ptr_hook, callback))
                    (ptr_hook->callback_data, buffer, line->data->date,
                     line->data->tags_count,
                     (const char **)line->data->tags_array,
                     (int)line->data->displayed, (int)line->data->highlight,
                     (HOOK_PRINT(ptr_hook, strip_colors)) ? prefix_no_color : line->data->prefix,
                     (HOOK_PRINT(ptr_hook, strip_colors)) ? message_no_color : line->data->message);
                ptr_hook->running = 0;
            }
        }

        ptr_hook = next_hook;
    }

    if (prefix_no_color)
        free (prefix_no_color);
    if (message_no_color)
        free (message_no_color);

    hook_exec_end ();
}