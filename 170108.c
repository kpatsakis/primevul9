inline bool IsSpecialFileName(const WCHAR* pName)
{
    /* specical file names are devices that the system can open
     * these include AUX, CON, NUL, PRN, COMx, LPTx, CLOCK$, CONIN$, CONOUT$
     * (x is a single digit, and names are case-insensitive)
     */
    WCHAR ch = (pName[0] & ~0x20);
    switch (ch)
    {
	case 'A': /* AUX */
	    if (((pName[1] & ~0x20) == 'U')
		&& ((pName[2] & ~0x20) == 'X')
		&& !pName[3])
		    return true;
	    break;
	case 'C': /* CLOCK$, COMx,  CON, CONIN$ CONOUT$ */
	    ch = (pName[1] & ~0x20);
	    switch (ch)
	    {
		case 'L': /* CLOCK$ */
		    if (((pName[2] & ~0x20) == 'O')
			&& ((pName[3] & ~0x20) == 'C')
			&& ((pName[4] & ~0x20) == 'K')
			&& (pName[5] == '$')
			&& !pName[6])
			    return true;
		    break;
		case 'O': /* COMx,  CON, CONIN$ CONOUT$ */
		    if ((pName[2] & ~0x20) == 'M') {
			if ((pName[3] >= '1') && (pName[3] <= '9')
			    && !pName[4])
			    return true;
		    }
		    else if ((pName[2] & ~0x20) == 'N') {
			if (!pName[3])
			    return true;
			else if ((pName[3] & ~0x20) == 'I') {
			    if (((pName[4] & ~0x20) == 'N')
				&& (pName[5] == '$')
				&& !pName[6])
			    return true;
			}
			else if ((pName[3] & ~0x20) == 'O') {
			    if (((pName[4] & ~0x20) == 'U')
				&& ((pName[5] & ~0x20) == 'T')
				&& (pName[6] == '$')
				&& !pName[7])
			    return true;
			}
		    }
		    break;
	    }
	    break;
	case 'L': /* LPTx */
	    if (((pName[1] & ~0x20) == 'U')
		&& ((pName[2] & ~0x20) == 'X')
		&& (pName[3] >= '1') && (pName[3] <= '9')
		&& !pName[4])
		    return true;
	    break;
	case 'N': /* NUL */
	    if (((pName[1] & ~0x20) == 'U')
		&& ((pName[2] & ~0x20) == 'L')
		&& !pName[3])
		    return true;
	    break;
	case 'P': /* PRN */
	    if (((pName[1] & ~0x20) == 'R')
		&& ((pName[2] & ~0x20) == 'N')
		&& !pName[3])
		    return true;
	    break;
    }
    return false;
}