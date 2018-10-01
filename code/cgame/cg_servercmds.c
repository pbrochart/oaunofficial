/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cg_local.h"
#include "../../ui/menudef.h" // bk001205 - for Q3_ui as well
#include "../game/bg_promode.h"

typedef struct {
    const char *order;
    int taskNum;
} orderTask_t;

#ifdef MISSIONPACK // bk001204
static int CG_ValidOrder ( const char *p ) {
    int i;
    for ( i = 0; i < numValidOrders; i++ ) {
        if ( Q_stricmp ( p, validOrders[i].order ) == 0 ) {
            return validOrders[i].taskNum;
        }
    }
    return -1;
}
#endif

char	gameString[128];

void CG_SetGameString ( void ) {
    qtime_t	now;
    char		*mapname;
    char		*buf;
    char		playerName[128];
    clientInfo_t	*ci1, *ci2;
    int 		i, count;

    char *gameNames[] = {
        "FFA",
        "1v1",
        "SP",
        "TDM",
        "CTF",
        "OCTF",
        "O",
        "H",
        "CA",
        "CTFE",
        "LMS",
        "DD",
        "D"
    };

    trap_RealTime ( &now );

    mapname = strchr ( cgs.mapname, '/' );
    mapname++;
    buf = strstr ( mapname, ".bsp" );
    buf[0] = '\0';

    if ( cgs.gametype == GT_TOURNAMENT ) {
        // find the two active players
        ci1 = NULL;
        ci2 = NULL;

        for ( i = 0 ; i < cgs.maxclients ; i++ ) {

            if ( cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_FREE ) {
                if ( !ci1 ) {
                    ci1 = &cgs.clientinfo[i];
                } else {
                    ci2 = &cgs.clientinfo[i];
                }
            }

        }
        Com_sprintf ( playerName, sizeof ( playerName ),"%sVS%s", ci1->name, ci2->name );
    } else {
        Com_sprintf ( playerName, sizeof ( playerName ),"%s", cgs.clientinfo[cg.clientNum].name );
    }

    count = 0;
    for ( i = 0; i < 128 ; i++ ) {
        if ( playerName[i] == '^' && ( ( playerName[i+1] >= '0' && playerName[i+1] <= '9' ) || ( playerName[i+1] >= 'a' && playerName[i+1] <= 'z' ) || ( playerName[i+1] >= 'A' && playerName[i+1] <= 'Z' ) ) ) {
            i++;
            continue;
        } else if ( ( ! ( playerName[i] >= '0' && playerName[i] <= '9' ) && ! ( playerName[i] >= 'a' && playerName[i] <= 'z' ) && ! ( playerName[i] >= 'A' && playerName[i] <= 'Z' ) ) ) {
            continue;
        }

        else {
            playerName[count] = playerName[i];
            count++;
        }
    }

    playerName[count] = '\0';


    Com_sprintf ( gameString, sizeof ( gameString ), "%04d-%02d-%02d/%04d%02d%02d%02d%02d%02d-%s-%s-%s",
                  1900 + now.tm_year,
                  1 + now.tm_mon,
                  now.tm_mday,
                  1900 + now.tm_year,
                  1 + now.tm_mon,
                  now.tm_mday,
                  now.tm_hour,
                  now.tm_min,
                  now.tm_sec,
                  gameNames[cgs.gametype],
                  playerName,
                  mapname );

    CG_Printf ( "gamestring: %s\n", gameString );
}

/*
=================
CG_StartOfGame

=================
*/
void CG_StartOfGame ( void ) {
    char 		*cmd;

    if ( cg.demoPlayback )
        return;
    
    if ( ( cg_autoaction.integer & 16 ) && cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	return;

    if ( !cg.demoStarted )
        CG_SetGameString();

    if ( cg_autoaction.integer & 1 && !( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR && ( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) ) ) {
        cg.demoStarted = 1;
        cmd = va ( "record %s\n", gameString );
        trap_SendConsoleCommand ( cmd );
    }
}

/*
=================
CG_EndOfGame

=================
*/
static void CG_EndOfGame ( void ) {
    char		*cmd;

    if ( cg.demoPlayback )
        return;

    if ( cg_autoaction.integer & 1 ) {
        cg.demoStarted = 0;
        trap_SendConsoleCommand ( "stoprecord;" );
    }
    
    if ( ( cg_autoaction.integer & 16 ) && cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	return;
    
    if ( cg_autoaction.integer & 2 ) {
        cmd = va ( "screenshotJPEG %s;",gameString );
        trap_SendConsoleCommand ( cmd );
        CG_Printf ( cmd );
    }
    if ( cg_autoaction.integer & 4 )
        trap_SendConsoleCommand ( "stats;" );
}

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseScores ( void ) {
    int		i, powerups;

    cg.numScores = atoi ( CG_Argv ( 1 ) );
    if ( cg.numScores > MAX_CLIENTS ) {
        cg.numScores = MAX_CLIENTS;
    }

    cg.teamScores[0] = atoi ( CG_Argv ( 2 ) );
    cg.teamScores[1] = atoi ( CG_Argv ( 3 ) );

    cgs.roundStartTime = atoi ( CG_Argv ( 4 ) );

    //Update thing in lower-right corner
    if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ) {
        cgs.scores1 = cg.teamScores[0];
        cgs.scores2 = cg.teamScores[1];
    }

    memset ( cg.scores, 0, sizeof ( cg.scores ) );

#define NUM_DATA 26
#define NUM_DATA_DUEL 47
#define FIRST_DATA 4

    for ( i = 0 ; i < cg.numScores ; i++ ) {
        if ( cgs.gametype == GT_TOURNAMENT ) {
            //
            cg.scores[i].client = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 1 ) );
            cg.scores[i].score = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 2 ) );
            cg.scores[i].ping = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 3 ) );
            cg.scores[i].time = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 4 ) );
            cg.scores[i].scoreFlags = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 5 ) );
            powerups = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 6 ) );
            cg.scores[i].accuracy = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 7 ) );
            cg.scores[i].impressiveCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 8 ) );
            cg.scores[i].excellentCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 9 ) );
            cg.scores[i].guantletCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 10 ) );
            cg.scores[i].defendCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 11 ) );
            cg.scores[i].assistCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 12 ) );
            cg.scores[i].perfect = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 13 ) );
            cg.scores[i].captures = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 14 ) );
            cg.scores[i].isDead = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 15 ) );
            cg.scores[i].dmgdone = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 16 ) );
            cg.scores[i].dmgtaken = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 17 ) );
            cg.scores[i].specOnly = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 18 ) );
            cg.scores[i].deathCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 19 ) );
            cg.scores[i].frags = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 20 ) );
            cg.scores[i].airrocketCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 21 ) );
            cg.scores[i].airgrenadeCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 22 ) );
            cg.scores[i].fullshotgunCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 23 ) );
            cg.scores[i].rocketRailCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 24 ) );
            cg.scores[i].itemDeniedCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 25 ) );
            cg.scores[i].health = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 26 ) );
            cg.scores[i].armor = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 27 ) );
            cg.scores[i].yellowArmor = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 28 ) );
            cg.scores[i].redArmor = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 29 ) );
            cg.scores[i].megaHealth = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 30 ) );
            cg.scores[i].accuracys[0][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 31 ) );
            cg.scores[i].accuracys[0][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 32 ) );
            cg.scores[i].accuracys[1][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 33 ) );
            cg.scores[i].accuracys[1][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 34 ) );
            cg.scores[i].accuracys[2][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 35 ) );
            cg.scores[i].accuracys[2][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 36 ) );
            cg.scores[i].accuracys[3][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 37 ) );
            cg.scores[i].accuracys[3][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 38 ) );
            cg.scores[i].accuracys[4][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 39 ) );
            cg.scores[i].accuracys[4][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 40 ) );
            cg.scores[i].accuracys[5][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 41 ) );
            cg.scores[i].accuracys[5][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 42 ) );
            cg.scores[i].accuracys[6][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 43 ) );
            cg.scores[i].accuracys[6][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 44 ) );
            cg.scores[i].accuracys[7][0] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 45 ) );
            cg.scores[i].accuracys[7][1] = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 46 ) );
            cg.scores[i].spawnkillCount = atoi ( CG_Argv ( i * NUM_DATA_DUEL + FIRST_DATA + 47 ) );
            //cgs.roundStartTime =


            if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
                cg.scores[i].client = 0;
            }
            cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
            cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;
            cgs.clientinfo[ cg.scores[i].client ].isDead = cg.scores[i].isDead;

            cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
        }
        else {
            cg.scores[i].client = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 1 ) );
            cg.scores[i].score = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 2 ) );
            cg.scores[i].ping = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 3 ) );
            cg.scores[i].time = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 4 ) );
            cg.scores[i].scoreFlags = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 5 ) );
            powerups = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 6 ) );
            cg.scores[i].accuracy = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 7 ) );
            cg.scores[i].impressiveCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 8 ) );
            cg.scores[i].excellentCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 9 ) );
            cg.scores[i].guantletCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 10 ) );
            cg.scores[i].defendCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 11 ) );
            cg.scores[i].assistCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 12 ) );
            cg.scores[i].perfect = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 13 ) );
            cg.scores[i].captures = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 14 ) );
            cg.scores[i].isDead = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 15 ) );
            cg.scores[i].dmgdone = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 16 ) );
            cg.scores[i].dmgtaken = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 17 ) );
            cg.scores[i].specOnly = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 18 ) );
            cg.scores[i].deathCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 19 ) );
            cg.scores[i].frags = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 20 ) );
            cg.scores[i].airrocketCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 21 ) );
            cg.scores[i].airgrenadeCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 22 ) );
            cg.scores[i].fullshotgunCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 23 ) );
            cg.scores[i].rocketRailCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 24 ) );
            cg.scores[i].itemDeniedCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 25 ) );
            cg.scores[i].spawnkillCount = atoi ( CG_Argv ( i * NUM_DATA + FIRST_DATA + 26 ) );
            //cgs.roundStartTime =


            if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
                cg.scores[i].client = 0;
            }
            cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
            cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;
            cgs.clientinfo[ cg.scores[i].client ].isDead = cg.scores[i].isDead;

            cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
        }
    }
