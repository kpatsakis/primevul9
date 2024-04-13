static void ide_atapi_cmd_check_status(IDEState *s)
{
#ifdef DEBUG_IDE_ATAPI
    printf("atapi_cmd_check_status\n");
#endif
    s->error = MC_ERR | (UNIT_ATTENTION << 4);
    s->status = ERR_STAT;
    s->nsector = 0;
    ide_set_irq(s->bus);
}