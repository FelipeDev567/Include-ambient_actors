# 🧍 ambient_actors.inc

> Include para **open.mp / SA-MP** que popula o mapa com atores ambientes dinâmicos — pessoas varrendo, sentadas, conversando e muito mais — sem ocupar nenhum slot de jogador.

---

## 💡 O que é isso?

Sabe aquelas cidades vazias em servidores de SA-MP onde não tem ninguém nas ruas?  
Essa include resolve isso de forma inteligente.

Você registra pontos no mapa com uma posição, uma skin e uma ação. O sistema cuida do resto:

- ✅ Quando um jogador **chega perto** → o ator aparece automaticamente
- ✅ Quando o jogador **se afasta** → o ator some automaticamente
- ✅ **Não ocupa slot de player** — usa o sistema de Actors nativo
- ✅ **Zero impacto** em áreas sem jogadores

---

## 🎬 Ações disponíveis

| Constante | O que faz |
|---|---|
| `AA_ACTION_IDLE` | Fica parado, olhando ao redor |
| `AA_ACTION_SWEEP` | Varre o chão |
| `AA_ACTION_CHAT` | Gesticula como se estivesse conversando |
| `AA_ACTION_SIT` | Senta em banco ou chão |
| `AA_ACTION_SMOKE` | Fuma um cigarro |
| `AA_ACTION_PHONE` | Fala no celular |

---

## 📦 Instalação

1. Baixe o arquivo `ambient_actors.inc`
2. Coloque em `qawno/include/`
3. Adicione no topo do seu gamemode:

```pawn
#include <ambient_actors>
```

---

## 🚀 Como usar

```pawn
public OnGameModeInit()
{
    // 1. Inicia o sistema
    AA_Init();

    // 2. Registra os pontos no mapa
    // AA_RegisterPoint(skin, x, y, z, angulo, ação)

    AA_RegisterPoint(167, 2048.5, -1650.2, 13.5, 180.0, AA_ACTION_SWEEP); // varredor
    AA_RegisterPoint(86,  2035.0, -1644.0, 13.5,  90.0, AA_ACTION_SIT);   // sentado
    AA_RegisterPoint(72,  2055.0, -1638.0, 13.5, 270.0, AA_ACTION_CHAT);  // conversando
    AA_RegisterPoint(9,   2057.0, -1638.0, 13.5,  90.0, AA_ACTION_CHAT);  // par de conversa
    AA_RegisterPoint(34,  2042.0, -1655.0, 13.5,   0.0, AA_ACTION_SMOKE); // fumante

    return 1;
}
```

---

## 🔧 API completa

```pawn
// Inicia o sistema — chame no OnGameModeInit
AA_Init()

// Registra um ponto de spawn de ator
// Retorna o ID do ponto ou -1 se não houver slots disponíveis
AA_RegisterPoint(skin, Float:x, Float:y, Float:z, Float:angle, action)

// Remove um ponto e destrói o ator se estiver ativo
AA_RemovePoint(pointid)

// Altera o raio de spawn/despawn em unidades de jogo (padrão: 40.0)
AA_SetRadius(Float:radius)

// Altera o intervalo do timer de verificação em ms (padrão: 1500)
AA_SetSyncRate(ms)

// Retorna o ID do ator ativo de um ponto, ou INVALID_ACTOR_ID
AA_GetActor(pointid)
```

---

## ⚙️ Configurações internas

Você pode ajustar as defines no topo do arquivo:

| Define | Padrão | Descrição |
|---|---|---|
| `AA_MAX_POINTS` | `64` | Máximo de pontos registrados |
| `AA_DEFAULT_RADIUS` | `40.0` | Raio de spawn/despawn |
| `AA_DEFAULT_SYNC_MS` | `1500` | Intervalo do timer em ms |

---

## 📋 Requisitos

- open.mp **ou** SA-MP com suporte a Actors
- Pawn compiler 3.10+

---

## 📄 Licença

MIT — use, modifique e distribua à vontade. Créditos são bem-vindos. 🙂