#ifdef MISSIONPACK
    CG_SetScoreSelection ( NULL );
#endif

}

typedef enum {
    STATS_CLIENTNUM,

    STATS_GAUNT_DMG,
    STATS_GAUNT_KILLS,
    STATS_GAUNT_DEATH,

    STATS_MG_SHOTS,
    STATS_MG_HITS,
    STATS_MG_DMG,
    STATS_MG_KILLS,
    STATS_MG_DEATH,

    STATS_SG_SHOTS,
    STATS_SG_HITS,
    STATS_SG_DMG,
    STATS_SG_KILLS,
    STATS_SG_DEATH,

    STATS_GL_SHOTS,
    STATS_GL_HITS,
    STATS_GL_DMG,
    STATS_GL_KILLS,
    STATS_GL_DEATH,

    STATS_RL_SHOTS,
    STATS_RL_HITS,
    STATS_RL_DMG,
    STATS_RL_KILLS,
    STATS_RL_DEATH,

    STATS_LG_SHOTS,
    STATS_LG_HITS,
    STATS_LG_DMG,
    STATS_LG_KILLS,
    STATS_LG_DEATH,

    STATS_RG_SHOTS,
    STATS_RG_HITS,
    STATS_RG_DMG,
    STATS_RG_KILLS,
    STATS_RG_DEATH,

    STATS_PG_SHOTS,
    STATS_PG_HITS,
    STATS_PG_DMG,
    STATS_PG_KILLS,
    STATS_PG_DEATH,

    STATS_BFG_SHOTS,
    STATS_BFG_HITS,
    STATS_BFG_DMG,
    STATS_BFG_KILLS,
    STATS_BFG_DEATH,

    STATS_HEALTH_COUNT,
    STATS_ARMOR_COUNT,
    STATS_YA_COUNT,
    STATS_RA_COUNT,
    STATS_MH_COUNT,

    STATS_IMPESSIVE,
    STATS_EXCELLENT,
    STATS_ASSIST,
    STATS_DEFEND,
    STATS_CAPTURE,

    STATS_AIRGRENADE,
    STATS_AIRROCKET,
    STATS_FULLSG,
    STATS_RLRG,
    STATS_ITEMDENIED,
    STATISTIC_MAX
} statistic_t;


static float accuracy ( int shots, int hits ) {
    float acc;
    if ( shots > 0 )
        acc = 100.0f* ( ( float ) hits ) / ( ( float ) shots );
    else
        acc = 0;
    return acc;
}

/*
=================
CG_ParseStatistics

=================
*/
static void CG_ParseStatistics ( void ) {
    int		i, j, numStats, len = 128;
    fileHandle_t f;
    char		*name, *string;
    char		*team;
    clientInfo_t    *ci;
    //int 		scorenum;
    score_t		*score = &cg.scores[0];
    int		clientnum;
    char		*mapname;
    char		*buf;
    int		clientStats[MAX_CLIENTS][STATISTIC_MAX];

    char *gameNames[] = {
        "FFA",
        "1v1",
        "SP",
        "TDM",
        "CTF",
        "OCTF",
        "O",
        "H",
        "CA",
        "CTFE",
        "LMS",
        "DD",
        "D"
    };

    numStats = atoi ( CG_Argv ( 1 ) );
    if ( numStats > MAX_CLIENTS ) {
        numStats = MAX_CLIENTS;
    }

#define NUM_DATA_STATS 59
#define FIRST_DATA_STATS 1
#define NUM_WEAPONDATA_STATS 5

    mapname = strchr ( cgs.mapname, '/' );
    mapname++;
    buf = strstr ( mapname, ".bsp" );
    buf[0] = '\0';

    trap_FS_FOpenFile ( va ( "stats/%s.html", gameString ), &f, FS_WRITE );

    string = va ( "<html>\n<title> %s </title>\n<body>\n", gameString );
    len = strlen ( string );
    trap_FS_Write ( string, len, f );

    string = va ( "<h3 align=center> %s on %s</h3>\n", gameNames[cgs.gametype], mapname );
    len = strlen ( string );
    trap_FS_Write ( string, len, f );


    if ( cg_autoaction.integer & 2 ) {
        string = va ( "<p align=center><a href=\"../../screenshots/%s.jpg\"><img src=\"../../screenshots/%s.jpg\" width=\"480\"></a></p>", gameString, gameString );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );
    }

    string = va ( "<p align=center> <b>Gametype</b>:   %s<br><b>Map</b>:    %s<br>\n", gameNames[cgs.gametype], mapname );
    len = strlen ( string );
    trap_FS_Write ( string, len, f );

    if ( cgs.gametype >= GT_TEAM ) {
        string = va ( "<b><font color=red>Red</font></b>:   %i     <b><font color=blue>Blue</font></b> :   %i</p><br>", cgs.scores1, cgs.scores2 );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );
    } else {
        string = va ( "</p><br>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );
    }


    for ( i = 0 ; i < numStats ; i++ ) {

        for ( j = 0; j < STATISTIC_MAX; j++ ) {
            clientStats[i][j] = atoi ( CG_Argv ( i * NUM_DATA_STATS + FIRST_DATA_STATS + 1 + j ) );
        }
    }

    for ( i = 0; i < numStats; i++ ) {

        string = va ( "<hr>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        clientnum = clientStats[i][STATS_CLIENTNUM];

        for ( j = 0 ; j < cg.numScores ; j++ ) {
            if ( clientnum == cg.scores[j].client ) {
                //scorenum = j;
                score = &cg.scores[j];
                break;
            }
        }

        ci = &cgs.clientinfo[clientnum];
        name = ci->name;

        if ( ci->team == TEAM_FREE )
            team = "<font color=green>Free</font>";
        else if ( ci->team == TEAM_BLUE )
            team = "<font color=blue>Blue</font>";
        else if ( ci->team == TEAM_RED )
            team = "<font color=red>Red</font>";
        else //if( ci->team == TEAM_SPECTATOR )
            team = "<font color=grey>Spectator</font>";


        string = va ( "<p align=center> <b>Name</b>:%s     <b>Team</b>:%s     <b>Time</b>:%i     <b>Handicap</b>:%i <br> ", name, team, score->time, ci->handicap );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );


        string = va ( "<b>Score</b>: %i     <b>Accuracy</b>: %i     <b>Frags</b>: %i    <b>Death</b>: %i</p><br>", score->score, score->accuracy, score->frags, score->deathCount );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );


        string = va ( "<p align=center><font color=green><b>Damage done</b>: %i</font>     <font color=red><b>Damage taken</b>: %i</font></p> \n", score->dmgdone, score->dmgtaken );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<p align=center> <b>Rewards</b>: </p>\n" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<p align=center><table border=\"1\"><tr><td>Impressive</td> <td>Excellent</td> <td>Airgrenade</td> <td>Airrocket</td> <td>FullSG</td> <td>RocketRail</td> <td>Item Denied</td>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION ) {
            string = va ( " <td>Assist</td> <td>Defend</td> <td>Capture</td>" );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );
        }

        string = va ( "</tr><br>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td>", clientStats[i][STATS_IMPESSIVE], clientStats[i][STATS_EXCELLENT],
                      clientStats[i][STATS_AIRGRENADE], clientStats[i][STATS_AIRROCKET], clientStats[i][STATS_FULLSG], clientStats[i][STATS_RLRG], clientStats[i][STATS_ITEMDENIED] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION ) {
            string = va ( " <td>%i</td> <td>%i</td> <td>%i</td>", clientStats[i][STATS_ASSIST], clientStats[i][STATS_DEFEND], clientStats[i][STATS_CAPTURE] );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );
        }

        string = va ( "</tr></table></p><br>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<p align=center> <b>Accuracy</b>: </p>\n" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<p align=center><table border=\"1\"><tr><td></td> <td>Shots</td> <td>Hits</td> <td>Accuracy</td> <td>Damage done</td> <td>Kills</td> <td>Death</td></tr>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>Gauntlet</td> <td></td> <td></td> <td></td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_GAUNT_DMG], clientStats[i][STATS_GAUNT_KILLS], clientStats[i][STATS_GAUNT_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>MG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_MG_SHOTS], clientStats[i][STATS_MG_HITS], accuracy ( clientStats[i][STATS_MG_SHOTS], clientStats[i][STATS_MG_HITS] ),
                      clientStats[i][STATS_MG_DMG], clientStats[i][STATS_MG_KILLS], clientStats[i][STATS_MG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>SG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_SG_SHOTS], clientStats[i][STATS_SG_HITS], accuracy ( clientStats[i][STATS_SG_SHOTS], clientStats[i][STATS_SG_HITS] ),
                      clientStats[i][STATS_SG_DMG], clientStats[i][STATS_SG_KILLS], clientStats[i][STATS_SG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>GL</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_GL_SHOTS], clientStats[i][STATS_GL_HITS], accuracy ( clientStats[i][STATS_GL_SHOTS], clientStats[i][STATS_GL_HITS] ),
                      clientStats[i][STATS_GL_DMG], clientStats[i][STATS_GL_KILLS], clientStats[i][STATS_GL_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>RL</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_RL_SHOTS], clientStats[i][STATS_RL_HITS], accuracy ( clientStats[i][STATS_RL_SHOTS], clientStats[i][STATS_RL_HITS] ),
                      clientStats[i][STATS_RL_DMG], clientStats[i][STATS_RL_KILLS], clientStats[i][STATS_RL_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>LG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_LG_SHOTS], clientStats[i][STATS_LG_HITS], accuracy ( clientStats[i][STATS_LG_SHOTS], clientStats[i][STATS_LG_HITS] ),
                      clientStats[i][STATS_LG_DMG], clientStats[i][STATS_LG_KILLS], clientStats[i][STATS_LG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>RG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_RG_SHOTS], clientStats[i][STATS_RG_HITS], accuracy ( clientStats[i][STATS_RG_SHOTS], clientStats[i][STATS_RG_HITS] ),
                      clientStats[i][STATS_RG_DMG], clientStats[i][STATS_RG_KILLS], clientStats[i][STATS_RG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>PG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_PG_SHOTS], clientStats[i][STATS_PG_HITS], accuracy ( clientStats[i][STATS_PG_SHOTS], clientStats[i][STATS_PG_HITS] ),
                      clientStats[i][STATS_PG_DMG], clientStats[i][STATS_PG_KILLS], clientStats[i][STATS_PG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "<tr><td>BFG</td> <td>%i</td> <td>%i</td> <td>%.1f</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_BFG_SHOTS], clientStats[i][STATS_BFG_HITS], accuracy ( clientStats[i][STATS_BFG_SHOTS], clientStats[i][STATS_BFG_HITS] ),
                      clientStats[i][STATS_BFG_DMG], clientStats[i][STATS_BFG_KILLS], clientStats[i][STATS_BFG_DEATH] );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        string = va ( "</table></p>" );
        len = strlen ( string );
        trap_FS_Write ( string, len, f );

        if ( cgs.gametype != GT_ELIMINATION && cgs.gametype != GT_CTF_ELIMINATION ) {

            string = va ( "<p align=center> <b>Item stats</b>: </p>\n" );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );

            string = va ( "<p align=center><table border=\"1\"><tr><td>Health</td> <td>Armor</td> <td>Mega health</td> <td>Yellow armor</td> <td>Red armor</td></tr>" );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );

            string = va ( "<tr><td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td> <td>%i</td></tr>", clientStats[i][STATS_HEALTH_COUNT], clientStats[i][STATS_ARMOR_COUNT],
                          clientStats[i][STATS_MH_COUNT], clientStats[i][STATS_YA_COUNT], clientStats[i][STATS_RA_COUNT] );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );

            string = va ( "</table></p>" );
            len = strlen ( string );
            trap_FS_Write ( string, len, f );

        }
    }

    string = va ( "</body></html>" );
    len = strlen ( string );
    trap_FS_Write ( string, len, f );

    trap_FS_FCloseFile ( f );


}

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseAccuracy ( void ) {
    int		i;

    for ( i = 0 ; i < 8 ; i++ ) {
        cg.accuracys[i][0] = atoi ( CG_Argv ( i*2 + 1 ) );
        cg.accuracys[i][1] = atoi ( CG_Argv ( i*2 + 2 ) );
    }

}

