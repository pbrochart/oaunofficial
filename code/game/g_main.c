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
#include "bg_promode.h"

level_locals_t	level;

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
  qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];
gitemInfos_t		g_itemInfos[MAX_GENTITIES];

vmCvar_t	g_gametype;
vmCvar_t	g_dmflags;
vmCvar_t        g_fairflags;
vmCvar_t	g_elimflags;
vmCvar_t	g_voteflags;
vmCvar_t	g_fraglimit;
vmCvar_t	g_timelimit;
vmCvar_t	g_capturelimit;
vmCvar_t	g_friendlyFire;
vmCvar_t	g_password;
vmCvar_t	g_needpass;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_quadfactor;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_weaponRespawn;
vmCvar_t	g_weaponTeamRespawn;
vmCvar_t	g_motd;
vmCvar_t	g_synchronousClients;
vmCvar_t	g_warmup;
vmCvar_t	g_doWarmup;
vmCvar_t	g_restarted;
vmCvar_t	g_logfile;
vmCvar_t	g_logfileSync;
vmCvar_t	g_gibs;
vmCvar_t	g_podiumDist;
vmCvar_t	g_podiumDrop;
vmCvar_t	g_allowVote;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_smoothClients;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;
vmCvar_t        pmove_float;
vmCvar_t	g_rankings;
vmCvar_t	g_listEntity;
vmCvar_t	g_obeliskHealth;
vmCvar_t	g_obeliskRegenPeriod;
vmCvar_t	g_obeliskRegenAmount;
vmCvar_t	g_obeliskRespawnDelay;
vmCvar_t	g_cubeTimeout;
#ifdef MISSIONPACK
vmCvar_t	g_redteam;
vmCvar_t	g_blueteam;
vmCvar_t	g_singlePlayer;
#endif
vmCvar_t	g_enableDust;
vmCvar_t	g_enableBreath;
vmCvar_t	g_proxMineTimeout;
vmCvar_t	g_music;
//Following for elimination:
vmCvar_t	g_elimination_selfdamage;
vmCvar_t	g_elimination_startHealth;
vmCvar_t	g_elimination_startArmor;
vmCvar_t	g_elimination_bfg;
vmCvar_t	g_elimination_grapple;
vmCvar_t	g_elimination_roundtime;
vmCvar_t	g_elimination_warmup;
vmCvar_t	g_elimination_activewarmup;
vmCvar_t        g_elimination_allgametypes;
vmCvar_t	g_elimination_machinegun;
vmCvar_t	g_elimination_shotgun;
vmCvar_t	g_elimination_grenade;
vmCvar_t	g_elimination_rocket;
vmCvar_t	g_elimination_railgun;
vmCvar_t	g_elimination_lightning;
vmCvar_t	g_elimination_plasmagun;
#ifdef MISSIONPACK
vmCvar_t	g_elimination_chain;
vmCvar_t	g_elimination_mine;
vmCvar_t	g_elimination_nail;
#endif
vmCvar_t        g_elimination_lockspectator;

vmCvar_t	g_rockets;

//dmn_clowns suggestions (with my idea of implementing):
vmCvar_t	g_instantgib;
vmCvar_t	g_vampire;
vmCvar_t	g_vampireMaxHealth;
//Regen
vmCvar_t	g_regen;
int	g_ffa_gt; //Are this a FFA gametype even if gametype is high?
vmCvar_t	g_lms_lives;
vmCvar_t	g_lms_mode;
vmCvar_t	g_elimination_ctf_oneway;
vmCvar_t        g_awardpushing; //The server can decide if players are awarded for pushing people in lave etc.
vmCvar_t        g_persistantpowerups; //Allow missionpack style persistant powerups?

vmCvar_t        g_catchup; //Favors the week players

vmCvar_t        g_voteNames;
vmCvar_t        g_voteGametypes;
vmCvar_t        g_voteMinTimelimit;
vmCvar_t        g_voteMaxTimelimit;
vmCvar_t        g_voteMinFraglimit;
vmCvar_t        g_voteMaxFraglimit;

vmCvar_t        g_humanplayers;

//used for voIP
vmCvar_t         g_redTeamClientNumbers;
vmCvar_t         g_blueTeamClientNumbers;

//unlagged - server options
vmCvar_t	g_delagHitscan;
vmCvar_t	g_truePing;
vmCvar_t	sv_fps;
vmCvar_t        g_lagLightning; //Adds a little lag to the lightninggun to make it less powerfull
//unlagged - server options
//KK-OAX
vmCvar_t        g_sprees;
vmCvar_t        g_altExcellent;
vmCvar_t        g_spreeDiv;

//Command/Chat spamming/flooding
vmCvar_t        g_floodMaxDemerits;
vmCvar_t        g_floodMinTime;

//Admin
vmCvar_t        g_admin;
vmCvar_t        g_adminLog;
vmCvar_t        g_adminParseSay;
vmCvar_t        g_adminNameProtect;
vmCvar_t        g_adminTempBan;
vmCvar_t        g_adminMaxBan;
vmCvar_t        g_specChat;
vmCvar_t        g_publicAdminMessages;

vmCvar_t        g_maxWarnings;
vmCvar_t        g_warningExpire;

vmCvar_t        g_minNameChangePeriod;
vmCvar_t        g_maxNameChanges;

vmCvar_t        g_newItemHeight;

#ifndef MISSIONPACK

vmCvar_t     disable_weapon_grapplinghook;
vmCvar_t     disable_weapon_nailgun;
vmCvar_t     disable_weapon_prox_launcher;
vmCvar_t     disable_weapon_chaingun;
vmCvar_t     disable_item_ammoregen;
vmCvar_t     disable_item_doubler;
vmCvar_t     disable_item_guard;
vmCvar_t     disable_item_scout;
vmCvar_t     disable_ammo_belt;
vmCvar_t     disable_ammo_mines;
vmCvar_t     disable_ammo_nails;
vmCvar_t     disable_holdable_kamikaze;

#endif

vmCvar_t     g_allowRespawnTimer;

vmCvar_t     g_startWhenReady;
vmCvar_t     g_autoReady;

vmCvar_t     g_timeoutAllowed;
vmCvar_t     g_timeoutTime;

vmCvar_t     g_delagprojectiles;

vmCvar_t     g_itemDrop;

vmCvar_t     g_writeStats;
vmCvar_t     g_statsPath;

vmCvar_t     g_teamLock;
vmCvar_t     g_redLocked;
vmCvar_t     g_blueLocked;

vmCvar_t     g_reduceRailDamage;
vmCvar_t     g_reduceLightningDamage;

vmCvar_t     g_extrapolateFrames;

vmCvar_t     g_refPassword;
vmCvar_t     g_refNames;

vmCvar_t     g_muteSpec;

vmCvar_t     g_mapcycle;
vmCvar_t     g_useMapcycle;

vmCvar_t     g_allowMultiview;
//vmCvar_t     g_demoState;

vmCvar_t     g_disableSpecs;

vmCvar_t     g_aftershockPhysic;

vmCvar_t     g_friendsThroughWalls;
vmCvar_t     g_allowKill;
vmCvar_t     g_fadeToBlack;

vmCvar_t     g_spawnProtection;

vmCvar_t     g_disableVotingTime;

vmCvar_t     g_maxWarp;
vmCvar_t     g_skipCorrection;

vmCvar_t     g_selfdamage;

vmCvar_t     g_overtime;
vmCvar_t     g_overtime_ctf_respawnDelay;
vmCvar_t     g_thrufloors;
vmCvar_t     g_forceIntermissionExit;
vmCvar_t     g_aftershockRespawn;
vmCvar_t     g_nameCheck;

vmCvar_t     g_autoServerDemos;
vmCvar_t     g_autoRestart;
vmCvar_t     g_writePlayerCoords;
vmCvar_t     g_crosshairNamesFog;
vmCvar_t     g_damagePlums;

