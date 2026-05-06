// =============================================================================
//  ambient_actors.inc — Atores Ambientes Dinâmicos para open.mp
//
//  O que faz:
//    - Registra pontos de spawn de atores no mapa (posição, skin, ação)
//    - Quando um jogador entra no raio de um ponto, o ator é criado
//    - Quando o jogador sai do raio, o ator é destruído (não ocupa slot fixo)
//    - Suporta ações: varrer, conversar, sentar, parado, fumar, celular
//    - Atores ficam visíveis apenas para jogadores próximos (via timer de sync)
//
//  Como usar:
//    1. #include <ambient_actors> no seu gamemode
//    2. No OnGameModeInit, chame AA_Init() e registre os pontos:
//         AA_RegisterPoint(skin, x, y, z, angle, AA_ACTION_SWEEP);
//         AA_RegisterPoint(skin, x, y, z, angle, AA_ACTION_CHAT);
//    3. Pronto — o sistema cuida do resto automaticamente
//
//  API pública:
//    AA_Init()                                          — inicia o sistema
//    AA_RegisterPoint(skin, Float:x, Float:y, Float:z,
//                     Float:angle, action)              — registra um ponto
//    AA_RemovePoint(pointid)                            — remove um ponto
//    AA_SetRadius(Float:radius)                         — raio de spawn (padrão 40.0)
//    AA_SetSyncRate(ms)                                 — intervalo do timer (padrão 1500ms)
//
//  Ações disponíveis:
//    AA_ACTION_IDLE    — parado (animação neutra)
//    AA_ACTION_SWEEP   — varrendo o chão
//    AA_ACTION_CHAT    — conversando (gesticula)
//    AA_ACTION_SIT     — sentado em banco
//    AA_ACTION_SMOKE   — fumando
//    AA_ACTION_PHONE   — no celular
// =============================================================================

#if defined _ambient_actors_included
    #endinput
#endif
#define _ambient_actors_included

#include <open.mp>

// -----------------------------------------------------------------------------
//  Configurações
// -----------------------------------------------------------------------------
#define AA_MAX_POINTS       64      // máximo de pontos registrados
#define AA_DEFAULT_RADIUS   40.0    // raio de spawn/despawn
#define AA_DEFAULT_SYNC_MS  1500    // intervalo do timer de sincronização
#define AA_INVALID_ACTOR    INVALID_ACTOR_ID

// -----------------------------------------------------------------------------
//  Ações
// -----------------------------------------------------------------------------
#define AA_ACTION_IDLE      0
#define AA_ACTION_SWEEP     1
#define AA_ACTION_CHAT      2
#define AA_ACTION_SIT       3
#define AA_ACTION_SMOKE     4
#define AA_ACTION_PHONE     5

// -----------------------------------------------------------------------------
//  Estado interno
// -----------------------------------------------------------------------------

// Dados de cada ponto registrado
static aa_skin  [AA_MAX_POINTS];
static Float:aa_x     [AA_MAX_POINTS];
static Float:aa_y     [AA_MAX_POINTS];
static Float:aa_z     [AA_MAX_POINTS];
static Float:aa_angle [AA_MAX_POINTS];
static aa_action[AA_MAX_POINTS];
static bool:aa_used  [AA_MAX_POINTS];

// Ator ativo para cada ponto (INVALID_ACTOR_ID se não criado)
static aa_actor [AA_MAX_POINTS];

// Quantos jogadores estão no raio de cada ponto
static aa_viewers[AA_MAX_POINTS];

// Configurações globais
static Float:aa_radius  = AA_DEFAULT_RADIUS;
static aa_syncTimer     = -1;
static aa_syncRate      = AA_DEFAULT_SYNC_MS;
static aa_pointCount    = 0;

// =============================================================================
//  Utilitários internos
// =============================================================================

