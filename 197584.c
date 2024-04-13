static void gic_update_virt(GICState *s)
{
    gic_update_internal(s, true);
    gic_update_maintenance(s);
}