static uint64_t vmxnet3_device_serial_num(VMXNET3State *s)
{
    uint64_t dsn_payload;
    uint8_t *dsnp = (uint8_t *)&dsn_payload;

    dsnp[0] = 0xfe;
    dsnp[1] = s->conf.macaddr.a[3];
    dsnp[2] = s->conf.macaddr.a[4];
    dsnp[3] = s->conf.macaddr.a[5];
    dsnp[4] = s->conf.macaddr.a[0];
    dsnp[5] = s->conf.macaddr.a[1];
    dsnp[6] = s->conf.macaddr.a[2];
    dsnp[7] = 0xff;
    return dsn_payload;
}