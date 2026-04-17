#include <stdio.h>      // Para entrada/salida estándar
#include <stdbool.h>    // Para usar tipo bool
#include <string.h>     // Para manejo de cadenas (strcmp, strcpy, strlen)
#include <ctype.h>      // Para funciones de caracteres (isdigit, isupper, etc)
#include <stdlib.h>     // Para atof (convertir string a float)
#include <time.h>       // Para manejo de tiempo y bloqueos
#include <unistd.h>     // Para sleep()

#define CLAVE_CIFRADO 42   // Constante usada para cifrar y descifrar PIN con XOR
#define MAX_USUARIOS 100   // Máximo de usuarios en el sistema

// Declaración de funciones que se usan en el programa
void cifrarPIN(char pin[]);
void guardarDatos(const char* nombreArchivo, char ids[][20], char pins[][10], float saldos[], char nombres[][30], char apellidos[][30], int cantidad);
void cargarDatos(const char* nombreArchivo, char ids[][20], char pins[][10], float saldos[], char nombres[][30], char apellidos[][30], int* cantidad);

void inicioSesion(char ids[][20], char pins[][10], int intentos[], bool bloqueados[], time_t bloqueosTiempo[], char id[20], char pin[10], int MaxUsuarios, char nombres[][30], char apellidos[][30], float saldos[]);
void mostrarMenu(char ids[][20], char pins[][10], float saldos[], int indiceUsuario); 
void consultarSaldo(float saldos[], int indiceUsuario);
void salir();
void retirarDinero(float saldos[], int indiceUsuario);
void depositarDinero(float saldos[], int indiceUsuario);
void cambiarPIN(char pins[][10], int indiceUsuario);
bool esIDValido(char id[]);
bool esPINValido(char pin[]);
bool esNumeroValido(char texto[]);

int main()
{
	int MaxUsuarios = 3;  // Número fijo de usuarios en esta ejercitación (puede cambiar)

	// Arrays para almacenar datos de usuarios (simula base de datos)
	char nombres[MaxUsuarios][30];
	char apellidos[MaxUsuarios][30];
	char ids[MaxUsuarios][20];
	char pins[MaxUsuarios][10];
	float saldos[MaxUsuarios];
	int intentos[MaxUsuarios] = {0, 0, 0};        // Contador de intentos de PIN fallidos
	bool bloqueados[MaxUsuarios] = {false, false, false};  // Usuarios bloqueados
	time_t bloqueosTiempo[MaxUsuarios] = {0};     // Tiempo en que se bloqueó cada usuario
	int cantidadUsuarios = 0;                      // Cantidad real de usuarios cargados

	char id[20];   // Para pedir ID al usuario
	char pin[10];  // Para pedir PIN al usuario
	
	// Cargar datos guardados de archivo
	cargarDatos("usuarios.txt", ids, pins, saldos, nombres, apellidos, &cantidadUsuarios);

	// Bucle principal del programa: inicia sesión y luego pregunta si se cierra o no
	while (1) {
		inicioSesion(ids, pins, intentos, bloqueados, bloqueosTiempo, id, pin, MaxUsuarios, nombres, apellidos, saldos);

		// Preguntar si quiere cerrar el programa
		char opcion;
		do {
			printf("\n¿Desea cerrar el programa? (s/n): ");
			scanf(" %c", &opcion);
			opcion = tolower(opcion);  // convertir a minúscula para facilitar comparación

			if (opcion != 's' && opcion != 'n') {
				printf("Opción inválida. Por favor ingrese 's' para salir o 'n' para continuar.\n");
			}
		} while (opcion != 's' && opcion != 'n');

		if (opcion == 's') break;  // Si el usuario quiere salir, terminar el programa
	}
	
	// Guardar datos actualizados al salir (por ejemplo cambios de PIN o saldo)
    guardarDatos("usuarios.txt", ids, pins, saldos, nombres, apellidos, cantidadUsuarios);
	return 0;
}


