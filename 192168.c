static void __mce_disable_bank(void *arg)
{
	int bank = *((int *)arg);
	__clear_bit(bank, this_cpu_ptr(mce_poll_banks));
	cmci_disable_bank(bank);
}