// bk001129 - made static to avoid aliasing
static cvarTable_t		gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, 0, qfalse },

	// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
	{ NULL, "gamedate", __DATE__ , CVAR_ROM, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
	{ NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

	// latched vars
	{ &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, 0, qfalse  },

	{ &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

	// change anytime vars
	{ &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
        { &g_fairflags, "fairflags", "7", /*CVAR_SERVERINFO |*/ CVAR_ARCHIVE, 0, qtrue  },
        { &g_elimflags, "elimflags", "0", CVAR_SERVERINFO, 0, qfalse  },
        { &g_voteflags, "voteflags", "0", CVAR_SERVERINFO, 0, qfalse  },
	{ &g_fraglimit, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_capturelimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },

	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "1", CVAR_CHEAT | CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_logfile, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_logfileSync, "g_logsync", "0", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
	{ &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "5", CVAR_CHEAT, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
	{ &g_motd, "g_motd", "", 0, 0, qfalse },
	{ &g_gibs, "g_gibs", "1", 0, 0, qfalse },

	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	{ &g_allowVote, "g_allowVote", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        
        //new in beta 19
        { &g_voteNames, "g_voteNames", "/map_restart/nextmap/map/g_gametype/kick/clientkick/timelimit/fraglimit/shuffle/", CVAR_ARCHIVE, 0, qfalse }, //clientkick g_doWarmup timelimit fraglimit
        { &g_voteGametypes, "g_voteGametypes", "/0/1/3/4/5/6/7/8/9/10/11/12/", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMaxTimelimit, "g_voteMaxTimelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMinTimelimit, "g_voteMinTimelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMaxFraglimit, "g_voteMaxFraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        { &g_voteMinFraglimit, "g_voteMinFraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
        //new in beta 19 end
        
	{ &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },

	{ &g_obeliskHealth, "g_obeliskHealth", "2500", 0, 0, qfalse },
	{ &g_obeliskRegenPeriod, "g_obeliskRegenPeriod", "1", 0, 0, qfalse },
	{ &g_obeliskRegenAmount, "g_obeliskRegenAmount", "15", 0, 0, qfalse },
	{ &g_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, 0, qfalse },

	{ &g_cubeTimeout, "g_cubeTimeout", "30", 0, 0, qfalse },
        #ifdef MISSIONPACK
	{ &g_redteam, "g_redteam", "Stroggs", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue },
	{ &g_blueteam, "g_blueteam", "Pagans", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue  },
	{ &g_singlePlayer, "ui_singlePlayerActive", "", 0, 0, qfalse, qfalse  },
        #endif

	{ &g_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_proxMineTimeout, "g_proxMineTimeout", "20000", 0, 0, qfalse },

	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse},
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse},

        { &pmove_float, "pmove_float", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qtrue},

//unlagged - server options
	{ &g_delagHitscan, "g_delagHitscan", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue },
	{ &g_truePing, "g_truePing", "0", CVAR_ARCHIVE, 0, qtrue },
	// it's CVAR_SYSTEMINFO so the client's sv_fps will be automagically set to its value
	{ &sv_fps, "sv_fps", "40", CVAR_SYSTEMINFO | CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse },
        { &g_lagLightning, "g_lagLightning", "0", CVAR_ARCHIVE, 0, qtrue },
//unlagged - server options

	{ &g_rankings, "g_rankings", "0", 0, 0, qfalse},
        { &g_music, "g_music", "", 0, 0, qfalse},
	//Now for elimination stuff:
	{ &g_elimination_selfdamage, "elimination_selfdamage", "0", 0, 0, qtrue },
	{ &g_elimination_startHealth, "elimination_startHealth", "200", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_startArmor, "elimination_startArmor", "150", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_bfg, "elimination_bfg", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
        { &g_elimination_grapple, "elimination_grapple", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_roundtime, "elimination_roundtime", "300", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_warmup, "elimination_warmup", "7", CVAR_ARCHIVE | CVAR_NORESTART , 0, qtrue },
	{ &g_elimination_activewarmup, "elimination_activewarmup", "5", CVAR_ARCHIVE | CVAR_NORESTART , 0, qtrue },
        { &g_elimination_allgametypes, "g_elimination", "0", CVAR_LATCH | CVAR_NORESTART, 0, qfalse },

	{ &g_elimination_machinegun, "elimination_machinegun", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_shotgun, "elimination_shotgun", "30", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_grenade, "elimination_grenade", "20", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_rocket, "elimination_rocket", "50", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_railgun, "elimination_railgun", "20", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_lightning, "elimination_lightning", "200", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_plasmagun, "elimination_plasmagun", "150", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
#ifdef MISSIONPACK
	{ &g_elimination_chain, "elimination_chain", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_mine, "elimination_mine", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
	{ &g_elimination_nail, "elimination_nail", "0", CVAR_ARCHIVE| CVAR_NORESTART, 0, qtrue },
#endif
	{ &g_elimination_ctf_oneway, "elimination_ctf_oneway", "1", CVAR_CHEAT| CVAR_NORESTART, 0, qtrue },

        { &g_elimination_lockspectator, "elimination_lockspectator", "2", CVAR_NORESTART, 0, qtrue },
        
        { &g_awardpushing, "g_awardpushing", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

        //g_persistantpowerups
        #ifdef MISSIONPACK
        { &g_persistantpowerups, "g_runes", "1", CVAR_LATCH, 0, qfalse },
        #else
        { &g_persistantpowerups, "g_runes", "0", CVAR_LATCH|CVAR_ARCHIVE, 0, qfalse },
        #endif


	//nexuiz style rocket arena
	{ &g_rockets, "g_rockets", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_NORESTART, 0, qfalse },

	//Instantgib and Vampire thingies
	{ &g_instantgib, "g_instantgib", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_vampire, "g_vampire", "0.0", CVAR_NORESTART, 0, qtrue },
	{ &g_regen, "g_regen", "0", CVAR_NORESTART, 0, qtrue },
	{ &g_vampireMaxHealth, "g_vampire_max_health", "500", CVAR_NORESTART, 0, qtrue },
	{ &g_lms_lives, "g_lms_lives", "1", CVAR_NORESTART, 0, qtrue },
	{ &g_lms_mode, "g_lms_mode", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

        { &g_catchup, "g_catchup", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue},

        { &g_humanplayers, "g_humanplayers", "0", CVAR_ROM | CVAR_NORESTART, 0, qfalse },
//used for voIP
        { &g_redTeamClientNumbers, "g_redTeamClientNumbers", "0",CVAR_ROM, 0, qfalse },
        { &g_blueTeamClientNumbers, "g_blueTeamClientNumbers", "0",CVAR_ROM, 0, qfalse },
        
        //KK-OAX
        { &g_sprees, "g_sprees", "sprees.dat", 0, 0, qfalse },
        { &g_altExcellent, "g_altExcellent", "0", CVAR_SERVERINFO, 0, qtrue}, 
        { &g_spreeDiv, "g_spreeDiv", "30", 0, 0, qfalse},
        
        //Used for command/chat flooding
        { &g_floodMaxDemerits, "g_floodMaxDemerits", "5000", CVAR_ARCHIVE, 0, qfalse  },
        { &g_floodMinTime, "g_floodMinTime", "2000", CVAR_ARCHIVE, 0, qfalse  },
        
        //Admin
        { &g_admin, "g_admin", "admin.dat", CVAR_ARCHIVE, 0, qfalse  },
        { &g_adminLog, "g_adminLog", "admin.log", CVAR_ARCHIVE, 0, qfalse  },
        { &g_adminParseSay, "g_adminParseSay", "1", CVAR_ARCHIVE, 0, qfalse  },
        { &g_adminNameProtect, "g_adminNameProtect", "1", CVAR_ARCHIVE, 0, qfalse  },
        { &g_adminTempBan, "g_adminTempBan", "2m", CVAR_ARCHIVE, 0, qfalse  },
        { &g_adminMaxBan, "g_adminMaxBan", "2w", CVAR_ARCHIVE, 0, qfalse  },
        
        { &g_specChat, "g_specChat", "1", CVAR_ARCHIVE, 0, qfalse  },
        { &g_publicAdminMessages, "g_publicAdminMessages", "1", CVAR_ARCHIVE, 0, qfalse  },
        
        { &g_maxWarnings, "g_maxWarnings", "3", CVAR_ARCHIVE, 0, qfalse },
	    { &g_warningExpire, "g_warningExpire", "3600", CVAR_ARCHIVE, 0, qfalse },
	    
	    { &g_minNameChangePeriod, "g_minNameChangePeriod", "5", 0, 0, qfalse},
        { &g_maxNameChanges, "g_maxNameChanges", "5", 0, 0, qfalse},
	
	{ &g_newItemHeight, "g_newItemHeight", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},

#ifndef MISSIONPACK

	{ &disable_weapon_grapplinghook, "disable_weapon_grapplinghook", "1", 0, 0, qfalse},
	{ &disable_weapon_nailgun, "disable_weapon_nailgun", "1", 0, 0, qfalse},
	{ &disable_weapon_prox_launcher, "disable_weapon_prox_launcher", "1", 0, 0, qfalse},
	{ &disable_weapon_chaingun, "disable_weapon_chaingun", "1", 0, 0, qfalse},
	{ &disable_item_ammoregen, "disable_item_ammoregen", "1", 0, 0, qfalse},
	{ &disable_item_doubler, "disable_item_doubler", "1", 0, 0, qfalse},
	{ &disable_item_guard, "disable_item_guard", "1", 0, 0, qfalse},
	{ &disable_item_scout, "disable_item_scout", "1", 0, 0, qfalse},
	{ &disable_ammo_belt, "disable_ammo_belt", "1", 0, 0, qfalse},
	{ &disable_ammo_mines, "disable_ammo_mines", "1", 0, 0, qfalse},
	{ &disable_ammo_nails, "disable_ammo_nails", "1", 0, 0, qfalse},
	{ &disable_holdable_kamikaze, "disable_holdable_kamikaze", "1", 0, 0, qfalse},
	{ &disable_holdable_kamikaze, "disable_holdable_invulnerability", "1", 0, 0, qfalse},
	
#endif
	
	{ &g_allowRespawnTimer, "g_allowRespawnTimer", "0", CVAR_ARCHIVE, 0, qfalse},
	{ &g_startWhenReady, "g_startWhenReady", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},
		{ &g_autoReady, "g_autoReady", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},
	
	{ &g_timeoutAllowed, "g_timeoutAllowed", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},
	{ &g_timeoutTime, "g_timeoutTime", "30000", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},
	
	{ &g_delagprojectiles, "g_delagprojectiles", "100", CVAR_SYSTEMINFO, 0, qfalse },
	
	{ &g_itemDrop, "g_itemDrop", "1", CVAR_SYSTEMINFO, 0, qfalse },
	
	{ &g_writeStats, "g_writeStats", "1", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_statsPath, "g_statsPath", "serverstats", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	
	{ &g_teamLock, "g_teamLock", "0", CVAR_SERVERINFO |CVAR_NORESTART, 0, qfalse },
	{ &g_redLocked, "g_redLocked", "0", CVAR_SERVERINFO | CVAR_NORESTART, 0, qfalse },
	{ &g_blueLocked, "g_blueLocked", "0", CVAR_SERVERINFO | CVAR_NORESTART, 0, qfalse },

	{ &g_reduceRailDamage, "g_reduceRailDamage", "1", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_reduceLightningDamage, "g_reduceLightningDamage", "1", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_extrapolateFrames, "g_extrapolateFrames", "2", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_refPassword, "g_refPassword", "", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_refNames, "g_refNames", "/map_restart/nextmap/map/g_gametype/kick/clientkick/timelimit/fraglimit/remove/clientremove/lock/unlock/startgame/", CVAR_ARCHIVE, 0, qfalse },
	{ &g_muteSpec, "g_muteSpec", "0", CVAR_ARCHIVE, 0, qfalse},
	
	{ &g_mapcycle, "g_mapcycle", "mapcycle.cfg", CVAR_ARCHIVE, 0, qfalse},
	{ &g_useMapcycle, "g_useMapcycle", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse},
	{ &g_allowMultiview, "g_allowMultiview", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},
	// demo state
	//{ &g_demoState, "sv_demoState", "", 0, 0, qfalse },
	{ &g_disableSpecs, "g_disableSpecs", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_aftershockPhysic, "g_aftershockPhysic", "0", CVAR_SERVERINFO, 0, qfalse },
	{ &g_friendsThroughWalls, "g_friendsThroughWalls", "0", CVAR_SERVERINFO, 0, qfalse },
	
	{ &g_allowKill, "g_allowKill", "1", CVAR_SERVERINFO, 0, qfalse },
	{ &g_fadeToBlack, "g_fadeToBlack", "0", CVAR_SERVERINFO, 0, qfalse },
	{ &g_spawnProtection, "g_spawnProtection", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_disableVotingTime, "g_disableVotingTime", "0", CVAR_ARCHIVE, 0 ,qfalse },
	{ &g_maxWarp, "g_maxWarp", "0", CVAR_CHEAT, 0, qfalse  },
	{ &g_skipCorrection, "g_skipCorrection", "0", CVAR_CHEAT, 0, qfalse  },
	{ &g_selfdamage, "g_selfdamage", "1", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse  },
	{ &g_overtime, "g_overtime", "120", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse  },
	{ &g_overtime_ctf_respawnDelay, "g_overtime_ctf_respawnDelay", "5", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_thrufloors, "g_thrufloors", "0", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_forceIntermissionExit, "g_forceIntermissionExit", "0", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_aftershockRespawn, "g_aftershockRespawn", "1", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_nameCheck, "g_nameCheck", "1", CVAR_ARCHIVE, 0, qfalse },
	{ &g_autoServerDemos, "g_autoServerDemos", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_autoRestart, "g_autoRestart", "1", CVAR_ARCHIVE, 0, qfalse },
	{ &g_writePlayerCoords, "g_writePlayerCoords", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_crosshairNamesFog, "g_crosshairNamesFog", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_damagePlums, "g_damagePlums", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse }
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );
//void G_DemoSetClient( void );
//void G_DemoRemoveClient( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {
	switch ( command ) {
	case GAME_INIT:
		G_InitGame( arg0, arg1, arg2 );
		return 0;
	case GAME_SHUTDOWN:
		G_ShutdownGame( arg0 );
		return 0;
	case GAME_CLIENT_CONNECT:
		return (intptr_t)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		return 0;
	case GAME_CONSOLE_COMMAND:
		return ConsoleCommand();
	case BOTAI_START_FRAME:
		return BotAIStartFrame( arg0 );
	/*case GAME_DEMO_COMMAND:
		switch ( arg0 )
		{
			case DC_CLIENT_SET:
				G_DemoSetClient( );
				break;
			case DC_CLIENT_REMOVE:
				G_DemoRemoveClient( );
				break;
		}
		return 0;*/
	}
	return -1;
}


void QDECL G_Printf( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=MAX_CLIENTS, e=g_entities+i ; i < level.num_entities ; i++,e++ ) {
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}
        G_Printf ("%i teams with %i entities\n", c, c2);
}

void G_RemapTeamShaders( void ) {
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf( string, sizeof(string), "team_icon/%s_red", g_redteam.string );
	AddRemap("textures/ctf2/redteam01", string, f); 
	AddRemap("textures/ctf2/redteam02", string, f); 
	Com_sprintf( string, sizeof(string), "team_icon/%s_blue", g_blueteam.string );
	AddRemap("textures/ctf2/blueteam01", string, f); 
	AddRemap("textures/ctf2/blueteam02", string, f); 
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
#endif
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
			cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar )
			cv->modificationCount = cv->vmCvar->modificationCount;

		if (cv->teamShader) {
			remapped = qtrue;
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	if ( g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE ) {
                G_Printf( "g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "0" );
		trap_Cvar_Update( &g_gametype );
	}

	//set FFA status for high gametypes:
	if ( g_gametype.integer == GT_LMS ) {
		g_ffa_gt = 1;	//Last Man standig is a FFA gametype
	} else {
		g_ffa_gt = 0;	//If >GT_CTF use bases
	}

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", 
						cv->cvarName, cv->vmCvar->string ) );
				}

                                if ( cv->vmCvar == &g_voteNames ) {
                                    //Set vote flags
                                    int voteflags=0;
                                    if( allowedVote("map_restart") )
                                        voteflags|=VF_map_restart;

                                    if( allowedVote("map") )
                                        voteflags|=VF_map;

                                    if( allowedVote("clientkick") )
                                        voteflags|=VF_clientkick;

                                    if( allowedVote("shuffle") )
                                        voteflags|=VF_shuffle;

                                    if( allowedVote("nextmap") )
                                        voteflags|=VF_nextmap;

                                    if( allowedVote("g_gametype") )
                                        voteflags|=VF_g_gametype;
                                    
                                    if( allowedVote("g_doWarmup") )
                                        voteflags|=VF_g_doWarmup;

                                    if( allowedVote("timelimit") )
                                        voteflags|=VF_timelimit;

                                    if( allowedVote("fraglimit") )
                                        voteflags|=VF_fraglimit;

                                    if( allowedVote("custom") )
                                        voteflags|=VF_custom;

                                    trap_Cvar_Set("voteflags",va("%i",voteflags));
                                }
 
				if (cv->teamShader) {
					remapped = qtrue;
				}
			}
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}
}

void G_SendAllItems( void ){
	gentity_t *ent;
	int i,j;
	
	if( !g_allowRespawnTimer.integer )
		return;
	
	if( g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION )
		return;
	
	/*for( i = 0; i < MAX_GENTITIES; i++ ){
		if( g_entities[i].s.eType == ET_ITEM )
			g_itemInfos[i].team = G_ItemTeam( i );
	}*/
	
	//G_Printf("check\n");
	if( g_gametype.integer == GT_CTF ){
		for( j = 0; j < 3; j++ ){
			for( i = 0; i < MAX_GENTITIES; i++ ){
				ent = &g_entities[i];
				if( !ent->inuse )
					continue;
				if( !(ent->s.eType == ET_ITEM) )
					continue;
				if( ent->flags == FL_DROPPED_ITEM )
					continue;
				//G_Printf("%s\n", ent->item->shortPickup_name);
				if( ( G_ItemTeam( ent->s.number ) == TEAM_RED && j == 0 ) || ( G_ItemTeam( ent->s.number ) == TEAM_BLUE && j == 1 ) || ( G_ItemTeam( ent->s.number ) == -1 && j == 2 ) )
					G_SendRespawnTimer( ent->s.number, ent->item->giType, ent->item->quantity, ent->nextthink, G_FindNearestItemSpawn( ent ), -1 );
			}
		}
	}
	else{
		for( i = 0; i < MAX_GENTITIES; i++ ){
			ent = &g_entities[i];
			if( !ent->inuse )
				continue;
			if( !(ent->s.eType == ET_ITEM) )
				continue;
			if( ent->flags == FL_DROPPED_ITEM )
				continue;
			//G_Printf("%s\n", ent->item->shortPickup_name);
			
			G_SendRespawnTimer( ent->s.number, ent->item->giType, ent->item->quantity, ent->nextthink, G_FindNearestItemSpawn( ent ), -1 );
		}
	}
}
/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int					i;
	char 		mapname[64];
	char 		mapfile[MAX_QPATH];
	char 		lastmap[64];
	
        G_Printf ("------- Game Initialization -------\n");
        G_Printf ("gamename: %s\n", GAMEVERSION);
        G_Printf ("gamedate: %s\n", __DATE__);

	srand( randomSeed );

	G_RegisterCvars();
	
	if( restart == 0 ){
		trap_Cvar_Set("g_redLocked","0");
		trap_Cvar_Set("g_blueLocked","0");
		for( i = 0; i < MAX_CLIENTS; i++ ){
			trap_Cvar_Set(va( "demopath%i", i ), "");
		}
	}
        
        //disable unwanted cvars
        if( g_gametype.integer == GT_SINGLE_PLAYER )
        {
            g_instantgib.integer = 0;
            g_rockets.integer = 0;
            g_vampire.value = 0.0f;
        }

	G_ProcessIPBans();
    
    //KK-OAX Changed to Tremulous's BG_InitMemory
	BG_InitMemory();

	// CPM: Initialize
	// Update all settings
	CPM_UpdateSettings(g_aftershockPhysic.integer);
	// Set the config string
	trap_SetConfigstring(CS_PRO_MODE, va("%d", g_aftershockPhysic.integer));
	// !CPM

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	
	level.time = levelTime;
	level.startTime = levelTime;
	level.timeComplete = levelTime;
	
	level.timeoutAdd = 0;
	level.timeoutDelay = 0;
	level.endgameSend = qfalse;
	
	level.overtimeCount = 0;
	
	/*trap_Cvar_VariableStringBuffer( "session", buffer, sizeof( buffer ) );
	sscanf( buffer, "%i %i", &a, &b );
	if ( a != trap_Cvar_VariableIntegerValue( "sv_maxclients" ) ||
		b != trap_Cvar_VariableIntegerValue( "sv_democlients" ) )
		level.newSession = qtrue;*/

	level.snd_fry = G_SoundIndex("sound/player/fry.wav");	// FIXME standing in lava / slime

	if ( g_gametype.integer != GT_SINGLE_PLAYER && g_logfile.string[0] ) {
		if ( g_logfileSync.integer ) {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_logfile.string );
		} else {
			char	serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", serverinfo );
                        G_LogPrintf("Info: ServerInfo length: %d of %d\n", strlen(serverinfo), MAX_INFO_STRING );
		}
	} else {
		G_Printf( "Not logging to disk.\n" );
	}

        //Parse the custom vote names:
        VoteParseCustomVotes();

	G_InitWorldSession();
    
    //KK-OAX Get Admin Configuration
    G_admin_readconfig( NULL, 0 );
	//Let's Load up any killing sprees/multikills
	G_ReadAltKillSettings( NULL, 0 );

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;
	
	memset( g_itemInfos, 0, MAX_GENTITIES * sizeof(g_itemInfos[0]));
	level.itemInfo = g_itemInfos;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ), 
		&level.clients[0].ps, sizeof( level.clients[0] ) );

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if( g_gametype.integer >= GT_TEAM && (g_ffa_gt!=1)) {
		G_CheckTeamItems();
	}

	SaveRegisteredItems();
	
	trap_Cvar_VariableStringBuffer("lastmap", lastmap, sizeof(lastmap));
	trap_Cvar_VariableStringBuffer("mapfile", mapfile, sizeof(mapfile));
	trap_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));
	
	if( !strcmp(mapname,lastmap) && strcmp("",mapfile) && strcmp(mapname,"") ) {
		G_LoadMapfile(mapfile);
	} else {
		trap_Cvar_Set("mapfile","");
		trap_Cvar_Set("lastmap","");
	}
	if( g_useMapcycle.integer ){
		G_LoadMapcycle();
		if( strcmp(mapname,lastmap) || !strcmp("",mapfile))
			G_GetMapfile(mapname);
		
	}
        
        G_Printf ("-----------------------------------\n");

	if( g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue( "com_buildScript" ) ) {
		G_ModelIndex( SP_PODIUM_MODEL );
		G_SoundIndex( "sound/player/gurp1.wav" );
		G_SoundIndex( "sound/player/gurp2.wav" );
	}

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}

	G_RemapTeamShaders();

	//elimination:
	level.roundNumber = 1;
	level.roundNumberStarted = 0;
	level.roundStartTime = level.time+g_elimination_warmup.integer*1000;
	level.roundRespawned = qfalse;
	level.eliminationSides = rand()%2; //0 or 1

	//Challenges:
	level.teamSize = 0;
	level.hadBots = qfalse;

	if(g_gametype.integer == GT_DOUBLE_D)
		Team_SpawnDoubleDominationPoints();

	if(g_gametype.integer == GT_DOMINATION ){
		level.dom_scoreGiven = 0;
		for(i=0;i<MAX_DOMINATION_POINTS;i++)
			level.pointStatusDom[i] = TEAM_NONE;
		level.domination_points_count = 0; //make sure its not too big
	}

        PlayerStoreInit();

        //Set vote flags
        {
            int voteflags=0;
            if( allowedVote("map_restart") )
                voteflags|=VF_map_restart;

            if( allowedVote("map") )
                voteflags|=VF_map;

            if( allowedVote("clientkick") )
                voteflags|=VF_clientkick;

            if( allowedVote("shuffle") )
                voteflags|=VF_shuffle;

            if( allowedVote("nextmap") )
                voteflags|=VF_nextmap;

            if( allowedVote("g_gametype") )
                voteflags|=VF_g_gametype;

            if( allowedVote("g_doWarmup") )
                voteflags|=VF_g_doWarmup;

            if( allowedVote("timelimit") )
                voteflags|=VF_timelimit;

            if( allowedVote("fraglimit") )
                voteflags|=VF_fraglimit;

            if( allowedVote("custom") )
                voteflags|=VF_custom;

            trap_Cvar_Set("voteflags",va("%i",voteflags));
        }
        
        G_SendAllItems();
	
	
	memset(level.disconnectedClients, 0, MAX_DISCONNECTEDCLIENTS * sizeof(level.disconnectedClients[0]));
	level.disconnectedClientsNumber = 0;
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {
        G_Printf ("==== ShutdownGame ====\n");

	if ( level.logFile ) {
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();
	
	//KK-OAX Admin Cleanup
    G_admin_cleanup( );
    G_admin_namelog_cleanup( );

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAIShutdown( restart );
	}
}