/*
=================
CG_ParseTimeout

=================
*/
static void CG_ParseTimeout ( void ) {
    qhandle_t sound;
    cgs.timeout = qtrue;
    cgs.timeoutTime = atoi ( CG_Argv ( 1 ) );
    cgs.timeoutAdd = atoi ( CG_Argv ( 2 ) );
    cgs.timeoutDelay = cgs.timeoutDelay + cgs.timeoutAdd;
    CG_Printf ( "Timeout for %f seconds\n", ( ( float ) cgs.timeoutAdd ) /1000.0f );

    sound = trap_S_RegisterSound ( "sound/movers/doors/dr1_end.wav", qfalse );
    trap_S_StartLocalSound ( sound, CHAN_LOCAL_SOUND );

}

/*
=================
CG_ParseTimein

=================
*/
static void CG_ParseTimein ( void ) {
    cgs.timeout = qfalse;
}

/*
=================
CG_ParseRespawnTimer

=================
*/
static void CG_ParseRespawnTimer ( void ) {
    int		entityNum;
    int 		itemType;
    int 		quantity;
    int		respawnTime;
    int		nextItem;
    qboolean	found;
    int 		i;
    int		team;
    int clientNum;

    found = qfalse;

    entityNum = atoi ( CG_Argv ( 1 ) );
    itemType = atoi ( CG_Argv ( 2 ) );
    quantity = atoi ( CG_Argv ( 3 ) );
    respawnTime = atoi ( CG_Argv ( 4 ) );
    nextItem = atoi ( CG_Argv ( 5 ) );
    team = atoi ( CG_Argv ( 6 ) );
    clientNum = atoi ( CG_Argv ( 7 ) );



    if ( ( itemType == IT_ARMOR && quantity >= 50 ) || ( itemType == IT_HEALTH && quantity == 100 ) || itemType == IT_POWERUP ) {
        for ( i = 0 ; i < MAX_RESPAWN_TIMERS && cgs.respawnTimerUsed[i] ; i++ ) {
            if ( cgs.respawnTimerEntitynum[ i ] == entityNum ) {
                cgs.respawnTimerTime[ i ] = respawnTime;
		cgs.respawnTimerClientNum[ i ] = clientNum;
                found = qtrue;
            }
        }
        if ( !found && cg_items[cg_entities[entityNum].currentState.modelindex].registered ) {
            cgs.respawnTimerUsed[ i ] = qtrue;
            cgs.respawnTimerQuantity[ i ] = quantity;
            cgs.respawnTimerTime[ i ] = respawnTime;
            cgs.respawnTimerType[ i ] = itemType;
            cgs.respawnTimerEntitynum[ i ] = entityNum;
            cgs.respawnTimerNumber = i + 1;
            cgs.respawnTimerNextItem[ i ] = nextItem;
            cgs.respawnTimerTeam[ i ] = team;
	    cgs.respawnTimerClientNum[ i ] = clientNum;
        }
    }

}

/*
=================
CG_ParseRespawnTimer

=================
*/
static void CG_ParseReadyMask ( void ) {
    int readyMask, i;
    readyMask = atoi ( CG_Argv ( 1 ) );

    if ( cg.warmup >= 0 )
        return;

    if ( readyMask != cg.readyMask ) {
        for ( i = 0; i < 32 ; i++ ) {
            if ( ( cg.readyMask & ( 1 << i ) ) != ( readyMask & ( 1 << i ) ) ) {

                if ( readyMask & ( 1 << i ) )
                    CG_CenterPrint ( va ( "%s ^2is ready", cgs.clientinfo[ i ].name ), 120, BIGCHAR_WIDTH );
                else
                    CG_CenterPrint ( va ( "%s ^1is not ready", cgs.clientinfo[ i ].name ), 120, BIGCHAR_WIDTH );
            }
        }
        cg.readyMask = readyMask;
    }
}

/*
=================
CG_ParseElimination

=================
*/
static void CG_ParseElimination ( void ) {
    if ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ) {
        cgs.scores1 = atoi ( CG_Argv ( 1 ) );
        cgs.scores2 = atoi ( CG_Argv ( 2 ) );
    }
    cgs.roundStartTime = atoi ( CG_Argv ( 3 ) );
}

/*
=================
CG_ParseMappage
Sago: This parses values from the server rather directly. Some checks are performed, but beware if you change it or new
security holes are found
=================
*/
static void CG_ParseMappage ( void ) {
    char command[1024];
    const char *temp;
    const char*	c;
    int i;

    temp = CG_Argv ( 1 );
    for ( c = temp; *c; ++c ) {
        switch ( *c ) {
        case '\n':
        case '\r':
        case ';':
            //The server tried something bad!
            return;
            break;
        }
    }
    Q_strncpyz ( command,va ( "ui_mappage %s",temp ),1024 );
    for ( i=2;i<12;i++ ) {
        temp = CG_Argv ( i );
        for ( c = temp; *c; ++c ) {
            switch ( *c ) {
            case '\n':
            case '\r':
            case ';':
                //The server tried something bad!
                return;
                break;
            }
        }
        if ( strlen ( temp ) <1 )
            temp = "---";
        Q_strcat ( command,1024,va ( " %s ",temp ) );
    }
    trap_SendConsoleCommand ( command );

}

/*
=================
CG_ParseDDtimetaken

=================
*/
static void CG_ParseDDtimetaken ( void ) {
    cgs.timetaken = atoi ( CG_Argv ( 1 ) );
}

/*
=================
CG_ParseDomPointNames
=================
*/

