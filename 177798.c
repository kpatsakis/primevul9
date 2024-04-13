compress_hfa_type (void *dest, void *reg, int h)
{
  switch (h)
    {
    case AARCH64_RET_S1:
      if (dest == reg)
	{
#ifdef __AARCH64EB__
	  dest += 12;
#endif
	}
      else
	*(float *)dest = *(float *)reg;
      break;
    case AARCH64_RET_S2:
      asm ("ldp q16, q17, [%1]\n\t"
	   "st2 { v16.s, v17.s }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17");
      break;
    case AARCH64_RET_S3:
      asm ("ldp q16, q17, [%1]\n\t"
	   "ldr q18, [%1, #32]\n\t"
	   "st3 { v16.s, v17.s, v18.s }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17", "v18");
      break;
    case AARCH64_RET_S4:
      asm ("ldp q16, q17, [%1]\n\t"
	   "ldp q18, q19, [%1, #32]\n\t"
	   "st4 { v16.s, v17.s, v18.s, v19.s }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17", "v18", "v19");
      break;

    case AARCH64_RET_D1:
      if (dest == reg)
	{
#ifdef __AARCH64EB__
	  dest += 8;
#endif
	}
      else
	*(double *)dest = *(double *)reg;
      break;
    case AARCH64_RET_D2:
      asm ("ldp q16, q17, [%1]\n\t"
	   "st2 { v16.d, v17.d }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17");
      break;
    case AARCH64_RET_D3:
      asm ("ldp q16, q17, [%1]\n\t"
	   "ldr q18, [%1, #32]\n\t"
	   "st3 { v16.d, v17.d, v18.d }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17", "v18");
      break;
    case AARCH64_RET_D4:
      asm ("ldp q16, q17, [%1]\n\t"
	   "ldp q18, q19, [%1, #32]\n\t"
	   "st4 { v16.d, v17.d, v18.d, v19.d }[0], [%0]"
	   : : "r"(dest), "r"(reg) : "memory", "v16", "v17", "v18", "v19");
      break;

    default:
      if (dest != reg)
	return memcpy (dest, reg, 16 * (4 - (h & 3)));
      break;
    }
  return dest;
}