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
#include "g_local.h"

#include "../../ui/menudef.h"			// for the voice chats

/*
==================
DeathmatchScoreboardMessage
TODO: add teamtask in teamgames/ctf
==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent ) {
	char		entry[2048];
	char		string[4096];
	int			stringlength;
	int			i, j;
	gclient_t	*cl;
	int			numSorted, scoreFlags, accuracy, perfect;

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;
	scoreFlags = 0;

	numSorted = level.numConnectedClients;

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->pers.connected == CON_CONNECTING ) {
			ping = -1;
		} else {
//unlagged - true ping
			//ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
			ping = cl->pers.realPing < 999 ? cl->pers.realPing : 999;
//unlagged - true ping
		}

		if ( cl->accuracy_shots ) {
			accuracy = cl->accuracy_hits * 100 / cl->accuracy_shots;
		}
		else {
			accuracy = 0;
		}
		perfect = ( cl->ps.persistant[PERS_RANK] == 0 && cl->ps.persistant[PERS_KILLED] == 0 ) ? 1 : 0;

		if ( g_gametype.integer == GT_TOURNAMENT ) {
			if ( ( ent->client->ps.clientNum == cl->ps.clientNum ) || level.intermissiontime || ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
				Com_sprintf (entry, sizeof(entry),
					" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i ", level.sortedClients[i],
					cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
					scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
					cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
					cl->ps.persistant[PERS_EXCELLENT_COUNT],
					cl->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
					cl->ps.persistant[PERS_DEFEND_COUNT],
					cl->ps.persistant[PERS_ASSIST_COUNT],
					perfect,
					cl->ps.persistant[PERS_CAPTURES],
					cl->isEliminated,
					cl->dmgdone,
					cl->dmgtaken,
					cl->sess.specOnly,
					cl->ps.persistant[PERS_KILLED],
					cl->kills,
					cl->rewards[REWARD_AIRROCKET],
					cl->rewards[REWARD_AIRGRENADE],
					cl->rewards[REWARD_FULLSG],
					cl->rewards[REWARD_RLRG],
					cl->rewards[REWARD_ITEMDENIED],
					cl->stats[STATS_HEALTH],
					cl->stats[STATS_ARMOR],
					cl->stats[STATS_YA],
					cl->stats[STATS_RA],
					cl->stats[STATS_MH],
					cl->accuracy[WP_MACHINEGUN][0], cl->accuracy[WP_MACHINEGUN][1],
					cl->accuracy[WP_SHOTGUN][0], cl->accuracy[WP_SHOTGUN][1],
					cl->accuracy[WP_GRENADE_LAUNCHER][0], cl->accuracy[WP_GRENADE_LAUNCHER][1],
					cl->accuracy[WP_ROCKET_LAUNCHER][0], cl->accuracy[WP_ROCKET_LAUNCHER][1],
					cl->accuracy[WP_LIGHTNING][0], cl->accuracy[WP_LIGHTNING][1],
					cl->accuracy[WP_RAILGUN][0], cl->accuracy[WP_RAILGUN][1],
					cl->accuracy[WP_PLASMAGUN][0], cl->accuracy[WP_PLASMAGUN][1],
					cl->accuracy[WP_BFG][0], cl->accuracy[WP_BFG][1],
					cl->rewards[REWARD_SPAWNKILL]
					);
			} else {
				Com_sprintf (entry, sizeof(entry),
					" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 %i ", level.sortedClients[i],
					cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
					scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
					cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
					cl->ps.persistant[PERS_EXCELLENT_COUNT],
					cl->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
					cl->ps.persistant[PERS_DEFEND_COUNT],
					cl->ps.persistant[PERS_ASSIST_COUNT],
					perfect,
					cl->ps.persistant[PERS_CAPTURES],
					cl->isEliminated,
					cl->dmgdone,
					cl->dmgtaken,
					cl->sess.specOnly,
					cl->ps.persistant[PERS_KILLED],
					cl->kills,
					cl->rewards[REWARD_AIRROCKET],
					cl->rewards[REWARD_AIRGRENADE],
					cl->rewards[REWARD_FULLSG],
					cl->rewards[REWARD_RLRG],
					cl->rewards[REWARD_ITEMDENIED],
					cl->rewards[REWARD_SPAWNKILL]
					);
			}
		} else {
			if ( ( ent->client->ps.clientNum == cl->ps.clientNum ) || level.intermissiontime || ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
				Com_sprintf (entry, sizeof(entry),
					" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i ", level.sortedClients[i],
					cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
					scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
					cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
					cl->ps.persistant[PERS_EXCELLENT_COUNT],
					cl->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
					cl->ps.persistant[PERS_DEFEND_COUNT],
					cl->ps.persistant[PERS_ASSIST_COUNT],
					perfect,
					cl->ps.persistant[PERS_CAPTURES],
					cl->isEliminated,
					cl->dmgdone,
					cl->dmgtaken,
					cl->sess.specOnly,
					cl->ps.persistant[PERS_KILLED],
					cl->kills,
					cl->rewards[REWARD_AIRROCKET],
					cl->rewards[REWARD_AIRGRENADE],
					cl->rewards[REWARD_FULLSG],
					cl->rewards[REWARD_RLRG],
					cl->rewards[REWARD_ITEMDENIED],
					cl->rewards[REWARD_SPAWNKILL]
					);
			}
		else {
			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i -1 -1 -1 -1 -1 -1 ", level.sortedClients[i],
				cl->ps.persistant[PERS_SCORE], ping, (level.time - cl->pers.enterTime)/60000,
				scoreFlags, g_entities[level.sortedClients[i]].s.powerups, accuracy,
				cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
				cl->ps.persistant[PERS_EXCELLENT_COUNT],
				cl->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
				cl->ps.persistant[PERS_DEFEND_COUNT],
				cl->ps.persistant[PERS_ASSIST_COUNT],
				perfect,
				cl->ps.persistant[PERS_CAPTURES],
				cl->isEliminated,
				cl->dmgdone,
				cl->dmgtaken,
				cl->sess.specOnly,
				cl->ps.persistant[PERS_KILLED],
				cl->kills
				);
		}
	}
	
        j = strlen(entry);
        if (stringlength + j > 1024 ) {
		G_Printf("Too many clients connected for scoreboard info!\n");
		break;
        }
        strcpy (string + stringlength, entry);
        stringlength += j;
    }

    trap_SendServerCommand( ent-g_entities, va("scores %i %i %i %i%s", i,
                            level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE], level.roundStartTime,
                            string ) );
}

/*
==================
G_SendAccMessage

==================
*/
void G_SendAccMessage( gentity_t *ent ) {
	char		entry[128];

	Com_sprintf (entry, sizeof(entry),
		" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i ", ent->client->accuracy[WP_MACHINEGUN][0], ent->client->accuracy[WP_MACHINEGUN][1],
		ent->client->accuracy[WP_SHOTGUN][0], ent->client->accuracy[WP_SHOTGUN][1],
		ent->client->accuracy[WP_GRENADE_LAUNCHER][0], ent->client->accuracy[WP_GRENADE_LAUNCHER][1],
		ent->client->accuracy[WP_ROCKET_LAUNCHER][0], ent->client->accuracy[WP_ROCKET_LAUNCHER][1],
		ent->client->accuracy[WP_LIGHTNING][0], ent->client->accuracy[WP_LIGHTNING][1],
		ent->client->accuracy[WP_RAILGUN][0], ent->client->accuracy[WP_RAILGUN][1],
		ent->client->accuracy[WP_PLASMAGUN][0], ent->client->accuracy[WP_PLASMAGUN][1],
		ent->client->accuracy[WP_BFG][0], ent->client->accuracy[WP_BFG][1]
	      );

	trap_SendServerCommand( ent-g_entities, va("accs%s", entry ));
}

/*
==================
G_SendLivingCount

==================
*/
void G_SendLivingCount( void ) {
	char		entry[128];

	if ( level.roundRespawned )
		Com_sprintf (entry, sizeof(entry),
                     " %i %i %i %i ", TeamLivingCount( -1, TEAM_RED ), TeamLivingCount( -1, TEAM_BLUE ), TeamCount( -1, TEAM_RED ), TeamCount( -1, TEAM_BLUE ) );
	else
		Com_sprintf (entry, sizeof(entry),
                     " %i %i %i %i ", TeamCount( -1, TEAM_RED ), TeamCount( -1, TEAM_BLUE ), TeamCount( -1, TEAM_RED ), TeamCount( -1, TEAM_BLUE ) );

	trap_SendServerCommand( -1, va("livingCount%s", entry ));
}

/*
==================
G_ItemCount

returns qtrue if an item is only once on the map
==================
*/
qboolean G_ItemCount( int type, int quantity, int tag ) {
	gentity_t *ent;
	int i, count = 0;

	for ( i = 0; i < MAX_GENTITIES; i++ ) {
		ent = &g_entities[i];
		if ( !ent->inuse )
			continue;
		if ( ent->s.eType != ET_ITEM )
			continue;
		if ( ( ent->item->giType == type ) && ( ent->item->quantity == quantity ) && ( ent->item->giTag == tag ) )
			count++;
	}
	return( count == 1 );
}

/*
==================
G_SendRespawnTimer

==================
*/
void G_SendRespawnTimer( int entityNum, int type, int quantity, int respawnTime, int nextItemEntityNum, int clientNum ) {
	char		entry[32];
	gentity_t	*ent;
	int		i;
	int team;
	fileHandle_t f;

	//G_Printf("entityNum %i, type %i, quantity %i, respawnTime %i, nextItemEntityNum %i\n", entityNum, type, quantity, respawnTime, nextItemEntityNum);

	if ( level.warmupTime ) {
		return;
	}

	if ( ( type != IT_ARMOR ) && ( type != IT_HEALTH ) && ( type != IT_POWERUP ) )
		return;

	if ( type == IT_ARMOR && quantity < 50 )
		return;

	if ( type == IT_HEALTH && quantity != 100 )
		return;

	//G_Printf("type %i, quantity %i\n", type, quantity );

	if ( G_ItemCount(type, quantity, g_entities[entityNum].item->giTag) )
		nextItemEntityNum = -1;

	team = G_ItemTeam( entityNum );
	
	if( g_gametype.integer >= GT_TEAM )
		clientNum = level.clients[clientNum].sess.sessionTeam;
	
	Com_sprintf (entry, sizeof(entry), " %i %i %i %i %i %i %i ", entityNum, type, quantity, respawnTime, nextItemEntityNum , team, clientNum );
	/*if( g_autoServerDemos.integer ) {
		char gamestring[128];
		char matchstring[128];
	
		trap_Cvar_VariableStringBuffer("gamestring", gamestring, sizeof(gamestring));
		trap_Cvar_VariableStringBuffer("matchstring", matchstring, sizeof(matchstring));
		trap_FS_FOpenFile( va("%s/%s",gamestring,matchstring), &f, FS_APPEND );
		trap_FS_Write( va("%i %s\n", level.time, entry), strlen(va("%i %s\n", level.time, entry)), f );
		trap_FS_FCloseFile( f );
	}*/

	for (i = 0; i < MAX_CLIENTS; i++) {
		ent = &g_entities[i];
		if ( ( ent->inuse ) && ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) ) {
			trap_SendServerCommand( ent-g_entities, va("respawnTime%s", entry ));
		}
	}	
}

