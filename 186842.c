fribidi_get_par_embedding_levels_ex (
  /* input */
  const FriBidiCharType *bidi_types,
  const FriBidiBracketType *bracket_types,
  const FriBidiStrIndex len,
  /* input and output */
  FriBidiParType *pbase_dir,
  /* output */
  FriBidiLevel *embedding_levels
)
{
  FriBidiLevel base_level_per_iso_level[FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL];
  FriBidiLevel base_level, max_level = 0;
  FriBidiParType base_dir;
  FriBidiRun *main_run_list = NULL, *explicits_list = NULL, *pp;
  fribidi_boolean status = false;
  int max_iso_level = 0;

  if UNLIKELY
    (!len)
    {
      status = true;
      goto out;
    }

  DBG ("in fribidi_get_par_embedding_levels");

  fribidi_assert (bidi_types);
  fribidi_assert (pbase_dir);
  fribidi_assert (embedding_levels);

  /* Determinate character types */
  {
    /* Get run-length encoded character types */
    main_run_list = run_list_encode_bidi_types (bidi_types, bracket_types, len);
    if UNLIKELY
      (!main_run_list) goto out;
  }

  /* Find base level */
  /* If no strong base_dir was found, resort to the weak direction
     that was passed on input. */
  base_level = FRIBIDI_DIR_TO_LEVEL (*pbase_dir);
  if (!FRIBIDI_IS_STRONG (*pbase_dir))
    /* P2. P3. Search for first strong character and use its direction as
       base direction */
    {
      int valid_isolate_count = 0;
      for_run_list (pp, main_run_list)
        {
          if (RL_TYPE(pp) == FRIBIDI_TYPE_PDI)
            {
              /* Ignore if there is no matching isolate */
              if (valid_isolate_count>0)
                valid_isolate_count--;
            }
          else if (FRIBIDI_IS_ISOLATE(RL_TYPE(pp)))
            valid_isolate_count++;
          else if (valid_isolate_count==0 && FRIBIDI_IS_LETTER (RL_TYPE (pp)))
            {
              base_level = FRIBIDI_DIR_TO_LEVEL (RL_TYPE (pp));
              *pbase_dir = FRIBIDI_LEVEL_TO_DIR (base_level);
              break;
            }
        }
    }
  base_dir = FRIBIDI_LEVEL_TO_DIR (base_level);
  DBG2 ("  base level : %c", fribidi_char_from_level (base_level));
  DBG2 ("  base dir   : %s", fribidi_get_bidi_type_name (base_dir));

  base_level_per_iso_level[0] = base_level;

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_types_re (main_run_list);
    }
