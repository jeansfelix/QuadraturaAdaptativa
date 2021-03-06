#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "../libs/timer.h"

typedef struct _ARG_Thread{
    int tid;
    long double erro;
}ARG_Thread;

typedef struct _intervalo{
    long double inicio, fim;
}INTERVALO;

INTERVALO *intervalos;

int numThreads, numThreadsOciosas, numIntervalos;
int start;

long double (*funcao)(long double);
long double *somas_parciais;

pthread_mutex_t mutex;
pthread_cond_t cond_pegaIntervalo;

//Retorna o valor absoluto do valor passado.
long double absoluto(long double valor);

//Funções de ajuda para o usuário
void listarFuncoes();
void ajuda(char *prog);

//Funções que foram implementadas para se calcular a integral
long double umMaisX(long double x);
long double raizDeUmMaisXAoQuadrado(long double x);
long double raizDeUmMaisXElevadoAQuartaPotencia(long double x);
long double senoXAoQuadrado(long double x);
long double cincoMilesimosVezesXAoCuboMaisUm_VezesCossenoDeExpMenosX(long double x);

int produzIntervalo(long double inicio, long double fim) 
{
	INTERVALO novo = {inicio,fim};

	if (numIntervalos < numThreads) 
	{
		intervalos[numIntervalos] = novo;
		numIntervalos++;
		return 1;
	}
	else 
	{
		return 0;
	}
}

INTERVALO pegaIntervalo()
{
	printf("%d  ", numIntervalos);
	numIntervalos--;
	printf("%.20Lf , %.20Lf\n", intervalos[numIntervalos].inicio, intervalos[numIntervalos].fim);
	return intervalos[numIntervalos];
}

void *calculaIntervalo(void *arg)
{
    ARG_Thread argThread = *(ARG_Thread*) arg;
    
    long double pontoMedio_InicioFim, pontoMedio_InicioMeio, pontoMedio_MeioFim;
    long double valorFunc_PontoMedio_InicioFim, valorFunc_PontoMedio_InicioMeio, valorFunc_PontoMedio_MeioFim;
    long double intervalo_InicioFim, intervalo_InicioMeio, intervalo_MeioFim;
    long double areaAprox_InicioFim, areaAprox_InicioMeio, areaAprox_MeioFim;

    long double erroAtual;
    long double inicioAtual, fimAtual;
    long double somaPartesInicioAteFim;
    
    long int iteracoes = 0;
    
    INTERVALO intervalo;

	while (1) 
	{
		pthread_mutex_lock(&mutex);
		numThreadsOciosas++;
	
		while (numIntervalos == 0)
		{
			if (numThreadsOciosas == numThreads && start == 0) 
			{
				printf("intervalos %d,  t%d = %ld\n", numIntervalos, argThread.tid, iteracoes);
				pthread_cond_broadcast(&cond_pegaIntervalo);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}
			pthread_cond_wait(&cond_pegaIntervalo, &mutex);
		}

		start = 0;
		intervalo = pegaIntervalo();

		inicioAtual = intervalo.inicio;
    	fimAtual = intervalo.fim;
		somaPartesInicioAteFim = 0.0L;
		
		numThreadsOciosas--;
		pthread_mutex_unlock(&mutex);

		while(inicioAtual != intervalo.fim)
		{   
		    iteracoes++;
		    
		    pontoMedio_InicioFim    = (fimAtual + inicioAtual) / 2.0L;
		    pontoMedio_InicioMeio   = (inicioAtual + pontoMedio_InicioFim) / 2.0L;
		    pontoMedio_MeioFim      = (pontoMedio_InicioFim + fimAtual) / 2.0L;

		    intervalo_InicioFim     = fimAtual - inicioAtual;
		    intervalo_InicioMeio    = pontoMedio_InicioFim - inicioAtual;
		    intervalo_MeioFim       = fimAtual - pontoMedio_InicioFim;

		    valorFunc_PontoMedio_InicioFim  = (funcao)(pontoMedio_InicioFim);
		    valorFunc_PontoMedio_InicioMeio = (funcao)(pontoMedio_InicioMeio);
		    valorFunc_PontoMedio_MeioFim    = (funcao)(pontoMedio_MeioFim);

		    areaAprox_InicioFim     = intervalo_InicioFim * valorFunc_PontoMedio_InicioFim;
		    areaAprox_InicioMeio    = intervalo_InicioMeio * valorFunc_PontoMedio_InicioMeio;
		    areaAprox_MeioFim       = intervalo_MeioFim * valorFunc_PontoMedio_MeioFim;

		    erroAtual = absoluto(areaAprox_InicioFim - (areaAprox_InicioMeio + areaAprox_MeioFim));
		    
		    if (erroAtual <= argThread.erro)
		    {
		        //Soma a area da parte atual e avança para a próxima parte não calculada.
		        somaPartesInicioAteFim  += areaAprox_InicioFim;
		        
		        inicioAtual = fimAtual;
		        fimAtual = intervalo.inicio + (2.0L * (inicioAtual - intervalo.inicio));
		        
		        if ( fimAtual >= intervalo.fim ) 
		        {
		            fimAtual = intervalo.fim;
		        }
		    }
		    else 
		    {
		        fimAtual = pontoMedio_InicioFim;
				
				pthread_mutex_lock(&mutex);
				if (numThreadsOciosas > 0) 
				{
					if (produzIntervalo(fimAtual, intervalo.fim))
					{
						pthread_cond_signal(&cond_pegaIntervalo);
						intervalo.fim = fimAtual;
					}
				}
				pthread_mutex_unlock(&mutex);
		    }
		}
		
		somas_parciais[argThread.tid] += somaPartesInicioAteFim;
	}
    
    pthread_exit(NULL);
}

