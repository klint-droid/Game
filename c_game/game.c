#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Struct Definition ---
struct Account {
    char accountId[15];
    char username[50];
    char password[15];
};

// --- Function Declarations ---
void generateAccountId(char *id);
int loadAccounts(const char *file, struct Account **acc);
void saveAccounts(const char *file, struct Account *acc, int total);
void registerAccount(struct Account **acc, int *total);
void loginAccount(struct Account *acc, int total);
void deleteAccount(struct Account **acc, int *total);
void playGame(const char *accountId, const char *username, const char *password);
void viewStats();
void sendStats(const char *accountId, const char *username, const char *password, int score, int wins, int losses);
void clearInputBuffer();

// --- Utility Function ---
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// --- ID Generator ---
void generateAccountId(char *id) {
    const char *digits = "0123456789";
    for (int i = 0; i < 14; i++) {
        id[i] = digits[rand() % 10];
    }
    id[14] = '\0';
}

// --- Load Accounts from File ---
int loadAccounts(const char *file, struct Account **acc) {
    FILE *fp = fopen(file, "r");
    if (!fp) return 0;

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "account_id=")) count++;
    }

    rewind(fp);
    *acc = malloc(sizeof(struct Account) * count);
    if (!*acc) {
        fclose(fp);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fscanf(fp, "account_id=%s username=%s password=%s\n",
               (*acc)[i].accountId, (*acc)[i].username, (*acc)[i].password);
    }

    fclose(fp);
    return count;
}

// --- Save Accounts to File ---
void saveAccounts(const char *file, struct Account *acc, int total) {
    FILE *fp = fopen(file, "w");
    if (!fp) return;

    for (int i = 0; i < total; i++) {
        fprintf(fp, "account_id=%s username=%s password=%s\n",
                acc[i].accountId, acc[i].username, acc[i].password);
    }
    fclose(fp);
}

// --- Send Game Stats to Server ---
void sendStats(const char *accountId, const char *username, const char *password, int score, int wins, int losses) {
    char command[1024];
    snprintf(command, sizeof(command),
         "node ../sendStats.js \"%s\" \"%s\" \"%s\" %d %d %d",
         accountId, username, password, score, wins, losses);

    int result = system(command);
    if (result != 0)
        printf("\nFailed to send stats to server.\n");
    else
        printf("\nStats sent successfully.\n");
}

// --- Register Account ---
void registerAccount(struct Account **acc, int *total) {
    struct Account newAcc;
    generateAccountId(newAcc.accountId);

    printf("Generated Account ID: %s\n", newAcc.accountId);
    printf("Enter username: ");
    scanf("%s", newAcc.username);
    printf("Enter password: ");
    scanf("%s", newAcc.password);

    *acc = realloc(*acc, sizeof(struct Account) * (*total + 1));
    (*acc)[*total] = newAcc;
    (*total)++;

    saveAccounts("accounts.txt", *acc, *total);
    printf("Account registered successfully.\n");
}

