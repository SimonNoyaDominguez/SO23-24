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

#include "list.h"					//Libreria con las cabeceras de las listas

// Definiciones globales de la shell
#define COMMAND_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido
#define MAX_PATH 		1024		//Longitud máxima para el path
#define MAX_NAME_LEN	70			// Longitud máxima para nombres auxiliares del programa 'char[]'
#define TAMANO 2048

enum asign_type {NOT_DEFINED, MALLOC_MEM, SHARED_MEM, MAPPED_MEM};
typedef enum asign_type t_asign;



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



// Métodos del sistema
void printPrompt();
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine();
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]);

// Programas shell-in Build
int cmd_Autores(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Pid(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Carpeta(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Fecha(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Time(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Hist(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Comando(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Infosys(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Help(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Exit(const int lenArg, char *args[COMMAND_LEN]);
int cmd_ListOpen(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Open(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Close(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Dup(const int lenArg, char *args[COMMAND_LEN]);
// P1
int cmd_Create(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Stat(const int lenArg, char *args[COMMAND_LEN]);
int cmd_List(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Delete(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Deltree(const int lenArg, char *args[COMMAND_LEN]);
static int checkZeroPharam(char arg[COMMAND_LEN]);
//P2
int cmd_Malloc(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Shared(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Mmap(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Read(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Write(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Memdump(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Memory(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Memfill(const int lenArg, char *args[COMMAND_LEN]);
int cmd_Recurse(const int lenArg, char *args[COMMAND_LEN]);

// Tablas necesarias para la práctica

typedef struct{
	key_t key;				// Clave utilizada en share
	char *file_name;		// Nombre del archivo utilizado en mmap
	int file_descriptor;	// Redireccionador utilizado en mmap
} t_oinfo;

struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[COMMAND_LEN]);
};

struct mem_table_data{
	void *dir;				// Dirección de memoria
	size_t size;			// Tamaño de la reserva
	struct tm time;			// Tiempo de asignación
	t_asign type;			// Tipo de asignación
	t_oinfo data;			// Otra información
};
typedef struct mem_table_data memory_item;

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
int main(int argc, char const *argv[]) {
    // Crear la lista del histórico
    createList(&historicList);
    createList(&memList);
    obtenerInfoDescriptores();

    do{
		printPrompt();
		getCmdLine();
	}while(executeCommand(argLen, args)!=0);

    // Liberar la memoria de la lista del histórico
    deleteList(historicList, free);
    deleteList(memList,freeMemoryListItem);

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
void printPrompt(){
	printf("-> ");
}

// Sepra la línea en partes usando como delimitador los espacios(' '), saltos de línea('\n') y tabuladores('\t')
int TrocearCadena(char *line, char *tokens[]){
	int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}

// Línea donde el usuario introduce el comando, se guarda en el histórico y se comprueba si es válido
void getCmdLine(){
	// Comprobar que la lista está inicializada
	if(historicList==NULL)
		createList(&historicList);

	// Obteniendo el string de los comandos
	if(fgets(get_input, COMMAND_BUFFER, stdin)==NULL){
		printf("\n[!] Fin...\n");
		exit(0);
	}

	// Comprobación del comando introducido
	if(strcmp(get_input, "\n")!=0 && get_input!=NULL){
		char *token=strtok(get_input, "\n");
		strcpy(linea, token);

		// Insertar en la lista
		if(insertElement(historicList, strndup(linea, COMMAND_BUFFER))==0)
			printf("[!] Error: %s\n", strerror(12));

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);

	// En caso de no ser válido, la línea se vacía y el número de argumentos se pone a 0
	}else{
		strcpy(get_input, " ");
		strcpy(linea, " ");
		argLen = 0;
	}
}

// Método que ejecuta el comando introducido por el usuario
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]){
	int i=0;

	// Captador del salto de linea sin comando introducido
	if(argLen==0) return 1;

	// Obtenemos la posicion en la tabla de comandos la posicion del comando a ejecutar
	while (cmd_table[i].cmd_name != NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobamos de que no sea una posicion erronea
	if(cmd_table[i].cmd_name != NULL)

		// En caso de que no se haya implementado la funcion de dicho programa
		if(cmd_table[i].cmd_func == NULL)
			printf("[!] Error: %s\n", strerror(38));
		
		else
			return cmd_table[i].cmd_func(numTrozos, tokens);

	// En caso de que no se encuentre el comando en la lista
	else
		printf("[!] Error: %s\n", strerror(38));

	return 1;
}

// == PROGRAMAS SHELL-IN BUILD ==
int cmd_Autores(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Pid(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Carpeta(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1)
		return currentDirectory();

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(ENOENT));
	return 1;
}

int cmd_Fecha(const int lenArg, char *args[COMMAND_LEN]){
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


int cmd_Time(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Hist(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Comando(const int lenArg, char *args[COMMAND_LEN]){
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

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);
		return executeCommand(argLen, args);
	}

	print_Ncommands(-1);
	return 1;
}

int cmd_Infosys(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_ListOpen(const int lenArg, char *args[COMMAND_LEN]) {
	imprimirListaArchivos(listaArchivos); 
    return 1;
}



int cmd_Open(const int lenArg, char *args[COMMAND_LEN]) {
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

int cmd_Dup(const int lenArg, char *args[COMMAND_LEN]) {
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




int cmd_Close(const int lenArg, char *args[COMMAND_LEN]) {
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
int cmd_Help(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Exit(const int lenArg, char *args[COMMAND_LEN]){
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


int cmd_Create(const int lenArg, char *args[COMMAND_LEN]){
	
	char error [MAX_NAME_LEN] = "[!] Error";
	if(lenArg >1){
	
	char path [MAX_PATH];
	
	
	getcwd(path,sizeof(path));  //Obtener directorio de trabajo actual
	strcat(path,"/");           //Añadir /
	
	if(strcmp(args[1],"-f") == 0){		// Si se pasa con -f se crea un archivo
		if (lenArg<3)
			cmd_Carpeta(1,0);       //Si solo se pasa f devolvemos el directorio actual
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
	
	cmd_Carpeta(1,0);
	return 1;
	}

	return 1;
}



int cmd_Stat(const int lenArg, char *args[COMMAND_LEN]){
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
		cmd_Carpeta(1,0);

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


int cmd_List(const int lenArg, char *args[COMMAND_LEN]){
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
		cmd_Carpeta(1,0);

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

int cmd_Delete(const int lenArg, char *args[COMMAND_LEN]) {
    char error [MAX_NAME_LEN] = "[!] Error";
    
    if(lenArg >1){ 		//Borra el archivo o la carpeta
        for(int i=1; i<lenArg; i++){
            if(remove(args[i]) !=0){
                perror(error);               
            }
        }
    }else { 			// Muestra directorio actual
          cmd_Carpeta(1,0);
    }
    return 1;
}
int cmd_Deltree(const int lenArg, char *args[COMMAND_LEN]){
	
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
       cmd_Carpeta(1,0);
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
int cmd_Malloc(const int lenArg, char *args[COMMAND_LEN]) {
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
int cmd_Shared(const int lenArg, char *args[COMMAND_LEN]) {
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
        // Desmapear el bloque de memoria
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

int cmd_Mmap(const int lenArg, char *args[COMMAND_LEN]) {
    Lpos auxPos;

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

int cmd_Read(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Write(const int lenArg, char *args[COMMAND_LEN]){

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


int cmd_Memdump(const int lenArg, char *args[COMMAND_LEN]) {
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

int cmd_Memory(const int lenArg, char *args[COMMAND_LEN]){
	
    if(lenArg >1){
        
    	if(strcmp(args[1], "-vars")== 0){
                int x=0,y=0,z=0;
                static int a=0,b=0,c=0,d,e,f;

                printf("Variables locales:\t%p, %p, %p\n", &x, &y, &z);
                printf("Variables estaticas:\t%p, %p, %p\n", &a, &b, &c);
                printf("Var (N.I.)estaticas:\t%p, %p, %p\n", &d, &e, &f);
                printf("Variables globales:\t%p, %p, %p\n", &n1, &n2, &n3);
                printf("Var (N.I.)globales:\t%p, %p, %p\n", &n4, &n5, &n6);
               

            }if(strcmp(args[1], "-funcs")== 0){
                printf("Funciones programa:\t%p, %p, %p\n", cmd_Autores, cmd_Pid, cmd_Infosys);
                printf("Funciones libreria:\t%p, %p, %p\n", malloc, printf, strcmp);

            }if(strcmp(args[1], "-blocks")== 0){
                print_memList(NOT_DEFINED);

            }else if(strcmp(args[1], "-all")== 0){
                
                char *input[COMMAND_LEN] = {" ","-vars"};
                cmd_Memory(2,input);
                char *input2[COMMAND_LEN] = {" ","-funcs"};
                cmd_Memory(2,input2);
                char *input3[COMMAND_LEN] = {" ","-blocks"};
                cmd_Memory(2,input3);

            }else if(strcmp(args[1], "-pmap")== 0){
                Do_pmap();
            }
        
    }else{
        char *input[COMMAND_LEN] = {" ","-all"};
        cmd_Memory(2,input);
        
    }
    
	return 1;
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

int cmd_Memfill(const int lenArg, char *args[COMMAND_LEN]){
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

int cmd_Recurse(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1) return 1;
	Recursiva(atoi(args[1]));
	return 1;
}
