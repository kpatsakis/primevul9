make_al(line_list *ll)
{
    active_line *alp = ll->next_active;

    if (alp == ll->limit) {     /* Allocate separately */
        alp = gs_alloc_struct(ll->memory, active_line,
                              &st_active_line, "active line");
        if (alp == 0)
            return NULL;
        alp->alloc_next = ll->active_area;
        ll->active_area = alp;
        INCR(fill_alloc);
    } else
        ll->next_active++;
    alp->contour_count = ll->contour_count;
    return alp;
}