void inicioSesion(char ids[][20], char pins[][10], int intentos[], bool bloqueados[], time_t bloqueosTiempo[], char id[20], char pin[10], int MaxUsuarios, char nombres[][30], char apellidos[][30], float saldos[])
{
    int i, encontrado = -1;
    int intentosID = 0;

    printf("----- INICIO DE SESION -----\n");

    // Permite hasta 3 intentos para ingresar un ID válido y que exista en la base de datos
    while (intentosID < 3) {
        printf("Ingrese ID (solo números, mínimo 6 digitos): ");
        scanf("%19s", id);

        if (!esIDValido(id)) {   // Verifica formato correcto del ID
            printf("ID inválido. Debe contener solo números y tener al menos 6 digitos.\n");
            intentosID++;
            continue;  // Volver a pedir ID
        }

        // Buscar si el ID existe
        encontrado = -1;
        for (i = 0; i < MaxUsuarios; i++) {
            if (strcmp(id, ids[i]) == 0) {
                encontrado = i;
                break;
            }
        }

        if (encontrado == -1) {
            printf("Usuario no encontrado.\n");
            intentosID++;
            continue;  // Volver a pedir ID
        }

        // Verificar bloqueo por intentos fallidos de PIN
        if (bloqueados[encontrado]) 
		{
   			time_t ahora = time(NULL);  // Hora actual
    		double segundosPasados = difftime(ahora, bloqueosTiempo[encontrado]);

    		// Mientras no pasen 60 segundos, se muestra el tiempo restante para desbloquear
    		while (segundosPasados < 60) 
			{
    			printf("Cuenta bloqueada. Intente nuevamente en %.0f segundos.\n", 60 - segundosPasados);
    			sleep(5);  // Espera 5 segundos antes de volver a revisar
    			ahora = time(NULL);
   				segundosPasados = difftime(ahora, bloqueosTiempo[encontrado]);
			}
			// Desbloquea al usuario y resetea intentos
			bloqueados[encontrado] = false;
			intentos[encontrado] = 0;
			printf("Cuenta desbloqueada. Puede intentar ingresar.\n");

		}

        // Si ID válido y usuario encontrado, salir del ciclo para pedir PIN
        break;
    }

    if (intentosID >= 3) {
        printf("Demasiados intentos fallidos para ingresar el ID. Finalizando.\n");
        return;
    }

    // Mostrar nombre del usuario al iniciar sesión correctamente con ID
    printf("Bienvenido %s %s\n", nombres[encontrado], apellidos[encontrado]);

    // Permite hasta 3 intentos para ingresar el PIN correcto
    for (int t = 0; t < 3; t++) {
        printf("Ingrese PIN: ");
        scanf("%9s", pin);

        // Comparar PIN ingresado con el guardado (texto plano)
        if (strcmp(pin, pins[encontrado]) == 0) {
            intentos[encontrado] = 0; // Reinicia contador de intentos fallidos
            printf("Acceso concedido.\n");
            mostrarMenu(ids, pins, saldos, encontrado);  // Muestra menú principal
            return;
        } else {
            intentos[encontrado]++;
            printf("PIN incorrecto. Intento %d de 3\n", intentos[encontrado]);

            // Si supera los 3 intentos, bloquea usuario
            if (intentos[encontrado] >= 3) {
                bloqueados[encontrado] = true;
                printf("Cuenta bloqueada por demasiados intentos fallidos.\n");
                bloqueosTiempo[encontrado] = time(NULL);  // Marca tiempo bloqueo
				return;
            }
        }
    }

    // Si no pudo autenticarse, muestra mensaje
    printf("No se pudo autenticar. Intente más tarde.\n");
}


