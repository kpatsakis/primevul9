flatpak_extension_compare_by_path (gconstpointer _a,
                                   gconstpointer _b)
{
  const FlatpakExtension *a = _a;
  const FlatpakExtension *b = _b;

  return g_strcmp0 (a->directory, b->directory);
}