//===================================================================

void QDECL Com_Error ( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	G_Error( "%s", text);
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

        G_Printf ("%s", text);
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer( void ) {
	int			i;
	gclient_t	*client;
	gclient_t	*nextInLine;

	if ( level.numPlayingClients >= 2 ) {
		return;
	}

	// never change during intermission
	if ( level.intermissiontime || level.intermissionQueued ) {
		return;
	}

	nextInLine = NULL;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD || 
			client->sess.spectatorClient < 0  ) {
			continue;
		}
		if ( client->sess.specOnly )
			continue;

		if ( !nextInLine || client->sess.spectatorTime < nextInLine->sess.spectatorTime ) {
			nextInLine = client;
		}
	}

	if ( !nextInLine ) {
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam( &g_entities[ nextInLine - level.clients ], "f" );
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[1];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
RemoveTournamentWinner
=======================
*/
void RemoveTournamentWinner( void ) {
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[0];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
AdjustTournamentScores
=======================
*/
void AdjustTournamentScores( void ) {
	int			clientNum;

	clientNum = level.sortedClients[0];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED && level.clients[ clientNum ].sess.sessionTeam == TEAM_FREE ) {
		level.clients[ clientNum ].sess.wins++;
		ClientUserinfoChanged( clientNum );
	}

	clientNum = level.sortedClients[1];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED && level.clients[ clientNum ].sess.sessionTeam == TEAM_FREE ) {
		level.clients[ clientNum ].sess.losses++;
		ClientUserinfoChanged( clientNum );
	}

}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if( ( ca->sess.specOnly && cb->sess.specOnly ) || ( !ca->sess.specOnly && !cb->sess.specOnly ) ) {
			if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
				return -1;
			}
			if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
				return 1;
			}
			return 0;
		}
		if( ca->sess.specOnly ){
			return 1;
		}
		if( cb->sess.specOnly ){
			return -1;
		}
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

        //In elimination and CTF elimination, sort dead players last
        /*if((g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION)
                && level.roundNumber==level.roundNumberStarted && (ca->isEliminated != cb->isEliminated)) {
            if( ca->isEliminated )
                return 1;
            if( cb->isEliminated )
                return -1;
        }*/ //that sucks
        
        if(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION){
		if( ca->dmgdone > cb->dmgdone )
		    return -1;
		if( ca->dmgdone < cb->dmgdone )
		    return 1;
	}
	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
		> cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
		< cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int		i;
	int		rank;
	int		score;
	int		newScore;
        int             humanplayers;
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
        humanplayers = 0; // don't count bots
	for ( i = 0; i < ARRAY_LEN(level.numteamVotingClients); i++ )
		level.numteamVotingClients[i] = 0;
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

                        //We just set humanplayers to 0 during intermission
                        if ( !level.intermissiontime && level.clients[i].pers.connected == CON_CONNECTED /*&& !level.clients[ i ].pers.demoClient*/ && !(g_entities[i].r.svFlags & SVF_BOT) ) {
                            humanplayers++;
                        }

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR /*&& !level.clients[ i ].pers.demoClient*/) {
                                level.numNonSpectatorClients++;
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						if ( level.clients[i].sess.sessionTeam == TEAM_RED )
							level.numteamVotingClients[0]++;
						else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )
							level.numteamVotingClients[1]++;
					}
					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {	
		rank = -1;
		score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE] ) );
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", SCORE_NOT_PRESENT) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		} else {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}
        
        if(g_humanplayers.integer != humanplayers) //Presume all spectators are humans!
            trap_Cvar_Set( "g_humanplayers", va("%i", humanplayers) );
}

