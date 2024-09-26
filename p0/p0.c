//Autores: Ignacio Dosil Francesch, ignacio.dosil@udc.es
//	   Simon Noya Domínguez, simonnoyad@udc.es


#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <fcntl.h>
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <errno.h>					// Librería de captador de errores
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]

// Definiciones globales de la shell
#define COMMAND_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido

// Definicion de tipos de la lista
typedef struct node *Lpos;
typedef struct node *List;
struct node {
	char comando[COMMAND_BUFFER];
	struct node *next;
};

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
List historicList;					// Lista del histórico de comandos

// Métodos de la implementacion de la lista
List createList();											// Devuelve una lista vacía
int isEmptyList(List l);									// Comprueba si la lista está vacía
Lpos firstElement(List l);									// Devuelve la primera posicion de la lista
Lpos nextElement(List l, Lpos p);							// Devuelve la siguiente posicion de la lsita si es válida
int insertElement(List l, char element[COMMAND_BUFFER]);	// Inserta un nuevo elemento al final de la lista
void getElement(List l, Lpos p, char *element);				// Devuelve el contenido de la lista en 'element'
void clearList(List l);										// Elimina todos los elementos de la lista
void deleteList(List l);									// Elimina la lista pasada por referencia

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


// Tablas necesarias para la práctica
struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[COMMAND_LEN]);
};
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
    historicList = createList();
    
    obtenerInfoDescriptores();

    do{
		printPrompt();
		getCmdLine();
	}while(executeCommand(argLen, args)!=0);

    // Liberar la memoria de la lista del histórico
    deleteList(historicList);

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

// === IMPLEMENTACION DE LA LISTA DEL HISTORICO ===
// Crea una lista vacía
List createList(){
	struct node *l = (struct node *)malloc(sizeof(struct node));
	l->next=NULL;
	return l;
}

// Comprueba si la lista está vacía
int isEmptyList(List l){
	return (l->next==NULL)? 1 : 0; 
}

// Retorna el primer elemento de la lista
Lpos firstElement(List l){
	return l->next;
}

// Retorna el siguiente elemento si es válido
Lpos nextElement(List l, Lpos p){
	return (p->next==NULL)? NULL : p->next;
}

// Inserta un elemento en la lista
int insertElement(List l, char element[COMMAND_BUFFER]){
	struct node *nwPos = (struct node *)malloc(sizeof(struct node));
	struct node *auxPos;
	
	if(nwPos==NULL) return 0;

	// Compia de un duplicado del elemento a introducir
	strcpy(nwPos->comando, strndup(element, COMMAND_BUFFER));
	nwPos->next=NULL;

	if(l->next==NULL)
		l->next=nwPos;
	else{
		for(auxPos=l->next; auxPos->next!=NULL; auxPos=auxPos->next);
		auxPos->next=nwPos;
	}

	return 1;
}

// Retorna el elemento de la posicion en element
void getElement(List l, Lpos p, char *element){
	if(p==NULL || l->next==NULL) return;
	strcpy(element, p->comando);
}

// Elimina todos los elementos de la lista
void clearList(List l){
	struct node *auxPos;

	while(l->next!=NULL){
		auxPos = l->next;
		l->next = auxPos->next;
		auxPos->next=NULL;
		free(auxPos);
	}
}

// Elimina la lista
void deleteList(List l){
	clearList(l);
	l->next=NULL;
	free(l);
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
	if(isEmptyList(historicList)==1)
		historicList = createList();

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
		if(insertElement(historicList, linea)==0)
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
	if(lenArg==1){
		char path[COMMAND_BUFFER];
		if(getcwd(path, COMMAND_BUFFER)==NULL)
			printf("[!] Error: %s\n", strerror(20));
		else
			printf("%s\n", path);
		return 1;
	}

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(2));
	return 1;
}

// ====== CODIGO OFUSCADO ======
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
	char comm[COMMAND_BUFFER] = "";
	Lpos auxPos;
	int iter=0;

	if(n>0){
		for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos)){
			getElement(historicList, auxPos, comm);
			printf("%d->%s\n", iter, comm);
		}

	}else{
		for(auxPos=firstElement(historicList); auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos)){
			getElement(historicList, auxPos, comm);
			printf("%d->%s\n", iter, comm);
		}
	}
}

int cmd_Hist(const int lenArg, char *args[COMMAND_LEN]){
	char *num;
	int n=0;

	if(lenArg==1){
		print_Ncommands(-1);
		return 1;
	}

	if(strcmp(args[1], "-c")==0){
		clearList(historicList);
		return 1;
	}

	if ((num=strtok(args[1],"-"))==NULL){
		printf("[!] Error: %s\n", strerror(22));
		return 1;
	}

	if((n=atoi(num))!=0)
		print_Ncommands(n);
	else
		printf("[!] Error: %s\n", strerror(22));
	
	return 1;
}

int cmd_Comando(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg>1){
		int iter=0, nCommand = atoi(args[1]);
		
		if(nCommand<=0 && strcmp(args[1], "0")!=0){
			printf("[!] Error: %s\n", strerror(22));
			return 1;
		}

		Lpos auxPos;
		char comando[COMMAND_BUFFER] = "";
		for(auxPos=firstElement(historicList); iter<nCommand && auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos));

		// Comprobar la salida del bucle
		if(auxPos==NULL){
			printf("[!] Error: %s\n", strerror(95));
			return 1;
		}

		getElement(historicList, auxPos, comando);
		strcpy(linea, comando);
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


