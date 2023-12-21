#ifndef CONFIG_STACK
#define CONFIG_STACK

typedef size_t Elem_t;

#define output_id "%lu"

const Elem_t POISON = 1e6;
//#define logs_file "logs.txt" // -- an example of file define
#define PROTECT // protection activation

#endif
