output_cursor(cursor *cr, fixed x)
{
    int iy = fixed2int(cr->y) - cr->base;
    int *row;
    int count;

    if (iy < 0 || iy >= cr->scanlines) {
        /* Out of range, nothing to do */
    } else if (cr->first) {
        /* Store this one for later, for when we match up */
        cr->save_left  = cr->left;
        cr->save_right = cr->right;
        cr->save_y     = cr->y;
        cr->save_d     = cr->d;
        cr->first      = 0;
    } else {
        /* Put it in the table */
        assert(cr->d != DIRN_UNSET);

        row = &cr->table[cr->index[iy]];
        *row = count = (*row)+1; /* Increment the count */
        row[2 * count-1] = (cr->left&~1) | cr->d;
        row[2 * count  ] = cr->right;
    }
    cr->left  = x;
    cr->right = x;
}