/*
============
G_DemoCommand

Store a demo command to a demo if we are recording
============
*/
/*void G_DemoCommand( demoCommand_t cmd, const char *string )
{
  if( level.demoState == DS_RECORDING )
    trap_DemoCommand( cmd, string );
}*/

/*
============
G_DemoSetClient

Mark a client as a demo client and load info into it
============
*/
/*void G_DemoSetClient( void )
{
  char buffer[ MAX_INFO_STRING ];
  gclient_t *client;
  char *s;

  G_Printf("DemoSet\n");
  
  trap_Argv( 0, buffer, sizeof( buffer ) );
  client = level.clients + atoi( buffer );
  client->pers.demoClient = qtrue;

  trap_Argv( 1, buffer, sizeof( buffer ) );
  s = Info_ValueForKey( buffer, "name" );
  if( *s )
    Q_strncpyz( client->pers.netname, s, sizeof( client->pers.netname ) );
  s = Info_ValueForKey( buffer, "ip" );
  if( *s )
    Q_strncpyz( client->pers.ip, s, sizeof( client->pers.ip ) );
  s = Info_ValueForKey( buffer, "team" );
  if( *s )
    client->sess.sessionTeam = atoi( s );
  client->sess.spectatorState = SPECTATOR_NOT;
}*/

/*
============
G_DemoRemoveClient

Unmark a client as a demo client
============
*/
/*void G_DemoRemoveClient( void )
{
  char buffer[ 3 ];
  gclient_t *client;

  G_Printf("DemoRemove\n");
  
  trap_Argv( 0, buffer, sizeof( buffer ) );
  client = level.clients + atoi( buffer );
  client->pers.demoClient = qfalse;
}*/


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
			EliminationMessage( g_entities + i );
		}
	}
}

/*
========================
SendElimiantionMessageToAllClients

Used to send information important to Elimination
========================
*/
void SendEliminationMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			EliminationMessage( g_entities + i );
		}
	}
}

