static void test_map_at_the_end(void)
{
    uc_engine *uc;
    uint8_t mem[0x1000];

    memset(mem, 0xff, 0x100);

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    OK(uc_mem_map(uc, 0xfffffffffffff000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0xfffffffffffff000, mem, sizeof(mem)));

    uc_assert_err(UC_ERR_WRITE_UNMAPPED,
                  uc_mem_write(uc, 0xffffffffffffff00, mem, sizeof(mem)));
    uc_assert_err(UC_ERR_WRITE_UNMAPPED, uc_mem_write(uc, 0, mem, sizeof(mem)));

    OK(uc_close(uc));
}