static void CG_ParseDomPointNames ( void ) {
    int i,j;
    cgs.domination_points_count = atoi ( CG_Argv ( 1 ) );
    if ( cgs.domination_points_count>=MAX_DOMINATION_POINTS )
        cgs.domination_points_count = MAX_DOMINATION_POINTS;
    for ( i = 0;i<cgs.domination_points_count;i++ ) {
        Q_strncpyz ( cgs.domination_points_names[i],CG_Argv ( 2 ) +i*MAX_DOMINATION_POINTS_NAMES,MAX_DOMINATION_POINTS_NAMES-1 );
        for ( j=MAX_DOMINATION_POINTS_NAMES-1; cgs.domination_points_names[i][j] < '0' && j>0; j-- ) {
            cgs.domination_points_names[i][j] = 0;
        }
    }
}

/*
=================
CG_ParseDomScores
=================
*/

static void CG_ParseDomStatus ( void ) {
    int i;
    if ( cgs.domination_points_count!=atoi ( CG_Argv ( 1 ) ) ) {
        cgs.domination_points_count = 0;
        return;
    }
    for ( i = 0;i<cgs.domination_points_count;i++ ) {
        cgs.domination_points_status[i] = atoi ( CG_Argv ( 2+i ) );
    }
}

/*
=================
CG_ParseChallenge
=================
*/

static void CG_ParseChallenge ( void ) {
    addChallenge ( atoi ( CG_Argv ( 1 ) ) );
}

/*
=================
CG_ParseRewards
=================
*/

static void CG_ParseRewards ( void ) {
    int reward, rewardCount;

    if ( !cg_newRewards.integer )
        return;

    reward = atoi ( CG_Argv ( 1 ) );
    rewardCount = atoi ( CG_Argv ( 2 ) );
    if ( reward == REWARD_AIRROCKET ) {
        pushReward ( cgs.media.airrocketSound, cgs.media.medalAirrocket, rewardCount );
    } else if ( reward == REWARD_AIRGRENADE ) {
        pushReward ( cgs.media.airgrenadeSound, cgs.media.medalAirgrenade, rewardCount );
    } else if ( reward == REWARD_RLRG ) {
        pushReward ( cgs.media.impressiveSound, cgs.media.medalRocketrail, rewardCount );
    } else if ( reward == REWARD_FULLSG ) {
        pushReward ( cgs.media.airrocketSound, cgs.media.medalFullsg, rewardCount );
    } else if ( reward == REWARD_ITEMDENIED ) {
        pushReward ( cgs.media.deniedSound, cgs.media.medalItemdenied, rewardCount );
    } else if ( reward == REWARD_SPAWNKILL ) {
        pushReward ( cgs.media.spawnkillSound, cgs.media.medalSpawnkill, rewardCount );
    } else if ( reward == REWARD_LGACCURACY ) {
        pushReward ( cgs.media.lgaccuracySound , cgs.media.medalLgaccuracy, rewardCount );
    } else if ( reward == REWARD_DOUBLE_AIRROCKET ) {
        pushReward ( cgs.media.impressiveSound , cgs.media.medalDoubleAirrocket, rewardCount );
    }
}

/*
=================
CG_ParseTeam
=================
*/

static void CG_ParseTeam ( void ) {
    //TODO: Add code here
    if ( cg_voip_teamonly.integer )
        trap_Cvar_Set ( "cl_voipSendTarget",CG_Argv ( 1 ) );
}

/*
=================
CG_ParseAttackingTeam

=================
*/
static void CG_ParseAttackingTeam ( void ) {
    int temp;
    temp = atoi ( CG_Argv ( 1 ) );
    if ( temp==TEAM_RED )
        cgs.attackingTeam = TEAM_RED;
    else if ( temp==TEAM_BLUE )
        cgs.attackingTeam = TEAM_BLUE;
    else
        cgs.attackingTeam = TEAM_NONE; //Should never happen.
}

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo ( void ) {
    int		i;
    int		client;

    numSortedTeamPlayers = atoi ( CG_Argv ( 1 ) );
    if ( numSortedTeamPlayers < 0 || numSortedTeamPlayers > TEAM_MAXOVERLAY ) {
        CG_Error ( "CG_ParseTeamInfo: numSortedTeamPlayers out of range (%d)",
                   numSortedTeamPlayers );
        return;
    }

    for ( i = 0 ; i < numSortedTeamPlayers ; i++ ) {
        client = atoi ( CG_Argv ( i * 6 + 2 ) );
        if ( client < 0 || client >= MAX_CLIENTS ) {
            CG_Error ( "CG_ParseTeamInfo: bad client number: %d", client );
            return;
        }


        sortedTeamPlayers[i] = client;

        cgs.clientinfo[ client ].location = atoi ( CG_Argv ( i * 6 + 3 ) );
        cgs.clientinfo[ client ].health = atoi ( CG_Argv ( i * 6 + 4 ) );
        cgs.clientinfo[ client ].armor = atoi ( CG_Argv ( i * 6 + 5 ) );
        cgs.clientinfo[ client ].curWeapon = atoi ( CG_Argv ( i * 6 + 6 ) );
        cgs.clientinfo[ client ].powerups = atoi ( CG_Argv ( i * 6 + 7 ) );
    }
}


/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo ( void ) {
    const char	*info;
    char	*mapname;
    char 	buf[128];

    info = CG_ConfigString ( CS_SERVERINFO );
    cgs.gametype = atoi ( Info_ValueForKey ( info, "g_gametype" ) );
    //By default do as normal:
    cgs.ffa_gt = 0;
    //See if ffa gametype
    if ( cgs.gametype == GT_LMS )
        cgs.ffa_gt = 1;
    trap_Cvar_Set ( "g_gametype", va ( "%i", cgs.gametype ) );
    cgs.dmflags = atoi ( Info_ValueForKey ( info, "dmflags" ) );
    cgs.fairflags = atoi ( Info_ValueForKey ( info, "fairflags" ) );
    cgs.elimflags = atoi ( Info_ValueForKey ( info, "elimflags" ) );
    cgs.teamflags = atoi ( Info_ValueForKey ( info, "teamflags" ) );
    cgs.fraglimit = atoi ( Info_ValueForKey ( info, "fraglimit" ) );
    cgs.capturelimit = atoi ( Info_ValueForKey ( info, "capturelimit" ) );
    cgs.timelimit = atoi ( Info_ValueForKey ( info, "timelimit" ) );
    cgs.maxclients = atoi ( Info_ValueForKey ( info, "sv_maxclients" ) );
    cgs.roundtime = atoi ( Info_ValueForKey ( info, "elimination_roundtime" ) );
    cgs.nopickup = atoi ( Info_ValueForKey ( info, "g_rockets" ) ) + atoi ( Info_ValueForKey ( info, "g_instantgib" ) ) + atoi ( Info_ValueForKey ( info, "g_elimination" ) );
    cgs.lms_mode = atoi ( Info_ValueForKey ( info, "g_lms_mode" ) );
    cgs.altExcellent = atoi ( Info_ValueForKey ( info, "g_altExcellent" ) );
    cgs.overtime = atoi ( Info_ValueForKey( info, "g_overtime" ) );
    mapname = Info_ValueForKey ( info, "mapname" );
    Com_sprintf ( cgs.mapname, sizeof ( cgs.mapname ), "maps/%s.bsp", mapname );
    Q_strncpyz ( cgs.redTeam, Info_ValueForKey ( info, "g_redTeam" ), sizeof ( cgs.redTeam ) );
    trap_Cvar_Set ( "g_redTeam", cgs.redTeam );
    Q_strncpyz ( cgs.blueTeam, Info_ValueForKey ( info, "g_blueTeam" ), sizeof ( cgs.blueTeam ) );
    trap_Cvar_Set ( "g_blueTeam", cgs.blueTeam );

//unlagged - server options
    // we'll need this for deciding whether or not to predict weapon effects
    cgs.delagHitscan = atoi ( Info_ValueForKey ( info, "g_delagHitscan" ) );
    trap_Cvar_Set ( "g_delagHitscan", va ( "%i", cgs.delagHitscan ) );
//unlagged - server options

    //Copy allowed votes directly to the client:
    trap_Cvar_Set ( "cg_voteflags",Info_ValueForKey ( info, "voteflags" ) );

    cgs.newItemHeight = atoi ( Info_ValueForKey ( info, "g_newItemHeight" ) );
    trap_Cvar_Set ( "g_newItemHeight", va ( "%i", cgs.newItemHeight ) );

    cgs.startWhenReady = atoi ( Info_ValueForKey ( info, "g_startWhenReady" ) );
    trap_Cvar_Set ( "g_startWhenReady", va ( "%i", cgs.startWhenReady ) );

    cgs.allowMultiview = atoi ( Info_ValueForKey ( info, "g_allowMultiview" ) );
    trap_Cvar_Set ( "g_allowMultiview", va ( "%i", cgs.allowMultiview ) );

    if ( cg_autosnaps.integer )
        trap_Cvar_Set ( "snaps", va ( "%i", atoi ( Info_ValueForKey ( info, "sv_fps" ) ) ) );

    memset(buf, 0, sizeof(buf));
    trap_Cvar_VariableStringBuffer("cl_maxpackets", buf, sizeof(buf));
    
    if( atoi(buf) < atoi ( Info_ValueForKey ( info, "sv_fps" ) )*1.5f ){
	    CG_Printf("cl_maxpackets low, setting cl_maxpackets to %i (1.5*sv_fps)\n", (int)( atoi ( Info_ValueForKey ( info, "sv_fps" ) ) * 1.5f ) );
	    trap_Cvar_Set ( "cl_maxpackets", va ( "%i", (int)( atoi ( Info_ValueForKey ( info, "sv_fps" ) ) * 1.5f ) ) );
    }

    trap_Cvar_Set ( "g_promode",va("%i",atoi (Info_ValueForKey ( info, "g_promode" ))) );

    cgs.friendsThroughWalls = atoi ( Info_ValueForKey ( info, "g_friendsThroughWalls" ) );
    trap_Cvar_Set ( "g_friendsThroughWalls", va ( "%i", cgs.friendsThroughWalls ) );

    cgs.redLocked = atoi ( Info_ValueForKey ( info, "g_redLocked" ) );
    trap_Cvar_Set ( "g_redLocked", va ( "%i", cgs.redLocked ) );

    cgs.blueLocked = atoi ( Info_ValueForKey ( info, "g_blueLocked" ) );
    trap_Cvar_Set ( "g_blueLocked", va ( "%i", cgs.blueLocked ) );
    
    cgs.fadeToBlack = atoi ( Info_ValueForKey ( info, "g_fadeToBlack" ) );
    trap_Cvar_Set ( "g_fadeToBlack", va ( "%i", cgs.fadeToBlack ) );
    
}

