static ram_addr_t qxl_rom_size(void)
{
#define QXL_REQUIRED_SZ (sizeof(QXLRom) + sizeof(QXLModes) + sizeof(qxl_modes))
#define QXL_ROM_SZ 8192

    QEMU_BUILD_BUG_ON(QXL_REQUIRED_SZ > QXL_ROM_SZ);
    return QXL_ROM_SZ;
}