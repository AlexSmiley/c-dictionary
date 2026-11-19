#ifndef DICTIONARY_H
#define DICTIONARY_H

#define TRANSLATION_MAX 256

/*
 * One node per letter. `next` points to the following letter of a word.
 * `sibling` points to another letter at the same position (so "map" and
 * "mat" can share "m" -> "a", then branch into "p" and "t").
 * `translation` is filled only on the last letter of a stored word.
 */
typedef struct Dictionary {
    char letter;
    char translation[TRANSLATION_MAX];
    struct Dictionary *next;
    struct Dictionary *sibling;
} Dictionary;

Dictionary *dict_create(void);
void dict_destroy(Dictionary *root);
int dict_add(Dictionary *root, const char *word, const char *translation);
const char *dict_search(const Dictionary *root, const char *word);
int dict_remove(Dictionary *root, const char *word);

#endif