/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup ( void ) {
    const char	*info;
    int			warmup;

    info = CG_ConfigString ( CS_WARMUP );

    warmup = atoi ( info );
    cg.warmupCount = -1;

    if ( warmup == 0 && cg.warmup ) {

    } else if ( warmup > 0 && cg.warmup <= 0 ) {
#ifdef MISSIONPACK
        if ( cgs.gametype >= GT_CTF && cgs.gametype < GT_MAX_GAME_TYPE && !cgs.ffa_gt ) {
            trap_S_StartLocalSound ( cgs.media.countPrepareTeamSound, CHAN_ANNOUNCER );
        } else
#endif
        {
            trap_S_StartLocalSound ( cgs.media.countPrepareSound, CHAN_ANNOUNCER );
        }
    }

    cg.warmup = warmup;
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues ( void ) {
    const char *s;

    cgs.scores1 = atoi ( CG_ConfigString ( CS_SCORES1 ) );
    cgs.scores2 = atoi ( CG_ConfigString ( CS_SCORES2 ) );
    cgs.levelStartTime = atoi ( CG_ConfigString ( CS_LEVEL_START_TIME ) );
    if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION || cgs.gametype == GT_DOUBLE_D ) {
        s = CG_ConfigString ( CS_FLAGSTATUS );
        cgs.redflag = s[0] - '0';
        cgs.blueflag = s[1] - '0';
    }
#ifdef MISSIONPACK
    else if ( cgs.gametype == GT_1FCTF ) {
        s = CG_ConfigString ( CS_FLAGSTATUS );
        cgs.flagStatus = s[0] - '0';
    }
#endif
    cg.warmup = atoi ( CG_ConfigString ( CS_WARMUP ) );
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged ( void ) {
    char originalShader[MAX_QPATH];
    char newShader[MAX_QPATH];
    char timeOffset[16];
    const char *o;
    char *n,*t;

    o = CG_ConfigString ( CS_SHADERSTATE );
    while ( o && *o ) {
        n = strstr ( o, "=" );
        if ( n && *n ) {
            strncpy ( originalShader, o, n-o );
            originalShader[n-o] = 0;
            n++;
            t = strstr ( n, ":" );
            if ( t && *t ) {
                strncpy ( newShader, n, t-n );
                newShader[t-n] = 0;
            } else {
                break;
            }
            t++;
            o = strstr ( t, "@" );
            if ( o ) {
                strncpy ( timeOffset, t, o-t );
                timeOffset[o-t] = 0;
                o++;
                trap_R_RemapShader ( originalShader, newShader, timeOffset );
            }
        } else {
            break;
        }
    }
}

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified ( void ) {
    const char	*str;
    int		num;

    num = atoi ( CG_Argv ( 1 ) );

    // get the gamestate from the client system, which will have the
    // new configstring already integrated
    trap_GetGameState ( &cgs.gameState );

    // look up the individual string that was modified
    str = CG_ConfigString ( num );

    // do something with it if necessary
    if ( num == CS_MUSIC ) {
        CG_StartMusic();
    } else if ( num == CS_SERVERINFO ) {
        CG_ParseServerinfo();
    } else if ( num == CS_WARMUP ) {
        CG_ParseWarmup();
    } else if ( num == CS_SCORES1 ) {
        cgs.scores1 = atoi ( str );
    } else if ( num == CS_SCORES2 ) {
        cgs.scores2 = atoi ( str );
    } else if ( num == CS_LEVEL_START_TIME ) {
        cgs.levelStartTime = atoi ( str );
    } else if ( num == CS_VOTE_TIME ) {
        cgs.voteTime = atoi ( str );
        cgs.voteModified = qtrue;
    } else if ( num == CS_VOTE_YES ) {
        cgs.voteYes = atoi ( str );
        cgs.voteModified = qtrue;
    } else if ( num == CS_VOTE_NO ) {
        cgs.voteNo = atoi ( str );
        cgs.voteModified = qtrue;
    } else if ( num == CS_VOTE_STRING ) {
        Q_strncpyz ( cgs.voteString, str, sizeof ( cgs.voteString ) );
//#ifdef MISSIONPACK
        trap_S_StartLocalSound ( cgs.media.voteNow, CHAN_ANNOUNCER );
//#endif //MISSIONPACK
    } else if ( num >= CS_TEAMVOTE_TIME && num <= CS_TEAMVOTE_TIME + 1 ) {
        cgs.teamVoteTime[num-CS_TEAMVOTE_TIME] = atoi ( str );
        cgs.teamVoteModified[num-CS_TEAMVOTE_TIME] = qtrue;
    } else if ( num >= CS_TEAMVOTE_YES && num <= CS_TEAMVOTE_YES + 1 ) {
        cgs.teamVoteYes[num-CS_TEAMVOTE_YES] = atoi ( str );
        cgs.teamVoteModified[num-CS_TEAMVOTE_YES] = qtrue;
    } else if ( num >= CS_TEAMVOTE_NO && num <= CS_TEAMVOTE_NO + 1 ) {
        cgs.teamVoteNo[num-CS_TEAMVOTE_NO] = atoi ( str );
        cgs.teamVoteModified[num-CS_TEAMVOTE_NO] = qtrue;
    } else if ( num >= CS_TEAMVOTE_STRING && num <= CS_TEAMVOTE_STRING + 1 ) {
        Q_strncpyz ( cgs.teamVoteString[num-CS_TEAMVOTE_STRING], str, sizeof ( cgs.teamVoteString[0] ) );
//#ifdef MISSIONPACK
        trap_S_StartLocalSound ( cgs.media.voteNow, CHAN_ANNOUNCER );
//#endif
    } else if ( num == CS_INTERMISSION ) {
        cg.intermissionStarted = atoi ( str );
    } else if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) {
        cgs.gameModels[ num-CS_MODELS ] = trap_R_RegisterModel ( str );
    } else if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) {
        if ( str[0] != '*' ) {	// player specific sounds don't register here
            cgs.gameSounds[ num-CS_SOUNDS] = trap_S_RegisterSound ( str, qfalse );
        }
    } else if ( num >= CS_PLAYERS && num < CS_PLAYERS+MAX_CLIENTS ) {
        CG_NewClientInfo ( num - CS_PLAYERS );
        CG_BuildSpectatorString();
    } else if ( num == CS_FLAGSTATUS ) {
        if ( ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION || cgs.gametype == GT_DOUBLE_D ) && cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR ) {
            // format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
            if ( cgs.redflag == 1 && ( ( str[0] - '0' ) == 2 ) ) {
                if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED ) {
                    CG_CenterPrint ( va ( "^2The enemy dropped your flag" ), 100, SMALLCHAR_WIDTH );
                } else {
                    CG_CenterPrint ( va ( "^1Your team dropped the flag" ), 100, SMALLCHAR_WIDTH );
                }
            }
            if ( cgs.blueflag == 1 && ( ( str[1] - '0' ) == 2 ) ) {
                if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE ) {
                    CG_CenterPrint ( va ( "^2The enemy dropped your flag" ), 100, SMALLCHAR_WIDTH );
                } else {
                    CG_CenterPrint ( va ( "^1Your team dropped the flag" ), 100, SMALLCHAR_WIDTH );
                }
            }

            cgs.redflag = str[0] - '0';
            cgs.blueflag = str[1] - '0';
        }
#ifdef MISSIONPACK
        else if ( cgs.gametype == GT_1FCTF ) {
            cgs.flagStatus = str[0] - '0';
        }
#endif
    } else if ( num == CS_SHADERSTATE ) {
        CG_ShaderStateChanged();
    } else if ( num == CS_PROMODE ) {
        CPM_UpdateSettings( atoi ( str ) );
    }

}


/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat ( const char *str ) {
    int len;
    char *p, *ls;
    int lastcolor;
    int chatHeight = TEAMCHAT_HEIGHT;

    if ( cg_teamChatTime.integer <= 0 ) {
        // team chat disabled, dump into normal chat
        cgs.teamChatPos = cgs.teamLastChatPos = 0;
        return;
    }

    len = 0;

    p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
    *p = 0;

    lastcolor = '7';

    ls = NULL;
    while ( *str ) {
        if ( len > TEAMCHAT_WIDTH - 1 ) {
            if ( ls ) {
                str -= ( p - ls );
                str++;
                p -= ( p - ls );
            }
            *p = 0;

            cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;

            cgs.teamChatPos++;
            p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
            *p = 0;
            *p++ = Q_COLOR_ESCAPE;
            *p++ = lastcolor;
            len = 0;
            ls = NULL;
        }

        if ( Q_IsColorString ( str ) ) {
            *p++ = *str++;
            lastcolor = *str;
            *p++ = *str++;
            continue;
        }
        if ( *str == ' ' ) {
            ls = p;
        }
        *p++ = *str++;
        len++;
    }
    *p = 0;

    cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
    cgs.teamChatPos++;

    if ( cgs.teamChatPos - cgs.teamLastChatPos > chatHeight )
        cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
}

