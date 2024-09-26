//Autores: Ignacio Dosil Francesch, ignacio.dosil@udc.es
//	   Simon Noya Domínguez, simonnoyad@udc.es


#include <pwd.h>					// Aporta la definicion de datos de la estructura passwd
#include <grp.h>					// Aporta la definicion de datos de los grupos de linux
#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <errno.h>					// Librería de captador de errores
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <dirent.h>					// Librería que importa las entradas de directorios
#include <signal.h>					// Librería que define macros para captar salidas anómalas
#include <sys/shm.h>				// Librería con utilidades para la memoria compartida
#include <sys/mman.h>				// Librería para la declaración de gestiones de memoria
#include <sys/stat.h>				// Obtener información de los archivos
#include <sys/types.h>				// Obtiene los tipos de datos del sistema
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]
#include <fcntl.h>					// ?
#include <sys/wait.h>				//Incluye funciones wait
#include <ctype.h>				//Incluye funciones para comprobar si es un numero
#include <signal.h>
#include <sys/stat.h>	
#include <sys/resource.h>


#include "list.h"					//Libreria con las cabeceras de las listas

// Definiciones globales de la shell
#define COMMAND_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido
#define MAX_PATH 		1024		//Longitud máxima para el path
#define MAX_NAME_LEN	70			// Longitud máxima para nombres auxiliares del programa 'char[]'
#define TAMANO 2048
#define SCSS_EXIT			 0		// Salida de programa
#define SSUCC_EXIT			 1		// Salida exitosa de comando
#define FSUCC_EXIT			-1		// Salida erronea de comando
#define FFAIL_EXIT			 1		// Salida forzada de programa
#define _GNU_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1

enum asign_type {NOT_DEFINED, MALLOC_MEM, SHARED_MEM, MAPPED_MEM};
typedef enum asign_type t_asign;
extern char **environ;



// Definicion de tipos de la lista

struct FileInfo {
    int descriptor;
    char nombre[COMMAND_LEN];  // Ajusta el tamaño según tus necesidades
    char modo[COMMAND_LEN] ;     // Ajusta el tamaño según tus necesidades
    struct FileInfo* siguiente;
};


// Definiciones globales
int argLen=0;						// Número de parametros del comadno introducido
char *args[COMMAND_LEN];			// Parámetros del comando introducido
char linea[COMMAND_BUFFER]=" ";		// String con el comando introducido
char get_input[COMMAND_BUFFER]=" ";	// Obtiene la líne introducida por el usuario, se crea para splitearla por \n y que no se sobreescriba en la misma direccion de memoria
List historicList;			// Lista del histórico de comandos
List memList;
List procList;		



// Métodos del sistema
void printPrompt(int exitStatus);
void freed_list_memory(List historicList, List memoryList, List procList);
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine(char linea[COMMAND_BUFFER], int *argLen, char *args[COMMAND_LEN], List historicList, List memoryList, List procList);
int executeCommand(const int numargs, char *tokens[COMMAND_LEN], char *envp[], List historicList, List memoryList, List procList);
void controled_exit(List historicList, List memoryList, List procList, int exitCode);
int report_error_exit(int errorCode);


