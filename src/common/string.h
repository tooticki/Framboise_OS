#ifndef STRING_H
#define STRING_H

int strcmp_(const char *str1, const char *str2);          // Compare the two strings str1 and str2.

char *strchrs_(char *string, const char *searched_chars); // Return a pointer to the first occurrence of one of the characters
                                                          // from searched_chars in the string s.

char *strtok_(char *string, const char *delimiters);      // Break a string into a sequence of zero or more nonempty tokens
                                                          // On the first call the string to be parsed should be specified in string
                                                          // In each subsequent call that should parse the same string, string must be 0

void read_line(char *buf, unsigned int size);             // Read a line from the terminal, assing this line to buf.

#endif
