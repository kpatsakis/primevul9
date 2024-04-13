static void build_guest_fsinfo_for_real_device(char const *syspath,
                                               GuestFilesystemInfo *fs,
                                               Error **errp)
{
    unsigned int pci[4], host, hosts[8], tgt[3];
    int i, nhosts = 0, pcilen;
    GuestDiskAddress *disk;
    GuestPCIAddress *pciaddr;
    GuestDiskAddressList *list = NULL;
    bool has_ata = false, has_host = false, has_tgt = false;
    char *p, *q, *driver = NULL;

    p = strstr(syspath, "/devices/pci");
    if (!p || sscanf(p + 12, "%*x:%*x/%x:%x:%x.%x%n",
                     pci, pci + 1, pci + 2, pci + 3, &pcilen) < 4) {
        g_debug("only pci device is supported: sysfs path \"%s\"", syspath);
        return;
    }

    driver = get_pci_driver(syspath, (p + 12 + pcilen) - syspath, errp);
    if (!driver) {
        goto cleanup;
    }

    p = strstr(syspath, "/target");
    if (p && sscanf(p + 7, "%*u:%*u:%*u/%*u:%u:%u:%u",
                    tgt, tgt + 1, tgt + 2) == 3) {
        has_tgt = true;
    }

    p = strstr(syspath, "/ata");
    if (p) {
        q = p + 4;
        has_ata = true;
    } else {
        p = strstr(syspath, "/host");
        q = p + 5;
    }
    if (p && sscanf(q, "%u", &host) == 1) {
        has_host = true;
        nhosts = build_hosts(syspath, p, has_ata, hosts,
                             ARRAY_SIZE(hosts), errp);
        if (nhosts < 0) {
            goto cleanup;
        }
    }

    pciaddr = g_malloc0(sizeof(*pciaddr));
    pciaddr->domain = pci[0];
    pciaddr->bus = pci[1];
    pciaddr->slot = pci[2];
    pciaddr->function = pci[3];

    disk = g_malloc0(sizeof(*disk));
    disk->pci_controller = pciaddr;

    list = g_malloc0(sizeof(*list));
    list->value = disk;

    if (strcmp(driver, "ata_piix") == 0) {
        /* a host per ide bus, target*:0:<unit>:0 */
        if (!has_host || !has_tgt) {
            g_debug("invalid sysfs path '%s' (driver '%s')", syspath, driver);
            goto cleanup;
        }
        for (i = 0; i < nhosts; i++) {
            if (host == hosts[i]) {
                disk->bus_type = GUEST_DISK_BUS_TYPE_IDE;
                disk->bus = i;
                disk->unit = tgt[1];
                break;
            }
        }
        if (i >= nhosts) {
            g_debug("no host for '%s' (driver '%s')", syspath, driver);
            goto cleanup;
        }
    } else if (strcmp(driver, "sym53c8xx") == 0) {
        /* scsi(LSI Logic): target*:0:<unit>:0 */
        if (!has_tgt) {
            g_debug("invalid sysfs path '%s' (driver '%s')", syspath, driver);
            goto cleanup;
        }
        disk->bus_type = GUEST_DISK_BUS_TYPE_SCSI;
        disk->unit = tgt[1];
    } else if (strcmp(driver, "virtio-pci") == 0) {
        if (has_tgt) {
            /* virtio-scsi: target*:0:0:<unit> */
            disk->bus_type = GUEST_DISK_BUS_TYPE_SCSI;
            disk->unit = tgt[2];
        } else {
            /* virtio-blk: 1 disk per 1 device */
            disk->bus_type = GUEST_DISK_BUS_TYPE_VIRTIO;
        }
    } else if (strcmp(driver, "ahci") == 0) {
        /* ahci: 1 host per 1 unit */
        if (!has_host || !has_tgt) {
            g_debug("invalid sysfs path '%s' (driver '%s')", syspath, driver);
            goto cleanup;
        }
        for (i = 0; i < nhosts; i++) {
            if (host == hosts[i]) {
                disk->unit = i;
                disk->bus_type = GUEST_DISK_BUS_TYPE_SATA;
                break;
            }
        }
        if (i >= nhosts) {
            g_debug("no host for '%s' (driver '%s')", syspath, driver);
            goto cleanup;
        }
    } else {
        g_debug("unknown driver '%s' (sysfs path '%s')", driver, syspath);
        goto cleanup;
    }

    list->next = fs->disk;
    fs->disk = list;
    g_free(driver);
    return;

cleanup:
    if (list) {
        qapi_free_GuestDiskAddressList(list);
    }
    g_free(driver);
}