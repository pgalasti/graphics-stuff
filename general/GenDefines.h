#ifndef GSTUFF_GENERAL_DEFINES_H
#define GSTUFF_GENERAL_DEFINES_H

#define PROGRAM_DESC(name, desc, ...)            \
    std::cout << "===== " << name << " =====\n"  \
              << "Expectation: " << desc << "\n" \
              __VA_OPT__(<< __VA_ARGS__ << "\n") \
              << std::endl

#endif
