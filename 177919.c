static int cd_read_sector(IDEState *s, int lba, uint8_t *buf, int sector_size)
{
    int ret;

    switch(sector_size) {
    case 2048:
        bdrv_acct_start(s->bs, &s->acct, 4 * BDRV_SECTOR_SIZE, BDRV_ACCT_READ);
        ret = bdrv_read(s->bs, (int64_t)lba << 2, buf, 4);
        bdrv_acct_done(s->bs, &s->acct);
        break;
    case 2352:
        bdrv_acct_start(s->bs, &s->acct, 4 * BDRV_SECTOR_SIZE, BDRV_ACCT_READ);
        ret = bdrv_read(s->bs, (int64_t)lba << 2, buf + 16, 4);
        bdrv_acct_done(s->bs, &s->acct);
        if (ret < 0)
            return ret;
        cd_data_to_raw(buf, lba);
        break;
    default:
        ret = -EIO;
        break;
    }
    return ret;
}