/*
==================
G_SendSpawnpoints

sends spawnpoints to the given entity
TODO: add teamspawnpoint color for teambased gametypes
==================
*/
void G_SendSpawnpoints( gentity_t *ent ){
	gentity_t *spot;
	int spotnumber = 0;
	char entry[64];
	char string[2048];
	
	if( level.warmupTime != -1 )
		return;
	
	if( g_gametype.integer < GT_CTF || g_gametype.integer == GT_ELIMINATION ){
		while(( spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL ) {
			Com_sprintf( entry, sizeof(entry), "%i %i %i %i %i %i %i ", (int)spot->s.origin[0], (int)spot->s.origin[1], (int)spot->s.origin[2], 
										   (int)spot->s.angles[0], (int)spot->s.angles[1], (int)spot->s.angles[2], TEAM_FREE );
			spotnumber++;
			strcat( string, entry );
		}
	}
	else{
		while(( spot = G_Find (spot, FOFS(classname), "team_CTF_redspawn")) != NULL ) {
			Com_sprintf( entry, sizeof(entry), "%i %i %i %i %i %i %i ", (int)spot->s.origin[0], (int)spot->s.origin[1], (int)spot->s.origin[2], 
										   (int)spot->s.angles[0], (int)spot->s.angles[1], (int)spot->s.angles[2], TEAM_RED );
			spotnumber++;
			strcat( string, entry );
		}
		while(( spot = G_Find (spot, FOFS(classname), "team_CTF_bluespawn")) != NULL ) {
			Com_sprintf( entry, sizeof(entry), "%i %i %i %i %i %i %i ", (int)spot->s.origin[0], (int)spot->s.origin[1], (int)spot->s.origin[2], 
										   (int)spot->s.angles[0], (int)spot->s.angles[1], (int)spot->s.angles[2], TEAM_BLUE );
			spotnumber++;
			strcat( string, entry );
		}
	}
	trap_SendServerCommand( ent-g_entities, va( "spawnPoints %i %s", spotnumber, string ));
}

/*
==================
G_SendStartGame

==================
*/
void G_SendStartGame( void ) {
	if ( level.intermissiontime )
		return;
    
	trap_SendServerCommand( -1, va("startOfGame"));
}

/*
==================
G_SendEndGame
==================
*/
void G_SendEndGame( void ) {
	level.endgameSend = qtrue;
        trap_SendServerCommand( -1, va("endOfGame"));
}

/*
==================
G_SendStats

==================
*/
void G_SendStats( gentity_t *ent ) {
	char		entry[2048];
	char		string[2800];
	int			stringlength;
	int			i, j;
	gclient_t	*cl;
	int			numSorted;

	//Stats only possible during intermission
	if ( !level.intermissiontime )
		return;

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;

	numSorted = level.numConnectedClients;

	for (i=0 ; i < numSorted ; i++) {

		cl = &level.clients[level.sortedClients[i]];

		Com_sprintf (entry, sizeof(entry),
			" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i", level.sortedClients[i], cl->accuracy[WP_GAUNTLET][2], cl->accuracy[WP_GAUNTLET][3], cl->accuracy[WP_GAUNTLET][4],
			cl->accuracy[WP_MACHINEGUN][0], cl->accuracy[WP_MACHINEGUN][1], cl->accuracy[WP_MACHINEGUN][2], cl->accuracy[WP_MACHINEGUN][3], cl->accuracy[WP_MACHINEGUN][4],
			cl->accuracy[WP_SHOTGUN][0], cl->accuracy[WP_SHOTGUN][1], cl->accuracy[WP_SHOTGUN][2], cl->accuracy[WP_SHOTGUN][3], cl->accuracy[WP_SHOTGUN][4],
			cl->accuracy[WP_GRENADE_LAUNCHER][0], cl->accuracy[WP_GRENADE_LAUNCHER][1], cl->accuracy[WP_GRENADE_LAUNCHER][2], cl->accuracy[WP_GRENADE_LAUNCHER][3], cl->accuracy[WP_GRENADE_LAUNCHER][4],
			cl->accuracy[WP_ROCKET_LAUNCHER][0], cl->accuracy[WP_ROCKET_LAUNCHER][1], cl->accuracy[WP_ROCKET_LAUNCHER][2], cl->accuracy[WP_ROCKET_LAUNCHER][3], cl->accuracy[WP_ROCKET_LAUNCHER][4],
			cl->accuracy[WP_LIGHTNING][0], cl->accuracy[WP_LIGHTNING][1], cl->accuracy[WP_LIGHTNING][2], cl->accuracy[WP_LIGHTNING][3], cl->accuracy[WP_LIGHTNING][4],
			cl->accuracy[WP_RAILGUN][0], cl->accuracy[WP_RAILGUN][1], cl->accuracy[WP_RAILGUN][2], cl->accuracy[WP_RAILGUN][3], cl->accuracy[WP_RAILGUN][4],
			cl->accuracy[WP_PLASMAGUN][0], cl->accuracy[WP_PLASMAGUN][1], cl->accuracy[WP_PLASMAGUN][2], cl->accuracy[WP_PLASMAGUN][3], cl->accuracy[WP_PLASMAGUN][4],
			cl->accuracy[WP_BFG][0], cl->accuracy[WP_BFG][1], cl->accuracy[WP_BFG][2], cl->accuracy[WP_BFG][3], cl->accuracy[WP_BFG][4],
			cl->stats[STATS_HEALTH],
			cl->stats[STATS_ARMOR],
			cl->stats[STATS_YA],
			cl->stats[STATS_RA],
			cl->stats[STATS_MH],
			cl->ps.persistant[PERS_IMPRESSIVE_COUNT],
			cl->ps.persistant[PERS_EXCELLENT_COUNT],
			cl->ps.persistant[PERS_ASSIST_COUNT],
			cl->ps.persistant[PERS_DEFEND_COUNT],
			cl->ps.persistant[PERS_CAPTURES],
			cl->rewards[REWARD_AIRGRENADE],
			cl->rewards[REWARD_AIRROCKET],
			cl->rewards[REWARD_FULLSG],
			cl->rewards[REWARD_RLRG],
			cl->rewards[REWARD_ITEMDENIED]/*,
			cl->rewards[REWARD_SPAWNKILL]*/); //TOO much for the engine

		j = strlen(entry);
		if (stringlength + j > 2048)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	trap_SendServerCommand( ent-g_entities, va("statistics %i %s", i, string ) );
}

void G_StartServerDemos( void ) {
	int i,j;
	char gamestring[128];
	char matchstring[128];
	char playerName[128];
	int count;
	
	//TODO: Add other gametypes
	if( ( g_gametype.integer != GT_TOURNAMENT ) || !g_autoServerDemos.integer )
		return;
	
	trap_Cvar_VariableStringBuffer("gamestring", gamestring, sizeof(gamestring));
	trap_Cvar_VariableStringBuffer("matchstring", matchstring, sizeof(matchstring));
	
	for( i = 0; i < level.numConnectedClients; i++ ){
		if( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR )
			continue;
		if ( g_entities[ &level.clients[i] - level.clients].r.svFlags & SVF_BOT)
			continue;
		
		Com_sprintf ( playerName, sizeof ( playerName ),"%s", level.clients[i].pers.netname );
		count = 0;
		
		for ( j = 0; j < 128 ; j++ ) {
			if ( playerName[j] == '^' && ( ( playerName[j+1] >= '0' && playerName[j+1] <= '9' ) || ( playerName[j+1] >= 'a' && playerName[j+1] <= 'z' ) || ( playerName[j+1] >= 'A' && playerName[j+1] <= 'Z' ) ) ) {
				j++;
				continue;
			} else if ( ( ! ( playerName[j] >= '0' && playerName[j] <= '9' ) && ! ( playerName[j] >= 'a' && playerName[j] <= 'z' ) && ! ( playerName[j] >= 'A' && playerName[j] <= 'Z' ) ) ) {
				continue;
			}

			else {
				playerName[count] = playerName[j];
				count++;
			}
		}
		playerName[count] = '\0';
		trap_Cvar_Set(va( "demopath%i", i ), va("%s/%s-POV(%s)", gamestring, matchstring, playerName));
		//Com_sprintf(level.clients[i].demopath, MAX_QPATH, "%s/%s-POV(%s)", gamestring, matchstring, playerName );
		trap_SendConsoleCommand(EXEC_APPEND, va("svrecord %i %s/%s-POV(%s) \n", &level.clients[i] - level.clients, gamestring, matchstring, playerName ) );
	}
}

void G_StopServerDemos( void ) {
	int i;
	char gamestring[128];
	
	//TODO: Add other gametypes
	if( ( g_gametype.integer != GT_TOURNAMENT ) || !g_autoServerDemos.integer )
		return;
	
	
	for( i = 0; i < level.numConnectedClients; i++ ){
		if( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR )
			continue;
		if ( g_entities[ &level.clients[i] - level.clients].r.svFlags & SVF_BOT)
			continue;
		
		trap_SendConsoleCommand(EXEC_APPEND, va("stopsvrecord %i \n", &level.clients[i] - level.clients ) );
	}
}

/*
==================
DominationPointStatusMessage

==================
*/
void DominationPointStatusMessage( gentity_t *ent ) {
    char		entry[10]; //Will more likely be 2... in fact cannot be more since we are the server
    char		string[10*(MAX_DOMINATION_POINTS+1)];
    int			stringlength;
    int i, j;

    string[0] = 0;
    stringlength = 0;

    for (i = 0;i<MAX_DOMINATION_POINTS && i<level.domination_points_count; i++) {
        Com_sprintf (entry, sizeof(entry)," %i",level.pointStatusDom[i]);
        j = strlen(entry);
        if (stringlength + j > 10*MAX_DOMINATION_POINTS)
            break;
        strcpy (string + stringlength, entry);
        stringlength += j;
    }

    trap_SendServerCommand( ent-g_entities, va("domStatus %i%s", level.domination_points_count, string ) );
}

/*
==================
EliminationMessage

==================
*/

void EliminationMessage(gentity_t *ent) {
    trap_SendServerCommand( ent-g_entities, va("elimination %i %i %i",
                            level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE], level.roundStartTime) );
}

/*
==================
DoubleDominationScoreTime

==================
*/
void DoubleDominationScoreTimeMessage( gentity_t *ent ) {
    trap_SendServerCommand( ent-g_entities, va("ddtaken %i", level.timeTaken));
}

/*
==================
DominationPointNames
==================
*/

void DominationPointNamesMessage( gentity_t *ent ) {
    char text[MAX_DOMINATION_POINTS_NAMES*MAX_DOMINATION_POINTS];
    int i,j;
    qboolean nullFound;
    for (i=0;i<MAX_DOMINATION_POINTS;i++) {
        Q_strncpyz(text+i*MAX_DOMINATION_POINTS_NAMES,level.domination_points_names[i],MAX_DOMINATION_POINTS_NAMES-1);
        if (i!=MAX_DOMINATION_POINTS-1) {
            //Don't allow "/0"!
            nullFound = qfalse;
            for (j=i*MAX_DOMINATION_POINTS_NAMES; j<(i+1)*MAX_DOMINATION_POINTS_NAMES;j++) {
                if (text[j]==0)
                    nullFound = qtrue;
                if (nullFound)
                    text[j] = ' ';
            }
        }
        text[MAX_DOMINATION_POINTS_NAMES*MAX_DOMINATION_POINTS-2]=0x19;
        text[MAX_DOMINATION_POINTS_NAMES*MAX_DOMINATION_POINTS-1]=0;
    }
    trap_SendServerCommand( ent-g_entities, va("dompointnames %i \"%s\"", level.domination_points_count, text));
}

/*
==================
YourTeamMessage
==================
*/

void YourTeamMessage( gentity_t *ent) {
    int team = level.clients[ent-g_entities].sess.sessionTeam;

    switch (team) {
    case TEAM_RED:
        trap_SendServerCommand( ent-g_entities, va("team \"%s\"", g_redTeamClientNumbers.string));
        break;
    case TEAM_BLUE:
        trap_SendServerCommand( ent-g_entities, va("team \"%s\"", g_blueTeamClientNumbers.string));
        break;
    default:
        trap_SendServerCommand( ent-g_entities, "team \"all\"");
    };
}

/*
==================
AttackingTeamMessage

==================
*/
void AttackingTeamMessage( gentity_t *ent ) {
    int team;
    if ( (level.eliminationSides+level.roundNumber)%2 == 0 )
        team = TEAM_RED;
    else
        team = TEAM_BLUE;
    trap_SendServerCommand( ent-g_entities, va("attackingteam %i", team));
}

/*
==================
ChallengeMessage

==================
*/

void ChallengeMessage(gentity_t *ent, int challenge) {
    if ( level.warmupTime != 0)
        return; //We don't send anything doring warmup
    trap_SendServerCommand( ent-g_entities, va("ch %u", challenge) );
}

/*
==================
RewardMessage

==================
*/

void RewardMessage(gentity_t *ent, int reward, int rewardCount) {
	gentity_t *other;
	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
	  
		other = &g_entities[i];
		if (  other->client->ps.clientNum == ent->client->ps.clientNum ) {
		  
			trap_SendServerCommand( other-g_entities, va("reward %i %i", reward, rewardCount) );
		}
	}
}

/*
==================
SendReadymask

sends the readymask to the player with clientnum, if clientnum = -1 its send to every player
==================
*/

