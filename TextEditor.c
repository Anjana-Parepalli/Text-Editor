/**
 * Design a suitable data structure for a text editor which includes operations such as 
 * inserting a line after the enter key is pressed, deleting a line, and searching for 
 * a word in a line. When the key F2 is pressed, the contents of the buffer should be 
 * saved in a file and displayed on the screen.
 * 
 * (Hint: an array of pointers to strings can be used as a data structure)
 * 
 * @author : Anjana Parepalli (anjanaparepalli@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Searches for a word in a string and returns the index of its first occurence
// Uses the Knutt-Morris-Pratt pattern searching algorithm
int WordSearch(char *line, char *word) {
	// Store the lengths of the line and word strings
	int line_length = strlen(line);
	int word_length = strlen(word);

	// Input validation : a pattern longer than the text cannot be present in the text
	if (word_length > line_length) return -1;

	// Edge case : empty pattern
	if (word_length == 0) return 0;

	// Edge case : pattern and text are the same size
	if (word_length == line_length) {
		// Check if the pattern and text are the same
		if (strcmp(line, word) == 0) return 0;
		else return -1;
	}

	// Construct the Longest Prefix-Suffix array
	int *LPS = (int *) calloc(word_length, sizeof(int));

	// Use two pointers to keep track of the prefixes and suffixes of the word (pattern)
	int prefixPointer = 0;
	int suffixPointer = 1;
	
	while (suffixPointer < word_length) {
		// Check if the prefix and suffix characters match
		if (word[suffixPointer] == word[prefixPointer]) {
			// Advance prefix pointer
			prefixPointer++;

			// Can skip these many characters from prefix pointer is encountered
			LPS[suffixPointer] = prefixPointer;

			// Advance suffix pointer
			suffixPointer++;
		} else {
			if (prefixPointer != 0) {
				// Set prefix pointer back (may be able to skip some characters)
				prefixPointer = LPS[prefixPointer - 1];
			} else {
				// Advance suffix pointer
				suffixPointer++;
			}
		}
	}

	// Use two pointers to keep track of the word (pattern) and line (text) strings
	int wordPointer = 0;
	int linePointer = 0;

	while (linePointer < line_length) {
		// Check if the current word character matches the current line character
		if (word[wordPointer] == line[linePointer]) {
			// Advance word and line pointers
			wordPointer++;
			linePointer++;

			// Check if the word was found (pattern completely matches a substring of the text)
			if (wordPointer == word_length) {
				// Free LPS table from heap memory
				free(LPS);

				// Return the index of the first match
				return linePointer - wordPointer;
			}
		} else {
			if (wordPointer != 0) {
				// Set word pointer back
				// We can skip the number of characters that we already know matches the line characters
				wordPointer = LPS[wordPointer - 1];
			} else {
				// Advance line pointer
				linePointer++;
			}
		}
	}
	// Free LPS table from heap memory
	free(LPS);
	
	// Return -1 if no match was found
	return -1;
}

// Frees a buffer of strings from heap memory
void FreeBuffer(char **buffer, int max_strings) {
	for (int i = 0; i < max_strings; i++) {
		free(buffer[i]);
	}
	free(buffer);
}

// Driver code
int main() {
	// Get dimensions of text buffer
	int max_length, max_strings;
	printf("\nEnter the maximum length of a string: ");
	scanf(" %d", &max_length);
	printf("Enter the maximum number of strings the buffer can hold: ");
	scanf(" %d", &max_strings);
	getchar();

	// Create a buffer data structure
	// Array of strings to hold text lines entered by the user
	char **buffer = (char **) malloc(max_strings * sizeof(char *));
	for (int i = 0; i < max_strings; i++) {
		buffer[i] = (char *) malloc(max_length * sizeof(char));

		if (buffer[i] == NULL) {
			printf("Memory allocation failed.\n");
			exit(1);
		}
	}

	// Dynamic menu
	int num_strings = 0;
	int choice;
	printf("\n\n\nSTARTING INTERACTIVE MENU...\n");
	do {
		printf("\nChoose an option from below...\n\n");
		printf("1. Insert text line into buffer\n");
		printf("2. Delete text line from buffer\n");
		printf("3. Search word in text line\n");
		printf("4. Push buffer contents to file\n");
		printf("5. Read from file\n");
		printf("6. Quit program\n");

		printf("\nEnter choice: ");
		scanf(" %d", &choice);
		getchar();
		printf("\n");

		char text[max_length];
		int line_num;
		char word[max_length];

		// File pointer
		FILE *fptr;

		switch (choice) {
			case 1:
			// Increment the count of strings stored in the buffer
			num_strings++;

			// Check if the buffer is full
			if (num_strings > max_strings) {
				printf("\nBuffer overflow error : maximum number of lines reached.");
				num_strings--;
				break;
			}

			// Get a string of text from the user
			printf("Enter text: ");
			if (!(fgets(text, max_length, stdin))) {
				printf("\nInput error.\n");
				break;
			}

			// Remove trailing newline character
			text[strcspn(text, "\n")] = '\0';

			// Insert the text into the current buffer line
			strcpy(buffer[num_strings - 1], text);

			// Print success message
			printf("\nText entered successfully!");

			break;

			case 2:
			// Check if the buffer is empty
			if (num_strings == 0) {
				printf("\nBuffer underflow error : cannot delete from empty buffer.");
				break;
			}

			// Get the line number to be deleted
			printf("Enter line number to delete: ");
			scanf(" %d", &line_num);

			// Validate the line number
			if (line_num > num_strings || line_num < 0) {
				printf("\nNothing stored at line %d.\n", line_num);
				break;
			}

			// Shift all text strings after that line by one, to rewrite the line to be deleted
			for (int i = line_num; i < num_strings; i++) {
				strcpy(buffer[i - 1], buffer[i]);
			}

			// Delete the last text string
			free(buffer[num_strings - 1]);
			buffer[num_strings - 1] = (char *) malloc(max_length * sizeof(char));

			// Decrement the count of strings stored in the buffer
			num_strings--;

			// Print success message
			printf("\nLine %d deleted successfully!", line_num);

			break;

			case 3:
			// Check if the buffer is empty
			if (num_strings == 0) {
				printf("\nBuffer is empty.");
				break;
			}

			// Get the line number to search in
			printf("Enter line number to search in: ");
			scanf(" %d", &line_num);
			getchar();

			// Get the word to search for
			printf("Enter word to search for: ");
			if (!(fgets(word, max_length, stdin))) {
				printf("\nInput error.\n");
				break;
			}

			// Remove trailing newline character
			word[strcspn(word, "\n")] = '\0';

			// Commence word search
			int res = WordSearch(buffer[line_num - 1], word);

			// Display search result
			if (res == -1) {
				printf("\nWord was not found in the given line.");
			} else {
				printf("\nWord was found starting at position %d in the given line.", res + 1);
			}

			break;

			case 4:
			// Open the file in write mode, to overwrite previous contents
			fptr = fopen("TextFile.txt", "w");

			// Check if the file failed to open
			if (fptr == NULL) {
				// Print message
				printf("Failed to open file. Abort editor.\n");

				// Free dynamically allocated memory
				FreeBuffer(buffer, max_strings);

				return 1;
			}

			// Push buffer to the file
			for (int i = 0; i < num_strings; i++) {
				fputs(buffer[i], fptr);
				fputs("\n", fptr);
			}

			printf("Buffer written to file successfully!");

			// Close the file
			fclose(fptr);

			break;

			case 5:
			// Open the file in read mode, to read its current contents
			fptr = fopen("TextFile.txt", "r");

			// Check if the file failed to open
			if (fptr == NULL) {
				// Print message
				printf("Failed to open file. Abort editor.\n");

				// Free dynamically allocated memory
				FreeBuffer(buffer, max_strings);

				return 1;
			}

			// Read file into the buffer
			for (int i = 0; i < num_strings; i++) {
				// Retrieve each line from the file
				fgets(buffer[i], max_length, fptr);

				// Remove trailing newline character
				buffer[i][strcspn(buffer[i], "\n")] = '\0';

				// Print line to console
				printf("%s\n", buffer[i]);
			}

			// Close the file
			fclose(fptr);

			break;

			case 6:
			printf("Goodbye!~ QUITTING PROGRAM...");
			break;

			default:
			printf("Invalid option. Try again...");
		}
		printf("\n");
	} while (choice != 6);

	// Free dynamically allocated memory
	FreeBuffer(buffer, max_strings);

	return 0;
}
