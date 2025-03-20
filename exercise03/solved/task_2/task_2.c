#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void string_shift_right(int arg_index, char* input, const char* original) {
	size_t input_length = strlen(input);

	// FOUND we need to add one more space in the array, as strlen(...) doesn't take the null terminator into account
	// char shifted_input[input_length];
	char shifted_input[input_length + 1];

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		// FOUND use the modulo operator instead of division for new pos
		// size_t new_position = (char_index + 2) / input_length;
		size_t new_position = (char_index + 2) % input_length;
		shifted_input[new_position] = input[char_index];
	}

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		input[char_index] = toupper(input[char_index]);
	}

	// FOUND we have to add the null terminator to the end of the newly created array
	// FOUND (the input array already has one, so it's good)
	shifted_input[input_length] = '\0';

	printf("original argv[%d]: %s\nuppercased: %s\nshifted: %s\n", arg_index, original, input, shifted_input);
}

int main(int argc, const char** argv) {
	char* shared_strings[argc];
	for (int arg_index = 0; arg_index < argc; ++arg_index) {
		// FOUND need to adjust arg_length for null terminator
		// size_t arg_length = strlen(argv[arg_index]);
		size_t arg_length = strlen(argv[arg_index]) + 1;

		shared_strings[arg_index] = malloc(arg_length * sizeof(char));
		if (shared_strings[arg_index] == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		strcpy(shared_strings[arg_index], argv[arg_index]);
	}

	// FOUND the usage of <= leads to one extra fork being created
	// for (int arg_index = 0; arg_index <= argc; arg_index++) {
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		pid_t pid = fork();
		if (pid == -1) {
			perror("Could not create fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0) {
			string_shift_right(arg_index, shared_strings[arg_index], argv[arg_index]);

			// FOUND free the shared_strings array, because it is copied
			for (int i = 0; i < argc; i++) {
				free(shared_strings[i]);
			}

			exit(EXIT_SUCCESS);
		}
	}

	// FOUND we don't need to join threads, we need to wait for child processes
	// for (int arg_index = 0; arg_index <= argc; arg_index++) {
	// 	pthread_join(arg_index, NULL);
	// }
    while (wait(NULL) > 0);

	// FOUND the pointers in the shared_strings array are never freed
	for (int i = 0; i < argc; i++) {
		free(shared_strings[i]);
	}

	// FOUND no newline after print
	// printf("Done.");
	printf("Done.\n");
	return EXIT_SUCCESS;
}
