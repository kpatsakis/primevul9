aac_profile_idx (const gchar * codec_id)
{
  gint profile;

  if (strlen (codec_id) <= 12)
    profile = 3;
  else if (!strncmp (&codec_id[12], "MAIN", 4))
    profile = 0;
  else if (!strncmp (&codec_id[12], "LC", 2))
    profile = 1;
  else if (!strncmp (&codec_id[12], "SSR", 3))
    profile = 2;
  else
    profile = 3;

  return profile;
}