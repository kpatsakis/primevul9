static FITS_RECORD_LIST *fits_read_header (FILE *fp, int *nrec)

{char record[FITS_RECORD_SIZE];
 FITS_RECORD_LIST *start_list = NULL, *cu_record = NULL, *new_record;
 FITS_DATA *fdat;
 int k, simple, xtension;

 *nrec = 0;

 k = fread (record, 1, FITS_RECORD_SIZE, fp);
 if (k != FITS_RECORD_SIZE)
   FITS_RETURN ("fits_read_header: Error in read of first record", NULL);

 simple = (strncmp (record, "SIMPLE  ", 8) == 0);
 xtension = (strncmp (record, "XTENSION", 8) == 0);
 if ((!simple) && (!xtension))
   FITS_RETURN ("fits_read_header: Missing keyword SIMPLE or XTENSION", NULL);

 if (simple)
 {
   fdat = fits_decode_card (record, typ_fbool);
   if (fdat && !fdat->fbool)
     fits_set_error ("fits_read_header (warning): keyword SIMPLE does not have\
 value T");
 }

 for (;;)   /* Process all header records */
 {
   new_record = (FITS_RECORD_LIST *)malloc (sizeof (FITS_RECORD_LIST));
   if (new_record == NULL)
   {
     fits_delete_recordlist (start_list);
     FITS_RETURN ("fits_read_header: Not enough memory", NULL);
   }
   memcpy (new_record->data, record, FITS_RECORD_SIZE);
   new_record->next_record = NULL;
   (*nrec)++;

   if (start_list == NULL)      /* Add new record to the list */
     start_list = new_record;
   else
     cu_record->next_record = new_record;

   cu_record = new_record;
                                /* Was this the last record ? */
   if (fits_search_card (cu_record, "END") != NULL) break;

    k = fread (record, 1, FITS_RECORD_SIZE, fp);
    if (k != FITS_RECORD_SIZE)
      FITS_RETURN ("fits_read_header: Error in read of record", NULL);
 }
 return (start_list);
}