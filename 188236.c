static bool hard_acs_rdy(struct temac_local *lp)
{
	return temac_ior(lp, XTE_RDY0_OFFSET) & XTE_RDY0_HARD_ACS_RDY_MASK;
}