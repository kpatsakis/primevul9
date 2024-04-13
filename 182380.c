int fits_write_header (FITS_FILE *ff, FITS_HDU_LIST *hdulist)

{int numcards;
 int k;

 if (ff->openmode != 'w')
   FITS_RETURN ("fits_write_header: file not open for writing", -1);

 numcards = 0;

 if (hdulist->used.simple)
 {
   FITS_WRITE_BOOLCARD (ff->fp, "SIMPLE", 1);
   numcards++;
 }
 else if (hdulist->used.xtension)
 {
   FITS_WRITE_STRINGCARD (ff->fp, "XTENSION", hdulist->xtension);
   numcards++;
 }

 FITS_WRITE_LONGCARD (ff->fp, "BITPIX", hdulist->bitpix);
 numcards++;

 FITS_WRITE_LONGCARD (ff->fp, "NAXIS", hdulist->naxis);
 numcards++;

 for (k = 0; k < hdulist->naxis; k++)
 {char naxisn[10];
   sprintf (naxisn, "NAXIS%d", k+1);
   FITS_WRITE_LONGCARD (ff->fp, naxisn, hdulist->naxisn[k]);
   numcards++;
 }

 if (hdulist->used.extend)
 {
   FITS_WRITE_BOOLCARD (ff->fp, "EXTEND", hdulist->extend);
   numcards++;
 }

 if (hdulist->used.groups)
 {
   FITS_WRITE_BOOLCARD (ff->fp, "GROUPS", hdulist->groups);
   numcards++;
 }

 if (hdulist->used.pcount)
 {
   FITS_WRITE_LONGCARD (ff->fp, "PCOUNT", hdulist->pcount);
   numcards++;
 }
 if (hdulist->used.gcount)
 {
   FITS_WRITE_LONGCARD (ff->fp, "GCOUNT", hdulist->gcount);
   numcards++;
 }

 if (hdulist->used.bzero)
 {
   FITS_WRITE_DOUBLECARD (ff->fp, "BZERO", hdulist->bzero);
   numcards++;
 }
 if (hdulist->used.bscale)
 {
   FITS_WRITE_DOUBLECARD (ff->fp, "BSCALE", hdulist->bscale);
   numcards++;
 }

 if (hdulist->used.datamin)
 {
   FITS_WRITE_DOUBLECARD (ff->fp, "DATAMIN", hdulist->datamin);
   numcards++;
 }
 if (hdulist->used.datamax)
 {
   FITS_WRITE_DOUBLECARD (ff->fp, "DATAMAX", hdulist->datamax);
   numcards++;
 }

 if (hdulist->used.blank)
 {
   FITS_WRITE_LONGCARD (ff->fp, "BLANK", hdulist->blank);
   numcards++;
 }

 /* Write additional cards */
 if (hdulist->naddcards > 0)
 {
   fwrite (hdulist->addcards, FITS_CARD_SIZE, hdulist->naddcards, ff->fp);
   numcards += hdulist->naddcards;
 }

 FITS_WRITE_CARD (ff->fp, "END");
 numcards++;

 k = (numcards*FITS_CARD_SIZE) % FITS_RECORD_SIZE;
 if (k)  /* Must the record be filled up ? */
 {
   while (k++ < FITS_RECORD_SIZE)
     putc (' ', ff->fp);
 }


 return (ferror (ff->fp) ? -1 : 0);
}