dotraplinkage void do_mce(struct pt_regs *regs, long error_code)
{
	machine_check_vector(regs, error_code);
}