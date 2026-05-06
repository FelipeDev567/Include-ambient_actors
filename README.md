ambient_actors.inc
Include para open.mp que gera atores ambientes dinâmicos no mapa.

O problema que resolve
Servidores open.mp/SA-MP têm um limite de slots de jogadores para NPCs. Atores de decoração (pessoas na rua, varredor, etc.) desperdiçam esses slots. Esta include usa o sistema de Actors nativos, que não ocupam slot de player.

Como funciona
Você registra pontos no mapa com posição, skin e ação
Um timer verifica a cada 1.5s quais jogadores estão dentro do raio (padrão 40 unidades)
Se algum jogador está perto → o ator é criado automaticamente
Se ninguém está perto → o ator é destruído automaticamente
Resultado: zero impacto em performance quando nenhum player está na área
Ações disponíveis
Constante	Animação
AA_ACTION_IDLE	Parado
AA_ACTION_SWEEP	Varrendo o chão
AA_ACTION_CHAT	Conversando
AA_ACTION_SIT	Sentado
AA_ACTION_SMOKE	Fumando
AA_ACTION_PHONE	No celular
Instalação
Coloque ambient_actors.inc em qawno/include/ e adicione no seu gamemode:

#include <ambient_actors>
Uso
public OnGameModeInit()
{
    AA_Init();

    AA_RegisterPoint(167, 2048.5, -1650.2, 13.5, 180.0, AA_ACTION_SWEEP);
    AA_RegisterPoint(86,  2035.0, -1644.0, 13.5, 90.0,  AA_ACTION_SIT);
    AA_RegisterPoint(72,  2055.0, -1638.0, 13.5, 270.0, AA_ACTION_CHAT);
}
API
AA_Init()                                              // inicia o sistema
AA_RegisterPoint(skin, x, y, z, angle, action)        // registra um ponto
AA_RemovePoint(pointid)                                // remove um ponto
AA_SetRadius(Float:radius)                             // muda o raio (padrão 40.0)
AA_SetSyncRate(ms)                                     // muda o intervalo do timer
AA_GetActor(pointid)                                   // retorna o ID do ator ativo
Requisitos
open.mp ou SA-MP com suporte a Actors