void mostrarMenu(char ids[][20], char pins[][10], float saldos[], int indiceUsuario) 
{
    int opcion;
    do {
        printf("\n------ MENU PRINCIPAL ------\n");
        printf("1. Consultar saldo\n");
        printf("2. Retirar dinero\n");
        printf("3. Depositar dinero\n");
        printf("4. Cambiar PIN\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        // Ejecuta la función según la opción elegida
        switch(opcion) {
            case 1:
            	consultarSaldo(saldos, indiceUsuario); 
                break;
            case 2:
            	retirarDinero(saldos, indiceUsuario); 
                break;
            case 3:
				depositarDinero(saldos, indiceUsuario); 
                break;
            case 4:
				cambiarPIN(pins, indiceUsuario); 
                break;
            case 5:
				salir(); 
                break;
            default:
                printf("Opcion invalida. Intente de nuevo.\n");
        }
    } while(opcion != 5);
}

void consultarSaldo(float saldos[], int indiceUsuario) 
{
    // Muestra el saldo del usuario actual
    printf("Su saldo disponible es: $%.2f\n", saldos[indiceUsuario]);
}

void salir() 
{
    printf("Sesion finalizada. Retire su tarjeta.\n");
}

void retirarDinero(float saldos[], int indiceUsuario) 
{
    float monto;
    char confirmar;
    
    char input[20];
	printf("Ingrese el monto a retirar: $");
	scanf("%s", input);

	if (!esNumeroValido(input))  // Validar que sea número válido (float positivo)
	{
    	printf("Entrada inválida. Solo se permiten números.\n");
    	return;
	}

	monto = atof(input);  // Convierte texto a número flotante

    if (monto <= 0) {
        printf("Monto inválido.\n");
        return;
    }

    if (monto > saldos[indiceUsuario]) {
        printf("Saldo insuficiente. Su saldo actual es $%.2f\n", saldos[indiceUsuario]);
    } else {
    	
    	printf("¿Confirmar la operación? (s/n): ");
		scanf(" %c", &confirmar);
		if (confirmar != 's' && confirmar != 'S') 
		{
		    printf("Operación cancelada.\n");
		    return;
		}

        saldos[indiceUsuario] = saldos[indiceUsuario] - monto;
        printf("Retiro exitoso. Nuevo saldo: $%.2f\n", saldos[indiceUsuario]);
    }
}

void depositarDinero(float saldos[], int indiceUsuario) 
{
    float monto;
    char confirmar;
    
    char input[20];
	printf("Ingrese el monto a depositar: $");
	scanf("%s", input);

	if (!esNumeroValido(input))  // Validar que sea número válido (float positivo)
	{
	    printf("Entrada inválida. Solo se permiten números.\n");
	    return;
	}

	monto = atof(input);  // Convierte texto a número flotante

    if (monto <= 0) {
        printf("Monto inválido.\n");
        return;
    }
    
    printf("¿Confirmar la operación? (s/n): ");
	scanf(" %c", &confirmar);
	if (confirmar != 's' && confirmar != 'S') 
	{
	    printf("? Operación cancelada.\n");
	    return;
	}

    saldos[indiceUsuario] = saldos[indiceUsuario] + monto;
    printf("Depósito exitoso. Nuevo saldo: $%.2f\n", saldos[indiceUsuario]);
}

void cambiarPIN(char pins[][10], int indiceUsuario) 
{
    char pinActual[10];
    char nuevoPin[10];
    char confirmar;

    printf("Ingrese su PIN actual: ");
    scanf("%s", pinActual);

    // Compara el PIN ingresado con el PIN almacenado (en texto plano)
    if (strcmp(pinActual, pins[indiceUsuario]) != 0) {
        printf("PIN incorrecto.\n");
        return;
    }

    printf("Ingrese el nuevo PIN (mínimo 6 caracteres, al menos una mayúscula, una minúscula y un número): ");
    scanf("%s", nuevoPin);

    // Valida formato del nuevo PIN
    if (!esPINValido(nuevoPin)) {
        printf("PIN inválido. Debe tener al menos 6 caracteres, incluir mayúsculas, minúsculas, números y no tener símbolos.\n");
        return;
    }

    // Verifica que el nuevo PIN no sea igual al actual
    if (strcmp(nuevoPin, pinActual) == 0) {
        printf("El nuevo PIN no puede ser igual al anterior.\n");
        return;
    }

    // Confirmación antes de cambiar el PIN
    printf("¿Confirmar la operación? (s/n): ");
    scanf(" %c", &confirmar);
    if (confirmar != 's' && confirmar != 'S') {
        printf("Operación cancelada.\n");
        return;
    }

    // Guarda el PIN nuevo (sin cifrar) en memoria
    strcpy(pins[indiceUsuario], nuevoPin);
    printf("PIN cambiado exitosamente.\n");
}


// Función que valida si el ID contiene solo números y tiene al menos 6 caracteres
bool esIDValido(char id[]) {
    int largo = strlen(id);
    if (largo < 6) return false;

    for (int i = 0; i < largo; i++) {
        if (!isdigit(id[i])) {
            return false; // Si algún carácter no es dígito, es inválido
        }
    }
    return true;
}

// Función que valida si el PIN cumple con las condiciones:
// mínimo 6 caracteres, al menos una mayúscula, una minúscula, un número, y sin símbolos
bool esPINValido(char pin[]) {
    int largo = strlen(pin);
    if (largo < 6) return false;

    bool tieneMayus = false;
    bool tieneMinus = false;
    bool tieneNumero = false;

    for (int i = 0; i < largo; i++) {
        char c = pin[i];
        
        if (isupper(c)) {
            tieneMayus = true;
        } else if (islower(c)) {
            tieneMinus = true;
        } else if (isdigit(c)) {
            tieneNumero = true;
        } else {
            return false; // Si hay algún símbolo o carácter inválido
        }
    }

    // Devuelve true solo si tiene mayúscula, minúscula y número
    return tieneMayus && tieneMinus && tieneNumero;
}

// Función que valida que una cadena sea un número válido (float positivo con máximo un punto decimal)
bool esNumeroValido(char texto[]) {
    int punto = 0;

    if (strlen(texto) == 0) return false;

    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '.') {
            punto++;
            if (punto > 1) return false;  // No más de un punto decimal
        } else if (!isdigit(texto[i])) {
            return false; // Si no es dígito ni punto, inválido
        }
    }

    return true;
}

