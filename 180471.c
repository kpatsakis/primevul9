flush_cursor(cursor *cr, fixed x)
{
    /* This should only happen if we were entirely out of bounds */
    if (cr->first)
        return;

    /* Merge save into current if we can */
    if (fixed2int(cr->y) == fixed2int(cr->save_y) &&
        (cr->d == cr->save_d || cr->save_d == DIRN_UNSET)) {
        if (cr->left > cr->save_left)
            cr->left = cr->save_left;
        if (cr->right < cr->save_right)
            cr->right = cr->save_right;
        output_cursor(cr, x);
        return;
    }

    /* Merge not possible */
    output_cursor(cr, x);
    cr->left  = cr->save_left;
    cr->right = cr->save_right;
    cr->y     = cr->save_y;
    if (cr->save_d != -1)
        cr->d = cr->save_d;
    output_cursor(cr, x);
}