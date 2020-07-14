#pragma once

extern int str2hexachar (const char *str, int str_len, char *hexachar, int hexachar_len);
extern int hexachar2str (const char *hexachar, char *str, int str_len);
extern int str2escseq (const char *str, int str_len, char *escseq, int escseq_len);
extern int escseq2str (const char *escseq, char *str, int str_len);
extern int escseq2hexachar (const char *escseq, char *hexachar, int hexachar_len);
extern int hexachar2escseq (const char *hexachar, char *escseq, int escseq_len);