void SendReadymask( int clientnum ) {
	int			ready, notReady, playerCount;
	int			i;
	gclient_t	*cl;
	int			readyMask;
	char		entry[16];

	if ( !level.warmupTime ) {
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	playerCount = 0;
	
	for ( i = 0; i < g_maxclients.integer; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		playerCount++;
		if ( cl->ready || ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) ) {
			ready++;
			if ( i < 16 ) {
			    readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	level.readyMask = readyMask;
	Com_sprintf (entry, sizeof(entry), " %i ", readyMask);

	trap_SendServerCommand( clientnum, va("readyMask%s", entry ));
}

/*
==================
SendCustomVoteCommands

==================
*/

void SendCustomVoteCommands(int clientNum) {
	trap_SendServerCommand( clientNum, va("customvotes %s", custom_vote_info) );
}

/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) {
	DeathmatchScoreboardMessage( ent );
}

/*
==================
Cmd_Acc_f

Request current scoreboard information
==================
*/
void Cmd_Acc_f( gentity_t *ent ) {
	G_SendAccMessage( ent );
}

/*
==================
CheatsOk

checks if cheats are enabled and if the player is alive
used by the give command
==================
*/
qboolean CheatsOk( gentity_t *ent ) {
	if ( !g_cheats.integer ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
		return qfalse;
	}
	if ( ent->health <= 0 ) {
		trap_SendServerCommand( ent-g_entities, va("print \"You must be alive to use this command.\n\""));
		return qfalse;
	}
	return qtrue;
}


/*
==================
ConcatArgs
==================
*/
char *ConcatArgs( int start ) {
	int		i, c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for ( i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s ) {
	gclient_t	*cl;
	int			idnum;
	char        cleanName[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if (s[0] >= '0' && s[0] <= '9') {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			trap_SendServerCommand( to-g_entities, va("print \"Bad client slot: %i\n\"", idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			trap_SendServerCommand( to-g_entities, va("print \"Client %i is not active\n\"", idnum));
			return -1;
		}
		return idnum;
	}

	// check for a name match
	for ( idnum=0,cl=level.clients ; idnum < level.maxclients ; idnum++,cl++ ) {
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		Q_strncpyz(cleanName, cl->pers.netname, sizeof(cleanName));
		Q_CleanStr(cleanName);
		if ( !Q_stricmp( cleanName, s ) ) {
			return idnum;
		}
	}

	trap_SendServerCommand( to-g_entities, va("print \"User %s is not on the server\n\"", s));
	return -1;
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (gentity_t *ent)
{
	char		*name;
	gitem_t		*it;
	int			i;
	qboolean	give_all;
	gentity_t		*it_ent;
	trace_t		trace;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	name = ConcatArgs( 1 );

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if (give_all || Q_stricmp( name, "health") == 0) {
		ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0) {
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 -
						      ( 1 << WP_GRAPPLING_HOOK ) - ( 1 << WP_NONE );
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0) {
		for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
			ent->client->ps.ammo[i] = 999;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0) {
		ent->client->ps.stats[STAT_ARMOR] = 200;

		if (!give_all)
			return;
	}

	if (Q_stricmp(name, "excellent") == 0) {
		ent->client->ps.persistant[PERS_EXCELLENT_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "impressive") == 0) {
		ent->client->ps.persistant[PERS_IMPRESSIVE_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "gauntletaward") == 0) {
		ent->client->ps.persistant[PERS_GAUNTLET_FRAG_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "defend") == 0) {
		ent->client->ps.persistant[PERS_DEFEND_COUNT]++;
		return;
	}
	if (Q_stricmp(name, "assist") == 0) {
		ent->client->ps.persistant[PERS_ASSIST_COUNT]++;
		return;
	}

	// spawn a specific item right on the player
	if ( !give_all ) {
		it = BG_FindItem (name);
		if (!it) {
			return;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem (it_ent, it);
		FinishSpawningItem(it_ent );
		memset( &trace, 0, sizeof( trace ) );
		Touch_Item (it_ent, ent, &trace);
		if (it_ent->inuse) {
			G_FreeEntity( it_ent );
		}
	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (gentity_t *ent)
{
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent ) {
    char	*msg;

    if ( !CheatsOk( ent ) ) {
        return;
    }

    ent->flags ^= FL_NOTARGET;
    if (!(ent->flags & FL_NOTARGET) )
        msg = "notarget OFF\n";
    else
        msg = "notarget ON\n";

    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
    char	*msg;

    if ( !CheatsOk( ent ) ) {
        return;
    }

    if ( ent->client->noclip ) {
        msg = "noclip OFF\n";
    } else {
        msg = "noclip ON\n";
    }
    ent->client->noclip = !ent->client->noclip;

    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", msg));
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent ) {
    if ( !CheatsOk( ent ) ) {
        return;
    }

    // doesn't work in single player
    if ( g_gametype.integer != 0 ) {
        trap_SendServerCommand( ent-g_entities,
                                "print \"Must be in g_gametype 0 for levelshot\n\"" );
        return;
    }

    BeginIntermission();
    trap_SendServerCommand( ent-g_entities, "clientLevelShot" );
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_TeamTask_f( gentity_t *ent ) {
    char userinfo[MAX_INFO_STRING];
    char		arg[MAX_TOKEN_CHARS];
    int task;
    int client = ent->client - level.clients;

    if ( trap_Argc() != 2 ) {
        return;
    }
    trap_Argv( 1, arg, sizeof( arg ) );
    task = atoi( arg );

    trap_GetUserinfo(client, userinfo, sizeof(userinfo));
    Info_SetValueForKey(userinfo, "teamtask", va("%d", task));
    trap_SetUserinfo(client, userinfo);
    ClientUserinfoChanged(client);
}



/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent ) {
  
	if ( (ent->client->sess.sessionTeam == TEAM_SPECTATOR) || ent->client->isEliminated ) {
		return;
	}
	
	if (ent->health <= 0) {
		return;
	}
  
	if ( !g_allowKill.integer ) {
		trap_SendServerCommand( ent-g_entities,
                                "print \"Selfkill not allowed on this server\n\"" );
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
	if (ent->client->lastSentFlying>-1 && g_awardpushing.integer )
		//If player is in the air because of knockback we give credit to the person who sent it flying
		player_die (ent, ent, &g_entities[ent->client->lastSentFlying], 100000, MOD_FALLING);
	else
		player_die (ent, ent, ent, 100000, MOD_SUICIDE);
}

/*
=================
Cmd_Timeout_f
=================
*/
void Cmd_Timeout_f( gentity_t *player ) {
    int i,j;
    gentity_t *ent;

    if ( !g_timeoutAllowed.integer ) {
        trap_SendServerCommand(player-g_entities,va("screenPrint \"" S_COLOR_CYAN "timeout not allowed\"" ) );
        return;
    }
    if ( level.warmupTime ) {
        trap_SendServerCommand(player-g_entities,va("screenPrint \"" S_COLOR_CYAN "timeout not allowed in warmup\"" ) );
        return;
    }

    if ( player->client->sess.sessionTeam == TEAM_SPECTATOR && !player->client->referee) {
        trap_SendServerCommand(player-g_entities,va("screenPrint \"" S_COLOR_CYAN "timeout not allowed as spectator\"" ) );
        return;
    }
    if ( !(player->client->timeouts < g_timeoutAllowed.integer) ) {
        trap_SendServerCommand(player-g_entities,va("screenPrint \"" S_COLOR_CYAN "timeout limit reached\"" ) );
        return;
    }

    if ( level.timeout )
        trap_SendServerCommand( player-g_entities, va("timeout %i %i", level.timeoutTime, level.timeoutAdd ) );
    else {
        level.timeout = qtrue;
        level.timeoutTime = level.time;
        level.timeoutAdd = g_timeoutTime.integer;
        level.timeoutDelay += level.timeoutAdd;

        if ( !player->client->referee )
            player->client->timeouts++;

        ent = &g_entities[0];
        for (i=0 ; i < level.num_entities ; i++, ent++) {
            if ( ent->inuse ) {
                if ( ent->nextthink > 0 )
                    ent->nextthink += level.timeoutAdd;
                if ( ent->eventTime > 0 )
                    ent->eventTime += level.timeoutAdd;
            }
        }

        //TODO: POWERUPS

        for ( i = 0; i < level.numConnectedClients ;i++) {
            for ( j = 0; j < MAX_POWERUPS; j++ ) {
                if ( level.clients[i].ps.powerups[j] > 0 )
                    level.clients->ps.powerups[j] += level.timeoutAdd;
            }
        }

        trap_SendServerCommand(-1,va("screenPrint \"%s" S_COLOR_CYAN " called a timeout\"", player->client->pers.netname ) );
        trap_SendServerCommand( -1, va("timeout %i %i", level.timeoutTime, level.timeoutAdd ) );
    }
}

void G_Timein( void ) {
    level.timeout = qfalse;
    trap_SendServerCommand( -1, va("timein") );
}

/*
=================
Cmd_Ready_f
=================
*/
void Cmd_Ready_f( gentity_t *ent ) {

    if ( level.warmupTime != -1 )
        return;

    if ( g_startWhenReady.integer == 3 ) {
        trap_SendServerCommand(ent-g_entities,va("screenPrint \"only a referee can start the game\"" ) );
        trap_SendServerCommand(ent-g_entities,va("print \"only a referee can start the game\"" ) );
        return;
    }

    if ( ent->client->ready )
        ent->client->ready = qfalse;
    else
        ent->client->ready = qtrue;

    SendReadymask( -1 );
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
    if ( level.time -level.startTime < 1000 )
        return;

    if ( client->sess.sessionTeam == TEAM_RED ) {
        trap_SendServerCommand( -1, va("screenPrint \"%s" S_COLOR_WHITE " joined the " S_COLOR_RED "red team.\"",
                                       client->pers.netname) );
    } else if ( client->sess.sessionTeam == TEAM_BLUE ) {
        trap_SendServerCommand( -1, va("screenPrint \"%s" S_COLOR_WHITE " joined the " S_COLOR_BLUE "blue team.\"",
                                       client->pers.netname));
    } else if ( client->sess.sessionTeam == TEAM_SPECTATOR && oldTeam != TEAM_SPECTATOR ) {
        trap_SendServerCommand( -1, va("screenPrint \"%s" S_COLOR_WHITE " joined the spectators.\"",
                                       client->pers.netname));
    } else if ( client->sess.sessionTeam == TEAM_FREE ) {
        trap_SendServerCommand( -1, va("screenPrint \"%s" S_COLOR_WHITE " joined the battle.\"",
                                       client->pers.netname));
    }
}

/*
=================
SetTeam
KK-OAX Modded this to accept a forced admin change.
=================
*/
void SetTeam( gentity_t *ent, char *s ) {
    int					team, oldTeam;
    gclient_t			*client;
    int					clientNum;
    spectatorState_t	specState;
    int					specClient;
    int					teamLeader;
    int				specOnly;
    char	            userinfo[MAX_INFO_STRING];
    qboolean            force;
    //char    buf[ MAX_INFO_STRING ];

    force = G_admin_permission(ent, ADMF_FORCETEAMCHANGE);

    //
    // see what change is requested
    //
    client = ent->client;
    clientNum = client - level.clients;
    trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );
    specClient = 0;
    specState = SPECTATOR_NOT;
    //specOnly = client->sess.specOnly;

    /*if( !ClientNameAllowed(client->pers.netname, sizeof(client->pers.netname) ) ){
    	team =  TEAM_SPECTATOR;
    	specState = SPECTATOR_FREE;
    	trap_SendServerCommand( ent-g_entities, va("screenPrint \"" S_COLOR_YELLOW "Invalid playername, please choose a different name\"") );
    	trap_SendServerCommand( ent-g_entities, va("print \"" S_COLOR_YELLOW "Invalid playername, please choose a different name\"") );
    }
    else*/
    if ( !Q_stricmp( s, "speconly" ) || !Q_stricmp( s, "so" ) ) {
        if ( g_gametype.integer == GT_TOURNAMENT ) {
            team =  TEAM_SPECTATOR;
            specState = SPECTATOR_FREE;
            specOnly = 1;
        } else {
            team =  TEAM_SPECTATOR;
            specState = SPECTATOR_FREE;
            specOnly = 0;
        }
    } else if ( !Q_stricmp( s, "scoreboard" ) || !Q_stricmp( s, "score" )  ) {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_SCOREBOARD;
        specOnly = 1;
    } else if ( !Q_stricmp( s, "follow1" ) ) {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FOLLOW;
        specClient = -1;
    } else if ( !Q_stricmp( s, "follow2" ) ) {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FOLLOW;
        specClient = -2;
    } else if ( !Q_stricmp( s, "spectator" ) || !Q_stricmp( s, "s" ) ) {
        team = TEAM_SPECTATOR;
        specState = SPECTATOR_FREE;
        specOnly = 0;
    } else if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
        // if running a team game, assign player to one of the teams
        specState = SPECTATOR_NOT;
        if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) ) {
            team = TEAM_RED;
        } else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) {
            team = TEAM_BLUE;
        } else {
            // pick the team with the least number of players
            team = PickTeam( clientNum );
        }
        if ( !force ) {
            if ( g_teamForceBalance.integer  ) {
                int		counts[TEAM_NUM_TEAMS];

                counts[TEAM_BLUE] = TeamCount( ent->client->ps.clientNum, TEAM_BLUE );
                counts[TEAM_RED] = TeamCount( ent->client->ps.clientNum, TEAM_RED );

                // We allow a spread of two
                if ( team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] > 1 ) {
                    trap_SendServerCommand( ent->client->ps.clientNum,
                                            "cp \"Red team has too many players.\n\"" );
                    return; // ignore the request
                }
                if ( team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] > 1 ) {
                    trap_SendServerCommand( ent->client->ps.clientNum,
                                            "cp \"Blue team has too many players.\n\"" );
                    return; // ignore the request
                }

                // It's ok, the team we are switching to has less or same number of players
            }
        }
        specOnly = 0;
    } else {
        // force them to spectators if there aren't any spots free
        team = TEAM_FREE;
        specOnly = 0;
    }
    if ( !force ) {
        // override decision if limiting the players
        if ( (g_gametype.integer == GT_TOURNAMENT)
                && level.numNonSpectatorClients >= 2 ) {
            team = TEAM_SPECTATOR;
        } else if ( g_maxGameClients.integer > 0 &&
                    level.numNonSpectatorClients >= g_maxGameClients.integer ) {
            team = TEAM_SPECTATOR;
        }
    }

    //
    // decide if we will allow the change
    //
    oldTeam = client->sess.sessionTeam;
    if ( team == oldTeam && team != TEAM_SPECTATOR ) {
        return;
    }
    //KK-OAX Check to make sure the team is not locked from Admin
    if ( !force ) {
        if ( team == TEAM_RED && g_redLocked.integer ) {
            trap_SendServerCommand( ent->client->ps.clientNum,
                                    "cp \"The Red Team has been locked! \n\"" );
            return;
        }
        if ( team == TEAM_BLUE && g_blueLocked.integer ) {
            trap_SendServerCommand( ent->client->ps.clientNum,
                                    "cp \"The Blue Team has been locked! \n\"" );
            return;
        }
        if ( team == TEAM_FREE && level.FFALocked ) {
            trap_SendServerCommand( ent->client->ps.clientNum,
                                    "cp \"This Deathmatch has been locked! \n\"" );
        }
    }
    //
    // execute the team change
    //

    // if the player was dead leave the body
    if ( client->ps.stats[STAT_HEALTH] <= 0 ) {
        CopyToBodyQue(ent);
    }

    // he starts at 'base'
    client->pers.teamState.state = TEAM_BEGIN;
    if ( oldTeam != TEAM_SPECTATOR ) {
        //Prevent a team from loosing point because of player leaving team
        if (g_gametype.integer == GT_TEAM && ent->client->ps.stats[STAT_HEALTH])
            level.teamScores[ ent->client->sess.sessionTeam ]++;
        // Kill him (makes sure he loses flags, etc)
        ent->flags &= ~FL_GODMODE;
        ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
        player_die (ent, ent, ent, 100000, MOD_SUICIDE);

    }

    /*if(oldTeam!=TEAM_SPECTATOR)
        PlayerStore_store(Info_ValueForKey(userinfo,"cl_guid"),client->ps);*/

    // they go to the end of the line for tournements
    if ( team == TEAM_SPECTATOR ) {
        client->sess.spectatorTime = level.time;
    }

    client->sess.sessionTeam = team;
    client->sess.spectatorState = specState;
    client->sess.spectatorClient = specClient;
    client->sess.specOnly = specOnly;

    client->sess.teamLeader = qfalse;
    if ( team == TEAM_RED || team == TEAM_BLUE ) {
        teamLeader = TeamLeader( team );
        // if there is no team leader or the team leader is a bot and this client is not a bot
        if ( teamLeader == -1 || ( !(g_entities[clientNum].r.svFlags & SVF_BOT) && (g_entities[teamLeader].r.svFlags & SVF_BOT) ) ) {
            SetLeader( team, clientNum );
        }
    }
    // make sure there is a team leader on the team the player came from
    if ( oldTeam == TEAM_RED || oldTeam == TEAM_BLUE ) {
        CheckTeamLeader( oldTeam );
    }

    BroadcastTeamChange( client, oldTeam );

    // get and distribute relevent paramters
    ClientUserinfoChanged( clientNum );

    // log team changes to demo
    /*Info_SetValueForKey( buf, "team", va( "%d", ent->client->sess.sessionTeam ) );
    G_DemoCommand( DC_CLIENT_SET, va( "%d %s", (int)(ent - g_entities), buf ) );*/

    ClientBegin( clientNum );
    
    trap_SendServerCommand( ent->client->ps.clientNum,
                                    "loadModel" );
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent ) {
    if (g_gametype.integer<GT_ELIMINATION || g_gametype.integer>GT_LMS)
    {
        //Shouldn't this already be the case?
        ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;
        ent->client->sess.sessionTeam = TEAM_SPECTATOR;
    }
    else {
        ent->client->ps.stats[STAT_HEALTH] = 0;
        ent->health = 0;
    }
    ent->client->sess.spectatorState = SPECTATOR_FREE;
    ent->client->ps.pm_flags &= ~PMF_FOLLOW;
    ent->r.svFlags &= ~SVF_BOT;
    ent->client->ps.clientNum = ent - g_entities;
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent ) {
    int			oldTeam;
    char		s[MAX_TOKEN_CHARS];
    qboolean    force;

    if ( trap_Argc() != 2 ) {
        oldTeam = ent->client->sess.sessionTeam;
        switch ( oldTeam ) {
        case TEAM_BLUE:
            trap_SendServerCommand( ent-g_entities, "print \"Blue team\n\"" );
            break;
        case TEAM_RED:
            trap_SendServerCommand( ent-g_entities, "print \"Red team\n\"" );
            break;
        case TEAM_FREE:
            trap_SendServerCommand( ent-g_entities, "print \"Deathmatch-Playing\n\"" );
            break;
        case TEAM_SPECTATOR:
            if ( ent->client->sess.specOnly )
                trap_SendServerCommand( ent-g_entities, "print \"Speconly team\n\"" );
            else
                trap_SendServerCommand( ent-g_entities, "print \"Spectator team\n\"" );
            break;
        }
        return;
    }

    force = G_admin_permission(ent, ADMF_FORCETEAMCHANGE);

    if ( !force ) {
        if ( ent->client->switchTeamTime > level.time ) {
            trap_SendServerCommand( ent-g_entities, "print \"May not switch teams more than once per 5 seconds.\n\"" );
            return;
        }
    }

    /*if( level.demoState == DS_PLAYBACK ) {
        trap_SendServerCommand( ent-g_entities, "print \"You cannot join a team "
          "while a demo is being played\n\"" );
        return;
    }*/

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE && !level.warmupTime ) {
        ent->client->sess.losses++;
    }

    trap_Argv( 1, s, sizeof( s ) );

    SetTeam( ent, s );

    ent->client->switchTeamTime = level.time + 5000;
}


/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent ) {
    int		i;
    char	arg[MAX_TOKEN_CHARS];

    if ( trap_Argc() != 2 ) {
        if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
            StopFollowing( ent );
        }
        return;
    }


    trap_Argv( 1, arg, sizeof( arg ) );
    i = ClientNumberFromString( ent, arg );
    if ( i == -1 ) {
        return;
    }



    // can't follow self
    if ( &level.clients[ i ] == ent->client ) {
        return;
    }

    // can't follow another spectator (or an eliminated player)
    if ( (level.clients[ i ].sess.sessionTeam == TEAM_SPECTATOR) || level.clients[ i ].isEliminated) {
        return;
    }

    if ( (g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) && g_elimination_lockspectator.integer
            &&  ((ent->client->sess.sessionTeam == TEAM_RED && level.clients[ i ].sess.sessionTeam == TEAM_BLUE) ||
                 (ent->client->sess.sessionTeam == TEAM_BLUE && level.clients[ i ].sess.sessionTeam == TEAM_RED) ) ) {
        return;
    }

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE ) {
        ent->client->sess.losses++;
    }

    // first set them to spectator
    //if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
    if ( ent->client->sess.spectatorState == SPECTATOR_NOT ) {
        SetTeam( ent, "spectator" );
    }

    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