static Float:AA_Dist2D(Float:x1, Float:y1, Float:x2, Float:y2)
{
    return floatsqroot((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

static AA_ApplyAction(actorid, action)
{
    switch(action)
    {
        case AA_ACTION_IDLE:
        {
            // Animação neutra — parado olhando ao redor
            ApplyActorAnimation(actorid, "PED", "IDLE_TIRED", 4.0, true, false, false, false, 0);
        }
        case AA_ACTION_SWEEP:
        {
            // Varrendo o chão
            ApplyActorAnimation(actorid, "SWEEPER", "SWEEP_L", 4.0, true, false, false, false, 0);
        }
        case AA_ACTION_CHAT:
        {
            // Conversando / gesticulando
            ApplyActorAnimation(actorid, "PED", "IDLE_CHAT", 4.0, true, false, false, false, 0);
        }
        case AA_ACTION_SIT:
        {
            // Sentado
            ApplyActorAnimation(actorid, "BEACH", "SIT_relax", 4.0, true, false, false, false, 0);
        }
        case AA_ACTION_SMOKE:
        {
            // Fumando
            ApplyActorAnimation(actorid, "SMOKING", "M_smkstnd_loop", 4.0, true, false, false, false, 0);
        }
        case AA_ACTION_PHONE:
        {
            // No celular
            ApplyActorAnimation(actorid, "PHONE", "PHONE_TALK", 4.0, true, false, false, false, 0);
        }
    }
}

static AA_SpawnActor(pointid)
{
    if(aa_actor[pointid] != AA_INVALID_ACTOR) return; // já existe

    new id = CreateActor(
        aa_skin[pointid],
        aa_x[pointid],
        aa_y[pointid],
        aa_z[pointid],
        aa_angle[pointid]
    );

    if(id == AA_INVALID_ACTOR) return;

    aa_actor[pointid] = id;
    SetActorHealth(id, 99999.0);
    AA_ApplyAction(id, aa_action[pointid]);
}

static AA_DespawnActor(pointid)
{
    if(aa_actor[pointid] == AA_INVALID_ACTOR) return;
    DestroyActor(aa_actor[pointid]);
    aa_actor[pointid] = AA_INVALID_ACTOR;
}

// =============================================================================
//  Timer de sincronização
// =============================================================================

forward AA_SyncTick();
public AA_SyncTick()
{
    for(new p = 0; p < AA_MAX_POINTS; p++)
    {
        if(!aa_used[p]) continue;

        new viewers = 0;

        for(new i = 0; i < MAX_PLAYERS; i++)
        {
            if(!IsPlayerConnected(i)) continue;
            if(IsPlayerNPC(i)) continue;

            new Float:px, Float:py, Float:pz;
            GetPlayerPos(i, px, py, pz);

            if(AA_Dist2D(px, py, aa_x[p], aa_y[p]) <= aa_radius)
                viewers++;
        }

        aa_viewers[p] = viewers;

        if(viewers > 0)
            AA_SpawnActor(p);
        else
            AA_DespawnActor(p);
    }
}

// =============================================================================
//  API Pública
// =============================================================================

/**
 * Inicializa o sistema. Chame no OnGameModeInit.
 */
stock AA_Init()
{
    for(new p = 0; p < AA_MAX_POINTS; p++)
    {
        aa_used[p]    = false;
        aa_actor[p]   = AA_INVALID_ACTOR;
        aa_viewers[p] = 0;
    }

    if(aa_syncTimer != -1)
        KillTimer(aa_syncTimer);

    aa_syncTimer = SetTimer("AA_SyncTick", aa_syncRate, true);
}

/**
 * Registra um ponto de spawn de ator ambiente.
 * Retorna o ID do ponto (0..AA_MAX_POINTS-1) ou -1 se cheio.
 */
stock AA_RegisterPoint(skin, Float:x, Float:y, Float:z, Float:angle, action = AA_ACTION_IDLE)
{
    for(new p = 0; p < AA_MAX_POINTS; p++)
    {
        if(aa_used[p]) continue;

        aa_skin[p]   = skin;
        aa_x[p]      = x;
        aa_y[p]      = y;
        aa_z[p]      = z;
        aa_angle[p]  = angle;
        aa_action[p] = action;
        aa_used[p]   = true;
        aa_actor[p]  = AA_INVALID_ACTOR;
        aa_pointCount++;
        return p;
    }
    return -1; // sem slots
}

/**
 * Remove um ponto registrado e destrói o ator se estiver ativo.
 */
stock AA_RemovePoint(pointid)
{
    if(pointid < 0 || pointid >= AA_MAX_POINTS) return;
    if(!aa_used[pointid]) return;

    AA_DespawnActor(pointid);
    aa_used[pointid] = false;
    aa_pointCount--;
}

/**
 * Altera o raio de spawn/despawn (em unidades de jogo).
 */
stock AA_SetRadius(Float:radius)
{
    aa_radius = (radius > 0.0) ? radius : AA_DEFAULT_RADIUS;
}

/**
 * Altera o intervalo do timer de sincronização (em ms).
 * Chame antes de AA_Init() para ter efeito imediato,
 * ou depois para reiniciar o timer.
 */
stock AA_SetSyncRate(ms)
{
    aa_syncRate = (ms > 100) ? ms : 100;

    if(aa_syncTimer != -1)
    {
        KillTimer(aa_syncTimer);
        aa_syncTimer = SetTimer("AA_SyncTick", aa_syncRate, true);
    }
}

/**
 * Retorna o ID do ator ativo de um ponto, ou INVALID_ACTOR_ID.
 */
stock AA_GetActor(pointid)
{
    if(pointid < 0 || pointid >= AA_MAX_POINTS) return AA_INVALID_ACTOR;
    return aa_actor[pointid];
}

// =============================================================================
//  Limpeza ao fechar o servidor
// =============================================================================

public OnGameModeExit()
{
    for(new p = 0; p < AA_MAX_POINTS; p++)
        AA_DespawnActor(p);

    if(aa_syncTimer != -1)
    {
        KillTimer(aa_syncTimer);
        aa_syncTimer = -1;
    }

    #if defined AA_OnGameModeExit
        return AA_OnGameModeExit();
    #else
        return 1;
    #endif
}

#if defined _ALS_OnGameModeExit
    #undef OnGameModeExit
#else
    #define _ALS_OnGameModeExit
#endif
#define OnGameModeExit AA_OnGameModeExit
#if defined AA_OnGameModeExit
    forward AA_OnGameModeExit();
#endif
