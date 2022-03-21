
#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL (2)
#endif


#if DEBUG_LEVEL > 0
  #include <iostream>
  #define LOG(debug_level, msg) {std::cerr << msg << std::endl;}
#else
  #define LOG(debug_level, msg)
#endif