KK-OAX Modified to trap arguments.
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent ) {
    int		clientnum;
    int		original;
    int     count;
    char    args[11];
    int     dir;
    int i;

    /*for( i = 0 ; i < level.numConnectedClients; i++ ){
      if( level.clients[i].pers.demoClient )
    	  G_Printf("DEMO LIST %s, %i, %i\n", level.clients[i].pers.netname, level.clients[i].sess.sessionTeam, level.clients[i].sess.spectatorState );
      else
    	  G_Printf("NODEMO LIST %s, %i, %i\n", level.clients[i].pers.netname, level.clients[i].sess.sessionTeam, level.clients[i].sess.spectatorState );
    }*/

    if ( ent->client->sess.sessionTeam == TEAM_NONE ) {
        dir = 1;
    }

    //G_Printf("%s, %i, %i\n", ent->client->pers.netname, ent->client->sess.sessionTeam, ent->client->sess.spectatorState );

    trap_Argv( 0, args, sizeof( args ) );
    if ( Q_stricmp( args, "followprev" ) == 0 ) {
        dir = -1;
    } else if ( Q_stricmp( args, "follownext" ) == 0 ) {
        dir = 1;
    } else {
        dir = 1;
    }

    // if they are playing a tournement game, count as a loss
    if ( (g_gametype.integer == GT_TOURNAMENT )
            && ent->client->sess.sessionTeam == TEAM_FREE ) {
        ent->client->sess.losses++;
    }
    // first set them to spectator
    if ( ent->client->sess.spectatorState == SPECTATOR_NOT ) {
        SetTeam( ent, "spectator" );
    }

    if ( dir != 1 && dir != -1 ) {
        G_Error( "Cmd_FollowCycle_f: bad dir %i", dir );
    }

    clientnum = ent->client->sess.spectatorClient;
    original = clientnum;
    count = 0;
    do {
        clientnum += dir;
        count++;
        if ( clientnum >= level.maxclients ) {
            clientnum = 0;
        }
        if ( clientnum < 0 ) {
            clientnum = level.maxclients - 1;
        }

        //if (count>level.maxclients) //We have looked at all clients at least once and found nothing
        if ( count > level.maxclients )
            return; //We might end up in an infinite loop here. Stop it!*/

        // can only follow connected clients
        if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED /*&& !level.clients[ clientnum ].pers.demoClient*/ ) {
            //G_Printf("1.%s, %i, %i\n", level.clients[ clientnum ].pers.netname, level.clients[ clientnum ].sess.sessionTeam, level.clients[ clientnum ].sess.spectatorState );
            continue;
        }

        // can't follow another spectator
        if ( ((level.clients[ clientnum ].sess.sessionTeam == TEAM_SPECTATOR) /*&& !level.clients[ clientnum ].pers.demoClient*/ )|| level.clients[ clientnum ].isEliminated) {
            //G_Printf("2.%s, %i, %i\n", level.clients[ clientnum ].pers.netname, level.clients[ clientnum ].sess.sessionTeam, level.clients[ clientnum ].sess.spectatorState );
            continue;
        }

        //Stop players from spectating players on the enemy team in elimination modes.
        if ( (g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) && g_elimination_lockspectator.integer
                &&  ((ent->client->sess.sessionTeam == TEAM_RED && level.clients[ clientnum ].sess.sessionTeam == TEAM_BLUE) ||
                     (ent->client->sess.sessionTeam == TEAM_BLUE && level.clients[ clientnum ].sess.sessionTeam == TEAM_RED) ) ) {
            continue;
        }

        // this is good, we can use it
        ent->client->sess.spectatorClient = clientnum;
        ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
        //G_Printf("%i %i\n", ent->client->sess.spectatorClient, ent->client->sess.spectatorState );
        return;
    } while ( clientnum != original );

    // leave it where it was
}


/*
==================
G_Say
==================
*/

static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message ) {
    if (!other) {
        return;
    }
    if (!other->inuse) {
        return;
    }
    if (!other->client) {
        return;
    }
    if ( other->client->pers.connected != CON_CONNECTED ) {
        return;
    }
    if ( mode == SAY_TEAM  && !OnSameTeam(ent, other) ) {
        return;
    }

    if ((ent->r.svFlags & SVF_BOT) && trap_Cvar_VariableValue( "bot_nochat" )>1) return;

    // no chatting to players in tournements
    if ( (g_gametype.integer == GT_TOURNAMENT && !level.warmupTime )
            && other->client->sess.sessionTeam == TEAM_FREE
            && ent->client->sess.sessionTeam != TEAM_FREE ) {
        return;
    }

    trap_SendServerCommand( other-g_entities, va("%s \"%s%c%c%s\"",
                            mode == SAY_TEAM ? "tchat" : "chat",
                            name, Q_COLOR_ESCAPE, color, message));
}

#define EC		"\x19"

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) {
    int 		counter;
    int			j;
    gentity_t	*other;
    int			color;
    char		name[64];
    // don't let text be too long for malicious reasons
    char		text[MAX_SAY_TEXT];
    char		location[64];
    trace_t		tr;
    vec3_t		end, forward, right, up, muzzle;
    int		count;
    int 		enemies;
    qboolean	powerup;
    char	userinfo[MAX_INFO_STRING];
    float		fov;
    vec3_t		angles;
    vec3_t		dist;

    if ((ent->r.svFlags & SVF_BOT) && trap_Cvar_VariableValue( "bot_nochat" )>1) return;

    if ( ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) && !level.warmupTime && g_muteSpec.integer ) {
        mode = SAY_TEAM;
    }

    if ( (g_gametype.integer < GT_TEAM || g_ffa_gt == 1) && mode == SAY_TEAM ) {
        mode = SAY_ALL;
    }



    switch ( mode ) {
    default:
    case SAY_ALL:
        G_LogPrintf( "say: %s: %s\n", ent->client->pers.netname, chatText );
        Com_sprintf (name, sizeof(name), "%s%c%c"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
        color = COLOR_GREEN;
        //G_DemoCommand( DC_SERVER_COMMAND, va( "chat \"%s^2%s\"", name, chatText ) );
        break;
    case SAY_TEAM:
        G_LogPrintf( "sayteam: %s: %s\n", ent->client->pers.netname, chatText );
        if (Team_GetLocationMsg(ent, location, sizeof(location)))
            Com_sprintf (name, sizeof(name), EC"(%s%c%c"EC") (%s)"EC": ",
                         ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location);
        else
            Com_sprintf (name, sizeof(name), EC"(%s%c%c"EC")"EC": ",
                         ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
        color = COLOR_CYAN;
        //G_DemoCommand( DC_SERVER_COMMAND, va( "tchat \"%s^5%s\"", name, chatText ) );
        break;
    case SAY_TELL:
        if (target && g_gametype.integer >= GT_TEAM && g_ffa_gt != 1 &&
                target->client->sess.sessionTeam == ent->client->sess.sessionTeam &&
                Team_GetLocationMsg(ent, location, sizeof(location)))
            Com_sprintf (name, sizeof(name), EC"[%s%c%c"EC"] (%s)"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
        else
            Com_sprintf (name, sizeof(name), EC"[%s%c%c"EC"]"EC": ", ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
        color = COLOR_MAGENTA;
        break;
    }

    Q_strncpyz( text, chatText, sizeof(text) );


    if ( mode == SAY_TEAM  && ent->client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
        for ( counter = 0 ; counter < MAX_SAY_TEXT ; counter++ ) {
            if ( ( text[counter] == '#' ) && ( counter < MAX_SAY_TEXT - 1 ) ) {

                if ( text[counter + 1] == 'A' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( ent->client->ps.stats[STAT_ARMOR] < 50 )
                        Com_sprintf( text, sizeof(text), text, va("^1%i",ent->client->ps.stats[STAT_ARMOR]) );
                    else if ( ent->client->ps.stats[STAT_ARMOR] < 100 )
                        Com_sprintf( text, sizeof(text), text, va("^3%i",ent->client->ps.stats[STAT_ARMOR]) );
                    else
                        Com_sprintf( text, sizeof(text), text, va("^2%i",ent->client->ps.stats[STAT_ARMOR]) );
                }
                else if ( text[counter + 1] == 'a' ) {
                    text[counter] = '%';
                    text[counter + 1] = 'i';
                    Com_sprintf( text, sizeof(text), text, ent->client->ps.stats[STAT_ARMOR] );
                }
                else if ( text[counter + 1] == 'C' ) {

                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( ent->client->ps.persistant[PERS_KILLED] ) {
                        Team_GetDeathLocationMsg(ent, location, sizeof(location));
                        Com_sprintf( text, sizeof(text), text, location );
                    }
                    else
                        Com_sprintf( text, sizeof(text), text, "");
                }
                else if ( text[counter + 1] == 'd' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    Com_sprintf( text, sizeof(text), text, ent->client->lastDrop );
                }
                else if ( text[counter + 1] == 'D' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    if ( ent->client->lastAttacker != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[ent->client->lastAttacker].client->pers.netname );
                    else
                        Com_sprintf( text, sizeof(text), text, "");

                }
                else if ( text[counter + 1] == 'E' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    enemies = 0;

                    AngleVectors (ent->client->ps.viewangles, forward, right, up);
                    CalcMuzzlePoint ( ent, forward, right, up, muzzle );

                    trap_GetUserinfo( ent->client->ps.clientNum , userinfo, sizeof(userinfo) );
                    fov = atof( Info_ValueForKey( userinfo, "cg_fov" ) );

                    for ( count = 0; count < MAX_GENTITIES; count++ ) {

                        if ( g_entities[count].inuse && g_entities[count].s.eType == ET_PLAYER &&
                                ( g_entities[count].client->ps.persistant[PERS_TEAM] != ent->client->ps.persistant[PERS_TEAM] ) && ( ent->health > 0 ) &&
                                g_entities[count].client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {

                            trap_Trace(&tr, muzzle, NULL, NULL, g_entities[count].r.currentOrigin, ENTITYNUM_NONE, MASK_SOLID);

                            dist[0] = g_entities[count].r.currentOrigin[0]-muzzle[0];
                            dist[1] = g_entities[count].r.currentOrigin[1]-muzzle[1];
                            dist[2] = g_entities[count].r.currentOrigin[2]-muzzle[2];

                            vectoangles(dist, angles);

                            //G_Printf(" %f   %f   %f\n", angles[0], angles[1], angles[2]);

                            angles[1] = ent->client->ps.viewangles[1] - angles[1];

                            if ( angles[1] < -180 )
                                angles[1] += 360;
                            if ( angles[1] > 180 )
                                angles[1] -= 360;

                            angles[0] = ent->client->ps.viewangles[0] - angles[0];

                            if ( angles[0] < -180 )
                                angles[0] += 360;
                            if ( angles[0] > 180 )
                                angles[0] -= 360;

                            //G_Printf("angle %f   %f\n", angles[1], angles[0] );

                            if ( tr.fraction == 1.0 && ( angles[1] >= -fov/2 && angles[1] <= fov/2) && ( angles[0] >= -fov/2 && angles[0] <= fov/2) ) {
                                powerup = qfalse;
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_BLUEFLAG] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^4FLAG", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_REDFLAG] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^1FLAG", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_NEUTRALFLAG] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^7FLAG", "%s" ));
                                    powerup = qtrue;
                                }
                                /*if( g_entities[tr.entityNum].client->ps.powerups[PW_INVIS] == INT_MAX )
                                	Com_sprintf( text, sizeof(text), text, va("%s %s", "^7INVIS", "%s" ));*/
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_QUAD] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^5QUAD", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_REGEN] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^1REGEN", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_FLIGHT] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^2FLIGHT", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( g_entities[tr.entityNum].client->ps.powerups[PW_BATTLESUIT] == INT_MAX ) {
                                    Com_sprintf( text, sizeof(text), text, va("%s %s", "^2BATTLESUIT", "%s" ));
                                    powerup = qtrue;
                                }
                                if ( !powerup )
                                    enemies++;
                                else
                                    Com_sprintf( text, sizeof(text), text, va("+ %s","%s"));
                            }
                        }
                    }
                    if ( enemies == 0 )
                        Com_sprintf( text, sizeof(text), text, "0 enemies");
                    else if ( enemies == 1 )
                        Com_sprintf( text, sizeof(text), text, "1 enemy");
                    else
                        Com_sprintf( text, sizeof(text), text, va("%i enemies", enemies ));
                }
                else if ( text[counter + 1] == 'F' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( G_FindNearestTeammate( ent ) != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[G_FindNearestTeammate( ent )].client->pers.netname );
                    else
                        Com_sprintf( text, sizeof(text), text, "" );
                }
                else if ( text[counter + 1] == 'H' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( ent->health < 25 )
                        Com_sprintf( text, sizeof(text), text, va("^1%i",ent->health) );
                    else if ( ent->health < 75 )
                        Com_sprintf( text, sizeof(text), text, va("^3%i",ent->health) );
                    else
                        Com_sprintf( text, sizeof(text), text, va("^2%i",ent->health) );
                }
                else if ( text[counter + 1] == 'h' ) {
                    text[counter] = '%';
                    text[counter + 1] = 'i';
                    Com_sprintf( text, sizeof(text), text, ent->health );
                }
                else if ( text[counter + 1] == 'I' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( G_FindNearestItem( ent ) != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[G_FindNearestItem( ent )].item->shortPickup_name );
                    else
                        Com_sprintf( text, sizeof(text), text, "" );
                }
                else if ( text[counter + 1] == 'K' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    if ( ent->client->lastKiller != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[ent->client->lastKiller].client->pers.netname );
                    else
                        Com_sprintf( text, sizeof(text), text, "");

                }
                else if ( text[counter + 1] == 'l' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    if ( G_FindNearestItemSpawn( ent ) != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[G_FindNearestItemSpawn( ent )].item->shortPickup_name );
                    else
                        Com_sprintf( text, sizeof(text), text, "" );
                }
                else if ( text[counter + 1] == 'L' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    Team_GetLocationMsg(ent, location, sizeof(location));
                    Com_sprintf( text, sizeof(text), text, location );
                }
                else if ( text[counter + 1] == 'M' ) {
                    text[counter] = '%';
                    text[counter + 1] = 'i';
                    Com_sprintf( text, sizeof(text), text, ent->client->ps.ammo[ ent->client->ps.weapon ] );
                }
                else if ( text[counter + 1] == 'P' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    Com_sprintf( text, sizeof(text), text, ent->client->lastPickup );
                }
                else if ( text[counter + 1] == 'S' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    AngleVectors (ent->client->ps.viewangles, forward, right, up);
                    CalcMuzzlePoint ( ent, forward, right, up, muzzle );
                    VectorMA (muzzle, 8192 * 16, forward, end);
                    trap_Trace (&tr, muzzle, NULL, NULL, end, ent->s.number, MASK_ALL);

                    if ( g_entities[tr.entityNum].inuse && g_entities[tr.entityNum].s.eType == ET_ITEM )
                        Com_sprintf( text, sizeof(text), text, g_entities[tr.entityNum].item->shortPickup_name );
                    else
                        Com_sprintf( text, sizeof(text), text, "" );
                }
                else if ( text[counter + 1] == 'T' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    if ( ent->client->lastTarget != -1 )
                        Com_sprintf( text, sizeof(text), text, g_entities[ent->client->lastTarget].client->pers.netname );
                    else
                        Com_sprintf( text, sizeof(text), text, "");

                }
                else if ( text[counter + 1] == 'U' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';

                    if ( ent->client->ps.powerups[PW_BLUEFLAG] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^4FLAG", "%s" ));
                    if ( ent->client->ps.powerups[PW_REDFLAG] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^1FLAG", "%s" ));
                    if ( ent->client->ps.powerups[PW_NEUTRALFLAG] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^7FLAG", "%s" ));
                    if ( ent->client->ps.powerups[PW_INVIS] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^7INVIS", "%s" ));
                    if ( ent->client->ps.powerups[PW_QUAD] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^5QUAD", "%s" ));
                    if ( ent->client->ps.powerups[PW_REGEN] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^1REGEN", "%s" ));
                    if ( ent->client->ps.powerups[PW_FLIGHT] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^2FLIGHT", "%s" ));
                    if ( ent->client->ps.powerups[PW_BATTLESUIT] == INT_MAX )
                        Com_sprintf( text, sizeof(text), text, va("%s %s", "^2BATTLESUIT", "%s" ));
                    Com_sprintf( text, sizeof(text), text, "");

                }
                else if ( text[counter + 1] == 'W' ) {
                    text[counter] = '%';
                    text[counter + 1] = 's';
                    switch ( ent->client->ps.weapon ) {
                    case WP_GAUNTLET:
                        Com_sprintf( text, sizeof(text), text, "^5Gaunt" );
                        break;
                    case WP_MACHINEGUN:
                        Com_sprintf( text, sizeof(text), text, "^3MG^5" );
                        break;
                    case WP_SHOTGUN:
                        Com_sprintf( text, sizeof(text), text, "^3SG^5" );
                        break;
                    case WP_GRENADE_LAUNCHER:
                        Com_sprintf( text, sizeof(text), text, "^2GL^5" );
                        break;
                    case WP_ROCKET_LAUNCHER:
                        Com_sprintf( text, sizeof(text), text, "^1RL^5" );
                        break;
                    case WP_LIGHTNING:
                        Com_sprintf( text, sizeof(text), text, "^7LG^5" );
                        break;
                    case WP_RAILGUN:
                        Com_sprintf( text, sizeof(text), text, "^2RG^5" );
                        break;
                    case WP_PLASMAGUN:
                        Com_sprintf( text, sizeof(text), text, "^6PG^5" );
                        break;
                    case WP_BFG:
                        Com_sprintf( text, sizeof(text), text, "^5BFG" );
                        break;
                    default:
                        Com_sprintf( text, sizeof(text), text, "" );
                    }
                }
            }
        }
    }

    if ( target ) {
        if ( !target->client->mute[ent->s.clientNum] )
            G_SayTo( ent, target, mode, color, name, text );
        return;
    }

    // echo the text to the console
    if ( g_dedicated.integer ) {
        G_Printf( "%s%s\n", name, text);
    }

    // send it to all the apropriate clients
    for (j = 0; j < level.maxclients; j++) {
        other = &g_entities[j];
        if ( !other->client->mute[ent->s.clientNum] )
            G_SayTo( ent, other, mode, color, name, text );
    }
    //KK-OAX Admin Command Check from Say/SayTeam line
    if ( g_adminParseSay.integer )
    {
        G_admin_cmd_check ( ent, qtrue );
    }
}


/*
==================
Cmd_Say_f
KK-OAX Modified this to trap the additional arguments from console.
==================
*/
static void Cmd_Say_f( gentity_t *ent ) {
    char		*p;
    char        arg[MAX_TOKEN_CHARS];
    int         mode = SAY_ALL;

    trap_Argv( 0, arg, sizeof( arg ) );
    if ( Q_stricmp ( arg, "say_team" ) == 0 )
        mode = SAY_TEAM ;
    // KK-OAX Disabled until PM'ing is added
    // support parsing /m out of say text since some people have a hard
    // time figuring out what the console is.
    /*if( !Q_stricmpn( args, "say /m ", 7 ) ||
      !Q_stricmpn( args, "say_team /m ", 12 ) ||
      !Q_stricmpn( args, "say /mt ", 8 ) ||
      !Q_stricmpn( args, "say_team /mt ", 13 ) )
    {
        Cmd_PrivateMessage_f( ent );
        return;
    }

    // support parsing /a out of say text for the same reason
    if( !Q_stricmpn( args, "say /a ", 7 ) ||
    !Q_stricmpn( args, "say_team /a ", 12 ) )
    {
        Cmd_AdminMessage_f( ent );
        return;
    }*/

    if ( trap_Argc( ) < 2 )
        return;

    p = ConcatArgs( 1 );

    G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent ) {
    int			targetNum;
    gentity_t	*target;
    char		*p;
    char		arg[MAX_TOKEN_CHARS];

    if ( trap_Argc () < 2 ) {
        return;
    }

    trap_Argv( 1, arg, sizeof( arg ) );
    targetNum = atoi( arg );
    if ( targetNum < 0 || targetNum >= level.maxclients ) {
        return;
    }

    target = &g_entities[targetNum];
    if ( !target || !target->inuse || !target->client ) {
        return;
    }

    p = ConcatArgs( 2 );

    G_LogPrintf( "tell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, p );
    G_Say( ent, target, SAY_TELL, p );
    // don't tell to the player self if it was already directed to this player
    // also don't send the chat back to a bot
    if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
        G_Say( ent, ent, SAY_TELL, p );
    }
}


static void G_VoiceTo( gentity_t *ent, gentity_t *other, int mode, const char *id, qboolean voiceonly ) {
    int color;
    char *cmd;

    if (!other) {
        return;
    }
    if (!other->inuse) {
        return;
    }
    if (!other->client) {
        return;
    }
    if ( mode == SAY_TEAM && !OnSameTeam(ent, other) ) {
        return;
    }
    // no chatting to players in tournements
    if ( (g_gametype.integer == GT_TOURNAMENT )) {
        return;
    }

    if (mode == SAY_TEAM) {
        color = COLOR_CYAN;
        cmd = "vtchat";
    }
    else if (mode == SAY_TELL) {
        color = COLOR_MAGENTA;
        cmd = "vtell";
    }
    else {
        color = COLOR_GREEN;
        cmd = "vchat";
    }

    trap_SendServerCommand( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
}

void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly ) {
    int			j;
    gentity_t	*other;

    if ( (g_gametype.integer < GT_TEAM || g_ffa_gt==1 ) && mode == SAY_TEAM ) {
        mode = SAY_ALL;
    }

    if ( target ) {
        G_VoiceTo( ent, target, mode, id, voiceonly );
        return;
    }

    // echo the text to the console
    if ( g_dedicated.integer ) {
        G_Printf( "voice: %s %s\n", ent->client->pers.netname, id);
    }

    // send it to all the apropriate clients
    for (j = 0; j < level.maxclients; j++) {
        other = &g_entities[j];
        G_VoiceTo( ent, other, mode, id, voiceonly );
    }
}

/*
==================
Cmd_Voice_f
KK-OAX Modified this to trap args.

In the original, every call to this function would always set "arg0" to false, and it was
never passed along to other functions, so I removed/commented it out.
==================
*/
static void Cmd_Voice_f( gentity_t *ent ) {
    char		*p;
    char        arg[MAX_TOKEN_CHARS];
    int         mode = SAY_ALL;
    qboolean    voiceonly = qfalse;

    trap_Argv( 0, arg, sizeof( arg ) );
    if ((Q_stricmp ( arg, "vsay_team" ) == 0 ) ||
            Q_stricmp ( arg, "vosay_team" ) == 0 )
        mode = SAY_TEAM;

    if ((Q_stricmp ( arg, "vosay" ) == 0 ) ||
            Q_stricmp ( arg, "vosay_team" ) == 0 )
        voiceonly = qtrue;

    //KK-OAX Removed "arg0" since it will always be set to qfalse.
    if ( trap_Argc () < 2  ) {
        return;
    }
    //KK-OAX This was tricky to figure out, but since none of the original command handlings
    //set it to "qtrue"...

    /*if (arg0)
    {
    	p = ConcatArgs( 0 );
    }
    else
    {*/
    p = ConcatArgs( 1 );
    //}

    G_Voice( ent, NULL, mode, p, voiceonly );
}

/*
==================
Cmd_VoiceTell_f
KK-OAX Modified this to trap args.
==================
*/
static void Cmd_VoiceTell_f( gentity_t *ent ) {
    int			targetNum;
    gentity_t	*target;
    char		*id;
    char		arg[MAX_TOKEN_CHARS];
    qboolean    voiceonly = qfalse;

    if ( trap_Argc () < 2 ) {
        return;
    }

    trap_Argv( 0, arg, sizeof( arg ) );
    if ( Q_stricmp ( arg, "votell" ) == 0 )
        voiceonly = qtrue;

    trap_Argv( 1, arg, sizeof( arg ) );
    targetNum = atoi( arg );
    if ( targetNum < 0 || targetNum >= level.maxclients ) {
        return;
    }

    target = &g_entities[targetNum];
    if ( !target || !target->inuse || !target->client ) {
        return;
    }

    id = ConcatArgs( 2 );

    G_LogPrintf( "vtell: %s to %s: %s\n", ent->client->pers.netname, target->client->pers.netname, id );
    G_Voice( ent, target, SAY_TELL, id, voiceonly );
    // don't tell to the player self if it was already directed to this player
    // also don't send the chat back to a bot
    if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
        G_Voice( ent, ent, SAY_TELL, id, voiceonly );
    }
}


/*
==================
Cmd_VoiceTaunt_f
==================
*/
static void Cmd_VoiceTaunt_f( gentity_t *ent ) {
    gentity_t *who;
    int i;

    if (!ent->client) {
        return;
    }

    // insult someone who just killed you
    if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number) {
        // i am a dead corpse
        if (!(ent->enemy->r.svFlags & SVF_BOT)) {
            G_Voice( ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
        }
        if (!(ent->r.svFlags & SVF_BOT)) {
            G_Voice( ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
        }
        ent->enemy = NULL;
        return;
    }
    // insult someone you just killed
    if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number) {
        who = g_entities + ent->client->lastkilled_client;
        if (who->client) {
            // who is the person I just killed
            if (who->client->lasthurt_mod == MOD_GAUNTLET) {
                if (!(who->r.svFlags & SVF_BOT)) {
                    G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );	// and I killed them with a gauntlet
                }
                if (!(ent->r.svFlags & SVF_BOT)) {
                    G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );
                }
            } else {
                if (!(who->r.svFlags & SVF_BOT)) {
                    G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );	// and I killed them with something else
                }
                if (!(ent->r.svFlags & SVF_BOT)) {
                    G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );
                }
            }
            ent->client->lastkilled_client = -1;
            return;
        }
    }

    if (g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
        // praise a team mate who just got a reward
        for (i = 0; i < MAX_CLIENTS; i++) {
            who = g_entities + i;
            if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
                if (who->client->rewardTime > level.time) {
                    if (!(who->r.svFlags & SVF_BOT)) {
                        G_Voice( ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse );
                    }
                    if (!(ent->r.svFlags & SVF_BOT)) {
                        G_Voice( ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse );
                    }
                    return;
                }
            }
        }
    }

    // just say something
    G_Voice( ent, NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse );
}



