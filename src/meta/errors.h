#ifndef H_ERRORS
#define H_ERRORS

void die(char* msg);

#define die_on_failure(function) if (function() == EXIT_FAILURE) {die("function()");}

#endif
