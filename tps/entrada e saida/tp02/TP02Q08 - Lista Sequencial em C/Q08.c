#define _XOPEN_SOURCE 700
#define MAX 70

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct Game {

	struct tm releaseDate;
	uint32_t appId;
	uint16_t avgPt;
	uint8_t age, dlcs, languagesAmount, genresAmount;
	float price, upvotes;
	char name[50], owners[50], website[60], developers[50];
	char languages[30][50], genres[10][50];
	bool win, lin, mac;

} Game;

/* Global variables */
const char *MONTHS[] =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
Game GAMES[MAX];
int ELEMENTS;

// Functions to manipulate data
char *search(int key);
char *parseData(char buffer[], uint16_t *position, char separator);
void fetchGame(Game *game, char buffer[]);
void parseLanguages(Game *game, char languages[]);
bool toBool(char expression[]);
void parseGenres(Game *game, char genres[]);
void printData(Game game);

// Functions related to the list
void constructor();
void insert(int position, char buffer[]);
void insertFirst(char buffer[]);
void insertLast(char buffer[]);
void removeAt(int position);
void removeFirst();
void removeLast();

int main() {
	
	// "Initializing" the structure
	constructor();

	int input[50], index = 0, operations;

	char buffer[1024];

	/* Getting the IDs selected */
	while (true) {

		fgets(buffer, sizeof(buffer), stdin);

		if (strcmp(buffer, "FIM\n") == 0) break;

		input[index++] = atoi(buffer);
	}
	
	// Searches for the specified games and inserts them into the list
	for (int i = 0; i < sizeof(input)/sizeof(int); i++) insertLast(search(input[i]));
	
	/* Second part of the input */
	scanf("%i", &operations);

	for (int i = 0; i < operations; i++) {

		scanf("%s", buffer);

		if (strcmp(buffer, "II") == 0) { int key; scanf("%i", &key); insertFirst(search(key)); }

		else if (strcmp(buffer, "IF") == 0) { int key; scanf("%i", &key); insertLast(search(key)); }

		else if (strcmp(buffer, "I*") == 0) { int position, key; scanf("%i %i", &position, &key); insert(position, search(key)); }

		else if (strcmp(buffer, "RI") == 0) removeFirst();

		else if (strcmp(buffer, "RF") == 0) removeLast();

		else if (strcmp(buffer, "R*") == 0) { int position; scanf("%i", &position); removeAt(position); }
	}
	
	// Shows the remaining games after all operations
	for (int i = 0; i < ELEMENTS; i++) { printf("[%i] ", i); printData(GAMES[i]); }
}

void constructor() { ELEMENTS = 0; }

// Gets the line containing data from the game with the specified key
char *search(int key) {

	char *buffer = malloc(sizeof(char)*1024);

	FILE *fp = fopen("/tmp/games.csv", "r");

	while (fgets(buffer, 1024, fp) != NULL) {

		uint16_t position = 0;

		if (atoi(parseData(buffer, &position, ',')) == key) break;
	}

	fclose(fp);

	return buffer;
}

void insert(int position, char buffer[]) {
	
	for (int i = ELEMENTS; i > position; i--) GAMES[i] = GAMES[i - 1];

	fetchGame(&GAMES[position], buffer);

	ELEMENTS++;
}

void insertFirst(char buffer[]) {

	for (int i = ELEMENTS; i > 0; i--) GAMES[i] = GAMES[i - 1];

	fetchGame(&GAMES[0], buffer);

	ELEMENTS++;
}

void insertLast(char buffer[]) { fetchGame(&GAMES[ELEMENTS++], buffer); }

void removeAt(int position) {

	Game removed = GAMES[position];

	for (int i = position; i < ELEMENTS; i++) GAMES[i] = GAMES[i + 1];

	printf("(R) %s\n", removed.name);

	ELEMENTS--;
}

void removeFirst() {

	Game removed = GAMES[0];

	for (int i = 0; i < ELEMENTS; i++) GAMES[i] = GAMES[i + 1];

	printf("(R) %s\n", removed.name);

	ELEMENTS--;
}

void removeLast() {
	
	Game removed = GAMES[--ELEMENTS];

	printf("(R) %s\n", removed.name);
}

