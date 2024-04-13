static void init_qxl_rom(PCIQXLDevice *d)
{
    QXLRom *rom = memory_region_get_ram_ptr(&d->rom_bar);
    QXLModes *modes = (QXLModes *)(rom + 1);
    uint32_t ram_header_size;
    uint32_t surface0_area_size;
    uint32_t num_pages;
    uint32_t fb;
    int i, n;

    memset(rom, 0, d->rom_size);

    rom->magic         = cpu_to_le32(QXL_ROM_MAGIC);
    rom->id            = cpu_to_le32(d->id);
    rom->log_level     = cpu_to_le32(d->guestdebug);
    rom->modes_offset  = cpu_to_le32(sizeof(QXLRom));

    rom->slot_gen_bits = MEMSLOT_GENERATION_BITS;
    rom->slot_id_bits  = MEMSLOT_SLOT_BITS;
    rom->slots_start   = 1;
    rom->slots_end     = NUM_MEMSLOTS - 1;
    rom->n_surfaces    = cpu_to_le32(d->ssd.num_surfaces);

    for (i = 0, n = 0; i < ARRAY_SIZE(qxl_modes); i++) {
        fb = qxl_modes[i].y_res * qxl_modes[i].stride;
        if (fb > d->vgamem_size) {
            continue;
        }
        modes->modes[n].id          = cpu_to_le32(i);
        modes->modes[n].x_res       = cpu_to_le32(qxl_modes[i].x_res);
        modes->modes[n].y_res       = cpu_to_le32(qxl_modes[i].y_res);
        modes->modes[n].bits        = cpu_to_le32(qxl_modes[i].bits);
        modes->modes[n].stride      = cpu_to_le32(qxl_modes[i].stride);
        modes->modes[n].x_mili      = cpu_to_le32(qxl_modes[i].x_mili);
        modes->modes[n].y_mili      = cpu_to_le32(qxl_modes[i].y_mili);
        modes->modes[n].orientation = cpu_to_le32(qxl_modes[i].orientation);
        n++;
    }
    modes->n_modes     = cpu_to_le32(n);

    ram_header_size    = ALIGN(sizeof(QXLRam), 4096);
    surface0_area_size = ALIGN(d->vgamem_size, 4096);
    num_pages          = d->vga.vram_size;
    num_pages         -= ram_header_size;
    num_pages         -= surface0_area_size;
    num_pages          = num_pages / QXL_PAGE_SIZE;

    assert(ram_header_size + surface0_area_size <= d->vga.vram_size);

    rom->draw_area_offset   = cpu_to_le32(0);
    rom->surface0_area_size = cpu_to_le32(surface0_area_size);
    rom->pages_offset       = cpu_to_le32(surface0_area_size);
    rom->num_pages          = cpu_to_le32(num_pages);
    rom->ram_header_offset  = cpu_to_le32(d->vga.vram_size - ram_header_size);

    if (d->xres && d->yres) {
        /* needs linux kernel 4.12+ to work */
        rom->client_monitors_config.count = 1;
        rom->client_monitors_config.heads[0].left = 0;
        rom->client_monitors_config.heads[0].top = 0;
        rom->client_monitors_config.heads[0].right = cpu_to_le32(d->xres);
        rom->client_monitors_config.heads[0].bottom = cpu_to_le32(d->yres);
        rom->client_monitors_config_crc = qxl_crc32(
            (const uint8_t *)&rom->client_monitors_config,
            sizeof(rom->client_monitors_config));
    }

    d->shadow_rom = *rom;
    d->rom        = rom;
    d->modes      = modes;
}