free_line_list(line_list *ll)
{
    /* Free any individually allocated active_lines. */
    gs_memory_t *mem = ll->memory;
    active_line *alp;

    while ((alp = ll->active_area) != 0) {
        active_line *next = alp->alloc_next;

        gs_free_object(mem, alp, "active line");
        ll->active_area = next;
    }
}