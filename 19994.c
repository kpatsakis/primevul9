static void flatviews_reset(struct uc_struct *uc)
{
    AddressSpace *as;

    if (uc->flat_views) {
        g_hash_table_destroy(uc->flat_views);
        uc->flat_views = NULL;
    }
    flatviews_init(uc);

    /* Render unique FVs */
    QTAILQ_FOREACH(as, &uc->address_spaces, address_spaces_link) {
        MemoryRegion *physmr = memory_region_get_flatview_root(as->root);

        if (g_hash_table_lookup(uc->flat_views, physmr)) {
            continue;
        }

        generate_memory_topology(uc, physmr);
    }
}