long double calculaIntegral(long double inicio, long double fim, long double erro, long double (*func)(long double))
{
    int i;
    ARG_Thread *arg;

    pthread_t tidSistema[numThreads];
    
    long double somaPartesInicioAteFim = 0.0L;
    
    arg = (ARG_Thread *) malloc(numThreads * sizeof(ARG_Thread));
    
    funcao = func;

    numThreadsOciosas = 0;
	numIntervalos = 0;

	intervalos = (INTERVALO*) malloc(numThreads * sizeof(INTERVALO));
    
    for (i = 0; i < numThreads; i++)
    {
        arg[i].tid = i;
        arg[i].erro = erro;
        
        pthread_create(&tidSistema[i], NULL, calculaIntervalo, (void*)&arg[i]);
    }

    pthread_mutex_lock(&mutex);
	start = 1;
	produzIntervalo(inicio, fim);
	pthread_mutex_unlock(&mutex);

	pthread_cond_signal(&cond_pegaIntervalo);

    for (i = 0; i < numThreads; i++) 
    {
        pthread_join(tidSistema[i], NULL);
    }
    
    for (i=0; i<numThreads; i++) 
    {
        somaPartesInicioAteFim += somas_parciais[i];
    }
    
    return somaPartesInicioAteFim;
}

int main (int argc, char *argv[])
{
    long double integral_A_B;
    long double inicio, fim, erro;
    double tempo_inicio, tempo_fim;
    
    int funcao = 0;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond_pegaIntervalo , NULL);
    
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h')
    {
        ajuda(argv[0]);
        return 0;
    }
    
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'l')
    {
        listarFuncoes();
        return 0;
    }
    
    if (argc < 6) 
    {
        puts("Número de argumentos incompatível para ajuda tente:");
        printf("\t%s [-h]\n", argv[0]);
        
        return -1;
    }
    
    inicio = strtold(argv[1], NULL);
    fim = strtold(argv[2], NULL);
    erro = strtold(argv[3], NULL);
    
    funcao = atoi(argv[4]);
    numThreads = atoi(argv[5]);
    
    GET_TIME(tempo_inicio);

    somas_parciais = (long double *) calloc(numThreads, sizeof(long double));
    
    switch(funcao) 
    {
        case 1:
            integral_A_B = calculaIntegral(inicio, fim, erro, &umMaisX);
            break;
        case 2:
            integral_A_B = calculaIntegral(inicio, fim, erro, &raizDeUmMaisXAoQuadrado);
            break;
        case 3:
            integral_A_B = calculaIntegral(inicio, fim, erro, &raizDeUmMaisXElevadoAQuartaPotencia);
            break;
        case 4:
            integral_A_B = calculaIntegral(inicio, fim, erro, &senoXAoQuadrado);
            break;
        case 5:
            integral_A_B = calculaIntegral(inicio, fim, erro, &cincoMilesimosVezesXAoCuboMaisUm_VezesCossenoDeExpMenosX);
            break;
        default: 
            listarFuncoes();
            return -1;
    }
    
    free(somas_parciais);
    
    GET_TIME(tempo_fim);
    
    
    printf("Valor da integral: %.20Lf\n", integral_A_B);
    printf("Tempo de calculo: %.20lf\n", tempo_fim - tempo_inicio);
    
    return 0;
}

long double umMaisX(long double x)
{
    long double retorno;

    retorno = 1.0L + x;
    
    return retorno;
}

long double raizDeUmMaisXAoQuadrado(long double x)
{
    long double retorno;

    retorno = sqrt(1.0L + x*x);
    
    return retorno;
}

long double raizDeUmMaisXElevadoAQuartaPotencia(long double x)
{
    long double retorno;

    retorno = sqrt(1.0L + x*x*x*x);
    
    return retorno;
}

long double senoXAoQuadrado(long double x)
{
    long double retorno;

    retorno = sin(x*x);
    
    return retorno;
}

long double cincoMilesimosVezesXAoCuboMaisUm_VezesCossenoDeExpMenosX(long double x)
{
    long double retorno;

    retorno = (0.005L * x*x*x + 1.0L)*cos(exp(-x));
    
    return retorno;
}

void ajuda(char *prog)
{
    puts("Tente usar: ");
    printf("\t%s <inicioIntervalo> <fimIntervalo> <erro> <codFunc> <numThreads>\n\n", prog);
    puts("Para descobrir os codigos de funcao execute:");
    printf("\t%s [-l]\n", prog);
}

void listarFuncoes()
{
    puts("Valores válidos para funcao:");
    puts("-- f(x) = 1 + x \t\t\t\t-> 1");
    puts("-- f(x) = sqrt(1 + x^2) \t\t\t-> 2");
    puts("-- f(x) = sqrt(1 + x^4) \t\t\t-> 3");
    puts("-- f(x) = sen(x^2) \t\t\t\t-> 4");
    puts("-- f(x) = cos(e^(-x)) * (0.005 * x^3 + 1)\t-> 5");
}

long double absoluto(long double valor)
{
    if (valor < 0.0L) return -valor;
    return valor;
}
