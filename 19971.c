static void test_mem_protect(void)
{
    uc_engine *qc;
    int r_eax = 0x2000;
    int r_esi = 0xdeadbeef;
    uint32_t mem;
    // add [eax + 4], esi
    char code[] = {0x01, 0x70, 0x04};

    OK(uc_open(UC_ARCH_X86, UC_MODE_32, &qc));
    OK(uc_reg_write(qc, UC_X86_REG_EAX, &r_eax));
    OK(uc_reg_write(qc, UC_X86_REG_ESI, &r_esi));
    OK(uc_mem_map(qc, 0x1000, 0x1000, UC_PROT_READ | UC_PROT_EXEC));
    OK(uc_mem_map(qc, 0x2000, 0x1000, UC_PROT_READ));
    OK(uc_mem_protect(qc, 0x2000, 0x1000, UC_PROT_READ | UC_PROT_WRITE));
    OK(uc_mem_write(qc, 0x1000, code, sizeof(code)));

    OK(uc_emu_start(qc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 1));
    OK(uc_mem_read(qc, 0x2000 + 4, &mem, 4));

    TEST_CHECK(mem == 0xdeadbeef);

    OK(uc_close(qc));
}