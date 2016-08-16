
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#if ! defined(TP_UTILS_H_)
#define TP_UTILS_H_

void tp_put_log(CHAR16* fmt, ...);
void tp_error_print(CHAR16* fmt, ...);
EFI_FILE* open_print_info(CHAR16* path, EFI_HANDLE device_handle);
void close_print_info(void);

#endif

