#define NOB_IMPLEMENTATION
#include <nob/nob.h>
#define NOB_MOD
#define NO_LINKERS
#define WFLAGS_PRESET
#include <nob/nob_mod.h>

enum STATE {
	DEFAULT = 0xA0,
	INPUT,
	OUTPUT,
	FLUSH_ARGS,
	FLUSH
};

static int cat_state = DEFAULT;

int main(int argc, char** argv){
	Nob_Cmd cflags = {0};
	nob_cmd_append(&cflags, "-std=c11", "-pedantic", "-I./include");
	CUSTOM_GO_REBUILD_URSELF(argc, argv, &cflags, "./cat.c");
	
	int return_value = 0, character = 0;
	system("echo [----------------------]");
	FILE *read_pointer;
	FILE *write_pointer;
	
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-o") == 0){
			cat_state = OUTPUT;
			++i;
		}
		if(strcmp(argv[i], "-i") == 0){
			cat_state = INPUT;
			++i;
		}
		if(strcmp(argv[i], "-flush") == 0){
			cat_state = FLUSH;
			++i;
		}
		if(strcmp(argv[i], "-flush-args") == 0){
			cat_state = FLUSH_ARGS;
			++i;
		}
		
		switch(cat_state){
			case OUTPUT:
				if(write_pointer) fclose(write_pointer);
				write_pointer = fopen(argv[i], "a");
				break;
			case INPUT:
				read_pointer = fopen(argv[i], "r");
				while((character = fgetc(read_pointer)) != EOF){
					fputc(character, write_pointer);
				}
				fprintf(write_pointer, "\n");
				if(read_pointer) fclose(read_pointer);
				break;
			case FLUSH_ARGS:
				printf("%s ", argv[i]);
				break;
			case FLUSH:
				// TO-DO
				break;
		}
		
	endloop:
	}
	system("echo.");
	system("echo [----------------------]");
	
shutdown:
	fclose(write_pointer);
	return return_value;
}