static uint64_t sdhci_read(void *opaque, hwaddr offset, unsigned size)
{
    SDHCIState *s = (SDHCIState *)opaque;
    uint32_t ret = 0;

    switch (offset & ~0x3) {
    case SDHC_SYSAD:
        ret = s->sdmasysad;
        break;
    case SDHC_BLKSIZE:
        ret = s->blksize | (s->blkcnt << 16);
        break;
    case SDHC_ARGUMENT:
        ret = s->argument;
        break;
    case SDHC_TRNMOD:
        ret = s->trnmod | (s->cmdreg << 16);
        break;
    case SDHC_RSPREG0 ... SDHC_RSPREG3:
        ret = s->rspreg[((offset & ~0x3) - SDHC_RSPREG0) >> 2];
        break;
    case  SDHC_BDATA:
        if (sdhci_buff_access_is_sequential(s, offset - SDHC_BDATA)) {
            ret = sdhci_read_dataport(s, size);
            trace_sdhci_access("rd", size << 3, offset, "->", ret, ret);
            return ret;
        }
        break;
    case SDHC_PRNSTS:
        ret = s->prnsts;
        ret = FIELD_DP32(ret, SDHC_PRNSTS, DAT_LVL,
                         sdbus_get_dat_lines(&s->sdbus));
        ret = FIELD_DP32(ret, SDHC_PRNSTS, CMD_LVL,
                         sdbus_get_cmd_line(&s->sdbus));
        break;
    case SDHC_HOSTCTL:
        ret = s->hostctl1 | (s->pwrcon << 8) | (s->blkgap << 16) |
              (s->wakcon << 24);
        break;
    case SDHC_CLKCON:
        ret = s->clkcon | (s->timeoutcon << 16);
        break;
    case SDHC_NORINTSTS:
        ret = s->norintsts | (s->errintsts << 16);
        break;
    case SDHC_NORINTSTSEN:
        ret = s->norintstsen | (s->errintstsen << 16);
        break;
    case SDHC_NORINTSIGEN:
        ret = s->norintsigen | (s->errintsigen << 16);
        break;
    case SDHC_ACMD12ERRSTS:
        ret = s->acmd12errsts | (s->hostctl2 << 16);
        break;
    case SDHC_CAPAB:
        ret = (uint32_t)s->capareg;
        break;
    case SDHC_CAPAB + 4:
        ret = (uint32_t)(s->capareg >> 32);
        break;
    case SDHC_MAXCURR:
        ret = (uint32_t)s->maxcurr;
        break;
    case SDHC_MAXCURR + 4:
        ret = (uint32_t)(s->maxcurr >> 32);
        break;
    case SDHC_ADMAERR:
        ret =  s->admaerr;
        break;
    case SDHC_ADMASYSADDR:
        ret = (uint32_t)s->admasysaddr;
        break;
    case SDHC_ADMASYSADDR + 4:
        ret = (uint32_t)(s->admasysaddr >> 32);
        break;
    case SDHC_SLOT_INT_STATUS:
        ret = (s->version << 16) | sdhci_slotint(s);
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "SDHC rd_%ub @0x%02" HWADDR_PRIx " "
                      "not implemented\n", size, offset);
        break;
    }

    ret >>= (offset & 0x3) * 8;
    ret &= (1ULL << (size * 8)) - 1;
    trace_sdhci_access("rd", size << 3, offset, "->", ret, ret);
    return ret;
}