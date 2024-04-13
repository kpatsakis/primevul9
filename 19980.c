static void address_space_set_flatview(AddressSpace *as)
{
    FlatView *old_view = address_space_to_flatview(as);
    MemoryRegion *physmr = memory_region_get_flatview_root(as->root);
    FlatView *new_view = g_hash_table_lookup(as->uc->flat_views, physmr);

    assert(new_view);

    if (old_view == new_view) {
        return;
    }

    flatview_ref(new_view);
    if (!QTAILQ_EMPTY(&as->listeners)) {
        FlatView tmpview = { .nr = 0 }, *old_view2 = old_view;

        if (!old_view2) {
            old_view2 = &tmpview;
        }
        address_space_update_topology_pass(as, old_view2, new_view, false);
        address_space_update_topology_pass(as, old_view2, new_view, true);
    }

    as->current_map = new_view;
    if (old_view) {
        flatview_unref(old_view);
    }
}