# endif	/* DEBUG */

  /* Explicit Levels and Directions */
  DBG ("explicit levels and directions");
  {
    FriBidiLevel level, new_level = 0;
    int isolate_level = 0;
    FriBidiCharType override, new_override;
    FriBidiStrIndex i;
    int stack_size, over_pushed, first_interval;
    int valid_isolate_count = 0;
    int isolate_overflow = 0;
    int isolate = 0; /* The isolate status flag */
    struct
    {
      FriBidiCharType override;	/* only LTR, RTL and ON are valid */
      FriBidiLevel level;
      int isolate;
      int isolate_level;
    } status_stack[FRIBIDI_BIDI_MAX_RESOLVED_LEVELS];
    FriBidiRun temp_link;
    FriBidiRun *run_per_isolate_level[FRIBIDI_BIDI_MAX_RESOLVED_LEVELS];

    memset(run_per_isolate_level, 0, sizeof(run_per_isolate_level[0])
           * FRIBIDI_BIDI_MAX_RESOLVED_LEVELS);

/* explicits_list is a list like main_run_list, that holds the explicit
   codes that are removed from main_run_list, to reinsert them later by
   calling the shadow_run_list.
*/
    explicits_list = new_run_list ();
    if UNLIKELY
      (!explicits_list) goto out;

    /* X1. Begin by setting the current embedding level to the paragraph
       embedding level. Set the directional override status to neutral,
       and directional isolate status to false.

       Process each character iteratively, applying rules X2 through X8.
       Only embedding levels from 0 to 123 are valid in this phase. */

    level = base_level;
    override = FRIBIDI_TYPE_ON;
    /* stack */
    stack_size = 0;
    over_pushed = 0;
    first_interval = 0;
    valid_isolate_count = 0;
    isolate_overflow = 0;

    for_run_list (pp, main_run_list)
    {
      FriBidiCharType this_type = RL_TYPE (pp);
      RL_ISOLATE_LEVEL (pp) = isolate_level;

      if (FRIBIDI_IS_EXPLICIT_OR_BN (this_type))
	{
	  if (FRIBIDI_IS_STRONG (this_type))
	    {			/* LRE, RLE, LRO, RLO */
	      /* 1. Explicit Embeddings */
	      /*   X2. With each RLE, compute the least greater odd
	         embedding level. */
	      /*   X3. With each LRE, compute the least greater even
	         embedding level. */
	      /* 2. Explicit Overrides */
	      /*   X4. With each RLO, compute the least greater odd
	         embedding level. */
	      /*   X5. With each LRO, compute the least greater even
	         embedding level. */
	      new_override = FRIBIDI_EXPLICIT_TO_OVERRIDE_DIR (this_type);
	      for (i = RL_LEN (pp); i; i--)
		{
		  new_level =
		    ((level + FRIBIDI_DIR_TO_LEVEL (this_type) + 2) & ~1) -
		    FRIBIDI_DIR_TO_LEVEL (this_type);
                  isolate = 0;
		  PUSH_STATUS;
		}
	    }
	  else if (this_type == FRIBIDI_TYPE_PDF)
	    {
	      /* 3. Terminating Embeddings and overrides */
	      /*   X7. With each PDF, determine the matching embedding or
	         override code. */
              for (i = RL_LEN (pp); i; i--)
                {
                  if (stack_size && status_stack[stack_size-1].isolate != 0)
                    break;
                  POP_STATUS;
                }
	    }

	  /* X9. Remove all RLE, LRE, RLO, LRO, PDF, and BN codes. */
	  /* Remove element and add it to explicits_list */
	  RL_LEVEL (pp) = FRIBIDI_SENTINEL;
	  temp_link.next = pp->next;
	  move_node_before (pp, explicits_list);
	  pp = &temp_link;
	}
      else if (this_type == FRIBIDI_TYPE_PDI)
        /* X6a. pop the direction of the stack */
        {
          for (i = RL_LEN (pp); i; i--)
            {
              if (isolate_overflow > 0)
                {
                  isolate_overflow--;
                  RL_LEVEL (pp) = level;
                }

              else if (valid_isolate_count > 0)
                {
                  /* Pop away all LRE,RLE,LRO, RLO levels
                     from the stack, as these are implicitly
                     terminated by the PDI */
                  while (stack_size && !status_stack[stack_size-1].isolate)
                    POP_STATUS;
                  over_pushed = 0; /* The PDI resets the overpushed! */
                  POP_STATUS;
                  isolate_level-- ;
                  valid_isolate_count--;
                  RL_LEVEL (pp) = level;
                  RL_ISOLATE_LEVEL (pp) = isolate_level;
                }
              else
                {
                  /* Ignore isolated PDI's by turning them into ON's */
                  RL_TYPE (pp) = FRIBIDI_TYPE_ON;
                  RL_LEVEL (pp) = level;
                }
            }
        }
      else if (FRIBIDI_IS_ISOLATE(this_type))
        {
          /* TBD support RL_LEN > 1 */
          new_override = FRIBIDI_TYPE_ON;
          isolate = 1;
          if (this_type == FRIBIDI_TYPE_LRI)
            new_level = level + 2 - (level%2);
          else if (this_type == FRIBIDI_TYPE_RLI)
            new_level = level + 1 + (level%2);
          else if (this_type == FRIBIDI_TYPE_FSI)
            {
              /* Search for a local strong character until we
                 meet the corresponding PDI or the end of the
                 paragraph */
              FriBidiRun *fsi_pp;
              int isolate_count = 0;
              int fsi_base_level = 0;
              for_run_list (fsi_pp, pp)
                {
                  if (RL_TYPE(fsi_pp) == FRIBIDI_TYPE_PDI)
                    {
                      isolate_count--;
                      if (valid_isolate_count < 0)
                        break;
                    }
                  else if (FRIBIDI_IS_ISOLATE(RL_TYPE(fsi_pp)))
                    isolate_count++;
                  else if (isolate_count==0 && FRIBIDI_IS_LETTER (RL_TYPE (fsi_pp)))
                    {
                      fsi_base_level = FRIBIDI_DIR_TO_LEVEL (RL_TYPE (fsi_pp));
                      break;
                    }
                }

              /* Same behavior like RLI and LRI above */
              if (FRIBIDI_LEVEL_IS_RTL (fsi_base_level))
                new_level = level + 1 + (level%2);
              else
                new_level = level + 2 - (level%2);
            }

	  RL_LEVEL (pp) = level;
          RL_ISOLATE_LEVEL (pp) = isolate_level;
          if (isolate_level < FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL-1)
              isolate_level++;
          base_level_per_iso_level[isolate_level] = new_level;

	  if (!FRIBIDI_IS_NEUTRAL (override))
	    RL_TYPE (pp) = override;

          if (new_level <= FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL)
            {
              valid_isolate_count++;
              PUSH_STATUS;
              level = new_level;
            }
          else
            isolate_overflow += 1;
        }
      else if (this_type == FRIBIDI_TYPE_BS)
	{
	  /* X8. All explicit directional embeddings and overrides are
	     completely terminated at the end of each paragraph. Paragraph
	     separators are not included in the embedding. */
	  break;
	}
      else
	{
	  /* X6. For all types besides RLE, LRE, RLO, LRO, and PDF:
	     a. Set the level of the current character to the current
	     embedding level.
	     b. Whenever the directional override status is not neutral,
	     reset the current character type to the directional override
	     status. */
	  RL_LEVEL (pp) = level;
	  if (!FRIBIDI_IS_NEUTRAL (override))
	    RL_TYPE (pp) = override;
	}
    }

    /* Build the isolate_level connections */
    for_run_list (pp, main_run_list)
    {
      int isolate_level = RL_ISOLATE_LEVEL (pp);
      if (run_per_isolate_level[isolate_level])
        {
          run_per_isolate_level[isolate_level]->next_isolate = pp;
          pp->prev_isolate = run_per_isolate_level[isolate_level];
        }
      run_per_isolate_level[isolate_level] = pp;
    }

    /* Implementing X8. It has no effect on a single paragraph! */
    level = base_level;
    override = FRIBIDI_TYPE_ON;
    stack_size = 0;
    over_pushed = 0;
  }
  /* X10. The remaining rules are applied to each run of characters at the
     same level. For each run, determine the start-of-level-run (sor) and
     end-of-level-run (eor) type, either L or R. This depends on the
     higher of the two levels on either side of the boundary (at the start
     or end of the paragraph, the level of the 'other' run is the base
     embedding level). If the higher level is odd, the type is R, otherwise
     it is L. */
  /* Resolving Implicit Levels can be done out of X10 loop, so only change
     of Resolving Weak Types and Resolving Neutral Types is needed. */

  compact_list (main_run_list);

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_types_re (main_run_list);
      print_bidi_string (bidi_types, len);
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

  /* 4. Resolving weak types. Also calculate the maximum isolate level */
  max_iso_level = 0;
  DBG ("resolving weak types");
  {
    int last_strong_stack[FRIBIDI_BIDI_MAX_RESOLVED_LEVELS];
    FriBidiCharType prev_type_orig;
    fribidi_boolean w4;

    last_strong_stack[0] = base_dir;

    for_run_list (pp, main_run_list)
    {
      register FriBidiCharType prev_type, this_type, next_type;
      FriBidiRun *ppp_prev, *ppp_next;
      int iso_level;

      ppp_prev = get_adjacent_run(pp, false, false);
      ppp_next = get_adjacent_run(pp, true, false);

      this_type = RL_TYPE (pp);
      iso_level = RL_ISOLATE_LEVEL(pp);

      if (iso_level > max_iso_level)
        max_iso_level = iso_level;

      if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
        prev_type = RL_TYPE(ppp_prev);
      else
        prev_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

      if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
        next_type = RL_TYPE(ppp_next);
      else
        next_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

      if (FRIBIDI_IS_STRONG (prev_type))
	last_strong_stack[iso_level] = prev_type;

      /* W1. NSM
         Examine each non-spacing mark (NSM) in the level run, and change the
         type of the NSM to the type of the previous character. If the NSM
         is at the start of the level run, it will get the type of sor. */
      /* Implementation note: it is important that if the previous character
         is not sor, then we should merge this run with the previous,
         because of rules like W5, that we assume all of a sequence of
         adjacent ETs are in one FriBidiRun. */
      if (this_type == FRIBIDI_TYPE_NSM)
	{
          /* New rule in Unicode 6.3 */
          if (FRIBIDI_IS_ISOLATE (RL_TYPE (pp->prev)))
              RL_TYPE(pp) = FRIBIDI_TYPE_ON;

	  if (RL_LEVEL (ppp_prev) == RL_LEVEL (pp))
            {
              if (ppp_prev == pp->prev)
                pp = merge_with_prev (pp);
            }
	  else
	    RL_TYPE (pp) = prev_type;

	  if (prev_type == next_type && RL_LEVEL (pp) == RL_LEVEL (pp->next))
	    {
              if (ppp_next == pp->next)
                pp = merge_with_prev (pp->next);
	    }
	  continue;		/* As we know the next condition cannot be true. */
	}

      /* W2: European numbers. */
      if (this_type == FRIBIDI_TYPE_EN && last_strong_stack[iso_level] == FRIBIDI_TYPE_AL)
	{
	  RL_TYPE (pp) = FRIBIDI_TYPE_AN;

	  /* Resolving dependency of loops for rules W1 and W2, so we
	     can merge them in one loop. */
	  if (next_type == FRIBIDI_TYPE_NSM)
	    RL_TYPE (ppp_next) = FRIBIDI_TYPE_AN;
	}
    }


    last_strong_stack[0] = base_dir;

    /* Resolving dependency of loops for rules W4 and W5, W5 may
       want to prevent W4 to take effect in the next turn, do this
       through "w4". */
    w4 = true;
    /* Resolving dependency of loops for rules W4 and W5 with W7,
       W7 may change an EN to L but it sets the prev_type_orig if needed,
       so W4 and W5 in next turn can still do their works. */
    prev_type_orig = FRIBIDI_TYPE_ON;

    /* Each isolate level has its own memory of the last strong character */
    for_run_list (pp, main_run_list)
    {
      register FriBidiCharType prev_type, this_type, next_type;
      int iso_level;
      FriBidiRun *ppp_prev, *ppp_next;

      this_type = RL_TYPE (pp);
      iso_level = RL_ISOLATE_LEVEL(pp);

      ppp_prev = get_adjacent_run(pp, false, false);
      ppp_next = get_adjacent_run(pp, true, false);

      if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
        prev_type = RL_TYPE(ppp_prev);
      else
        prev_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

      if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
        next_type = RL_TYPE(ppp_next);
      else
        next_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

      if (FRIBIDI_IS_STRONG (prev_type))
	last_strong_stack[iso_level] = prev_type;

      /* W2 ??? */

      /* W3: Change ALs to R. */
      if (this_type == FRIBIDI_TYPE_AL)
	{
	  RL_TYPE (pp) = FRIBIDI_TYPE_RTL;
	  w4 = true;
	  prev_type_orig = FRIBIDI_TYPE_ON;
	  continue;
	}

      /* W4. A single european separator changes to a european number.
         A single common separator between two numbers of the same type
         changes to that type. */
      if (w4
	  && RL_LEN (pp) == 1 && FRIBIDI_IS_ES_OR_CS (this_type)
	  && FRIBIDI_IS_NUMBER (prev_type_orig)
	  && prev_type_orig == next_type
	  && (prev_type_orig == FRIBIDI_TYPE_EN
	      || this_type == FRIBIDI_TYPE_CS))
	{
	  RL_TYPE (pp) = prev_type;
	  this_type = RL_TYPE (pp);
	}
      w4 = true;

      /* W5. A sequence of European terminators adjacent to European
         numbers changes to All European numbers. */
      if (this_type == FRIBIDI_TYPE_ET
	  && (prev_type_orig == FRIBIDI_TYPE_EN
	      || next_type == FRIBIDI_TYPE_EN))
	{
	  RL_TYPE (pp) = FRIBIDI_TYPE_EN;
	  w4 = false;
	  this_type = RL_TYPE (pp);
	}

      /* W6. Otherwise change separators and terminators to other neutral. */
      if (FRIBIDI_IS_NUMBER_SEPARATOR_OR_TERMINATOR (this_type))
	RL_TYPE (pp) = FRIBIDI_TYPE_ON;

      /* W7. Change european numbers to L. */
      if (this_type == FRIBIDI_TYPE_EN && last_strong_stack[iso_level] == FRIBIDI_TYPE_LTR)
	{
	  RL_TYPE (pp) = FRIBIDI_TYPE_LTR;
	  prev_type_orig = (RL_LEVEL (pp) == RL_LEVEL (pp->next) ?
			    FRIBIDI_TYPE_EN : FRIBIDI_TYPE_ON);
	}
      else
	prev_type_orig = PREV_TYPE_OR_SOR (pp->next);
    }
  }

  compact_neutrals (main_run_list);

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

  /* 5. Resolving Neutral Types */

  DBG ("resolving neutral types - N0");
  {
    /*  BD16 - Build list of all pairs*/
    int num_iso_levels = max_iso_level + 1;
    FriBidiPairingNode *pairing_nodes = NULL;
    FriBidiRun *local_bracket_stack[FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL][LOCAL_BRACKET_SIZE];
    FriBidiRun **bracket_stack[FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL];
    int bracket_stack_size[FRIBIDI_BIDI_MAX_EXPLICIT_LEVEL];
    int last_level = RL_LEVEL(main_run_list);
    int last_iso_level = 0;

    memset(bracket_stack, 0, sizeof(bracket_stack[0])*num_iso_levels);
    memset(bracket_stack_size, 0, sizeof(bracket_stack_size[0])*num_iso_levels);

    /* populate the bracket_size. The first LOCAL_BRACKET_SIZE entries
       of the stack are one the stack. Allocate the rest of the entries.
     */
    {
      int iso_level;
      for (iso_level=0; iso_level < LOCAL_BRACKET_SIZE; iso_level++)
        bracket_stack[iso_level] = local_bracket_stack[iso_level];

      for (iso_level=LOCAL_BRACKET_SIZE; iso_level < num_iso_levels; iso_level++)
        bracket_stack[iso_level] = fribidi_malloc (sizeof (bracket_stack[0])
                                                       * FRIBIDI_BIDI_MAX_NESTED_BRACKET_PAIRS);
    }

    /* Build the bd16 pair stack. */
    for_run_list (pp, main_run_list)
      {
        int level = RL_LEVEL(pp);
        int iso_level = RL_ISOLATE_LEVEL(pp);
        FriBidiBracketType brack_prop = RL_BRACKET_TYPE(pp);

        /* Interpret the isolating run sequence as such that they
           end at a change in the level, unless the iso_level has been
           raised. */
        if (level != last_level && last_iso_level == iso_level)
          bracket_stack_size[last_iso_level] = 0;

        if (brack_prop!= FRIBIDI_NO_BRACKET
            && RL_TYPE(pp)==FRIBIDI_TYPE_ON)
          {
            if (FRIBIDI_IS_BRACKET_OPEN(brack_prop))
              {
                if (bracket_stack_size[iso_level]==FRIBIDI_BIDI_MAX_NESTED_BRACKET_PAIRS)
                  break;

                /* push onto the pair stack */
                bracket_stack[iso_level][bracket_stack_size[iso_level]++] = pp;
              }
            else
              {
                int stack_idx = bracket_stack_size[iso_level] - 1;
                while (stack_idx >= 0)
                  {
                    FriBidiBracketType se_brack_prop = RL_BRACKET_TYPE(bracket_stack[iso_level][stack_idx]);
                    if (FRIBIDI_BRACKET_ID(se_brack_prop) == FRIBIDI_BRACKET_ID(brack_prop))
                      {
                        bracket_stack_size[iso_level] = stack_idx;

                        pairing_nodes = pairing_nodes_push(pairing_nodes,
                                                           bracket_stack[iso_level][stack_idx],
                                                           pp);
                        break;
                    }
                    stack_idx--;
                  }
              }
          }
        last_level = level;
        last_iso_level = iso_level;
      }

    /* The list must now be sorted for the next algo to work! */
    sort_pairing_nodes(&pairing_nodes);

# if DEBUG
    if UNLIKELY
    (fribidi_debug_status ())
      {
        print_pairing_nodes (pairing_nodes);
      }
# endif	/* DEBUG */

    /* Start the N0 */
    {
      FriBidiPairingNode *ppairs = pairing_nodes;
      while (ppairs)
        {
          int iso_level = ppairs->open->isolate_level;
          int embedding_level = base_level_per_iso_level[iso_level];

          /* Find matching strong. */
          fribidi_boolean found = false;
          FriBidiRun *ppn;
          for (ppn = ppairs->open; ppn!= ppairs->close; ppn = ppn->next)
            {
              FriBidiCharType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);

              /* Calculate level like in resolve implicit levels below to prevent
                 embedded levels not to match the base_level */
              int this_level = RL_LEVEL (ppn) +
                (FRIBIDI_LEVEL_IS_RTL (RL_LEVEL(ppn)) ^ FRIBIDI_DIR_TO_LEVEL (this_type));

              /* N0b */
              if (FRIBIDI_IS_STRONG (this_type) && this_level == embedding_level)
                {
                  RL_TYPE(ppairs->open) = RL_TYPE(ppairs->close) = this_level%2 ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
                  found = true;
                  break;
                }
            }

          /* N0c */
          /* Search for any strong type preceding and within the bracket pair */
          if (!found)
            {
              /* Search for a preceding strong */
              int prec_strong_level = embedding_level; /* TBDov! Extract from Isolate level in effect */
              int iso_level = RL_ISOLATE_LEVEL(ppairs->open);
              for (ppn = ppairs->open->prev; ppn->type != FRIBIDI_TYPE_SENTINEL; ppn=ppn->prev)
                {
                  FriBidiCharType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);
                  if (FRIBIDI_IS_STRONG (this_type) && RL_ISOLATE_LEVEL(ppn) == iso_level)
                    {
                      prec_strong_level = RL_LEVEL (ppn) +
                        (FRIBIDI_LEVEL_IS_RTL (RL_LEVEL(ppn)) ^ FRIBIDI_DIR_TO_LEVEL (this_type));

                      break;
                    }
                }

              for (ppn = ppairs->open; ppn!= ppairs->close; ppn = ppn->next)
                {
                  FriBidiCharType this_type = RL_TYPE_AN_EN_AS_RTL(ppn);
                  if (FRIBIDI_IS_STRONG (this_type) && RL_ISOLATE_LEVEL(ppn) == iso_level)
                    {
                      /* By constraint this is opposite the embedding direction,
                         since we did not match the N0b rule. We must now
                         compare with the preceding strong to establish whether
                         to apply N0c1 (opposite) or N0c2 embedding */
                      RL_TYPE(ppairs->open) = RL_TYPE(ppairs->close) = prec_strong_level % 2 ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
                      RL_LEVEL(ppairs->open) = RL_LEVEL(ppairs->close) = prec_strong_level;
                      found = true;
                      break;
                    }
                }
            }

          ppairs = ppairs->next;
        }

      free_pairing_nodes(pairing_nodes);

      if (num_iso_levels >= LOCAL_BRACKET_SIZE)
        {
          int i;
          /* Only need to free the non static members */
          for (i=LOCAL_BRACKET_SIZE; i<num_iso_levels; i++)
            fribidi_free(bracket_stack[i]);
        }

      /* Remove the bracket property and re-compact */
      {
        const FriBidiBracketType NoBracket = FRIBIDI_NO_BRACKET;
        for_run_list (pp, main_run_list)
          pp->bracket_type = NoBracket;
        compact_neutrals (main_run_list);
      }
    }

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */
  }

  DBG ("resolving neutral types - N1+N2");
  {
    for_run_list (pp, main_run_list)
    {
      FriBidiCharType prev_type, this_type, next_type;
      FriBidiRun *ppp_prev, *ppp_next;

      ppp_prev = get_adjacent_run(pp, false, false);
      ppp_next = get_adjacent_run(pp, true, false);

      /* "European and Arabic numbers are treated as though they were R"
         FRIBIDI_CHANGE_NUMBER_TO_RTL does this. */
      this_type = FRIBIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE (pp));

      if (RL_LEVEL(ppp_prev) == RL_LEVEL(pp))
        prev_type = FRIBIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE(ppp_prev));
      else
        prev_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_prev), RL_LEVEL(pp)));

      if (RL_LEVEL(ppp_next) == RL_LEVEL(pp))
        next_type = FRIBIDI_CHANGE_NUMBER_TO_RTL (RL_TYPE(ppp_next));
      else
        next_type = FRIBIDI_LEVEL_TO_DIR(MAX(RL_LEVEL(ppp_next), RL_LEVEL(pp)));

      if (FRIBIDI_IS_NEUTRAL (this_type))
	RL_TYPE (pp) = (prev_type == next_type) ?
	  /* N1. */ prev_type :
	  /* N2. */ FRIBIDI_EMBEDDING_DIRECTION (pp);
    }
  }

  compact_list (main_run_list);

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

  /* 6. Resolving implicit levels */
  DBG ("resolving implicit levels");
  {
    max_level = base_level;

    for_run_list (pp, main_run_list)
    {
      FriBidiCharType this_type;
      int level;

      this_type = RL_TYPE (pp);
      level = RL_LEVEL (pp);

      /* I1. Even */
      /* I2. Odd */
      if (FRIBIDI_IS_NUMBER (this_type))
	RL_LEVEL (pp) = (level + 2) & ~1;
      else
	RL_LEVEL (pp) =
	  level +
	  (FRIBIDI_LEVEL_IS_RTL (level) ^ FRIBIDI_DIR_TO_LEVEL (this_type));

      if (RL_LEVEL (pp) > max_level)
	max_level = RL_LEVEL (pp);
    }
  }

  compact_list (main_run_list);

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_bidi_string (bidi_types, len);
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

