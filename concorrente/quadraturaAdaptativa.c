#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../libs/timer.h"

int numThreads;

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
    puts("-- f(x) = 1 + x \t\t\t-> 1");
    puts("-- f(x) = sqrt(1 + x^2) \t\t-> 2");
    puts("-- f(x) = sqrt(1 + x^4) \t\t-> 3");
    puts("-- f(x) = sen(x^2) \t\t\t-> 4");
    puts("-- f(x) = cos(e^(-x)) * (0.005 * x^3 + 1)\t-> 5");
}

long double absoluto(long double valor)
{
    if (valor < 0.0L) return -valor;
    return valor;
}

long double umMaisX(long double x);

long double raizDeUmMaisXAoQuadrado(long double x);

long double raizDeUmMaisXElevadoAQuartaPotencia(long double x);

long double senoXAoQuadrado(long double x);

long double cincoMilesimosVezesXAoCuboMaisUm_VezesCossenoDeExpMenosX(long double x);

long double calculaIntervalo(long double inicio, long double fim, long double erro, long double (*func)(long double) )
{
    long double pontoMedio_InicioFim, pontoMedio_InicioMeio, pontoMedio_MeioFim;
    long double valorFunc_PontoMedio_InicioFim, valorFunc_PontoMedio_InicioMeio, valorFunc_PontoMedio_MeioFim;
    long double intervalo_InicioFim, intervalo_InicioMeio, intervalo_MeioFim;
    long double areaAprox_InicioFim, areaAprox_InicioMeio, areaAprox_MeioFim;

    long double erroAtual;
    long double inicioAtual, fimAtual;
    long double somaPartesInicioAteFim;
    
    inicioAtual = inicio;
    fimAtual = fim;
    somaPartesInicioAteFim = 0.0L;
    
    long i=0;
    while(inicioAtual != fim)
    {   
        i++;
        
        pontoMedio_InicioFim    = (fimAtual + inicioAtual) / 2.0L;
        pontoMedio_InicioMeio   = (inicioAtual + pontoMedio_InicioFim) / 2.0L;
        pontoMedio_MeioFim      = (pontoMedio_InicioFim + fimAtual) / 2.0L;

        intervalo_InicioFim     = fimAtual - inicioAtual;
        intervalo_InicioMeio    = pontoMedio_InicioFim - inicioAtual;
        intervalo_MeioFim       = fimAtual - pontoMedio_InicioFim;

        valorFunc_PontoMedio_InicioFim  = (func)(pontoMedio_InicioFim);
        valorFunc_PontoMedio_InicioMeio = (func)(pontoMedio_InicioMeio);
        valorFunc_PontoMedio_MeioFim    = (func)(pontoMedio_MeioFim);

        areaAprox_InicioFim     = intervalo_InicioFim * valorFunc_PontoMedio_InicioFim;
        areaAprox_InicioMeio    = intervalo_InicioMeio * valorFunc_PontoMedio_InicioMeio;
        areaAprox_MeioFim       = intervalo_MeioFim * valorFunc_PontoMedio_MeioFim;

        erroAtual = absoluto(areaAprox_InicioFim - (areaAprox_InicioMeio + areaAprox_MeioFim));
        
        if (erroAtual <= erro) 
        {
            //Soma a area da parte atual e avança para a próxima parte não calculada.
            somaPartesInicioAteFim  += areaAprox_InicioFim;
            
            inicioAtual = fimAtual;
            fimAtual = inicio + (2.0L * (inicioAtual - inicio));
            
            if ( fimAtual >= fim ) 
            {
                fimAtual = fim;
            }
        }
        else 
        {
            fimAtual = pontoMedio_InicioFim;
        }
    }
    
    printf("Número de iterações: %ld\n", i);
    
    return somaPartesInicioAteFim;
}

int main (int argc, char *argv[])
{
    long double integral_A_B;
    long double inicio, fim, erro;
    double tempo_inicio, tempo_fim;
    
    int funcao = 0;
    
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
        printf("%s [-h]\n", argv[0]);
        
        return -1;
    }
    
    
    inicio = strtold(argv[1], NULL);
    fim = strtold(argv[2], NULL);
    erro = strtold(argv[3], NULL);
    
    funcao = atoi(argv[4]);
    numThreads = atoi(argv[5]);

    GET_TIME(tempo_inicio);
    
    switch(funcao) 
    {
        case 1:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &umMaisX);
            break;
        case 2:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &raizDeUmMaisXAoQuadrado);
            break;
        case 3:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &raizDeUmMaisXElevadoAQuartaPotencia);
            break;
        case 4:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &senoXAoQuadrado);
            break;
        case 5:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &cincoMilesimosVezesXAoCuboMaisUm_VezesCossenoDeExpMenosX);
            break;
        default: 
            listarFuncoes();
            return -1;
    }
    
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

