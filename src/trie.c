#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

const int MAX_PATTERN_LEN = 10;

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


Trie* make_trie(char** words, int num_words) {
	Trie* trie = malloc(sizeof(Trie));
	trie->root = make_trie_node();
	
	for (int i = 0; i < num_words; i++) {
		insert_at_node(trie->root, words[i]);
	}
	return trie;
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
	char* next;
	char* compiled = malloc(strlen(string));
	compiled[0] = '\0';
	while (1) {
		tmp[0] = '\0';
		next = trie_node_look_up(trie->root, string, tmp);
		if (tmp[0] != '\0') {
			//printf("%s\n", tmp);
			strcat(compiled, g_hash_table_lookup(trie->table, tmp));
		}
		if (next[0] == '\0') break;
		else {
			if (string == next) next += 1;
			if (tmp[0] == '\0') strncat(compiled, string, next-string);
			string = next;
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
char* trie_node_look_up(TrieNode* node, char* string, char* out) {
	if (node->is_end_state) return string; 
	else { // if it's the NULL character, we won't find a next node and return NULL
		char c[1];
		c[0] = *string;
		TrieNode* next = g_hash_table_lookup(node->children, c); 
		if (next) return trie_node_look_up(next, string + 1, strcat(out, c));
		else {
			out[0] = '\0';
			return string;
		}
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

Trie* make_trie_from_pattern_file(char* filename) {
	Trie* trie = malloc(sizeof(Trie));
	trie->root = make_trie_node();
	trie->table = g_hash_table_new(g_str_hash, g_str_equal);

	FILE* fp = fopen(filename, "r");
	char pattern[MAX_PATTERN_LEN];
	char mapping[MAX_PATTERN_LEN];
	int i = 0;
	char* h_pattern;
	char* h_mapping;
	while(!feof(fp)) {
		i++;
		fscanf(fp, "%s", pattern);
		fscanf(fp, "%s\n", mapping);
		h_pattern = malloc(sizeof(char) * strlen(pattern));
		h_mapping = malloc(sizeof(char) * strlen(mapping));
		g_hash_table_insert(trie->table, strcpy(h_pattern, pattern), strcpy(h_mapping, mapping));
		insert_at_node(trie->root, pattern);
	}

	return trie;
}