png_check_fp_number(png_const_charp string, png_size_t size, int *statep,
    png_size_tp whereami)
{
   int state = *statep;
   png_size_t i = *whereami;

   while (i < size)
   {
      int type;
      /* First find the type of the next character */
      switch (string[i])
      {
      case 43:  type = PNG_FP_SAW_SIGN;                   break;
      case 45:  type = PNG_FP_SAW_SIGN + PNG_FP_NEGATIVE; break;
      case 46:  type = PNG_FP_SAW_DOT;                    break;
      case 48:  type = PNG_FP_SAW_DIGIT;                  break;
      case 49: case 50: case 51: case 52:
      case 53: case 54: case 55: case 56:
      case 57:  type = PNG_FP_SAW_DIGIT + PNG_FP_NONZERO; break;
      case 69:
      case 101: type = PNG_FP_SAW_E;                      break;
      default:  goto PNG_FP_End;
      }

      /* Now deal with this type according to the current
       * state, the type is arranged to not overlap the
       * bits of the PNG_FP_STATE.
       */
      switch ((state & PNG_FP_STATE) + (type & PNG_FP_SAW_ANY))
      {
      case PNG_FP_INTEGER + PNG_FP_SAW_SIGN:
         if ((state & PNG_FP_SAW_ANY) != 0)
            goto PNG_FP_End; /* not a part of the number */

         png_fp_add(state, type);
         break;

      case PNG_FP_INTEGER + PNG_FP_SAW_DOT:
         /* Ok as trailer, ok as lead of fraction. */
         if ((state & PNG_FP_SAW_DOT) != 0) /* two dots */
            goto PNG_FP_End;

         else if ((state & PNG_FP_SAW_DIGIT) != 0) /* trailing dot? */
            png_fp_add(state, type);

         else
            png_fp_set(state, PNG_FP_FRACTION | type);

         break;

      case PNG_FP_INTEGER + PNG_FP_SAW_DIGIT:
         if ((state & PNG_FP_SAW_DOT) != 0) /* delayed fraction */
            png_fp_set(state, PNG_FP_FRACTION | PNG_FP_SAW_DOT);

         png_fp_add(state, type | PNG_FP_WAS_VALID);

         break;

      case PNG_FP_INTEGER + PNG_FP_SAW_E:
         if ((state & PNG_FP_SAW_DIGIT) == 0)
            goto PNG_FP_End;

         png_fp_set(state, PNG_FP_EXPONENT);

         break;

   /* case PNG_FP_FRACTION + PNG_FP_SAW_SIGN:
         goto PNG_FP_End; ** no sign in fraction */

   /* case PNG_FP_FRACTION + PNG_FP_SAW_DOT:
         goto PNG_FP_End; ** Because SAW_DOT is always set */

      case PNG_FP_FRACTION + PNG_FP_SAW_DIGIT:
         png_fp_add(state, type | PNG_FP_WAS_VALID);
         break;

      case PNG_FP_FRACTION + PNG_FP_SAW_E:
         /* This is correct because the trailing '.' on an
          * integer is handled above - so we can only get here
          * with the sequence ".E" (with no preceding digits).
          */
         if ((state & PNG_FP_SAW_DIGIT) == 0)
            goto PNG_FP_End;

         png_fp_set(state, PNG_FP_EXPONENT);

         break;

      case PNG_FP_EXPONENT + PNG_FP_SAW_SIGN:
         if ((state & PNG_FP_SAW_ANY) != 0)
            goto PNG_FP_End; /* not a part of the number */

         png_fp_add(state, PNG_FP_SAW_SIGN);

         break;

   /* case PNG_FP_EXPONENT + PNG_FP_SAW_DOT:
         goto PNG_FP_End; */

      case PNG_FP_EXPONENT + PNG_FP_SAW_DIGIT:
         png_fp_add(state, PNG_FP_SAW_DIGIT | PNG_FP_WAS_VALID);

         break;

   /* case PNG_FP_EXPONEXT + PNG_FP_SAW_E:
         goto PNG_FP_End; */

      default: goto PNG_FP_End; /* I.e. break 2 */
      }

      /* The character seems ok, continue. */
      ++i;
   }

PNG_FP_End:
   /* Here at the end, update the state and return the correct
    * return code.
    */
   *statep = state;
   *whereami = i;

   return (state & PNG_FP_SAW_DIGIT) != 0;
}