/*
=======================
CG_AddToConsole

=======================
*/
void CG_AddToConsole ( const char *str ) {
    //char *str;
    int len;
    char *p, *ls;
    int lastcolor;
    int consoleHeight = TEAMCHAT_HEIGHT;

    /*strcpy( str, in );
    len = strlen(str);

    str[len-1]='\0';*/

    if ( cg_consoleTime.integer <= 0 ) {
        // team chat disabled, dump into normal chat
        cgs.consolePos = cgs.lastConsolePos = 0;
        return;
    }

    len = 0;

    p = cgs.consoleMsgs[cgs.consolePos % consoleHeight];
    *p = 0;

    lastcolor = '7';

    ls = NULL;
    while ( *str ) {
        if ( len > TEAMCHAT_WIDTH - 1 ) {
            if ( ls ) {
                str -= ( p - ls );
                str++;
                p -= ( p - ls );
            }
            *p = 0;

            cgs.consoleMsgTimes[cgs.consolePos % consoleHeight] = cg.time;

            cgs.consolePos++;
            p = cgs.consoleMsgs[cgs.consolePos % consoleHeight];
            *p = 0;
            *p++ = Q_COLOR_ESCAPE;
            *p++ = lastcolor;
            len = 0;
            ls = NULL;
        }

        if ( Q_IsColorString ( str ) ) {
            *p++ = *str++;
            lastcolor = *str;
            *p++ = *str++;
            continue;
        }
        if ( *str == ' ' ) {
            ls = p;
        }
        if ( *str == '\n' ) {
            str++;
        }
        *p++ = *str++;
        len++;
    }
    *p = 0;

    cgs.consoleMsgTimes[cgs.consolePos % consoleHeight] = cg.time;
    cgs.consolePos++;

    if ( cgs.consolePos - cgs.lastConsolePos > consoleHeight )
        cgs.lastConsolePos = cgs.consolePos - consoleHeight;
}

/*
=======================
CG_AddToChat

=======================
*/
void CG_AddToChat ( const char *str ) {
    int len;
    char *p, *ls;
    int lastcolor;
    int chatHeight = TEAMCHAT_HEIGHT;

    if ( cg_chatTime.integer <= 0 ) {
        // team chat disabled, dump into normal chat
        cgs.chatPos = cgs.lastChatPos = 0;
        return;
    }

    len = 0;

    p = cgs.chatMsgs[cgs.chatPos % chatHeight];
    *p = 0;

    lastcolor = '7';

    ls = NULL;
    while ( *str ) {
        if ( len > TEAMCHAT_WIDTH - 1 ) {
            if ( ls ) {
                str -= ( p - ls );
                str++;
                p -= ( p - ls );
            }
            *p = 0;

            cgs.chatMsgTimes[cgs.chatPos % chatHeight] = cg.time;

            cgs.chatPos++;
            p = cgs.chatMsgs[cgs.chatPos % chatHeight];
            *p = 0;
            *p++ = Q_COLOR_ESCAPE;
            *p++ = lastcolor;
            len = 0;
            ls = NULL;
        }

        if ( Q_IsColorString ( str ) ) {
            *p++ = *str++;
            lastcolor = *str;
            *p++ = *str++;
            continue;
        }
        if ( *str == ' ' ) {
            ls = p;
        }
        *p++ = *str++;
        len++;
    }
    *p = 0;

    cgs.chatMsgTimes[cgs.chatPos % chatHeight] = cg.time;
    cgs.chatPos++;

    if ( cgs.chatPos - cgs.lastChatPos > chatHeight )
        cgs.lastChatPos = cgs.chatPos - chatHeight;
}

/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart ( void ) {
    int i;
    i = 0;

    if ( cg_showmiss.integer ) {
        CG_Printf ( "CG_MapRestart\n" );
    }

    CG_InitLocalEntities();
    CG_InitMarkPolys();
    CG_ClearParticles ();

    // make sure the "3 frags left" warnings play again
    cg.fraglimitWarnings = 0;

    cg.timelimitWarnings = 0;

    cg.rewardTime = 0;

    cg.rewardStack = 0;

    cg.intermissionStarted = qfalse;

    cgs.voteTime = 0;

    cg.mapRestart = qtrue;

    CG_StartMusic();

    trap_S_ClearLoopingSounds ( qtrue );

    // we really should clear more parts of cg here and stop sounds

    // play the "fight" sound if this is a restart without warmup
    if ( cg.warmup == 0 /* && cgs.gametype == GT_TOURNAMENT */ ) {
        trap_S_StartLocalSound ( cgs.media.countFightSound, CHAN_ANNOUNCER );
        CG_CenterPrint ( "FIGHT!", 120, GIANTCHAR_WIDTH*2 );
    }
#ifdef MISSIONPACK
    if ( cg_singlePlayerActive.integer ) {
        trap_Cvar_Set ( "ui_matchStartTime", va ( "%i", cg.time ) );
        if ( cg_recordSPDemo.integer && cg_recordSPDemoName.string && *cg_recordSPDemoName.string ) {
            trap_SendConsoleCommand ( va ( "set g_synchronousclients 1 ; record %s \n", cg_recordSPDemoName.string ) );
        }
    }
#endif
    trap_Cvar_Set ( "cg_thirdPerson", "0" );

    for ( i = 0 ; i < MAX_RESPAWN_TIMERS ; i++ ) {
        cgs.respawnTimerEntitynum[i] = -1;
        cgs.respawnTimerQuantity[i] = 0;
        cgs.respawnTimerTime[i] = -1;
        cgs.respawnTimerType[i] = -1;
        cgs.respawnTimerUsed[i] = qfalse;
        cgs.respawnTimerNextItem[i] = -1;
    }
    cgs.respawnTimerNumber = 0;

    cg.readyMask = 0;

    cgs.timeoutAdd = 0;
    cgs.timeoutDelay = 0;

}

#define MAX_VOICEFILESIZE	16384
#define MAX_VOICEFILES		8
#define MAX_VOICECHATS		64
#define MAX_VOICESOUNDS		64
#define MAX_CHATSIZE		64
#define MAX_HEADMODELS		64

typedef struct voiceChat_s {
    char id[64];
    int numSounds;
    sfxHandle_t sounds[MAX_VOICESOUNDS];
    char chats[MAX_VOICESOUNDS][MAX_CHATSIZE];
} voiceChat_t;

typedef struct voiceChatList_s {
    char name[64];
    int gender;
    int numVoiceChats;
    voiceChat_t voiceChats[MAX_VOICECHATS];
} voiceChatList_t;

typedef struct headModelVoiceChat_s {
    char headmodel[64];
    int voiceChatNum;
} headModelVoiceChat_t;

voiceChatList_t voiceChatLists[MAX_VOICEFILES];
headModelVoiceChat_t headModelVoiceChat[MAX_HEADMODELS];