void SendCaptureStrikeMessage( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			if( ( level.clients[i].sess.sessionTeam == TEAM_RED && (level.eliminationSides+level.roundNumber)%2 == 0 ) || 
			  ( level.clients[i].sess.sessionTeam == TEAM_BLUE && (level.eliminationSides+level.roundNumber)%2 != 0 ) )
				trap_SendServerCommand( i, "cp \"^2Capture!\"");
			else if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR )	
				trap_SendServerCommand( i, "cp \"^1Defend!\"");
		}
	}
}

/*
========================
SendDDtimetakenMessageToAllClients

Do this if a team just started dominating.
========================
*/
void SendDDtimetakenMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DoubleDominationScoreTimeMessage( g_entities + i );
		}
	}
}

/*
========================
SendAttackingTeamMessageToAllClients

Used for CTF Elimination oneway
========================
*/
void SendAttackingTeamMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			AttackingTeamMessage( g_entities + i );
		}
	}
}

/*
========================
SendDominationPointsStatusMessageToAllClients

Used for Standard domination
========================
*/
void SendDominationPointsStatusMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DominationPointStatusMessage( g_entities + i );
		}
	}
}
/*
========================
SendYourTeamMessageToTeam

Tell all players on a given team who there allies are. Used for VoIP
========================
*/
void SendYourTeamMessageToTeam( team_t team ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED && level.clients[ i ].sess.sessionTeam == team ) {
			YourTeamMessage( g_entities + i );
		}
	}
}


/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}


	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t	*ent, *target;
	vec3_t		dir;

	// find the intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if ( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle, qfalse );
	} else {
		VectorCopy (ent->s.origin, level.intermission_origin);
		VectorCopy (ent->s.angles, level.intermission_angle);
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int			i;
	gentity_t	*client;

	if ( level.intermissiontime ) {
		return;		// already active
	}

	// if in tournement mode, change the wins / losses
	if ( g_gametype.integer == GT_TOURNAMENT ) {
		AdjustTournamentScores();
	}

	level.intermissiontime = level.time;
	FindIntermissionPoint();

#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		trap_Cvar_Set("ui_singlePlayerActive", "0");
		UpdateTournamentInfo();
	}
#else
	// if single player game
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		UpdateTournamentInfo();
		SpawnModelsOnVictoryPads();
	}
#endif

	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		// respawn if dead
		if (client->health <= 0) {
			respawn(client);
		}
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();
	

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar 

=============
*/
void ExitLevel (void) {
	int		i;
	gclient_t *cl;
	char nextmap[MAX_STRING_CHARS];
	char d1[MAX_STRING_CHARS];
	char	command[1024];
	char    mapname[MAX_MAPNAME];

	//bot interbreeding
	BotInterbreedEndMatch();

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
	if ( g_gametype.integer == GT_TOURNAMENT  ) {
		if ( !level.restarted ) {
			RemoveTournamentLoser();
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;	
	}
	if( g_useMapcycle.integer ){
	    trap_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));
            Com_sprintf(command, sizeof( command ),"map %s\n", G_GetNextMap(mapname));
	    trap_SendConsoleCommand( EXEC_APPEND, command );
	}
	else {
	    trap_Cvar_VariableStringBuffer( "nextmap", nextmap, sizeof(nextmap) );
	    trap_Cvar_VariableStringBuffer( "d1", d1, sizeof(d1) );

	    if( !Q_stricmp( nextmap, "map_restart 0" ) && Q_stricmp( d1, "" ) ) {
		    trap_Cvar_Set( "nextmap", "vstr d2" );
		    trap_SendConsoleCommand( EXEC_APPEND, "vstr d1\n" );
	    } else {
		    trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	    }
	}

	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	Q_vsnprintf(string + 7, sizeof(string) - 7, fmt, argptr);
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int				i, numSorted;
	gclient_t		*cl;
#ifdef MISSIONPACK
	qboolean won = qtrue;
#endif
	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		G_LogPrintf( "red:%i  blue:%i\n",
			level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],	cl->pers.netname );
#ifdef MISSIONPACK
		if (g_singlePlayer.integer && g_gametype.integer == GT_TOURNAMENT) {
			if (g_entities[cl - level.clients].r.svFlags & SVF_BOT && cl->ps.persistant[PERS_RANK] == 0) {
				won = qfalse;
			}
		}
#endif

	}

#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		if (g_gametype.integer >= GT_CTF && g_ffa_gt==0) {
			won = level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE];
		}
		trap_SendConsoleCommand( EXEC_APPEND, (won) ? "spWin\n" : "spLose\n" );
	}
#endif


}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	int			ready, notReady, playerCount;
	int			i;
	gclient_t	*cl;
	int			readyMask;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
        playerCount = 0;
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

                playerCount++;
		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// only test ready status when there are real players present
	if ( playerCount > 0 ) {
		// if nobody wants to go, clear timer
		if ( !ready ) {
			level.readyToExit = qfalse;
			return;
		}

		// if everyone wants to go, go now
		if ( !notReady ) {
			ExitLevel();
			return;
		}
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int		a, b;

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}
        
        //Sago: In Elimination and Oneway Flag Capture teams must win by two points.
        if ( g_gametype.integer == GT_ELIMINATION || 
                (g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer)) {
            return (level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] /*|| 
                    level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]+1 ||
                    level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]-1*/);
        }
	
	if ( g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void ) {
 	int			i;
	gclient_t	*cl;
	
	// don't exit in demos
  	/*if( level.demoState == DS_PLAYBACK )
    		return;*/
	
	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		if( ( level.time > level.intermissiontime + 2000 ) && ( !level.endgameSend ) ){
			//G_StopServerDemo();
			G_SendEndGame();
			if( g_writeStats.integer )
				G_WriteXMLStats();
			G_StopServerDemos();
		}
		CheckIntermissionExit ();
		return;
	} else {
            //sago: Find the reason for this to be neccesary.
            for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
                }
                cl->ps.stats[STAT_CLIENTS_READY] = 0;
            }
        }

	if ( level.intermissionQueued ) {
#ifdef MISSIONPACK
		int time = (g_singlePlayer.integer) ? SP_INTERMISSION_DELAY_TIME : INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#else
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#endif
		return;
	}

	// check for sudden death
	if ( ScoreIsTied() && g_overtime.integer <= 0) {
		// always wait for sudden death
		return;
	}

	if( g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer != 0 ){
	  if( (level.eliminationSides+level.roundNumber)%2 != 0 )
		return;
	}
	
	if( g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_ELIMINATION ) {
	  if( level.roundNumber == level.roundNumberStarted )
		return;
	}

	if ( g_timelimit.integer < 0 || g_timelimit.integer > INT_MAX / 60000 ) {
		G_Printf( "timelimit %i is out of range, defaulting to 0\n", g_timelimit.integer );
		trap_Cvar_Set( "timelimit", "0" );
		trap_Cvar_Update( &g_timelimit );
	}

	if ( g_timelimit.integer && !level.warmupTime ) {
		if ( level.time - level.startTime - level.timeoutDelay >= g_timelimit.integer*60000 + g_overtime.integer * level.overtimeCount * 1000 ) {
			if( ScoreIsTied() && g_overtime.integer > 0 ){
				level.overtimeCount++;
				trap_SendServerCommand( -1, va("print \"" S_COLOR_YELLOW "OVERTIME " S_COLOR_WHITE "%i" S_COLOR_YELLOW " seconds added.\n\"", g_overtime.integer));
				trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_YELLOW "OVERTIME " S_COLOR_WHITE "%i" S_COLOR_YELLOW " seconds added.\"", g_overtime.integer));
				trap_SendServerCommand( -1, "overtime" );
				if( g_gametype.integer == GT_CTF && g_overtime_ctf_respawnDelay.integer > 0 ){
					trap_SendServerCommand( -1, va("print \"" S_COLOR_WHITE "%i" S_COLOR_YELLOW " seconds respawn delay.\n\"", g_overtime_ctf_respawnDelay.integer));
					trap_SendServerCommand( -1, va("screenPrint \"" S_COLOR_WHITE "%i" S_COLOR_YELLOW " seconds respawn delay.\"", g_overtime_ctf_respawnDelay.integer));
				}
			} else {
				if( g_overtime.integer <= 0 || !level.overtimeCount ) {
					trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
					LogExit( "Timelimit hit." );  
				} else {
					trap_SendServerCommand( -1, "print \"Overtime complete.\n\"");
					LogExit( "Overtime complete." );
				}
			}
			return;
		}
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if ( g_fraglimit.integer < 0 ) {
		G_Printf( "fraglimit %i is out of range, defaulting to 0\n", g_fraglimit.integer );
		trap_Cvar_Set( "fraglimit", "0" );
		trap_Cvar_Update( &g_fraglimit );
	}

	if ( (g_gametype.integer < GT_CTF || g_ffa_gt>0 ) && g_fraglimit.integer ) {
		if ( level.teamScores[TEAM_RED] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the fraglimit.\n\"" );
			LogExit( "Fraglimit hit." );
			return;
		}

		for ( i=0 ; i< g_maxclients.integer ; i++ ) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( cl->sess.sessionTeam != TEAM_FREE ) {
				continue;
			}

			if ( cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer ) {
				LogExit( "Fraglimit hit." );
				trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
					cl->pers.netname ) );
				return;
			}
		}
	}

	if ( g_capturelimit.integer < 0 ) {
		G_Printf( "capturelimit %i is out of range, defaulting to 0\n", g_capturelimit.integer );
		trap_Cvar_Set( "capturelimit", "0" );
		trap_Cvar_Update( &g_capturelimit );
	}

	if ( (g_gametype.integer >= GT_CTF && g_ffa_gt<1) && g_capturelimit.integer ) {

		if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the scorelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the scorelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}
	}
}

//LMS - Last man Stading functions:
void StartLMSRound(void) {
	int		countsLiving;
	countsLiving = TeamLivingCount( -1, TEAM_FREE );
	if(countsLiving<2) {
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}

	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers

        
        
	SendEliminationMessageToAllClients();
	EnableWeapons();
}

