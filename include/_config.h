#pragma once

#define MAIN_EXPORT __attribute__((visibility("default")))
#define MAIN_LOCAL __attribute__((visibility("hidden")))

// Helper defines for reducing codesize
#ifdef NO_EXCESS_LOGGING
#define NO_SPECIFIC_FAIL_LOGS
#define NO_STAT_DUMPS
#endif
