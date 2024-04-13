static void phys_page_set(AddressSpaceDispatch *d,
                          hwaddr index, uint64_t nb,
                          uint16_t leaf)
{
#ifdef TARGET_ARM
    struct uc_struct *uc = d->uc;
#endif
    /* Wildly overreserve - it doesn't matter much. */
    phys_map_node_reserve(d, &d->map, 3 * P_L2_LEVELS);

    phys_page_set_level(&d->map, &d->phys_map, &index, &nb, leaf, P_L2_LEVELS - 1);
}