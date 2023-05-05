#include "s21_grep.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char **argv) {
  grep_flags gf;
  set_false(&gf);

  char s[SMALL_SIZE] = {0}, f_files[SMALL_SIZE] = {0},
       input_files[BIG_SIZE] = {0};
  get_flags(&gf, argc, argv, s, f_files);

  int cnt_files = 0;
  parse_argv(gf, argc, argv, s, input_files, &cnt_files);

  prepare_s21_grep(gf, s, f_files, input_files, cnt_files);
  return 0;
}

void set_false(grep_flags *gf) {
  gf->flag_e = false;
  gf->flag_i = false;
  gf->flag_v = false;
  gf->flag_c = false;
  gf->flag_l = false;
  gf->flag_n = false;
  gf->flag_h = false;
  gf->flag_s = false;
  gf->flag_f = false;
  gf->flag_o = false;
  gf->cnt_e = 0;
  gf->cnt_f = 0;
}

void make_upd(int *need_cnt, char need_upd[], char **argv, char symbol, int *i,
              int *j, int n) {
  (*need_cnt)++;
  strcat(need_upd, ((*need_cnt) > 1) ? "|" : "");
  if ((*j) != n - 1) {
    strcat(need_upd, (strchr(argv[(*i)], symbol) + 1));
    (*j) = n;
  } else {
    strcat(need_upd, argv[++(*i)]);
  }
}

void get_flags(grep_flags *gf, int argc, char **argv, char s[],
               char f_files[]) {
  bool incorrect_input = false;
  char incorrect_c;
  for (int i = 1; (i < argc && !incorrect_input); ++i) {
    if (argv[i][0] == '-') {
      int cur_sz = (int)strlen(argv[i]);
      for (int j = 1; (j < cur_sz && !incorrect_input); ++j) {
        char c = argv[i][j];
        if (c == 'e') {
          make_upd(&(gf->cnt_e), s, argv, 'e', &i, &j, cur_sz);
          gf->flag_e = 1;
        } else if (c == 'i') {
          gf->flag_i = 1;
        } else if (c == 'v') {
          gf->flag_v = 1;
        } else if (c == 'c') {
          gf->flag_c = 1;
        } else if (c == 'l') {
          gf->flag_l = 1;
        } else if (c == 'n') {
          gf->flag_n = 1;
        } else if (c == 'h') {
          gf->flag_h = 1;
        } else if (c == 's') {
          gf->flag_s = 1;
        } else if (c == 'f') {
          make_upd(&(gf->cnt_f), f_files, argv, 'f', &i, &j, cur_sz);
          gf->flag_f = 1;
        } else if (c == 'o') {
          gf->flag_o = 1;
        } else {
          incorrect_input = true;
          incorrect_c = c;
        }
      }
    }
  }
  if (incorrect_input) {
    fprintf(stderr, "s21_grep: illegal option -- %c", incorrect_c);
    exit(1);
  }
}

void parse_argv(grep_flags gf, int argc, char **argv, char s[],
                char input_files[], int *cnt_files) {
  bool get_temp = false;

  if (gf.cnt_e || gf.cnt_f) get_temp = true;

  for (int i = 1; i < argc; ++i) {
    bool prev_strchr = (argv[i - 1][0] == '-');

    int prev_size = (int)strlen(argv[i - 1]);
    char *get_e = strchr(argv[i - 1], 'e');
    char *get_f = strchr(argv[i - 1], 'f');
    bool check_e = (get_e && ((int)(get_e - argv[i - 1] + 1) == prev_size));
    bool check_f = (get_f && ((int)(get_f - argv[i - 1] + 1) == prev_size));

    prev_strchr &= (check_e || check_f);
    bool isFile = ((gf.cnt_e || gf.cnt_f) ? (!prev_strchr ? 1 : 0) : 1);

    if (argv[i][0] != '-') {
      if (!get_temp) {
        strcat(s, argv[i]);
        get_temp = true;
      } else if (isFile) {
        (*cnt_files)++;
        strcat(input_files, ((*cnt_files) > 1) ? "|" : "");
        strcat(input_files, argv[i]);
      }
    }
  }
}

void do_lc(grep_flags gf, char *file_name, int founded_lines, bool few_files) {
  if (gf.flag_l) {
    if (founded_lines) printf("%s\n", file_name);
  } else if (gf.flag_c) {
    if (few_files && !gf.flag_h) printf("%s:", file_name);
    printf("%d\n", founded_lines);
  }
}

