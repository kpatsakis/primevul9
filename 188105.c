node_extended_grapheme_cluster(Node** np, ScanEnv* env)
{
  Node* tmp = NULL;
  Node* np1 = NULL;
  Node* top_alt = NULL;
  int r = 0;
  int num1;
  int i;
  int any_target_position;
  UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN * 2];
  OnigOptionType option;
  /* node_common is function-global so that we can free all nodes
   * in case of error. Unused slots are set to NULL_NODE at all times. */
  Node *node_common[NODE_COMMON_SIZE];
  Node **alts = node_common+0; /* size: 5 */

  for (i=0; i<NODE_COMMON_SIZE; i++)
    node_common[i] = NULL_NODE;

  /* CRLF, common for both Unicode and non-Unicode */
  /* \x0D\x0A */
  r = ONIGENC_CODE_TO_MBC(env->enc, 0x0D, buf);
  if (r < 0) goto err;
  num1 = r;
  r = ONIGENC_CODE_TO_MBC(env->enc, 0x0A, buf + num1);
  if (r < 0) goto err;
  alts[0] = node_new_str_raw(buf, buf + num1 + r);
  if (IS_NULL(alts[0])) goto err;

#ifdef USE_UNICODE_PROPERTIES
  if (ONIGENC_IS_UNICODE(env->enc)) {  /* UTF-8, UTF-16BE/LE, UTF-32BE/LE */
    CClassNode* cc;

    if (propname2ctype(env, "Grapheme_Cluster_Break=Extend") < 0) goto err;
    /* Unicode 11.0.0
     *   CRLF     (already done)
     * | [Control CR LF]
     * | precore* core postcore*
     * | .      (to catch invalid stuff, because this seems to be spec for String#grapheme_clusters) */

    /* [Control CR LF]    (CR and LF are not in the spec, but this is a conformed fix) */
    alts[1] = node_new_cclass();
    if (IS_NULL(alts[1])) goto err;
    cc = NCCLASS(alts[1]);
    R_ERR(add_property_to_cc(cc, "Grapheme_Cluster_Break=Control", 0, env));
    if (ONIGENC_MBC_MINLEN(env->enc) > 1) { /* UTF-16/UTF-32 */
      R_ERR(add_code_range(&(cc->mbuf), env, 0x000A, 0x000A)); /* CR */
      R_ERR(add_code_range(&(cc->mbuf), env, 0x000D, 0x000D)); /* LF */
    }
    else {
      BITSET_SET_BIT(cc->bs, 0x0a);
      BITSET_SET_BIT(cc->bs, 0x0d);
    }

    /* precore* core postcore* */
    {
      Node **list = alts + 3; /* size: 4 */

      /* precore*; precore := Prepend */
      R_ERR(quantify_property_node(list+0, env, "Grapheme_Cluster_Break=Prepend", '*'));

      /* core := hangul-syllable
       *       | ri-sequence
       *       | xpicto-sequence
       *       | [^Control CR LF] */
      {
        Node **core_alts = list + 2; /* size: 7 */

        /* hangul-syllable :=
         *     L* (V+ | LV V* | LVT) T*
         *   | L+
         *   | T+ */
        /* hangul-syllable is an alternative (would be called H_alt)
         * inside an alternative, but we flatten it into core_alts */

        /* L* (V+ | LV V* | LVT) T* */
        {
          Node **H_list = core_alts + 1; /* size: 4 */
          R_ERR(quantify_property_node(H_list+0, env, "Grapheme_Cluster_Break=L", '*'));

          /* V+ | LV V* | LVT */
          {
            Node **H_alt2 = H_list + 2; /* size: 4 */
            R_ERR(quantify_property_node(H_alt2+0, env, "Grapheme_Cluster_Break=V", '+'));

            /* LV V* */
            {
              Node **H_list2 = H_alt2 + 2; /* size: 3 */

              R_ERR(create_property_node(H_list2+0, env, "Grapheme_Cluster_Break=LV"));
              R_ERR(quantify_property_node(H_list2+1, env, "Grapheme_Cluster_Break=V", '*'));
              R_ERR(create_node_from_array(LIST, H_alt2+1, H_list2));
            }

            R_ERR(create_property_node(H_alt2+2, env, "Grapheme_Cluster_Break=LVT"));
            R_ERR(create_node_from_array(ALT, H_list+1, H_alt2));
          }

          R_ERR(quantify_property_node(H_list+2, env, "Grapheme_Cluster_Break=T", '*'));
          R_ERR(create_node_from_array(LIST, core_alts+0, H_list));
        }

        R_ERR(quantify_property_node(core_alts+1, env, "Grapheme_Cluster_Break=L", '+'));
        R_ERR(quantify_property_node(core_alts+2, env, "Grapheme_Cluster_Break=T", '+'));
        /* end of hangul-syllable */

        /* ri-sequence := RI RI */
        R_ERR(quantify_property_node(core_alts+3, env, "Regional_Indicator", '2'));

        /* xpicto-sequence := \p{Extended_Pictographic} (Extend* ZWJ \p{Extended_Pictographic})* */
        {
          Node **XP_list = core_alts + 5; /* size: 3 */
          R_ERR(create_property_node(XP_list+0, env, "Extended_Pictographic"));

          /* (Extend* ZWJ \p{Extended_Pictographic})* */
          {
            Node **Ex_list = XP_list + 2; /* size: 4 */
            /* assert(Ex_list+4 == node_common+NODE_COMMON_SIZE); */
            R_ERR(quantify_property_node(Ex_list+0, env, "Grapheme_Cluster_Break=Extend", '*'));

            /* ZWJ (ZERO WIDTH JOINER) */
            r = ONIGENC_CODE_TO_MBC(env->enc, 0x200D, buf);
            if (r < 0) goto err;
            Ex_list[1] = node_new_str_raw(buf, buf + r);
            if (IS_NULL(Ex_list[1])) goto err;

            R_ERR(create_property_node(Ex_list+2, env, "Extended_Pictographic"));
            R_ERR(create_node_from_array(LIST, XP_list+1, Ex_list));
          }
          R_ERR(quantify_node(XP_list+1, 0, REPEAT_INFINITE)); /* TODO: Check about node freeing */

          R_ERR(create_node_from_array(LIST, core_alts+4, XP_list));
        }

        /* [^Control CR LF] */
        core_alts[5] = node_new_cclass();
        if (IS_NULL(core_alts[5])) goto err;
        cc = NCCLASS(core_alts[5]);
        if (ONIGENC_MBC_MINLEN(env->enc) > 1) { /* UTF-16/UTF-32 */
          BBuf *inverted_buf = NULL;

          /* Start with a positive buffer and invert at the end.
           * Otherwise, adding single-character ranges work the wrong way. */
          R_ERR(add_property_to_cc(cc, "Grapheme_Cluster_Break=Control", 0, env));
          R_ERR(add_code_range(&(cc->mbuf), env, 0x000A, 0x000A)); /* CR */
          R_ERR(add_code_range(&(cc->mbuf), env, 0x000D, 0x000D)); /* LF */
          R_ERR(not_code_range_buf(env->enc, cc->mbuf, &inverted_buf, env));
          cc->mbuf = inverted_buf; /* TODO: check what to do with buffer before inversion */
        }
        else {
          R_ERR(add_property_to_cc(cc, "Grapheme_Cluster_Break=Control", 1, env));
          BITSET_CLEAR_BIT(cc->bs, 0x0a);
          BITSET_CLEAR_BIT(cc->bs, 0x0d);
        }

        R_ERR(create_node_from_array(ALT, list+1, core_alts));
      }

      /* postcore*; postcore = [Extend ZWJ SpacingMark] */
      R_ERR(create_property_node(list+2, env, "Grapheme_Cluster_Break=Extend"));
      cc = NCCLASS(list[2]);
      R_ERR(add_property_to_cc(cc, "Grapheme_Cluster_Break=SpacingMark", 0, env));
      R_ERR(add_code_range(&(cc->mbuf), env, 0x200D, 0x200D));
      R_ERR(quantify_node(list+2, 0, REPEAT_INFINITE));

      R_ERR(create_node_from_array(LIST, alts+2, list));
    }

    any_target_position = 3;
  }
  else
