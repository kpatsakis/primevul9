insert_y_line(line_list *ll, active_line *alp)
{
    active_line *yp = ll->y_line;
    active_line *nyp;
    fixed y_start = alp->start.y;

    if (yp == 0) {
        alp->next = alp->prev = 0;
        ll->y_list = alp;
    } else if (y_start >= yp->start.y) {        /* Insert the new line after y_line */
        while (INCR_EXPR(y_up),
               ((nyp = yp->next) != NULL &&
                y_start > nyp->start.y)
            )
            yp = nyp;
        alp->next = nyp;
        alp->prev = yp;
        yp->next = alp;
        if (nyp)
            nyp->prev = alp;
    } else {            /* Insert the new line before y_line */
        while (INCR_EXPR(y_down),
               ((nyp = yp->prev) != NULL &&
                y_start < nyp->start.y)
            )
            yp = nyp;
        alp->prev = nyp;
        alp->next = yp;
        yp->prev = alp;
        if (nyp)
            nyp->next = alp;
        else
            ll->y_list = alp;
    }
    ll->y_line = alp;
    print_al(ll->memory, "add ", alp);
}