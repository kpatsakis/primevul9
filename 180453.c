gx_edgebuffer_print_tr_app(gx_edgebuffer * edgebuffer)
{
    int i;
    int borked = 0;

    dlprintf1("Edgebuffer %x\n", edgebuffer);
    dlprintf4("xmin=%d xmax=%d base=%d height=%d\n",
              edgebuffer->xmin, edgebuffer->xmax, edgebuffer->base, edgebuffer->height);
    for (i=0; i < edgebuffer->height; i++)
    {
        int  offset = edgebuffer->index[i];
        int *row    = &edgebuffer->table[offset];
        int count   = *row++;
        int c       = count;
        int wind    = 0;
        dlprintf3("%x @ %d: %d =", i, offset, count);
        while (count-- > 0) {
            int left  = *row++;
            int lid   = *row++;
            int right = *row++;
            int rid   = *row++;
            int ww    = lid & 1;
            int w     = -ww | 1;
            lid >>= 1;
            wind += w;
            dlprintf5(" (%x:%d,%x:%d)%c", left, lid, right, rid, ww ? 'v' : '^');
        }
        if (wind != 0 || c & 1) {
            dlprintf(" <- BROKEN");
            borked = 1;
        }
        dlprintf("\n");
    }
    if (borked) {
        borked = borked; /* Breakpoint here */
    }
}