#endif /* USE_UNICODE_PROPERTIES */
  {
    any_target_position = 1;
  }

  /* PerlSyntax: (?s:.), RubySyntax: (?m:.), common for both Unicode and non-Unicode */
  /* Not in Unicode spec (UAX #29), but added to catch invalid stuff,
   * because this is Ruby spec for String#grapheme_clusters. */
  np1 = node_new_anychar();
  if (IS_NULL(np1)) goto err;

  option = env->option;
  ONOFF(option, ONIG_OPTION_MULTILINE, 0);
  tmp = node_new_option(option);
  if (IS_NULL(tmp)) goto err;
  NENCLOSE(tmp)->target = np1;
  alts[any_target_position] = tmp;
  np1 = NULL;

  R_ERR(create_node_from_array(ALT, &top_alt, alts));

  /* (?>): For efficiency, because there is no text piece
   *       that is not in a grapheme cluster, and there is only one way
   *       to split a string into grapheme clusters. */
  tmp = node_new_enclose(ENCLOSE_STOP_BACKTRACK);
  if (IS_NULL(tmp)) goto err;
  NENCLOSE(tmp)->target = top_alt;
  np1 = tmp;

#ifdef USE_UNICODE_PROPERTIES
  if (ONIGENC_IS_UNICODE(env->enc)) {
    /* Don't ignore case. */
    option = env->option;
    ONOFF(option, ONIG_OPTION_IGNORECASE, 1);
    *np = node_new_option(option);
    if (IS_NULL(*np)) goto err;
    NENCLOSE(*np)->target = np1;
  }
  else
#endif
  {
    *np = np1;
  }
  return ONIG_NORMAL;

 err:
  onig_node_free(np1);
  for (i=0; i<NODE_COMMON_SIZE; i++)
    onig_node_free(node_common[i]);
  return (r == 0) ? ONIGERR_MEMORY : r;
}