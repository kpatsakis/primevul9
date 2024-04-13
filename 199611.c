    **/
    bool operator==(const char *const expression) const {
      return *this==(+*this)._fill(expression,true,true,0,0,"operator==",this);