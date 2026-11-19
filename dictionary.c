#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Dictionary *node_create(char letter)
{
    Dictionary *node = calloc(1, sizeof(*node));
    if (node == NULL) {
        return NULL;
    }
    node->letter = letter;
    node->translation[0] = '\0';
    return node;
}

static Dictionary *find_child(Dictionary *parent, char letter)
{
    Dictionary *cur = parent->next;
    while (cur != NULL) {
        if (cur->letter == letter) {
            return cur;
        }
        cur = cur->sibling;
    }
    return NULL;
}

static const Dictionary *find_child_const(const Dictionary *parent, char letter)
{
    const Dictionary *cur = parent->next;
    while (cur != NULL) {
        if (cur->letter == letter) {
            return cur;
        }
        cur = cur->sibling;
    }
    return NULL;
}

static Dictionary *find_or_create_child(Dictionary *parent, char letter)
{
    Dictionary *cur = parent->next;
    Dictionary *last = NULL;

    while (cur != NULL) {
        if (cur->letter == letter) {
            return cur;
        }
        last = cur;
        cur = cur->sibling;
    }

    Dictionary *created = node_create(letter);
    if (created == NULL) {
        return NULL;
    }

    if (last == NULL) {
        parent->next = created;
    } else {
        last->sibling = created;
    }
    return created;
}

static void unlink_child(Dictionary *parent, Dictionary *child)
{
    if (parent->next == child) {
        parent->next = child->sibling;
    } else {
        Dictionary *cur = parent->next;
        while (cur != NULL && cur->sibling != child) {
            cur = cur->sibling;
        }
        if (cur != NULL) {
            cur->sibling = child->sibling;
        }
    }
    child->sibling = NULL;
    child->next = NULL;
    free(child);
}

static int is_word_end(const Dictionary *node)
{
    return node != NULL && node->translation[0] != '\0';
}

static int node_is_unused(const Dictionary *node)
{
    return node != NULL && !is_word_end(node) && node->next == NULL;
}

Dictionary *dict_create(void)
{
    /* Dummy root: its `next` list holds the first letters of all words. */
    return node_create('\0');
}

void dict_destroy(Dictionary *root)
{
    if (root == NULL) {
        return;
    }
    dict_destroy(root->next);
    dict_destroy(root->sibling);
    free(root);
}

int dict_add(Dictionary *root, const char *word, const char *translation)
{
    if (root == NULL || word == NULL || word[0] == '\0' || translation == NULL ||
        translation[0] == '\0') {
        return 0;
    }
    if (strlen(translation) >= TRANSLATION_MAX) {
        return 0;
    }

    Dictionary *node = root;
    for (size_t i = 0; word[i] != '\0'; i++) {
        node = find_or_create_child(node, word[i]);
        if (node == NULL) {
            return 0;
        }
    }

    strncpy(node->translation, translation, TRANSLATION_MAX - 1);
    node->translation[TRANSLATION_MAX - 1] = '\0';
    return 1;
}

const char *dict_search(const Dictionary *root, const char *word)
{
    if (root == NULL || word == NULL || word[0] == '\0') {
        return NULL;
    }

    const Dictionary *node = root;
    for (size_t i = 0; word[i] != '\0'; i++) {
        node = find_child_const(node, word[i]);
        if (node == NULL) {
            return NULL;
        }
    }

    if (!is_word_end(node)) {
        return NULL;
    }
    return node->translation;
}

static int remove_from(Dictionary *parent, const char *word)
{
    if (parent == NULL || word[0] == '\0') {
        return 0;
    }

    Dictionary *child = find_child(parent, word[0]);
    if (child == NULL) {
        return 0;
    }

    if (word[1] == '\0') {
        if (!is_word_end(child)) {
            return 0;
        }
        child->translation[0] = '\0';
        if (node_is_unused(child)) {
            unlink_child(parent, child);
        }
        return 1;
    }

    if (!remove_from(child, word + 1)) {
        return 0;
    }

    if (node_is_unused(child)) {
        unlink_child(parent, child);
    }
    return 1;
}

int dict_remove(Dictionary *root, const char *word)
{
    if (root == NULL || word == NULL || word[0] == '\0') {
        return 0;
    }
    return remove_from(root, word);
}
