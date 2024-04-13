static int qxl_pre_save(void *opaque)
{
    PCIQXLDevice* d = opaque;
    uint8_t *ram_start = d->vga.vram_ptr;

    trace_qxl_pre_save(d->id);
    if (d->last_release == NULL) {
        d->last_release_offset = 0;
    } else {
        d->last_release_offset = (uint8_t *)d->last_release - ram_start;
    }
    assert(d->last_release_offset < d->vga.vram_size);

    return 0;
}