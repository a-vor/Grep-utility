#include <stdio.h>
#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#define SMALL_SIZE 4096
#define BIG_SIZE 16384

typedef enum bool { false, true } bool;
typedef struct grep_flags {
  bool flag_e, flag_i, flag_v, flag_c, flag_l;
  bool flag_n, flag_h, flag_s, flag_f, flag_o;
  int cnt_e, cnt_f;
} grep_flags;

void set_false(grep_flags *gf);
void test_out(grep_flags gf, char *s, char *f_files, char *input_files,
              int cnt_files);
void make_upd(int *need_cnt, char need_upd[], char **argv, char symbol, int *i,
              int *j, int n);
void get_flags(grep_flags *gf, int argc, char **argv, char s[], char f_files[]);
void parse_argv(grep_flags gf, int argc, char **argv, char s[],
                char input_files[], int *cnt_files);
void do_lc(grep_flags gf, char *file_name, int founded_lines, bool few_files);
void s21_grep_with_o(FILE *file, char *file_name, grep_flags gf, char *s,
                     bool few_files);
void s21_grep_without_o(FILE *file, char *file_name, grep_flags gf, char *s,
                        bool few_files);
void prepare_s21_grep(grep_flags gf, char *s, char *f_files, char *input_files,
                      int cnt_files);

#endif  // SRC_GREP_S21_GREP_H_