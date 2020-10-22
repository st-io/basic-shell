
/* *
 * Prompts for input showing which shell
 * is being used
 * */
void *prompt(char *args);

/* *
 * Reads a command and its arguments
 * char by char and stores them in a
 * dynamically allocated buffer.
 * Returns char pointer to buffer or
 * null if something goes wrong.
 * */
char *read_cmd();

/* *
 * Creates a child proccess with fork().
 * */
int create_child();

/* *
 * Executes the command with its
 * arguments(if any) using execvp.
 * */
void exec_cmd(char **args);

/* *
 * Makes a deep copy of *cmd and creates tokens.
 * Returns number of tokens.
 * */
int cnt_tok(char *cmd);

/* *
 * Tokenizes the command into arguments.
 * Returns char pointer to array with arguments.
 * */
char **tok_cmd(char *cmd, int tokens);

/* *
 * Checks if there is piping.
 * Returns position of pipe if there is, 0 if not.
 * */
int chk_pipe(char **args);

/* *
 * Creates a copy of **args replacing '>', '>>',
 * '<' and '|' with NULL pointers.
 * Returns char pointer to new array
 *  */
char **split_cmd(char **args, int len);

/* *
 * Redirects stdin/stdout to fd[0]/fd[1] where needed.
 * Returns 1 if redirection took place, 0 if not.
 * */
int do_redir(char **args, int len, int *fd);

