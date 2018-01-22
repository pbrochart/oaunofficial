/*
===========================================================================
Copyright (C) 2010-2011 Manuel Wiese

This file is part of AfterShock source code.

AfterShock source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

AfterShock source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AfterShock source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

static const char *gameShortNames[] = {
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

void cleanString( char *dest, size_t max, const char *instring ){
	int i;
	int count = 0;
	
	for ( i = 0; i < max ; i++ ) {
		if ( instring[i] == '&' ) {
		  continue;
		}
		else {
			dest[count] = instring[i];
			count++;
		}
	}
	dest[count] = '\0';
}

void G_SetGameString ( void ) {
	qtime_t	now;
	//char		*p;
	char		mapname[MAX_MAPNAME];
	//char		*buf;
	char		playerName[128];
	int 		i, count;
	char gameString[128];

	trap_RealTime ( &now );

	trap_Cvar_VariableStringBuffer ( "mapname", mapname, sizeof ( mapname ) );

	if ( g_gametype.integer == GT_TOURNAMENT ) {
		// find the two active players
		Com_sprintf ( playerName, sizeof ( playerName ),"-%sVS%s", level.clients[level.sortedClients[0]].pers.netname, level.clients[level.sortedClients[1]].pers.netname );
	} else {
		Com_sprintf ( playerName, sizeof ( playerName ),"-" );
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

	Com_sprintf ( gameString, sizeof ( gameString ), "%04d-%02d-%02d/%04d%02d%02d%02d%02d%02d-%s%s-%s",
	              1900 + now.tm_year,
	              1 + now.tm_mon,
	              now.tm_mday,
	              1900 + now.tm_year,
	              1 + now.tm_mon,
	              now.tm_mday,
	              now.tm_hour,
	              now.tm_min,
	              now.tm_sec,
	              gameShortNames[g_gametype.integer],
	              playerName,
	              mapname );

	//G_Printf("gamestring: %s\n",gameString );
	trap_Cvar_Set( "gamestring", gameString );
	
	Com_sprintf ( gameString, sizeof ( gameString ), "%04d%02d%02d%02d%02d%02d-%s%s-%s",
	              1900 + now.tm_year,
	              1 + now.tm_mon,
	              now.tm_mday,
	              now.tm_hour,
	              now.tm_min,
	              now.tm_sec,
	              gameShortNames[g_gametype.integer],
	              playerName,
	              mapname );
	
	trap_Cvar_Set( "matchstring", gameString );
}

static void writeToFile ( char *string, fileHandle_t *f ) {
	int len =strlen ( string );
	trap_FS_Write ( string, len, *f );
}

static char *boolToChar ( qboolean in ) {
	if ( in )
		return "true";
	return "false";
}

static char *teamToChar ( int in ) {
	if ( in == TEAM_BLUE )
		return "Blue";
	else if ( in == TEAM_RED )
		return "Red";
	else if ( in == TEAM_FREE )
		return "Free";
	else if ( in == TEAM_SPECTATOR )
		return "Spectator";
	else
		return "None";
}

static void writePlayerData ( gclient_t *cl, fileHandle_t *f, qboolean disconnected ) {
	float time;
	int count;
	char cleanName[36], cleanAftershockName[36], cleanAftershockHash[36];
	int i;
	char demopath[MAX_QPATH];

	if ( !disconnected )
		time = (level.time - cl->pers.enterTime)/1000;
	else
		time = cl->pers.enterTime/1000;

	cleanString( cleanName, sizeof(cleanName), cl->pers.netname);
	cleanString( cleanAftershockName, sizeof(cleanAftershockName), cl->aftershock_name);
	cleanString( cleanAftershockHash, sizeof(cleanAftershockHash), cl->aftershock_hash);
	
	writeToFile ( va ( "\n\t\t<player name=\"%s\" time=\"%i\" aftershock_login=\"%s\" aftershock_hash=\"%s\" team=\"%s\" timeouts=\"%i\" handicap=\"%i\">\n", cleanName , (int)time, cleanAftershockName , cleanAftershockHash, teamToChar(cl->sess.sessionTeam), cl->timeouts, cl->pers.maxHealth ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"Score\" value=\"%i\"/>\n", cl->ps.persistant[PERS_SCORE] ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"Kills\" value=\"%i\"/>\n", cl->kills ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"Death\" value=\"%i\"/>\n", cl->ps.persistant[PERS_KILLED] ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"DamageGiven\" value=\"%i\"/>\n", cl->dmgdone ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"DamageTaken\" value=\"%i\"/>\n", cl->dmgtaken ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"HealthTotal\" value=\"%i\"/>\n", cl->stats[STATS_HEALTH] ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"ArmorTotal\" value=\"%i\"/>\n", cl->stats[STATS_ARMOR] ), f );
	writeToFile ( va ( "\t\t\t<stat name=\"IsBot\" value=\"%s\"/>\n", boolToChar( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) ), f );

	if ( cl->accuracy_shots ) {
		writeToFile ( va ( "\t\t\t<stat name=\"Accuracy\" value=\"%i\"/>\n", cl->accuracy_hits * 100 / cl->accuracy_shots ), f );
	}
	
	if( ( g_gametype.integer == GT_TOURNAMENT ) && g_autoServerDemos.integer ) {
		trap_Cvar_VariableStringBuffer(va("demopath%i", cl->ps.clientNum), demopath, sizeof(demopath));
		if( Q_stricmp(demopath, "" ) )
			writeToFile ( va ( "\t\t\t<stat name=\"Demopath\" value=\"%s.dm_71\"/>\n", demopath ), f );
	}

	count = cl->accuracy[WP_GAUNTLET][2] + cl->accuracy[WP_MACHINEGUN][0] + cl->accuracy[WP_SHOTGUN][0]
	        + cl->accuracy[WP_GRENADE_LAUNCHER][0]  + cl->accuracy[WP_ROCKET_LAUNCHER][0]  + cl->accuracy[WP_LIGHTNING][0]
	        + cl->accuracy[WP_RAILGUN][0]  + cl->accuracy[WP_PLASMAGUN][0]  + cl->accuracy[WP_BFG][0];

	if ( count != 0 ) {
		writeToFile ( "\t\t\t<weapons>\n", f );
		if ( cl->accuracy[WP_GAUNTLET][2] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"GAUNT\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_GAUNTLET][2], cl->accuracy[WP_GAUNTLET][3], cl->accuracy[WP_GAUNTLET][4] ), f );
		if ( cl->accuracy[WP_MACHINEGUN][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"MG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_MACHINEGUN][0], cl->accuracy[WP_MACHINEGUN][1], cl->accuracy[WP_MACHINEGUN][2], cl->accuracy[WP_MACHINEGUN][3], cl->accuracy[WP_MACHINEGUN][4] ), f );
		if ( cl->accuracy[WP_SHOTGUN][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"SG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_SHOTGUN][0], cl->accuracy[WP_SHOTGUN][1], cl->accuracy[WP_SHOTGUN][2], cl->accuracy[WP_SHOTGUN][3], cl->accuracy[WP_SHOTGUN][4] ), f );
		if ( cl->accuracy[WP_GRENADE_LAUNCHER][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"GL\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_GRENADE_LAUNCHER][0], cl->accuracy[WP_GRENADE_LAUNCHER][1], cl->accuracy[WP_GRENADE_LAUNCHER][2], cl->accuracy[WP_GRENADE_LAUNCHER][3], cl->accuracy[WP_GRENADE_LAUNCHER][4] ), f );
		if ( cl->accuracy[WP_ROCKET_LAUNCHER][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"RL\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_ROCKET_LAUNCHER][0], cl->accuracy[WP_ROCKET_LAUNCHER][1], cl->accuracy[WP_ROCKET_LAUNCHER][2], cl->accuracy[WP_ROCKET_LAUNCHER][3], cl->accuracy[WP_ROCKET_LAUNCHER][4] ), f );
		if ( cl->accuracy[WP_LIGHTNING][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"LG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_LIGHTNING][0], cl->accuracy[WP_LIGHTNING][1], cl->accuracy[WP_LIGHTNING][2], cl->accuracy[WP_LIGHTNING][3], cl->accuracy[WP_LIGHTNING][4] ), f );
		if ( cl->accuracy[WP_RAILGUN][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"RG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_RAILGUN][0], cl->accuracy[WP_RAILGUN][1], cl->accuracy[WP_RAILGUN][2], cl->accuracy[WP_RAILGUN][3], cl->accuracy[WP_RAILGUN][4] ), f );
		if ( cl->accuracy[WP_PLASMAGUN][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"PG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_PLASMAGUN][0], cl->accuracy[WP_PLASMAGUN][1], cl->accuracy[WP_PLASMAGUN][2], cl->accuracy[WP_PLASMAGUN][3], cl->accuracy[WP_PLASMAGUN][4] ), f );
		if ( cl->accuracy[WP_BFG][0] )
			writeToFile ( va ( "\t\t\t\t<weapon name=\"BFG\" shots=\"%i\" hits=\"%i\" damage=\"%i\" kills=\"%i\" death=\"%i\"/>\n", cl->accuracy[WP_BFG][0], cl->accuracy[WP_BFG][1], cl->accuracy[WP_BFG][2], cl->accuracy[WP_BFG][3], cl->accuracy[WP_BFG][4] ), f );
		writeToFile ( "\t\t\t</weapons>\n", f );
	}

	count = cl->stats[STATS_MH] + cl->stats[STATS_RA] + cl->stats[STATS_YA];

	if ( count ) {
		writeToFile ( "\t\t\t<items>\n", f );
		if ( cl->stats[STATS_MH] )
			writeToFile ( va ( "\t\t\t\t<item name=\"MH\" pickups=\"%i\"/>\n", cl->stats[STATS_MH] ), f );
		if ( cl->stats[STATS_RA] )
			writeToFile ( va ( "\t\t\t\t<item name=\"RA\" pickups=\"%i\"/>\n", cl->stats[STATS_RA] ), f );
		if ( cl->stats[STATS_YA] )
			writeToFile ( va ( "\t\t\t\t<item name=\"YA\" pickups=\"%i\"/>\n", cl->stats[STATS_YA] ), f );
		writeToFile ( "\t\t\t</items>\n", f );
	}

	count = cl->ps.persistant[PERS_ASSIST_COUNT] + cl->ps.persistant[PERS_DEFEND_COUNT] + cl->ps.persistant[PERS_CAPTURES] + cl->ps.persistant[PERS_IMPRESSIVE_COUNT]
	        + cl->ps.persistant[PERS_EXCELLENT_COUNT] + cl->rewards[REWARD_AIRROCKET] + cl->rewards[REWARD_AIRGRENADE] + cl->rewards[REWARD_FULLSG] + cl->rewards[REWARD_RLRG] + cl->rewards[REWARD_ITEMDENIED];

	if ( count ) {
		writeToFile ( "\t\t\t<rewards>\n", f );
		if ( cl->ps.persistant[PERS_ASSIST_COUNT] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Assist\" value=\"%i\"/>\n", cl->ps.persistant[PERS_ASSIST_COUNT] ), f );
		if ( cl->ps.persistant[PERS_DEFEND_COUNT] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Defense\" value=\"%i\"/>\n", cl->ps.persistant[PERS_DEFEND_COUNT] ), f );
		if ( cl->ps.persistant[PERS_CAPTURES] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Capture\" value=\"%i\"/>\n", cl->ps.persistant[PERS_CAPTURES] ), f );
		if ( cl->ps.persistant[PERS_IMPRESSIVE_COUNT] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Impressive\" value=\"%i\"/>\n", cl->ps.persistant[PERS_IMPRESSIVE_COUNT] ), f );
		if ( cl->ps.persistant[PERS_EXCELLENT_COUNT] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Excellent\" value=\"%i\"/>\n", cl->ps.persistant[PERS_EXCELLENT_COUNT] ), f );
		if ( cl->rewards[REWARD_AIRROCKET] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Airrocket\" value=\"%i\"/>\n", cl->rewards[REWARD_AIRROCKET] ), f );
		if ( cl->rewards[REWARD_DOUBLE_AIRROCKET] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"DoubleAirrocket\" value=\"%i\"/>\n", cl->rewards[REWARD_DOUBLE_AIRROCKET] ), f );
		if ( cl->rewards[REWARD_AIRGRENADE] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Airgrenade\" value=\"%i\"/>\n", cl->rewards[REWARD_AIRGRENADE] ), f );
		if ( cl->rewards[REWARD_FULLSG] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"FullSG\" value=\"%i\"/>\n", cl->rewards[REWARD_FULLSG] ), f );
		if ( cl->rewards[REWARD_RLRG] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Rocket2Rail\" value=\"%i\"/>\n", cl->rewards[REWARD_RLRG] ), f );
		if ( cl->rewards[REWARD_ITEMDENIED] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Itemdenied\" value=\"%i\"/>\n", cl->rewards[REWARD_ITEMDENIED] ), f );
		if ( cl->rewards[REWARD_SPAWNKILL] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"Spawnkill\" value=\"%i\"/>\n", cl->rewards[REWARD_SPAWNKILL] ), f );
		if ( cl->rewards[REWARD_LGACCURACY] )
			writeToFile ( va ( "\t\t\t\t<reward name=\"LightningAccuracy\" value=\"%i\"/>\n", cl->rewards[REWARD_LGACCURACY] ), f );
		writeToFile ( "\t\t\t</rewards>\n", f );
	}
	
	count = cl->stats[STATS_QUAD];
	
	if( count ) {
		writeToFile ( "\t\t\t<powerups>\n", f );
		if( cl->stats[STATS_QUAD] )
			writeToFile ( va ( "\t\t\t\t<powerup name=\"QuadDamage\" pickups=\"%i\" kills=\"%i\" streak=\"%i\"/>\n", cl->stats[STATS_QUAD], cl->stats[STATS_QUADKILLS], cl->stats[STATS_QUADSTREAK]  ), f );
		writeToFile ( "\t\t\t</powerups>\n", f );
	  
	}
	
	if( ( g_gametype.integer == GT_CTF || g_gametype.integer == GT_CTF_ELIMINATION ) && cl->captureCount ){
		writeToFile( "\t\t\t<captures>\n", f );
		for( i=0; i<cl->captureCount; i++ ){
			writeToFile( va( "\t\t\t\t<capture team=\"%s\" perfect=\"%s\" duration=\"%i\" gametime=\"%i\"/>\n", cl->captures[ i ].team == TEAM_RED? "Red" : "Blue", boolToChar( cl->captures[ i ].perfect ), cl->captures[ i ].duration, cl->captures[ i ].gametime ), f );
		}
		writeToFile( "\t\t\t</captures>\n", f );
	}
	writeToFile ( "\t\t</player>\n", f );
}



void G_WriteXMLStats ( void ) {
	int		i, numStats;
	fileHandle_t f;
	char		mapname[MAX_MAPNAME];
	qboolean isTeamGame = g_gametype.integer >= GT_TEAM;
	qboolean spectatorInGame = qfalse;
	qtime_t	now;
	static char gameString[128];

	trap_RealTime ( &now );

	//G_SetGameString();
	
	trap_Cvar_VariableStringBuffer("gamestring", gameString, sizeof(gameString));

	numStats = level.numConnectedClients;

	for ( i = 0; i < numStats; i++ ) {
		if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_SPECTATOR ) {
			spectatorInGame = qtrue;
			break;
		}
	}

	trap_Cvar_VariableStringBuffer ( "mapname", mapname, sizeof ( mapname ) );

	trap_FS_FOpenFile ( va ( "%s/%s.xml", g_statsPath.string,gameString ), &f, FS_WRITE );

	writeToFile ( "<?xml version=\"1.0\"?><?xml-stylesheet type=\"text/xsl\"?>\n", &f );
	writeToFile ( va ( "<match datetime=\"%i/%02i/%02i %02i:%02i:%02i\" duration=\"%i\" map=\"%s\" type=\"%s\" isTeamGame=\"%s\" instagib=\"%i\" rocketsOnly=\"%s\" reducedLightning=\"%s\" reducedRail=\"%s\" aftershockRevision=\"%i\" g_aftershockPhysic=\"%i\" serverdemo=\"%s\">\n\n",
	                   1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, level.time-level.startTime, mapname, gameShortNames[g_gametype.integer], boolToChar ( isTeamGame ),
	                   g_instantgib.integer, boolToChar ( g_rockets.integer == 1 ), boolToChar( g_reduceLightningDamage.integer == 1 ), boolToChar( g_reduceRailDamage.integer == 1 ), REVISION, g_aftershockPhysic.integer, boolToChar( ( g_gametype.integer == GT_TOURNAMENT ) && g_autoServerDemos.integer ) ), &f );

	if ( isTeamGame ) {
		writeToFile ( va ( "\t<team name=\"Blue\" score=\"%i\">\n", level.teamScores[TEAM_BLUE] ), &f );
		for ( i = 0; i < numStats; i++ ) {
			if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_BLUE )
				writePlayerData ( &level.clients[level.sortedClients[i]], &f, qfalse );
		}
		writeToFile ( "\t</team>\n",&f );
		writeToFile ( va ( "\t<team name=\"Red\" score=\"%i\">\n", level.teamScores[TEAM_RED] ), &f );
		for ( i = 0; i < numStats; i++ ) {
			if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_RED )
				writePlayerData ( &level.clients[level.sortedClients[i]], &f, qfalse );
		}
		writeToFile ( "\t</team>\n",&f );

		if ( spectatorInGame ) {
			writeToFile ( "\t<team name=\"Spectator\">\n", &f );
			for ( i = 0; i < numStats; i++ ) {
				if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_SPECTATOR )
					writePlayerData ( &level.clients[level.sortedClients[i]], &f, qfalse );
			}
			writeToFile ( "\t</team>\n",&f );
		}

		if ( level.disconnectedClientsNumber ) {
			writeToFile ( "\t<team name=\"Disconnected\">\n", &f );
			for ( i = 0; i < level.disconnectedClientsNumber; i++ ) {
				writePlayerData ( &level.disconnectedClients[i], &f, qtrue );
			}
			writeToFile ( "\t</team>\n",&f );
		}
	} else {
		writeToFile ( "\t<team name=\"Free\">\n", &f );
		for ( i = 0; i < numStats; i++ ) {
			if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_FREE )
				writePlayerData ( &level.clients[level.sortedClients[i]], &f, qfalse );
		}
		writeToFile ( "\t</team>\n",&f );

		if ( spectatorInGame ) {
			writeToFile ( "\t<team name=\"Spectator\">\n", &f );
			for ( i = 0; i < numStats; i++ ) {
				if ( level.clients[level.sortedClients[i]].sess.sessionTeam == TEAM_SPECTATOR )
					writePlayerData ( &level.clients[level.sortedClients[i]], &f, qfalse );
			}
			writeToFile ( "\t</team>\n",&f );
		}

		if ( level.disconnectedClientsNumber ) {
			writeToFile ( "\t<team name=\"Disconnected\">\n", &f );
			for ( i = 0; i < level.disconnectedClientsNumber; i++ ) {
				writePlayerData ( &level.disconnectedClients[i], &f, qtrue );
			}
			writeToFile ( "\t</team>\n",&f );
		}
	}

	writeToFile ( "</match>\n",&f );

	trap_FS_FCloseFile ( f );
	return;
}
