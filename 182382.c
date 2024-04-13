static void fits_delete_hdulist (FITS_HDU_LIST *hl)

{FITS_HDU_LIST *next;

 while (hl != NULL)
 {
   fits_delete_recordlist (hl->header_record_list);
   next = hl->next_hdu;
   hl->next_hdu = NULL;
   free ((char *)hl);
   hl = next;
 }
}