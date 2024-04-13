static void memory_region_update_container_subregions(MemoryRegion *subregion)
{
    MemoryRegion *mr = subregion->container;
    MemoryRegion *other;

    memory_region_transaction_begin();

    QTAILQ_FOREACH(other, &mr->subregions, subregions_link) {
        QTAILQ_INSERT_BEFORE(other, subregion, subregions_link);
        goto done;
    }
    QTAILQ_INSERT_TAIL(&mr->subregions, subregion, subregions_link);

done:
    mr->uc->memory_region_update_pending = true;
    memory_region_transaction_commit(mr);
}