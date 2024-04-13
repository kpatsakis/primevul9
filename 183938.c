static int test_cc(unsigned int condition, unsigned int flags)
{
	int rc = 0;

	switch ((condition & 15) >> 1) {
	case 0: /* o */
		rc |= (flags & EFLG_OF);
		break;
	case 1: /* b/c/nae */
		rc |= (flags & EFLG_CF);
		break;
	case 2: /* z/e */
		rc |= (flags & EFLG_ZF);
		break;
	case 3: /* be/na */
		rc |= (flags & (EFLG_CF|EFLG_ZF));
		break;
	case 4: /* s */
		rc |= (flags & EFLG_SF);
		break;
	case 5: /* p/pe */
		rc |= (flags & EFLG_PF);
		break;
	case 7: /* le/ng */
		rc |= (flags & EFLG_ZF);
		/* fall through */
	case 6: /* l/nge */
		rc |= (!(flags & EFLG_SF) != !(flags & EFLG_OF));
		break;
	}

	/* Odd condition identifiers (lsb == 1) have inverted sense. */
	return (!!rc ^ (condition & 1));
}