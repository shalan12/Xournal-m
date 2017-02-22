#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

const int MAX_PATTERN_LEN = 30;

char * get_ascii_characters(char* unicode_str) 
{
	long numChars = g_utf8_strlen(unicode_str, -1); // number of unicode characters in the string
	char * ascii_part = malloc(sizeof(char) * (numChars + 1));
	
	char * iter = unicode_str; // to iterate over the string
	int sizeChar; // to store the number of bytes taken by each unicode character
	int idx = 0;

	for (int i = 0; i < numChars; i++) {
		sizeChar = g_utf8_next_char(iter) - iter; // get the number of bytes taken by the character
		if (sizeChar == 1) ascii_part[idx++] = *iter; // copy ascii character
		iter += sizeChar; // move over to the next character
	}

	ascii_part[idx] = '\0';
	return ascii_part;
}

TrieNode* make_trie_node() {
	TrieNode* node = malloc(sizeof(TrieNode));
	node->children = g_hash_table_new(g_str_hash, g_str_equal);
	node->is_end_state = 0;
	return node;
}

void insert_at_node(TrieNode* node, char* word) {
	if (*word == '\0') node->is_end_state = 1;
	else {
		char * c = malloc(sizeof(char) * 2); // have to put these on heap, because the hash_table doesn't make a copy
		c[0] = word[0];
		c[1] = '\0';
		TrieNode* next = g_hash_table_lookup(node->children, c);
		if (next == NULL) {
			next = make_trie_node();
			g_hash_table_insert(node->children, c, next);
		}
		insert_at_node(next, word + 1);
	}
}

char* compile_string(Trie* trie, char* string) {
	char tmp[MAX_PATTERN_LEN];
	char* curr = string; // copy pointer
	char* next;
	char* compiled = malloc(sizeof(char) * strlen(string) * MAX_PATTERN_LEN);
	compiled[0] = '\0';
	int last_good_idx;
	char matched[MAX_PATTERN_LEN];

	while (1) {
		last_good_idx = 0;
		tmp[0] = '\0';
		matched[0] = '\0';
		next = trie_node_look_up(trie->root, curr, tmp, 0, &last_good_idx);
		strncat(matched, tmp, last_good_idx);

		if (matched[0] != '\0') {
			//printf("matched : %s\n", matched);
			strcat(compiled, g_hash_table_lookup(trie->table, matched));
		}

		if (next[0] == '\0') break;
		else {
			if (curr == next) next += 1;
			if (matched[0] == '\0') strncat(compiled, curr, next-curr);
			else strcat(compiled, matched+last_good_idx);
			curr = next;
		}
		
	}
	return compiled;
}

/*
/ @param string : a null terminated string to search for patterns
/ @param out : the pattern found
/ @return a pointer pointing to the first character that hasn't been read 
/		  or NULL if reached end of string without matching anything
/
/ reads a string until a pattern has been matched or the string has terminated 
*/
char* trie_node_look_up(TrieNode* node, char* string, char* out, int idx, int* last_good_idx) {
	if (node->is_end_state) *last_good_idx = idx; 
	char c[1];
	c[0] = *string;
	TrieNode* next = g_hash_table_lookup(node->children, c); 
	if (next) return trie_node_look_up(next, string + 1, strcat(out, c), idx+1, last_good_idx);
	else {
		return string;
	}
}

void print_trie(Trie* trie) {
	char temp[MAX_PATTERN_LEN];
	temp[0] = '\0';
	print_trie_node(trie->root, temp);

}

void print_trie_node(TrieNode* node, char* c) {
	if (node->is_end_state) printf("%s\n", c);
	else
	{
		GList* key = g_hash_table_get_keys(node->children);
		while (key) {
			char temp[MAX_PATTERN_LEN];
			print_trie_node(g_hash_table_lookup(node->children, key->data), strcat(strcpy(temp, c), key->data));
			key = key->next;
		}
	}
}

Trie* make_trie(GArray* words, GArray* mappings, int len) {
	Trie* trie = malloc(sizeof(Trie));
	trie->root = make_trie_node();
	trie->table = g_hash_table_new(g_str_hash, g_str_equal);
	char* word;

	for (int i = 0; i < len; i++) {
		word = g_array_index(words,char*,i);
		g_hash_table_insert(trie->table, word, g_array_index(mappings, char*, i));
		insert_at_node(trie->root, word);
	}

	return trie;
}

// returns the length of the arrays
int get_words_mappings_from_file(char* filename, GArray* words, GArray* mappings) {
	
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) printf("Couldn't open patterns file\n");
	char* word;
	char* mapping;
	int i = 0;
	
	while(!feof(fp)) {
		i++;
		word = malloc(sizeof(char) * MAX_PATTERN_LEN);
		mapping = malloc(sizeof(char) * MAX_PATTERN_LEN);
		fscanf(fp, "%s", word);
		fscanf(fp, "%s\n", mapping);
		g_array_append_val(words, word);
		g_array_append_val(mappings, mapping);
	}

	return i;
}