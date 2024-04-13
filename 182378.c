int fits_add_card (FITS_HDU_LIST *hdulist, const char *card)

{int k;

 if (hdulist->naddcards >= FITS_NADD_CARDS) return (-1);

 k = strlen (card);
 if (k < FITS_CARD_SIZE)
 {
   memset (&(hdulist->addcards[hdulist->naddcards][k]), ' ', FITS_CARD_SIZE-k);
   memcpy (hdulist->addcards[(hdulist->naddcards)++], card, k);
 }
 else
 {
   memcpy (hdulist->addcards[(hdulist->naddcards)++], card, FITS_CARD_SIZE);
 }
 return (0);
}