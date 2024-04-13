static unsigned char *ReadImage(FILE * fd, int width, int height,
	unsigned char cmap[256][3],
	int ncols,
	int bpp, int compression, int rowbytes,
	gboolean    Grey,
	const Bitmap_Channel * masks,
	at_exception_type * exp)
{
  unsigned char v, n;
  int xpos = 0;
  int ypos = 0;
  unsigned char * image;
  unsigned char *dest, *temp, *row_buf;
  long rowstride, channels;
  unsigned short rgb;
  int i, i_max, j;
  int total_bytes_read;
  unsigned int px32;

  if (!(compression == BI_RGB ||
	  (bpp == 8 && compression == BI_RLE8) ||
	  (bpp == 4 && compression == BI_RLE4) ||
	  (bpp == 16 && compression == BI_BITFIELDS) ||
	  (bpp == 32 && compression == BI_BITFIELDS)))
  {
	  LOG("Unrecognized or invalid BMP compression format.\n");
	  at_exception_fatal(exp, "Unrecognized or invalid BMP compression format.");
	  return NULL;
  }

  if (bpp >= 16) {              /* color image */
    XMALLOC(image, width * height * 3 * sizeof(unsigned char));
    if (masks[3].mask != 0)
    {
      channels = 4;
    }
    else
    {
      channels = 3;
    }
  }
  else if (Grey) /* Grey image */
  {
    XMALLOC(image, width * height * 1 * sizeof(unsigned char));
    channels = 1;
  } else {                      /* indexed image */

    XMALLOC(image, width * height * 1 * sizeof(unsigned char));
    channels = 1;
  }

  /* use XCALLOC to initialize the dest row_buf so that unspecified
	 pixels in RLE bitmaps show up as the zeroth element in the palette.
  */
  XCALLOC(dest, width * height * channels);
  XMALLOC (row_buf, rowbytes); 
  rowstride = width * channels;

  ypos = height - 1;            /* Bitmaps begin in the lower left corner */

  switch (bpp) {

  case 32:
    {
      while (ReadOK (fd, row_buf, rowbytes))
      {
        temp = image + (ypos * rowstride);
        for (xpos = 0; xpos < width; ++xpos) {
			  px32 = ToL(&row_buf[xpos * 4]);
			  unsigned char red = *(temp++) = ((px32 & masks[0].mask) >> masks[0].shiftin) * 255.0 / masks[0].max_value + 0.5;
			  unsigned char green = *(temp++) = ((px32 & masks[1].mask) >> masks[1].shiftin) * 255.0 / masks[1].max_value + 0.5;
			  unsigned char blue = *(temp++) = ((px32 & masks[2].mask) >> masks[2].shiftin) * 255.0 / masks[2].max_value + 0.5;
			  /* currently alpha channels are not supported by AutoTrace, thus simply ignored */
			  /*if (channels > 3)
				  *(temp++) = ((px32 & masks[3].mask) >> masks[3].shiftin) * 255.0 / masks[3].max_value + 0.5;*/
		  }

		  if (ypos == 0)
			  break;

		  --ypos; /* next line */
      }
    }
    break;

  case 24:
    {
      while (ReadOK (fd, row_buf, rowbytes))
      {
        temp = image + (ypos * rowstride);
        for (xpos = 0; xpos < width; ++xpos) {
          *(temp++) = row_buf[xpos * 3 + 2];
          *(temp++) = row_buf[xpos * 3 + 1];
          *(temp++) = row_buf[xpos * 3];
        }
        --ypos;                 /* next line */
      }
    }
    break;

  case 16:
    {
      while (ReadOK (fd, row_buf, rowbytes))
      {
        temp = image + (ypos * rowstride);
        for (xpos = 0; xpos < width; ++xpos)
        {
			  rgb = ToS(&row_buf[xpos * 2]);
			  *(temp++) = ((rgb & masks[0].mask) >> masks[0].shiftin) * 255.0 / masks[0].max_value + 0.5;
			  *(temp++) = ((rgb & masks[1].mask) >> masks[1].shiftin) * 255.0 / masks[1].max_value + 0.5;
			  *(temp++) = ((rgb & masks[2].mask) >> masks[2].shiftin) * 255.0 / masks[2].max_value + 0.5;
			  /* currently alpha channels are not supported by AutoTrace, thus simply ignored */
			  /*if (channels > 3)
				  *(temp++) = ((rgb & masks[3].mask) >> masks[3].shiftin) * 255.0 / masks[3].max_value + 0.5;*/
		  }

		  if (ypos == 0)
			  break;

		  --ypos; /* next line */
      }
    }
    break;

  case 8:
  case 4:
  case 1:
    {
      if (compression == 0) {
        while (ReadOK(fd, &v, 1)) {
          for (i = 1; (i <= (8 / bpp)) && (xpos < width); i++, xpos++) {
			temp = image + (ypos * rowstride) + (xpos * channels);
			*temp = (v & (((1 << bpp) - 1) << (8 - (i*bpp)))) >> (8 - (i*bpp));
			if (Grey)
				*temp = cmap[*temp][0];
	  }

          if (xpos == width) {
	    ReadOK (fd, row_buf, rowbytes - 1 - (width * bpp - 1) / 8);
            ypos--;
            xpos = 0;

          }
          if (ypos < 0)
            break;
        }
        break;
      } else {
	/* compressed image (either RLE8 or RLE4) */
        while (ypos >= 0 && xpos <= width) {
			if (!ReadOK(fd, row_buf, 2))
			{
				LOG("The bitmap ends unexpectedly.");
				break;
			}

			if ((unsigned char) row_buf[0] != 0)
		      /* Count + Color - record */
		      {
				/* encoded mode run -
				 *   row_buf[0] == run_length
				 *   row_buf[1] == pixel data
				 */
		        for (j = 0; ((unsigned char) j < (unsigned char) row_buf[0]) && (xpos < width);)
		          {
#ifdef DEBUG2
              printf("%u %u | ", xpos, width);
#endif
			        for (i = 1;
			             ((i <= (8 / bpp)) &&
			             (xpos < width) &&
			             ((unsigned char) j < (unsigned char) row_buf[0]));
			             i++, xpos++, j++)
			          {
			            temp = dest + (ypos * rowstride) + (xpos * channels);
			            *temp = (unsigned char) ((row_buf[1] & (((1<<bpp)-1) << (8 - (i * bpp)))) >> (8 - (i * bpp)));
				        if (Grey)
					      *temp = cmap[*temp][0];
			          }
            }
          }
          if ((row_buf[0] == 0) && (row_buf[1] > 2))
            /* uncompressed record */
		{
			n = row_buf[1];
			total_bytes_read = 0;

			for (j = 0; j < n; j += (8 / bpp))
			{
				/* read the next byte in the record */
				if (!ReadOK(fd, &v, 1))
				{
					LOG("The bitmap ends unexpectedly.");
					break;
				} 

				total_bytes_read++;

				/* read all pixels from that byte */
				i_max = 8 / bpp;
				if (n - j < i_max)
				{
					i_max = n - j;
				}

				i = 1;
				while ((i <= i_max) && (xpos < width))
				{
					temp =
						dest + (ypos * rowstride) + (xpos * channels);
					*temp = (v >> (8 - (i*bpp))) & ((1 << bpp) - 1);
					if (Grey)
						*temp = cmap[*temp][0];
					i++;
					xpos++;
				}
			}

			/* absolute mode runs are padded to 16-bit alignment */
			if (total_bytes_read % 2)
				fread(&v, 1, 1, fd); //ReadOk
		}
	  if (((unsigned char) row_buf[0] == 0) && ((unsigned char) row_buf[1]==0))
            /* Line end */
          {
            ypos--;
            xpos = 0;
          }
          if (((unsigned char)row_buf[0] == 0) && ((unsigned char)row_buf[1] == 1))
            /* Bitmap end */
          {
            break;
          }
          if (((unsigned char)row_buf[0] == 0) && ((unsigned char)row_buf[1] == 2))
            /* Deltarecord */
          {
			if (!ReadOK(fd, row_buf, 2))
			  {
				LOG("The bitmap ends unexpectedly.");
				break;
			  }
			xpos += (unsigned char) row_buf[0];
		    ypos -= (unsigned char) row_buf[1];
          }
        }
        break;
      }
    }
    break;
  default:
    /* This is very bad, we should not be here */
    break;
  }

  if (bpp <= 8) {
    unsigned char *temp2, *temp3;
    unsigned char index;
    temp2 = temp = image;
    XMALLOC (image, width * height * 3 * sizeof (unsigned char)); //???
    temp3 = image;
    for (ypos = 0; ypos < height; ypos++) {
      for (xpos = 0; xpos < width; xpos++) {
        index = *temp2++;
        *temp3++ = cmap[index][0];
        if (!Grey) {
          *temp3++ = cmap[index][1];
          *temp3++ = cmap[index][2];
        }
      }
    }
    free(temp);
  }

  free (row_buf);
  free(dest);
  return image;
}