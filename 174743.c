pwg_free_material(_pwg_material_t *m)	/* I - Material value */
{
  _cupsStrFree(m->key);
  _cupsStrFree(m->name);

  cupsFreeOptions(m->num_props, m->props);

  free(m);
}