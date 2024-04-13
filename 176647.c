  void Inspect::operator()(List_Ptr list)
  {
    if (list->empty() && (output_style() == TO_SASS || list->is_bracketed())) {
      append_string(lbracket(list));
      append_string(rbracket(list));
      return;
    }
    std::string sep(list->separator() == SASS_SPACE ? " " : ",");
    if ((output_style() != COMPRESSED) && sep == ",") sep += " ";
    else if (in_media_block && sep != " ") sep += " "; // verified
    if (list->empty()) return;
    bool items_output = false;

    bool was_space_array = in_space_array;
    bool was_comma_array = in_comma_array;
    // if the list is bracketed, always include the left bracket
    if (list->is_bracketed()) {
      append_string(lbracket(list));
    }
    // probably ruby sass eqivalent of element_needs_parens
    else if (output_style() == TO_SASS &&
        list->length() == 1 &&
        !list->from_selector() &&
        !Cast<List>(list->at(0)) &&
        !Cast<Selector_List>(list->at(0))
    ) {
      append_string(lbracket(list));
    }
    else if (!in_declaration && (list->separator() == SASS_HASH ||
        (list->separator() == SASS_SPACE && in_space_array) ||
        (list->separator() == SASS_COMMA && in_comma_array)
    )) {
      append_string(lbracket(list));
    }

    if (list->separator() == SASS_SPACE) in_space_array = true;
    else if (list->separator() == SASS_COMMA) in_comma_array = true;

    for (size_t i = 0, L = list->size(); i < L; ++i) {
      if (list->separator() == SASS_HASH)
      { sep[0] = i % 2 ? ':' : ','; }
      Expression_Obj list_item = list->at(i);
      if (output_style() != TO_SASS) {
        if (list_item->is_invisible()) {
          // this fixes an issue with "" in a list
          if (!Cast<String_Constant>(list_item)) {
            continue;
          }
        }
      }
      if (items_output) {
        append_string(sep);
      }
      if (items_output && sep != " ")
        append_optional_space();
      list_item->perform(this);
      items_output = true;
    }

    in_comma_array = was_comma_array;
    in_space_array = was_space_array;

    // if the list is bracketed, always include the right bracket
    if (list->is_bracketed()) {
      if (list->separator() == SASS_COMMA && list->size() == 1) {
        append_string(",");
      }
      append_string(rbracket(list));
    }
    // probably ruby sass eqivalent of element_needs_parens
    else if (output_style() == TO_SASS &&
        list->length() == 1 &&
        !list->from_selector() &&
        !Cast<List>(list->at(0)) &&
        !Cast<Selector_List>(list->at(0))
    ) {
      append_string(",");
      append_string(rbracket(list));
    }
    else if (!in_declaration && (list->separator() == SASS_HASH ||
        (list->separator() == SASS_SPACE && in_space_array) ||
        (list->separator() == SASS_COMMA && in_comma_array)
    )) {
      append_string(rbracket(list));
    }

  }