/*
=================
CG_ParseVoiceChats
=================
*/
int CG_ParseVoiceChats ( const char *filename, voiceChatList_t *voiceChatList, int maxVoiceChats ) {
    int	len, i;
    fileHandle_t f;
    char buf[MAX_VOICEFILESIZE];
    char **p, *ptr;
    char *token;
    voiceChat_t *voiceChats;
    qboolean compress;
    sfxHandle_t sound;

    compress = qtrue;
    if ( cg_buildScript.integer ) {
        compress = qfalse;
    }

    len = trap_FS_FOpenFile ( filename, &f, FS_READ );
    if ( !f ) {
        trap_Print ( va ( S_COLOR_RED "voice chat file not found: %s\n", filename ) );
        return qfalse;
    }
    if ( len >= MAX_VOICEFILESIZE ) {
        trap_Print ( va ( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
        trap_FS_FCloseFile ( f );
        return qfalse;
    }

    trap_FS_Read ( buf, len, f );
    buf[len] = 0;
    trap_FS_FCloseFile ( f );

    ptr = buf;
    p = &ptr;

    Com_sprintf ( voiceChatList->name, sizeof ( voiceChatList->name ), "%s", filename );
    voiceChats = voiceChatList->voiceChats;
    for ( i = 0; i < maxVoiceChats; i++ ) {
        voiceChats[i].id[0] = 0;
    }
    token = COM_ParseExt ( p, qtrue );
    if ( !token || token[0] == 0 ) {
        return qtrue;
    }
    if ( !Q_stricmp ( token, "female" ) ) {
        voiceChatList->gender = GENDER_FEMALE;
    } else if ( !Q_stricmp ( token, "male" ) ) {
        voiceChatList->gender = GENDER_MALE;
    } else if ( !Q_stricmp ( token, "neuter" ) ) {
        voiceChatList->gender = GENDER_NEUTER;
    } else {
        trap_Print ( va ( S_COLOR_RED "expected gender not found in voice chat file: %s\n", filename ) );
        return qfalse;
    }

    voiceChatList->numVoiceChats = 0;
    while ( 1 ) {
        token = COM_ParseExt ( p, qtrue );
        if ( !token || token[0] == 0 ) {
            return qtrue;
        }
        Com_sprintf ( voiceChats[voiceChatList->numVoiceChats].id, sizeof ( voiceChats[voiceChatList->numVoiceChats].id ), "%s", token );
        token = COM_ParseExt ( p, qtrue );
        if ( Q_stricmp ( token, "{" ) ) {
            trap_Print ( va ( S_COLOR_RED "expected { found %s in voice chat file: %s\n", token, filename ) );
            return qfalse;
        }
        voiceChats[voiceChatList->numVoiceChats].numSounds = 0;
        while ( 1 ) {
            token = COM_ParseExt ( p, qtrue );
            if ( !token || token[0] == 0 ) {
                return qtrue;
            }
            if ( !Q_stricmp ( token, "}" ) )
                break;
            sound = trap_S_RegisterSound ( token, compress );
            voiceChats[voiceChatList->numVoiceChats].sounds[voiceChats[voiceChatList->numVoiceChats].numSounds] = sound;
            token = COM_ParseExt ( p, qtrue );
            if ( !token || token[0] == 0 ) {
                return qtrue;
            }
            Com_sprintf ( voiceChats[voiceChatList->numVoiceChats].chats[
                              voiceChats[voiceChatList->numVoiceChats].numSounds], MAX_CHATSIZE, "%s", token );
            if ( sound )
                voiceChats[voiceChatList->numVoiceChats].numSounds++;
            if ( voiceChats[voiceChatList->numVoiceChats].numSounds >= MAX_VOICESOUNDS )
                break;
        }
        voiceChatList->numVoiceChats++;
        if ( voiceChatList->numVoiceChats >= maxVoiceChats )
            return qtrue;
    }
    return qtrue;
}

/*
=================
CG_LoadVoiceChats
=================
*/
void CG_LoadVoiceChats ( void ) {
    int size;

    size = trap_MemoryRemaining();
    CG_ParseVoiceChats ( "scripts/female1.voice", &voiceChatLists[0], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/female2.voice", &voiceChatLists[1], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/female3.voice", &voiceChatLists[2], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/male1.voice", &voiceChatLists[3], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/male2.voice", &voiceChatLists[4], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/male3.voice", &voiceChatLists[5], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/male4.voice", &voiceChatLists[6], MAX_VOICECHATS );
    CG_ParseVoiceChats ( "scripts/male5.voice", &voiceChatLists[7], MAX_VOICECHATS );
    CG_Printf ( "voice chat memory size = %d\n", size - trap_MemoryRemaining() );
}

/*
=================
CG_HeadModelVoiceChats
=================
*/
int CG_HeadModelVoiceChats ( char *filename ) {
    int	len, i;
    fileHandle_t f;
    char buf[MAX_VOICEFILESIZE];
    char **p, *ptr;
    char *token;

    len = trap_FS_FOpenFile ( filename, &f, FS_READ );
    if ( !f ) {
        //trap_Print( va( "voice chat file not found: %s\n", filename ) );
        return -1;
    }
    if ( len >= MAX_VOICEFILESIZE ) {
        trap_Print ( va ( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
        trap_FS_FCloseFile ( f );
        return -1;
    }

    trap_FS_Read ( buf, len, f );
    buf[len] = 0;
    trap_FS_FCloseFile ( f );

    ptr = buf;
    p = &ptr;

    token = COM_ParseExt ( p, qtrue );
    if ( !token || token[0] == 0 ) {
        return -1;
    }

    for ( i = 0; i < MAX_VOICEFILES; i++ ) {
        if ( !Q_stricmp ( token, voiceChatLists[i].name ) ) {
            return i;
        }
    }

    //FIXME: maybe try to load the .voice file which name is stored in token?

    return -1;
}


/*
=================
CG_GetVoiceChat
=================
*/
int CG_GetVoiceChat ( voiceChatList_t *voiceChatList, const char *id, sfxHandle_t *snd, char **chat ) {
    int i, rnd;

    for ( i = 0; i < voiceChatList->numVoiceChats; i++ ) {
        if ( !Q_stricmp ( id, voiceChatList->voiceChats[i].id ) ) {
            rnd = random() * voiceChatList->voiceChats[i].numSounds;
            *snd = voiceChatList->voiceChats[i].sounds[rnd];
            *chat = voiceChatList->voiceChats[i].chats[rnd];
            return qtrue;
        }
    }
    return qfalse;
}

/*
=================
CG_VoiceChatListForClient
=================
*/
voiceChatList_t *CG_VoiceChatListForClient ( int clientNum ) {
    clientInfo_t *ci;
    int voiceChatNum, i, j, k, gender;
    char filename[MAX_QPATH], headModelName[MAX_QPATH];

    if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
        clientNum = 0;
    }
    ci = &cgs.clientinfo[ clientNum ];

    for ( k = 0; k < 2; k++ ) {
        if ( k == 0 ) {
            if ( ci->headModelName[0] == '*' ) {
                Com_sprintf ( headModelName, sizeof ( headModelName ), "%s/%s", ci->headModelName+1, ci->headSkinName );
            } else {
                Com_sprintf ( headModelName, sizeof ( headModelName ), "%s/%s", ci->headModelName, ci->headSkinName );
            }
        } else {
            if ( ci->headModelName[0] == '*' ) {
                Com_sprintf ( headModelName, sizeof ( headModelName ), "%s", ci->headModelName+1 );
            } else {
                Com_sprintf ( headModelName, sizeof ( headModelName ), "%s", ci->headModelName );
            }
        }
        // find the voice file for the head model the client uses
        for ( i = 0; i < MAX_HEADMODELS; i++ ) {
            if ( !Q_stricmp ( headModelVoiceChat[i].headmodel, headModelName ) ) {
                break;
            }
        }
        if ( i < MAX_HEADMODELS ) {
            return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
        }
        // find a <headmodelname>.vc file
        for ( i = 0; i < MAX_HEADMODELS; i++ ) {
            if ( !strlen ( headModelVoiceChat[i].headmodel ) ) {
                Com_sprintf ( filename, sizeof ( filename ), "scripts/%s.vc", headModelName );
                voiceChatNum = CG_HeadModelVoiceChats ( filename );
                if ( voiceChatNum == -1 )
                    break;
                Com_sprintf ( headModelVoiceChat[i].headmodel, sizeof ( headModelVoiceChat[i].headmodel ),
                              "%s", headModelName );
                headModelVoiceChat[i].voiceChatNum = voiceChatNum;
                return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
            }
        }
    }
    gender = ci->gender;
    for ( k = 0; k < 2; k++ ) {
        // just pick the first with the right gender
        for ( i = 0; i < MAX_VOICEFILES; i++ ) {
            if ( strlen ( voiceChatLists[i].name ) ) {
                if ( voiceChatLists[i].gender == gender ) {
                    // store this head model with voice chat for future reference
                    for ( j = 0; j < MAX_HEADMODELS; j++ ) {
                        if ( !strlen ( headModelVoiceChat[j].headmodel ) ) {
                            Com_sprintf ( headModelVoiceChat[j].headmodel, sizeof ( headModelVoiceChat[j].headmodel ),
                                          "%s", headModelName );
                            headModelVoiceChat[j].voiceChatNum = i;
                            break;
                        }
                    }
                    return &voiceChatLists[i];
                }
            }
        }
        // fall back to male gender because we don't have neuter in the mission pack
        if ( gender == GENDER_MALE )
            break;
        gender = GENDER_MALE;
    }
    // store this head model with voice chat for future reference
    for ( j = 0; j < MAX_HEADMODELS; j++ ) {
        if ( !strlen ( headModelVoiceChat[j].headmodel ) ) {
            Com_sprintf ( headModelVoiceChat[j].headmodel, sizeof ( headModelVoiceChat[j].headmodel ),
                          "%s", headModelName );
            headModelVoiceChat[j].voiceChatNum = 0;
            break;
        }
    }
    // just return the first voice chat list
    return &voiceChatLists[0];
}

void CG_ParseLivingCount ( void ) {
    int newRed, newBlue;
    int countRed, countBlue;

    countRed = atoi ( CG_Argv ( 3 ) );
    countBlue = atoi ( CG_Argv ( 4 ) );

    if ( cg.warmup < 0 ) {
        cgs.redLivingCount = countRed;
        cgs.blueLivingCount = countBlue;
        return;
    }

    newRed = atoi ( CG_Argv ( 1 ) );
    newBlue = atoi ( CG_Argv ( 2 ) );
    countRed = atoi ( CG_Argv ( 3 ) );
    countBlue = atoi ( CG_Argv ( 4 ) );

    if ( newRed == 1 && newRed != cgs.redLivingCount && cgs.clientinfo[cg.clientNum].team == TEAM_RED && !cgs.clientinfo[cg.clientNum].isDead ) {
        if ( countRed != 1 )
            CG_CenterPrint ( va ( "You are the last in your team" ), 100, SMALLCHAR_WIDTH );
    }
    if ( newBlue == 1 && newBlue != cgs.blueLivingCount && cgs.clientinfo[cg.clientNum].team == TEAM_BLUE && !cgs.clientinfo[cg.clientNum].isDead ) {
        if ( countBlue != 1 )
            CG_CenterPrint ( va ( "You are the last in your team" ), 100, SMALLCHAR_WIDTH );
    }

    cgs.redLivingCount = newRed;
    cgs.blueLivingCount = newBlue;
}

/*
=================
CG_RemoveChatEscapeChar
=================
*/
static void CG_RemoveChatEscapeChar ( char *text ) {
    int i, l;

    l = 0;
    for ( i = 0; text[i]; i++ ) {
        if ( text[i] == '\x19' )
            continue;
        text[l++] = text[i];
    }
    text[l] = '\0';
}

static void CG_ParseSpawnpoints( void ){
    int i;
    cg.numSpawnpoints = atoi( CG_Argv(1) );
    for( i = 0; i < cg.numSpawnpoints ; i++ ){
	cg.spawnOrg[i][0] = atof(CG_Argv( 2 + i*7 ));
	cg.spawnOrg[i][1] = atof(CG_Argv( 3 + i*7 ));
	cg.spawnOrg[i][2] = atof(CG_Argv( 4 + i*7 ));
	cg.spawnAngle[i][0] = atof(CG_Argv( 5 + i*7 ));
	cg.spawnAngle[i][1] = atof(CG_Argv( 6 + i*7 ));
	cg.spawnAngle[i][2] = atof(CG_Argv( 7 + i*7 ));
	cg.spawnTeam[i] = atoi(CG_Argv( 8 + i*7 ));
    }
}

/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
static void CG_ServerCommand ( void ) {
    const char	*cmd;
    char		text[MAX_SAY_TEXT];

    cmd = CG_Argv ( 0 );

    if ( !cmd[0] ) {
        // server claimed the command
        return;
    }
    
    if ( !strcmp ( cmd, "loadModel" ) ) {
        CG_ForceModelChange();
        return;
    }
    
    if ( !strcmp ( cmd, "overtime" ) ) {
        trap_S_StartLocalSound ( cgs.media.protectSound, CHAN_ANNOUNCER );
        return;
    }
    
    if ( !strcmp ( cmd, "holyshit" ) ) {
        CG_AddBufferedSound ( cgs.media.airgrenadeSound );
        return;
    }
    
    if ( !strcmp ( cmd, "quadKill" ) ) {
        cg.quadKills = atoi( CG_Argv(1) );
        return;
    }

    if ( !strcmp ( cmd, "endOfGame" ) ) {
        CG_EndOfGame();
        return;
    }

    if ( !strcmp ( cmd, "startOfGame" ) ) {
        CG_StartOfGame();
        return;
    }

    if ( !strcmp ( cmd, "cp" ) ) {
        CG_CenterPrint ( CG_Argv ( 1 ), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
	if( !strcmp ( cmd, "^1Defend!" ) )
		cgs.csStatus = 1;
	if( !strcmp ( cmd, "^2Capture!" ) )
		cgs.csStatus = 2;
        return;
    }

    if ( !strcmp ( cmd, "cs" ) ) {
        CG_ConfigStringModified();
        return;
    }

    if ( !strcmp ( cmd, "print" ) ) {
        CG_Printf ( "%s", CG_Argv ( 1 ) );
//#ifdef MISSIONPACK
        cmd = CG_Argv ( 1 );			// yes, this is obviously a hack, but so is the way we hear about
        // votes passing or failing
        if ( !Q_stricmpn ( cmd, "vote failed", 11 ) || !Q_stricmpn ( cmd, "team vote failed", 16 ) ) {
            trap_S_StartLocalSound ( cgs.media.voteFailed, CHAN_ANNOUNCER );
        } else if ( !Q_stricmpn ( cmd, "vote passed", 11 ) || !Q_stricmpn ( cmd, "team vote passed", 16 ) ) {
            trap_S_StartLocalSound ( cgs.media.votePassed, CHAN_ANNOUNCER );
        }
//#endif
        return;
    }
    if ( !strcmp ( cmd, "screenPrint" ) ) {
        Q_strncpyz ( text, CG_Argv ( 1 ), MAX_SAY_TEXT );
        CG_RemoveChatEscapeChar ( text );
        CG_AddToChat ( text );
        return;
    }
    if ( !strcmp ( cmd, "chat" ) ) {
        if ( !cg_teamChatsOnly.integer && !cg_noChat.integer ) {
            if ( cg_chatBeep.integer )
                trap_S_StartLocalSound ( cgs.media.talkSound, CHAN_LOCAL_SOUND );
            Q_strncpyz ( text, CG_Argv ( 1 ), MAX_SAY_TEXT );
            CG_RemoveChatEscapeChar ( text );
            CG_AddToChat ( text );
            CG_Printf ( "%s\n", text );
        }
        return;
    }

    if ( !strcmp ( cmd, "tchat" ) ) {
        if ( !cg_noChat.integer ) {
            if ( cg_teamChatBeep.integer )
                trap_S_StartLocalSound ( cgs.media.talkSound, CHAN_LOCAL_SOUND );
            Q_strncpyz ( text, CG_Argv ( 1 ), MAX_SAY_TEXT );
            CG_RemoveChatEscapeChar ( text );
            CG_AddToTeamChat ( text );
            CG_Printf ( "%s\n", text );
        }
        return;
    }
    if ( !strcmp ( cmd, "scores" ) ) {
        CG_ParseScores();
        return;
    }

    if ( !strcmp ( cmd, "statistics" ) ) {
        CG_ParseStatistics();
        return;
    }

    if ( !strcmp ( cmd, "livingCount" ) ) {
        CG_ParseLivingCount();
        return;
    }

    if ( !strcmp ( cmd, "accs" ) ) {
        CG_ParseAccuracy();
        return;
    }

    if ( !strcmp ( cmd, "timeout" ) ) {
        CG_ParseTimeout();
        return;
    }

    if ( !strcmp ( cmd, "timein" ) ) {
        CG_ParseTimein();
        return;
    }

    if ( !strcmp ( cmd, "ddtaken" ) ) {
        CG_ParseDDtimetaken();
        return;
    }

    if ( !strcmp ( cmd, "dompointnames" ) ) {
        CG_ParseDomPointNames();
        return;
    }

    if ( !strcmp ( cmd, "domStatus" ) ) {
        CG_ParseDomStatus();
        return;
    }

    if ( !strcmp ( cmd, "elimination" ) ) {
        CG_ParseElimination();
        return;
    }

    if ( !strcmp ( cmd, "mappage" ) ) {
        CG_ParseMappage();
        return;
    }

    if ( !strcmp ( cmd, "attackingteam" ) ) {
        CG_ParseAttackingTeam();
        return;
    }

    if ( !strcmp ( cmd, "tinfo" ) ) {
        CG_ParseTeamInfo();
        return;
    }

    if ( !strcmp ( cmd, "map_restart" ) ) {
        CG_MapRestart();
        return;
    }

    if ( Q_stricmp ( cmd, "remapShader" ) == 0 ) {
        if ( trap_Argc() == 4 ) {
            char shader1[MAX_QPATH];
            char shader2[MAX_QPATH];
            char shader3[MAX_QPATH];

            Q_strncpyz ( shader1, CG_Argv ( 1 ), sizeof ( shader1 ) );
            Q_strncpyz ( shader2, CG_Argv ( 2 ), sizeof ( shader2 ) );
            Q_strncpyz ( shader3, CG_Argv ( 3 ), sizeof ( shader3 ) );

            trap_R_RemapShader ( shader1, shader2, shader3 );
        }

        return;
    }

    // loaddeferred can be both a servercmd and a consolecmd
    if ( !strcmp ( cmd, "loaddefered" ) ) {	// FIXME: spelled wrong, but not changing for demo
        CG_LoadDeferredPlayers();
        return;
    }

    // clientLevelShot is sent before taking a special screenshot for
    // the menu system during development
    if ( !strcmp ( cmd, "clientLevelShot" ) ) {
        cg.levelShot = qtrue;
        return;
    }

    // challenge completed is determened by the server. A client should consider this message valid:
    if ( !strcmp ( cmd, "ch" ) ) {
        CG_ParseChallenge();
        return;
    }

    if ( !strcmp ( cmd, "respawnTime" ) ) {
        CG_ParseRespawnTimer();
        return;
    }

    if ( !strcmp ( cmd, "readyMask" ) ) {
        CG_ParseReadyMask();
        return;
    }

    if ( !strcmp ( cmd, "reward" ) ) {
        CG_ParseRewards();
        return;
    }
    
    if ( !strcmp ( cmd, "spawnPoints" ) ) {
        CG_ParseSpawnpoints();
        return;
    }

    if ( !strcmp ( cmd, "team" ) ) {
        CG_ParseTeam();
        return;
    }

    if ( !strcmp ( cmd, "customvotes" ) ) {
        char infoString[1024];
        int i;
        //TODO: Create a ParseCustomvotes function
        memset ( &infoString,0,sizeof ( infoString ) );
        for ( i=1;i<=12;i++ ) {
            Q_strcat ( infoString,sizeof ( infoString ),CG_Argv ( i ) );
            Q_strcat ( infoString,sizeof ( infoString )," " );
        }
        trap_Cvar_Set ( "cg_vote_custom_commands",infoString );
        return;
    }
    //
    //Removed voicechats
    if ( !strcmp ( cmd, "vtell" ) ) {
        return;
    } else if ( !strcmp ( cmd, "vchat" ) ) {
        return;
    }

    CG_Printf ( "Unknown client game command: %s\n", cmd );
}


/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this this snapshot.
====================
*/
void CG_ExecuteNewServerCommands ( int latestSequence ) {
    while ( cgs.serverCommandSequence < latestSequence ) {
        if ( trap_GetServerCommand ( ++cgs.serverCommandSequence ) ) {
            CG_ServerCommand();
        }
    }
}
