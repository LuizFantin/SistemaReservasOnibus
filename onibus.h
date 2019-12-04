typedef struct
{
    int numero;
    int disponivel; // 1 para disponivel ou 0 para indisponivel.
}Poltrona;

typedef struct onibus
{
    int linha;
    char* horario;
    int lugaresDisponiveis;
    Poltrona mapa[4][10];
}Onibus;


