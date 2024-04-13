static int ide_dvd_read_structure(IDEState *s, int format,
                                  const uint8_t *packet, uint8_t *buf)
{
    switch (format) {
        case 0x0: /* Physical format information */
            {
                int layer = packet[6];
                uint64_t total_sectors;

                if (layer != 0)
                    return -ASC_INV_FIELD_IN_CMD_PACKET;

                total_sectors = s->nb_sectors >> 2;
                if (total_sectors == 0) {
                    return -ASC_MEDIUM_NOT_PRESENT;
                }

                buf[4] = 1;   /* DVD-ROM, part version 1 */
                buf[5] = 0xf; /* 120mm disc, minimum rate unspecified */
                buf[6] = 1;   /* one layer, read-only (per MMC-2 spec) */
                buf[7] = 0;   /* default densities */

                /* FIXME: 0x30000 per spec? */
                cpu_to_ube32(buf + 8, 0); /* start sector */
                cpu_to_ube32(buf + 12, total_sectors - 1); /* end sector */
                cpu_to_ube32(buf + 16, total_sectors - 1); /* l0 end sector */

                /* Size of buffer, not including 2 byte size field */
                cpu_to_be16wu((uint16_t *)buf, 2048 + 2);

                /* 2k data + 4 byte header */
                return (2048 + 4);
            }

        case 0x01: /* DVD copyright information */
            buf[4] = 0; /* no copyright data */
            buf[5] = 0; /* no region restrictions */

            /* Size of buffer, not including 2 byte size field */
            cpu_to_be16wu((uint16_t *)buf, 4 + 2);

            /* 4 byte header + 4 byte data */
            return (4 + 4);

        case 0x03: /* BCA information - invalid field for no BCA info */
            return -ASC_INV_FIELD_IN_CMD_PACKET;

        case 0x04: /* DVD disc manufacturing information */
            /* Size of buffer, not including 2 byte size field */
            cpu_to_be16wu((uint16_t *)buf, 2048 + 2);

            /* 2k data + 4 byte header */
            return (2048 + 4);

        case 0xff:
            /*
             * This lists all the command capabilities above.  Add new ones
             * in order and update the length and buffer return values.
             */

            buf[4] = 0x00; /* Physical format */
            buf[5] = 0x40; /* Not writable, is readable */
            cpu_to_be16wu((uint16_t *)(buf + 6), 2048 + 4);

            buf[8] = 0x01; /* Copyright info */
            buf[9] = 0x40; /* Not writable, is readable */
            cpu_to_be16wu((uint16_t *)(buf + 10), 4 + 4);

            buf[12] = 0x03; /* BCA info */
            buf[13] = 0x40; /* Not writable, is readable */
            cpu_to_be16wu((uint16_t *)(buf + 14), 188 + 4);

            buf[16] = 0x04; /* Manufacturing info */
            buf[17] = 0x40; /* Not writable, is readable */
            cpu_to_be16wu((uint16_t *)(buf + 18), 2048 + 4);

            /* Size of buffer, not including 2 byte size field */
            cpu_to_be16wu((uint16_t *)buf, 16 + 2);

            /* data written + 4 byte header */
            return (16 + 4);

        default: /* TODO: formats beyond DVD-ROM requires */
            return -ASC_INV_FIELD_IN_CMD_PACKET;
    }
}