/* Reinsert the explicit codes & BN's that are already removed, from the
   explicits_list to main_run_list. */
  DBG ("reinserting explicit codes");
  if UNLIKELY
    (explicits_list->next != explicits_list)
    {
      register FriBidiRun *p;
      register fribidi_boolean stat =
	shadow_run_list (main_run_list, explicits_list, true);
      explicits_list = NULL;
      if UNLIKELY
	(!stat) goto out;

      /* Set level of inserted explicit chars to that of their previous
       * char, such that they do not affect reordering. */
      p = main_run_list->next;
      if (p != main_run_list && p->level == FRIBIDI_SENTINEL)
	p->level = base_level;
      for_run_list (p, main_run_list) if (p->level == FRIBIDI_SENTINEL)
	p->level = p->prev->level;
    }

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_types_re (main_run_list);
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

  DBG ("reset the embedding levels, 1, 2, 3.");
  {
    register int j, state, pos;
    register FriBidiCharType char_type;
    register FriBidiRun *p, *q, *list;

    /* L1. Reset the embedding levels of some chars:
       1. segment separators,
       2. paragraph separators,
       3. any sequence of whitespace characters preceding a segment
          separator or paragraph separator, and
       4. any sequence of whitespace characters and/or isolate formatting
          characters at the end of the line.
       ... (to be continued in fribidi_reorder_line()). */
    list = new_run_list ();
    if UNLIKELY
      (!list) goto out;
    q = list;
    state = 1;
    pos = len - 1;
    for (j = len - 1; j >= -1; j--)
      {
	/* close up the open link at the end */
	if (j >= 0)
	  char_type = bidi_types[j];
	else
	  char_type = FRIBIDI_TYPE_ON;
	if (!state && FRIBIDI_IS_SEPARATOR (char_type))
	  {
	    state = 1;
	    pos = j;
	  }
	else if (state &&
                 !(FRIBIDI_IS_EXPLICIT_OR_SEPARATOR_OR_BN_OR_WS(char_type)
                   || FRIBIDI_IS_ISOLATE(char_type)))
	  {
	    state = 0;
	    p = new_run ();
	    if UNLIKELY
	      (!p)
	      {
		free_run_list (list);
		goto out;
	      }
	    p->pos = j + 1;
	    p->len = pos - j;
	    p->type = base_dir;
	    p->level = base_level;
	    move_node_before (p, q);
	    q = p;
	  }
      }
    if UNLIKELY
      (!shadow_run_list (main_run_list, list, false)) goto out;
  }

# if DEBUG
  if UNLIKELY
    (fribidi_debug_status ())
    {
      print_types_re (main_run_list);
      print_resolved_levels (main_run_list);
      print_resolved_types (main_run_list);
    }
# endif	/* DEBUG */

  {
    FriBidiStrIndex pos = 0;
    for_run_list (pp, main_run_list)
    {
      register FriBidiStrIndex l;
      register FriBidiLevel level = pp->level;
      for (l = pp->len; l; l--)
	embedding_levels[pos++] = level;
    }
  }

  status = true;

out:
  DBG ("leaving fribidi_get_par_embedding_levels");

  if (main_run_list)
    free_run_list (main_run_list);
  if UNLIKELY
    (explicits_list) free_run_list (explicits_list);

  return status ? max_level + 1 : 0;
}