// Programas shell-in Build
int cmd_Autores(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Pid(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Carpeta(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Fecha(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Time(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Hist(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Comando(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Infosys(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Help(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Exit(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_ListOpen(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Open(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Close(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Dup(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
// P1
int cmd_Create(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Stat(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_List(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Delete(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Deltree(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
static int checkZeroPharam(char arg[COMMAND_LEN]);
//P2
int cmd_Malloc(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Shared(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Mmap(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Read(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Write(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Memdump(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Memory(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Memfill(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmd_Recurse(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
//P3
int cmdShowvar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdChangevar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdShowenv(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdFork(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdExecute(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdListjobs(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdDeljobs(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdJob(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdUid(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdsubsvar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);


// Tablas necesarias para la práctica

typedef struct{
	key_t key;				// Clave utilizada en share
	char *file_name;		// Nombre del archivo utilizado en mmap
	int file_descriptor;	// Redireccionador utilizado en mmap
} t_oinfo;

struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList);
};
struct mem_table_data{
	void *dir;				// Dirección de memoria
	size_t size;			// Tamaño de la reserva
	struct tm time;			// Tiempo de asignación
	t_asign type;			// Tipo de asignación
	t_oinfo data;			// Otra información
};
typedef struct mem_table_data memory_item;

// Las siguientes funciones nos permiten obtener el nombre de una senal a partir del número y viceversa
struct SEN{
	char *sen;
	int senal;
};


enum proc_stats {ACTIVE, STOPPED, SIGNALED, FINISHED, UNKNOWN};
typedef enum proc_stats t_pstat;

// Tipos de información almacenada en los items de la lista de procesos
typedef struct{
	pid_t pid;					// PID del nuevo proceso
	char *user;					// Usuario que ejecuta el proceso
	struct tm time;				// Tiempo en el que se ha ejecutado el proceso
	t_pstat status;				// Estado del proceso
	char *line;					// Linea que ejecuta este proceso
	int priority;				// Prioridad del proceso
	int end;					// Finalización del proceso
} t_proc;

// P1
static int currentDirectory();
static void print_file_info(const char *name, const char *allPath, const struct stat *std, int longp, int accp, int linkp);
static void print_dir_data(const char *name, int hidp, int longp, int accp, int linkp);
static void process_dir_data(const char *name, int recap, int recbp, int hidp, int longp, int accp, int linkp);
static void list_fd_data(const char *name, const struct stat *std, int recap, int recbp, int hidp, int longp, int accp, int linkp);
//P2
static void freeMemoryListItem(void *data);
static void print_memList(t_asign asign);
static void do_DeallocateDelkey(char *args);
void * ObtenerMemoriaShmget(key_t clave, size_t tam, memory_item *item, List memList);
static void * MapearFichero(char * fichero, int protection);
static void do_AllocateMmap(char *fich, char *permisos);
static int LeerFichero(char *fich, void *p, int n);
static int EscribirFichero(char *fich, void *p, int n);
static void Recursiva(int n);
void LlenarMemoria(void *p, size_t cont, unsigned char byte);
//P3
int print_environ_values(char *env[], char *env_type);			
int update_data(t_proc *item);									
void print_proc_dataItem(t_proc *item);							
void imprimir_lista_procesos(List procList, pid_t pid);		
void del_process_by_status(List procList, t_pstat status);
void freeProcessListItem(void *data);											
char *t_stattoa(t_pstat stat);															
int BuscarVariable(char *var, char *e[]);												
int CambiarVariable(char *var, char *valor, char *e[]);									
int external_functionality(const int argLen, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List procList);	

struct cmd_data cmd_table[] = {
	{"authors", cmd_Autores},
	{"pid", cmd_Pid},
	{"chdir", cmd_Carpeta},
	{"date", cmd_Fecha},
	{"time",cmd_Time},
	{"hist", cmd_Hist},
	{"comand", cmd_Comando},
	{"infosys", cmd_Infosys},
	{"help", cmd_Help},
    	{"open", cmd_Open},
    	{"close", cmd_Close},
    	{"dup", cmd_Dup},
    	{"listopen",cmd_ListOpen},
	{"fin", cmd_Exit},
	{"salir", cmd_Exit},
	{"bye", cmd_Exit},
	
	// P1
	{"create", cmd_Create},
	{"stat", cmd_Stat},
	{"list", cmd_List},
	{"delete", cmd_Delete},
	{"deltree", cmd_Deltree},

	//P2
	{"malloc", cmd_Malloc},
	{"shared",cmd_Shared},
	{"mmap",cmd_Mmap},
	{"read",cmd_Read},
	{"write",cmd_Write},
	{"memdump",cmd_Memdump},
	{"mem",cmd_Memory},
	{"memfill",cmd_Memfill},
	{"recurse",cmd_Recurse},
	
	//P3
	{"showvar", cmdShowvar},
	{"changevar", cmdChangevar},
	{"showenv", cmdShowenv},
	{"fork", cmdFork},
	{"execute", cmdExecute},
	{"jobs", cmdListjobs},
	{"deljobs", cmdDeljobs},
	{"job", cmdJob},
	{"uid", cmdUid},
	{"subsvar", cmdsubsvar},
		
	
	{NULL, NULL}
	 
};

// Tabla para la ayuda de los comandos
struct cmd_help_data{
	char *cmd_name;
	char *cmd_usage;
};
struct cmd_help_data cmd_help[] = {
	{"authors", "[-n|-l]\tMuestra los nombres y logins de los autores\n"},
	{"pid", "[-p]\tMuestra el pid del shell o de su proceso padre\n"},
	{"chdir", "[dir]\tCambia (o muestra) el directorio actual del shell\n"},
	{"date", "\tMuestra la fecha\n"},
	{"time", "\t Muestra la hora actual\n"},
	{"hist", "[-c|-N]\tMuestra el historico de comandos, con -c lo borra\n"},
	{"close", "[df]\tCierra el fichero asociado al descriptor\n"},
	{"open", "[file] mode\tAbre un fichero usando el modo solicitado\n"},
	{"dup", "[df]\tDuplica el fichero usando el descriptor dado\n"},
	{"listopen", "\t Lista abiertos escribiendo el nombre, modo y descriptor\n"},
	{"comand", "[-N]\tRepite el comando N (del historico)\n"},
	{"infosys", "\tMuestra informacion de la maquina donde corre el shell\n"},
	{"help", "[cmd]\tMuestra ayuda sobre los comandos\n"},
	{"fin", "\tTermina la ejecucion del shell\n"},
	{"salir", "\tTermina la ejecucion del shell\n"},
	{"bye", "\tTermina la ejecucion del shell\n"},
	
	// P1
	{"create", "\tCrea un directorio o un fichero (-f)\n"},
	{"stat", "lista ficheros;\n"},
	{"list", "lista contenidos de directorios\n"},
	{"delete", "Borra ficheros o directorios vacios\n"},
	{"deltree", "tBorra ficheros o directorios no vacios recursivamente\n"},
	
	{NULL, NULL}
};

// Tabla con las señales
struct SEN sigstrnum[]={   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH}, 
	{"IO", SIGIO},
	{"SYS", SIGSYS},
	/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
	{NULL,-1},
};
void liberarListaArchivos(struct FileInfo *listaArchivos) {
    struct FileInfo *actual = listaArchivos;
    struct FileInfo *temp;

    while (actual != NULL) {
        temp = actual;
        actual = actual->siguiente;
        free(temp); // Liberar la memoria de la entrada actual
    }
}

struct FileInfo *listaArchivos = NULL;

int obtenerInfoDescriptores() {
    // Obtener información de los descriptores de archivo heredados
    for (int fd = 0; fd < 3; fd++) {
        int flags = 0;
        char modo[COMMAND_LEN];

        // Intentar obtener información sobre el descriptor de archivo
        if ((flags = fcntl(fd, F_GETFL, 0)) != -1 ) {

            // Verificar los modos de apertura
            if (flags & O_RDONLY)
                strcpy(modo, "O_RDONLY");
            if (flags & O_WRONLY)
                strcpy(modo, "O_WRONLY");
            if (flags & O_RDWR)
                strcpy(modo, "O_RDWR");
            if (flags & O_APPEND)
                strcpy(modo, "O_APPEND");
            if (flags & O_CREAT)
                strcpy(modo, "O_CREAT");
            if (flags & O_TRUNC)
                strcpy(modo, "O_TRUNC");
            if (flags & O_EXCL)
                strcpy(modo, "O_EXCL");

            // Crear una nueva entrada de archivo
            struct FileInfo *nuevaEntrada = (struct FileInfo *)malloc(sizeof(struct FileInfo));
            if (nuevaEntrada == NULL) {
                perror("Error al asignar memoria");
                exit(1);
            }

            if (fd == 0) {
                strcpy(nuevaEntrada->nombre, "stdin");
            }
            if (fd == 1) {
                strcpy(nuevaEntrada->nombre, "stdout");
            }
            if (fd == 2) {
                strcpy(nuevaEntrada->nombre, "stderr");
            }

            nuevaEntrada->descriptor = fd;
            strcpy(nuevaEntrada->modo, modo);
            nuevaEntrada->siguiente = NULL; // Establecer el siguiente como NULL inicialmente

            // Agregar la nueva entrada al final de la lista
            if (listaArchivos == NULL) {
                listaArchivos = nuevaEntrada; // Si la lista está vacía, la nueva entrada se convierte en el primer elemento.
            } else {
                struct FileInfo *lastFile = listaArchivos;
                while (lastFile->siguiente != NULL) {
                    lastFile = lastFile->siguiente; // Encontrar el último elemento de la lista.
                }
                lastFile->siguiente = nuevaEntrada; // Agregar la nueva entrada al final de la lista.
            }
        }
    }

    return 1;
}

/* == MAIN FUNCTION == */
int main(int argc, char const *argv[], char *envp[]) {

    int exitCode=SSUCC_EXIT;
    // Crear la lista del histórico
    createList(&historicList);
    createList(&memList);
    createList(&procList);
    obtenerInfoDescriptores();

    do{
		printPrompt(exitCode);
		getCmdLine(linea, &argLen, args, historicList, memList, procList);
		exitCode = executeCommand(argLen, args, envp, historicList, memList, procList);
	}while(exitCode != SCSS_EXIT);

    // Liberar la memoria de la lista del histórico
    freed_list_memory(historicList, memList, procList);
    // Liberar la memoria de la lista de archivos
    liberarListaArchivos(listaArchivos);
    return 0;
}



// === DECLARACIONES PROPIAS DENTRO DE STRING.H ===
char *strndup(const char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}



// == SYSTEM METHODS ==
// Imprime por pantalla el propmt del usuario
void freed_list_memory(List historicList, List memList, List procList){
	// Elimina la lista del historico
	deleteList(historicList, free);

	// Elimina la lista de la memoria
	deleteList(memList, freeMemoryListItem);

	// Elimina la lista de memoria
	deleteList(procList, freeProcessListItem);
}
int executeCommand(const int numargs, char *tokens[COMMAND_LEN], char *envp[], List historicList, List memList, List procList){
	register int i=0;

	// Comprobando comando no introducido
	if(numargs==0) return SSUCC_EXIT;

	// Obtenemos la posicion en la tabla de comandos del comando a ejecutar
	while(cmd_table[i].cmd_name!=NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobando si es una posicion erronea
	if(cmd_table[i].cmd_name!=NULL)

		// Función no implementada en la shell
		if(cmd_table[i].cmd_func == NULL)
			report_error_exit(ENOSYS);
		else
			return cmd_table[i].cmd_func(numargs, tokens, envp, historicList, memList, procList);

	else
		return external_functionality(numargs, tokens, envp, historicList, memList, procList);
	return SSUCC_EXIT;
}
void controled_exit(List historicList, List memList, List procList, int exitCode){
	freed_list_memory(historicList, memList, procList);	// Liberar memoria de las listas
	exit(exitCode);
}
int TrocearCadena(char *line, char *tokens[]){
	register int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}
void check_init_lists(List historicList, List memList, List procList){
	// Comprobamos que la lista del histórico está inicializada
	if(historicList == NULL)	createList(&historicList);

	// Comprobamos que la lista de la memoria está inicializada
	if(memList == NULL)		createList(&memList);

	// Comprobamos que la lista de procesos está inicializada
	if(procList == NULL)		createList(&procList);
}
int report_error_exit(int errorCode){
	if(errorCode<=0 || errorCode>131)	printf("[!] Error: invalid errorCode\n");
	else	printf("[!] Error: %s\n", strerror(errorCode));
	return FSUCC_EXIT;
}
void printPrompt(int exitStatus){
	printf("-> ");	// Prompt de la shell de referencia
}
void getCmdLine(char linea[COMMAND_BUFFER], int *argLen, char *args[COMMAND_LEN], List historicList, List memoList, List procList){
	char get_input[COMMAND_BUFFER];

	// Comprobar que las listas están inicializadas
	check_init_lists(historicList, memList, procList);

	// Obteniendo el string de los comandos
	if(fgets(get_input, COMMAND_BUFFER, stdin)==NULL){
		printf("\n[!] Fin...\n");
		controled_exit(historicList, memList, procList, SCSS_EXIT);
	}

	// Comprobación del comando introducido
	if(strcmp(get_input, "\n")!=0 && get_input!=NULL){
		char *token = strtok(get_input, "\n");
		strcpy(linea, token);

		// Insertar en la lista
		if(!insertElement(historicList, strndup(linea, COMMAND_BUFFER)))
			printf("[!] Error: %s\n", strerror(ENOMEM));

		// Separar en args la cadena de texto introducida
		*argLen = TrocearCadena(linea, args);

	// En caso de no ser válido, la línea se vacía y el número de argumentos se pone a 0
	}else{
		strcpy(linea, " ");
		*argLen = 0;
	}
}

// == PROGRAMAS SHELL-IN BUILD ==
int cmd_Autores(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1){
		printf("Ignacio Dosil Francesch: ignacio.dosil\n");
		printf("Simon Noya Dominguez: simon.noyad\n");
		return 1;
	}

	if(strcmp("-l", args[1])==0)
		printf("ignacio.dosil\nsimon.noyad\n");
	
	else if(strcmp("-n", args[1])==0)
		printf("Ignacio Dosil Francesch\nSimon Noya Dominguez\n");
	
	else
		printf("[!] Error: %s\n", strerror(22));

	return 1;
}

int cmd_Pid(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1){
		printf("Pid de shell: %d\n", getppid());
		return 1;
	}

	if(strcmp(args[1], "-p")==0)
		printf("Pid del padre del shell: %d\n", getpid());
	else
		printf("[!] Error: %s\n", strerror(22));
	
	return 1;
}

int cmd_Carpeta(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)
		return currentDirectory();

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(ENOENT));
	return 1;
}

int cmd_Fecha(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	char datosFecha[70]="";
	if(strftime(datosFecha, sizeof datosFecha, "%d/%m/%Y", &tiempoLocal)==0){
		printf("[!] Error: %s\n", strerror(8));
		return 1;
	}

	if(lenArg==1){
		printf("%s\n",datosFecha);
		return 1;
	}
	
	return 1;
}


int cmd_Time(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	char datosHora[70]="";
	if(strftime(datosHora, sizeof datosHora, "%H:%M:%S", &tiempoLocal)==0){
		printf("[!] Error: %s\n", strerror(8));
		return 1;
	}
	
		if(lenArg==1){
		printf("%s\n", datosHora);
		return 1;
	}
	return 1;
}

// Función propia de esta consola para el comando hist
static void print_Ncommands(int n){
	Lpos auxPos;
	register int iter=0;

	if(n>0){
		for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos)){
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
		}

	}else{
		for(auxPos=firstElement(historicList); auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos)){
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
		}
	}
}

int cmd_Hist(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1){
		print_Ncommands(-1);
		return 1;
	}

	if(strcmp(args[1], "-c")==0){
		clearList(historicList,free);
		return 1;
	}

	int n=checkZeroPharam(args[1]);
	if(n<0)
		printf("[!] Error: %s\n", strerror(EINVAL));
	else
		print_Ncommands(n);

	return 1;
}

static int checkZeroPharam(char arg[COMMAND_LEN]){
	int n=-1;
	arg[0]='0';
	return ((n=atoi(arg))<=0 && arg[1]!='0')? -1 : n;
}

int cmd_Comando(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg>1){
		int iter=0, nCommand = atoi(args[1]);
		
		if(nCommand<=0 && strcmp(args[1], "0")!=0){
			printf("[!] Error: %s\n", strerror(22));
			return 1;
		}

		Lpos auxPos;
		for(auxPos=firstElement(historicList); iter<nCommand && auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos));

		// Comprobar la salida del bucle
		if(auxPos==NULL){
			printf("[!] Error: %s\n", strerror(95));
			return 1;
		}
		strcpy(linea, getElement(historicList, auxPos));
		printf("Ejecutando hist (%d): %s\n", nCommand, linea);

		// Separar en args la cadena de texto introducida
		argLen = TrocearCadena(linea, args);
		return executeCommand(TrocearCadena(linea, args), args, envp, historicList, memList, procList);
	}

	print_Ncommands(-1);
	return 1;
}

int cmd_Infosys(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	struct utsname systemData;
	if(uname(&systemData)==-1)
		printf("[!] Error: %s\n", strerror(61));
	else
		printf("%s (%s), OS: %s-%s-%s\n", systemData.nodename, systemData.machine, systemData.sysname, systemData.release, systemData.version);

	return 1;
}


void imprimirListaArchivos(struct FileInfo *listaArchivos) {
    struct FileInfo *actual = listaArchivos;

    if (actual == NULL) {
        printf("No hay archivos en la lista.\n");
        return;
    }

    printf("Lista de archivos:\n");

    while (actual != NULL) {
        printf(" %d ", actual->descriptor);
        printf(" %s ", actual->nombre);
        printf(" %s\n", actual->modo);
        actual = actual->siguiente;
    }
}

int cmd_ListOpen(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
	imprimirListaArchivos(listaArchivos); 
    return 1;
}



int cmd_Open(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    if (lenArg == 2) {
        char *file = args[1];

        // Verificar si el archivo existe
        if (access(file, F_OK) == -1) {
            printf("[!] Error: %s\n", strerror(2));
            return 1;
        }

        struct FileInfo *nuevaEntrada = (struct FileInfo *)malloc(sizeof(struct FileInfo));
        if (nuevaEntrada == NULL) {
            printf("[!] Error: %s\n", strerror(62));
            return 1;
        }

        int fd = open(file, O_RDONLY, 0777);

        if (fd == -1) {
            printf("[!] Error: %s\n", strerror(61));
            return 1;
        }

        printf("Archivo '%s' abierto con éxito\n", file);

        nuevaEntrada->descriptor = fd;
        strcpy(nuevaEntrada->nombre, file);
        strcpy(nuevaEntrada->modo, ""); // Modo nulo 

        nuevaEntrada->siguiente = NULL; 

        // Agregar al final de la lista
        if (listaArchivos == NULL) {
            // Si la lista está vacía, esta entrada se convierte en la primera.
            listaArchivos = nuevaEntrada;
        } else {
            struct FileInfo *temp = listaArchivos;
            while (temp->siguiente != NULL) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevaEntrada; // Agrega la nueva entrada al final
        }

        return 1;
    } else if (lenArg == 3) {
        // Si la longitud de args es 3, se comporta como el código anterior
        char *file = args[1];
        char *mode = args[2];
        int flags = 0;

        struct FileInfo *nuevaEntrada = (struct FileInfo *)malloc(sizeof(struct FileInfo));
        if (nuevaEntrada == NULL) {
            
            return 1;
        }

        // Definir el modo según el argumento proporcionado
        if (strcmp(mode, "cr") == 0) {
            flags = O_CREAT;
            strcpy(nuevaEntrada->modo, "O_CREAT");
        } else if (strcmp(mode, "ap") == 0) {
            flags = O_APPEND;
            strcpy(nuevaEntrada->modo, "O_APPEND");
        } else if (strcmp(mode, "ex") == 0) {
            flags = O_CREAT | O_EXCL;
            strcpy(nuevaEntrada->modo, "O_CREAT");
        } else if (strcmp(mode, "ro") == 0) {
            flags = O_RDONLY;
            strcpy(nuevaEntrada->modo, "O_RDONLY");
        } else if (strcmp(mode, "rw") == 0) {
            flags = O_RDWR;
            strcpy(nuevaEntrada->modo, "O_RDWR");
        } else if (strcmp(mode, "wo") == 0) {
            flags = O_WRONLY;
            strcpy(nuevaEntrada->modo, "O_WRONLY");
        } else if (strcmp(mode, "tr") == 0) {
            flags = O_TRUNC;
            strcpy(nuevaEntrada->modo, "O_TRUNC");
        } else {
            printf("[!] Modo no válido: %s\n", mode);
            return 1;
        }

        int fd = open(file, flags, 0777);

        if (fd == -1) {
            printf("[!] Error: %s\n", strerror(17));
            return 1;
        }

        printf("Archivo '%s' abierto con éxito\n", file);

        nuevaEntrada->descriptor = fd;
        strcpy(nuevaEntrada->nombre, file);
        nuevaEntrada->siguiente = NULL; // Establece el siguiente como NULL para agregar al final

        // Agregar al final de la lista
        if (listaArchivos == NULL) {
            // Si la lista está vacía, esta entrada se convierte en la primera.
            listaArchivos = nuevaEntrada;
        } else {
            struct FileInfo *temp = listaArchivos;
            while (temp->siguiente != NULL) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevaEntrada; // Agrega la nueva entrada al final
        }

        return 1;
    } else {
        imprimirListaArchivos(listaArchivos); // Debes pasar la lista de archivos como argumento
        return 1;
    }
}

int cmd_Dup(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    if (lenArg != 2) {
        imprimirListaArchivos(listaArchivos);
        return 1;
    }

    int sourceDescriptor = atoi(args[1]);
    if (sourceDescriptor < 0 || sourceDescriptor >= COMMAND_LEN) {
        printf("[!] Error: %s\n", strerror(22));
        return 1;
    }

    int newDescriptor = dup(sourceDescriptor);
    if (newDescriptor == -1) {
        perror("Error al duplicar el descriptor de archivo\n");
        return 1;
    }

    char originalName[COMMAND_LEN];
    char originalMode[COMMAND_LEN];

    // Obtener el nombre y el modo del archivo original
    struct FileInfo *currentFile = listaArchivos;
    while (currentFile != NULL) {
        if (currentFile->descriptor == sourceDescriptor) {
            strcpy(originalName, currentFile->nombre);
            strcpy(originalMode, currentFile->modo);
            break;
        }
        currentFile = currentFile->siguiente;
    }

    // Crear una nueva entrada de archivo para la lista.
    struct FileInfo *newFile = (struct FileInfo *)malloc(sizeof(struct FileInfo));
    if (newFile == NULL) {
        printf("[!] Error: %s\n", strerror(12));
        close(newDescriptor);  // Cerrar el nuevo descriptor si falla la asignación de memoria.
        return 1;
    }

    // Configurar la información de la nueva entrada de archivo.
    newFile->descriptor = newDescriptor;
    strcpy(newFile->nombre, originalName); // Copiar el nombre del archivo original
    strcpy(newFile->modo, originalMode);   // Copiar el modo del archivo original
    newFile->siguiente = NULL;

    // Verificar si la lista está vacía.
    if (listaArchivos == NULL) {
        listaArchivos = newFile;
    } else {
        struct FileInfo *lastFile = listaArchivos;
        while (lastFile->siguiente != NULL) {
            lastFile = lastFile->siguiente;
        }
        lastFile->siguiente = newFile;
    }

    return 1;
}




int cmd_Close(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    if (lenArg != 2) {
        imprimirListaArchivos(listaArchivos);
        return 1;
    }

    int descriptor = atoi(args[1]);

    struct FileInfo *actual = listaArchivos;
    struct FileInfo *anterior = NULL;

    // Buscar el archivo en la lista
    while (actual != NULL) {
        if (actual->descriptor == descriptor) {
            // Cerrar el archivo
            close(actual->descriptor);
            printf("Archivo '%s' cerrado con éxito\n", actual->nombre);

            // Eliminar el archivo de la lista
            if (anterior != NULL) {
                anterior->siguiente = actual->siguiente;
            } else {
                listaArchivos = actual->siguiente;
            }

            free(actual);
            return 1;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    printf("[!] Descriptor de archivo no encontrado: %d\n", descriptor);
    return 1;
}
int cmd_Help(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	int i=0;

	if(lenArg==2){
		while(cmd_help[i].cmd_name != NULL && strcmp(cmd_help[i].cmd_name, args[1])!=0)
			++i;

		if(cmd_help[i].cmd_name == NULL)
			printf("[!] Error: %s\n", strerror(38));
		else
			printf("%s %s", cmd_help[i].cmd_name, cmd_help[i].cmd_usage);
	}else{
		printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
		for(i=0; cmd_help[i].cmd_name!=NULL; ++i)
			printf("%s ", cmd_help[i].cmd_name);

		printf("\n");
	}

	return 1;
}

int cmd_Exit(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	return 0;
}


// ==================== PRÁCTICA 1 ====================

char LetraTF(mode_t m){
	switch (m&S_IFMT){	/*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK:	return 's'; /*socket */
		case S_IFLNK:	return 'l';	/*symbolic link*/
		case S_IFREG:	return '-';	/* fichero normal*/
		case S_IFBLK:	return 'b';	/*block device*/
		case S_IFDIR:	return 'd';	/*directorio */
		case S_IFCHR:	return 'c';	/*char device*/
		case S_IFIFO:	return 'p';	/*pipe*/
		default: return '?';	/*desconocido, no deberia aparecer*/
	}
}
char * ConvierteModo2(mode_t m){
	static char permisos[12];
	strcpy (permisos,"---------- ");

	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r';    /*resto*/
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';

	return permisos;
}
static int currentDirectory(){
	char path[COMMAND_BUFFER];
	if(getcwd(path, COMMAND_BUFFER)==NULL)
		printf("[!] Error: %s\n", strerror(ENOTDIR));
	else
		printf("%s\n", path);
	return 1;
}
static void print_file_info(const char *name, const char *allPath, const struct stat *std, int longp, int accp, int linkp){	 //allPath es por si la ruta completa es distinto de nulo 
	if(longp==1){
		struct tm tl;    //Struct para la fecha y hora(hacerla legible)
		struct passwd *pws=getpwuid(std->st_uid);  //Info sobre el usuario que posee el archivo(nombre)
		struct group *grp=getgrgid(std->st_gid);
		char *permisos=ConvierteModo2(std->st_mode), linkName[MAX_NAME_LEN]=" ", user[MAX_NAME_LEN], group[MAX_NAME_LEN];  //Para guardar los permisos de los archivos

		// Parseo del nombre de usuario
		if(pws!=NULL)	sprintf(user, "%s", pws->pw_name);              //Si se encuentra informacion se escribe el nombre del usuario
		else			sprintf(user, "%d", std->st_uid);       // Si no hay informacion, se escribe el identificador

		// Parseo del nombre del grupo
		if(grp!=NULL)	sprintf(group, "%s", grp->gr_name);
		else			sprintf(group, "%d", std->st_gid);

		// Parseo del tiempo de acceso
		if(accp==1)		tl = *localtime(&(std->st_atime));   //Convetir la Hora de accseo al archivo de std en un struct tm
		else			tl = *localtime(&(std->st_ctime));   //Hora de cambio de estado

		// Printeo parcial/completo de los datos por pantalla
		printf("%04d/%02d/%02d-%02d:%02d ", tl.tm_year+1900, tl.tm_mon+1, tl.tm_mday, tl.tm_hour, tl.tm_min);
		printf("%3ld (%8ld) %8s %8s %s%8ld %s", std->st_nlink, std->st_ino, user, group, permisos, std->st_size, name);   //std->st_nlink enlaces duros

		// Comprobando el linkeo
		if(linkp==1 && permisos[0]=='l'){
			readlink((allPath!=NULL)? allPath : name, linkName, sizeof(linkName));    //Si allPath es nulo usamos name. Readlink lee el destino y lo guarda en linkName
			printf(" -> %s", linkName);
		}

	}else
		printf("\t%ld\t%s", std->st_size, name);
	printf("\n");
} 
static void list_fd_data(const char *name, const struct stat *std, int recap, int recbp, int hidp, int longp, int accp, int linkp){
	// Se activa por defecto si -reca y -recb están activas a la vez
	// se ejecuta recb
	if(recbp==1){
		process_dir_data(name, recap, recbp, hidp, longp, accp, linkp);
		print_dir_data(name, hidp, longp, accp, linkp);

	// Se ejecuta reca
	}else if(recap==1){
		print_dir_data(name, hidp, longp, accp, linkp);
		process_dir_data(name, recap, recbp, hidp, longp, accp, linkp);

	// Se muestra el contenido del directorio
	}else
		print_dir_data(name, hidp, longp, accp, linkp);
}
static void print_dir_data(const char *name, int hidp, int longp, int accp, int linkp){
	DIR *dir;
	struct dirent *ent=NULL;
	struct stat fStd;
	char inn_name[COMMAND_LEN];

	// Abrimos el directorio
	if((dir=opendir(name))!=NULL){
		printf("************%s\n", name);

		// Obtenermos cada uno de los archivos del cirectorio
		while((ent=readdir(dir))!=NULL){
			sprintf(inn_name, "%s/%s", name, ent->d_name);

			// [-hid] pasado como parametro
			if(lstat(inn_name, &fStd)==0){
				if(hidp==1)
					print_file_info(ent->d_name, inn_name, &fStd, longp, accp, linkp);
				else
					if(ent->d_name[0]!='.')
						print_file_info(ent->d_name, inn_name, &fStd, longp, accp, linkp);
			}
		}

		// Cerramos el directorio
		closedir(dir);
	}else
		printf("[!] Error: No se puede abrir el directorio\n");
}
static void process_dir_data(const char *name, int recap, int recbp, int hidp, int longp, int accp, int linkp){
	DIR *dir;
	register struct dirent *ent=NULL;  //EStructura para almacenar info de las entradas de un directorio
	struct stat fStd;
	char inn_name[COMMAND_LEN];

	// Abrimos el directorio
	if((dir=opendir(name))!=NULL){

		// Obtenemos los archivos del directorio
		while((ent=readdir(dir))!=NULL){

			// Parseando los directorios . y ..  por la recursividad
			if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0)   //Omitir los directorios padre e actual 
				continue;

			// Concatenamos el path introducido con el nombre del directorio
			sprintf(inn_name, "%s/%s", name, ent->d_name);

			// Obtenemos los datos del archivo
			if(lstat(inn_name, &fStd)==0 && ConvierteModo2(fStd.st_mode)[0]=='d')  //Si hay dentro un directorio miramos su informacion
				list_fd_data(inn_name, &fStd, recap, recbp, hidp, longp, accp, linkp);
		}

		// Cerramos el directorio
		closedir(dir);
	}else
		printf("[!] Error: No se puede abrir el directorio\n");
}


int isDir(const char *path){
    struct stat s;
    lstat(path, &s);
    int out = S_ISDIR(s.st_mode);
    return out;
}
int borrarDir(char *dir){  		//Borra el directorio
    DIR *dirp;          //Puntero a estructura DIR que lo hace abierto
    struct dirent *flist;
    char aux[MAX_NAME_LEN];

    if((dirp=opendir(dir)) ==NULL)return 1;

    while ((flist=readdir(dirp))!=NULL) { 	//Recorre el directorio
        strcpy(aux, dir);
        strcat(strcat(aux, "/"),flist->d_name);  //Creamos el path

        if(strcmp(flist->d_name, "..") == 0 ||
                strcmp(flist->d_name, ".") == 0)continue;   //Omitimos estos directorios

        if(isDir(aux)){ 		//Recursivo
            borrarDir(aux);             //Si es un directorio hacemos una llamada recursiva
        }
        if(remove(aux))return 1; 	 //borramos el archivo actual
    }
    closedir(dirp);

    return 0;
}


int cmd_Create(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	
	char error [MAX_NAME_LEN] = "[!] Error";
	if(lenArg >1){
	
	char path [MAX_PATH];
	
	
	getcwd(path,sizeof(path));  //Obtener directorio de trabajo actual
	strcat(path,"/");           //Añadir /
	
	if(strcmp(args[1],"-f") == 0){		// Si se pasa con -f se crea un archivo
		if (lenArg<3)
			currentDirectory();       //Si solo se pasa f devolvemos el directorio actual
		else{
			char* name = args[2];
			if(creat(strcat(path,name),0644) == -1){       //Creamos un nuevo archivo con creat. strcat la usamos para concatenar path y name
				perror(error);
				}
		
		}
	}else{				// Si solo se pasa el nombre se crea un directorio
		
			char* name = args[1];
			if(mkdir(strcat(path,name), 0755) ==-1){   //Concatenamo y creamos un directorio con mkdir
				perror(error);	
			}
		}
	
	
	}else {
	
	currentDirectory();
	return 1;
	}

	return 1;
}



int cmd_Stat(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	struct stat std;                                //Estructura para almacenar la informacion
	int i=1;
	int numFiles=0, longp=0, accp=0, linkp=0;       //Opciones especiales  numFIles(numero fichros), longp(info mas detallas), accp(tiempo accaseo), linkp(path contenido)
	char error [MAX_NAME_LEN] = "[!] Error";
	
	// Parseo de parámetros 
	while(i<lenArg){                    //Recorremos el argumento
		if(args[i][0]=='-'){        //Miramos si tiene un guion y si lo tiene miramos que opcion es la seleccionada y la ponemos a uno
			if(strcmp(args[i], "-long")==0)
				longp=1;
			if(strcmp(args[i], "-acc")==0)
				accp=1;
			if(strcmp(args[i], "-link")==0)
				linkp=1;
		}else
			++numFiles;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg<=1 || numFiles==0)                   //Si no hay ficheros o la longuitod del argumento es igual o menor a una delvolvemos el path
		currentDirectory();

	// Stat para todos los archivos que se pasen como parámetros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)           // hacemos lstat sobre los archivo (da información sobre un archivo o un enlace simbólico en un sistema de archivos)
			print_file_info(args[i], NULL, &std, longp, accp, linkp);   //Le pasamos a la funcion print_file_info los ints para las opciones
		else
			if(args[i][0]!='-')
				perror(error);   //Si no se pudo acceder devolvemos un error
	}

	return 1;
}


int cmd_List(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	struct stat std;
	register int i=1;
	int numData=0, recap=0, recbp=0, hidp=0, longp=0, accp=0, linkp=0;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-reca")==0)
				recap=1;
			if(strcmp(args[i], "-recb")==0)
				recbp=1;
			if(strcmp(args[i], "-hid")==0)
				hidp=1;
			if(strcmp(args[i], "-long")==0)
				longp=1;
			if(strcmp(args[i], "-acc")==0)
				accp=1;
			if(strcmp(args[i], "-link")==0)
				linkp=1;
		}else
			++numData;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg<=1 || numData==0)
		currentDirectory();

	// Procesamos los archivos pasados como parametros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)
			if(S_ISDIR(std.st_mode))   //Comprobar si es un directorio
				list_fd_data(args[i], &std, recap, recbp, hidp, longp, accp, linkp);
			else
				print_file_info(args[i], NULL, &std, longp, accp, linkp);
		else
			if(args[i][0]!='-')
				printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}

	return 1;
}

int cmd_Delete(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    char error [MAX_NAME_LEN] = "[!] Error";
    
    if(lenArg >1){ 		//Borra el archivo o la carpeta
        for(int i=1; i<lenArg; i++){
            if(remove(args[i]) !=0){
                perror(error);               
            }
        }
    }else { 			// Muestra directorio actual
          currentDirectory();
    }
    return 1;
}
int cmd_Deltree(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	
	char error [MAX_NAME_LEN] = "[!] Error";

    if(lenArg >1){
        for(int i=1; i< lenArg; i++){
            if(isDir(args[i]) ){           //Comprobamos si es un directorio
                if(borrarDir(args[i])==-1 || remove(args[i]))   //Si lo es llamamos a borrarDir y luego a remove
                    perror(error);
            }else if(remove(args[i])){      //Sino llamamos a remove
                perror(error);
            }
        }
    }else { 		//Muestra el directorio actual
       currentDirectory();
    }
	return 1;
}

//------------Practica 2------------
static void freeMemoryListItem(void *data){
	memory_item *item = (memory_item *)(data);
	
	free(item->dir);
	item->size=0;
	item->data.key = 0;
	item->data.file_name = NULL;
	item->data.file_descriptor = 0;

	if(item->type == SHARED_MEM)
		shmdt(item->dir);
	else
		free(item);
}


static char *t_asigntoa(t_asign asign){
	static char asign_name[8];
	strcpy(asign_name, "no_def");

	if(asign == MALLOC_MEM)
		strcpy(asign_name, " malloc");
	if(asign == SHARED_MEM)
		strcpy(asign_name, " shared");
	if(asign == MAPPED_MEM)
		strcpy(asign_name, " mapped");

	return asign_name;
}
static void print_data_item(memory_item *item) {
    char time_format[MAX_NAME_LEN];

    strftime(time_format, MAX_NAME_LEN, "%b %d %R", &item->time);

    printf("\t%p\t%13lu %s", item->dir, (unsigned long)item->size, time_format);
    if (item->type == MALLOC_MEM)
        printf("%s", t_asigntoa(item->type));
    if (item->type == SHARED_MEM)
        printf("%s (key %u)", t_asigntoa(item->type), item->data.key);
    if (item->type == MAPPED_MEM)
        printf(" %s (descriptor %d)", item->data.file_name, item->data.file_descriptor);
    printf("\n");
}

void print_memList(t_asign asign){
	Lpos auxPos;
	memory_item *auxItem = NULL;

	printf("******Lista de bloques asignados para el proceso %d\n", getppid());

	for(auxPos = firstElement(memList); auxPos!=NULL; auxPos=nextElement(memList, auxPos)){
		auxItem = getElement(memList, auxPos);

		// Se muestran por pantalla todas las reservas de memoria
		// En caso de necesitar una salida concreta -> parseo del tipo de memoria reservada
		if(asign == NOT_DEFINED || auxItem->type == asign)
			print_data_item(auxItem);
	}
}
int cmd_Malloc(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    memory_item *infoData;
    size_t tam;
    Lpos auxPos;
    time_t currentTime;

    if (lenArg == 1) {
        print_memList(MALLOC_MEM);
    } else if (strcmp(args[1], "-free") == 0) {
        if (lenArg == 2) {
            print_memList(MALLOC_MEM);
        } else {
            // Comprobante de que se intenta hacer una reserva de 0 bytes
            if ((tam = (size_t) strtoul(args[2], NULL, 10)) == 0) {
                printf("No se pueden desasignar bloques de 0 bytes\n");
                return 1;
            }

            // Recorremos the list hasta el primer bloque de memoria asignado con ese tamaño
            for (auxPos = firstElement(memList); auxPos != NULL; auxPos = nextElement(memList, auxPos)) {
                infoData = getElement(memList, auxPos);
                if (infoData->type != MALLOC_MEM) continue;
                if (infoData->size == tam) break;
            }

            if (auxPos != NULL) {
                infoData = deletePosition(memList, auxPos);

                // Eliminamos el elemento
                free(infoData->dir);
		infoData->size=0;
		infoData->type=NOT_DEFINED;
		infoData->data.key=0;
		infoData->data.file_name=NULL;
		infoData->data.file_descriptor=0;
		free(infoData->data.file_name); 
                free(infoData);
            } else {
                printf("No se encontró un bloque de memoria con el tamaño especificado.\n");
            }
        }
    } else {
        memory_item *nwItem = (memory_item *) malloc(sizeof(memory_item));

        // Comprobante de que se intenta hacer una reserva de 0 bytes
        if ((nwItem->size = (size_t) strtoul(args[1], NULL, 10)) == 0) {
            printf("No se asignan bloques de 0 bytes\n");
            free(nwItem);
            return 1;
        }

        // Datos del item
        currentTime = time(NULL);
        nwItem->time = *localtime(&currentTime);
        nwItem->dir = (void *) malloc(nwItem->size);
        nwItem->type = MALLOC_MEM;
        nwItem->data.key = 0;
        nwItem->data.file_name = NULL;
        nwItem->data.file_descriptor = 0;

        // Comprobamos que podemos hacer la reservas
        if (nwItem->dir == NULL) {
            printf("Imposible obtener memoria con malloc: Cannot allocate memory\n");
            free(nwItem);
            return 1;
        }
        
        LlenarMemoria(nwItem->dir, nwItem->size, 0);

        if (!insertElement(memList, nwItem)) {
            printf("[!] Error: %s\n", strerror(ENOMEM));
            free(nwItem);
            return 1;
        }

        printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
    }

    return 1;
}
void * ObtenerMemoriaShmget(key_t clave, size_t tam, memory_item *item, List memList){
	void * p;
	int aux, id, flags = 0777;
	struct shmid_ds s;

	if(tam)     //tam distito de 0 indica crear
		flags = flags | IPC_CREAT | IPC_EXCL;

	if(clave==IPC_PRIVATE){  //no nos vale
		printf("La llave no sirve\n");
		errno=EINVAL;
		return NULL;

	}if((id=shmget(clave, tam, flags))==-1)
		return (NULL);

	if((p=shmat(id,NULL,0))==(void*)-1){
		aux=errno;

		if(tam)
			shmctl(id,IPC_RMID,NULL);

		errno=aux;
		return (NULL);
	}

	shmctl(id,IPC_STAT,&s);

	item->size = s.shm_segsz;
	item->dir = p;

	if(!insertElement(memList, item)){
		printf("[!] Error: %s\n", strerror(ENOMEM));
		free(item);
		return (NULL);
	}

	return (p);
}
int alloc_shared_block(key_t key, List memoryList){
	memory_item *nwItem = (memory_item *)malloc(sizeof(memory_item));
	time_t currentTime;
	

	if(nwItem==NULL){
		free(nwItem);
		return printf("[!] Error: %s\n", strerror(errno));
	}

	// Datos del item
	currentTime = time(NULL);
	nwItem->time = *localtime(&currentTime);
	nwItem->type = SHARED_MEM;
	nwItem->data.file_name = NULL;
	nwItem->data.file_descriptor = 0;
	nwItem->data.key = key;


	if(ObtenerMemoriaShmget(nwItem->data.key, 0, nwItem, memoryList)==NULL){
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)nwItem->data.key, strerror(errno));
		free(nwItem);
		return 1;
	}

	printf("Memoria compartida de clave %lu  en %p\n", (unsigned long) nwItem->size, nwItem->dir);
	return 1;
}
static void do_DeallocateDelkey(char *args){
	key_t clave;
	int id;
	char *key = args;

	if(key==NULL || (clave = (key_t)strtoul(key,NULL,10))==IPC_PRIVATE){
		printf("      delkey necesita clave_valida\n");
		return;
	}
	if((id=shmget(clave,0,0666))==-1){
		perror("shmget: imposible obtener memoria compartida");
		return;
	}

	if(shmctl(id,IPC_RMID,NULL)==-1)
		perror("shmctl: imposible eliminar memoria compartida\n");
}

int del_shared_block(key_t key, List memList){
	memory_item *infoData = NULL;
	Lpos auxPos;
	for(auxPos=firstElement(memList); auxPos!=NULL; auxPos=nextElement(memList, auxPos)){
		infoData = getElement(memList, auxPos);
		if(infoData->data.key==key){
			infoData = deletePosition(memList, auxPos);
			if(shmdt(infoData->dir)==-1)
				printf("[!] Error: %s\n", strerror(errno));
			free(infoData);
			break;
		}
	}
	return 1;
}

int allocate_nshared_block(key_t key, size_t size, List memList){
	memory_item *nwItem = (memory_item *)malloc(sizeof(memory_item));
	time_t currentTime;
	

	if(nwItem==NULL){
		free(nwItem);
		return printf("[!] Error: %s\n", strerror(errno));
	}

	// Datos del item
	currentTime = time(NULL);
	nwItem->time = *localtime(&currentTime);
	nwItem->type = SHARED_MEM;
	nwItem->data.file_name = NULL;
	nwItem->data.file_descriptor = 0;
	nwItem->size = size;
	nwItem->data.key = key;

	// Comprobamos si se pudo cerear la memoria compartida
	if(ObtenerMemoriaShmget(nwItem->data.key, nwItem->size, nwItem, memList) == NULL){
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) nwItem->data.key, strerror(errno));
		free(nwItem);
		return 1;
	}

	printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
	return 1;
}
int cmd_Shared(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    key_t llave;
    size_t size;

    if (lenArg == 1) {
        print_memList(SHARED_MEM);
    } else if(strcmp(args[1], "-free")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<3)	print_memList(SHARED_MEM);
		else if((llave=(key_t)strtoul(args[2], NULL, 10))!=0)		return del_shared_block(llave, memList);
		else			printf("[!] Error: %s\n", strerror(errno));
	
    } else if(strcmp(args[1], "-create")==0){
		if(lenArg<4)	print_memList(SHARED_MEM);
		else if((size = (size_t)strtoul(args[3], NULL, 10))==0){
			printf("No se asignan bloques de 0 bytes\n");
			return 1;
		}else if((llave = (key_t)strtoul(args[2], NULL, 10))==0){
			printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) llave, strerror(errno));
			return 1;
		// Comprobamos que haya un segmento de memoria compartido con la clave (key), en caso contrario lo asigna
		}else	return allocate_nshared_block(llave, size, memList);

    } else if(strcmp(args[1], "-delkey")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<3)	print_memList(SHARED_MEM);

		// Comprobante de que se intenta hacer una reserva de 0 bytes
		else if((llave=(key_t)strtoul(args[2], NULL, 10))!=0)		do_DeallocateDelkey(args[2]);
		else	printf("[!] Error: %s\n", strerror(errno));
	
    }else { if((llave = (key_t)strtoul(args[1], NULL, 10))==0){
			printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) llave, strerror(errno));
			return 1;
		// Comprobamos que haya un segmento de memoria compartido con la clave (key), en caso contrario lo asigna
	  } else	return alloc_shared_block(llave, memList);}
    return 1;
}

static void Recursiva(int n){
	char automatico[TAMANO];
	static char estatico[TAMANO];

	printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, (void *)&n, automatico, estatico);

	if(n>0)
		Recursiva(n-1);
}

static void *MapearFichero(char *fichero, int protection) {
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;

    time_t currentTime = time(NULL);

    // Crear una nueva instancia de memory_item
    memory_item *nwItem = (memory_item *)malloc(sizeof(memory_item));
    if (nwItem == NULL) {
        perror("Error al asignar memoria para nwItem");
        return NULL;
    }

    // Establecer valores iniciales en nwItem
    nwItem->dir = NULL;
    nwItem->size = 0;
    nwItem->type = MAPPED_MEM;
    nwItem->data.key = 0;
    nwItem->data.file_name = NULL;
    nwItem->data.file_descriptor = 0;

    if (protection & PROT_WRITE)
        modo = O_RDWR;

    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1) {
        perror("Error al obtener información del archivo o al abrirlo");
        free(nwItem);
        return NULL;
    }

    // Mapear el archivo
    p = mmap(NULL, s.st_size, protection, map, df, 0);
    if (p == MAP_FAILED) {
        perror("Error al mapear el archivo");
        close(df);
        free(nwItem);
        return NULL;
    }

    // Configurar los valores restantes en nwItem
    nwItem->dir = p;
    nwItem->size = s.st_size;
    nwItem->time = *localtime(&currentTime);
    nwItem->data.key = 0;

    // Duplicar el nombre del archivo
    nwItem->data.file_name = strdup(fichero);
    if (nwItem->data.file_name == NULL) {
        perror("Error duplicando el nombre del archivo");
        munmap(p, s.st_size);
        close(df);
        free(nwItem);
        return NULL;
    }

    nwItem->data.file_descriptor = df;

    // Insertar nwItem en la lista
    if (!insertElement(memList, nwItem)) {
        printf("[!] Error al insertar el elemento en la lista: %s\n", strerror(ENOMEM));
        free(nwItem->data.file_name);
        munmap(p, s.st_size);
        close(df);
        free(nwItem);
        return NULL;
    }

    return p;
}


static void do_AllocateMmap(char *fich, char *permisos){ 
	char *perm;
	void *p;
	int protection=0;

	if(fich==NULL){
		print_memList(MAPPED_MEM);
		return;

	}if((perm=permisos)!=NULL && strlen(perm)<4){
		if(strchr(perm,'r')!=NULL) protection|=PROT_READ;
		if(strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
		if(strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
	}	
	
	if((p=MapearFichero(fich,protection))==NULL)
		perror ("Imposible mapear fichero");
	else
		printf ("fichero %s mapeado en %p\n", fich, p);
}
int unmapped_memory_block(const char *file_name,  List memList) {
    Lpos auxPos;
    memory_item *infoData=NULL;

    for(auxPos=firstElement(memList); auxPos!=NULL; auxPos=nextElement(memList, auxPos)){
		infoData = getElement(memList, auxPos);
		if (infoData->data.file_name != NULL && file_name != NULL) {
			if(strcmp(infoData->data.file_name, file_name)==0) break;
		}
        }

    // Verificar si se encontró el bloque de memoria
    if (auxPos != NULL) {
        // Desmapear el bloque de memorias
        if ((infoData = deletePosition(memList, auxPos))!=NULL && munmap(infoData->dir, infoData->size) != -1) {    
            close(infoData->data.file_descriptor);        
            free(infoData->data.file_name);

            // Liberar el elemento
            free(infoData);
        } else {
            perror("Error al desmapear el bloque de memoria");
            return -1;
        }
    } else {
        printf("No se encontró un bloque de memoria con el nombre de archivo especificado.\n");
        return -1;
    }

    return 1;
}

int cmd_Mmap(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {

    if (lenArg == 1)
        print_memList(MAPPED_MEM);
    else if (strcmp(args[1], "-free") == 0) {
        if (lenArg <= 2) {
            print_memList(MAPPED_MEM);
        } else  
            return unmapped_memory_block(args[2], memList);
    } else {
        // Crear la memoria mapeada
        do_AllocateMmap(args[1], args[2]);
    }

    return 1;
}

static int LeerFichero(char *fich, void *p, int n){ 
    int nleidos,tam=n; 
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return -1;
    if (n==-1)
        tam= s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return (nleidos);
}

int cmd_Read(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
    if(lenArg<4){
	printf("Faltan parametros\n");
	return -1;
    }
    int t;
    char *ptr;
    int n = ((ssize_t)-1);
    long addr = strtoul(args[2],&ptr,16);
            
    n = atoi(args[3]);
            
    if((t=LeerFichero(args[1], (long *)addr, n))==-1)
      	perror("error de lectura");
    else printf("leidos %d bytes en %ld\n",t,addr);
     	
    return 1;
}

static int EscribirFichero(char *fich, void *p, int n){
    int nescritos,tam=n;
    int df, aux;
    df=open(fich,O_RDWR);
    if((nescritos=write(df,p,tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return (nescritos);
}

int cmd_Write(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){

    if(lenArg<4){
	printf("Faltan parametros\n");
	return -1;
    }

    int t;
    char *ptr;
    if((strcmp(args[1], "-o")==0)){
	creat(args[2], 0777);
       	long addr = strtoul(args[3],&ptr,16);
       	if((t=EscribirFichero(args[2], (long *)addr, atoi(args[4])))==-1)
      		perror("error de escritura");
        else printf("escritos %d bytes en %ld\n",t,addr);
    }
    else if(open(args[1],O_RDWR)==-1){
         creat(args[1], 0777);
         long addr = strtoul(args[2],&ptr,16);
         if((t=EscribirFichero(args[1], (long *)addr, atoi(args[3])))==-1)
    	        perror("error de escritura");
         else printf("escritos %d bytes en %ld\n",t,addr);
        }
    return 1;
}


int cmd_Memdump(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    int bytes = 25;
    char *address;
    unsigned char c;

    if (lenArg < 2 || args[1] == NULL) {
        printf("Se necesita una dirección\n");
        return 1;
    } else {
        if (sscanf(args[1], "0x%p", &address) == 0 || address == NULL) {
            fprintf(stderr, "%s no es una dirección válida\n", args[1]);
            return 1;
        } else {
            if (lenArg >= 3 && args[2] != NULL) {
                bytes = atoi(args[2]);
            }

            printf("Volcando %d bytes desde la dirección %p\n", bytes, address);

            for (int bytes2 = 0; bytes2 < bytes; bytes2 += 25) {
                for (int offset = bytes2; offset < bytes2 + 25 && offset < bytes; offset++) {
                    c = *(address + offset);
                    if (c >= 32 && c <= 127) {
                        printf(" %c ", c);
                    } else {
                        printf("  ");
                    }
                }
                printf("\n");
                for (int offset = bytes2; offset < bytes2 + 25 && offset < bytes; offset++) {
                    c = *(address + offset);
                    printf("%02X ", c);
                }
                printf("\n");
            }
            return 1;
        }
    }
}
void Do_pmap(){
   pid_t pid;       
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)//No hay pmap, probamos procstat FreeBSD
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  //probamos procmap OpenBSD
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) //probamos vmmap Mac-OS
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}

int n1=0,n2=0,n3=0,n4,n5,n6;	//variables glbales para memory


int cmd_Memory(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	register short blockp=0, funcp=0, varsp=0, allp=0, pmapp=0;
	register int i=1;

	// Variables para el programa
	int int_l=1;
	char char_l='A';
	float float_l=1.2;
	static int int_s=1;
	static char char_s='A';
	static float float_s=1.2;
	static int d,e,f;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-blocks")==0)		blockp=1;
			else if(strcmp(args[i], "-funcs")==0)	funcp=1;
			else if(strcmp(args[i], "-vars")==0)	varsp=1;
			else if(strcmp(args[i], "-all")==0)		{allp=1; break;}
			else if(strcmp(args[i], "-pmap")==0)	pmapp=1;
		}
		++i;
	}

	// Activar todas las flags
	if(allp){ blockp=1; funcp=1; varsp=1; pmapp=1; }

	if(varsp){
		printf("Variables locales\t%p, \t%p, \t%p\n", &int_l, &char_l, &float_l);
		printf("Variables globales:\t%p, %p, %p\n", &n1, &n2, &n3);
                printf("Var (N.I.)globales:\t%p, %p, %p\n", &n4, &n5, &n6);
		printf("Variables estaticas\t%p, \t%p, \t%p\n", &int_s, &char_s, &float_s);
                printf("Var (N.I.)estaticas:\t%p, %p, %p\n", &d, &e, &f);
	}
	if(funcp){
		printf("Funciones programa\t%p, \t%p, \t%p\n", &cmd_Malloc, &cmd_Memdump, &cmd_Recurse);
		printf("Funciones librería\t%p, \t%p, \t%p\n", &sscanf, &free, &printf);
	}
	if(blockp)
		print_memList(NOT_DEFINED);
	if(pmapp)
		Do_pmap();

	return SSUCC_EXIT;
}

void LlenarMemoria(void *p, size_t cont, unsigned char byte) {
    unsigned char *arr = (unsigned char *)p;
    size_t i;

    for (i = 0; i < cont; i++)
        arr[i] = byte;

    if (byte != 0) {
        printf("Llenando %ld bytes de memoria con el byte %c a partir de la direccion %p\n",
               (unsigned long)cont, byte, p);
    }
}

int cmd_Memfill(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg >1){
        
        size_t n = (unsigned long) 128;
        unsigned char  c = 'A';
        
        
        long addr = strtoul(args[1],NULL,16);
        
        if(lenArg==2){
        
        	LlenarMemoria((long *)addr,n,(unsigned char )c);
        }
        	
        if(lenArg >=3 && isdigit(*args[2])){
            long addr = strtoul(args[1],NULL,16);
            n=atoi(args[2]);
            if(lenArg==3) LlenarMemoria((long *)addr,n,c);
            
            else if(isdigit(*args[3])){ 
            c = atoi(args[3]);
            LlenarMemoria((long *)addr,n,c);}
            else{ 
            c = strtoul(args[3],NULL,16);
            LlenarMemoria((long *)addr,n,c);}         
        }   
    }    
    
    return 1;
}

int cmd_Recurse(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1) return 1;
	Recursiva(atoi(args[1]));
	return 1;
}


//P3
void freeProcessListItem(void *data){
	t_proc *item = (t_proc *)data;
	free(item->line);
	free(item);
}
char *t_stattoa(t_pstat stat){
	static char asign_name[9];
	strcpy(asign_name, "UNKNOWN");
	if(stat == FINISHED)	strcpy(asign_name, "FINISHED");
	if(stat == STOPPED)		strcpy(asign_name, "STOPPED");
	if(stat == SIGNALED)	strcpy(asign_name, "SIGNALED");
	if(stat == ACTIVE)		strcpy(asign_name, "ACTIVE");
	return asign_name;
}
int BuscarVariable(char *var, char *e[]){
	int pos=0;
	char aux[COMMAND_LEN];
	strcpy(aux,var);
	strcat(aux,"=");

	while(e[pos]!=NULL)
		if(!strncmp(e[pos],aux,strlen(aux)))
			return(pos);
		else
			pos++;

	errno=ENOENT;   // no hay tal variable
	return(-1);
}
int CambiarVariable(char *var, char *valor, char *e[]){
	int pos;
	char *aux;

	if((pos=BuscarVariable(var,e))==-1)
		return(-1);

	if((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
		return -1;
	strcpy(aux,var);
	strcat(aux,"=");
	strcat(aux,valor);
	e[pos]=aux;
	return (pos);
}

int external_functionality(const int argLen, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=0, j=0;
	int output_code=SSUCC_EXIT, prior=0;
	char *new_envp[COMMAND_LEN];
	static char linea[COMMAND_BUFFER]="";
	pid_t pid;
	t_proc *nwItem=NULL;
	time_t currentTime;
	currentTime = time(NULL);

	// Obteniendo las variables de entorno
	for(i=0; BuscarVariable(args[i], envp)!=-1; ++i)
		new_envp[i] = args[i];

	// Creando el proceso
	// EL proceso no se pudo crear correctamente
	if((pid=fork()) == -1)
		return report_error_exit(ESRCH);

	// Proceso hijo
	else if(pid==0){
		// Parseando el segundo plano del programa y la prioridad del programa 
		if(args[argLen-2] && args[argLen-2][0] == '@')
			args[argLen-2] = NULL;

		if(args[argLen-1] && (args[argLen-1][0] == '&' || args[argLen-1][0] == '@'))
			args[argLen-1] = NULL;

		if(i==0)	output_code = execvp(args[i], args+i);
		else		output_code = execvpe(args[i], args+i, new_envp);
		return SCSS_EXIT;	// Slida controlada de la shell

	// Proceso padre
	}else if(pid>0){
		if(args[argLen-1][0] == '&'){
			if((nwItem=(t_proc *)malloc(sizeof(t_proc)))==NULL){
				free(nwItem);
				return report_error_exit(ENOMEM);
			}

			// Línea del comando
			strcpy(linea, "");
			for(j=0; j<argLen-1; ++j){
				strcat(linea, args[j]);
				strcat(linea, " ");
			}

			// Parseando la prioridad del proceso
			if(args[argLen-2] && args[argLen-2][0]=='@'){
				args[argLen-2][0] = '0';
				prior = atoi(args[argLen-2]);
				args[argLen-2] = NULL;
			}

			// Crear el item
			nwItem->pid = pid;
			nwItem->user = getpwuid((uid_t)getuid())->pw_name;
                        nwItem->time = *localtime(&currentTime);
			nwItem->status = ACTIVE;
			nwItem->end = ACTIVE;
			nwItem->priority = prior;
			nwItem->line = strdup(linea);

			// Insertar en la lista de procesos el nuevo proceso
			if(!insertElement(processList, nwItem))
				printf("[!] Error: %s\n", strerror(ENOMEM));
		}else
			waitpid(pid, &output_code, 0);
	}

	return (output_code == SCSS_EXIT)? SSUCC_EXIT : FSUCC_EXIT ;
}
int print_environ_values(char *env[], char *env_type){
	register int i=0;
	for(i=0; environ[i]!=NULL; ++i)
		printf("%p->%s[%d]=(%p) %s\n", &env[i], env_type, i, env[i], env[i]);
	return SSUCC_EXIT;
}
int update_data(t_proc *item){
	int options = WNOHANG | WUNTRACED | WCONTINUED;
	int status;

	if(waitpid(item->pid, &status, options)==item->pid){
		if(WIFEXITED(status)){
			item->status = FINISHED;
			item->priority = -1;
			item->end = WEXITSTATUS(status);
		}else if(WIFCONTINUED(status)){
			item->status = ACTIVE;
		}else if(WIFSTOPPED(status)){
			item->status = STOPPED;
			item->end = WTERMSIG(status);
		}else if(WIFSIGNALED(status)){
			item->status = SIGNALED;
			item->end = WTERMSIG(status);
		}else
			item->status = UNKNOWN;
		return 1;
	}
	
	return 0;
}
void print_proc_dataItem(t_proc *item){
	printf("%6d %11s ", item->pid, item->user);
	printf("p=%d ", item->priority);
	printf("%4d/%2d/%2d %2d:%2d:%2d ", item->time.tm_year+1900, item->time.tm_mon+1, item->time.tm_mday, item->time.tm_hour, item->time.tm_min, item->time.tm_sec);
	printf("%s (%03d) %s\n", t_stattoa(item->status), item->end, item->line);
}
void imprimir_lista_procesos(List processList, pid_t pid){
	Lpos auxPos;
	t_proc *item = NULL;

	for(auxPos=firstElement(processList); auxPos!=NULL; auxPos=nextElement(processList, auxPos)){
		item = getElement(processList, auxPos);
		if(pid<0 || pid==item->pid){
			if(update_data(item))
				updateElement(processList, auxPos, item);
			print_proc_dataItem(item);
		}
	}
}
void del_process_by_status(List processList, t_pstat status){
	t_proc *item;
	Lpos auxPos=firstElement(processList);

	while(auxPos!=NULL){
		for(auxPos=firstElement(processList); auxPos!=NULL; auxPos=nextElement(processList, auxPos)){
			item = getElement(processList, auxPos);
			
			if(update_data(item))
				updateElement(processList, auxPos, item);
			
			if(item->status == status){
				item = deletePosition(processList, auxPos);
				freeProcessListItem(item);
				break;
			}
		}
	}
}

// ==================== PRÁCTICA 3 ====================
int cmdShowvar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	
	int pos;
    if(lenArg>1){
        if((pos=BuscarVariable(args[1],environ))==-1){
            perror("Imposible encontrar variable");
            return FSUCC_EXIT;
        }
        printf("Con arg3 main %s(%p) @%p\n",environ[pos],environ[pos],&envp[pos]);
        printf("  Con environ %s(%p) @%p\n",environ[pos],environ[pos],&environ[pos]);
        printf("   Con getenv %s(%p)\n",getenv(args[1]),&environ[pos]);
       
    }
    else{
        cmdShowenv(1,0,envp,historicList,memoryList,processList);}
    
    return SSUCC_EXIT;
}
int cmdChangevar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	char *aux=malloc(MAX_PATH);
    if(lenArg>1){
        if(lenArg==4){
            if(strcmp(args[1],"-a")==0){
                CambiarVariable(args[2],args[3],envp);
            }else if(strcmp(args[1],"-e")==0){
                CambiarVariable(args[2],args[3],environ);
            }else if(strcmp(args[1],"-p")==0){
                strcpy(aux,args[2]);
                strcat(aux,"=");
                strcat(aux,(args[3]));
                putenv(aux);
            }else
                printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
        }else
            printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
    }else
        printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
    
    free(aux);
    return SSUCC_EXIT;
}
int cmdShowenv(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)
		return print_environ_values(envp, "main args3");
	else if(strcmp(args[1], "-environ")==0)
		return print_environ_values(environ, "environ");
	else if(strcmp(args[1], "-addr")==0){
		printf("environ:   %p (almacenado en %p)\n", &environ[0], &environ);
		printf("main arg3: %p (almacenado en %p)\n", &envp[0], &envp);
		return SSUCC_EXIT;
	}

	printf("[!] Error: %s\n", strerror(EINVAL));
	return FSUCC_EXIT;
}
int cmdFork(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	int pid=fork();
	if(pid==0)
		printf("ejecutando proceso %d\n",getpid());
	else if(pid==-1){
		perror("[!] Error: fork no posible");
		return FSUCC_EXIT;}
	else waitpid(pid,NULL,0);
	return SSUCC_EXIT;
}
int cmdExecute(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=1;
	char *specific_environ[COMMAND_LEN];

	// Si solo se llama a esta función
	if(lenArg==1)
		return report_error_exit(EFAULT);

	// Coger el nuevo entorno
	for(i=1; BuscarVariable(args[i], envp)!=-1; ++i)
		specific_environ[i-1] = args[i];

	// Parseando el segundo plano del programa y la prioridad del programa 
	if(args[lenArg-2] && args[lenArg-2][0] == '@')
		args[lenArg-2] = NULL;

	if(args[lenArg-1] && (args[lenArg-1][0] == '&' || args[lenArg-1][0] == '@'))
		args[lenArg-1] = NULL;

	// Ejecución del programa
	if(i==1){
		if(execvp(args[i], args+i)==-1)
			printf("[!] Error: %s\n", strerror(errno));
	}else{
		if(execvpe(args[i], args+i, specific_environ)==-1)
			printf("[!] Error: %s\n", strerror(errno));
	}

	return SSUCC_EXIT;
}
int cmdListjobs(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	imprimir_lista_procesos(processList, -1);
	return SSUCC_EXIT;
}
int cmdDeljobs(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)
		imprimir_lista_procesos(processList, -1);

	else if(strcmp(args[1], "-term")==0)
		del_process_by_status(processList, FINISHED);
	
	else if(strcmp(args[1], "-sig")==0)
		del_process_by_status(processList, SIGNALED);

	else
		return report_error_exit(EINVAL);

	return SSUCC_EXIT;
}
int cmdJob(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList){
	Lpos auxPos;
	t_proc *item=NULL;
	int exit_status=0;
	pid_t pid;

	// En caso de que sea solo un argumento
	if(lenArg==1)
		imprimir_lista_procesos(processList, -1);

	// En caso que se desee pasar de segundo plano a primer plano
	else if(strcmp(args[1], "-fg")==0){
		if((pid=atoi(args[2]))!=0){
			// Pasamos a primer plano el proceso indicado
			waitpid(pid, &exit_status, 0);
			printf("Proceso %d terminado normalmente. Valor devuelto %d\n", pid, exit_status);

			// Eliminar de la lista
			for(auxPos=firstElement(processList); auxPos!=NULL; auxPos=nextElement(processList, auxPos)){
				item = getElement(processList, auxPos);
				if(pid == item->pid){
					deletePosition(processList, auxPos);
					break;
				}
			}
		}
		else return report_error_exit(EINVAL);
	
	// Obtener informacion concreta de un proceso
	}else if((pid=atoi(args[1]))!=0)
		imprimir_lista_procesos(processList, pid);

	return SSUCC_EXIT;
}

char *NombreUsuario(uid_t uid){
	struct passwd *p;

	if((p=getpwuid(uid))==NULL){
		return("??????");
	}
	return p->pw_name;
}

uid_t UidUsuario(char *nombre) {
    struct passwd *p;
    if ((p = getpwnam(nombre)) == NULL) {
        return (uid_t)-1;
    }
    return p->pw_uid;
}

// Función para imprimir las credenciales UID
void hgetuid() {
    uid_t real = getuid(), efec = geteuid();
    printf("Credencial real: %d, (%s)\n", real, getpwuid(real)->pw_name);
     printf("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario(efec));}
void hsetuid(char *args[]) {
    uid_t uid;
    int aux;

    if (args[2] != NULL) {
        if (strcmp(args[2], "-l") == 0) {
            if (args[3] != NULL) {
                if ((uid=UidUsuario(args[3]))==(uid_t) -1) {
                    printf("Usuario no existente %s\n", args[3]);
                    return;
                }
            } else {
                hgetuid();
                return;
            }
        } else {
            if ((uid = (uid_t)((aux = atoi(args[2])) < 0) ? -1 : aux) == (uid_t)-1) {
                printf("Valor no valido de la credencial %s\n", args[2]);
                return;
            }
        }
        if (seteuid(uid) == -1) {
            printf("Imposible cambiar credencial: %s\n", strerror(errno));
        }
    } else {
        hgetuid();
        return;
    }
}
// Función principal para el comando uid
int cmdUid(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    // Inserta aquí la lógica específica del comando uid
    if (lenArg < 2) {
        perror("Uso: uid [-get|-set] [-l] [id]");
        return EXIT_FAILURE;
    }

    if (strcmp(args[1], "-get") == 0) {
        hgetuid();
    } else if (strcmp(args[1], "-set") == 0) {
        hsetuid(args);
    } else {
        hsetuid(args);
    }

    return SSUCC_EXIT;  // Puedes cambiar esto según lo que desees devolver
}

int SustituirVariable(char *var1, char *var2, char *valor, char *e[]) {
    int pos;
    char *aux;

    if ((pos = BuscarVariable(var1, e)) == -1) {
        return -1;
    }

    if ((aux=(char *)malloc(strlen(var2)+strlen(valor)+2)) == NULL) {
        return -1;
    }
    strcpy(aux, var2);
    strcat(aux, "=");
    strcat(aux, valor);
    e[pos] = aux;
    return pos;
}

int cmdsubsvar(const int lenArg, char *args[COMMAND_LEN], char *envp[], List historicList, List memoryList, List processList) {
    if (lenArg < 5) {
        printf("Número de argumentos insuficiente.\n");
        return -1;
    }

    if (strcmp(args[1], "-a") == 0) {
        // Accede por el tercer arg de main
       SustituirVariable(args[2], args[3], args[4], envp);
    } else if (strcmp(args[1], "-e") == 0) {
        // Accede mediante environ
       SustituirVariable(args[2], args[3], args[4], environ);
    } else {
        printf("Uso: subsvar [-a|-e] var valor\n");
        return -1;
    }

    return SSUCC_EXIT;  
}
