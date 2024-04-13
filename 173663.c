static void note_transpose(struct SYMBOL *s)
{
	int i, j, m, n, d, a, dp, i1, i2, i3, i4, sf_old;
	static const signed char acc1[6] = {0, 1, 0, -1, 2, -2};
	static const char acc2[5] = {A_DF, A_FT, A_NT, A_SH, A_DS};

	m = s->nhd;
	sf_old = curvoice->okey.sf;
	i2 = curvoice->ckey.sf - sf_old;
	dp = cgd2cde[(i2 + 4 * 7) % 7];
	if (curvoice->transpose < 0
	 && dp != 0)
		dp -= 7;
	dp += curvoice->transpose / 3 / 12 * 7;
	for (i = 0; i <= m; i++) {

		/* pitch */
		n = s->u.note.notes[i].pit;
		s->u.note.notes[i].pit += dp;
		s->pits[i] += dp;

		/* accidental */
		i1 = cde2fcg[(n + 5 + 16 * 7) % 7];	/* fcgdaeb */
		a = s->u.note.notes[i].acc & 0x07;
		if (a == 0) {
			if (curvoice->okey.nacc == 0) {
				if (sf_old > 0) {
					if (i1 < sf_old - 1)
						a = A_SH;
				} else if (sf_old < 0) {
					if (i1 >= sf_old + 6)
						a = A_FT;
				}
			} else {
				for (j = 0; j < curvoice->okey.nacc; j++) {
					if ((n + 16 * 7 - curvoice->okey.pits[j]) % 7
								== 0) {
						a = curvoice->okey.accs[j];
						break;
					}
				}
			}
		}
		i3 = i1 + i2 + acc1[a] * 7;

		i1 = ((i3 + 1 + 21) / 7 + 2 - 3 + 32 * 5) % 5;
		a = acc2[(unsigned) i1];
		if (s->u.note.notes[i].acc != 0) {
			;
		} else if (curvoice->ckey.empty) {	/* key none */
			if (a == A_NT
			 || acc_same_pitch(s->u.note.notes[i].pit) >= 0)
				continue;
		} else if (curvoice->ckey.nacc > 0) {	/* acc list */
			i4 = cgd2cde[(unsigned) ((i3 + 16 * 7) % 7)];
			for (j = 0; j < curvoice->ckey.nacc; j++) {
				if ((i4 + 16 * 7 - curvoice->ckey.pits[j]) % 7
							== 0)
					break;
			}
			if (j < curvoice->ckey.nacc)
				continue;
		} else {
			continue;
		}
		i1 = s->u.note.notes[i].acc & 0x07;
		i4 = s->u.note.notes[i].acc >> 3;
		if (i4 != 0				/* microtone */
		 && i1 != a) {				/* different accidental type */
			if (s->u.note.microscale) {
				n = i4;
				d = s->u.note.microscale;
			} else {
				n = parse.micro_tb[i4];
				d = ((n & 0xff) + 1) * 2;
				n = (n >> 8) + 1;
			}
//fixme: double sharps/flats ?*/
//fixme: does not work in all cases (tied notes, previous accidental)
			switch (a) {
			case A_NT:
				if (n >= d / 2) {
					n -= d / 2;
					a = i1;
				} else {
					a = i1 == A_SH ? A_FT : A_SH;
				}
				break;
			case A_DS:
				if (n >= d / 2) {
					s->u.note.notes[i].pit += 1;
					s->pits[i] += 1;
					n -= d / 2;
				} else {
					n += d / 2;
				}
				a = i1;
				break;
			case A_DF:
				if (n >= d / 2) {
					s->u.note.notes[i].pit -= 1;
					s->pits[i] -= 1;
					n -= d / 2;
				} else {
					n += d / 2;
				}
				a = i1;
				break;
			}
			if (s->u.note.microscale) {
				i4 = n;
			} else {
				d = d / 2 - 1 + ((n - 1) << 8);
				for (i4 = 1; i4 < MAXMICRO; i4++) {
					if (parse.micro_tb[i4] == d)
						break;
					if (parse.micro_tb[i4] == 0) {
						parse.micro_tb[i4] = d;
						break;
					}
				}
				if (i4 == MAXMICRO) {
					error(1, s, "Too many microtone accidentals");
					i4 = 0;
				}
			}
		}
		s->u.note.notes[i].acc = (i4 << 3) | a;
	}
}