#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define N 5 //quantidade de filósofos à mesa
#define TIME 5 //tempo máximo para pensar e comer

enum states{THINKING, EATING, HUNGRY} state[N]; //estados possíveis para cada filósofo
pthread_t philosopher[N]; //thread dos filósofos
pthread_mutex_t chopstick[N]; //mutex para cada palito

/*
Função para testar se um filósofo pode comer
argumento: int num - posição do filósofo na mesa
retorno: void
testa se o filósofo pode comer
*/
void test(int num){
    //verifica se o filósofo está hungry e se os filósofos à esquerda e à direita não estão comendo
    if(state[num] == HUNGRY && (state[(num+(N-1))%N] != EATING && state[(num+1)%N] != EATING)){
        state[num] = EATING; //muda o estado do filósofo para comendo
        pthread_mutex_lock(&chopstick[num]); //locka o palito à esquerda
        pthread_mutex_lock(&chopstick[(num+1)%N]); //locka o palito à direita
        printf("philosopher %d is eating\n", num);
    }
}

/*
Função de pegar palitos
argumento: int num - posição do filósofo na mesa
retorno: void
altera o estado do filósofo para hungry e chama a função que testa se o filósofo pode comer
*/
void pickup(int num){
    state[num] = HUNGRY; //altera o estado do filósofo para hungry
    test(num); //função para testar se o filósofo pode comer
}
/*
Função para retornar os palitos à mesa
arguento: int num - posição do filósofo na mesa
retorno: void
retorna os palitos à mesa e altera o estado do filósofo para thinking
*/
void putdown(int num){
    //caso o filósofo esteja comendo 
    //(isso é importante para que o filósofo não retorne os palitos caso não esteja comendo.
    //Ou seja, sem essa cláusula, ele podria ir de hungry para thinking sem comer)
    if(state[num] == EATING){
        state[num] = THINKING; //retorna para o estado de thinking
        pthread_mutex_unlock(&chopstick[num]); //retorna o palito à esquerda
        pthread_mutex_unlock(&chopstick[(num+1)%N]); //retorna o palito à direita
        printf("philosopher %d finished eating\n", num);
        //testa os filósofos à esquerda e à direita para evitar starvation
        test((num+1)%N);
        test((num+(N-1))%N);
    }
}
/*
Função dos filósofos
argumento: void *val - posição do filósofo na mesa
retorno: void *
realiza os procedimentos de pegar os palitos e de devolvê-los à mesa
*/
void *dp(void *val){
    int num = *(int*) val; //converte o argumento val para int
    while(1){
        srand(time(NULL) + num); //seed aleatória
        int eatTime = (rand()%TIME) + 1; //calcula tempo de comer
        int thinkTime = (rand()%TIME) + 1;  //calcula tempo de pensar
        printf("philosopher %d is thinking for %d seconds\n", num, thinkTime);
        sleep(thinkTime); //aguarda tempo de pensar
        pickup(num); 
        sleep(eatTime); //aguarda o tempo de comer
        putdown(num);
    }
}

int main(void){
    //vetor auxiliar para enumerar os filósofos
    int pos[N];
    //criando as threads dos palitos
    for(int i = 0; i < N; i++){
        state[i] = THINKING; //inicializa o filósofo para estado de pensar
        pos[i] = i;
        pthread_mutex_init(&chopstick[i], NULL);
    }
    //criando as threads dos filósofos
    for(int i = 0; i < N; i++){
        state[i] = THINKING;
        pthread_create(&philosopher[i], NULL, &dp, &pos[i]);
    }
    //dando join nas threads dos filósofos
    for(int i = 0; i < N; i++){
        pthread_join(philosopher[i], NULL);
    }

    return 0;
}