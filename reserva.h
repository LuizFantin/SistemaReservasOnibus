typedef struct reserva
{
    int identificacao;
    int linha;
    char horario[6];
    int poltrona;
    int apagada; //1 para apagado e 0 para valida.
}Reserva;
