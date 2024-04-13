static void gic_update(GICState *s)
{
    gic_update_internal(s, false);
}