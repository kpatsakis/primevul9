free_resub_eval_result(void)
{
    int i;

    for (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}