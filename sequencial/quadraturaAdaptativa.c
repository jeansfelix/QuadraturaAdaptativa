#include <stdio.h>
#include <stdlib.h>

long double umMaisX(long double x)
{
    long double retorno;

    retorno = 1.0L + x;
    
    return retorno;
}

long double calculaIntervalo(long double inicio, long double fim, long double erro, long double (*func)(long double) )
{
    long double pontoMedio_InicioFim, pontoMedio_InicioMeio, pontoMedio_MeioFim;
    long double valorFunc_PontoMedio_InicioFim, valorFunc_PontoMedio_InicioMeio, valorFunc_PontoMedio_MeioFim;
    long double intervalo_InicioFim, intervalo_InicioMeio, intervalo_MeioFim;
    long double areaAprox_InicioFim, areaAprox_InicioMeio, areaAprox_MeioFim;
    long double areaAprox;
    
    pontoMedio_InicioFim = (fim + inicio) / 2.0L;
    pontoMedio_InicioMeio = (inicio + pontoMedio_InicioFim) / 2.0L;
    pontoMedio_MeioFim = (pontoMedio_InicioFim + fim) / 2.0L;

    intervalo_InicioFim     = fim - inicio;
    intervalo_InicioMeio    = pontoMedio_InicioFim - inicio;
    intervalo_MeioFim       = fim - pontoMedio_InicioFim;

    valorFunc_PontoMedio_InicioFim = (func)(pontoMedio_InicioFim);
    valorFunc_PontoMedio_InicioMeio = (func)(pontoMedio_InicioMeio);
    valorFunc_PontoMedio_MeioFim = (func)(pontoMedio_MeioFim);

    areaAprox_InicioFim     = intervalo_InicioFim * valorFunc_PontoMedio_InicioFim;
    areaAprox_InicioMeio    = intervalo_InicioMeio * valorFunc_PontoMedio_InicioMeio;
    areaAprox_MeioFim       = intervalo_MeioFim * valorFunc_PontoMedio_MeioFim;

    if ((areaAprox_InicioFim - (areaAprox_InicioMeio + areaAprox_MeioFim)) > erro) 
    {
        areaAprox = calculaIntervalo(inicio, pontoMedio_InicioFim, erro, func) + calculaIntervalo(pontoMedio_InicioFim, fim, erro, func);
        
        return areaAprox;
    }
    
    return areaAprox_InicioFim;
}

int main (int argc, char *argv[])
{
    long double integral_A_B;
    long double inicio, fim, erro; 
    
    if (argc != 4) 
    {
        printf("Número de argumentos incompatível tente: prog valorInicioIntervalo valorFimIntervalo valorErro.\nExemplo: para um intervalo de 0 a 1 e um erro de 0.0001 execute: prog 1 2 0.0001\n ");
        return -1;
    }
    
    inicio = strtold(argv[1], NULL);
    fim = strtold(argv[2], NULL);
    erro = strtold(argv[3], NULL);
    
    integral_A_B = calculaIntervalo(inicio, fim, erro, &umMaisX);
    
    printf("Valor da integral: %Lf\n", integral_A_B);
    
    return 0;
}
