FITS_DATA *fits_decode_card (const char *card, FITS_DATA_TYPES data_type)

{static FITS_DATA data;
 long l_long;
 double l_double;
 char l_card[FITS_CARD_SIZE+1], msg[256];
 char *cp, *dst, *end;
 int ErrCount = 0;

 if (card == NULL) return (NULL);

 memcpy (l_card, card, FITS_CARD_SIZE);
 l_card[FITS_CARD_SIZE] = '\0';

 if (strncmp (card+8, "= ", 2) != 0)
 {
   sprintf (msg, "fits_decode_card (warning): Missing value indicator\
 '= ' for %8.8s", l_card);
   fits_set_error (msg);
   ErrCount++;
 }

 switch (data_type)
 {
   case typ_bitpix8:
     data.bitpix8 = (FITS_BITPIX8)(l_card[10]);
     break;

   case typ_bitpix16:
     if (sscanf (l_card+10, "%ld", &l_long) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_bitpix16");
       ErrCount++;
       break;
     }
     data.bitpix16 = (FITS_BITPIX16)l_long;
     break;

   case typ_bitpix32:
     if (sscanf (l_card+10, "%ld", &l_long) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_bitpix32");
       ErrCount++;
       break;
     }
     data.bitpix32 = (FITS_BITPIX32)l_long;
     break;

   case typ_bitpixm32:
     if (fits_scanfdouble (l_card+10, &l_double) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_bitpixm32");
       ErrCount++;
       break;
     }
     data.bitpixm32 = (FITS_BITPIXM32)l_double;
     break;

   case typ_bitpixm64:
     if (fits_scanfdouble (l_card+10, &l_double) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_bitpixm64");
       ErrCount++;
       break;
     }
     data.bitpixm64 = (FITS_BITPIXM64)l_double;
     break;

   case typ_fbool:
     cp = l_card+10;
     while (*cp == ' ') cp++;
     if (*cp == 'T')
     {
       data.fbool = 1;
     }
     else if (*cp == 'F')
     {
       data.fbool = 0;
     }
     else
     {
       fits_set_error ("fits_decode_card: error decoding typ_fbool");
       ErrCount++;
       break;
     }
     break;

   case typ_flong:
     if (sscanf (l_card+10, "%ld", &l_long) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_flong");
       ErrCount++;
       break;
     }
     data.flong = (FITS_BITPIX32)l_long;
     break;

   case typ_fdouble:
     if (fits_scanfdouble (l_card+10, &l_double) != 1)
     {
       fits_set_error ("fits_decode_card: error decoding typ_fdouble");
       ErrCount++;
       break;
     }
     data.fdouble = (FITS_BITPIXM32)l_double;
     break;

   case typ_fstring:
     cp = l_card+10;
     if (*cp != '\'')
     {
       fits_set_error ("fits_decode_card: missing \' decoding typ_fstring");
       ErrCount++;
       break;
     }

     dst = data.fstring;
     cp++;
     end = l_card+FITS_CARD_SIZE-1;
     for (;;)   /* Search for trailing quote */
     {
       if (*cp != '\'')    /* All characters but quote are used. */
       {
         *(dst++) = *cp;
       }
       else                /* Maybe there is a quote in the string */
       {
         if (cp >= end) break;  /* End of card ? finished */
         if (*(cp+1) != '\'') break;
         *(dst++) = *(cp++);
       }
       if (cp >= end) break;
       cp++;
     }
     *dst = '\0';
     break;
 }

 return ((ErrCount == 0) ? &data : NULL);
}