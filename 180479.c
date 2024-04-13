gx_edgebuffer_init(gx_edgebuffer * edgebuffer)
{
    edgebuffer->base   = 0;
    edgebuffer->height = 0;
    edgebuffer->index  = NULL;
    edgebuffer->table  = NULL;
}