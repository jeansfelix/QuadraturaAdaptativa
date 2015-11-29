#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long double umMaisX(long double x)
{
    long double retorno;

    retorno = 1.0L + x;
    
    return retorno;
}

long double raizDeUmMaisXElevadoAoQuadrado(long double x)
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
    
    while(inicioAtual != fim)
    {   
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

        erroAtual = (areaAprox_InicioFim - (areaAprox_InicioMeio + areaAprox_MeioFim));
        
        if (erroAtual < 0.0L) erroAtual = -erroAtual;
        
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
    
    return somaPartesInicioAteFim;
}

int main (int argc, char *argv[])
{
    long double integral_A_B;
    long double inicio, fim, erro;
    
    int funcao = 0;
    
    if (argc < 5) 
    {
        puts("Número de argumentos incompatível tente: prog valorInicioIntervalo valorFimIntervalo valorErro codFunc.\nExemplo: para um intervalo de 0 a 1, um erro de 0.0001 e a funcao f = 1 + x:\n-> execute: prog 1 2 0.0001 1");
        return -1;
    }
    
    inicio = strtold(argv[1], NULL);
    fim = strtold(argv[2], NULL);
    erro = strtold(argv[3], NULL);
    
    funcao = atoi(argv[4]);
    
    switch(funcao) 
    {
        case 1:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &umMaisX);
            break;
        case 2:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &raizDeUmMaisXElevadoAoQuadrado);
            break;
        case 3:
            integral_A_B = calculaIntervalo(inicio, fim, erro, &raizDeUmMaisXElevadoAQuartaPotencia);
            break;
        default: 
            puts("Valores válidos para funcao:\n-- f(x) = 1 + x \t\t-> 1\n-- f(x) = sqrt(1 + x^2) \t-> 2\n-- f(x) = sqrt(1 + x^4) \t-> 3");
            return -1;
    }
    
    printf("Valor da integral: %.20Lf\n", integral_A_B);
    
    return 0;
}
