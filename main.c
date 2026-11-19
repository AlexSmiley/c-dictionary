#include "dictionary.h"

#include <stdio.h>
#include <string.h>

static void print_search(Dictionary *dict, const char *word)
{
    const char *translation = dict_search(dict, word);
    if (translation != NULL) {
        printf("%s -> %s\n", word, translation);
    } else {
        printf("there is no such word: %s\n", word);
    }
}

static void demo(Dictionary *dict)
{
    printf("Adding \"map\" and then \"maps\" (shares m-a-p, then adds s):\n");
    dict_add(dict, "map", "a drawing of an area");
    dict_add(dict, "maps", "plural of map");
    dict_add(dict, "mat", "a small rug");

    print_search(dict, "map");
    print_search(dict, "maps");
    print_search(dict, "mat");
    print_search(dict, "ma");
    print_search(dict, "mapping");

    printf("\nRemoving \"map\" (keeps \"maps\"):\n");
    dict_remove(dict, "map");
    print_search(dict, "map");
    print_search(dict, "maps");
}

int main(void)
{
    Dictionary *dict = dict_create();
    if (dict == NULL) {
        fprintf(stderr, "failed to create dictionary\n");
        return 1;
    }

    demo(dict);

    printf("\nCommands: add WORD TRANSLATION | search WORD | remove WORD | quit\n");

    char line[512];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }

        char command[32];
        char word[256];
        char translation[TRANSLATION_MAX];

        if (sscanf(line, "%31s %255s %255[^\n]", command, word, translation) >= 2) {
            if (strcmp(command, "add") == 0) {
                if (sscanf(line, "%*s %*s %255[^\n]", translation) != 1) {
                    printf("usage: add WORD TRANSLATION\n");
                    continue;
                }
                if (dict_add(dict, word, translation)) {
                    printf("added: %s\n", word);
                } else {
                    printf("could not add: %s\n", word);
                }
            } else if (strcmp(command, "search") == 0) {
                print_search(dict, word);
            } else if (strcmp(command, "remove") == 0) {
                if (dict_remove(dict, word)) {
                    printf("removed: %s\n", word);
                } else {
                    printf("there is no such word: %s\n", word);
                }
            } else if (strcmp(command, "quit") == 0) {
                break;
            } else {
                printf("unknown command\n");
            }
        } else if (sscanf(line, "%31s", command) == 1 && strcmp(command, "quit") == 0) {
            break;
        } else {
            printf("unknown command\n");
        }
    }

    dict_destroy(dict);
    return 0;
}
