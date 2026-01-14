#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t-n - create new database file\n");
  printf("\t-f - (required) path to database file\n");
  printf("\t-a - add an employee in the form of \"Timmy H.,123 Sheshire Ln.,120\"\n");
  printf("\t-l - list employees\n");

  return;
}

int main(int argc, char *argv[]) {
  char *filepath = NULL;
  bool newfile = false;
  bool list = false;
	int c;
  int dbfd = -1;
  struct dbheader_t *header = NULL;
  struct employee_t *employees = NULL;
  char *addstring = NULL;

  while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
    switch(c) {
    case 'f':
      filepath = optarg;
      break;
    case 'n':
      newfile = true;
      break;
    case 'a':
      addstring = optarg;
      break;
    case 'l':
      list = true;
      break;
    case '?':
      printf("Unknown option -%c\n", c);
      break;
    default:
      return -1;
    }
  }

  if(filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return 0;
  }

  if(newfile) {
    dbfd = create_db_file(filepath);
    if(dbfd == STATUS_ERROR) {
      printf("Unable to create database file\n");
      return -1;
    }

    if(create_db_header(&header) == STATUS_ERROR) {
      printf("Failed to create database header.\n");
      return -1;
    }
  } else {
    dbfd = open_db_file(filepath);
    if(dbfd == STATUS_ERROR) {
      printf("Unable to open database file\n");
      return -1;
    }

    if(validate_db_header(dbfd, &header) == STATUS_ERROR) {
      printf("Failed to validate database header.\n");
      return -1;
    }
  }

  printf("Newfile: %d\n", newfile);
  printf("Filepath: %s\n", filepath);

  if(read_employees(dbfd, header, &employees) != STATUS_SUCCESS) {
     printf("Failed to load employees\n");
     return 0;
   }

  if(addstring) {
    if(add_employee(header, &employees, addstring) == STATUS_ERROR) {
      printf("Failed to add employee |%s|", addstring);
      return STATUS_ERROR;
    }
  }

  if(list) {
    list_employees(header, employees);
  }

  output_file(dbfd, header, employees);
  close(dbfd);

  return STATUS_SUCCESS;
}
