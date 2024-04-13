hook_command_build_completion (struct t_hook_command *hook_command)
{
    int i, j, k, length, num_items;
    struct t_weelist *list;
    char *pos_completion, *pos_double_pipe, *pos_start, *pos_end;
    char **items, *last_space, *ptr_template;

    /* split templates using "||" as separator */
    hook_command->cplt_num_templates = 1;
    pos_completion = hook_command->completion;
    while ((pos_double_pipe = strstr (pos_completion, "||")) != NULL)
    {
        hook_command->cplt_num_templates++;
        pos_completion = pos_double_pipe + 2;
    }
    hook_command->cplt_templates = malloc (hook_command->cplt_num_templates *
                                           sizeof (*hook_command->cplt_templates));
    for (i = 0; i < hook_command->cplt_num_templates; i++)
    {
        hook_command->cplt_templates[i] = NULL;
    }
    pos_completion = hook_command->completion;
    i = 0;
    while (pos_completion)
    {
        pos_double_pipe = strstr (pos_completion, "||");
        if (!pos_double_pipe)
            pos_double_pipe = pos_completion + strlen (pos_completion);
        pos_start = pos_completion;
        pos_end = pos_double_pipe - 1;
        if (pos_end < pos_start)
        {
            hook_command->cplt_templates[i] = strdup ("");
        }
        else
        {
            while (pos_start[0] == ' ')
            {
                pos_start++;
            }
            pos_end = pos_double_pipe - 1;
            while ((pos_end > pos_start) && (pos_end[0] == ' '))
            {
                pos_end--;
            }
            hook_command->cplt_templates[i] = string_strndup (pos_start,
                                                              pos_end - pos_start + 1);
        }
        i++;
        if (!pos_double_pipe[0])
            pos_completion = NULL;
        else
            pos_completion = pos_double_pipe + 2;
    }

    /* for each template, split/count args */
    hook_command->cplt_templates_static = malloc (hook_command->cplt_num_templates *
                                                  sizeof (*hook_command->cplt_templates_static));
    hook_command->cplt_template_num_args = malloc (hook_command->cplt_num_templates *
                                                   sizeof (*hook_command->cplt_template_num_args));
    hook_command->cplt_template_args = malloc (hook_command->cplt_num_templates *
                                               sizeof (*hook_command->cplt_template_args));
    hook_command->cplt_template_num_args_concat = 0;
    for (i = 0; i < hook_command->cplt_num_templates; i++)
    {
        /*
         * build static part of template: it's first argument(s) which does not
         * contain "%" or "|"
         */
        last_space = NULL;
        ptr_template = hook_command->cplt_templates[i];
        while (ptr_template && ptr_template[0])
        {
            if (ptr_template[0] == ' ')
            {
                last_space = ptr_template;
                break;
            }
            else if ((ptr_template[0] == '%') || (ptr_template[0] == '|'))
                break;
            ptr_template = utf8_next_char (ptr_template);
        }
        if (last_space)
        {
            last_space--;
            while (last_space > hook_command->cplt_templates[i])
            {
                if (last_space[0] != ' ')
                    break;
            }
            if (last_space < hook_command->cplt_templates[i])
                last_space = NULL;
            else
                last_space++;
        }
        if (last_space)
            hook_command->cplt_templates_static[i] = string_strndup (hook_command->cplt_templates[i],
                                                                            last_space - hook_command->cplt_templates[i]);
        else
            hook_command->cplt_templates_static[i] = strdup (hook_command->cplt_templates[i]);

        /* build arguments for each template */
        hook_command->cplt_template_args[i] = string_split (hook_command->cplt_templates[i],
                                                            " ", 0, 0,
                                                            &(hook_command->cplt_template_num_args[i]));
        if (hook_command->cplt_template_num_args[i] > hook_command->cplt_template_num_args_concat)
            hook_command->cplt_template_num_args_concat = hook_command->cplt_template_num_args[i];
    }

    /*
     * build strings with concatentaion of items from different templates
     * for each argument: these strings will be used when completing argument
     * if we can't find which template to use (for example for first argument)
     */
    if (hook_command->cplt_template_num_args_concat == 0)
        hook_command->cplt_template_args_concat = NULL;
    else
    {
        hook_command->cplt_template_args_concat = malloc (hook_command->cplt_template_num_args_concat *
                                                          sizeof (*hook_command->cplt_template_args_concat));
        list = weelist_new ();
        for (i = 0; i < hook_command->cplt_template_num_args_concat; i++)
        {
            /* first compute length */
            length = 1;
            for (j = 0; j < hook_command->cplt_num_templates; j++)
            {
                if (i < hook_command->cplt_template_num_args[j])
                    length += strlen (hook_command->cplt_template_args[j][i]) + 1;
            }
            /* alloc memory */
            hook_command->cplt_template_args_concat[i] = malloc (length);
            if (hook_command->cplt_template_args_concat[i])
            {
                /* concatene items with "|" as separator */
                weelist_remove_all (list);
                hook_command->cplt_template_args_concat[i][0] = '\0';
                for (j = 0; j < hook_command->cplt_num_templates; j++)
                {
                    if (i < hook_command->cplt_template_num_args[j])
                    {
                        items = string_split (hook_command->cplt_template_args[j][i],
                                              "|", 0, 0, &num_items);
                        for (k = 0; k < num_items; k++)
                        {
                            if (!weelist_search (list, items[k]))
                            {
                                if (hook_command->cplt_template_args_concat[i][0])
                                    strcat (hook_command->cplt_template_args_concat[i], "|");
                                strcat (hook_command->cplt_template_args_concat[i],
                                        items[k]);
                                weelist_add (list, items[k], WEECHAT_LIST_POS_END,
                                             NULL);
                            }
                        }
                        string_free_split (items);
                    }
                }
            }
        }
        weelist_free (list);
    }
}