//the elimination start function
void StartEliminationRound(void) {

	int		countsLiving[TEAM_NUM_TEAMS];
	int i;
	countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
	countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );
	if((countsLiving[TEAM_BLUE]==0) || (countsLiving[TEAM_RED]==0))
	{
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundRespawned = qfalse;
		//Remember that one of the teams is empty!
		level.roundRedPlayers = countsLiving[TEAM_RED];
		level.roundBluePlayers = countsLiving[TEAM_BLUE];
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}
	
	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers
	level.roundRedPlayers = countsLiving[TEAM_RED];
	level.roundBluePlayers = countsLiving[TEAM_BLUE];
	if(g_gametype.integer == GT_CTF_ELIMINATION) {
		Team_ReturnFlag( TEAM_RED );
		Team_ReturnFlag( TEAM_BLUE );
	}
        if(g_gametype.integer == GT_ELIMINATION) {
            G_LogPrintf( "ELIMINATION: %i %i %i: Round %i has started!\n", level.roundNumber, -1, 0, level.roundNumber );
        } else if(g_gametype.integer == GT_CTF_ELIMINATION) {
            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i has started!\n", level.roundNumber, -1, -1, 4, level.roundNumber );
        }
	SendEliminationMessageToAllClients();
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients(); //Ensure that evaryone know who should attack.
	EnableWeapons();
	G_SendLivingCount();
	
	for( i = 0; i < level.numConnectedClients ;i++ ) {
		 g_clients[i].elimRoundDamage = 0;
		 g_clients[i].elimRoundDamageTaken = 0;
		 g_clients[i].elimRoundKills = 0;
	}
	
}

void G_SendEliminationStats( void ) {
	int i;
	int maxdmgdone = 0, maxdmgdoneClientnum = -1, mindmgtaken = 1000, mindmgtakenClientnum = -1, maxkills = -1, maxkillsClientnum = -1;
	
	for( i = 0; i < level.numConnectedClients ;i++ ) {
	  
		if( g_clients[level.sortedClients[i]].sess.sessionTeam == TEAM_SPECTATOR )
			continue;
		
		if( g_clients[level.sortedClients[i]].elimRoundDamage > maxdmgdone ){
			maxdmgdone = g_clients[level.sortedClients[i]].elimRoundDamage;
			maxdmgdoneClientnum = level.sortedClients[i];
		}
		
		if( g_clients[level.sortedClients[i]].elimRoundDamageTaken < mindmgtaken ){
			mindmgtaken = g_clients[level.sortedClients[i]].elimRoundDamageTaken;
			mindmgtakenClientnum = level.sortedClients[i];
		}
		
		if( g_clients[level.sortedClients[i]].elimRoundKills > maxkills || ( g_clients[level.sortedClients[i]].elimRoundKills == maxkills && g_clients[level.sortedClients[i]].elimRoundDamage > g_clients[maxkillsClientnum].elimRoundDamage ) ){
			maxkills = g_clients[level.sortedClients[i]].elimRoundKills;
			maxkillsClientnum = level.sortedClients[i];
		}
	}
	
	if (maxdmgdoneClientnum == -1)
		maxdmgdoneClientnum = 0;
	trap_SendServerCommand( -1, va( "print \"Max damage done: %s ^2%i\n\"", g_clients[maxdmgdoneClientnum].pers.netname, g_clients[maxdmgdoneClientnum].elimRoundDamage ) );
	trap_SendServerCommand( -1, va( "print \"Min damage taken: %s ^1%i\n\"", g_clients[mindmgtakenClientnum].pers.netname, g_clients[mindmgtakenClientnum].elimRoundDamageTaken ) );
	trap_SendServerCommand( -1, va( "print \"Max kills: %s ^3%i\n\"", g_clients[maxkillsClientnum].pers.netname, g_clients[maxkillsClientnum].elimRoundKills ) );
	
	trap_SendServerCommand( -1, va( "screenPrint \"Max damage done: %s ^2%i\"", g_clients[maxdmgdoneClientnum].pers.netname, g_clients[maxdmgdoneClientnum].elimRoundDamage ) );
	trap_SendServerCommand( -1, va( "screenPrint \"Min damage taken: %s ^1%i\"", g_clients[mindmgtakenClientnum].pers.netname, g_clients[mindmgtakenClientnum].elimRoundDamageTaken ) );
	trap_SendServerCommand( -1, va( "screenPrint \"Max kills: %s ^3%i\"", g_clients[maxkillsClientnum].pers.netname, g_clients[maxkillsClientnum].elimRoundKills ) );
}

//things to do at end of round:
void EndEliminationRound(void)
{
	//int i,j;
	G_SendEliminationStats();
	DisableWeapons();
	level.roundNumber++;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
        CalculateRanks();
	level.roundRespawned = qfalse;
	
	/*for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED && level.clients[i].died ) {
			G_Printf("yayks\n");
			level.clients[i].died = qfalse;
			for( j = 0; j < MAX_PERSISTANT; j++ )
				level.clients[ i ].ps.persistant[i] = level.clients[ i ].preservedScore[i];
		}
	}*/
	
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
	G_SendLivingCount();
}

