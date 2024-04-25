#include "../../MyLibraries/headers/my_vector.h"
#include "lex_analysis.h"
#include "parse.h"
#include "dump.h"
#include <assert.h>
#include "../../MyLibraries/headers/systemdata.h"
#include "../../MyLibraries/headers/file_func.h"

void dumpErrors(StringParseData *data, Vector *tokens, Vector *names_table) {

    assert(data);
    assert(tokens);
    assert(names_table);

    for (size_t i = data->position > 3 ? data->position - 4 : 0; i < tokens->size && i < data->position + 4; i++) {
            printf(BRIGHT_GREEN "%5lu) " END_OF_COLOR, i);
            switch (getTokenPtr(data, i)->type) {
                case (PUNCT_SYM): {
                    printf("%s\t\t[punctuation]", PUNCT_SYMBOLS[getTokenPtr(data, i)->sym_code - NEW_LINE]);
                    break;
                }
                case (BINARY_OP): {
                    printf("%s\t\t[binary_op]", KEY_WORDS[getTokenPtr(data, i)->bin_op]);
                    break;
                }
                case (UNARY_OP): {
                    printf("%s\t\t[unary_op]", KEY_WORDS[getTokenPtr(data, i)->un_op]);
                    break;
                }
                case (KEY_OP): {
                    printf("%s\t\t[key_op]", KEY_WORDS[getTokenPtr(data, i)->bin_op]);
                    break;
                }
                case (NUMBER): {
                    printf("%lg\t\t[number]", getTokenPtr(data, i)->number);
                    break;
                }
                case (VARIABLE): {
                    printf("%s\t\t[variable]", getStrPtr(names_table, getTokenPtr(data, i)->var_index));
                    break;
                }
                case (FUNCTION): {
                    if (getTokenPtr(data, i)->func_index == 0)
                        printf("main\t\t[function]");
                    else
                        printf("%s\t\t[function]", getStrPtr(names_table, getTokenPtr(data, i)->func_index));
                    break;
                }
                case (STRING): {
                    printf("\"%s\t\t[string]\" ", getTokenPtr(data, i)->string);
                    break;
                }
                default: {
                    printf(RED "print error: " END_OF_COLOR "invalid value type\n");
                    return;
                }
            }
            printf("\n");
        }
        printf("\n");
}

void dumpAll(Vector *tokens, Vector *names_table, const char *filename) {

    assert(tokens);
    assert(names_table);
    assert(filename);

    FILE *fn = openFile(filename, WRITE);
    if (!fn) return;

    for (size_t i = 0; i < tokens->size; i++) {
        fprintf(fn, "%5lu) ", i);
        switch (((Token *) getPtr(tokens, i))->type) {
            case (PUNCT_SYM): {
                fprintf(fn, "%s\t\t[punctuation]", PUNCT_SYMBOLS[((Token *) getPtr(tokens, i))->sym_code - NEW_LINE]);
                break;
            }
            case (BINARY_OP): {
                fprintf(fn, "%s\t\t[binary_op]", KEY_WORDS[((Token *) getPtr(tokens, i))->bin_op]);
                break;
            }
            case (UNARY_OP): {
                fprintf(fn, "%s\t\t[unary_op]", KEY_WORDS[((Token *) getPtr(tokens, i))->un_op]);
                break;
            }
            case (KEY_OP): {
                fprintf(fn, "%s\t\t[key_op]", KEY_WORDS[((Token *) getPtr(tokens, i))->bin_op]);
                break;
            }
            case (NUMBER): {
                fprintf(fn, "%lg\t\t[number]", ((Token *) getPtr(tokens, i))->number);
                break;
            }
            case (VARIABLE): {
                fprintf(fn, "%s\t\t[variable]", getStrPtr(names_table, ((Token *) getPtr(tokens, i))->var_index));
                break;
            }
            case (FUNCTION): {
                if (((Token *) getPtr(tokens, i))->func_index == 0)
                    fprintf(fn, "main\t\t[function]");
                else
                    fprintf(fn, "%s\t\t[function]", getStrPtr(names_table, ((Token *) getPtr(tokens, i))->func_index));
                break;
            }
            case (STRING): {
                fprintf(fn, "\"%s\t\t[string]\" ", ((Token *) getPtr(tokens, i))->string);
                break;
            }
            default: {
                fprintf(fn, RED "print error: " END_OF_COLOR "invalid value type\n");
                return;
            }
        }
        fprintf(fn, "\n");
    }

    closeFile(fn);
}
