#include <stdio.h>
#include <string.h>
#define MAX_CANDIDATOS 1000
#define MAX_VOTANTES 100000

typedef struct {
    int id;
    int votos;
    bool eliminado;
} Candidato;

typedef struct {
    int id;
    int candidato_id;
} Votante;

// Declaración de funciones
bool check_arguments(int argc, char **argv);
void init_votacion(int nc);
void registrar_voto(int votante_id, int candidato_id, FILE *output_file);
void conteo_parcial(FILE *output_file);
void conteo_total(FILE *output_file);
void conteo_rango(int min_votes, int max_votes, FILE *output_file);
void ordenar_candidatos(FILE *output_file);
void anular_voto(int votante_id, int candidato_id, FILE *output_file);
void eliminar_candidato(FILE *output_file);
void traspasar_exceso_votos(int candidato_id, int m, FILE *output_file);

Candidato candidatos[MAX_CANDIDATOS];
Votante votantes[MAX_VOTANTES];
int num_candidatos = 0;
int num_votantes = 0;

bool check_arguments(int argc, char **argv) {
    // Revisa si se proporcionaron los argumentos adecuados
    if (argc != 3) {
        printf("Uso: %s archivo_entrada archivo_salida\n", argv[0]);
        return false;
    }
    return true;
}

void init_votacion(int nc) {
    for (int i = 0; i < nc; i++) {
        candidatos[i].id = i;
        candidatos[i].votos = 0;
        candidatos[i].eliminado = false;
    }
    for (int i = 0; i < MAX_VOTANTES; i++) {
        votantes[i].id = i;
        votantes[i].candidato_id = -1; // Inicializa el votante con candidato_id = -1
    }
}
void registrar_voto(int votante_id, int candidato_id, FILE *output_file) {
    candidatos[candidato_id].votos++;
    votantes[votante_id].id = votante_id;
    votantes[votante_id].candidato_id = candidato_id;
    fprintf(output_file, "VOTO REGISTRADO %d\n", votante_id);
}


void conteo_parcial(FILE *output_file) {
    fprintf(output_file, "CONTEO-PARCIAL\n");
    for (int i = 0; i < num_candidatos; i++) {
        fprintf(output_file, "\tCANDIDATE %d\n", i);
        if (candidatos[i].votos == 0) {
            fprintf(output_file, "\t\tNO HAY VOTOS REGISTRADOS\n");
        } else {
            for (int j = 0; j <= num_votantes; j++) {
                if (votantes[j].candidato_id ==i) {
                    fprintf(output_file, "\t\tVOTE %d\n", votantes[j].id);
                }
            }
        }
    }
    fprintf(output_file, "TOTAL PARCIAL DE VOTOS: %d\n", num_votantes);
}



void conteo_total(FILE *output_file) {
    int total_votos = 0;

    fprintf(output_file, "CONTEO-TOTAL\n");
    for (int i = 0; i < num_candidatos; i++) {
        fprintf(output_file, "\tCANDIDATO %d: %d\n", i, candidatos[i].votos);
        total_votos += candidatos[i].votos;
    }
    fprintf(output_file, "TOTAL VOTOS: %d\n", total_votos);
}


void conteo_rango(int min_votes, int max_votes, FILE *output_file) {
    // Implementa la lógica para mostrar el conteo de votos en un rango específico
    fprintf(output_file, "CONTEO-RANGO %d %d\n", min_votes, max_votes);
    for (int i = 0; i < num_candidatos; i++) {
        if (candidatos[i].votos >= min_votes && candidatos[i].votos <= max_votes) {
            fprintf(output_file, "\tCANDIDATO %d: %d VOTOS\n", i, candidatos[i].votos);
        }
    }
}

void ordenar_candidatos(FILE *output_file) {
    fprintf(output_file, "CANDIDATOS-ORDENADOS\n");

    Candidato candidatos_copia[MAX_CANDIDATOS];
    memcpy(candidatos_copia, candidatos, sizeof(candidatos));
    int total_votos = 0;
    for (int i = 0; i < num_candidatos; i++) {
        int max_votos_idx = -1;
        int max_votos = -1;
        for (int j = 0; j < num_candidatos; j++) {
            if (!candidatos_copia[j].eliminado && candidatos_copia[j].votos > max_votos) {
                max_votos = candidatos_copia[j].votos;
                max_votos_idx = j;
            }
        }
        if (max_votos_idx != -1) {
            fprintf(output_file, "\tCANDIDATO %d: %d\n", candidatos_copia[max_votos_idx].id, candidatos_copia[max_votos_idx].votos);
            candidatos_copia[max_votos_idx].eliminado = true;
        }
        total_votos += candidatos[i].votos;
    }
    fprintf(output_file, "TOTAL DE VOTOS: %d\n", total_votos);
}


