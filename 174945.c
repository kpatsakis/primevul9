ReadChannelMasks (guint32 *tmp, Bitmap_Channel *masks, guint channels)
{
  guint32 mask;
  gint   i, nbits, offset, bit;

  for (i = 0; i < channels; i++)
    {
      mask = tmp[i];
      masks[i].mask = mask;
      nbits = 0;
      offset = -1;
      for (bit = 0; bit < 32; bit++)
        {
          if (mask & 1)
            {
              nbits++;
              if (offset == -1)
                offset = bit;
            }
          mask = mask >> 1;
        }
      masks[i].shiftin = offset;
      masks[i].max_value = (gfloat)((1<<(nbits))-1);

#ifdef DEBUG
      g_print ("Channel %d mask %08x in %d max_val %d\n",
               i, masks[i].mask, masks[i].shiftin, (gint)masks[i].max_value);
#endif
    }

  return TRUE;
}