dot24_improve_bitmap (byte *data, int count)
{
  int i;
  register byte *p = data + 6;

      for (i = 6; i < count; i += 3, p += 3)
        {
          p[-6] &= ~(~p[0] & p[-3]);
          p[-5] &= ~(~p[1] & p[-2]);
          p[-4] &= ~(~p[2] & p[-1]);
        }
      p[-6] &= ~p[-3];
      p[-5] &= ~p[-2];
      p[-4] &= ~p[-1];

}