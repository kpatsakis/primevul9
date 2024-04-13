compile_free_jump_to_end(endlabel_T **el)
{
    while (*el != NULL)
    {
	endlabel_T  *cur = (*el);

	*el = cur->el_next;
	vim_free(cur);
    }
}