//Things to do if we don't want to move the roundNumber
void RestartEliminationRound(void) {
	
	//int i,j;
	
	DisableWeapons();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	
	/*for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED && level.clients[i].isEliminated ) {
			for( j = 0; j < MAX_PERSISTANT; j++ )
				level.clients[ i ].ps.persistant[i] = level.clients[ i ].preservedScore[i];
		}
	}*/
	
	
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

//Things to do during match warmup
void WarmupEliminationRound(void) {
	EnableWeapons();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/

/*
CheckDoubleDomination
*/

void CheckDoubleDomination( void ) {
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	if ( level.warmupTime != 0) {
            if( ((level.pointStatusA == TEAM_BLUE && level.pointStatusB == TEAM_BLUE) ||
                 (level.pointStatusA == TEAM_RED && level.pointStatusB == TEAM_RED)) &&
                    level.timeTaken + 10*1000 <= level.time ) {
                        Team_RemoveDoubleDominationPoints();
                        level.roundStartTime = level.time + 10*1000;
                        SendScoreboardMessageToAllClients();
            }
            return;
        }

	if(g_gametype.integer != GT_DOUBLE_D)
		return;

	//Don't score if we are in intermission. Both points might have been taken when we went into intermission
	if(level.intermissiontime)
		return;

	if(level.pointStatusA == TEAM_RED && level.pointStatusB == TEAM_RED && level.timeTaken + 10*1000 <= level.time) {
		//Red scores
		trap_SendServerCommand( -1, "print \"Red team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_RED,1);
                G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_RED, 2, TeamName(TEAM_RED) );
		Team_ForceGesture(TEAM_RED);
		Team_DD_bonusAtPoints(TEAM_RED);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + 10*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if(level.pointStatusA == TEAM_BLUE && level.pointStatusB == TEAM_BLUE && level.timeTaken + 10*1000 <= level.time) {
		//Blue scores
		trap_SendServerCommand( -1, "print \"Blue team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
                G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_BLUE, 2, TeamName(TEAM_BLUE) );
		Team_ForceGesture(TEAM_BLUE);
		Team_DD_bonusAtPoints(TEAM_BLUE);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + 10*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if((level.pointStatusA == TEAM_NONE || level.pointStatusB == TEAM_NONE) && level.time>level.roundStartTime) {
		trap_SendServerCommand( -1, "print \"A new round has started\n\"");
		Team_SpawnDoubleDominationPoints();
		SendScoreboardMessageToAllClients();
	}
}

/*
CheckLMS
*/

void CheckLMS(void) {
	int mode;
	mode = g_lms_mode.integer;
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	

	//We don't want to do anything in intermission
	if(level.intermissiontime) {
		if(level.roundRespawned) {
			RestartEliminationRound();
		}
		level.roundStartTime = level.time; //so that a player might join at any time to fix the bots+no humans+autojoin bug
		return;
	}

	if(g_gametype.integer == GT_LMS)
	{
		int		countsLiving[TEAM_NUM_TEAMS];
		//trap_SendServerCommand( -1, "print \"This is LMS!\n\"");
		countsLiving[TEAM_FREE] = TeamLivingCount( -1, TEAM_FREE );
		if(countsLiving[TEAM_FREE]==1 && level.roundNumber==level.roundNumberStarted)
		{
			if(mode <=1 )
			LMSpoint();
			trap_SendServerCommand( -1, "print \"We have a winner!\n\"");
			EndEliminationRound();
			Team_ForceGesture(TEAM_FREE);
		}

		if(countsLiving[TEAM_FREE]==0 && level.roundNumber==level.roundNumberStarted)
		{
			trap_SendServerCommand( -1, "print \"All death... how sad\n\"");
			EndEliminationRound();
		}

		if((level.roundNumber==level.roundNumberStarted)&&(g_lms_mode.integer == 1 || g_lms_mode.integer==3)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
		{
			trap_SendServerCommand( -1, "print \"Time up - Overtime disabled\n\"");
			if(mode <=1 )
			LMSpoint();
			EndEliminationRound();
		}

		//This might be better placed another place:
		if(g_elimination_activewarmup.integer<2)
			g_elimination_activewarmup.integer=2; //We need at least 2 seconds to spawn all players
		if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
			g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

		//Force respawn
		if(level.roundNumber != level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
		{
			level.roundRespawned = qtrue;
			RespawnAll();
			DisableWeapons();
			SendEliminationMessageToAllClients();
		}

		if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
		{
			RespawnDead();
			//DisableWeapons();
		}

		if(level.roundNumber == level.roundNumberStarted)
		{
			EnableWeapons();
		}

		if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime))
			StartLMSRound();
	
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime && level.numPlayingClients < 2)
		{
			RespawnDead(); //Allow player to run around anyway
			WarmupEliminationRound(); //Start over
			return;
		}

		if(level.warmupTime != 0) {
			if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
			{
				RespawnDead();
				WarmupEliminationRound();
			}
		}

	}
}

/*
=============
CheckElimination
=============
*/
void CheckElimination(void) {
	if ( level.numPlayingClients < 1 ) {
		if( (g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) &&
			( level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime ))
			RestartEliminationRound(); //For spectators
		return;
	}	

	//We don't want to do anything in itnermission
	if(level.intermissiontime) {
		if(level.roundRespawned)
			RestartEliminationRound();
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}	

	if(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION)
	{
		int		counts[TEAM_NUM_TEAMS];
		int		countsLiving[TEAM_NUM_TEAMS];
		int		countsHealth[TEAM_NUM_TEAMS];
		counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
		counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

		countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
		countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );

		countsHealth[TEAM_BLUE] = TeamHealthCount( -1, TEAM_BLUE );
		countsHealth[TEAM_RED] = TeamHealthCount( -1, TEAM_RED );

		if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) { //Cannot score if one of the team never got any living players
			if((countsLiving[TEAM_BLUE]==0)&&(level.roundNumber==level.roundNumberStarted))
			{
				//Blue team has been eliminated!
				trap_SendServerCommand( -1, "print \"Blue Team eliminated!\n\"");
				
				if( g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer != 0 ){
				  if( (level.eliminationSides+level.roundNumber)%2 == 0 ){
				      AddTeamScore(level.intermission_origin, TEAM_RED, 2);
				  }
				}
				else {
				  AddTeamScore(level.intermission_origin,TEAM_RED,1);
				}
				
                                if(g_gametype.integer == GT_ELIMINATION) {
                                    G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_RED, 1, TeamName(TEAM_RED), level.roundNumber );
                                } else {
                                    G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_RED, 6, TeamName(TEAM_RED), level.roundNumber );
                                }
				EndEliminationRound();
				Team_ForceGesture(TEAM_RED);
			}
			else if((countsLiving[TEAM_RED]==0)&&(level.roundNumber==level.roundNumberStarted))
			{
				//Red team eliminated!
				trap_SendServerCommand( -1, "print \"Red Team eliminated!\n\"");
				
				if( g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer != 0 ){
				  if( (level.eliminationSides+level.roundNumber)%2 != 0 ){
				      AddTeamScore(level.intermission_origin, TEAM_BLUE, 2);
				  }
				}
				else {
				  AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
				}
				
                                if(g_gametype.integer == GT_ELIMINATION) {
                                    G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_BLUE, 1, TeamName(TEAM_BLUE), level.roundNumber );
                                } else {
                                    G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_BLUE, 6, TeamName(TEAM_BLUE), level.roundNumber );
                                }
				EndEliminationRound();
				Team_ForceGesture(TEAM_BLUE);
			}
		}

		//Time up
		if((level.roundNumber==level.roundNumberStarted)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
		{
			trap_SendServerCommand( -1, "print \"No teams eliminated!\n\"");

			if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) {//We don't want to divide by zero. (should not be possible)
				if(g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer) {
					//One way CTF, make defensice team the winner.
					/*if ( (level.eliminationSides+level.roundNumber)%2 == 0 ) { //Red was attacking
						trap_SendServerCommand( -1, "print \"Blue team defended the base\n\"");
						AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
					}
					else {
						trap_SendServerCommand( -1, "print \"Red team defended the base\n\"");
						AddTeamScore(level.intermission_origin,TEAM_RED,1);
					}*/
				}
				else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)>((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
				{
					//Red team has higher procentage survivors
					trap_SendServerCommand( -1, "print \"Red team has most survivers!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_RED,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_RED, 2, TeamName(TEAM_RED), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_RED, 7, TeamName(TEAM_RED), level.roundNumber );
                                        }
				}
				else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)<((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
				{
					//Blue team has higher procentage survivors
					trap_SendServerCommand( -1, "print \"Blue team has most survivers!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_BLUE,1);	
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_BLUE, 2, TeamName(TEAM_BLUE), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_BLUE, 7, TeamName(TEAM_BLUE), level.roundNumber );
                                        }
				}
				else if(countsHealth[TEAM_RED]>countsHealth[TEAM_BLUE])
				{
					//Red team has more health
					trap_SendServerCommand( -1, "print \"Red team has more health left!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_RED,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_RED, 3, TeamName(TEAM_RED), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_RED, 8, TeamName(TEAM_RED), level.roundNumber );
                                        }
				}
				else if(countsHealth[TEAM_RED]<countsHealth[TEAM_BLUE])
				{
					//Blue team has more health
					trap_SendServerCommand( -1, "print \"Blue team has more health left!\n\"");
					AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
                                        if(g_gametype.integer == GT_ELIMINATION) {
                                            G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_BLUE, 3, TeamName(TEAM_BLUE), level.roundNumber );
                                        } else {
                                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_BLUE, 8, TeamName(TEAM_BLUE), level.roundNumber );
                                        }
				}
			}
                        //Draw
                        if(g_gametype.integer == GT_ELIMINATION) {
                            G_LogPrintf( "ELIMINATION: %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, 4, level.roundNumber );
                        } else {
                            G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, -1, 9, level.roundNumber );
                        }
			EndEliminationRound();
		}

		//This might be better placed another place:
		if(g_elimination_activewarmup.integer<1)
			g_elimination_activewarmup.integer=1; //We need at least 1 second to spawn all players
		if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
			g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

		//Force respawn
		if(level.roundNumber!=level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
		{
			level.roundRespawned = qtrue;
			RespawnAll();
			SendEliminationMessageToAllClients();
			if( g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer != 0 )
				SendCaptureStrikeMessage();
		}

		if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
		{
			RespawnDead();
		}
			

		if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime)){
			StartEliminationRound();
			
		}
	
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
		if(counts[TEAM_BLUE]<1 || counts[TEAM_RED]<1)
		{
			RespawnDead(); //Allow players to run around anyway
			WarmupEliminationRound(); //Start over
			return;
		}

		if(level.warmupTime != 0) {
			if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
			{
				RespawnDead();
				WarmupEliminationRound();
			}
		}
	}
}

/*
=============
CheckDomination
=============
*/
void CheckDomination(void) {
	int i;
        int scoreFactor = 1;

	if ( (level.numPlayingClients < 1) || (g_gametype.integer != GT_DOMINATION) ) {
		return;
	}

	//Do nothing if warmup
	if(level.warmupTime != 0)
		return; 

	//Don't score if we are in intermission. Just plain stupid
	if(level.intermissiontime)
		return;

	//Sago: I use if instead of while, since if the server stops for more than 2 seconds people should not be allowed to score anyway
	if(level.domination_points_count>3)
            scoreFactor = 2; //score more slowly if there are many points
        if(level.time>=level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor) {
		for(i=0;i<level.domination_points_count;i++) {
			if ( level.pointStatusDom[i] == TEAM_RED )
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
			if ( level.pointStatusDom[i] == TEAM_BLUE )
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
		}
		level.dom_scoreGiven++;
		while(level.time>level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor)
			level.dom_scoreGiven++;
		CalculateRanks();
	}
}

/*
=============
CheckTournament

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournament( void ) {
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( level.numPlayingClients == 0 ) {
		return;
	}

	if ( g_gametype.integer == GT_TOURNAMENT ) {

		// pull in a spectator if needed
		if ( level.numPlayingClients < 2 ) {
			level.timeComplete = level.time;
			AddTournamentPlayer();
		}

		// if we don't have two players, go back to "waiting for players"
		if ( level.numPlayingClients != 2 ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return;
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			if ( level.numPlayingClients == 2 ) {
				if( ( g_startWhenReady.integer && 
				  ( g_entities[level.sortedClients[0]].client->ready || ( g_entities[level.sortedClients[0]].r.svFlags & SVF_BOT ) ) && 
				  ( g_entities[level.sortedClients[1]].client->ready || ( g_entities[level.sortedClients[1]].r.svFlags & SVF_BOT ) ) ) || 
				  !g_startWhenReady.integer || !g_doWarmup.integer || ( g_autoReady.integer > 0 && g_autoReady.integer*1000 < ( level.time - level.timeComplete ) ) ){
					// fudge by -1 to account for extra delays
					if ( g_warmup.integer > 1 ) {
						level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
					} else {
						level.warmupTime = 0;
					}
					
					trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
					G_SendStartGame();
					G_SetGameString();
					G_StartServerDemos();
				}
			}
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			//G_StartServerDemo();
			return;
		}
	} else if ( g_gametype.integer != GT_SINGLE_PLAYER && level.warmupTime != 0 ) {
		int		counts[TEAM_NUM_TEAMS];
		qboolean	notEnough = qfalse;
		int i, clientsReady = 0;

		if ( g_gametype.integer > GT_TEAM && !g_ffa_gt ) {
			counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

			if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1) {
				notEnough = qtrue;
				level.timeComplete = level.time;
			}
		} else if ( level.numPlayingClients < 2 ) {
			notEnough = qtrue;
			level.timeComplete = level.time;
		}
		
		if( g_startWhenReady.integer ){
			for( i = 0; i < level.numPlayingClients; i++ ){
				if( ( g_entities[level.sortedClients[i]].client->ready || g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) && g_entities[level.sortedClients[i]].inuse )
					clientsReady++;
			}
		}
		
		if( g_doWarmup.integer && g_startWhenReady.integer == 1 && ( clientsReady < level.numPlayingClients/2 + 1 ) && !( g_autoReady.integer > 0 && g_autoReady.integer*1000 < ( level.time - level.timeComplete ) ) )
			notEnough = qtrue;
		if( g_doWarmup.integer && g_startWhenReady.integer == 2 && ( clientsReady < level.numPlayingClients ) && !( g_autoReady.integer > 0 && g_autoReady.integer*1000 < ( level.time - level.timeComplete ) ) )
			notEnough = qtrue;
		

		if ( notEnough ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return; // still waiting for team members
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			// fudge by -1 to account for extra delays
			if ( g_warmup.integer > 1 ) {
				level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
			} else {
				 level.warmupTime = 0;
			}
			trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			G_SendStartGame();
			G_SetGameString();
			G_StartServerDemos();
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			//G_StartServerDemo();
			return;
		}
	}
}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char *message) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		trap_SendServerCommand( i, message );
	}
}

/*
==================
SetLeader
==================
*/
void SetLeader(int team, int client) {
	int i;

	if ( level.clients[client].pers.connected == CON_DISCONNECTED ) {
		PrintTeam(team, va("print \"%s is not connected\n\"", level.clients[client].pers.netname) );
		return;
	}
	if (level.clients[client].sess.sessionTeam != team) {
		PrintTeam(team, va("print \"%s is not on the team anymore\n\"", level.clients[client].pers.netname) );
		return;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader) {
			level.clients[i].sess.teamLeader = qfalse;
			ClientUserinfoChanged(i);
		}
	}
	level.clients[client].sess.teamLeader = qtrue;
	ClientUserinfoChanged( client );
	PrintTeam(team, va("print \"%s is the new team leader\n\"", level.clients[client].pers.netname) );
}

