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

// cg_scoreboard -- draw aftershock scoreboard on top of the game screen

#include "cg_local.h"

#define SB_MAXDISPLAY   		7
#define SB_INFOICON_SIZE		8

#define SB_CHAR_WIDTH			4
#define SB_CHAR_HEIGHT			8
#define SB_MEDCHAR_WIDTH 		6
#define SB_MEDCHAR_HEIGHT		10

#define SB_WIDTH				640
#define SB_HEIGHT				410
#define SB_XPOS 				( ( SCREEN_WIDTH - SB_WIDTH )/2 )
#define SB_YPOS					( ( SCREEN_HEIGHT - SB_HEIGHT )/2 )

#define SB_TEAM_WIDTH			( SB_WIDTH/2 - 40 )
#define SB_TEAM_HEIGHT			20
#define SB_TEAM_RED_X			( SCREEN_WIDTH/2 - SB_TEAM_WIDTH - 20 )
#define SB_TEAM_BLUE_X			( SCREEN_WIDTH/2 + 20 )
#define SB_TEAM_Y				60

#define SB_INFO_WIDTH			( SB_WIDTH - 150 )
#define SB_INFO_HEIGHT			20
#define SB_INFO_X				( SCREEN_WIDTH/2 )
#define SB_INFO_Y				335
#define SB_INFO_MAXNUM			13

#define SB_FFA_WIDTH			( SB_WIDTH - 190 )
#define SB_FFA_HEIGHT			20
#define SB_FFA_X				( SCREEN_WIDTH/2 - SB_FFA_WIDTH/2 )
#define SB_FFA_Y				75

#define SB_SPEC_WIDTH			( SB_WIDTH - 340 )
#define SB_SPEC_X				( SCREEN_WIDTH/2 - SB_SPEC_WIDTH/2 )
#define SB_SPEC_Y				370
#define SB_SPEC_MAXCHAR			( SB_SPEC_WIDTH/SB_MEDCHAR_WIDTH )

#define SB_TOURNEY_WIDTH		( SB_WIDTH/2 - 40 )
#define SB_TOURNEY_HEIGHT		16
#define SB_TOURNEY_X			( SCREEN_WIDTH/2 )
#define SB_TOURNEY_Y			60
#define SB_TOURNEY_PICKUP_Y		200
#define SB_TOURNEY_AWARD_Y		340
#define SB_TOURNEY_MAX_AWARDS	8
#define SB_TOURNEY_AWARDS		4

typedef struct{
	qhandle_t pic;
	int val;
	qboolean percent;
} picBar_t;

