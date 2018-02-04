#define NUM_STATES 6
#define NUM_EVENTS 6

// definicao dos possiveis eventos gerados pelos estados da FSM
typedef enum event_ {evento_wifi, evento_mqtt, evento_gerenciador, evento_btn, evento_tempo, evento_sair} event;
// definicao das funcoes que implementam o comportamento de cada estado
event conexao_wifi_state(void);
event conexao_mqtt_state(void);
event gerenciador_state(void);
event btn_envio_state(void);
event tempo_envio_state(void);
event sair_state(void);

// array de ponteiros para as funcoes dos estados
event (* state_functions[])(void) = {conexao_wifi_state, conexao_mqtt_state, gerenciador_state, btn_envio_state,
                                    tempo_envio_state, sair_state};
// definicao dos nomes dos estados
typedef enum state_ {wifi, mqtt, gerenciador, btn_envio, tempo_envio, sair} state;
// estrutura que define as transicoes dos estados
state state_transitions[NUM_STATES][NUM_EVENTS] = {{wifi, mqtt, wifi, wifi, wifi,wifi},
                                                   {mqtt, mqtt, gerenciador, mqtt, mqtt, mqtt},
                                                   {gerenciador, gerenciador,gerenciador, btn_envio,tempo_envio,gerenciador},
                                                   {btn_envio, btn_envio,gerenciador, btn_envio, btn_envio, btn_envio}, 
                                                   {tempo_envio, tempo_envio, gerenciador, tempo_envio, tempo_envio, tempo_envio},
                                                   {sair, sair, sair, sair, sair, sair}};
// definicao dos estados inicial e final
#define EXIT_STATE sair
#define ENTRY_STATE wifi

// funcao que implementa a transicao de estados
state lookup_transitions(state cur_state, event cur_evt) {
  return state_transitions[cur_state][cur_evt];
}