static char	*gc_orders[] = {
    "hold your position",
    "hold this position",
    "come here",
    "cover me",
    "guard location",
    "search and destroy",
    "report"
};

void Cmd_GameCommand_f( gentity_t *ent ) {
    int		player;
    int		order;
    char	str[MAX_TOKEN_CHARS];

    trap_Argv( 1, str, sizeof( str ) );
    player = atoi( str );
    trap_Argv( 2, str, sizeof( str ) );
    order = atoi( str );

    if ( player < 0 || player >= MAX_CLIENTS ) {
        return;
    }
    if ( order < 0 || order > sizeof(gc_orders)/sizeof(char *) ) {
        return;
    }
    G_Say( ent, &g_entities[player], SAY_TELL, gc_orders[order] );
    G_Say( ent, ent, SAY_TELL, gc_orders[order] );
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) {
    trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}

static const char *gameNames[] = {
    "Free For All",
    "Tournament",
    "Single Player",
    "Team Deathmatch",
    "Capture the Flag",
    "One Flag CTF",
    "Overload",
    "Harvester",
    "Elimination",
    "CTF Elimination",
    "Last Man Standing",
    "Double Domination",
    "Domination"
};

void Cmd_Ref_f( gentity_t *ent ) {
    char*	c;
    char	arg1[MAX_STRING_TOKENS];
    char	arg2[MAX_STRING_TOKENS];
    char	arg3[MAX_STRING_TOKENS];
    char    buffer[256];
    char	command[1024];
    int	i;

    if ( !ent->client->referee ) {
        trap_SendServerCommand( ent-g_entities, "print \"You are not referee.\n\"" );
        trap_SendServerCommand(ent-g_entities,va("screenPrint \"" S_COLOR_CYAN "You are not referee\"" ) );
        return;
    }

    trap_Argv( 1, arg1, sizeof( arg1 ) );
    trap_Argv( 2, arg2, sizeof( arg2 ) );
    trap_Argv( 3, arg3, sizeof( arg3 ) );

    for ( c = arg2; *c; ++c) {
        switch (*c) {
        case '\n':
        case '\r':
        case ';':
            trap_SendServerCommand( ent-g_entities, "print \"Invalid ref-string.\n\"" );
            return;
            break;
        }
    }

    if ( !Q_stricmp( arg1, "map_restart" ) ) {
    } else if ( !Q_stricmp( arg1, "nextmap" ) ) {
    } else if ( !Q_stricmp( arg1, "map" ) ) {
    } else if ( !Q_stricmp( arg1, "g_gametype" ) ) {
    } else if ( !Q_stricmp( arg1, "kick" ) ) {
    } else if ( !Q_stricmp( arg1, "clientkick" ) ) {
    } else if ( !Q_stricmp( arg1, "timelimit" ) ) {
    } else if ( !Q_stricmp( arg1, "fraglimit" ) ) {
    } else if ( !Q_stricmp( arg1, "remove" ) ) {
    } else if ( !Q_stricmp( arg1, "clientremove" ) ) {
    } else if ( !Q_stricmp( arg1, "lock" ) ) {
    } else if ( !Q_stricmp( arg1, "unlock" ) ) {
    } else if ( !Q_stricmp( arg1, "startgame" ) ) {
    } else {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid ref-string.\n\"" );
        //trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, fraglimit <frags>.\n\"" );
        buffer[0] = 0;
        strcat(buffer,"print \"referee commands are: ");
        if (allowedRef("map_restart"))
            strcat(buffer, "map_restart, ");
        if (allowedRef("nextmap"))
            strcat(buffer, "nextmap, ");
        if (allowedRef("map"))
            strcat(buffer, "map <mapname>, ");
        if (allowedRef("g_gametype"))
            strcat(buffer, "g_gametype <n>, ");
        if (allowedRef("kick"))
            strcat(buffer, "kick <player>, ");
        if (allowedRef("clientkick"))
            strcat(buffer, "clientkick <clientnum>, ");
        if (allowedRef("timelimit"))
            strcat(buffer, "timelimit <time>, ");
        if (allowedRef("fraglimit"))
            strcat(buffer, "fraglimit <frags>, ");
        if (allowedRef("remove"))
            strcat(buffer, "remove <player>, ");
        if (allowedRef("clientremove"))
            strcat(buffer, "clientremove <clientnum>, ");
        if (allowedRef("lock"))
            strcat(buffer, "lock, ");
        if (allowedRef("unlock"))
            strcat(buffer, "unlock, ");
        if (allowedRef("startgame"))
            strcat(buffer, "startgame, ");

        buffer[strlen(buffer)-2] = 0;
        strcat(buffer, ".\"");
        trap_SendServerCommand( ent-g_entities, buffer);
        return;
    }

    if (!allowedRef(arg1)) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed here.\n\"" );
        buffer[0] = 0;
        strcat(buffer,"print \"referee commands are: ");
        if (allowedRef("map_restart"))
            strcat(buffer, "map_restart, ");
        if (allowedRef("nextmap"))
            strcat(buffer, "nextmap, ");
        if (allowedRef("map"))
            strcat(buffer, "map <mapname>, ");
        if (allowedRef("g_gametype"))
            strcat(buffer, "g_gametype <n>, ");
        if (allowedRef("kick"))
            strcat(buffer, "kick <player>, ");
        if (allowedRef("clientkick"))
            strcat(buffer, "clientkick <clientnum>, ");
        if (allowedRef("timelimit"))
            strcat(buffer, "timelimit <time>, ");
        if (allowedRef("fraglimit"))
            strcat(buffer, "fraglimit <frags>, ");
        if (allowedRef("remove"))
            strcat(buffer, "remove <player>, ");
        if (allowedRef("clientremove"))
            strcat(buffer, "clientremove <clientnum>, ");
        if (allowedRef("lock"))
            strcat(buffer, "lock, ");
        if (allowedRef("unlock"))
            strcat(buffer, "unlock, ");
        if (allowedRef("startgame"))
            strcat(buffer, "startgame, ");
        buffer[strlen(buffer)-2] = 0;
        strcat(buffer, ".\"");
        trap_SendServerCommand( ent-g_entities, buffer);
        return;
    }

    // special case for g_gametype, check for bad values
    if ( !Q_stricmp( arg1, "g_gametype" ) ) {
        char	s[MAX_STRING_CHARS];
        i = atoi( arg2 );
        if ( i == GT_SINGLE_PLAYER || i < GT_FFA || i >= GT_MAX_GAME_TYPE) {
            trap_SendServerCommand( ent-g_entities, "print \"Invalid gametype.\n\"" );
            return;
        }

        if ( i== g_gametype.integer ) {
            trap_SendServerCommand( ent-g_entities, "print \"This is current gametype\n\"" );
            return;
        }

        if (!allowedGametype(arg2)) {
            trap_SendServerCommand( ent-g_entities, "print \"Gametype is not available.\n\"" );
            return;
        }

        if ( trap_Argc() >= 3 )
            trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );

        /*if(!allowedMap(arg3)){
                    trap_SendServerCommand( ent-g_entities, "print \"Map is not available.\n\"" );
                    return;
                }*/


        if (*s) {
            /*if( trap_Argc() >= 3 ){
            	Com_sprintf( command, sizeof( command ), "%s %d; map %s; set nextmap \"%s\"", arg1, i,arg3,s );
            }
            else{*/
            Com_sprintf( command, sizeof( command ), "%s %d; map_restart; set nextmap \"%s\"", arg1, i,s );
            //}
        } else {
            /*if( trap_Argc() >= 3 ){
            	Com_sprintf( command, sizeof( command ), "%s %d; map", arg1, i, arg3);
            }
            else{*/
            Com_sprintf( command, sizeof( command ), "%s %d; map_restart", arg1, i );
            //}
        }
    } else if ( !Q_stricmp( arg1, "map" ) ) {
        // special case for map changes, we want to reset the nextmap setting
        // this allows a player to change maps, but not upset the map rotation
        char	s[MAX_STRING_CHARS];

        if (!allowedMap(arg2)) {
            trap_SendServerCommand( ent-g_entities, "print \"Map is not available.\n\"" );
            return;
        }

        trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
        if (*s) {
            Com_sprintf( command, sizeof( command ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
        } else {
            Com_sprintf( command, sizeof( command ), "%s %s", arg1, arg2 );
        }
    } else if ( !Q_stricmp( arg1, "nextmap" ) ) {
        char	s[MAX_STRING_CHARS];
        char		mapname[MAX_MAPNAME];

        if ( g_useMapcycle.integer ) {
            trap_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));
            Com_sprintf(command, sizeof( command ),"map %s", G_GetNextMap(mapname));
        }
        else {

            trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
            if (!*s) {
                trap_SendServerCommand( ent-g_entities, "print \"nextmap not set.\n\"" );
                return;
            }
            Com_sprintf( command, sizeof( command ), "vstr nextmap");
        }

    } else if ( !Q_stricmp( arg1, "fraglimit" ) ) {
        i = atoi(arg2);
        if (!allowedFraglimit(i)) {
            trap_SendServerCommand( ent-g_entities, "print \"Cannot set fraglimit.\n\"" );
            return;
        }

        Com_sprintf( command, sizeof( command ), "%s \"%d\"", arg1, i );

    } else if ( !Q_stricmp( arg1, "timelimit" ) ) {
        i = atoi(arg2);
        if (!allowedTimelimit(i)) {
            trap_SendServerCommand( ent-g_entities, "print \"Cannot set timelimit.\n\"" );
            return;
        }

        Com_sprintf( command, sizeof( command ), "%s \"%d\"", arg1, i );

    } else if ( !Q_stricmp( arg1, "map_restart" ) ) {
        Com_sprintf( command, sizeof( command ), "map_restart" );
    } else if ( !Q_stricmp( arg1, "clientkick" ) ) {
        i = atoi(arg2);

        if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
            trap_SendServerCommand( ent-g_entities, "print \"Cannot kick that number.\n\"" );
            return;
        }
        Com_sprintf( command, sizeof( command ), "clientkick_game \"%d\"", i );
    } else if ( !Q_stricmp( arg1, "kick" ) ) {
        i = 0;
        while (Q_stricmp(arg2,(g_entities+i)->client->pers.netname)) {
            //Not client i, try next
            i++;
            if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
                trap_SendServerCommand( ent-g_entities, "print \"Cannot find the playername. Try clientkick instead.\n\"" );
                return;
            }
        }

        Com_sprintf( command, sizeof( command ), "clientkick_game \"%d\"", i );
    } else if ( !Q_stricmp( arg1, "clientremove" ) ) {
        i = atoi(arg2);

        if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
            trap_SendServerCommand( ent-g_entities, "print \"Cannot remove that number.\n\"" );
            return;
        }
        if ( (g_entities+i)->client->sess.sessionTeam != TEAM_SPECTATOR ) {
            SetTeam( g_entities + i, "s" );
        }
        else {
            trap_SendServerCommand( ent-g_entities, "print \"Client is spec already.\n\"" );
        }
    }
    else if ( !Q_stricmp( arg1, "remove" ) ) {
        i = 0;
        while (Q_stricmp(arg2,(g_entities+i)->client->pers.netname)) {
            //Not client i, try next
            i++;
            if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
                trap_SendServerCommand( ent-g_entities, "print \"Cannot find the playername. Try clientremove instead.\n\"" );
                return;
            }
        }
        if ( (g_entities+i)->client->sess.sessionTeam != TEAM_SPECTATOR ) {
            SetTeam( g_entities + i, "s" );
        }
        else {
            trap_SendServerCommand( ent-g_entities, "print \"Client is spec already.\n\"" );
        }
    } else if ( !Q_stricmp( arg1, "lock" ) ) {
        if ( g_teamLock.integer ) {
            if ( g_gametype.integer >= GT_TEAM ) {
                //level.RedTeamLocked = qtrue;
                //level.BlueTeamLocked = qtrue;
                trap_Cvar_Set("g_redLocked","1");
                trap_Cvar_Set("g_blueLocked","1");
                trap_SendServerCommand( ent-g_entities, "print \"Teams are locked now.\n\"" );
                trap_SendServerCommand( -1, "screenPrint \"Teams are locked now.\n\"" );
                return;
            }
            trap_SendServerCommand( ent-g_entities, "print \"Teamlock not allowed in this gametype.\n\"" );
            return;
        }
        trap_SendServerCommand( ent-g_entities, "print \"Teamlock not allowed on this server.\n\"" );
    } else if ( !Q_stricmp( arg1, "unlock" ) ) {
        if ( g_teamLock.integer ) {
            if ( g_gametype.integer >= GT_TEAM ) {
                //level.RedTeamLocked = qfalse;
                //level.BlueTeamLocked = qfalse;
                trap_Cvar_Set("g_redLocked","0");
                trap_Cvar_Set("g_blueLocked","0");
                trap_SendServerCommand( ent-g_entities, "print \"Teams are unlocked now.\n\"" );
                trap_SendServerCommand( -1, "screenPrint \"Teams are unlocked now.\n\"" );
                return;
            }
            trap_SendServerCommand( ent-g_entities, "print \"Teamlock not allowed in this gametype.\n\"" );
            return;
        }
        trap_SendServerCommand( ent-g_entities, "print \"Teamlock not allowed on this server.\n\"" );
    } else if ( !Q_stricmp( arg1, "startgame" ) ) {
        if ( !g_startWhenReady.integer ) {
            trap_SendServerCommand( ent-g_entities, "print \"g_startWhenReady not set on this server.\n\"" );
            return;
        }
        for (i = 0; i < MAX_CLIENTS; i++) {
            (g_entities+i)->client->ready = qtrue;
        }
    } else {
        //Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
        //Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
        trap_SendServerCommand( ent-g_entities, "print \"Server vality check failed, appears to be my fault. Sorry\n\"" );
        return;
    }

    trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", command ) );

}

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent ) {
    char*	c;
    int		i;
    char	arg1[MAX_STRING_TOKENS];
    char	arg2[MAX_STRING_TOKENS];
    char	arg3[MAX_STRING_TOKENS];
    char    buffer[256];
    qboolean mapAllowed = qtrue;

    if ( !g_allowVote.integer ) {
        trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
        return;
    }
    
    if( g_allowVote.integer == 3 ) {
	if ( level.warmupTime != -1 ){
	    trap_SendServerCommand( ent-g_entities, "print \"Voting only allowed during warmup.\n\"" );
	    return;
	}
    }

    if ( level.voteTime ) {
        trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
        return;
    }
    if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT ) {
        trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
        return;
    }
    if( level.time < g_disableVotingTime.integer * 1000 ) {
        trap_SendServerCommand( ent-g_entities, va( "print \"You have to wait %i seconds to call a vote .\n\"", g_disableVotingTime.integer ) );
        return;
    }

    // make sure it is a valid command to vote on
    trap_Argv( 1, arg1, sizeof( arg1 ) );
    trap_Argv( 2, arg2, sizeof( arg2 ) );
    trap_Argv( 3, arg3, sizeof( arg3 ) );
    
    if( g_allowVote.integer == 2 && ( !Q_stricmp( arg1, "map" ) || !Q_stricmp( arg1, "nextmap" ) ) ){
	if ( level.warmupTime != -1 ){
	    trap_SendServerCommand( ent-g_entities, "print \"Voting maps only allowed during warmup.\n\"" );
	    return;
	}
    }

    // check for command separators in arg2
    for ( c = arg2; *c; ++c) {
        switch (*c) {
        case '\n':
        case '\r':
        case ';':
            trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
            return;
            break;
        }
    }


    if ( !Q_stricmp( arg1, "map_restart" ) ) {
    } else if ( !Q_stricmp( arg1, "nextmap" ) ) {
    } else if ( !Q_stricmp( arg1, "map" ) ) {
    } else if ( !Q_stricmp( arg1, "g_gametype" ) ) {
    } else if ( !Q_stricmp( arg1, "kick" ) ) {
    } else if ( !Q_stricmp( arg1, "clientkick" ) ) {
    } else if ( !Q_stricmp( arg1, "g_doWarmup" ) ) {
    } else if ( !Q_stricmp( arg1, "timelimit" ) ) {
    } else if ( !Q_stricmp( arg1, "fraglimit" ) ) {
    } else if ( !Q_stricmp( arg1, "custom" ) ) {
    } else if ( !Q_stricmp( arg1, "shuffle" ) ) {
    } else {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        //trap_SendServerCommand( ent-g_entities, "print \"Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, fraglimit <frags>.\n\"" );
        buffer[0] = 0;
        strcat(buffer,"print \"Vote commands are: ");
        if (allowedVote("map_restart"))
            strcat(buffer, "map_restart, ");
        if (allowedVote("nextmap"))
            strcat(buffer, "nextmap, ");
        if (allowedVote("map"))
            strcat(buffer, "map <mapname>, ");
        if (allowedVote("g_gametype"))
            strcat(buffer, "g_gametype <n>, ");
        if (allowedVote("kick"))
            strcat(buffer, "kick <player>, ");
        if (allowedVote("clientkick"))
            strcat(buffer, "clientkick <clientnum>, ");
        if (allowedVote("g_doWarmup"))
            strcat(buffer, "g_doWarmup, ");
        if (allowedVote("timelimit"))
            strcat(buffer, "timelimit <time>, ");
        if (allowedVote("fraglimit"))
            strcat(buffer, "fraglimit <frags>, ");
        if (allowedVote("shuffle"))
            strcat(buffer, "shuffle, ");
        if (allowedVote("custom"))
            strcat(buffer, "custom <special>, ");
        buffer[strlen(buffer)-2] = 0;
        strcat(buffer, ".\"");
        trap_SendServerCommand( ent-g_entities, buffer);
        return;
    }

    if (!allowedVote(arg1)) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed here.\n\"" );
        buffer[0] = 0;
        strcat(buffer,"print \"Vote commands are: ");
        if (allowedVote("map_restart"))
            strcat(buffer, "map_restart, ");
        if (allowedVote("nextmap"))
            strcat(buffer, "nextmap, ");
        if (allowedVote("map"))
            strcat(buffer, "map <mapname>, ");
        if (allowedVote("g_gametype"))
            strcat(buffer, "g_gametype <n>, ");
        if (allowedVote("kick"))
            strcat(buffer, "kick <player>, ");
        if (allowedVote("clientkick"))
            strcat(buffer, "clientkick <clientnum>, ");
        if (allowedVote("shuffle"))
            strcat(buffer, "shuffle, ");
        if (allowedVote("g_doWarmup"))
            strcat(buffer, "g_doWarmup, ");
        if (allowedVote("timelimit"))
            strcat(buffer, "timelimit <time>, ");
        if (allowedVote("fraglimit"))
            strcat(buffer, "fraglimit <frags>, ");
        if (allowedVote("custom"))
            strcat(buffer, "custom <special>, ");
        buffer[strlen(buffer)-2] = 0;
        strcat(buffer, ".\"");
        trap_SendServerCommand( ent-g_entities, buffer);
        return;
    }

    // if there is still a vote to be executed
    if ( level.voteExecuteTime ) {
        level.voteExecuteTime = 0;
        trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
    }

    // special case for g_gametype, check for bad values
    if ( !Q_stricmp( arg1, "g_gametype" ) ) {
        char	s[MAX_STRING_CHARS];
        i = atoi( arg2 );
        if ( i == GT_SINGLE_PLAYER || i < GT_FFA || i >= GT_MAX_GAME_TYPE) {
            trap_SendServerCommand( ent-g_entities, "print \"Invalid gametype.\n\"" );
            return;
        }

        if ( i== g_gametype.integer ) {
            trap_SendServerCommand( ent-g_entities, "print \"This is current gametype\n\"" );
            return;
        }

        if (!allowedGametype(arg2)) {
            trap_SendServerCommand( ent-g_entities, "print \"Gametype is not available.\n\"" );
            return;
        }

        if ( trap_Argc() >= 3 )
            trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );

        if ( g_useMapcycle.integer ) {
            if ( !G_mapIsVoteable( arg2 ) ) {
                trap_SendServerCommand( ent-g_entities, "print \"Map is not allowed.\n\"" );
                G_drawAllowedMaps( ent );
                mapAllowed = qfalse;
            }
        }
        else {
            if (!allowedMap(arg3)) {
                trap_SendServerCommand( ent-g_entities, "print \"Map is not available.\n\"" );
                mapAllowed = qfalse;
            }
        }


        if (*s) {
            if ( trap_Argc() >= 3 && mapAllowed ) {
                Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d; map %s; set nextmap \"%s\"", arg1, i,arg3,s );
                Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change gametype to: %s   Map: %s?", gameNames[i], arg3 );
            }
            else {
                Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d; map_restart; set nextmap \"%s\"", arg1, i,s );
                Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change gametype to: %s?", gameNames[i] );
            }
        } else {
            if ( trap_Argc() >= 3  && mapAllowed) {
                Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d; map", arg1, i, arg3);
                Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change gametype to: %s   Map: %s?", gameNames[i], arg3 );
            }
            else {
                Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %d; map_restart", arg1, i );
                Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change gametype to: %s?", gameNames[i] );
            }
        }
    } else if ( !Q_stricmp( arg1, "map" ) ) {
        // special case for map changes, we want to reset the nextmap setting
        // this allows a player to change maps, but not upset the map rotation
        char	s[MAX_STRING_CHARS];

        if ( g_useMapcycle.integer ) {
            if ( !G_mapIsVoteable( arg2 ) ) {
                trap_SendServerCommand( ent-g_entities, "print \"Map is not allowed.\n\"" );
                G_drawAllowedMaps( ent );
                return;
            }
        }
        else {
            if (!allowedMap(arg2)) {
                trap_SendServerCommand( ent-g_entities, "print \"Map is not available.\n\"" );
                return;
            }
        }

        trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
        if (*s) {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s; set nextmap \"%s\"", arg1, arg2, s );
        } else {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
        }
        //Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change map to: %s?", arg2 );
    } else if ( !Q_stricmp( arg1, "nextmap" ) ) {
        char	s[MAX_STRING_CHARS];
        char		mapname[MAX_MAPNAME];

        if ( g_useMapcycle.integer ) {
            trap_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));
            Com_sprintf(level.voteString, sizeof( level.voteString ),"map %s", G_GetNextMap(mapname));
	    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s(%s)", "Next map?", mapname );
        }
        else {
            trap_Cvar_VariableStringBuffer( "nextmap", s, sizeof(s) );
            if (!*s) {
                trap_SendServerCommand( ent-g_entities, "print \"nextmap not set.\n\"" );
                return;
            }
            Com_sprintf( level.voteString, sizeof( level.voteString ), "vstr nextmap");
	    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", "Next map?" );
        }
        //Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
        
    } else if ( !Q_stricmp( arg1, "fraglimit" ) ) {
        i = atoi(arg2);
        if (!allowedFraglimit(i)) {
            trap_SendServerCommand( ent-g_entities, "print \"Cannot set fraglimit.\n\"" );
            return;
        }

        Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%d\"", arg1, i );
        if (i)
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change fraglimit to: %d", i );
        else
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Remove fraglimit?");
    } else if ( !Q_stricmp( arg1, "timelimit" ) ) {
        i = atoi(arg2);
        if (!allowedTimelimit(i)) {
            trap_SendServerCommand( ent-g_entities, "print \"Cannot set timelimit.\n\"" );
            return;
        }

        Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%d\"", arg1, i );
        if (i)
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Change timelimit to: %d", i );
        else
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Remove timelimit?" );
    } else if ( !Q_stricmp( arg1, "map_restart" ) ) {
        Com_sprintf( level.voteString, sizeof( level.voteString ), "map_restart" );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Restart map?" );
    } else if ( !Q_stricmp( arg1, "g_doWarmup" ) ) {
        i = atoi(arg2);
        if (i) {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "g_doWarmup \"1\"" );
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Enable warmup?" );
        }
        else {
            Com_sprintf( level.voteString, sizeof( level.voteString ), "g_doWarmup \"0\"" );
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Disable warmup?" );
        }
    } else if ( !Q_stricmp( arg1, "clientkick" ) ) {
        i = atoi(arg2);

        if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
            trap_SendServerCommand( ent-g_entities, "print \"Cannot kick that number.\n\"" );
            return;
        }
        Com_sprintf( level.voteString, sizeof( level.voteString ), "clientkick_game \"%d\"", i );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Kick %s?" , level.clients[i].pers.netname );
    } else if ( !Q_stricmp( arg1, "shuffle" ) ) {
        if (g_gametype.integer<GT_TEAM || g_ffa_gt==1) { //Not a team game
            trap_SendServerCommand( ent-g_entities, "print \"Can only be used in team games.\n\"" );
            return;
        }

        Com_sprintf( level.voteString, sizeof( level.voteString ), "shuffle" );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Shuffle teams?" );
    } else if ( !Q_stricmp( arg1, "kick" ) ) {
        i = 0;
        while (Q_stricmp(arg2,(g_entities+i)->client->pers.netname)) {
            //Not client i, try next
            i++;
            if (i>=MAX_CLIENTS) { //Only numbers <128 is clients
                trap_SendServerCommand( ent-g_entities, "print \"Cannot find the playername. Try clientkick instead.\n\"" );
                return;
            }
        }

        Com_sprintf( level.voteString, sizeof( level.voteString ), "clientkick_game \"%d\"", i );
        Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Kick %s?" , level.clients[i].pers.netname );
    } else if ( !Q_stricmp( arg1, "custom" ) ) {
        t_customvote customvote;
        //Sago: There must always be a test to ensure that length(arg2) is non-zero or the client might be able to execute random commands.
        if (strlen(arg2)<1) {
            trap_SendServerCommand( ent-g_entities, va("print \"Custom vote commands are: %s\n\"",custom_vote_info) );
            return;
        }
        customvote = getCustomVote(arg2);
        if (Q_stricmp(customvote.votename,arg2)) {
            trap_SendServerCommand( ent-g_entities, "print \"Command could not be found\n\"" );
            return;
        }
        Com_sprintf( level.voteString, sizeof( level.voteString ), "%s", customvote.command );
        if (strlen(customvote.displayname))
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", customvote.displayname );
        else
            Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", customvote.command );
    } else {
        //Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
        //Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
        trap_SendServerCommand( ent-g_entities, "print \"Server vality check failed, appears to be my fault. Sorry\n\"" );
        return;
    }

    ent->client->pers.voteCount++;
    trap_SendServerCommand( -1, va("print \"%s called a vote.\n\"", ent->client->pers.netname ) );

    // start the voting, the caller autoamtically votes yes
    level.voteTime = level.time;
    level.voteYes = 1;
    level.voteNo = 0;

    for ( i = 0 ; i < level.maxclients ; i++ ) {
        level.clients[i].ps.eFlags &= ~EF_VOTED;
        level.clients[i].vote = 0;
	
	if( i == ent->s.clientNum ){
		G_Printf("%s\n", level.clients[i].pers.netname );
		level.clients[i].ps.eFlags |= EF_VOTED;
		level.clients[i].vote = 1;
	}
    }
    if( ent->s.clientNum == ent->client->ps.clientNum ){
	    ent->client->ps.eFlags |= EF_VOTED;
	    ent->client->vote = 1;
    }
    //Do a first count to make sure that numvotingclients is correct!
    CountVotes();

    trap_SetConfigstring( CS_VOTE_TIME, va("%i", level.voteTime ) );
    trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
    trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
    trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent ) {
    char		msg[64];

    if ( !level.voteTime ) {
        trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
        return;
    }
    if ( ent->client->ps.eFlags & EF_VOTED ) {
        trap_SendServerCommand( ent-g_entities, "print \"Vote already cast.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
        return;
    }

    trap_SendServerCommand( ent-g_entities, "print \"Vote cast.\n\"" );

    ent->client->ps.eFlags |= EF_VOTED;

    trap_Argv( 1, msg, sizeof( msg ) );

    if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
        ent->client->vote = 1;
    } else {
        ent->client->vote = -1;
    }

    //Re count the votes
    CountVotes();

    // a majority will be determined in CheckVote, which will also account
    // for players entering or leaving
}