void s21_grep_with_o(FILE *file, char *file_name, grep_flags gf, char *s,
                     bool few_files) {
  int cflags = REG_EXTENDED;

  if (gf.flag_i) cflags |= REG_ICASE;

  const size_t nmatch = 1;
  regmatch_t pmatch[1];
  regex_t reg;

  regcomp(&reg, s, cflags);

  char str[SMALL_SIZE] = {0}, c = '\0', prev_c = '\0';
  int id = 0, line = 0, founded_lines = 0;

  char temp[SMALL_SIZE] = {0};
  int temp_sz = 0;

  do {
    prev_c = c;
    c = fgetc(file);
    if (c == '\n' || c == EOF) {
      line++;

      int status = regexec(&reg, str, nmatch, pmatch, 0);
      bool need_plus = !((status != REG_NOMATCH) ^ (!gf.flag_v));
      need_plus &= (c != EOF || (c == EOF && prev_c != '\n'));
      founded_lines += need_plus;

      int start = -1, finish = -1;
      bool need_while = !(gf.flag_c || gf.flag_l || gf.flag_v);

      while (status != REG_NOMATCH && need_while) {
        start = pmatch[0].rm_so;
        finish = pmatch[0].rm_eo;
        if (few_files && !gf.flag_h) printf("%s:", file_name);
        if (gf.flag_n) printf("%d:", line);
        for (int i = start; i < finish; ++i) printf("%c", str[i]);
        printf("\n");
        for (int i = 0; i < temp_sz; ++i) temp[i] = '\0';
        temp_sz = 0;
        for (int i = finish; i < id; ++i) temp[temp_sz++] = str[i];
        for (int i = 0; i < id; ++i) str[i] = '\0';
        id = temp_sz;
        for (int i = 0; i < id; ++i) str[i] = temp[i];
        status = regexec(&reg, str, nmatch, pmatch, 0);
      }
      for (int i = 0; i < id; ++i) str[i] = '\0';
      id = 0;
    } else {
      str[id++] = c;
    }
  } while (c != EOF);
  do_lc(gf, file_name, founded_lines, few_files);
  regfree(&reg);
}

void s21_grep_without_o(FILE *file, char *file_name, grep_flags gf, char *s,
                        bool few_files) {
  int cflags = REG_EXTENDED;

  if (gf.flag_i) cflags |= REG_ICASE;

  const size_t nmatch = 1;
  regmatch_t pmatch[1];
  regex_t reg;

  regcomp(&reg, s, cflags);

  char str[SMALL_SIZE] = {0}, c = '\0', prev_c = '\0';
  int id = 0, line = 0, founded_lines = 0;

  do {
    prev_c = c;
    c = fgetc(file);
    if (c == '\n' || c == EOF) {
      line++;
      int status = regexec(&reg, str, nmatch, pmatch, 0);
      bool need_out = !((status != REG_NOMATCH) ^ (!gf.flag_v));
      need_out &= (c != EOF || (c == EOF && prev_c != '\n'));
      founded_lines += need_out;
      need_out &= !(gf.flag_c || gf.flag_l);
      if (need_out) {
        if (few_files && !gf.flag_h) printf("%s:", file_name);
        if (gf.flag_n) printf("%d:", line);
        printf("%s\n", str);
        // printf((c != EOF) ? "\n" : (prev_c != '\n' ? "\n" : ""));
      }
      for (int i = 0; i < id; ++i) str[i] = '\0';
      id = 0;
    } else {
      str[id++] = c;
    }
  } while (c != EOF);
  do_lc(gf, file_name, founded_lines, few_files);
  regfree(&reg);
}

void prepare_s21_grep(grep_flags gf, char *s, char *f_files, char *input_files,
                      int cnt_files) {
  bool few_files = (cnt_files > 1);

  FILE *file;
  char *istr;

  bool f_error = false;

  if (gf.flag_f) {
    istr = strtok(f_files, "|");

    int n = (int)strlen(s);

    while (istr != NULL && !f_error) {
      file = fopen(istr, "r");
      if (file) {
        if (n) s[n++] = '|';
        char c = fgetc(file);
        while (c != EOF) {
          s[n++] = (c != '\n' ? c : '|');
          c = fgetc(file);
        }
        fclose(file);
      } else {
        fprintf(stderr, "grep: %s: No such file or directory\n", istr);
        f_error = true;
      }
      istr = strtok(NULL, "|");
    }
  }

  istr = strtok(input_files, "|");

  while (istr != NULL && !f_error) {
    file = fopen(istr, "r");
    if (file) {
      if (gf.flag_o)
        s21_grep_with_o(file, istr, gf, s, few_files);
      else
        s21_grep_without_o(file, istr, gf, s, few_files);
      fclose(file);
    } else if (!gf.flag_s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", istr);
    }
    istr = strtok(NULL, "|");
  }
}