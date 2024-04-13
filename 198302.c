static int tftp_read_data(struct tftp_session *spt, uint32_t block_nr,
                          uint8_t *buf, int len)
{
    int bytes_read = 0;

    if (spt->fd < 0) {
        spt->fd = open(spt->filename, O_RDONLY | O_BINARY);
    }

    if (spt->fd < 0) {
        return -1;
    }

    if (len) {
        lseek(spt->fd, block_nr * spt->block_size, SEEK_SET);

        bytes_read = read(spt->fd, buf, len);
    }

    return bytes_read;
}