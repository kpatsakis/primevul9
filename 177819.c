extend_hfa_type (void *dest, void *src, int h)
{
  ssize_t f = h - AARCH64_RET_S4;
  void *x0;

  asm volatile (
	"adr	%0, 0f\n"
"	add	%0, %0, %1\n"
"	br	%0\n"
"0:	ldp	s16, s17, [%3]\n"	/* S4 */
"	ldp	s18, s19, [%3, #8]\n"
"	b	4f\n"
"	ldp	s16, s17, [%3]\n"	/* S3 */
"	ldr	s18, [%3, #8]\n"
"	b	3f\n"
"	ldp	s16, s17, [%3]\n"	/* S2 */
"	b	2f\n"
"	nop\n"
"	ldr	s16, [%3]\n"		/* S1 */
"	b	1f\n"
"	nop\n"
"	ldp	d16, d17, [%3]\n"	/* D4 */
"	ldp	d18, d19, [%3, #16]\n"
"	b	4f\n"
"	ldp	d16, d17, [%3]\n"	/* D3 */
"	ldr	d18, [%3, #16]\n"
"	b	3f\n"
"	ldp	d16, d17, [%3]\n"	/* D2 */
"	b	2f\n"
"	nop\n"
"	ldr	d16, [%3]\n"		/* D1 */
"	b	1f\n"
"	nop\n"
"	ldp	q16, q17, [%3]\n"	/* Q4 */
"	ldp	q18, q19, [%3, #32]\n"
"	b	4f\n"
"	ldp	q16, q17, [%3]\n"	/* Q3 */
"	ldr	q18, [%3, #32]\n"
"	b	3f\n"
"	ldp	q16, q17, [%3]\n"	/* Q2 */
"	b	2f\n"
"	nop\n"
"	ldr	q16, [%3]\n"		/* Q1 */
"	b	1f\n"
"4:	str	q19, [%2, #48]\n"
"3:	str	q18, [%2, #32]\n"
"2:	str	q17, [%2, #16]\n"
"1:	str	q16, [%2]"
    : "=&r"(x0)
    : "r"(f * 12), "r"(dest), "r"(src)
    : "memory", "v16", "v17", "v18", "v19");
}