char *fits_search_card (FITS_RECORD_LIST *rl, char *keyword)

{int key_len, k;
 char *card;
 char key[9];

 key_len = strlen (keyword);
 if (key_len > 8) key_len = 8;
 if (key_len == 0)
   FITS_RETURN ("fits_search_card: Invalid parameter", NULL);

 strcpy (key, "        ");
 memcpy (key, keyword, key_len);

 while (rl != NULL)
 {
   card = (char *)rl->data;
   for (k = 0; k < FITS_RECORD_SIZE / FITS_CARD_SIZE; k++)
   {
     if (strncmp (card, key, 8) == 0) return (card);
     card += FITS_CARD_SIZE;
   }
   rl = rl->next_record;
 }
 return (NULL);
}