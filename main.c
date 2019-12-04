#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "onibus.h"
#include "reserva.h"

#define TAM 100

Onibus* linha;
Reserva* reservas;
int qtdHorarios;
int qtdReservas; //consequentemente o valor do identificador da última reserva
char nomeArquivo[TAM];

Onibus* buscarOnibus(char* horario){
	int i;
	for(i=0;i<qtdHorarios;i++)
		if(strcmp(linha[i].horario,horario) == 0)
			return &linha[i];
	return NULL;
}

Poltrona* buscarPoltrona(Onibus* onibus, int numero){
	numero--;
	return &(onibus->mapa[numero%4][numero/4]);
}

void inicializaMapa(Onibus* onibus)
{
	int i,j,numPoltrona;

	numPoltrona = 1;

	for(i=0;i<10;i++){
		for(j=0;j<4;j++){
			onibus->mapa[j][i].numero = numPoltrona;
			onibus->mapa[j][i].disponivel = 1;
			numPoltrona++;
		}
	}
}

int menu(){
	int opcao;
	printf("\nEscolha uma opcao:\n");
	printf("1. Criar Reserva;\n");
	printf("2. Apagar Reserva;\n");
	printf("3. Ver Reserva;\n");
	printf("4. Ver Mapa de assentos;\n");
	printf("5. Salvar alteracoes;\n");
	printf("0. Sair do programa;\n");
	printf("Insira a opcao que deseja: ");
	scanf("%d",&opcao);
	fflush(stdin);
	return opcao;
}

//Solicitar a quantidade de horários, alocar os onibus e criar arquivo.
void criarNovaLinha(int tipo){
	int i;
	if(tipo == 1)
	{
		printf("Digite o nome do arquivo (com a extensao Exemplo: teste.txt)\n");
    	fgets(nomeArquivo,100,stdin);
    	fflush(stdin);
		nomeArquivo[strlen(nomeArquivo)-1] = '\0';
	}
	printf("Insira a quantidade de horarios a serem cadastrados:");
	scanf("%d",&qtdHorarios);
	fflush(stdin);
	free(linha);
	linha = (Onibus*)malloc(sizeof(Onibus)*qtdHorarios);
	
	for (i = 0; i < qtdHorarios; i++){
		printf("Digite o horario %d (formato: hh:mm):  ",i+1);
		linha[i].horario = (char*)malloc(6);
		scanf("%s",linha[i].horario);
		fflush(stdin);
		linha[i].linha = 1;
		linha[i].lugaresDisponiveis = 40;
		inicializaMapa(&linha[i]);
	}
	qtdReservas = 0;
	reservas = (Reserva*)malloc(sizeof(Reserva));
}

void carregarLinha()
{
    int tam,i;
    FILE* pont_arq;

    printf("Digite o nome do arquivo (com a extensao Exemplo: teste.txt)\n");
    fgets(nomeArquivo,100,stdin);
    fflush(stdin);
	nomeArquivo[strlen(nomeArquivo)-1] = '\0';
    pont_arq = fopen(nomeArquivo,"r");

    if(pont_arq == NULL){
		printf("Arquivo inexistente, criando nova linha...\n");
        criarNovaLinha(0);
	}
	else
	{
		fscanf(pont_arq,"%d\n",&qtdHorarios);
		linha = (Onibus*)malloc(sizeof(Onibus)*qtdHorarios);

		for (i = 0; i < qtdHorarios; i++)
		{
			linha[i].horario = (char*)malloc(6);
			fscanf(pont_arq,"%s\n",linha[i].horario);
			linha[i].linha = 1;
			linha[i].lugaresDisponiveis = 40;
			inicializaMapa(&linha[i]);
		}

		fscanf(pont_arq,"%d\n",&qtdReservas);
		reservas = (Reserva*)malloc(sizeof(Reserva)*qtdReservas);
		char horario[6];
		for (i = 0; i < qtdReservas; i++)
		{
			fscanf(pont_arq,"%d\n", &reservas[i].identificacao);
			fscanf(pont_arq,"%d\n", &reservas[i].linha);
			fscanf(pont_arq,"%s\n", horario);
			fscanf(pont_arq,"%d\n",&reservas[i].poltrona);
			fscanf(pont_arq,"%d\n",&reservas[i].apagada);
			strcpy(reservas[i].horario,horario);
			Onibus* onibus = buscarOnibus(reservas[i].horario);
			Poltrona* poltrona = buscarPoltrona(onibus,reservas[i].poltrona);
			onibus->lugaresDisponiveis--;
			poltrona->disponivel = 0;
		}
		
		fclose(pont_arq);
		printf("\nInformacoes carregadas com sucesso\n");
	}
}

void mostrarMapa(int linha, Onibus onibus){
	int i,j,numPoltrona;
	printf("Onibus linha:%d, horario:%s\n\n",linha,onibus.horario);

	for(i=0;i<4;i++){
		numPoltrona = i+1;
		for(j=0;j<10;j++){
			printf("%d",numPoltrona);
			if (numPoltrona == 9)
				printf(" ");
			if(buscarPoltrona(&onibus,numPoltrona)->disponivel)
				printf(" | ");
			else
				printf("*| ");
			numPoltrona += 4;
		}
		printf("\n");
	}
}