// --- Login & Play ---
void loginAccount(struct Account *acc, int total) {
    char id[15], user[50], pass[15];
    int found = 0;

    printf("Enter Account ID: ");
    scanf("%s", id);
    printf("Enter Username: ");
    scanf("%s", user);
    printf("Enter Password: ");
    scanf("%s", pass);

    for (int i = 0; i < total; i++) {
        if (strcmp(acc[i].accountId, id) == 0 &&
            strcmp(acc[i].username, user) == 0 &&
            strcmp(acc[i].password, pass) == 0) {
            printf("\nLogin successful. Welcome, %s!\n", user);
            playGame(id, user, pass);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Invalid credentials.\n");
}

// --- Delete Account ---
void deleteAccount(struct Account **acc, int *total) {
    char id[15];
    printf("Enter Account ID to delete: ");
    scanf("%s", id);

    for (int i = 0; i < *total; i++) {
        if (strcmp((*acc)[i].accountId, id) == 0) {
            // Remove from local memory
            for (int j = i; j < *total - 1; j++) {
                (*acc)[j] = (*acc)[j + 1];
            }
            *acc = realloc(*acc, sizeof(struct Account) * (*total - 1));
            (*total)--;

            // Save to file
            saveAccounts("accounts.txt", *acc, *total);
            printf("Account deleted locally.\n");

            // Delete from database via Node.js
            char command[512];
            snprintf(command, sizeof(command), "node ../deleteAccount.js \"%s", id);
            int result = system(command);
            if (result != 0) {
                printf("Failed to delete account from database.\n");
            }
            return;
        }
    }

    printf("Account ID not found.\n");
}


// --- Game Logic ---
void playGame(const char *accountId, const char *username, const char *password) {
    char *words[] = {
    "algorithm", "array", "binary", "bug", "byte",
    "cache", "class", "compile", "constant", "constructor",
    "data", "debug", "declare", "decrement", "dynamic",
    "else", "enum", "execute", "exception", "expression",
    "file", "float", "function", "global", "heap",
    "if", "index", "inheritance", "initialize", "instance",
    "integer", "interface", "iterate", "keyword", "library",
    "linker", "list", "loop", "malloc", "member",
    "memory", "method", "module", "namespace", "null",
    "object", "operator", "overflow", "package", "parameter",
    "pointer", "polymorphism", "pop", "private", "procedure",
    "process", "program", "protected", "public", "queue",
    "recursion", "register", "return", "scope", "search",
    "shift", "sizeof", "stack", "static", "statement",
    "string", "struct", "subroutine", "switch", "syntax",
    "token", "tree", "type", "typedef", "union",
    "unsigned", "update", "variable", "vector", "virtual",
    "void", "volatile", "while", "write", "xor",
    "yield", "zip", "accessor", "adapter", "aggregate",
    "anchor", "atomic", "asynchronous", "binding", "bitmask"
};

    char word[50], scrambled[50], guess[50];
    int score = 0, wins = 0, losses = 0, again;
    int wordCount = sizeof(words)/sizeof(words[0]);

    printf("\nWelcome to Scrambled Words, %s (ID: %s)\n", username, accountId);
    srand(time(NULL));

    do {
        strcpy(word, words[rand() % wordCount]);
        strcpy(scrambled, word);

        for (int i = 0; i < strlen(scrambled); i++) {
            int j = rand() % strlen(scrambled);
            char tmp = scrambled[i];
            scrambled[i] = scrambled[j];
            scrambled[j] = tmp;
        }

        printf("\nScrambled: %s\nGuess: ", scrambled);
        clearInputBuffer();
        scanf("%s", guess);

        if (strcmp(guess, word) == 0) {
            printf("Correct!\n");
            score += 50;
            wins++;
        } else {
            printf("Wrong. Word was: %s\n", word);
            losses++;
        }

        sendStats(accountId, username, password, score, wins, losses);
        printf("Play again? (1=Yes, 0=No): ");
        scanf("%d", &again);

    } while (again == 1);

    printf("Game over!\n");
}

// --- View Stats ---
void viewStats() {
    printf("\nStats are now handled in the online leaderboard.\n");
}

// --- Main ---
int main() {
    struct Account *accounts = NULL;
    int total = 0, choice;

    srand(time(NULL));
    total = loadAccounts("accounts.txt", &accounts);

    do {
        printf("\n--- Scrambled Word Game ---\n");
        printf("[1] Register\n[2] Login & Play\n[3] View Stats\n[4] Delete Account\n[5] Exit\nChoose: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerAccount(&accounts, &total); break;
            case 2: loginAccount(accounts, total); break;
            case 3: viewStats(); break;
            case 4: deleteAccount(&accounts, &total); break;
            case 5: printf("Exiting...\n"); break;
            default: printf("Invalid option.\n");
        }

    } while (choice != 5);

    free(accounts);
    return 0;
}
