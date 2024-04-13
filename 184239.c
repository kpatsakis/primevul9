disable_noname_group_capture(Node** root, regex_t* reg, ScanEnv* env)
{
  int r, i, pos, counter;
  MemStatusType loc;
  GroupNumMap* map;

  map = (GroupNumMap* )xalloca(sizeof(GroupNumMap) * (env->num_mem + 1));
  CHECK_NULL_RETURN_MEMERR(map);
  for (i = 1; i <= env->num_mem; i++) {
    map[i].new_val = 0;
  }
  counter = 0;
  r = make_named_capture_number_map(root, map, &counter);
  if (r != 0) return r;

  r = renumber_backref_traverse(*root, map);
  if (r != 0) return r;

  for (i = 1, pos = 1; i <= env->num_mem; i++) {
    if (map[i].new_val > 0) {
      SCANENV_MEMENV(env)[pos] = SCANENV_MEMENV(env)[i];
      pos++;
    }
  }

  loc = env->cap_history;
  MEM_STATUS_CLEAR(env->cap_history);
  for (i = 1; i <= ONIG_MAX_CAPTURE_HISTORY_GROUP; i++) {
    if (MEM_STATUS_AT(loc, i)) {
      MEM_STATUS_ON_SIMPLE(env->cap_history, map[i].new_val);
    }
  }

  env->num_mem = env->num_named;
  reg->num_mem = env->num_named;

  return onig_renumber_name_table(reg, map);
}