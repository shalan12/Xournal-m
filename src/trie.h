#ifndef __TRIE__
#define __TRIE__

#include <glib.h>

typedef struct TrieNode
{
	GHashTable* children; // a hashtable from ascii characters to children
	int is_end_state;
} TrieNode ;

typedef struct Trie
{
	TrieNode * root;
	GHashTable* table;

} Trie;

Trie* make_trie(GArray* words, GArray* mappings, int len);
int get_words_mappings_from_file(char* filename, GArray* words, GArray* mappings);
Trie* make_trie_from_pattern_file(char* filename);
TrieNode* make_trie_node();
void insert_at_node(TrieNode* node, char* word);
char* compile_string(Trie* trie, char* string);
void print_trie(Trie* trie);
void print_trie_node(TrieNode* node, char* c);
char* trie_node_look_up(TrieNode* node, char* string, char* out, int idx, int* last_good_idx);

#endif 