static void CG_DrawClientScore( int x, int y, int w, int h, score_t *score, float *color ){
	
	clientInfo_t *ci;
	char string[ 128 ];
	int picSize;
	
	if( score->client < 0 || score->client >= cgs.maxclients ){
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}
	
	// don't draw the client while he's connecting
	if( score->ping == -1 )return;
	
	ci = &cgs.clientinfo[ score->client ];
	
	CG_FillRect( x, y, w, h, color );
	
	y += h/2;
	
	CG_DrawStringExt( x, y - SB_MEDCHAR_HEIGHT/2, ci->name, colorWhite, qfalse, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 31 );
	CG_DrawStringExt( x + w*0.7, y - SB_MEDCHAR_HEIGHT/2, va( "%i", score->score ), colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
	CG_DrawStringExt( x + w*0.8, y - SB_MEDCHAR_HEIGHT/2, va( "%i", score->ping ), colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 3 );
	CG_DrawStringExt( x + w*0.88, y - SB_MEDCHAR_HEIGHT/2, va( "%i", score->time ), colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 3 );
	
	if( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ){
		if( h >= SB_CHAR_HEIGHT*2 ){
			CG_DrawStringExt( x + w*0.96, y - SB_CHAR_HEIGHT, va( "%i", ( int )( score->dmgdone/100 ) ), colorGreen, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
			CG_DrawStringExt( x + w*0.96, y, va( "%i", ( int )( score->dmgtaken/100 ) ), colorRed, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
		}else{
			strcpy( string, va( "^2%i^7/^1%i", ( int )( score->dmgdone/100 ), ( int )( score->dmgtaken/100 ) ) );
			CG_DrawStringExt( x + w*0.96, y - SB_CHAR_HEIGHT/2, string, colorWhite, qfalse, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
		}
	}/*else if( cgs.gametype == GT_TOURNAMENT ){
		if( h >= SB_CHAR_HEIGHT*2 ){
			CG_DrawStringExt( x + w*0.96, y - SB_CHAR_HEIGHT, va( "%i", ( int )( ci->wins/100 ) ), colorGreen, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
			CG_DrawStringExt( x + w*0.96, y, va( "%i", ( int )( ci->losses/100 ) ), colorRed, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
		}else{
			strcpy( string, va( "^2%i^7/^1%i", ( int )( ci->wins/100 ), ( int )( ci->losses/100 ) ) );
			CG_DrawStringExt( x + w*0.96, y - SB_CHAR_HEIGHT/2, string, colorWhite, qfalse, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
		}
	}*/
	
	picSize = h*0.8;
	
	if( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << score->client ) ){
		// ready during intermission
		CG_DrawPic( x - picSize, y - picSize/2, picSize, picSize, cgs.media.sbReady );
	}else if( ci->isDead ){
		CG_DrawPic( x - picSize, y - picSize/2, picSize, picSize, cgs.media.sbSkull );
	}else if( ci->powerups & ( 1 << PW_REDFLAG ) ){
		CG_DrawFlagModel( x - picSize, y - picSize/2, picSize, picSize, TEAM_RED, qfalse );
	}else if( ci->powerups & ( 1 << PW_BLUEFLAG ) ){
		CG_DrawFlagModel( x - picSize, y - picSize/2, picSize, picSize, TEAM_BLUE, qfalse );
	}
	
	if( cg.warmup < 0 && ci->team != TEAM_SPECTATOR && cgs.startWhenReady ){
		if( cg.readyMask & ( 1 << score->client ) ){
			CG_DrawPic( x - picSize - 4, y - picSize/2, picSize, picSize, cgs.media.sbReady );
		}else{
			CG_DrawPic( x - picSize - 4, y - picSize/2, picSize, picSize, cgs.media.sbNotReady );
		}
	}
}

static int CG_TeamScoreboard( int x, int y, int w, int h, team_t team, float *color, int maxClients ){
	
	int i;
	score_t	*score;
	clientInfo_t *ci;
	int count;
	float transparent[ 4 ];
	qboolean notEnough;
	score_t *localClient;
	
	transparent[ 0 ] = 0;
	transparent[ 1 ] = 0;
	transparent[ 2 ] = 0;
	transparent[ 3 ] = 0;
	
	CG_DrawStringExt( x, y, "Name", colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
	CG_DrawStringExt( x + w*0.7, y, "Score", colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
	CG_DrawPic( x + w*0.8, y, SB_INFOICON_SIZE, SB_INFOICON_SIZE, cgs.media.sbPing );
	CG_DrawPic( x + w*0.88, y, SB_INFOICON_SIZE, SB_INFOICON_SIZE, cgs.media.sbClock );
	
	if( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ){
		CG_DrawStringExt( x + w*0.96, y, "Dmg", colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
	}/*else if( cgs.gametype == GT_TOURNAMENT ){
		CG_DrawStringExt( x + w*0.96, y, "W/L", colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
	}*/
	
	y += 20;
	
	count = 0;
	notEnough = qfalse;
	localClient = NULL;
	for( i=0; i<cg.numScores; i++ ){
		
		score = &cg.scores[ i ];
		ci = &cgs.clientinfo[ score->client ];
		
		if( ci->team != team ){
			continue;
		}
		
		if( count >= maxClients ){
		
			notEnough = qtrue;
			// we don't break the loop, so we can get the local client, and display it later
			if( score->client == cg.clientNum )localClient = score;
			
		}else{
			
			if( count%2 ){
				CG_DrawClientScore( x, y, w, h, score, transparent );
			}else{
				CG_DrawClientScore( x, y, w, h, score, color );
			}
			
			y += h + 1;
			count++;
		}
	}
	
	if( notEnough ){
	
		CG_DrawStringExt( x, y, "...", colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
		
		// draw the local client if he hasn't been drawn yet
		if( localClient != NULL ){
			y += h - 5;
			if( count%2 ){
				CG_DrawClientScore( x, y, w, h, localClient, transparent );
			}else{
				CG_DrawClientScore( x, y, w, h, localClient, color );
			}
			count++;
		}
	}
	
	return count;
	
}

static void CG_DrawSpecs( void ){
	
	int numLine;
	char string[ 128 ];
	int y;
	int i;
	score_t	*score;
	clientInfo_t *ci;
	qboolean notEnough;
	score_t *localClient;
	int queueNumber=1;
	
	strcpy( string, "Spectators" );
	CG_DrawStringExt( SB_SPEC_X + SB_SPEC_WIDTH/2 - SMALLCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_SPEC_Y, string, colorYellow, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
	
	y = SB_SPEC_Y + 20;
	strcpy( string, "" );
	numLine = 0;
	notEnough = qfalse;
	localClient = NULL;
	
	for( i=0; i<cg.numScores; i++ ){
		
		score = &cg.scores[ i ];
		ci = &cgs.clientinfo[ score->client ];
		
		if( ci->team != TEAM_SPECTATOR ){
			continue;
		}
		
		if( notEnough ){
		
			if( score->client == cg.clientNum )localClient = score;
			
		}else{
		
			if( CG_DrawStrlen( string ) + CG_DrawStrlen( ci->name ) + 3 > SB_SPEC_MAXCHAR && strlen( string ) ){
				CG_DrawStringExt( SB_SPEC_X, y, string, colorWhite, qfalse, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
				if( cgs.gametype == GT_TOURNAMENT ){
					if( score->specOnly )
						strcpy( string, va("^7(^2%i^7/^1%i^7)%s^7(^1s^7)", ci->wins, ci->losses, ci->name ) );
					else{
						strcpy( string, va("^7(^2%i^7/^1%i^7)%s^7(^2%i^7)", ci->wins, ci->losses, ci->name, queueNumber ) );
						queueNumber++;
					}
					
				}
				else
					strcpy( string, ci->name );
				
				y += SB_MEDCHAR_HEIGHT;
				numLine++;
				if( numLine >= 2 ){
					notEnough = qtrue;
					if( score->client == cg.clientNum )localClient = score;
				}
			}else{
				if( strlen( string ) )
					strcat( string, "   " );
				
				if( cgs.gametype == GT_TOURNAMENT ){
					if( score->specOnly )
						strcat( string, va("^7(^2%i^7/^1%i^7)%s^7(^1s^7)", ci->wins, ci->losses, ci->name ) );
					else{
						strcat( string, va("^7(^2%i^7/^1%i^7)%s^7(^2%i^7)", ci->wins, ci->losses, ci->name, queueNumber ) );
						queueNumber++;
					}
				}
				else
					strcat( string, ci->name );
			}
			
		}
	}
	
	if( notEnough ){
		
		strcpy( string, "... " );
		if( localClient != NULL )strcat( string, cgs.clientinfo[ score->client ].name ); // draw the local client if he hasn't been drawn yet
		CG_DrawStringExt( SB_SPEC_X, y, string, colorWhite, qfalse, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
		
	}else if( strlen( string ) ){
	
		CG_DrawStringExt( SB_SPEC_X, y, string, colorWhite, qfalse, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
	
	}
}

static void CG_DrawPicBar( picBar_t *tab, int count, int x, int y, int w, int h ){
	
	int i, offset, picBarX;
	char string[ 16 ];
	
	offset = w/( count*2 + 1 );
	for( i=0; i<count; i++ ){
		picBarX = x - w/2 + offset + i*2*offset;
		CG_DrawPic( picBarX, y - h/2, h, h, tab[ i ].pic );
		strcpy( string, va( "%i", tab[ i ].val ) );
		if( tab[ i ].percent )strcat( string, "%" );
		CG_DrawStringExt( picBarX + h + 3, y - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
	}
	
}

static void CG_DrawMapInfo( void ){
	
	char string[ 128 ];
	char *buf;
	
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
	
	strcpy( string, strchr( cgs.mapname, '/' ) + 1 );
	buf = strstr( string, ".bsp");
	buf[ 0 ] = '\0';
	strcat( string, " // " );
	strcat( string, gameNames[ cgs.gametype ] );
	CG_DrawStringExt( SB_XPOS + 10, SB_YPOS + 2, string, colorWhite, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
	
}

qboolean CG_DrawOldScoreboard( void ){
	
	vec4_t color;
	char string[ 128 ];
	score_t *score;
	picBar_t picBar[ SB_INFO_MAXNUM ];
	int infoCount;
	int i;
	
	// don't draw anything if the menu or console is up
	if( cg_paused.integer ){
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}
	
	if( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION ){
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}
	
	// don't draw scoreboard during death while warmup up
	if( cg.warmup && !cg.showScores ){
		return qfalse;
	}
	
	// TODO : most of this block seems useless, except the part with the return qfalse. clean up needed
	if( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION ){
		// fade = 1.0;
		// fadeColor = colorWhite;
	}else{
		float *fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		if( !fadeColor ){
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[ 0 ] = 0;
			return qfalse;
		}
	}
	
	CG_DrawPic( SB_XPOS, SB_YPOS, SB_WIDTH, SB_HEIGHT, cgs.media.sbBackground );
	
	CG_DrawMapInfo();
	
	if( cgs.gametype >= GT_TEAM && cgs.ffa_gt != 1 ){
		
		// scores
		strcpy( string, va( "^1%i ^4%i", cg.teamScores[ 0 ], cg.teamScores[ 1 ] ) );
		CG_DrawStringExt( SCREEN_WIDTH/2 - GIANTCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_TEAM_Y, string, colorWhite, qfalse, qtrue, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
		strcpy( string, "to" );
		CG_DrawStringExt( SCREEN_WIDTH/2 - SMALLCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_TEAM_Y + 25, string, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
		
		// team red
		strcpy( string, "Team red" );
		CG_DrawStringExt( SB_TEAM_RED_X + SB_TEAM_WIDTH/2 - BIGCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_TEAM_Y + 15, string, colorRed, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
		color[ 0 ] = 1;
		color[ 1 ] = 0;
		color[ 2 ] = 0;
		color[ 3 ] = 0.25;
		CG_TeamScoreboard( SB_TEAM_RED_X, SB_TEAM_Y + 65, SB_TEAM_WIDTH, SB_TEAM_HEIGHT, TEAM_RED, color, SB_MAXDISPLAY );
		if( cgs.redLocked )
		  CG_DrawPic( 0, SB_TEAM_Y, 32, 32, cgs.media.sbLocked );
		
		// team blue
		strcpy( string, "Team blue" );
		CG_DrawStringExt( SB_TEAM_BLUE_X + SB_TEAM_WIDTH/2 - BIGCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_TEAM_Y + 15, string, colorBlue, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
		color[ 0 ] = 0;
		color[ 1 ] = 0;
		color[ 2 ] = 1;
		color[ 3 ] = 0.25;
		CG_TeamScoreboard( SB_TEAM_BLUE_X, SB_TEAM_Y + 65, SB_TEAM_WIDTH, SB_TEAM_HEIGHT, TEAM_BLUE, color, SB_MAXDISPLAY );
		if( cgs.blueLocked )
		  CG_DrawPic( 640-32, SB_TEAM_Y, 32, 32, cgs.media.sbLocked );
	}else{
		
		// current rank
		if( cg.snap->ps.persistant[ PERS_TEAM ] != TEAM_SPECTATOR ){
			strcpy( string, va( "%s place with %i", CG_PlaceString( cg.snap->ps.persistant[ PERS_RANK ] + 1 ), cg.snap->ps.persistant[ PERS_SCORE ] ) );
			CG_DrawStringExt( SCREEN_WIDTH/2 - BIGCHAR_WIDTH*CG_DrawStrlen( string )/2, SB_FFA_Y, string, colorWhite, qfalse, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
		}
		
		// one team scoreboard
		color[ 0 ] = 0.7;
		color[ 1 ] = 0.7;
		color[ 2 ] = 0.7;
		color[ 3 ] = 0.25;
		CG_TeamScoreboard( SB_FFA_X, SB_FFA_Y + 50, SB_FFA_WIDTH, SB_FFA_HEIGHT, TEAM_FREE, color, SB_MAXDISPLAY );
		
	}
	
	// informations about the local/specced player (acc + awards)
	score = NULL;
	for( i=0; i<cg.numScores; i++ ){
		if( cg.scores[ i ].client == cg.snap->ps.clientNum )score = &cg.scores[ i ];
	}
	
	if( score != NULL ){
		infoCount = 0;
		picBar[ infoCount ].pic = cgs.media.medalGauntlet;
		picBar[ infoCount ].val = score->accuracy;
		picBar[ infoCount ].percent = qtrue;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalExcellent;
		picBar[ infoCount ].val = score->excellentCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalImpressive;
		picBar[ infoCount ].val = score->impressiveCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalGauntlet;
		picBar[ infoCount ].val = score->guantletCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalAirrocket;
		picBar[ infoCount ].val = score->airrocketCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalAirgrenade;
		picBar[ infoCount ].val = score->airgrenadeCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalFullsg;
		picBar[ infoCount ].val = score->fullshotgunCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		picBar[ infoCount ].pic = cgs.media.medalRocketrail;
		picBar[ infoCount ].val = score->rocketRailCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		if( !cgs.nopickup && cgs.gametype != GT_ELIMINATION && cgs.gametype != GT_CTF_ELIMINATION ){
			picBar[ infoCount ].pic = cgs.media.medalItemdenied;
			picBar[ infoCount ].val = score->itemDeniedCount;
			picBar[ infoCount ].percent = qfalse;
			infoCount++;
		}
		if( cgs.gametype == GT_CTF ){
			picBar[ infoCount ].pic = cgs.media.medalCapture;
			picBar[ infoCount ].val = score->captures;
			picBar[ infoCount ].percent = qfalse;
			infoCount++;
			picBar[ infoCount ].pic = cgs.media.medalDefend;
			picBar[ infoCount ].val = score->defendCount;
			picBar[ infoCount ].percent = qfalse;
			infoCount++;
			picBar[ infoCount ].pic = cgs.media.medalAssist;
			picBar[ infoCount ].val = score->assistCount;
			picBar[ infoCount ].percent = qfalse;
			infoCount++;
		}
		picBar[ infoCount ].pic = cgs.media.medalSpawnkill;
		picBar[ infoCount ].val = score->spawnkillCount;
		picBar[ infoCount ].percent = qfalse;
		infoCount++;
		CG_DrawPicBar( picBar, infoCount, SB_INFO_X, SB_INFO_Y, SB_INFO_WIDTH, SB_INFO_HEIGHT );
	}
	
	// spectators
	CG_DrawSpecs();
	
	// load any models that have been deferred
	if( ++cg.deferredPlayerLoading > 10 ){
		CG_LoadDeferredPlayers();
	}
	
	return qtrue;
	
}


qboolean CG_DrawOldTourneyScoreboard( void ){
	
	char string[ 128 ];
	int x, y, w, h;
	int i, j, k;
	int side, loop, offset;
	clientInfo_t *p1;
	score_t *p1Score;
	clientInfo_t *p2;
	score_t *p2Score;
	clientInfo_t *p;
	score_t *score;
	gitem_t *item;
	picBar_t picBar[ SB_TOURNEY_MAX_AWARDS ];
	picBar_t sortedPicBar[ SB_TOURNEY_AWARDS ];
	int awardCount;
	
	// don't draw anything if the menu or console is up
	if( cg_paused.integer ){
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}
	
	// don't draw scoreboard during death while warmup up
	if( cg.warmup && !cg.showScores ){
		return qfalse;
	}
	
	// TODO : most of this block seems useless, except the part with the return qfalse. clean up needed
	if( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION ){
		// fade = 1.0;
		// fadeColor = colorWhite;
	}else{
		float *fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );
		if( !fadeColor ){
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[ 0 ] = 0;
			return qfalse;
		}
	}
	
	CG_DrawPic( SB_XPOS, SB_YPOS, SB_WIDTH, SB_HEIGHT, cgs.media.sbBackground );
	
	CG_DrawMapInfo();
	
	// get the two active players
	p1 = NULL;
	p2 = NULL;
	loop = -1; // loop is used when displaying the stats. -1 : no player, 0 : left player only, 2 : both players
	
	for( i=0; i<cg.numScores; i++ ){
		if( cgs.clientinfo[ cg.scores[ i ].client ].team != TEAM_FREE )continue;
		if( p1 == NULL ){
			loop = 0;
			p1 = &cgs.clientinfo[ cg.scores[ i ].client ];
			p1Score = &cg.scores[ i ];
		}else{
			// draw the other player only if match is over or we're spectating
			if( cg.predictedPlayerState.pm_type == PM_INTERMISSION || cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR ){
				loop = 2;
			}
			if( cg.scores[ i ].client == cg.clientNum ){
				// local player always on the left side
				p2 = p1;
				p2Score = p1Score;
				p1 = &cgs.clientinfo[ cg.scores[ i ].client ];
				p1Score = &cg.scores[ i ];
			}else{
				p2 = &cgs.clientinfo[ cg.scores[ i ].client ];
				p2Score = &cg.scores[ i ];
			}
		}
	}
	
	// scores
	x = SB_TOURNEY_X;
	y = SB_TOURNEY_Y;
	w = SB_TOURNEY_WIDTH;
	h = SB_TOURNEY_HEIGHT;
	
	if( p1 != NULL && p2 != NULL ){
		strcpy( string, va( "%i %i", p1Score->score, p2Score->score ) );
		CG_DrawStringExt( x - GIANTCHAR_WIDTH*CG_DrawStrlen( string )/2, y, string, colorWhite, qtrue, qtrue, GIANTCHAR_WIDTH, GIANTCHAR_HEIGHT, 0 );
		strcpy( string, "to" );
		CG_DrawStringExt( x - SMALLCHAR_WIDTH*CG_DrawStrlen( string )/2, y + 25, string, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
	}
	
	// players headers
	// we don't use the loop var here, because we always display headers
	for( side=-1; side<2; side+=2 ){
		
		offset = side < 0? 0 : 1;
		p = side < 0? p1 : p2;
		score = side < 0? p1Score : p2Score;
		
		if( p == NULL )break;
	
		CG_DrawStringExt( x + side*w/2 - SMALLCHAR_WIDTH*CG_DrawStrlen( va("^7(^2%i^7/^1%i^7) %s",p->wins, p->losses, p->name ) )/2, y + 15, va("^7(^2%i^7/^1%i^7) %s",p->wins, p->losses, p->name ), colorWhite, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0 );
		CG_DrawPic( x + side*w*0.8 - offset*SB_INFOICON_SIZE, y + 30, SB_INFOICON_SIZE, SB_INFOICON_SIZE, cgs.media.sbPing );
		strcpy( string, va( "%i", score->ping ) );
		CG_DrawStringExt( x + side*w*0.8 - side*SB_INFOICON_SIZE*2 - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + 30, string, colorWhite, qfalse, qtrue, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
		
		if( cg.warmup < 0 && cgs.startWhenReady ){
			if( cg.readyMask & ( 1 << score->client ) ){
				strcpy( string, "^2Ready" );
				CG_DrawStringExt( x + side*w/2 - SB_MEDCHAR_WIDTH*CG_DrawStrlen( string )/2, y + 30, string, colorWhite, qfalse, qtrue, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
			}else{
				strcpy( string, "^1Not ready" );
				CG_DrawStringExt( x + side*w/2 - SB_MEDCHAR_WIDTH*CG_DrawStrlen( string )/2, y + 30, string, colorWhite, qfalse, qtrue, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
			}
		}
	}
	
	// TODO : highlight best values
	
	// weapons info
	y += 60;
	for( side=-1; side<loop; side+=2 ){
		strcpy( string, "Acc." );
		CG_DrawStringExt( x + side*0.3*w - SB_CHAR_WIDTH*CG_DrawStrlen( string )/2, y, string, colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
		strcpy( string, "Hit/Fired" );
		CG_DrawStringExt( x + side*0.6*w - SB_CHAR_WIDTH*CG_DrawStrlen( string )/2, y, string, colorWhite, qtrue, qfalse, SB_CHAR_WIDTH, SB_CHAR_HEIGHT, 0 );
	}
	
	y += 20;
	for( i=0; i<8; i++ ){
		
		if( cg_weapons[ i + 2 ].registered ){
			
			CG_DrawPic( x - h/2, y - h/2, h, h, cg_weapons[ i + 2 ].weaponIcon );
			
			for( side=-1; side<loop; side+=2 ){
				score = side < 0? p1Score : p2Score;
				// accuracy
				//CLOWN INSERT
				if( score->accuracys[ i ][ 0 ] > 0 ){
					strcpy( string, va( "%i%%", ( ( int )( 100*( float )score->accuracys[ i ][ 1 ]/( float )score->accuracys[ i ][ 0 ] ) ) ) );
				}else{
					strcpy( string, "-%" );
				}
				CG_DrawStringExt( x + side*0.3*w - SB_MEDCHAR_WIDTH*CG_DrawStrlen( string )/2, y, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
				
				// hit/fired
				//CLOWN INSERT
				if( score->accuracys[ i ][ 0 ] > 0 ){
					strcpy( string, va( "%i/%i", score->accuracys[ i ][ 1 ], score->accuracys[ i ][ 0 ] ) );
				}else{
					strcpy( string, "-/-" );
				}
				CG_DrawStringExt( x + side*0.6*w - SB_MEDCHAR_WIDTH*CG_DrawStrlen( string )/2, y, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
				
		}
			y += h + 10;
		}
	}

	// picked up armor/health and dmg done
	for( side=-1; side<loop; side+=2 ){
		
		y = SB_TOURNEY_PICKUP_Y;
		offset = side < 0? 0 : 1;
		score = side < 0? p1Score : p2Score;
		
		item = BG_FindItem( "Mega Health" );
		if( cg_items[ ITEM_INDEX( item ) ].registered ){
			CG_DrawPic( x + side*w - offset*h, y, h, h, cg_items[ ITEM_INDEX( item ) ].icon );
			strcpy( string, va( "%i", score->megaHealth ) );
			CG_DrawStringExt( x + side*( w - h*2 ) - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + h/2 - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
			y += h + 10;
		}
		item = BG_FindItem( "Red Armor" );
		if( cg_items[ ITEM_INDEX( item ) ].registered ){
			CG_DrawPic( x + side*w - offset*h, y, h, h, cg_items[ ITEM_INDEX( item ) ].icon );
			strcpy( string, va( "%i", score->redArmor ) );
			CG_DrawStringExt( x + side*( w - h*2 ) - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + h/2 - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
			y += h + 10;
		}
		item = BG_FindItem( "Yellow Armor" );
		if( cg_items[ ITEM_INDEX( item ) ].registered ){
			CG_DrawPic( x + side*w - offset*h, y, h, h, cg_items[ ITEM_INDEX( item ) ].icon );
			strcpy( string, va( "%i", score->yellowArmor ) );
			CG_DrawStringExt( x + side*( w - h*2 ) - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + h/2 - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
			y += h + 10;
		}
		strcpy( string, "Dmg." );
		CG_DrawStringExt( x + side*w - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + h/2 - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
		strcpy( string, va( "%i", score->dmgdone ) );
		CG_DrawStringExt( x + side*( w - h*2 ) - offset*SB_MEDCHAR_WIDTH*CG_DrawStrlen( string ), y + h/2 - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
		
	}
	
	// awards
	y = SB_TOURNEY_AWARD_Y;
	strcpy( string, "Awards" );
	CG_DrawStringExt( x - SB_MEDCHAR_WIDTH*CG_DrawStrlen( string )/2, y - SB_MEDCHAR_HEIGHT/2, string, colorWhite, qtrue, qfalse, SB_MEDCHAR_WIDTH, SB_MEDCHAR_HEIGHT, 0 );
	
	picBar[ 0 ].pic = cgs.media.medalExcellent;
	picBar[ 0 ].percent = qfalse;
	picBar[ 1 ].pic = cgs.media.medalImpressive;
	picBar[ 1 ].percent = qfalse;
	picBar[ 2 ].pic = cgs.media.medalGauntlet;
	picBar[ 2 ].percent = qfalse;
	picBar[ 3 ].pic = cgs.media.medalAirrocket;
	picBar[ 3 ].percent = qfalse;
	picBar[ 4 ].pic = cgs.media.medalAirgrenade;
	picBar[ 4 ].percent = qfalse;
	picBar[ 5 ].pic = cgs.media.medalFullsg;
	picBar[ 5 ].percent = qfalse;
	picBar[ 6 ].pic = cgs.media.medalRocketrail;
	picBar[ 6 ].percent = qfalse;
	picBar[ 7 ].pic = cgs.media.medalItemdenied;
	picBar[ 7 ].percent = qfalse;
	
	for( side=-1; side<loop; side+=2 ){
		
		score = side < 0? p1Score : p2Score;
		
		picBar[ 0 ].val = score->excellentCount;
		picBar[ 1 ].val = score->impressiveCount;
		picBar[ 2 ].val = score->guantletCount;
		picBar[ 3 ].val = score->airrocketCount;
		picBar[ 4 ].val = score->airgrenadeCount;
		picBar[ 5 ].val = score->fullshotgunCount;
		picBar[ 6 ].val = score->rocketRailCount;
		picBar[ 7 ].val = score->itemDeniedCount;
		
		awardCount = 0;
		for( i=0; i<SB_TOURNEY_MAX_AWARDS; i++ ){
			if( picBar[ i ].val == 0 )continue;
			for( j=0; j<awardCount; j++ ){
				if( picBar[ i ].val > sortedPicBar[ j ].val ){
					for( k=awardCount-1; k>=j; k-- ){
						sortedPicBar[ k + 1 ].pic = sortedPicBar[ k ].pic;
						sortedPicBar[ k + 1 ].val = sortedPicBar[ k ].val;
						sortedPicBar[ k + 1 ].percent = sortedPicBar[ k ].percent;
					}
					break;
				}
			}
			sortedPicBar[ j ].pic = picBar[ i ].pic;
			sortedPicBar[ j ].val = picBar[ i ].val;
			sortedPicBar[ j ].percent = picBar[ i ].percent;
			awardCount++;
			if( awardCount >= SB_TOURNEY_AWARDS )break;
		}
		
		CG_DrawPicBar( sortedPicBar, awardCount, x + side*w/2, y, w*0.8, h );
		
	}
	
	// spectators
	CG_DrawSpecs();
	
	// load any models that have been deferred
	if( ++cg.deferredPlayerLoading > 10 ){
		CG_LoadDeferredPlayers();
	}
	
	return qtrue;
	
}