/*
==================
Cmd_CallTeamVote_f
==================
*/
void Cmd_CallTeamVote_f( gentity_t *ent ) {
    int		i, team, cs_offset;
    char	arg1[MAX_STRING_TOKENS];
    char	arg2[MAX_STRING_TOKENS];

    team = ent->client->sess.sessionTeam;
    if ( team == TEAM_RED )
        cs_offset = 0;
    else if ( team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !g_allowVote.integer ) {
        trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
        return;
    }

    if ( level.teamVoteTime[cs_offset] ) {
        trap_SendServerCommand( ent-g_entities, "print \"A team vote is already in progress.\n\"" );
        return;
    }
    if ( ent->client->pers.teamVoteCount >= MAX_VOTE_COUNT ) {
        trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of team votes.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
        return;
    }

    // make sure it is a valid command to vote on
    trap_Argv( 1, arg1, sizeof( arg1 ) );
    arg2[0] = '\0';
    for ( i = 2; i < trap_Argc(); i++ ) {
        if (i > 2)
            strcat(arg2, " ");
        trap_Argv( i, &arg2[strlen(arg2)], sizeof( arg2 ) - strlen(arg2) );
    }

    if ( strchr( arg1, ';' ) || strchr( arg2, ';' ) ) {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        return;
    }

    if ( !Q_stricmp( arg1, "leader" ) ) {
        char netname[MAX_NETNAME], leader[MAX_NETNAME];

        if ( !arg2[0] ) {
            i = ent->client->ps.clientNum;
        }
        else {
            // numeric values are just slot numbers
            for (i = 0; i < 3; i++) {
                if ( !arg2[i] || arg2[i] < '0' || arg2[i] > '9' )
                    break;
            }
            if ( i >= 3 || !arg2[i]) {
                i = atoi( arg2 );
                if ( i < 0 || i >= level.maxclients ) {
                    trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %i\n\"", i) );
                    return;
                }

                if ( !g_entities[i].inuse ) {
                    trap_SendServerCommand( ent-g_entities, va("print \"Client %i is not active\n\"", i) );
                    return;
                }
            }
            else {
                Q_strncpyz(leader, arg2, sizeof(leader));
                Q_CleanStr(leader);
                for ( i = 0 ; i < level.maxclients ; i++ ) {
                    if ( level.clients[i].pers.connected == CON_DISCONNECTED )
                        continue;
                    if (level.clients[i].sess.sessionTeam != team)
                        continue;
                    Q_strncpyz(netname, level.clients[i].pers.netname, sizeof(netname));
                    Q_CleanStr(netname);
                    if ( !Q_stricmp(netname, leader) ) {
                        break;
                    }
                }
                if ( i >= level.maxclients ) {
                    trap_SendServerCommand( ent-g_entities, va("print \"%s is not a valid player on your team.\n\"", arg2) );
                    return;
                }
            }
        }
        Com_sprintf(arg2, sizeof(arg2), "%d", i);
    } else {
        trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
        trap_SendServerCommand( ent-g_entities, "print \"Team vote commands are: leader <player>.\n\"" );
        return;
    }

    Com_sprintf( level.teamVoteString[cs_offset], sizeof( level.teamVoteString[cs_offset] ), "%s %s", arg1, arg2 );

    for ( i = 0 ; i < level.maxclients ; i++ ) {
        if ( level.clients[i].pers.connected == CON_DISCONNECTED )
            continue;
        if (level.clients[i].sess.sessionTeam == team)
            trap_SendServerCommand( i, va("print \"%s called a team vote.\n\"", ent->client->pers.netname ) );
    }

    // start the voting, the caller autoamtically votes yes
    level.teamVoteTime[cs_offset] = level.time;
    level.teamVoteYes[cs_offset] = 1;
    level.teamVoteNo[cs_offset] = 0;

    for ( i = 0 ; i < level.maxclients ; i++ ) {
        if (level.clients[i].sess.sessionTeam == team)
            level.clients[i].ps.eFlags &= ~EF_TEAMVOTED;
    }
    ent->client->ps.eFlags |= EF_TEAMVOTED;

    trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, va("%i", level.teamVoteTime[cs_offset] ) );
    trap_SetConfigstring( CS_TEAMVOTE_STRING + cs_offset, level.teamVoteString[cs_offset] );
    trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset] ) );
    trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset] ) );
}

