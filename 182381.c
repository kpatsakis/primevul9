static void fits_delete_recordlist (FITS_RECORD_LIST *rl)

{FITS_RECORD_LIST *next;

 while (rl != NULL)
 {
   next = rl->next_record;
   rl->next_record = NULL;
   free ((char *)rl);
   rl = next;
 }
}