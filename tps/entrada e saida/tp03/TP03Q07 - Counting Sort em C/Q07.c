#define _XOPEN_SOURCE 	700
#define LIST_SIZE 		100
#define TABLE_SIZE 		524288 // 2^19
#define UPVOTE_MAX		10001

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct Game {

	struct tm release_date;
	uint32_t app_id, avg_pt;
	uint8_t age, dlcs, lang_amt, gen_amt;
	float price, upvotes;
	char *name, *owners, *website, *developers;
	char languages[30][50], genres[10][50];
	bool win, lin, mac;

} Game;

/* Hash map that considers game IDs as keys and file lines as values */
char *HASH_TABLE[TABLE_SIZE];

/* Represents the static list */
Game GAMES[LIST_SIZE];
unsigned int ELEMENTS, MOVEMENTS, COMPARISONS;

/* Used as an output formatter */
const char *MONTHS[] =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/* Functions to manipulate data */
void init_table();
char *parse_data(char *buffer, uint16_t *position, char separator);
unsigned int hash(uint32_t id);
void fetch_game(Game *game, char *buffer);
bool to_bool(char *expression);
void parse_languages(Game *game, char *languages);
void parse_genres(Game *game, char *genres);
void print_data(Game game);

/* Functions related to the list */
static inline void list_constructor();
void insert_last(char *buffer);
void sort();
unsigned int float_to_int(Game game);
void send_to_file(double duration);

int main() {
	
	list_constructor();
	init_table();

	char id[10];

	while (true) {
		
		scanf("%s", id);

		if (strcmp(id, "FIM") == 0) break;

		insert_last(HASH_TABLE[hash(atoi(id))]);
	}
	
	clock_t start = clock();

	sort();

	send_to_file((double)(clock() - start)/CLOCKS_PER_SEC);
	
	for (uint8_t i = 0; i < ELEMENTS; i++) print_data(GAMES[i]);
}

static inline void list_constructor() { ELEMENTS = COMPARISONS = MOVEMENTS = 0; }

void init_table() {
	
	FILE *fp = fopen("/tmp/games.csv", "r");

	char buffer[1024];

	while (fgets(buffer, sizeof(buffer), fp) != NULL) {

		uint16_t position = 0;
		uint32_t key = hash(atoi(parse_data(buffer, &position, ',')));

		HASH_TABLE[key] = malloc(sizeof(char)*(strlen(buffer) + 1));
		strcpy(HASH_TABLE[key], buffer);
	}

	fclose(fp);
}

char *parse_data(char *buffer, uint16_t *position, char separator) {

	char tmp[512];
	uint16_t i = *position, j = 0;

	while (buffer[i] != separator) tmp[j++] = buffer[i++];

	*position = i + 1;

	tmp[j] = '\0';

	char *slice = malloc(sizeof(char)*(strlen(tmp)+1));
	strcpy(slice, tmp);

	return slice;
}

unsigned int hash(uint32_t id) {
	
	id = (id ^ 61) ^ (id >> 16);
    id = id + (id << 3);
    id = id ^ (id >> 4);
    id = id * 0x27d4eb2d;
    id = id ^ (id >> 15);

	return id % TABLE_SIZE;
}

void insert_last(char *buffer) { fetch_game(&GAMES[ELEMENTS++], buffer); }

void fetch_game(Game *game, char *buffer) {

	uint16_t position = 0;

	game->app_id = atoi(parse_data(buffer, &position, ','));
	game->name = parse_data(buffer, &position, ',');
	
	if (buffer[position] == '"') {

		++position;
		strptime(parse_data(buffer, &position, '"'), "%b %d, %Y", &game->release_date);
		++position;

	} else strptime(parse_data(buffer, &position, ','), "%b %Y", &game->release_date);

	game->owners = parse_data(buffer, &position, ',');
	game->age = atoi(parse_data(buffer, &position, ','));
	game->price = atof(parse_data(buffer, &position, ','));
	game->dlcs = atoi(parse_data(buffer, &position, ','));

	if (buffer[position] == '"') {

		++position;
		parse_languages(game, parse_data(buffer, &position, '"'));
		++position;

	} else if (buffer[position + 1] != 93) {

		position += 2;
		strcpy(game->languages[0], parse_data(buffer, &position, 39));
		game->lang_amt = 1;
		position += 2;
	
	} else { strcpy(game->languages[0], "\0"); game->lang_amt = 0; position += 2; }

	game->website = parse_data(buffer, &position, ',');
	game->win = to_bool(parse_data(buffer, &position, ','));
	game->mac = to_bool(parse_data(buffer, &position, ','));
	game->lin = to_bool(parse_data(buffer, &position, ','));

	float upvotes = atof(parse_data(buffer, &position, ','));
	float downvotes = atof(parse_data(buffer, &position, ','));

	game->upvotes = 100*(upvotes/(upvotes + downvotes));
	game->avg_pt = atoi(parse_data(buffer, &position, ','));

if (buffer[position] == '"') {

		++position;
		game->developers = parse_data(buffer, &position, '"');
		++position;
	
	} else game->developers = parse_data(buffer, &position, ',');

	if (buffer[position] == '"') { ++position; parse_genres(game, parse_data(buffer, &position, '"')); }

	else parse_genres(game, parse_data(buffer, &position, '\n'));
}