/*
==================
Cmd_TeamVote_f
==================
*/
void Cmd_TeamVote_f( gentity_t *ent ) {
    int			team, cs_offset;
    char		msg[64];

    team = ent->client->sess.sessionTeam;
    if ( team == TEAM_RED )
        cs_offset = 0;
    else if ( team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !level.teamVoteTime[cs_offset] ) {
        trap_SendServerCommand( ent-g_entities, "print \"No team vote in progress.\n\"" );
        return;
    }
    if ( ent->client->ps.eFlags & EF_TEAMVOTED ) {
        trap_SendServerCommand( ent-g_entities, "print \"Team vote already cast.\n\"" );
        return;
    }
    if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
        trap_SendServerCommand( ent-g_entities, "print \"Not allowed to vote as spectator.\n\"" );
        return;
    }

    trap_SendServerCommand( ent-g_entities, "print \"Team vote cast.\n\"" );

    ent->client->ps.eFlags |= EF_TEAMVOTED;

    trap_Argv( 1, msg, sizeof( msg ) );

    if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) {
        level.teamVoteYes[cs_offset]++;
        trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, va("%i", level.teamVoteYes[cs_offset] ) );
    } else {
        level.teamVoteNo[cs_offset]++;
        trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, va("%i", level.teamVoteNo[cs_offset] ) );
    }

    // a majority will be determined in TeamCheckVote, which will also account
    // for players entering or leaving
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) {
    vec3_t		origin, angles;
    char		buffer[MAX_TOKEN_CHARS];
    int			i;

    if ( !g_cheats.integer ) {
        trap_SendServerCommand( ent-g_entities, va("print \"Cheats are not enabled on this server.\n\""));
        return;
    }
    if ( trap_Argc() != 5 ) {
        trap_SendServerCommand( ent-g_entities, va("print \"usage: setviewpos x y z yaw\n\""));
        return;
    }

    VectorClear( angles );
    for ( i = 0 ; i < 3 ; i++ ) {
        trap_Argv( i + 1, buffer, sizeof( buffer ) );
        origin[i] = atof( buffer );
    }

    trap_Argv( 4, buffer, sizeof( buffer ) );
    angles[YAW] = atof( buffer );

    TeleportPlayer( ent, origin, angles );
}



/*
=================
Cmd_Stats_f
=================
*/
void Cmd_Stats_f( gentity_t *ent ) {
    /*
    	int max, n, i;

    	max = trap_AAS_PointReachabilityAreaIndex( NULL );

    	n = 0;
    	for ( i = 0; i < max; i++ ) {
    		if ( ent->client->areabits[i >> 3] & (1 << (i & 7)) )
    			n++;
    	}

    	//trap_SendServerCommand( ent-g_entities, va("print \"visited %d of %d areas\n\"", n, max));
    	trap_SendServerCommand( ent-g_entities, va("print \"%d%% level coverage\n\"", n * 100 / max));
    */
}

void Cmd_GetMappage_f( gentity_t *ent ) {
    t_mappage page;
    char string[(MAX_MAPNAME+1)*MAPS_PER_PAGE+1];
    char arg[MAX_STRING_TOKENS];
    trap_Argv( 1, arg, sizeof( arg ) );
    page = getMappage(atoi(arg));
    Q_strncpyz(string,va("mappage %d %s %s %s %s %s %s %s %s %s %s",page.pagenumber,page.mapname[0],\
                         page.mapname[1],page.mapname[2],page.mapname[3],page.mapname[4],page.mapname[5],\
                         page.mapname[6],page.mapname[7],page.mapname[8],page.mapname[9]),sizeof(string));
    //G_Printf("Mappage sent: \"%s\"\n", string);
    trap_SendServerCommand( ent-g_entities, string );
}

void Cmd_DropArmor_f( gentity_t *ent ) {
    gitem_t		*item;
    gentity_t	*out;
    char arg1[128];
    int amount;

    if ( !( g_itemDrop.integer & 16 ) )
        return;

    if ( ent->client->ps.pm_type == PM_DEAD )
        return;
    
    if( trap_Argc() > 1 ){
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	amount = atoi(arg1);     
    } else
	amount = 50;
  
    if( amount >= 100 )
	amount = 100;
    else if( amount >= 50 )
	amount = 50;
    //else if( amount >= 25 )
	 //amount = 25;
    else 
	amount = 5;
    
    item = BG_FindArmorForQuantity( amount );
    
    out = Drop_Item_Armor( ent, item, 0 );
}

void Cmd_DropHealth_f( gentity_t *ent ) {
    gitem_t		*item;
    gentity_t	*out;
    char arg1[128];
    int amount;

    if ( !( g_itemDrop.integer & 8 ) )
        return;

    if ( ent->client->ps.pm_type == PM_DEAD )
        return;
    
    if( trap_Argc() > 1 ){
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	amount = atoi(arg1);     
    } else
	amount = 25;
  
    if( amount >= 100 )
	amount = 100;
    else if( amount >= 50 )
	amount = 50;
    else if( amount >= 25 )
	amount = 25;
    else 
	amount = 5;
    
    item = BG_FindHealthForQuantity( amount );
    
    out = Drop_Item_Health( ent, item, 0 );
}