// Función para cifrar y descifrar PIN usando XOR con clave
void cifrarPIN(char pin[]) {
    for (int i = 0; pin[i] != '\0'; i++) {
        pin[i] = pin[i] ^ CLAVE_CIFRADO;
    }
}

// Guarda los datos de los usuarios en un archivo (incluye cifrar PIN y guardarlo en hexadecimal)
void guardarDatos(const char* nombreArchivo,
                 char ids[][20], char pins[][10], float saldos[],
                 char nombres[][30], char apellidos[][30],
                 int cantidad) {
    FILE* archivo = fopen(nombreArchivo, "w");
    if (!archivo) {
        printf("Error al abrir el archivo para guardar datos.\n");
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        char pinCifrado[10];
        strcpy(pinCifrado, pins[i]);     // Copia el PIN sin cifrar
        cifrarPIN(pinCifrado);           // Lo cifra para guardar

        char pinHex[20] = {0};
        // Convierte cada byte cifrado a su representación hexadecimal
        for (int j = 0; pinCifrado[j] != '\0'; j++) {
            sprintf(pinHex + j*2, "%02X", (unsigned char)pinCifrado[j]);
        }

        // Guarda línea con ID, PIN cifrado en hex, saldo, nombre y apellido
        fprintf(archivo, "%s %s %.2f %s %s\n",
                ids[i], pinHex, saldos[i], nombres[i], apellidos[i]);
    }

    fclose(archivo);
}

// Carga datos desde archivo, descifra PINs y los guarda en memoria
void cargarDatos(const char* nombreArchivo,
                 char ids[][20], char pins[][10], float saldos[],
                 char nombres[][30], char apellidos[][30],
                 int* cantidad) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        *cantidad = 0;  // No hay archivo, no hay usuarios
        return;
    }

    int i = 0;
    while (i < MAX_USUARIOS) {
        char pinHex[20];
        // Lee una línea con formato esperado
        int leidos = fscanf(archivo, "%19s %19s %f %29s %29s",
                            ids[i], pinHex, &saldos[i], nombres[i], apellidos[i]);
        if (leidos != 5) break;

        // Convierte PIN cifrado en hex a bytes
        int len = strlen(pinHex);
        for (int j = 0; j < len; j += 2) {
            unsigned int byte;
            sscanf(pinHex + j, "%2X", &byte);
            pins[i][j/2] = (char)byte;
        }
        pins[i][len/2] = '\0';

        // Descifra PIN
        cifrarPIN(pins[i]);

        i++;
    }
    *cantidad = i;
    fclose(archivo);
}

