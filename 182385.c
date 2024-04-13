void fits_print_header (FITS_HDU_LIST *hdr)

{int k;
 char buf[G_ASCII_DTOSTR_BUF_SIZE];

 if (hdr->used.simple)
   printf ("Content of SIMPLE-header:\n");
 else
   printf ("Content of XTENSION-header %s:\n", hdr->xtension);
 printf ("header_offset : %ld\n", hdr->header_offset);
 printf ("data_offset   : %ld\n", hdr->data_offset);
 printf ("data_size     : %ld\n", hdr->data_size);
 printf ("used data_size: %ld\n", hdr->udata_size);
 printf ("bytes p.pixel : %d\n", hdr->bpp);
 printf ("pixmin        : %s\n", FDTOSTR (buf, hdr->pixmin));
 printf ("pixmax        : %s\n", FDTOSTR (buf, hdr->pixmax));

 printf ("naxis         : %d\n", hdr->naxis);
 for (k = 1; k <= hdr->naxis; k++)
   printf ("naxis%-3d      : %d\n", k, hdr->naxisn[k-1]);

 printf ("bitpix        : %d\n", hdr->bitpix);

 if (hdr->used.blank)
   printf ("blank         : %ld\n", hdr->blank);
 else
   printf ("blank         : not used\n");

 if (hdr->used.datamin)
   printf ("datamin       : %s\n", FDTOSTR (buf, hdr->datamin));
 else
   printf ("datamin       : not used\n");
 if (hdr->used.datamax)
   printf ("datamax       : %s\n", FDTOSTR (buf, hdr->datamax));
 else
   printf ("datamax       : not used\n");

 if (hdr->used.gcount)
   printf ("gcount        : %ld\n", hdr->gcount);
 else
   printf ("gcount        : not used\n");
 if (hdr->used.pcount)
   printf ("pcount        : %ld\n", hdr->pcount);
 else
   printf ("pcount        : not used\n");

 if (hdr->used.bscale)
   printf ("bscale        : %s\n", FDTOSTR (buf, hdr->bscale));
 else
   printf ("bscale        : not used\n");
 if (hdr->used.bzero)
   printf ("bzero         : %s\n", FDTOSTR (buf, hdr->bzero));
 else
   printf ("bzero         : not used\n");
}