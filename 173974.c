static unsigned int tabrand(unsigned int *tabrand_seed)
{
    return tabrand_chaos[++*tabrand_seed & TABRAND_SEEDMASK];
}