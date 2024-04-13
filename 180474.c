gx_edgebuffer_print_tr(gx_edgebuffer * edgebuffer)
{
    int i;

    dlprintf1("Edgebuffer %x\n", edgebuffer);
    dlprintf4("xmin=%d xmax=%d base=%d height=%d\n",
              edgebuffer->xmin, edgebuffer->xmax, edgebuffer->base, edgebuffer->height);
    for (i=0; i < edgebuffer->height; i++)
    {
        int  offset = edgebuffer->index[i];
        int *row    = &edgebuffer->table[offset];
        int count   = *row++;
        dlprintf3("%d @ %d: %d =", i, offset, count);
        while (count-- > 0) {
            int e  = *row++;
            int id = *row++;
            dlprintf3(" %d%c%d", e, id&1 ? 'v' : '^', id>>1);
        }
        dlprintf("\n");
    }
}