Reserva* criarReserva(){
	char horario[TAM];
	Onibus* onibus;
	int numPoltrona;
	Poltrona* poltrona;
	//entrada do horário a ser reservado
	printf("Insira o horario da sua reserva (formato hh:mm): \n");
	do{
		scanf("%s",horario);
		fflush(stdin);//comando para limpar o buffer de leitura
		onibus = buscarOnibus(horario);
	}while(onibus == NULL);

	//entrada da poltrona a ser reservada
	printf("Digite o numero da sua poltrona: \n");
	mostrarMapa(1,*onibus);
	scanf("%d",&numPoltrona);
	fflush(stdin);
	poltrona = buscarPoltrona(onibus,numPoltrona);
	
	//verificação de disponibilidade da poltrona
	while(numPoltrona <= 0 || numPoltrona > 40 || poltrona->disponivel != 1){
		printf("Poltrona Indisponivel, digite novamente: ");
		scanf("%d",&numPoltrona);
		fflush(stdin);
		poltrona = buscarPoltrona(onibus, numPoltrona);
	}
	
	//alterando outras variáveis relacionadas
	onibus->lugaresDisponiveis--;
	poltrona->disponivel = 0;
	//definição da reserva
	Reserva* reserva = (Reserva*)malloc(sizeof(Reserva));
	qtdReservas++;
	reserva->identificacao = qtdReservas;
	reserva->linha = 1;
	int tamanhoHorario = strlen(horario);
	//reserva->horario = (char*)malloc(tamanhoHorario);
	memcpy(reserva->horario,horario,6);
	reserva->poltrona = poltrona->numero;
	reserva->apagada = 0;
	return reserva;
}

int excluirReserva(Reserva* reserva){
	if (reserva == NULL)//se não houver reserva não exclui
		return 0;
	//busca variáveis relacionadas a reserva
	Onibus* onibus = buscarOnibus(reserva->horario);
	Poltrona* poltrona = buscarPoltrona(onibus, reserva->poltrona);
	
	poltrona->disponivel = 1;
	onibus->lugaresDisponiveis++;
	
	//desabilitando reserva
	reserva->apagada = 1;
	return 1;
}

Reserva* procurarReserva(int id){
	int i;
	for(i=0;i<qtdReservas;i++){
		if(reservas[i].identificacao == id && reservas[i].apagada == 0)
			return &reservas[i];
	}
	return NULL;
}

void salvarAlteracoes()
{
	int i;
    FILE* pont_arq;
	
	
    pont_arq = fopen(nomeArquivo,"w");
    
	if (pont_arq == NULL)
	{
		printf("ERRO! O arquivo não foi aberto!\n");
		return;
	}
    fprintf(pont_arq,"%d\n",qtdHorarios);

    for (i = 0; i < qtdHorarios; i++)
        fprintf(pont_arq,"%s\n",linha[i].horario);
	

	fprintf(pont_arq,"%d\n",qtdReservas);

	for (i = 0; i < qtdReservas; i++){
		fprintf(pont_arq,"%d\n",reservas[i].identificacao);
		fprintf(pont_arq,"%d\n",reservas[i].linha);
		reservas[i].horario[strlen(reservas[i].horario)] = '\0';
		fprintf(pont_arq,"%s\n",reservas[i].horario);
		fprintf(pont_arq,"%d\n",reservas[i].poltrona);
		fprintf(pont_arq,"%d\n",reservas[i].apagada);
		//fprintf(pont_arq,"%d , %d , %s , %d , %d\n",reservas[i].identificacao,reservas[i].linha,reservas[i].horario,reservas[i].poltrona,reservas[i].apagada);
	}
	fclose(pont_arq);
	printf("\nAlteracoes salvas com sucesso!\n");
}

int main()
{
	int operacao, repetir = 1;
	printf("Deseja criar uma nova linha ou usar a criada previamente? (1 - nova, 2 - antiga)");
	scanf("%d",&operacao);
	fflush(stdin);
	if (operacao == 1)
		criarNovaLinha(1);
	else if(operacao == 2)
		carregarLinha();
	else{
		printf("\nOperacao invalida, fechando programa!\n");
		repetir = 0;
	}
	
	Onibus* onibus;
	Poltrona* poltrona;
	Reserva* reserva;
	int identificador;
	char horario[TAM];
	while (repetir){
		operacao = menu();
		switch (operacao){
		case 1: //criar Reserva
			reserva = criarReserva();
			reservas[qtdReservas-1] = *reserva;
			//reservas = (Reserva*)realloc(reservas,qtdReservas+1);
			//free(reserva);
			printf("\nReserva criada com sucesso\n");
			break;
		
		case 2: //excluir Reserva
			printf("Insira a identificacao da reserva a ser excluida: ");
			scanf("%d",&identificador);
			fflush(stdin);
			if(excluirReserva(procurarReserva(identificador)))
				printf("\nReserva %d Excluida com sucesso\n",identificador);
			else
				printf("Nao foi possivel excluir\n");

			break;
			
		case 3: //ver Reserva
			printf("Insira a identificacao da reserva buscada: ");
			scanf("%d",&identificador);
			fflush(stdin);
			reserva = procurarReserva(identificador);
			if (reserva == NULL){
				printf("A reserva buscada não foi encontrada\n");
				break;
			}
			printf("\nId: %d, linha: %d, horario: %s, poltrona: %d\n\n", reserva->identificacao,reserva->linha,reserva->horario,reserva->poltrona);
			break;

		case 4: //ver mapa
			printf("Insira o horario do onibus que deseja ver o mapa de assentos:");
			scanf("%s",horario);
			fflush(stdin);
			onibus = buscarOnibus(horario);
			if (onibus == NULL){
				printf("\nHorario buscado nao encontrado\n");
				break;
			}
			mostrarMapa(1,*onibus);
			break;
			
		case 5: //Salvar alterações
			salvarAlteracoes();
			break;
			
		case 0: //sair
			repetir = 0;
			salvarAlteracoes();
			printf("\n\nSaindo...\n\n");
			break;
		default:
			printf("Operacao invalida\n");
			break;
		}
	}
	
	
    return 0;
}
