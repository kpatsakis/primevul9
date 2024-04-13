void ide_atapi_cmd_error(IDEState *s, int sense_key, int asc)
{
#ifdef DEBUG_IDE_ATAPI
    printf("atapi_cmd_error: sense=0x%x asc=0x%x\n", sense_key, asc);
#endif
    s->error = sense_key << 4;
    s->status = READY_STAT | ERR_STAT;
    s->nsector = (s->nsector & ~7) | ATAPI_INT_REASON_IO | ATAPI_INT_REASON_CD;
    s->sense_key = sense_key;
    s->asc = asc;
    ide_set_irq(s->bus);
}