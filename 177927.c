static void ide_atapi_cmd_read(IDEState *s, int lba, int nb_sectors,
                               int sector_size)
{
#ifdef DEBUG_IDE_ATAPI
    printf("read %s: LBA=%d nb_sectors=%d\n", s->atapi_dma ? "dma" : "pio",
        lba, nb_sectors);
#endif
    if (s->atapi_dma) {
        ide_atapi_cmd_read_dma(s, lba, nb_sectors, sector_size);
    } else {
        ide_atapi_cmd_read_pio(s, lba, nb_sectors, sector_size);
    }
}