void fetchGame(Game *game, char buffer[]) {

	uint16_t position = 0;

	game->appId = atoi(parseData(buffer, &position, ','));

	strcpy(game->name, parseData(buffer, &position, ','));

	if (buffer[position] == '"') {

		++position;
		strptime(parseData(buffer, &position, '"'), "%b %d, %Y", &game->releaseDate);
		++position;

	} else strptime(parseData(buffer, &position, ','), "%b %Y", &game->releaseDate);

	strcpy(game->owners, parseData(buffer, &position, ','));
	game->age = atoi(parseData(buffer, &position, ','));
	game->price = atof(parseData(buffer, &position, ','));
	game->dlcs = atoi(parseData(buffer, &position, ','));

	if (buffer[position] == '"') {

		++position;
		parseLanguages(game, parseData(buffer, &position, '"'));
		++position;
	
	} else if (buffer[position + 1] != 93) {

		position += 2;
		strcpy(game->languages[0], parseData(buffer, &position, 39));
		game->languagesAmount = 1;
		position += 2;
	
	} else { strcpy(game->languages[0], "\0"); game->languagesAmount = 0; position += 2; }

	char website[60];

	strcpy(website, parseData(buffer, &position, ','));

	(strcmp(website, "\0") == 0) ? strcpy(game->website, "null") : strcpy(game->website, website);

	game->win = toBool(parseData(buffer, &position, ','));
	game->mac = toBool(parseData(buffer, &position, ','));
	game->lin = toBool(parseData(buffer, &position, ','));

	float upvotes = atof(parseData(buffer, &position, ','));
	float downvotes = atof(parseData(buffer, &position, ','));

	game->upvotes = 100*(upvotes/(upvotes + downvotes));
	game->avgPt = atoi(parseData(buffer, &position, ','));

	if (buffer[position] == '"') {

		++position;
		strcpy(game->developers, parseData(buffer, &position, '"'));
		++position;
	
	} else strcpy(game->developers, parseData(buffer, &position, ','));

	if (buffer[position] == '"') { ++position; parseGenres(game, parseData(buffer, &position, '"')); }

	else parseGenres(game, parseData(buffer, &position, '\n'));
}

char *parseData(char buffer[], uint16_t *position, char separator) {

	char *tmp = malloc(sizeof(char));

	int i = *position, j = 0;

	while (buffer[i] != separator) { tmp[j++] = buffer[i++]; tmp = realloc(tmp, strlen(tmp) + 1); }

	*position = i + 1;

	tmp[j] = '\0';

	return tmp;
}

void parseLanguages(Game *game, char languages[]) {

	uint16_t position = 2;
	uint8_t elements = 1;

	for (uint16_t i = 0; i < strlen(languages); i++) elements += (languages[i] == ',') ? 1 : 0;

	if (elements == 1) { strcpy(game->languages[0], parseData(languages, &position, '\'')); return; }

	strcpy(game->languages[0], parseData(languages, &position, '\''));

	for (uint8_t i = 1; i < elements; i++) { position += 3; strcpy(game->languages[i], parseData(languages, &position, '\'')); }

	game->languagesAmount = elements;
}

bool toBool(char expression[]) { return (strcmp(expression, "True") == 0); }

void parseGenres(Game *game, char genres[]) {

	if (strcmp(genres, "\n") == 0) { strcpy(game->genres[0], "\0"); game->genresAmount = 0; return; }

	uint16_t position = 0;
	uint8_t elements = 1;

	for (uint8_t i = 0; i < strlen(genres); i++) elements += (genres[i] == ',') ? 1 : 0;

	for (uint8_t i = 0; i < elements - 1; i++) strcpy(game->genres[i], parseData(genres, &position, ','));

	strcpy(game->genres[elements - 1], parseData(genres, &position, '\0'));

	game->genresAmount = elements;
}

void printData(Game game) {

	printf("%i %s ", game.appId, game.name);

	printf("%s/%d ", MONTHS[game.releaseDate.tm_mon], game.releaseDate.tm_year + 1900);

	printf("%s %i %.2f %i [", game.owners, game.age, game.price, game.dlcs);

	for (uint8_t i = 0; i < game.languagesAmount - 1; i++) printf("%s, ", game.languages[i]);

	printf("%s] ", game.languages[game.languagesAmount - 1]);

	printf("%s %s %s %s ", game.website, (game.win) ? "true" : "false", (game.mac) ? "true" : "false", (game.lin) ? "true" : "false");

	printf("%.0f%% ", game.upvotes);

	uint8_t hours = game.avgPt / 60, minutes = game.avgPt % 60;

	if (hours == 0 && minutes == 0) printf("null ");

	else {

		(hours > 0) ? printf("%ih ", hours) : 0;
		(minutes > 0) ? printf("%im ", minutes) : 0;
	}

	printf("%s [", game.developers);

	for (uint8_t i = 0; i < game.genresAmount - 1; i++) printf("%s, ", game.genres[i]);

	printf("%s]\n", game.genres[game.genresAmount - 1]);
}