void Cmd_DropAmmo_f( gentity_t *ent ) {
    gitem_t		*item;
    gentity_t	*out;
    int			weapon;
    char arg1[128];

    if ( !( g_itemDrop.integer & 4 ) )
        return;

    if ( ent->client->ps.pm_type == PM_DEAD )
        return;
    
    if( trap_Argc() > 1 ){
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	weapon = atoi(arg1);     
    } else
	weapon = ent->s.weapon;
  
    if ( weapon <= WP_GAUNTLET || weapon >= WP_NUM_WEAPONS )
        return;
    
    item = BG_FindAmmoForWeapon( weapon );
    if ( ( ent->client->ps.stats[STAT_WEAPONS] & ( 1 << item->giTag ) ) ) {
        out = Drop_Item_Ammo( ent, item, 0 );
    }
}

void Cmd_DropWeapon_f( gentity_t *ent ) {
    gitem_t		*item;
    gentity_t	*out;
    int			weapon;
    char arg1[128];

    if ( !( g_itemDrop.integer & 2 ) )
        return;

    if ( ent->client->ps.pm_type == PM_DEAD )
        return;
    
    if( trap_Argc() > 1 ){
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	weapon = atoi(arg1);     
    } else
	weapon = ent->s.weapon;
  
    if ( weapon <= WP_GAUNTLET || weapon >= WP_NUM_WEAPONS )
        return;

    item = BG_FindItemForWeapon( weapon );
    if ( ( ent->client->ps.stats[STAT_WEAPONS] & ( 1 << item->giTag ) ) ) {
        out = Drop_Item_Weapon( ent, item, 0 );
    }
}

void Cmd_DropFlag_f( gentity_t *other ) {

    if ( !( g_itemDrop.integer & 1 ) )
        return;

    if ( other->client->ps.pm_type == PM_DEAD )
        return;

    if ( other->client->ps.powerups[PW_NEUTRALFLAG] ) {
        Drop_Item_Flag( other, BG_FindItemForPowerup( PW_NEUTRALFLAG ), 0 );
        other->client->ps.powerups[PW_NEUTRALFLAG] = 0;
    }
    else if ( other->client->ps.powerups[PW_REDFLAG] ) {
        Drop_Item_Flag( other, BG_FindItemForPowerup( PW_REDFLAG ), 0 );
        other->client->ps.powerups[PW_REDFLAG] = 0;
    }
    else if ( other->client->ps.powerups[PW_BLUEFLAG] ) {
        Drop_Item_Flag( other, BG_FindItemForPowerup( PW_BLUEFLAG ), 0 );
        other->client->ps.powerups[PW_BLUEFLAG] = 0;
    }
}

void Cmd_Drop_f( gentity_t *ent ) {

    if ( ( ent->client->ps.powerups[PW_NEUTRALFLAG] || ent->client->ps.powerups[PW_REDFLAG] || ent->client->ps.powerups[PW_BLUEFLAG] ) && g_itemDrop.integer & 1 )
        Cmd_DropFlag_f( ent );
    else if ( g_itemDrop.integer & 2 )
        Cmd_DropWeapon_f( ent );

}

void Cmd_Lock_f( gentity_t *ent ) {

    if ( g_teamLock.integer != 1 )
        return;

    if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_RED && !g_redLocked.integer ) {
        //level.RedTeamLocked = qtrue;
        trap_Cvar_Set("g_redLocked","1");
        trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_RED "Red team" S_COLOR_YELLOW" locked\"") );
    }
    if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_BLUE && !g_blueLocked.integer ) {
        //level.BlueTeamLocked = qtrue;
        trap_Cvar_Set("g_blueLocked","1");
        trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_BLUE "Blue team" S_COLOR_YELLOW" locked\"") );
    }

}

void Cmd_Unlock_f( gentity_t *ent ) {

    if ( g_teamLock.integer != 1 )
        return;

    if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_RED && g_redLocked.integer ) {
        //level.RedTeamLocked = qfalse;
        trap_Cvar_Set("g_redLocked","0");
        trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_RED "Red team" S_COLOR_YELLOW" unlocked\"") );
    }
    if ( ent->client->ps.persistant[PERS_TEAM] == TEAM_BLUE && g_blueLocked.integer ) {
        //level.BlueTeamLocked = qfalse;
        trap_Cvar_Set("g_blueLocked","0");
        trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_BLUE "Blue team" S_COLOR_YELLOW" unlocked\"") );
    }

}

void Cmd_Listplayers_f( gentity_t *ent ) {
    int i = 0;
    char buffer[1024];
    gclient_t *cl;
    buffer[0] = '\0';
    strcat(buffer, va("<clientNum>   <name>   <score>   <team>\n" ) );
    while ( i < MAX_CLIENTS ) {
        cl = (g_entities+i)->client;
        if ((g_entities+i)->client->pers.connected == CON_CONNECTED) { //Only numbers <128 is clients
            strcat(buffer, va("%i   %s^7   %i   %s   ", (g_entities+i)->s.clientNum, cl->pers.netname, cl->ps.persistant[PERS_SCORE], TeamName(cl->sess.sessionTeam) ) );

            if ( ent->client->mute[(g_entities+i)->s.clientNum] )
                strcat(buffer, "^1MUTED^7\n");
            else
                strcat(buffer,"\n");
        }
        i++;
    }
    trap_SendServerCommand( ent-g_entities, va("print \"%s\"", buffer));
}

void Cmd_Listmapcycle_f( gentity_t *ent ) {
    G_drawMapcycle( ent );
}

static qboolean isNumber( char *arg1 ) {
    int i;
    for ( i = 0; i < strlen(arg1); i++ ) {
        if ( arg1[i] < '0' || arg1[i] > '9' )
            return qfalse;
    }
    return qtrue;
}

void Cmd_Mute_f( gentity_t *ent ) {
    int num = -1;
    char arg1[MAX_STRING_TOKENS];

    if ( trap_Argc() == 1 || trap_Argc() > 2 )
        trap_SendServerCommand( ent-g_entities, "print \"usage: mute [clientnum]\n\"");

    trap_Argv( 1, arg1, sizeof( arg1 ) );

    if ( !isNumber(arg1) ) {
        trap_SendServerCommand( ent-g_entities, "print \"usage: mute [clientnum]\n\"");
        return;
    }

    num = atoi(arg1);
    if ( num >=0 && num < MAX_CLIENTS ) {
        if ( level.clients[num].pers.connected != CON_DISCONNECTED ) {
            ent->client->mute[num] = qtrue;
            trap_SendServerCommand( ent-g_entities, va("print \"muted %s\n\"",level.clients[num].pers.netname ));
        }
        else {
            trap_SendServerCommand( ent-g_entities, va("print \"client with clientnumber %i not connected\n\"", num ));
        }

    }
    else
        trap_SendServerCommand( ent-g_entities, "print \"Clientnumber out of Range\n\"");
}

void Cmd_Unmute_f( gentity_t *ent ) {
    int num = -1;
    char arg1[MAX_STRING_TOKENS];

    if ( trap_Argc() == 1 || trap_Argc() > 2 )
        trap_SendServerCommand( ent-g_entities, "print \"usage: unmute [clientnum]\n\"");

    trap_Argv( 1, arg1, sizeof( arg1 ) );

    if ( !isNumber(arg1) ) {
        trap_SendServerCommand( ent-g_entities, "print \"usage: unmute [clientnum]\n\"");
        return;
    }

    num = atoi(arg1);
    if ( num >=0 && num < MAX_CLIENTS ) {
        if ( level.clients[num].pers.connected != CON_DISCONNECTED ) {
            ent->client->mute[num] = qfalse;
            trap_SendServerCommand( ent-g_entities, va("print \"unmuted %s\n\"",level.clients[num].pers.netname ));
        }
        else {
            trap_SendServerCommand( ent-g_entities, va("print \"client with clientnumber %i not connected\n\"", num ));
        }

    }
    else
        trap_SendServerCommand( ent-g_entities, "print \"Clientnumber out of Range\n\"");
}

void Cmd_Forfeit_f( gentity_t *ent ) {
  
    if( g_gametype.integer != GT_TOURNAMENT ){
	  trap_SendServerCommand( ent-g_entities, va("screenPrint \"Forfeit is only available in tournament\"") );
	  trap_SendServerCommand( ent-g_entities, va("print \"Forfeit is only available in tournament\"") );
	  return;
    }
    
    if ( level.numPlayingClients != 2 )
	  return;
	
    if( ent->client->sess.sessionTeam != TEAM_FREE )
	  return;
    
    if ( level.warmupTime == -1 ){
	  trap_SendServerCommand( ent-g_entities, va("screenPrint \"Forfeit is not avaible during warmup\"") );
	  trap_SendServerCommand( ent-g_entities, va("print \"Forfeit is not avaible during warmup\"") );
	  return;
    }
    
    if( ent->client->ps.clientNum != level.sortedClients[1] ){
	  trap_SendServerCommand( ent-g_entities, va("screenPrint \"Forfeit is only avaible to the losing player\"") );
	  trap_SendServerCommand( ent-g_entities, va("print \"Forfeit is only avaible to the losing player\"") );
	  return;
    }
    
    if( g_entities[ level.sortedClients[0] ].client->ps.persistant[PERS_SCORE] > g_entities[ level.sortedClients[1] ].client->ps.persistant[PERS_SCORE] )
	  level.intermissionQueued = level.time;
    trap_SendServerCommand( -1, va("screenPrint \"Match has been forfeited.\"") );
    trap_SendServerCommand( -1, va("print \"Match has been forfeited.\"") );
  
}

//KK-OAX This is the table that ClientCommands runs the console entry against.
commands_t cmds[ ] =
{
    // normal commands
    { "team", 0, Cmd_Team_f },
    { "vote", 0, Cmd_Vote_f },
    /*{ "ignore", 0, Cmd_Ignore_f },
    { "unignore", 0, Cmd_Ignore_f },*/

    // communication commands
    { "tell", CMD_MESSAGE, Cmd_Tell_f },
    { "callvote", CMD_MESSAGE, Cmd_CallVote_f },
    { "callteamvote", CMD_MESSAGE|CMD_TEAM, Cmd_CallTeamVote_f },
    // can be used even during intermission
    { "say", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
    { "say_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
    { "vsay", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "vsay_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "vsay_local", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "vtell", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VoiceTell_f },
    { "vosay", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "vosay_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "vosay_local", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Voice_f },
    { "votell", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VoiceTell_f },
    { "vtaunt", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VoiceTaunt_f },
    /*{ "m", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
    { "mt", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
    { "a", CMD_MESSAGE|CMD_INTERMISSION, Cmd_AdminMessage_f },*/

    { "score", CMD_INTERMISSION, Cmd_Score_f },
    { "acc", CMD_INTERMISSION, Cmd_Acc_f},
    { "stats", CMD_INTERMISSION, G_SendStats },

    // cheats
    { "give", CMD_CHEAT|CMD_LIVING, Cmd_Give_f },
    { "god", CMD_CHEAT|CMD_LIVING, Cmd_God_f },
    { "notarget", CMD_CHEAT|CMD_LIVING, Cmd_Notarget_f },
    { "levelshot", CMD_CHEAT, Cmd_LevelShot_f },
    { "setviewpos", CMD_CHEAT, Cmd_SetViewpos_f },
    { "noclip", CMD_CHEAT, Cmd_Noclip_f },

    { "kill", CMD_TEAM|CMD_LIVING, Cmd_Kill_f },
    { "where", 0, Cmd_Where_f },

    // game commands

    { "follow", CMD_NOTEAM, Cmd_Follow_f },
    { "follownext", CMD_NOTEAM, Cmd_FollowCycle_f },
    { "followprev", CMD_NOTEAM, Cmd_FollowCycle_f },

    { "teamvote", CMD_TEAM, Cmd_TeamVote_f },
    { "teamtask", CMD_TEAM, Cmd_TeamTask_f },
    //KK-OAX
    { "freespectator", CMD_NOTEAM, StopFollowing },
    { "getmappage", 0, Cmd_GetMappage_f },
    { "gc", 0, Cmd_GameCommand_f },
    { "timeout", 0, Cmd_Timeout_f },
    { "ready", 0, Cmd_Ready_f },
    { "dropammo", 0, Cmd_DropAmmo_f },
    { "droparmor", 0, Cmd_DropArmor_f },
    { "drophealth", 0, Cmd_DropHealth_f },
    { "dropweapon", 0, Cmd_DropWeapon_f },
    { "dropflag", 0, Cmd_DropFlag_f },
    { "drop", 0, Cmd_Drop_f },
    { "lock", 0, Cmd_Lock_f },
    { "unlock", 0, Cmd_Unlock_f },
    { "ref", 0, Cmd_Ref_f },
    { "listplayers", 0, Cmd_Listplayers_f },
    { "mapcycle", 0, Cmd_Listmapcycle_f },
    { "mute", 0, Cmd_Mute_f },
    { "unmute", 0, Cmd_Unmute_f },
    { "forfeit", 0, Cmd_Forfeit_f }
};

static int numCmds = sizeof( cmds ) / sizeof( cmds[ 0 ] );

/*
=================
ClientCommand
KK-OAX, Takes the client command and runs it through a loop which matches
it against the table.
=================
*/
void ClientCommand( int clientNum )
{
    gentity_t *ent;
    char      cmd[ MAX_TOKEN_CHARS ];
    int       i;

    ent = g_entities + clientNum;
    if ( !ent->client )
        return;   // not fully in game yet

    trap_Argv( 0, cmd, sizeof( cmd ) );

    for ( i = 0; i < numCmds; i++ )
    {
        if ( Q_stricmp( cmd, cmds[ i ].cmdName ) == 0 )
            break;
    }

    if ( i == numCmds )
    {   // KK-OAX Admin Command Check
        if ( !G_admin_cmd_check( ent, qfalse ) )
            trap_SendServerCommand( clientNum,
                                    va( "print \"Unknown command %s\n\"", cmd ) );
        return;
    }

    // do tests here to reduce the amount of repeated code
    if ( !( cmds[ i ].cmdFlags & CMD_INTERMISSION ) && level.intermissiontime )
        return;

    if ( cmds[ i ].cmdFlags & CMD_CHEAT && !g_cheats.integer )
    {
        trap_SendServerCommand( clientNum,
                                "print \"Cheats are not enabled on this server\n\"" );
        return;
    }
    //KK-OAX When the corresponding code is integrated, I will activate these.
    //if( cmds[ i ].cmdFlags & CMD_MESSAGE &&
    //    ( ent->client->pers.muted || G_FloodLimited( ent ) ) )
    //    return;

    //KK-OAX Do I need to change this for FFA gametype?
    if ( cmds[ i ].cmdFlags & CMD_TEAM &&
            ent->client->sess.sessionTeam == TEAM_SPECTATOR )
    {
        trap_SendServerCommand( clientNum, "print \"Join a team first\n\"" );
        return;
    }

    if ( ( cmds[ i ].cmdFlags & CMD_NOTEAM ||
            ( cmds[ i ].cmdFlags & CMD_CHEAT_TEAM && !g_cheats.integer ) ) &&
            ent->client->sess.sessionTeam != TEAM_NONE )
    {
        trap_SendServerCommand( clientNum,
                                "print \"Cannot use this command when on a team\n\"" );
        return;
    }

    if ( cmds[ i ].cmdFlags & CMD_RED &&
            ent->client->sess.sessionTeam != TEAM_RED )
    {
        trap_SendServerCommand( clientNum,
                                "print \"Must be on the Red Team to use this command\n\"" );
        return;
    }

    if ( cmds[ i ].cmdFlags & CMD_BLUE &&
            ent->client->sess.sessionTeam != TEAM_BLUE )
    {
        trap_SendServerCommand( clientNum,
                                "print \"Must be on the Blue Team to use this command\n\"" );
        return;
    }

    if ( ( ent->client->ps.pm_type == PM_DEAD ) && ( cmds[ i ].cmdFlags & CMD_LIVING ) )
    {
        trap_SendServerCommand( clientNum,
                                "print \"Must be alive to use this command\n\"" );
        return;
    }

    cmds[ i ].cmdHandler( ent );
}