void sort() {

	int count[UPVOTE_MAX];

	memset(count, 0, sizeof(count));
	
	for (uint8_t i = 0; i < ELEMENTS; i++) { count[(uint16_t)(GAMES[i].upvotes*100)]++; MOVEMENTS++; }
	
	for (uint32_t i = 1; i < UPVOTE_MAX; i++) { count[i] += count[i - 1]; MOVEMENTS++; }

	Game output[ELEMENTS];

	for (uint8_t i = 0; i < ELEMENTS; i++) {

		uint16_t to_int = (uint16_t)(GAMES[i].upvotes*100);

		output[count[to_int] - 1] = GAMES[i];
		--count[to_int];

		MOVEMENTS++;
	}

	for (uint8_t i = 0; i < ELEMENTS; i++) GAMES[i] = output[i];
}

void send_to_file(double duration) {
	
	FILE *out = fopen("matrícula_countingsort.txt", "w");

	fprintf(out, "780888\t\t%i\t\t%i\t\t%fs", COMPARISONS, MOVEMENTS, duration);

	fclose(out);
}

bool to_bool(char *expression) { return (strcmp(expression, "True") == 0); }

void parse_languages(Game *game, char *languages) {

	uint16_t position = 2;
	uint8_t elements = 1;

	for (uint16_t i = 0; i < strlen(languages); i++) elements += (languages[i] == ',') ? 1 : 0;

	if (elements == 1) { strcpy(game->languages[0], parse_data(languages, &position, '\'')); return; }

	strcpy(game->languages[0], parse_data(languages, &position, '\''));

	for (uint8_t i = 1; i < elements; i++) { position += 3; strcpy(game->languages[i], parse_data(languages, &position, '\'')); }

	game->lang_amt = elements;
}

void parse_genres(Game *game, char *genres) {

	if (strcmp(genres, "\n") == 0) { strcpy(game->genres[0], "\0"); game->gen_amt = 0; return; }

	uint16_t position = 0;
	uint8_t elements = 1;

	for (uint16_t i = 0; i < strlen(genres); i++) elements += (genres[i] == ',') ? 1 : 0;

	for (uint8_t i = 0; i < elements - 1; i++) strcpy(game->genres[i], parse_data(genres, &position, ','));
	
	strcpy(game->genres[elements - 1], parse_data(genres, &position, '\0'));

	game->gen_amt = elements;
}

void print_data(Game game) {

	printf("%i %s ", game.app_id, game.name);
	printf("%s/%d ", MONTHS[game.release_date.tm_mon], game.release_date.tm_year + 1900);
	printf("%s %i %.2f %i [", game.owners, game.age, game.price, game.dlcs);

	for (uint8_t i = 0; i < game.lang_amt - 1; i++) printf("%s, ", game.languages[i]);

	printf("%s] ", game.languages[game.lang_amt - 1]);
	
	(strcmp(game.website, "\0") == 0) ? printf("null ") : printf("%s ", game.website);

	printf("%s %s %s ", (game.win) ? "true" : "false", (game.mac) ? "true" : "false", (game.lin) ? "true" : "false");
	printf("%.0f%% ", game.upvotes);

	uint16_t hours = game.avg_pt / 60, minutes = game.avg_pt % 60;

	if (hours == 0 && minutes == 0) printf("null ");

	else {

		(hours > 0) ? printf("%ih ", hours) : 0;
		(minutes > 0) ? printf("%im ", minutes) : 0;
	}

	printf("%s [", game.developers);
	
	if (game.gen_amt > 1) {

		for (uint8_t i = 0; i < game.gen_amt - 1; i++) printf("%s, ", game.genres[i]);

		printf("%s]\n", game.genres[game.gen_amt - 1]);
	}

	else {
		
		for (int i = 0; i < strlen(game.genres[0]) - 1; i++) printf("%c", game.genres[0][i]);

		printf("]\n");
	}
}