/*
==================
CheckTeamLeader
==================
*/
void CheckTeamLeader( int team ) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
			break;
	}
	if (i >= level.maxclients) {
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			if (!(g_entities[i].r.svFlags & SVF_BOT)) {
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}

		if (i >= level.maxclients) {
			for ( i = 0 ; i < level.maxclients ; i++ ) {
				if (level.clients[i].sess.sessionTeam != team)
					continue;
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}
	}
}

/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( int team ) {
	int cs_offset;

	if ( team == TEAM_RED )
		cs_offset = 0;
	else if ( team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !level.teamVoteTime[cs_offset] ) {
		return;
	}
	if ( level.time - level.teamVoteTime[cs_offset] >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
	} else {
		if ( level.teamVoteYes[cs_offset] > level.numteamVotingClients[cs_offset]/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Team vote passed.\n\"" );
			//
			if ( !Q_strncmp( "leader", level.teamVoteString[cs_offset], 6) ) {
				//set the team leader
				SetLeader(team, atoi(level.teamVoteString[cs_offset] + 7));
			}
			else {
				trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.teamVoteString[cs_offset] ) );
			}
		} else if ( level.teamVoteNo[cs_offset] >= level.numteamVotingClients[cs_offset]/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
		} else {
			// still waiting for a majority
			return;
		}
	}
	level.teamVoteTime[cs_offset] = 0;
	trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );

}


/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
==================
CheckDemo
==================
*/
/*void CheckDemo( void )
{
  int i;
  if( level.time - level.demoStarted < 1000 )
    return;
  // Don't do anything if no change
  if ( g_demoState.integer == level.demoState )
    return;

  // log all connected clients
  if ( g_demoState.integer == DS_RECORDING )
  {
    for ( i = 0; i < level.maxclients; i++ )
    {
      char buffer[ MAX_INFO_STRING ] = "";
      if ( level.clients[ i ].pers.connected == CON_CONNECTED )
      {
	G_Printf("DemoAdd %i %s\n", i, level.clients[ i ].pers.netname );
        Info_SetValueForKey( buffer, "name", level.clients[ i ].pers.netname );
        Info_SetValueForKey( buffer, "ip", level.clients[ i ].pers.ip );
        Info_SetValueForKey( buffer, "team", va( "%d", level.clients[ i ].sess.sessionTeam ) );
        G_DemoCommand( DC_CLIENT_SET, va( "%d %s", i, buffer ) );
      }
    }
  }

  // empty teams and display a message
  else if ( g_demoState.integer == DS_PLAYBACK )
  {
    trap_SendServerCommand( -1, "print \"A demo has been started on the server.\n\"" );
    for ( i = 0; i < level.maxclients; i++ )
    {
      if ( level.clients[ i ].sess.sessionTeam != TEAM_SPECTATOR )
        SetTeam( g_entities + i, "spectator" );
    }
  }
  
  level.demoState = g_demoState.integer;
}*/


/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent) {
	int	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}
	
	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime ) {
	int			i;
	gentity_t	*ent;
	int			msec;
	int start, end;

	/*for( i = 0; i < level.numConnectedClients; i++ ){
		G_Printf("%s,%i,%i\n", level.clients[level.sortedClients[i]].pers.netname, level.clients[level.sortedClients[i]].sess.sessionTeam, level.clients[level.sortedClients[i]].pers.demoClient );
	}*/
	
	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	} 
	
	if( g_autoRestart.integer && level.numConnectedClients == 0 && g_timelimit.integer && ( ( level.time - level.startTime ) > g_timelimit.integer*60000 ) ){
		G_Printf("AUTORESTART:timelimit hit, restarting map\n");
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
	}
	
	/*if( level.timeout && ( ( levelTime- level.timeoutAdd ) < level.timeoutTime ) )
		return;*/
	level.framenum++;
	level.previousTime = level.time;
		
	if( !(level.timeout && ( ( levelTime- level.timeoutAdd ) < level.timeoutTime ) ) ){
		level.time = levelTime;
	}
	
	if( (level.timeout && ( ( levelTime- level.timeoutAdd ) > level.timeoutTime ) ) ){
		G_Timein();
	}
	
	msec = level.time - level.previousTime;

	// get any cvar changes
	G_UpdateCvars();
	
	// check demo state
	//CheckDemo( );
	
	if( level.timeout )
		return;

        if( (g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION) && !(g_elimflags.integer & EF_NO_FREESPEC) && g_elimination_lockspectator.integer>1)
            trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer|EF_NO_FREESPEC));
        else
        if( (g_elimflags.integer & EF_NO_FREESPEC) && g_elimination_lockspectator.integer<2)
            trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer&(~EF_NO_FREESPEC) ) );

        if( g_elimination_ctf_oneway.integer && !(g_elimflags.integer & EF_ONEWAY) ) {
            trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer|EF_ONEWAY ) );
            //If the server admin has enabled it midgame imidiantly braodcast attacking team
            SendAttackingTeamMessageToAllClients();
        }
        else
        if( !g_elimination_ctf_oneway.integer && (g_elimflags.integer & EF_ONEWAY) ) {
            trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer&(~EF_ONEWAY) ) );
        }
        
        if( g_redLocked.integer && ( TeamCount( -1, TEAM_RED ) == 0 ) && (level.time-level.startTime) > 1000){
		trap_Cvar_Set("g_redLocked","0");
	}
	
	if( g_blueLocked.integer && ( TeamCount( -1, TEAM_BLUE ) == 0 ) && (level.time-level.startTime) > 1000){
		trap_Cvar_Set("g_blueLocked","0");
	}
	
	if( ( ( level.warmupTime == 0 ) && ( level.time - level.startTime > 2000 ) && !level.intermissionQueued && ( g_gametype.integer != GT_FFA ) ) && g_doWarmup.integer ){
		//G_Printf("%i \n", g_gametype.integer );
		//G_Printf("%i %i %i\n", level.numPlayingClients, TeamCount( -1, TEAM_RED ), TeamCount( -1, TEAM_BLUE ) );
		if ( ( level.numPlayingClients < 2 ) && ( g_gametype.integer < GT_TEAM ) ) {
			LogExit("opponent left");
		}
		
		if( ( g_gametype.integer >= GT_TEAM ) && ( TeamCount( -1, TEAM_RED ) == 0 ) ){
		//	G_Printf("red");
			LogExit("Red team left");
		}
		else if( ( g_gametype.integer >= GT_TEAM ) && ( TeamCount( -1, TEAM_BLUE ) == 0 ) ){
		//	G_Printf("blue");
			LogExit("Blue team left");
		}
	}	

	//
	// go through all allocated objects
	//
	start = trap_Milliseconds();
	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;	// &= EV_EVENT_BITS;
				if ( ent->client ) {
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			} else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

//unlagged - backward reconciliation #2
		// we'll run missiles separately to save CPU in backward reconciliation
/*
		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
			continue;
		}
*/
//unlagged - backward reconciliation #2

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER ) {
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS ) {
			G_RunClient( ent );
			continue;
		}

		G_RunThink( ent );
	}

//unlagged - backward reconciliation #2
	// NOW run the missiles, with all players backward-reconciled
	// to the positions they were in exactly 50ms ago, at the end
	// of the last server frame
	
	G_TimeShiftAllClients( level.previousTime, NULL );

	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
		}
	}

	G_UnTimeShiftAllClients( NULL );
//unlagged - backward reconciliation #2

end = trap_Milliseconds();

start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for (i=0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
		}
	}
end = trap_Milliseconds();

	// see if it is time to do a tournement restart
	CheckTournament();

	//Check Elimination state
	CheckElimination();
	CheckLMS();

	//Check Double Domination
	CheckDoubleDomination();

	CheckDomination();

	//Sago: I just need to think why I placed this here... they should only spawn once
	if(g_gametype.integer == GT_DOMINATION)
		Team_Dom_SpawnPoints();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();
	
	TeamplaySpectatorMessage();

	// cancel vote if timed out
	CheckVote();

	// check team votes
	CheckTeamVote( TEAM_RED );
	CheckTeamVote( TEAM_BLUE );

	// for tracking changes
	CheckCvars();

	if (g_listEntity.integer) {
		for (i = 0; i < MAX_GENTITIES; i++) {
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}

//unlagged - backward reconciliation #4
	// record the time at the end of this frame - it should be about
	// the time the next frame begins - when the server starts
	// accepting commands from connected clients
	level.frameStartTime = trap_Milliseconds();
//unlagged - backward reconciliation #4
}

