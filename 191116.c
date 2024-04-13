print_resource_usage(const gs_main_instance * minst, gs_dual_memory_t * dmem,
                     const char *msg)
{
    ulong used = 0;		/* this we accumulate for the PS memories */
    long utime[2];
    int i;
    gs_memory_status_t status;

    gp_get_realtime(utime);

    for (i = 0; i < countof(dmem->spaces_indexed); ++i) {
        gs_ref_memory_t *mem = dmem->spaces_indexed[i];

        if (mem != 0 && (i == 0 || mem != dmem->spaces_indexed[i - 1])) {
            gs_ref_memory_t *mem_stable =
                (gs_ref_memory_t *)gs_memory_stable((gs_memory_t *)mem);

            gs_memory_status((gs_memory_t *)mem, &status);
            used += status.used;
            if (mem_stable != mem) {
                gs_memory_status((gs_memory_t *)mem_stable, &status);
                used += status.used;
            }
        }
    }
    /* Now get the overall values from the heap memory */
    gs_memory_status(minst->heap, &status);
    dmprintf5(minst->heap, "%% %s time = %g, memory allocated = %lu, used = %lu, max_used = %lu\n",
              msg, utime[0] - minst->base_time[0] +
              (utime[1] - minst->base_time[1]) / 1000000000.0,
              status.allocated, used, status.max_used);
}