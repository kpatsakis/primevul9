static int get_transpose(char *p)
{
	int val, pit1, pit2, acc;
	static int pit_st[7] = {0, 2, 4, 5, 7, 9, 11};

	if (isdigit(*p) || *p == '-' || *p == '+') {
		sscanf(p, "%d", &val);
		val *= 3;
		switch (p[strlen(p) - 1]) {
		default:
			return val;
		case '#':
			val++;
			break;
		case 'b':
			val += 2;
			break;
		}
		if (val > 0)
			return val;
		return val - 3;
	}

	// by music interval
	p = parse_acc_pit(p, &pit1, &acc);
	if (acc < 0) {
		error(1, NULL, "  in %%%%transpose");
		return 0;
	}
	pit1 += 126 - 2;    // for value > 0 and 'C' % 7 == 0
	pit1 = (pit1 / 7) * 12 + pit_st[pit1 % 7];
	switch (acc) {
	case A_DS:
		pit1 += 2;
		break;
	case A_SH:
		pit1++;
		break;
	case A_FT:
		pit1--;
		break;
	case A_DF:
		pit1 -= 2;
		break;
	}
	p = parse_acc_pit(p, &pit2, &acc);
	if (acc < 0) {
		error(1, NULL, "  in %%%%transpose");
		return 0;
	}
	pit2 += 126 - 2;
	pit2 = (pit2 / 7) * 12 + pit_st[pit2 % 7];
	switch (acc) {
	case A_DS:
		pit2 += 2;
		break;
	case A_SH:
		pit2++;
		break;
	case A_FT:
		pit2--;
		break;
	case A_DF:
		pit2 -= 2;
		break;
	}

	val = (pit2 - pit1) * 3;
	switch (acc) {
	default:
		return val;
	case A_DS:
	case A_SH:
		val++;
		break;
	case A_FT:
	case A_DF:
		val += 2;
		break;
	}
	if (val > 0)
		return val;
	return val - 3;
}