void anular_voto(int votante_id, int candidato_id, FILE *output_file) {
    // Implementa la lógica para anular un voto
    candidatos[candidato_id].votos--;
    votantes[votante_id].candidato_id = -1;
    fprintf(output_file, "VOTO ELIMINADO CORRECTAMENTE\n");
}

void eliminar_candidato(FILE *output_file) {
    // Implementa la lógica para eliminar el candidato con menos votos
    int min_votos = 1000000;
    int candidato_eliminar = -1;
    for (int i = 0; i < num_candidatos; i++) {
        if (!candidatos[i].eliminado && candidatos[i].votos < min_votos) {
            min_votos = candidatos[i].votos;
            candidato_eliminar = i;
        }
    }
    if (candidato_eliminar != -1) {
        candidatos[candidato_eliminar].eliminado = true;
        fprintf(output_file, "CANDIDATO %d HA SIDO ELIMINADO\n", candidato_eliminar);
    }
}

void traspasar_exceso_votos(int candidato_id, int m, FILE *output_file) {
    // Implementa la lógica para traspasar el exceso de votos
    fprintf(output_file, "TRASPASAR-EXCESO-VOTOS %d %d\n", candidato_id, m);

    if (candidatos[candidato_id].votos <= m) {
        fprintf(output_file, "NO HAY VOTOS EXCEDENTES PARA TRASPASAR\n");
        return;
    }

    int votos_traspasados = 0;
    for (int i = 0; i < num_votantes && votos_traspasados < m; i++) {
        if (votantes[i].candidato_id == candidato_id) {
            // Encuentra el siguiente candidato no eliminado
            int next_candidate_id = (candidato_id + 1) % num_candidatos;
            while (candidatos[next_candidate_id].eliminado) {
                next_candidate_id = (next_candidate_id + 1) % num_candidatos;
            }

            // Traspasa el voto
            votantes[i].candidato_id = next_candidate_id;
            candidatos[candidato_id].votos--;
            candidatos[next_candidate_id].votos++;
            votos_traspasados++;

            fprintf(output_file, "\tVOTO %d TRASPASADO DEL CANDIDATO %d AL CANDIDATO %d\n", votantes[i].id, candidato_id, next_candidate_id);
        }
    }
}
int main(int argc, char **argv) {
    if (!check_arguments(argc, argv)) {
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    int N_CANDIDATES;
    fscanf(input_file, "%d", &N_CANDIDATES);

    int N_EVENTS;
    fscanf(input_file, "%d", &N_EVENTS);

    init_votacion(N_CANDIDATES);
    num_candidatos = N_CANDIDATES;

    char command[32];

    for (int event = 0; event < N_EVENTS; event++) {
        fscanf(input_file, "%s", command);

        if (!strcmp(command, "REGISTRAR-VOTO")) {
            int voter_ID, candidate_ID;
            fscanf(input_file, "%d %d", &voter_ID, &candidate_ID);
            registrar_voto(voter_ID, candidate_ID, output_file);
            num_votantes++;
        } else if (!strcmp(command, "CONTEO-PARCIAL")) {
            conteo_parcial(output_file);
        } else if (!strcmp(command, "CONTEO-TOTAL")) {
            conteo_total(output_file);
        } else if (!strcmp(command, "CONTEO-RANGO")) {
            int min_votes, max_votes;
            fscanf(input_file, "%d %d", &min_votes, &max_votes);
            conteo_rango(min_votes, max_votes, output_file);
        } else if (!strcmp(command, "ORDENAR-CANDIDATOS")) {
            ordenar_candidatos(output_file);
        } else if (!strcmp(command, "ANULAR-VOTO")) {
            int voter_ID, candidate_ID;
            fscanf(input_file, "%d %d", &voter_ID, &candidate_ID);
            anular_voto(voter_ID, candidate_ID, output_file);
            num_votantes--;
        } else if (!strcmp(command, "ELIMINAR-CANDIDATO")) {
            eliminar_candidato(output_file);
        } else if (!strcmp(command, "TRASPASAR-EXCESO-VOTOS")) {
            int M, N;
            fscanf(input_file, "%d %d", &M, &N);
            traspasar_exceso_votos(M, N, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}

