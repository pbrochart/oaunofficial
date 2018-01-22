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
// cg_weapons.c -- events and effects dealing with weapons
#include "cg_local.h"

/*
==========================
CG_MachineGunEjectBrass
==========================
*/
static void CG_MachineGunEjectBrass( centity_t *cent ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	vec3_t			offset, xoffset;
	float			waterScale = 1.0f;
	vec3_t			v[3];

	if ( cg_brassTime.integer <= 0 ) {
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 0;
	velocity[1] = -50 + 40 * crandom();
	velocity[2] = 100 + 50 * crandom();

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - (rand()&15);

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 8;
	offset[1] = -4;
	offset[2] = 24;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, re->origin );

	VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
	re->hModel = cgs.media.machinegunBrassModel;

	le->bounceFactor = 0.4 * waterScale;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;

	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_BRASS;
	le->leMarkType = LEMT_NONE;
}

/*
==========================
CG_ShotgunEjectBrass
==========================
*/
static void CG_ShotgunEjectBrass( centity_t *cent ) {
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	vec3_t			offset, xoffset;
	vec3_t			v[3];
	int				i;

	if ( cg_brassTime.integer <= 0 ) {
		return;
	}

	for ( i = 0; i < 2; i++ ) {
		float	waterScale = 1.0f;

		le = CG_AllocLocalEntity();
		re = &le->refEntity;

		velocity[0] = 60 + 60 * crandom();
		if ( i == 0 ) {
			velocity[1] = 40 + 10 * crandom();
		} else {
			velocity[1] = -40 + 10 * crandom();
		}
		velocity[2] = 100 + 50 * crandom();

		le->leType = LE_FRAGMENT;
		le->startTime = cg.time;
		le->endTime = le->startTime + cg_brassTime.integer*3 + cg_brassTime.integer * random();

		le->pos.trType = TR_GRAVITY;
		le->pos.trTime = cg.time;

		AnglesToAxis( cent->lerpAngles, v );

		offset[0] = 8;
		offset[1] = 0;
		offset[2] = 24;

		xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
		xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
		xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
		VectorAdd( cent->lerpOrigin, xoffset, re->origin );
		VectorCopy( re->origin, le->pos.trBase );
		if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
			waterScale = 0.10f;
		}

		xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
		xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
		xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
		VectorScale( xvelocity, waterScale, le->pos.trDelta );

		AxisCopy( axisDefault, re->axis );
		re->hModel = cgs.media.shotgunBrassModel;
		le->bounceFactor = 0.3f;

		le->angles.trType = TR_LINEAR;
		le->angles.trTime = cg.time;
		le->angles.trBase[0] = rand()&31;
		le->angles.trBase[1] = rand()&31;
		le->angles.trBase[2] = rand()&31;
		le->angles.trDelta[0] = 1;
		le->angles.trDelta[1] = 0.5;
		le->angles.trDelta[2] = 0;

		le->leFlags = LEF_TUMBLE;
		le->leBounceSoundType = LEBS_BRASS;
		le->leMarkType = LEMT_NONE;
	}
}


#ifdef MISSIONPACK
/*
==========================
CG_NailgunEjectBrass
==========================
*/
static void CG_NailgunEjectBrass( centity_t *cent ) {
	localEntity_t	*smoke;
	vec3_t			origin;
	vec3_t			v[3];
	vec3_t			offset;
	vec3_t			xoffset;
	vec3_t			up;

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 0;
	offset[1] = -12;
	offset[2] = 24;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, origin );

	VectorSet( up, 0, 0, 64 );

	smoke = CG_SmokePuff( origin, up, 32, 1, 1, 1, 0.33f, 700, cg.time, 0, 0, cgs.media.smokePuffShader );
	// use the optimized local entity add
	smoke->leType = LE_SCALE_FADE;
}
#endif


/*
==========================
CG_RailTrail
==========================
*/
void CG_RailTrail (clientInfo_t *ci, vec3_t start, vec3_t end) {
	vec3_t axis[36], move, move2,/* next_move,*/ vec, temp;
	float  len;
	int    i, j, skip;
 
	localEntity_t *le;
	refEntity_t   *re;

	clientInfo_t *localPlayer;
  	localPlayer = &cgs.clientinfo[cg.clientNum];
 
#define RADIUS   4
#define ROTATION 1
#define SPACING  5


	start[2] -= 4;
 
	le = CG_AllocLocalEntity();
	re = &le->refEntity;
 
	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + cg_railTrailTime.value;
	le->lifeRate = 1.0 / (le->endTime - le->startTime);
 
	re->shaderTime = cg.time / 1000.0f;
	re->reType = RT_RAIL_CORE;
	re->customShader = cgs.media.railCoreShader;
 
	VectorCopy(start, re->origin);
	VectorCopy(end, re->oldorigin);

	if( cg_forceWeaponColor.integer & 16 ){
		
		if( cgs.gametype >= GT_TEAM && cgs.ffa_gt != 1 ){
			if( localPlayer->team != ci->team ){
				CG_setRGBA(re->shaderRGBA, cg_enemyWeaponColor.string);
				/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
				re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
				re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
				re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/
	
				le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
				le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
				le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
				le->color[3] = ((float)re->shaderRGBA[3])/255.0;
			}
			else{
				CG_setRGBA(re->shaderRGBA, cg_teamWeaponColor.string);
				/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
				re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
				re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
				re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/
	
				le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
				le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
				le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
				le->color[3] = ((float)re->shaderRGBA[3])/255.0;
			}
		}
		else if( localPlayer != ci ){
			CG_setRGBA(re->shaderRGBA, cg_enemyWeaponColor.string);
			/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
			re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
			re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
			re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

			le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
			le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
			le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
			le->color[3] = ((float)re->shaderRGBA[3])/255.0;
		}
		else{
			CG_setRGBA(re->shaderRGBA, cg_teamWeaponColor.string);
			/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
			re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
			re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
			re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

			le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
			le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
			le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
			le->color[3] = ((float)re->shaderRGBA[3])/255.0;
		}
	}
	else{
		re->shaderRGBA[0] = ci->color1[0] * 255;
		re->shaderRGBA[1] = ci->color1[1] * 255;
		re->shaderRGBA[2] = ci->color1[2] * 255;
		re->shaderRGBA[3] = 255;

		le->color[0] = ci->color1[0] * 0.75;
		le->color[1] = ci->color1[1] * 0.75;
		le->color[2] = ci->color1[2] * 0.75;
		le->color[3] = 1.0f;
	}
	AxisClear( re->axis );
 
	if (cg_oldRail.integer)
	{
		// nudge down a bit so it isn't exactly in center
		/*re->origin[2] -= 8;
		re->oldorigin[2] -= 8;*/
		return;
	}

	VectorCopy (start, move);
	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);
	PerpendicularVector(temp, vec);
	for (i = 0 ; i < 36; i++)
	{
		RotatePointAroundVector(axis[i], vec, temp, i * 10);//banshee 2.4 was 10
	}

	VectorMA(move, 20, vec, move);
	//VectorCopy(move, next_move);
	VectorScale (vec, SPACING, vec);

	skip = -1;
 
	j = 18;
	for (i = 0; i < len; i += SPACING)
	{
		if (i != skip)
		{
			skip = i + SPACING;
			le = CG_AllocLocalEntity();
			re = &le->refEntity;
			le->leFlags = LEF_PUFF_DONT_SCALE;
			le->leType = LE_MOVE_SCALE_FADE;
			le->startTime = cg.time;
			le->endTime = cg.time + (i>>1) + 600;
			le->lifeRate = 1.0 / (le->endTime - le->startTime);

			re->shaderTime = cg.time / 1000.0f;
			re->reType = RT_SPRITE;
			re->radius = 1.1f;
			re->customShader = cgs.media.railRingsShader;

			if( cg_forceWeaponColor.integer & 16 ){
		
				if( cgs.gametype >= GT_TEAM && cgs.ffa_gt != 1 ){
					if( localPlayer->team != ci->team ){
						CG_setRGBA(re->shaderRGBA, cg_enemyWeaponColor.string);
						/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
						re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
						re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
						re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

						le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
						le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
						le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
						le->color[3] = ((float)re->shaderRGBA[3])/255.0;
					}
					else{
						CG_setRGBA(re->shaderRGBA, cg_teamWeaponColor.string);
						/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
						re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
						re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
						re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

						le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
						le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
						le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
						le->color[3] = ((float)re->shaderRGBA[3])/255.0;
					}
				}
				else if( localPlayer != ci ){
					CG_setRGBA(re->shaderRGBA, cg_enemyWeaponColor.string);
					/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
					re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
					re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
					re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

					le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
					le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
					le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
					le->color[3] = ((float)re->shaderRGBA[3])/255.0;
				}
				else{
					CG_setRGBA(re->shaderRGBA, cg_teamWeaponColor.string);
					/*re->shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
					re->shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
					re->shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
					re->shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/

					le->color[0] = ((float)re->shaderRGBA[0])/255.0 * 0.75;
					le->color[1] = ((float)re->shaderRGBA[1])/255.0 * 0.75;
					le->color[2] = ((float)re->shaderRGBA[2])/255.0 * 0.75;
					le->color[3] = ((float)re->shaderRGBA[3])/255.0;
				}
			}
			else{
				re->shaderRGBA[0] = ci->color2[0] * 255;
				re->shaderRGBA[1] = ci->color2[1] * 255;
				re->shaderRGBA[2] = ci->color2[2] * 255;
				re->shaderRGBA[3] = 255;
		
				le->color[0] = ci->color2[0] * 0.75;
				le->color[1] = ci->color2[1] * 0.75;
				le->color[2] = ci->color2[2] * 0.75;
				le->color[3] = 1.0f;
			}

			le->pos.trType = TR_LINEAR;
			le->pos.trTime = cg.time;

			VectorCopy( move, move2);
			VectorMA(move2, RADIUS , axis[j], move2);
			VectorCopy(move2, le->pos.trBase);

			le->pos.trDelta[0] = axis[j][0]*6;
			le->pos.trDelta[1] = axis[j][1]*6;
			le->pos.trDelta[2] = axis[j][2]*6;
		}

		VectorAdd (move, vec, move);

		j = (j + ROTATION) % 36;
	}
}

/*
==========================
CG_RocketTrail
==========================
*/
static void CG_RocketTrail( centity_t *ent, const weaponInfo_t *wi ) {
	int		step;
	vec3_t	origin, lastPos, lastPosSaved;
	int		t;
	int		startTime, contents;
	int		lastContents;
	entityState_t	*es;
	vec3_t	up;
	localEntity_t	*smoke;
	float		red, green, blue, alpha;
	localEntity_t *le;
	refEntity_t   *re;
	clientInfo_t *local, *other;
	byte color[4];
	
	//if( ( ( wi->item->giTag == WP_GRENADE_LAUNCHER ) && ( cg_forceWeaponColor.integer & 2 ) && ( cg_grenadeTrail.integer == TRAIL_TRACER ) ) || 
	//    ( ( wi->item->giTag == WP_ROCKET_LAUNCHER ) && ( cg_forceWeaponColor.integer & 4 ) && ( cg_rocketTrail.integer == TRAIL_TRACER ) ) ){
	  
	//	le = CG_AllocLocalEntity();
	//	re = &le->refEntity;
	//}
	
	if ( cg_noProjectileTrail.integer ) {
		return;
	}
	
  	local = &cgs.clientinfo[cg.clientNum];
	other = &cgs.clientinfo[cg_entities[ent->currentState.otherEntityNum].currentState.number];

	if( ( ( wi->item->giTag == WP_GRENADE_LAUNCHER ) && ( cg_forceWeaponColor.integer & 2 ) ) || ( ( wi->item->giTag == WP_ROCKET_LAUNCHER ) && ( cg_forceWeaponColor.integer & 4 ) ) ){
		if( cgs.gametype >= GT_TEAM && cgs.ffa_gt != 1 ){
			if( local->team != other->team ){
				CG_setRGBA(color,cg_enemyWeaponColor.string);
				/*red = ((float)hexToRed( cg_enemyWeaponColor.string ))/255.0f;
				green = ((float)hexToGreen( cg_enemyWeaponColor.string ))/255.0f;
				blue = ((float)hexToBlue( cg_enemyWeaponColor.string ))/255.0f;*/
			}
			else{
				CG_setRGBA(color,cg_teamWeaponColor.string);
				/*red = ((float)hexToRed( cg_teamWeaponColor.string ))/255.0f;
				green = ((float)hexToGreen( cg_teamWeaponColor.string ))/255.0f;
				blue = ((float)hexToBlue( cg_teamWeaponColor.string ))/255.0f;*/
			}
		}
		else if( cg.clientNum != cg_entities[ent->currentState.otherEntityNum].currentState.number ){
			CG_setRGBA(color,cg_enemyWeaponColor.string);
			/*red = ((float)hexToRed( cg_enemyWeaponColor.string ))/255.0f;
			green = ((float)hexToGreen( cg_enemyWeaponColor.string ))/255.0f;
			blue = ((float)hexToBlue( cg_enemyWeaponColor.string ))/255.0f;*/
		}
		else{
			CG_setRGBA(color,cg_teamWeaponColor.string);
			/*red = ((float)hexToRed( cg_teamWeaponColor.string ))/255.0f;
			green = ((float)hexToGreen( cg_teamWeaponColor.string ))/255.0f;
			blue = ((float)hexToBlue( cg_teamWeaponColor.string ))/255.0f;*/
		}
	}
	else{
		color[0] = 1;
		color[1] = 1;
		color[2] = 1;
	}
	
	red = ((float)color[0])/255.0f;
	green = ((float)color[1])/255.0f;
	blue = ((float)color[2])/255.0f;
	alpha = 0.33f;

	up[0] = 0;
	up[1] = 0;
	up[2] = 0;

	step = 20;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
	lastContents = CG_PointContents( lastPos, -1 );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		if ( contents & lastContents & CONTENTS_WATER ) {
			CG_BubbleTrail( lastPos, origin, 8 );
		}
		return;
	}
	BG_EvaluateTrajectory( &es->pos, t, lastPos );
	for ( ; t <= ent->trailTime ; t += step ) {
		BG_EvaluateTrajectory( &es->pos, t+step, lastPosSaved );
		BG_EvaluateTrajectory( &es->pos, t, lastPos );
		if( wi->item->giTag == WP_GRENADE_LAUNCHER ){
			if( cg_grenadeTrail.integer == TRAIL_SMOKE ){
				smoke = CG_SmokePuff( lastPos, up, 
					  cg_grenadeTrailRadius.integer, 
					  red, green, blue, alpha,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.smokePuffShader );
				// use the optimized local entity add
				smoke->leType = LE_SCALE_FADE;
			} else if( cg_grenadeTrail.integer == TRAIL_PARTICLESMOKE ) {
				smoke = CG_SmokePuff( lastPos, up, 
					  4, 
					  red, green, blue, alpha,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.smokePuffShader );
				// use the optimized local entity add
				smoke->leType = LE_SCALE_FADE;
				CG_LaunchParticleTrail(lastPos );
			} else if( cg_grenadeTrail.integer == TRAIL_PARTICLE ) {
				CG_LaunchParticleTrail(lastPos );
			} else if( cg_grenadeTrail.integer == TRAIL_TRACER ){
				le = CG_AllocLocalEntity();
				re = &le->refEntity;
 
				le->leType = LE_FADE_RGB;
				le->startTime = cg.time;
				le->endTime = cg.time + 300;
				le->lifeRate = 1.0 / (le->endTime - le->startTime);
 
				re->shaderTime = cg.time / 1000.0f;
				re->reType = RT_RAIL_CORE;
				re->customShader = cgs.media.railCoreShader;
				
				if( !( ( wi->item->giTag == WP_GRENADE_LAUNCHER ) && ( cg_forceWeaponColor.integer & 2 ) ) ){
				
					re->shaderRGBA[0] = 1.0 * 255;
					re->shaderRGBA[1] = 1.0 * 255;
					re->shaderRGBA[2] = 0.0 * 255;
					re->shaderRGBA[3] = 255;

				} else {
					re->shaderRGBA[0] = color[0];
					re->shaderRGBA[1] = color[1];
					re->shaderRGBA[2] = color[2];
					re->shaderRGBA[3] = 255;
				}
				
				le->color[0] = ((float)re->shaderRGBA[0])/255.0f;
				le->color[1] = ((float)re->shaderRGBA[1])/255.0f;
				le->color[2] = ((float)re->shaderRGBA[2])/255.0f;
				le->color[3] = ((float)re->shaderRGBA[3])/255.0f;
 
				VectorCopy(lastPosSaved, re->origin);
				VectorCopy(lastPos, re->oldorigin);
				
			}
		}
		else if( wi->item->giTag == WP_ROCKET_LAUNCHER ){
			if( cg_rocketTrail.integer == TRAIL_SMOKE ){
				smoke = CG_SmokePuff( lastPos, up, 
					  cg_rocketTrailRadius.integer, 
					  red, green, blue, alpha,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.smokePuffShader );
				// use the optimized local entity add
				smoke->leType = LE_SCALE_FADE;
			} else if( cg_rocketTrail.integer == TRAIL_PARTICLESMOKE ) {
				smoke = CG_SmokePuff( lastPos, up, 
					  4, 
					  red, green, blue, alpha,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.smokePuffShader );
				// use the optimized local entity add
				smoke->leType = LE_SCALE_FADE;
				CG_LaunchParticleTrail(lastPos );
			} else if( cg_rocketTrail.integer == TRAIL_PARTICLE ) {
				CG_LaunchParticleTrail(lastPos );
			} else if( cg_rocketTrail.integer == TRAIL_TRACER ){
				le = CG_AllocLocalEntity();
				re = &le->refEntity;
 
				le->leType = LE_FADE_RGB;
				le->startTime = cg.time;
				le->endTime = cg.time + 300;
				le->lifeRate = 1.0 / (le->endTime - le->startTime);
 
				re->shaderTime = cg.time / 1000.0f;
				re->reType = RT_RAIL_CORE;
				re->customShader = cgs.media.railCoreShader;
				
				if( !( ( wi->item->giTag == WP_ROCKET_LAUNCHER ) && ( cg_forceWeaponColor.integer & 4 ) ) ){
					re->shaderRGBA[0] = 1.0 * 255;
					re->shaderRGBA[1] = 1.0 * 255;
					re->shaderRGBA[2] = 0.0 * 255;
					re->shaderRGBA[3] = 255;

				} else {
					re->shaderRGBA[0] = color[0];
					re->shaderRGBA[1] = color[1];
					re->shaderRGBA[2] = color[2];
					re->shaderRGBA[3] = 255;
				}
				
				le->color[0] = ((float)re->shaderRGBA[0])/255.0f;
				le->color[1] = ((float)re->shaderRGBA[1])/255.0f;
				le->color[2] = ((float)re->shaderRGBA[2])/255.0f;
				le->color[3] = ((float)re->shaderRGBA[3])/255.0f;
 
				VectorCopy(lastPosSaved, re->origin);
				VectorCopy(lastPos, re->oldorigin);
				
			}
		}
		else{
			smoke = CG_SmokePuff( lastPos, up, 
					  wi->trailRadius, 
					  1, 1, 1, 0.33f,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.smokePuffShader );
			// use the optimized local entity add
			smoke->leType = LE_SCALE_FADE;
		}
	}

}

#ifdef MISSIONPACK
/*
==========================
CG_NailTrail
==========================
*/
static void CG_NailTrail( centity_t *ent, const weaponInfo_t *wi ) {
	int		step;
	vec3_t	origin, lastPos;
	int		t;
	int		startTime, contents;
	int		lastContents;
	entityState_t	*es;
	vec3_t	up;
	localEntity_t	*smoke;

	

	if ( cg_noProjectileTrail.integer ) {
		return;
	}

	up[0] = 0;
	up[1] = 0;
	up[2] = 0;

	step = 50;

	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY ) {
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos );
	lastContents = CG_PointContents( lastPos, -1 );

	ent->trailTime = cg.time;

	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ) {
		if ( contents & lastContents & CONTENTS_WATER ) {
			CG_BubbleTrail( lastPos, origin, 8 );
		}
		return;
	}

	for ( ; t <= ent->trailTime ; t += step ) {
		BG_EvaluateTrajectory( &es->pos, t, lastPos );

		smoke = CG_SmokePuff( lastPos, up, 
					  wi->trailRadius, 
					  1, 1, 1, 0.33f,
					  wi->wiTrailTime, 
					  t,
					  0,
					  0, 
					  cgs.media.nailPuffShader );
		// use the optimized local entity add
		smoke->leType = LE_SCALE_FADE;
	}

}
#endif

/*
==========================
CG_NailTrail
==========================
*/
static void CG_PlasmaTrail( centity_t *cent, const weaponInfo_t *wi ) {
	localEntity_t	*le;
	refEntity_t		*re;
	entityState_t	*es;
	vec3_t			velocity, xvelocity, origin;
	vec3_t			offset, xoffset;
	vec3_t			v[3];
	//int				t, startTime, step;

	float	waterScale = 1.0f;

	if ( cg_noProjectileTrail.integer || !cg_plasmaTrail.integer ) {
		return;
	}

	//step = 50;

	es = &cent->currentState;
	//startTime = cent->trailTime;
	//t = step * ( (startTime + step) / step );

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	velocity[0] = 60 - 120 * crandom();
	velocity[1] = 40 - 80 * crandom();
	velocity[2] = 100 - 200 * crandom();

	le->leType = LE_MOVE_SCALE_FADE;
	le->leFlags = LEF_TUMBLE;
	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;

	le->startTime = cg.time;
	le->endTime = le->startTime + 600;

	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time;

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 2;
	offset[1] = 2;
	offset[2] = 2;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];

	VectorAdd( origin, xoffset, re->origin );
	VectorCopy( re->origin, le->pos.trBase );

	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10f;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );

	AxisCopy( axisDefault, re->axis );
    re->shaderTime = cg.time / 1000.0f;
    re->reType = RT_SPRITE;
    re->radius = 0.25f;
	re->customShader = /*cgs.media.railRingsShader*/ cgs.media.particlePlasma;
	le->bounceFactor = 0.3f;

    re->shaderRGBA[0] = wi->flashDlightColor[0] * 63;
    re->shaderRGBA[1] = wi->flashDlightColor[1] * 63;
    re->shaderRGBA[2] = wi->flashDlightColor[2] * 63;
    re->shaderRGBA[3] = 63;

    le->color[0] = wi->flashDlightColor[0] * 0.2;
    le->color[1] = wi->flashDlightColor[1] * 0.2;
    le->color[2] = wi->flashDlightColor[2] * 0.2;
    le->color[3] = 0.25f;

	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = rand()&31;
	le->angles.trBase[1] = rand()&31;
	le->angles.trBase[2] = rand()&31;
	le->angles.trDelta[0] = 1;
	le->angles.trDelta[1] = 0.5;
	le->angles.trDelta[2] = 0;

}
/*
==========================
CG_GrappleTrail
==========================
*/
void CG_GrappleTrail( centity_t *ent, const weaponInfo_t *wi ) {
	vec3_t	origin;
	entityState_t	*es;
	vec3_t			forward, up;
	refEntity_t		beam;

	es = &ent->currentState;

	BG_EvaluateTrajectory( &es->pos, cg.time, origin );
	ent->trailTime = cg.time;

	memset( &beam, 0, sizeof( beam ) );
	//FIXME adjust for muzzle position
	VectorCopy ( cg_entities[ ent->currentState.otherEntityNum ].lerpOrigin, beam.origin );
	beam.origin[2] += 26;
	AngleVectors( cg_entities[ ent->currentState.otherEntityNum ].lerpAngles, forward, NULL, up );
	VectorMA( beam.origin, -6, up, beam.origin );
	VectorCopy( origin, beam.oldorigin );

	if (Distance( beam.origin, beam.oldorigin ) < 64 )
		return; // Don't draw if close

	beam.reType = RT_LIGHTNING;
	beam.customShader = cgs.media.lightningShader[0];

	AxisClear( beam.axis );
	beam.shaderRGBA[0] = 0xff;
	beam.shaderRGBA[1] = 0xff;
	beam.shaderRGBA[2] = 0xff;
	beam.shaderRGBA[3] = 0xff;
	trap_R_AddRefEntityToScene( &beam );
}

/*
==========================
CG_GrenadeTrail
==========================
*/
static void CG_GrenadeTrail( centity_t *ent, const weaponInfo_t *wi ) {
	CG_RocketTrail( ent, wi );
}


/*
=================
CG_RegisterWeapon

The server says this item is used on this level
=================
*/
void CG_RegisterWeapon( int weaponNum ) {
	weaponInfo_t	*weaponInfo;
	gitem_t			*item, *ammo;
	char			path[MAX_QPATH];
	vec3_t			mins, maxs;
	int				i;

	weaponInfo = &cg_weapons[weaponNum];

	if ( weaponNum == 0 ) {
		return;
	}

	if ( weaponInfo->registered ) {
		return;
	}

	memset( weaponInfo, 0, sizeof( *weaponInfo ) );
	weaponInfo->registered = qtrue;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType == IT_WEAPON && item->giTag == weaponNum ) {
			weaponInfo->item = item;
			break;
		}
	}
	if ( !item->classname ) {
		CG_Error( "Couldn't find weapon %i", weaponNum );
	}
	CG_RegisterItemVisuals( item - bg_itemlist );

	// load cmodel before model so filecache works
	weaponInfo->weaponModel = trap_R_RegisterModel( item->world_model[0] );

	if( item->brightSkin )
		weaponInfo->brightSkin = trap_R_RegisterShader( item->brightSkin );
	
	if( item->brightColor[0] || item->brightColor[1] || item->brightColor[2] ){
		weaponInfo->brightColor[0] = item->brightColor[0];
		weaponInfo->brightColor[1] = item->brightColor[1];
		weaponInfo->brightColor[2] = item->brightColor[2];
	}
	  
	// calc midpoint for rotation
	trap_R_ModelBounds( weaponInfo->weaponModel, mins, maxs );
	for ( i = 0 ; i < 3 ; i++ ) {
		weaponInfo->weaponMidpoint[i] = mins[i] + 0.5 * ( maxs[i] - mins[i] );
	}

	weaponInfo->weaponIcon = trap_R_RegisterShader( item->icon );
	weaponInfo->ammoIcon = trap_R_RegisterShader( item->icon );

	for ( ammo = bg_itemlist + 1 ; ammo->classname ; ammo++ ) {
		if ( ammo->giType == IT_AMMO && ammo->giTag == weaponNum ) {
			break;
		}
	}
	if ( ammo->classname && ammo->world_model[0] ) {
		weaponInfo->ammoModel = trap_R_RegisterModel( ammo->world_model[0] );
	}

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	strcat( path, "_flash.md3" );
	weaponInfo->flashModel = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	strcat( path, "_barrel.md3" );
	weaponInfo->barrelModel = trap_R_RegisterModel( path );

	Q_strncpyz( path, item->world_model[0], MAX_QPATH );
	COM_StripExtension(path, path, sizeof(path));
	strcat( path, "_hand.md3" );
	weaponInfo->handsModel = trap_R_RegisterModel( path );

	if ( !weaponInfo->handsModel ) {
		weaponInfo->handsModel = trap_R_RegisterModel( "models/weapons2/shotgun/shotgun_hand.md3" );
	}

	weaponInfo->loopFireSound = qfalse;

	switch ( weaponNum ) {
	case WP_GAUNTLET:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/melee/fstrun.wav", qfalse );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav", qfalse );
		break;

	case WP_LIGHTNING:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/melee/fsthum.wav", qfalse );
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/lightning/lg_hum.wav", qfalse );

		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/lightning/lg_fire.wav", qfalse );
		
		if( cg_nomip.integer & 1 ){
			cgs.media.lightningShader[0] = trap_R_RegisterShaderNoMip( "lightningBoltScroll_nomip");
			cgs.media.lightningShaderColor[0] = trap_R_RegisterShaderNoMip( "lightningBoltScrollColor_nomip");
			cgs.media.lightningShader[1] = trap_R_RegisterShaderNoMip( "lightningBoltScrollThin_nomip");
			cgs.media.lightningShaderColor[1] = trap_R_RegisterShaderNoMip( "lightningBoltScrollThinColor_nomip");
			cgs.media.lightningShader[2] = trap_R_RegisterShaderNoMip( "lightningBoltnew_nomip");
			cgs.media.lightningShaderColor[2] = trap_R_RegisterShaderNoMip( "lightningBoltnewColor_nomip");
			cgs.media.lightningShader[3] = trap_R_RegisterShaderNoMip( "lightningBoltSimple_nomip");
			cgs.media.lightningShaderColor[3] = trap_R_RegisterShaderNoMip( "lightningBoltSimpleColor_nomip");
		}
		else{
			cgs.media.lightningShader[0] = trap_R_RegisterShaderNoMip( "lightningBoltScroll");
			cgs.media.lightningShaderColor[0] = trap_R_RegisterShaderNoMip( "lightningBoltScrollColor");
			cgs.media.lightningShader[1] = trap_R_RegisterShaderNoMip( "lightningBoltScrollThin");
			cgs.media.lightningShaderColor[1] = trap_R_RegisterShaderNoMip( "lightningBoltScrollThinColor");
			cgs.media.lightningShader[2] = trap_R_RegisterShaderNoMip( "lightningBoltnew");
			cgs.media.lightningShaderColor[2] = trap_R_RegisterShaderNoMip( "lightningBoltnewColor");
			cgs.media.lightningShader[3] = trap_R_RegisterShaderNoMip( "lightningBoltSimple");
			cgs.media.lightningShaderColor[3] = trap_R_RegisterShaderNoMip( "lightningBoltSimpleColor");
		}
		cgs.media.lightningExplosionModel = trap_R_RegisterModel( "models/weaphits/crackle.md3" );
		cgs.media.lightningExplosionNomip = trap_R_RegisterShaderNoMip( "LightnnFlash_nomip" );
		
		cgs.media.sfx_lghit1 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit.wav", qfalse );
		cgs.media.sfx_lghit2 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit2.wav", qfalse );
		cgs.media.sfx_lghit3 = trap_S_RegisterSound( "sound/weapons/lightning/lg_hit3.wav", qfalse );

		break;

	case WP_GRAPPLING_HOOK:
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/rocket/rocket.md3" );
		weaponInfo->missileTrailFunc = CG_GrappleTrail;
		weaponInfo->missileDlight = 200;
		weaponInfo->wiTrailTime = 2000;
		weaponInfo->trailRadius = 64;
		MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
		weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/melee/fsthum.wav", qfalse );
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/melee/fstrun.wav", qfalse );
		break;

#ifdef MISSIONPACK
	case WP_CHAINGUN:
		weaponInfo->firingSound = trap_S_RegisterSound( "sound/weapons/vulcan/wvulfire.wav", qfalse );
		weaponInfo->loopFireSound = qtrue;
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/vulcan/vulcanf1b.wav", qfalse );
		weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/vulcan/vulcanf2b.wav", qfalse );
		weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/vulcan/vulcanf3b.wav", qfalse );
		weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/vulcan/vulcanf4b.wav", qfalse );
		weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
		if( cg_nomip.integer & 16 )
			cgs.media.bulletExplosionShader = trap_R_RegisterShader( "aftershock_bulletExplosion_nomip" );
		else
			cgs.media.bulletExplosionShader = trap_R_RegisterShader( "aftershock_bulletExplosion" );
		break;
#endif

	case WP_MACHINEGUN:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf1b.wav", qfalse );
		weaponInfo->flashSound[1] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf2b.wav", qfalse );
		weaponInfo->flashSound[2] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf3b.wav", qfalse );
		weaponInfo->flashSound[3] = trap_S_RegisterSound( "sound/weapons/machinegun/machgf4b.wav", qfalse );
		weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
		if( cg_nomip.integer & 16 )
			cgs.media.bulletExplosionShader = trap_R_RegisterShader( "aftershock_bulletExplosion_nomip" );
		else
			cgs.media.bulletExplosionShader = trap_R_RegisterShader( "aftershock_bulletExplosion" );
		break;

	case WP_SHOTGUN:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/shotgun/sshotf1b.wav", qfalse );
		weaponInfo->ejectBrassFunc = CG_ShotgunEjectBrass;
		break;

	case WP_ROCKET_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/rocket/rocket.md3" );
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
		weaponInfo->missileTrailFunc = CG_RocketTrail;
		weaponInfo->missileDlight = 200;
		weaponInfo->wiTrailTime = 2000;
		weaponInfo->trailRadius = 64;
		
		MAKERGB( weaponInfo->missileDlightColor, 1, 0.75f, 0 );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );

		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
		if( cg_nomip.integer & 4 ) {
			cgs.media.rocketExplosionShader = trap_R_RegisterShader( "aftershock_rocketExplosion_nomip" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore_nomip" );
		}
		else {
			cgs.media.rocketExplosionShader = trap_R_RegisterShader( "aftershock_rocketExplosion" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
		}
		break;

#ifdef MISSIONPACK
	case WP_PROX_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/proxmine.md3" );
		weaponInfo->missileTrailFunc = CG_GrenadeTrail;
		weaponInfo->wiTrailTime = 700;
		weaponInfo->trailRadius = 32;
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.70f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/proxmine/wstbfire.wav", qfalse );
		cgs.media.grenadeExplosionShader = trap_R_RegisterShader( "aftershock_grenadeExplosion2" );
		break;
#endif

	case WP_GRENADE_LAUNCHER:
		weaponInfo->missileModel = trap_R_RegisterModel( "models/ammo/grenade.md3" );
		weaponInfo->missileTrailFunc = CG_GrenadeTrail;
		weaponInfo->wiTrailTime = 700;
		weaponInfo->trailRadius = 32;
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.70f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/grenade/grenlf1a.wav", qfalse );
		if( cg_nomip.integer & 8 ){
			cgs.media.grenadeExplosionShader = trap_R_RegisterShader( "aftershock_grenadeExplosion2_nomip" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore_nomip" );
		}
		else {
			cgs.media.grenadeExplosionShader = trap_R_RegisterShader( "aftershock_grenadeExplosion2" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
		}
		break;

#ifdef MISSIONPACK
	case WP_NAILGUN:
		weaponInfo->ejectBrassFunc = CG_NailgunEjectBrass;
		weaponInfo->missileTrailFunc = CG_NailTrail;
//		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/nailgun/wnalflit.wav", qfalse );
		weaponInfo->trailRadius = 16;
		weaponInfo->wiTrailTime = 250;
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/nail.md3" );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.75f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/nailgun/wnalfire.wav", qfalse );
		break;
#endif

	case WP_PLASMAGUN:
//		weaponInfo->missileModel = cgs.media.invulnerabilityPowerupModel;
		weaponInfo->missileTrailFunc = CG_PlasmaTrail;
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/plasma/lasfly.wav", qfalse );
		weaponInfo->missileDlight = 75;
		
		MAKERGB( weaponInfo->flashDlightColor, 0.6f, 0.6f, 1.0f );
		MAKERGB( weaponInfo->missileDlightColor, 0.6f, 0.6f, 1.0f );
		
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/plasma/hyprbf1a.wav", qfalse );
		
		if( cg_nomip.integer & 2 ){
			cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "aftershock_plasmaExplosion_nomip" );
			cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc_nomip" );
		}
		else{
			cgs.media.plasmaExplosionShader = trap_R_RegisterShader( "aftershock_plasmaExplosion" );
			cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
		}
		
		break;

	case WP_RAILGUN:
		weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/railgun/rg_hum.wav", qfalse );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.5f, 0 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/railgun/railgf1a.wav", qfalse );
		
		if( cg_nomip.integer & 32 ){
			cgs.media.railExplosionShader = trap_R_RegisterShader( "aftershock_railExplosion_nomip" );
			cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc_nomip" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore_nomip" );
		}
		else{
			cgs.media.railExplosionShader = trap_R_RegisterShader( "aftershock_railExplosion" );
			cgs.media.railRingsShader = trap_R_RegisterShader( "railDisc" );
			cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
		}
		

		break;

	case WP_BFG:
		weaponInfo->readySound = trap_S_RegisterSound( "sound/weapons/bfg/bfg_hum.wav", qfalse );
		MAKERGB( weaponInfo->flashDlightColor, 1, 0.7f, 1 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/bfg/bfg_fire.wav", qfalse );
		
		if( cg_nomip.integer & 64 )
			cgs.media.bfgExplosionShader = trap_R_RegisterShader( "aftershock_bfgExplosion_nomip" );
		else
			cgs.media.bfgExplosionShader = trap_R_RegisterShader( "aftershock_bfgExplosion" );
		
		weaponInfo->missileModel = trap_R_RegisterModel( "models/weaphits/bfg.md3" );
		weaponInfo->missileSound = trap_S_RegisterSound( "sound/weapons/rocket/rockfly.wav", qfalse );
		break;

	 default:
		MAKERGB( weaponInfo->flashDlightColor, 1, 1, 1 );
		weaponInfo->flashSound[0] = trap_S_RegisterSound( "sound/weapons/rocket/rocklf1a.wav", qfalse );
		break;
	}
}

/*
=================
CG_RegisterItemVisuals

The server says this item is used on this level
=================
*/
void CG_RegisterItemVisuals( int itemNum ) {
	itemInfo_t		*itemInfo;
	gitem_t			*item;

	if ( itemNum < 0 || itemNum >= bg_numItems ) {
		CG_Error( "CG_RegisterItemVisuals: itemNum %d out of range [0-%d]", itemNum, bg_numItems-1 );
	}

	itemInfo = &cg_items[ itemNum ];
	if ( itemInfo->registered ) {
		return;
	}

	item = &bg_itemlist[ itemNum ];

	memset( itemInfo, 0, sizeof( &itemInfo ) );
	itemInfo->registered = qtrue;

	itemInfo->models[0] = trap_R_RegisterModel( item->world_model[0] );

	itemInfo->icon = trap_R_RegisterShader( item->icon );

	if ( item->giType == IT_WEAPON ) {
		CG_RegisterWeapon( item->giTag );
	}

	//
	// powerups have an accompanying ring or sphere
	//
	if ( item->giType == IT_POWERUP || item->giType == IT_HEALTH || 
		item->giType == IT_ARMOR || item->giType == IT_HOLDABLE ) {
		if ( item->world_model[1] ) {
			itemInfo->models[1] = trap_R_RegisterModel( item->world_model[1] );
		}
	}
	if ( item->brightSkin )
		itemInfo->brightSkin = trap_R_RegisterShader( item->brightSkin );
	
	if( item->brightColor[0] || item->brightColor[1] || item->brightColor[2] ){
		itemInfo->brightColor[0] = item->brightColor[0];
		itemInfo->brightColor[1] = item->brightColor[1];
		itemInfo->brightColor[2] = item->brightColor[2];
		
	}
}


/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

/*
=================
CG_MapTorsoToWeaponFrame

=================
*/
static int CG_MapTorsoToWeaponFrame( clientInfo_t *ci, int frame ) {

	// change weapon
	if ( frame >= ci->animations[TORSO_DROP].firstFrame 
		&& frame < ci->animations[TORSO_DROP].firstFrame + 9 ) {
		return frame - ci->animations[TORSO_DROP].firstFrame + 6;
	}

	// stand attack
	if ( frame >= ci->animations[TORSO_ATTACK].firstFrame 
		&& frame < ci->animations[TORSO_ATTACK].firstFrame + 6 ) {
		return 1 + frame - ci->animations[TORSO_ATTACK].firstFrame;
	}

	// stand attack 2
	if ( frame >= ci->animations[TORSO_ATTACK2].firstFrame 
		&& frame < ci->animations[TORSO_ATTACK2].firstFrame + 6 ) {
		return 1 + frame - ci->animations[TORSO_ATTACK2].firstFrame;
	}
	
	return 0;
}


/*
==============
CG_CalculateWeaponPosition
==============
*/
static void CG_CalculateWeaponPosition( vec3_t origin, vec3_t angles ) {
	float	scale;
	int		delta;
	float	fracsin;
	

	VectorCopy( cg.refdef.vieworg, origin );
	VectorCopy( cg.refdefViewAngles, angles );

	if( !cg_weaponBobbing.integer )
		return;

	// on odd legs, invert some angles
	if ( cg.bobcycle & 1 ) {
		scale = -cg.xyspeed;
	} else {
		scale = cg.xyspeed;
	}

	// gun angles from bobbing
	angles[ROLL] += scale * cg.bobfracsin * 0.005;
	angles[YAW] += scale * cg.bobfracsin * 0.01;
	angles[PITCH] += cg.xyspeed * cg.bobfracsin * 0.005;

	// drop the weapon when landing
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		origin[2] += cg.landChange*0.25 * delta / LAND_DEFLECT_TIME;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		origin[2] += cg.landChange*0.25 * 
			(LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta) / LAND_RETURN_TIME;
	}

#if 0
	// drop the weapon when stair climbing
	delta = cg.time - cg.stepTime;
	if ( delta < STEP_TIME/2 ) {
		origin[2] -= cg.stepChange*0.25 * delta / (STEP_TIME/2);
	} else if ( delta < STEP_TIME ) {
		origin[2] -= cg.stepChange*0.25 * (STEP_TIME - delta) / (STEP_TIME/2);
	}
#endif

	// idle drift
	scale = cg.xyspeed + 40;
	fracsin = sin( cg.time * 0.001 );
	angles[ROLL] += scale * fracsin * 0.01;
	angles[YAW] += scale * fracsin * 0.01;
	angles[PITCH] += scale * fracsin * 0.01;
}

/*
=================
CG_ExplosionParticles


=================
*/

void CG_ExplosionParticles( int weapon, vec3_t origin , vec3_t dir, vec3_t color) {
	int number; // number of particles
	int jump; // amount to nudge the particles trajectory vector up by
	int speed; // speed of particles
	int light; // amount of light for each particle
	vec4_t lColor; // color of light for each particle
	qhandle_t shader; // shader to use for the particles
	int index;
	vec3_t randVec, tempVec;
	
	if( !cg_particles.integer )
		return;
	
	VectorMA( origin, 1, dir, origin);

	// set defaults
	number = 32;
	jump = 50;
	speed = 300;
	light = 50;
	lColor[0] = 1.0f;
	lColor[1] = 1.0f;
	lColor[2] = 1.0f;
	lColor[3] = 1.0f; // alpha

	switch( weapon ) {
		case WP_MACHINEGUN:
			number = 1;
			jump = 30;
			light = 100;
			speed = 150;
			lColor[0] = 1.0f;
			lColor[1] = 0.56f;
			lColor[2] = 0.0f;
			shader = cgs.media.particleSpark;
			break;
		case WP_SHOTGUN:
			number = 1;
			jump = 30;
			light = 100;
			speed = 150;
			lColor[0] = 1.0f;
			lColor[1] = 0.56f;
			lColor[2] = 0.0f;
			shader = cgs.media.particleSpark;
			break;
		case WP_ROCKET_LAUNCHER:
			number = 32;
			jump = 70;
			light = 100;
			lColor[0] = 1.0f;
			lColor[1] = 0.56f;
			lColor[2] = 0.0f;
			shader = cgs.media.particleSpark;
			break;
		case WP_GRENADE_LAUNCHER:
			number = 32;
			jump = 60;
			light = 100;
			lColor[0] = 1.0f;
			lColor[1] = 0.56f;
			lColor[2] = 0.0f;
			shader = cgs.media.particleSpark;
			break;
		case WP_LIGHTNING:
			number = 1;
			jump = 30;
			light = 100;
			lColor[0] = 1.0f;
			lColor[1] = 0.56f;
			lColor[2] = 0.0f;
			speed = 150;
			shader = cgs.media.particlePlasma;
			break;
		case WP_PLASMAGUN:
			number = 2;
			jump = 30;
			light = 100;
			speed = 150;
			lColor[0] = 0.0f;
			lColor[1] = 0.56f;
			lColor[2] = 1.0f;
			shader = cgs.media.particlePlasma;
			break;
		case WP_RAILGUN:
			number = 2;
			jump = 30;
			light = 100;
			speed = 150;
			lColor[0] = 0.0f;
			lColor[1] = 0.56f;
			lColor[2] = 1.0f;
			shader = cgs.media.particlePlasma;
			break;
		default:
			return;
	}

	for( index = 0; index < number; index++ ) {
		localEntity_t *le;
		refEntity_t *re;

		le = CG_AllocLocalEntity(); //allocate a local entity
		re = &le->refEntity;
		le->leFlags = LEF_PUFF_DONT_SCALE; //don't change the particle size
		le->leType = /*LE_MOVE_SCALE_FADE*/LE_FRAGMENT; // particle should fade over time
		le->startTime = cg.time; // set the start time of the particle to the current time
		le->endTime = cg.time + 2000 + random() * 1000; //set the end time
		le->lifeRate = 1.0 / ( le->endTime - le->startTime );
		re = &le->refEntity;
		re->shaderTime = cg.time / 1000.0f;
		re->reType = RT_SPRITE;
		re->rotation = 0;
		re->radius = 5*random();
		
		re->customShader = shader;
		re->shaderRGBA[0] = 0xff;
		re->shaderRGBA[1] = 0xff;
		re->shaderRGBA[2] = 0xff;
		re->shaderRGBA[3] = 0xff;
		le->light = light;
		VectorCopy( lColor, le->lightColor );
		le->color[3] = 1.0;
		le->pos.trType = TR_GRAVITY; // moves in a gravity affected arc
		le->pos.trTime = cg.time;
		le->bounceFactor = 0.2f;
		VectorCopy( origin, le->pos.trBase );
		VectorCopy( origin, re->origin );

		tempVec[0] = crandom(); //between 1 and -1
		tempVec[1] = crandom();
		tempVec[2] = crandom();
		
		//if( weapon == WP_PLASMAGUN || weapon == WP_MACHINEGUN || weapon == WP_SHOTGUN || weapon == WP_RAILGUN)
		if( weapon != WP_LIGHTNING  && weapon != WP_GRENADE_LAUNCHER )
			VectorMA( tempVec, 2, dir, tempVec );
		
		/*if( weapon == WP_RAILGUN ){
			re->shaderRGBA[0] = (int)(color[0]*255);
			re->shaderRGBA[1] = (int)(color[1]*255);
			re->shaderRGBA[2] = (int)(color[2]*255);
			re->shaderRGBA[3] = 255;
			le->color[0] = color[0];
			le->color[1] = color[1];
			le->color[2] = color[2];
			le->color[3] = 1.0f;
		}*/
			
		
		VectorNormalize(tempVec);
		VectorScale(tempVec, speed + crandom() * speed/2, randVec);
		randVec[2] += jump; //nudge the particles up a bit
		VectorCopy( randVec, le->pos.trDelta );
	}
}

/*
===============
CG_LightningBolt

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
The cent should be the non-predicted cent if it is from the player,
so the endpoint will reflect the simulated strike (lagging the predicted
angle)
===============
*/
static void CG_LightningBolt( centity_t *cent, vec3_t origin ) {
	trace_t  trace;
	refEntity_t  beam;
	vec3_t   forward;
	vec3_t   muzzlePoint, endPoint;
	int	style;
	clientInfo_t *local, *other;
  	local = &cgs.clientinfo[cg.clientNum];
	other = &cgs.clientinfo[cent->currentState.number];

	if (cent->currentState.weapon != WP_LIGHTNING) {
		return;
	}

	memset( &beam, 0, sizeof( beam ) );

	if( ( cg_trueLightning.value < 0.0f ) && (cent->currentState.number == cg.predictedPlayerState.clientNum) )
		return;

//unlagged - attack prediction #1
	// if the entity is us, unlagged is on server-side, and we've got it on for the lightning gun
	if ( (cent->currentState.number == cg.predictedPlayerState.clientNum) && cgs.delagHitscan &&
			( cg_delag.integer & 1 || cg_delag.integer & 8 ) ) {
		// always shoot straight forward from our current position
		AngleVectors( cg.predictedPlayerState.viewangles, forward, NULL, NULL );
		VectorCopy( cg.predictedPlayerState.origin, muzzlePoint );
	}
	else
//unlagged - attack prediction #1

	// CPMA  "true" lightning
        if ((cent->currentState.number == cg.predictedPlayerState.clientNum) && (cg_trueLightning.value != 0)) {
		vec3_t angle;
		int i;

//unlagged - true lightning
		// might as well fix up true lightning while we're at it
		//vec3_t viewangles;
		//VectorCopy( cg.predictedPlayerState.viewangles, viewangles );
//unlagged - true lightning

		for (i = 0; i < 3; i++) {
			float a = cent->lerpAngles[i] - cg.refdefViewAngles[i];
			if (a > 180) {
				a -= 360;
			}
			if (a < -180) {
				a += 360;
			}

			angle[i] = cg.refdefViewAngles[i] + a * (1.0 - cg_trueLightning.value);
			if (angle[i] < 0) {
				angle[i] += 360;
			}
			if (angle[i] > 360) {
				angle[i] -= 360;
			}
		}

		AngleVectors(angle, forward, NULL, NULL );
//unlagged - true lightning
//		VectorCopy(cent->lerpOrigin, muzzlePoint );
//		VectorCopy(cg.refdef.vieworg, muzzlePoint );
		// *this* is the correct origin for true lightning
		VectorCopy(cg.predictedPlayerState.origin, muzzlePoint );
//unlagged - true lightning
	} else {
		// !CPMA
		AngleVectors( cent->lerpAngles, forward, NULL, NULL );
		VectorCopy(cent->lerpOrigin, muzzlePoint );
	}

	// FIXME: crouch
	muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

	VectorMA( muzzlePoint, 14, forward, muzzlePoint );

	// project forward by the lightning range
	VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

	// see if it hit a wall
	CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint, 
		cent->currentState.number, MASK_SHOT );

	// this is the endpoint
	VectorCopy( trace.endpos, beam.oldorigin );

	// use the provided origin, even though it may be slightly
	// different than the muzzle origin
	VectorCopy( origin, beam.origin );

	beam.reType = RT_LIGHTNING;
	
	style = cg_lightningStyle.integer;
	if( style < 0 )
		style = 0;
	if( style >= MAX_LGSTYLES )
		style = MAX_LGSTYLES - 1;
	
	if( cg_forceWeaponColor.integer & 8 ){
		beam.customShader = cgs.media.lightningShaderColor[style];
		if( cgs.gametype >= GT_TEAM && cgs.ffa_gt != 1 ){
			if( local->team != other->team ){
				CG_setRGBA(beam.shaderRGBA, cg_enemyWeaponColor.string);
				/*beam.shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
				beam.shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
				beam.shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
				beam.shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/
			}
			else{
				CG_setRGBA(beam.shaderRGBA, cg_teamWeaponColor.string);
				/*beam.shaderRGBA[0] = hexToRed( cg_teamWeaponColor.string );
				beam.shaderRGBA[1] = hexToGreen( cg_teamWeaponColor.string );
				beam.shaderRGBA[2] = hexToBlue( cg_teamWeaponColor.string );
				beam.shaderRGBA[3] = hexToAlpha( cg_teamWeaponColor.string );*/
			}
		}
		else if( cg.clientNum != cent->currentState.number ){
			CG_setRGBA(beam.shaderRGBA, cg_enemyWeaponColor.string);
			/*beam.shaderRGBA[0] = hexToRed( cg_enemyWeaponColor.string );
			beam.shaderRGBA[1] = hexToGreen( cg_enemyWeaponColor.string );
			beam.shaderRGBA[2] = hexToBlue( cg_enemyWeaponColor.string );
			beam.shaderRGBA[3] = hexToAlpha( cg_enemyWeaponColor.string );*/
		}
		else{
			CG_setRGBA(beam.shaderRGBA, cg_teamWeaponColor.string);
			/*beam.shaderRGBA[0] = hexToRed( cg_teamWeaponColor.string );
			beam.shaderRGBA[1] = hexToGreen( cg_teamWeaponColor.string );
			beam.shaderRGBA[2] = hexToBlue( cg_teamWeaponColor.string );
			beam.shaderRGBA[3] = hexToAlpha( cg_teamWeaponColor.string );*/
		}
	}
	else
		beam.customShader = cgs.media.lightningShader[style];
	
	trap_R_AddRefEntityToScene( &beam );

	// add the impact flare if it hit something
	if ( trace.fraction < 1.0 ) {
		vec3_t	angles;
		vec3_t	dir;

		VectorSubtract( beam.oldorigin, beam.origin, dir );
		VectorNormalize( dir );

		memset( &beam, 0, sizeof( beam ) );
		beam.hModel = cgs.media.lightningExplosionModel;
		
		/*if( cg_nomip.integer & 1 )
			beam.customShader = cgs.media.lightningExplosionNomip;*/
		
		VectorMA( trace.endpos, -16, dir, beam.origin );

		// make a random orientation
		angles[0] = rand() % 360;
		angles[1] = rand() % 360;
		angles[2] = rand() % 360;
		AnglesToAxis( angles, beam.axis );
		if( cg_explosion.integer & ( 1 << ( WP_LIGHTNING - 2 )) )
			trap_R_AddRefEntityToScene( &beam );
		CG_ExplosionParticles(WP_LIGHTNING, beam.origin, NULL, NULL);
	}
}
/*

static void CG_LightningBolt( centity_t *cent, vec3_t origin ) {
	trace_t		trace;
	refEntity_t		beam;
	vec3_t			forward;
	vec3_t			muzzlePoint, endPoint;

	if ( cent->currentState.weapon != WP_LIGHTNING ) {
		return;
	}

	memset( &beam, 0, sizeof( beam ) );

	// find muzzle point for this frame
	VectorCopy( cent->lerpOrigin, muzzlePoint );
	AngleVectors( cent->lerpAngles, forward, NULL, NULL );

	// FIXME: crouch
	muzzlePoint[2] += DEFAULT_VIEWHEIGHT;

	VectorMA( muzzlePoint, 14, forward, muzzlePoint );

	// project forward by the lightning range
	VectorMA( muzzlePoint, LIGHTNING_RANGE, forward, endPoint );

	// see if it hit a wall
	CG_Trace( &trace, muzzlePoint, vec3_origin, vec3_origin, endPoint, 
		cent->currentState.number, MASK_SHOT );

	// this is the endpoint
	VectorCopy( trace.endpos, beam.oldorigin );

	// use the provided origin, even though it may be slightly
	// different than the muzzle origin
	VectorCopy( origin, beam.origin );

	beam.reType = RT_LIGHTNING;
	beam.customShader = cgs.media.lightningShader;
	trap_R_AddRefEntityToScene( &beam );

	// add the impact flare if it hit something
	if ( trace.fraction < 1.0 ) {
		vec3_t	angles;
		vec3_t	dir;

		VectorSubtract( beam.oldorigin, beam.origin, dir );
		VectorNormalize( dir );

		memset( &beam, 0, sizeof( beam ) );
		beam.hModel = cgs.media.lightningExplosionModel;

		VectorMA( trace.endpos, -16, dir, beam.origin );

		// make a random orientation
		angles[0] = rand() % 360;
		angles[1] = rand() % 360;
		angles[2] = rand() % 360;
		AnglesToAxis( angles, beam.axis );
		trap_R_AddRefEntityToScene( &beam );
	}
}
*/

/*
===============
CG_SpawnRailTrail

Origin will be the exact tag point, which is slightly
different than the muzzle point used for determining hits.
===============
*/
static void CG_SpawnRailTrail( centity_t *cent, vec3_t origin ) {
	clientInfo_t	*ci;

	if ( cent->currentState.weapon != WP_RAILGUN ) {
		return;
	}
	if ( !cent->pe.railgunFlash ) {
		return;
	}
	cent->pe.railgunFlash = qtrue;
	ci = &cgs.clientinfo[ cent->currentState.clientNum ];
	CG_RailTrail( ci, origin, cent->pe.railgunImpact );
}


/*
======================
CG_MachinegunSpinAngle
======================
*/
#define		SPIN_SPEED	0.9
#define		COAST_TIME	1000
static float	CG_MachinegunSpinAngle( centity_t *cent ) {
	int		delta;
	float	angle;
	float	speed;

	delta = cg.time - cent->pe.barrelTime;
	if ( cent->pe.barrelSpinning ) {
		angle = cent->pe.barrelAngle + delta * SPIN_SPEED;
	} else {
		if ( delta > COAST_TIME ) {
			delta = COAST_TIME;
		}

		speed = 0.5 * ( SPIN_SPEED + (float)( COAST_TIME - delta ) / COAST_TIME );
		angle = cent->pe.barrelAngle + delta * speed;
	}

	if ( cent->pe.barrelSpinning == !(cent->currentState.eFlags & EF_FIRING) ) {
		cent->pe.barrelTime = cg.time;
		cent->pe.barrelAngle = AngleMod( angle );
		cent->pe.barrelSpinning = !!(cent->currentState.eFlags & EF_FIRING);
#ifdef MISSIONPACK
		if ( cent->currentState.weapon == WP_CHAINGUN && !cent->pe.barrelSpinning ) {
			trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, trap_S_RegisterSound( "sound/weapons/vulcan/wvulwind.wav", qfalse ) );
		}
#endif
	}

	return angle;
}


/*
========================
CG_AddWeaponWithPowerups
========================
*/
static void CG_AddWeaponWithPowerups( refEntity_t *gun, int powerups ) {
	// add powerup effects
	if ( powerups & ( 1 << PW_INVIS ) ) {
		gun->customShader = cgs.media.invisShader;
		//trap_R_AddRefEntityToScene( gun );
	} else {
		trap_R_AddRefEntityToScene( gun );

		if ( powerups & ( 1 << PW_BATTLESUIT ) ) {
			gun->customShader = cgs.media.battleWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}
		if ( powerups & ( 1 << PW_QUAD ) ) {
			gun->customShader = cgs.media.quadWeaponShader;
			trap_R_AddRefEntityToScene( gun );
		}
	}
}


/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
void CG_AddPlayerWeapon( refEntity_t *parent, playerState_t *ps, centity_t *cent, int team ) {
	refEntity_t	gun;
	refEntity_t	barrel;
	refEntity_t	flash;
	vec3_t		angles;
	weapon_t	weaponNum;
	weaponInfo_t	*weapon;
	centity_t	*nonPredictedCent;
	orientation_t	lerped;

	weaponNum = cent->currentState.weapon;

	CG_RegisterWeapon( weaponNum );
	weapon = &cg_weapons[weaponNum];
		

	// add the weapon
	memset( &gun, 0, sizeof( gun ) );
	VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
	gun.shadowPlane = parent->shadowPlane;
	gun.renderfx = parent->renderfx;

	// set custom shading for railgun refire rate
	if ( ps ) {
		if ( cg.predictedPlayerState.weapon == WP_RAILGUN 
			&& cg.predictedPlayerState.weaponstate == WEAPON_FIRING ) {
			float	f;

			f = (float)cg.predictedPlayerState.weaponTime / 1500;
			gun.shaderRGBA[1] = 0;
			gun.shaderRGBA[0] = 
			gun.shaderRGBA[2] = 255 * ( 1.0 - f );
		} else {
			gun.shaderRGBA[0] = 255;
			gun.shaderRGBA[1] = 255;
			gun.shaderRGBA[2] = 255;
			gun.shaderRGBA[3] = 255;
		}
	}

	gun.hModel = weapon->weaponModel;
	if (!gun.hModel) {
		return;
	}
	
	if( weapon->brightSkin && cg_brightItems.integer == 1 )
		gun.customShader = weapon->brightSkin;
	else if( cg_brightItems.integer == 2 && ( weapon->brightColor[0] || weapon->brightColor[1] || weapon->brightColor[2] ) ){
		gun.customShader = trap_R_RegisterShader("models/players/flat");
		
		gun.shaderRGBA[0] = weapon->brightColor[0];
		gun.shaderRGBA[1] = weapon->brightColor[1];
		gun.shaderRGBA[2] = weapon->brightColor[2];
		gun.shaderRGBA[3] = 255;
	}

	if ( !ps ) {
		// add weapon ready sound
		cent->pe.lightningFiring = qfalse;
		if ( ( cent->currentState.eFlags & EF_FIRING ) && weapon->firingSound ) {
			// lightning gun and guantlet make a different sound when fire is held down
			trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->firingSound );
			cent->pe.lightningFiring = qtrue;
		} else if ( weapon->readySound ) {
			trap_S_AddLoopingSound( cent->currentState.number, cent->lerpOrigin, vec3_origin, weapon->readySound );
		}
	}

	trap_R_LerpTag(&lerped, parent->hModel, parent->oldframe, parent->frame,
		1.0 - parent->backlerp, "tag_weapon");
	VectorCopy(parent->origin, gun.origin);

	VectorMA(gun.origin, lerped.origin[0], parent->axis[0], gun.origin);

	// Make weapon appear left-handed for 2 and centered for 3
	if(ps && cg_drawGun.integer == 2)
		VectorMA(gun.origin, -lerped.origin[1], parent->axis[1], gun.origin);
	else if(!ps || cg_drawGun.integer != 3)
       	VectorMA(gun.origin, lerped.origin[1], parent->axis[1], gun.origin);

	VectorMA(gun.origin, lerped.origin[2], parent->axis[2], gun.origin);

	MatrixMultiply(lerped.axis, ((refEntity_t *)parent)->axis, gun.axis);
	gun.backlerp = parent->backlerp;

	CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups );

	// add the spinning barrel
	if ( weapon->barrelModel ) {
		memset( &barrel, 0, sizeof( barrel ) );
		VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
		barrel.shadowPlane = parent->shadowPlane;
		barrel.renderfx = parent->renderfx;

		barrel.hModel = weapon->barrelModel;
		angles[YAW] = 0;
		angles[PITCH] = 0;
		angles[ROLL] = CG_MachinegunSpinAngle( cent );
		AnglesToAxis( angles, barrel.axis );

		CG_PositionRotatedEntityOnTag( &barrel, &gun, weapon->weaponModel, "tag_barrel" );
		
		if( weapon->brightSkin && cg_brightItems.integer == 1 ) {
			barrel.customShader = weapon->brightSkin;
		} else if( cg_brightItems.integer == 2 ){
			barrel.customShader = trap_R_RegisterShader("models/players/flat");
			barrel.shaderRGBA[0] = gun.shaderRGBA[0];
			barrel.shaderRGBA[1] = gun.shaderRGBA[1];
			barrel.shaderRGBA[2] = gun.shaderRGBA[2];
			barrel.shaderRGBA[3] = gun.shaderRGBA[3];
		}

		CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups );
	}
	
	if ( ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ) && ( cg.time < cgs.roundStartTime && cg.warmup == 0)   ){
		return;
	}

	// make sure we aren't looking at cg.predictedPlayerEntity for LG
	nonPredictedCent = &cg_entities[cent->currentState.clientNum];

	// if the index of the nonPredictedCent is not the same as the clientNum
	// then this is a fake player (like on teh single player podiums), so
	// go ahead and use the cent
	if( ( nonPredictedCent - cg_entities ) != cent->currentState.clientNum ) {
		nonPredictedCent = cent;
	}

	// add the flash
	if ( ( weaponNum == WP_LIGHTNING || weaponNum == WP_GAUNTLET || weaponNum == WP_GRAPPLING_HOOK )
		&& ( nonPredictedCent->currentState.eFlags & EF_FIRING ) ) 
	{
		// continuous flash
	} else {
		// impulse flash
		if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME && !cent->pe.railgunFlash ) {
			return;
		}
	}
	
	if( cg_muzzleFlash.integer ) {
		memset( &flash, 0, sizeof( flash ) );
		VectorCopy( parent->lightingOrigin, flash.lightingOrigin );
		flash.shadowPlane = parent->shadowPlane;
		flash.renderfx = parent->renderfx;

		flash.hModel = weapon->flashModel;
		if (!flash.hModel) {
			return;
		}
		angles[YAW] = 0;
		angles[PITCH] = 0;
		angles[ROLL] = crandom() * 10;
		AnglesToAxis( angles, flash.axis );

		// colorize the railgun blast
		if ( weaponNum == WP_RAILGUN ) {
			clientInfo_t	*ci;

			ci = &cgs.clientinfo[ cent->currentState.clientNum ];
			flash.shaderRGBA[0] = 255 * ci->color1[0];
			flash.shaderRGBA[1] = 255 * ci->color1[1];
			flash.shaderRGBA[2] = 255 * ci->color1[2];
		}

		CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_flash");
		trap_R_AddRefEntityToScene( &flash );
	
	} else {
		CG_PositionRotatedEntityOnTag( &flash, &gun, weapon->weaponModel, "tag_flash");
	}

	if ( ps || cg.renderingThirdPerson ||
		cent->currentState.number != cg.predictedPlayerState.clientNum ) {
		// add lightning bolt
		CG_LightningBolt( nonPredictedCent, flash.origin );

		// add rail trail
		CG_SpawnRailTrail( cent, flash.origin );

		if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] ) {
			trap_R_AddLightToScene( flash.origin, 300 + (rand()&31), weapon->flashDlightColor[0],
				weapon->flashDlightColor[1], weapon->flashDlightColor[2] );
		}
	}
}

/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void CG_AddViewWeapon( playerState_t *ps ) {
	refEntity_t	hand;
	centity_t	*cent;
	clientInfo_t	*ci;
	float		fovOffset;
	vec3_t		angles;
	weaponInfo_t	*weapon;

	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	if ( ps->pm_type == PM_INTERMISSION ) {
		return;
	}

	// no gun if in third person view or a camera is active
	//if ( cg.renderingThirdPerson || cg.cameraMode) {
	if ( cg.renderingThirdPerson ) {
		return;
	}


	// allow the gun to be completely removed
	if ( !cg_drawGun.integer ) {
		vec3_t		origin;

		if ( ( cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ) && ( cg.time < cgs.roundStartTime && cg.warmup == 0 ) )
			return;
		
		if ( cg.predictedPlayerState.eFlags & EF_FIRING ) {
			// special hack for lightning gun...
			VectorCopy( cg.refdef.vieworg, origin );
			VectorMA( origin, -8, cg.refdef.viewaxis[2], origin );
			CG_LightningBolt( &cg_entities[ps->clientNum], origin );
		}
		return;
	}

	// don't draw if testing a gun model
	if ( cg.testGun ) {
		return;
	}

	// drop gun lower at higher fov
	if ( cg_fov.integer > 90 ) {
		fovOffset = -0.2 * ( cg_fov.integer - 90 );
	} else {
		fovOffset = 0;
	}

	cent = &cg.predictedPlayerEntity;	// &cg_entities[cg.snap->ps.clientNum];
	CG_RegisterWeapon( ps->weapon );
	weapon = &cg_weapons[ ps->weapon ];

	memset (&hand, 0, sizeof(hand));

	// set up gun position
	CG_CalculateWeaponPosition( hand.origin, angles );

	VectorMA( hand.origin, cg_gun_x.value, cg.refdef.viewaxis[0], hand.origin );
	VectorMA( hand.origin, cg_gun_y.value, cg.refdef.viewaxis[1], hand.origin );
	VectorMA( hand.origin, (cg_gun_z.value+fovOffset), cg.refdef.viewaxis[2], hand.origin );

	AnglesToAxis( angles, hand.axis );

	// map torso animations to weapon animations
	if ( cg_gun_frame.integer ) {
		// development tool
		hand.frame = hand.oldframe = cg_gun_frame.integer;
		hand.backlerp = 0;
	} else {
		// get clientinfo for animation map
		ci = &cgs.clientinfo[ cent->currentState.clientNum ];
		hand.frame = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.frame );
		hand.oldframe = CG_MapTorsoToWeaponFrame( ci, cent->pe.torso.oldFrame );
		hand.backlerp = cent->pe.torso.backlerp;
	}

	hand.hModel = weapon->handsModel;
	hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT;

	// add everything onto the hand
	CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity, ps->persistant[PERS_TEAM] );
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/

/*
===================
CG_DrawWeaponSelect
===================
*/
void CG_DrawWeaponSelect( void ) {
	//int		i;
	int		bits;
	float		*color;
	vec4_t		realColor; 
	
	

	// don't display if dead
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	color = CG_FadeColor( cg.weaponSelectTime, cgs.hud[HUD_WEAPONLIST].time );

	//Elimination: Always show weapon bar
	if(!cgs.hud[HUD_WEAPONLIST].time) {
		realColor[0] = 1.0;
		realColor[1] = 1.0;
		realColor[2] = 1.0;
		realColor[3] = 1.0;
		color = realColor;
	}

	if ( !color ) {
		return;
	}
	
	trap_R_SetColor( color );

	// showing weapon select clears pickup item display, but not the blend blob
	cg.itemPickupTime = 0;

	// count the number of weapons owned
	bits = cg.snap->ps.stats[ STAT_WEAPONS ];
	
	
	if( cgs.hud[HUD_WEAPONLIST].inuse )
		CG_DrawWeaponBar( 0, bits, color );
	trap_R_SetColor(NULL);
	return;
}

void CG_DrawWeaponBar( int count, int bits, float *color ){
	hudElements_t hudelement = cgs.hud[HUD_WEAPONLIST];
	int height, width, xpos, ypos, textalign;
	qboolean horizontal;
	int boxWidth, boxHeight, x,y;
	int i;
	int iconsize;
	int icon_xrel, icon_yrel, text_xrel, text_yrel, text_step;
	char* s;
	int w;
	vec4_t charColor;
	int ammoPack;
	vec4_t bgcolor;
	
	charColor[3] = color[3];
	
	height = hudelement.height;
	width = hudelement.width;
	xpos = hudelement.xpos;
	ypos = hudelement.ypos;
	textalign = hudelement.textAlign;
	
	horizontal = height < width;
	
	count = 0;
	for( i = WP_MACHINEGUN ; i <= WP_BFG; i++ ){
		if( cg_weapons[i].weaponIcon )
			count++;
	}
	if( hudelement.bgcolor[3] != 0 ){
		if( cgs.gametype >= GT_TEAM && hudelement.teamBgColor == 1 ){
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
				bgcolor[0] = 1;
				bgcolor[1] = 0;
				bgcolor[2] = 0;
			} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
				bgcolor[0] = 0;
				bgcolor[1] = 0;
				bgcolor[2] = 1;
			}
		}
		else if( cgs.gametype >= GT_TEAM && hudelement.teamBgColor == 2 ){
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
				bgcolor[0] = 1;
				bgcolor[1] = 0;
				bgcolor[2] = 0;
			} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
				bgcolor[0] = 0;
				bgcolor[1] = 0;
				bgcolor[2] = 1;
			}
		}
	
		else{
			bgcolor[0] = hudelement.bgcolor[0];
			bgcolor[1] = hudelement.bgcolor[1];
			bgcolor[2] = hudelement.bgcolor[2];
		}
		bgcolor[3] = hudelement.bgcolor[3];
	}
	else{
		bgcolor[0] = 0;
		bgcolor[1] = 0;
		bgcolor[2] = 0;
		bgcolor[3] = 0;
	}
		
	
	if( horizontal ){
		boxHeight = height;
		boxWidth = width/8;
		x = xpos + width/2 - (boxWidth*count)/2;
		y = ypos;
		CG_FillRect( x, y, count*boxWidth, boxHeight, bgcolor );
	}
	else{
		boxHeight = height/8;
		boxWidth = width;
		x = xpos;
		y = ypos + height/2 - (boxHeight*count)/2;
		CG_FillRect( x, y, boxWidth, count*boxHeight, bgcolor);
	}
	
	if( textalign == 0 ){
			if( boxHeight < ( boxWidth - 3*hudelement.fontWidth - 6 ) )
				iconsize = boxHeight;
			else
				iconsize = ( boxWidth - 3*hudelement.fontWidth - 6 );
			
			icon_yrel = boxHeight/2 - iconsize/2;
			icon_xrel = boxWidth - iconsize - 2;
			
			text_yrel = boxHeight/2 - hudelement.fontHeight/2;
			text_xrel = 2;
			text_step = 0;
	}
	else if( textalign == 2 ){
			if( boxHeight < ( boxWidth - 3*hudelement.fontWidth - 6 ) )
				iconsize = boxHeight;
			else
				iconsize = ( boxWidth - 3*hudelement.fontWidth - 6 );
			
			icon_yrel = boxHeight/2 - iconsize/2;
			icon_xrel = 2;
			
			text_yrel = boxHeight/2 - hudelement.fontHeight/2;
			text_xrel = boxWidth - 3*hudelement.fontWidth - 2;
			text_step = hudelement.fontWidth;
	}
	else{
			if( boxWidth < ( boxHeight - hudelement.fontHeight - 6 ) )
				iconsize = boxWidth;
			else
				iconsize = ( boxHeight - hudelement.fontHeight - 6 );
			
			icon_xrel = boxWidth/2 - iconsize/2;
			icon_yrel = 2;
			
			text_xrel = boxWidth/2 - 3*hudelement.fontWidth/2;
			text_yrel = boxHeight - hudelement.fontHeight - 2;
			text_step = hudelement.fontWidth/2;
	}
	
	if( iconsize < 0 )
		iconsize = 0;
			
	
		
		for( i = WP_MACHINEGUN; i <= WP_BFG; i++ ){
			if( cg_weapons[i].weaponIcon ){
				
				switch( i ){
					case WP_MACHINEGUN:
						ammoPack = 50;
						break;	
					case WP_SHOTGUN:
						ammoPack = 10;
						break;
					case WP_GRENADE_LAUNCHER:
						ammoPack = 5;
						break;
					case WP_ROCKET_LAUNCHER:
						ammoPack = 5;
						break;
					case WP_LIGHTNING:
						ammoPack = 60;
						break;
					case WP_RAILGUN:
						ammoPack = 5;
						break;
					case WP_PLASMAGUN:
						ammoPack = 30;
						break;
					case WP_BFG:
						ammoPack = 15;
						break;
					default: 
						ammoPack = 200;
						break;
				}
			
				if ( cg.snap->ps.ammo[ i ] < ammoPack/2+1 ){
					charColor[0] = 1.0;
					charColor[1] = 0.0;
					charColor[2] = 0.0;
				} else if ( cg.snap->ps.ammo[ i ] < ammoPack ){
					charColor[0] = 1.0;
					charColor[1] = 1.0;
					charColor[2] = 0.0;
				} else{
					charColor[0] = 1.0;
					charColor[1] = 1.0;
					charColor[2] = 1.0;
				}
				
				if ( ( ( i == cg.weaponSelect ) && !(cg.snap->ps.pm_flags & PMF_FOLLOW) ) || ( ( i == cg_entities[cg.snap->ps.clientNum].currentState.weapon ) && (cg.snap->ps.pm_flags & PMF_FOLLOW) ) ) {
					if( hudelement.imageHandle )
						CG_DrawPic( x, y, boxWidth, boxHeight, hudelement.imageHandle );
					else if( hudelement.fill )
						CG_FillRect( x, y, boxWidth, boxHeight, hudelement.color );
					else
						CG_DrawRect( x, y, boxWidth, boxHeight, 2, hudelement.color );
				}
				
				CG_DrawPic( x + icon_xrel, y + icon_yrel, iconsize, iconsize, cg_weapons[i].weaponIcon );
				
				if ( ( bits & ( 1 << i ) ) ) {
					if(cg.snap->ps.ammo[i] == 0){
						CG_DrawPic( x + icon_xrel, y + icon_yrel, iconsize, iconsize, cgs.media.noammoShader );
					}	
			
					/** Draw Weapon Ammo **/
					if(cg.snap->ps.ammo[ i ]!=-1){
						s = va("%i", cg.snap->ps.ammo[ i ] );
						w = CG_DrawStrlen( s );
						CG_DrawStringExt( x + text_xrel + ( 3 - w )*text_step, y + text_yrel, s, charColor, qfalse, hudelement.textstyle & 1, hudelement.fontWidth, hudelement.fontHeight, 0 ); 
					}
				}
			
				if( horizontal )
					x += boxWidth;
				else
					y += boxHeight;
			}
		}
	
	
	
	
	
	
	
}

/*
===============
CG_WeaponSelectable
===============
*/
static qboolean CG_WeaponSelectable( int i ) {
	if ( !cg.snap->ps.ammo[i] ) {
		return qfalse;
	}
	if ( ! (cg.snap->ps.stats[ STAT_WEAPONS ] & ( 1 << i ) ) ) {
		return qfalse;
	}

	return qtrue;
}

/*
===============
CG_NextWeapon_f
===============
*/
void CG_NextWeapon_f( void ) {
	int		i;
	int		original;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;
        //Part of mad hook select code:
        if(cg.weaponSelect == WP_GRAPPLING_HOOK)
            cg.weaponSelect = 0;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		cg.weaponSelect++;
		if ( cg.weaponSelect == MAX_WEAPONS ) {
			cg.weaponSelect = 0;
		}
		if ( cg.weaponSelect == WP_GAUNTLET ) {
			continue;		// never cycle to gauntlet
		}
                //Sago: Mad change of grapple order
                if( cg.weaponSelect == WP_GRAPPLING_HOOK)  {
                    continue;
                }
                if( cg.weaponSelect == 0)
                    cg.weaponSelect = WP_GRAPPLING_HOOK;
                if ( cg.weaponSelect == WP_GRAPPLING_HOOK && !cg_cyclegrapple.integer ) {
                    cg.weaponSelect = 0;
                    continue;		// never cycle to grapple unless the client wants it
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
                if( cg.weaponSelect == WP_GRAPPLING_HOOK)
                    cg.weaponSelect = 0;
	}
	if ( i == MAX_WEAPONS ) {
		cg.weaponSelect = original;
	}
	
	for( i = 0 ; i < WEAPON_COUNT; i++ )
		cg.nextweapons[i] = 0;
}

/*
===============
CG_PrevWeapon_f
===============
*/
void CG_PrevWeapon_f( void ) {
	int		i;
	int		original;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	cg.weaponSelectTime = cg.time;
	original = cg.weaponSelect;
        //Part of mad hook select code:
        if(cg.weaponSelect == WP_GRAPPLING_HOOK)
            cg.weaponSelect = 0;

	for ( i = 0 ; i < MAX_WEAPONS ; i++ ) {
		cg.weaponSelect--;
		if ( cg.weaponSelect == -1 ) {
			cg.weaponSelect = MAX_WEAPONS - 1;
		}
		if ( cg.weaponSelect == WP_GAUNTLET ) {
			continue;		// never cycle to gauntlet
		}
                //Sago: Mad change of grapple order
                if( cg.weaponSelect == WP_GRAPPLING_HOOK)  {
                    continue;
                }
                if( cg.weaponSelect == 0)
                    cg.weaponSelect = WP_GRAPPLING_HOOK;
                if ( cg.weaponSelect == WP_GRAPPLING_HOOK && !cg_cyclegrapple.integer ) {
                    cg.weaponSelect = 0;
                    continue;		// never cycle to grapple unless the client wants it
		}
		if ( CG_WeaponSelectable( cg.weaponSelect ) ) {
			break;
		}
                if( cg.weaponSelect == WP_GRAPPLING_HOOK)
                    cg.weaponSelect = 0;
	}
	if ( i == MAX_WEAPONS ) {
		cg.weaponSelect = original;
	}
	
	for( i = 0 ; i < WEAPON_COUNT; i++ )
		cg.nextweapons[i] = 0;
}

/*
===============
CG_Weapon_f
===============
*/
void CG_Weapon_f( void ) {
	int		num;
	int		i;
	qboolean	weaponFound = qfalse;

	if ( !cg.snap ) {
		return;
	}
	if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) {
		return;
	}

	for( i = 0; i < WEAPON_COUNT; i++ ){
		cg.nextweapons[i] = atoi( CG_Argv( i + 1 ) );
		if( !weaponFound ){
			num = cg.nextweapons[i];
			
			if ( num < 1 || num > MAX_WEAPONS-1 ) {
				continue;
			}
			if( cg_noAmmoChange.integer ){
				if ( ! ( cg.snap->ps.stats[STAT_WEAPONS] & ( 1 << num ) ) )
					continue;
			}
			else{
				if( !CG_WeaponSelectable( num ) )
					continue;
			}
			
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = num;
			weaponFound = qtrue;
		}
	}
	
	/*
	num = atoi( CG_Argv( 1 ) );

	if ( num < 1 || num > MAX_WEAPONS-1 ) {
		return;
	}

	cg.weaponSelectTime = cg.time;

	if ( ! ( cg.snap->ps.stats[STAT_WEAPONS] & ( 1 << num ) ) ) {
		return;		// don't have the weapon
	}

	cg.weaponSelect = num;*/
	
}

/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void CG_OutOfAmmoChange( void ) {
	int		i;
	
	cg.weaponSelectTime = cg.time;
	
	for( i = 0; i < WEAPON_COUNT; i++ ){
		if ( CG_WeaponSelectable( cg.nextweapons[i] ) && cg.nextweapons[i] != WP_GRAPPLING_HOOK ) {
			cg.weaponSelect = cg.nextweapons[i];
			return;
		}
	}
	
	for ( i = MAX_WEAPONS-1 ; i > 0 ; i-- ) {
		if ( CG_WeaponSelectable( i ) && i != WP_GRAPPLING_HOOK ) {
			cg.weaponSelect = i;
			break;
		}
	}
}


#define ACCBOARD_XPOS 500
#define ACCBOARD_YPOS 150
#define ACCBOARD_HEIGHT 20
#define ACCBOARD_WIDTH 75
#define ACCITEM_SIZE 16

qboolean CG_DrawAccboard( void ) {
	int counter, i;
	
	i = 0;

	if( !cg.showAcc ){
		return qfalse;
	}
	trap_R_SetColor( colorWhite );
	
	for( counter = 0; counter < 8 ; counter++ ){
		if( cg_weapons[counter+2].weaponIcon )
			i++;
	}

	CG_DrawPic( ACCBOARD_XPOS, ACCBOARD_YPOS, ACCBOARD_WIDTH, 20*(i + 1), cgs.media.accBackground );
	
	i = 0;

	for( counter = 0 ; counter < 8 ; counter++ ){
		if( cg_weapons[counter+2].weaponIcon ){
			CG_DrawPic( ACCBOARD_XPOS + 10, ACCBOARD_YPOS + 10 +i*20, ACCITEM_SIZE, ACCITEM_SIZE, cg_weapons[counter+2].weaponIcon );
			if( cg.accuracys[counter][0] > 0 )
				CG_DrawSmallStringColor(ACCBOARD_XPOS + 10 + ACCITEM_SIZE + 10, ACCBOARD_YPOS + 10 +i*20 + ACCITEM_SIZE/2 - SMALLCHAR_HEIGHT/2 ,
					 va("%i%s",(int)(((float)cg.accuracys[counter][1]*100)/((float)(cg.accuracys[counter][0]))),"%"), colorWhite);
			else
				CG_DrawSmallStringColor(ACCBOARD_XPOS + 10 + ACCITEM_SIZE + 10, ACCBOARD_YPOS + 10 +i*20 + ACCITEM_SIZE/2 - SMALLCHAR_HEIGHT/2 , "-%", colorWhite);
			i++;
		}
	}

	trap_R_SetColor(NULL);
	return qtrue;
}



/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/

/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event
================
*/
void CG_FireWeapon( centity_t *cent ) {
	entityState_t *ent;
	int				c;
	weaponInfo_t	*weap;

	if((cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION) && ( cgs.roundStartTime>=cg.time ) && cg.warmup == 0 )
		return; //if we havn't started in ELIMINATION then do not fire
		

	ent = &cent->currentState;
	if ( ent->weapon == WP_NONE ) {
		return;
	}
	if ( ent->weapon >= WP_NUM_WEAPONS ) {
		CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
		return;
	}
	weap = &cg_weapons[ ent->weapon ];

	// mark the entity as muzzle flashing, so when it is added it will
	// append the flash to the weapon model
	cent->muzzleFlashTime = cg.time;

	// lightning gun only does this this on initial press
	if ( ent->weapon == WP_LIGHTNING ) {
		if ( cent->pe.lightningFiring ) {
			return;
		}
	}

	// play quad sound if needed
	if ( cent->currentState.powerups & ( 1 << PW_QUAD ) ) {
		trap_S_StartSound (NULL, cent->currentState.number, CHAN_ITEM, cgs.media.quadSound );
	}

	// play a sound
	for ( c = 0 ; c < 4 ; c++ ) {
		if ( !weap->flashSound[c] ) {
			break;
		}
	}
	if ( c > 0 ) {
		c = rand() % c;
		if ( weap->flashSound[c] )
		{
			trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, weap->flashSound[c] );
		}
	}

	// do brass ejection
	if ( weap->ejectBrassFunc && cg_brassTime.integer > 0 ) {
		weap->ejectBrassFunc( cent );
	}

//unlagged - attack prediction #1
	CG_PredictWeaponEffects( cent );
//unlagged - attack prediction #1
}

/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing
=================
*/
void CG_MissileHitWall( int weapon, int clientNum, vec3_t origin, vec3_t dir, impactSound_t soundType ) {
	qhandle_t		mod;
	qhandle_t		mark;
	qhandle_t		shader;
	sfxHandle_t		sfx;
	float			radius;
	float			light;
	vec3_t			lightColor;
	localEntity_t	*le;
	int				r;
	qboolean		alphaFade;
	qboolean		isSprite;
	int				duration;

	mark = 0;
	radius = 32;
	sfx = 0;
	mod = 0;
	shader = 0;
	light = 0;
	lightColor[0] = 1;
	lightColor[1] = 1;
	lightColor[2] = 0;

	// set defaults
	isSprite = qfalse;
	duration = 600;

	switch ( weapon ) {
	default:
#ifdef MISSIONPACK
	case WP_NAILGUN:
		if( soundType == IMPACTSOUND_FLESH ) {
			sfx = cgs.media.sfx_nghitflesh;
		} else if( soundType == IMPACTSOUND_METAL ) {
			sfx = cgs.media.sfx_nghitmetal;
		} else {
			sfx = cgs.media.sfx_nghit;
		}
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;
#endif
	case WP_LIGHTNING:
		// no explosion at LG impact, it is added with the beam
		if( cg_lgHitSfx.integer ){
			r = rand() & 3;
			if ( r < 2 ) {
				sfx = cgs.media.sfx_lghit2;
			} else if ( r == 2 ) {
				sfx = cgs.media.sfx_lghit1;
			} else {
				sfx = cgs.media.sfx_lghit3;
			}
		}
		mark = cgs.media.holeMarkShader;
		radius = 12;
		break;
#ifdef MISSIONPACK
	case WP_PROX_LAUNCHER:
		mod = cgs.media.dishFlashModel;
		shader = cgs.media.grenadeExplosionShader;
		sfx = cgs.media.sfx_proxexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		break;
#endif
	case WP_GRENADE_LAUNCHER:
		if( cg_explosion.integer & 1 << ( WP_GRENADE_LAUNCHER - 2 ) ){
			mod = cgs.media.dishFlashModel;
			shader = cgs.media.grenadeExplosionShader;
		}
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		break;
	case WP_ROCKET_LAUNCHER:
		if( cg_explosion.integer & 1 << ( WP_ROCKET_LAUNCHER - 2 ) ){
			mod = cgs.media.dishFlashModel;
			shader = cgs.media.rocketExplosionShader;
		}
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 64;
		light = 300;
		isSprite = qtrue;
		duration = 1000;
		lightColor[0] = 1;
		lightColor[1] = 0.75;
		lightColor[2] = 0.0;
		/*if (!cg_oldRocket.integer) {
			// explosion sprite animation
			VectorMA( origin, 24, dir, sprOrg );
			VectorScale( dir, 64, sprVel );

			CG_ParticleExplosion( "explode1", sprOrg, sprVel, 1400, 20, 30 );
		}*/
		break;
	case WP_RAILGUN:
		if( cg_explosion.integer & 1 << ( WP_RAILGUN - 2 ) ){
			mod = cgs.media.ringFlashModel;
			shader = cgs.media.railExplosionShader;
		}
		sfx = cgs.media.sfx_plasmaexp;
		mark = cgs.media.energyMarkShader;
		radius = 24;
		break;
	case WP_PLASMAGUN:
		if( cg_explosion.integer & 1 << ( WP_PLASMAGUN - 2 ) ){
			mod = cgs.media.plasmaExplosionModel;
			shader = cgs.media.plasmaExplosionShader;
		}
		sfx = cgs.media.sfx_plasmaexp;
		mark = cgs.media.energyMarkShader;
		radius = 16;
		light = 40;		//maybe later some light
		lightColor[0] = 0.6f;
		lightColor[1] = 0.6f;
		lightColor[2] = 1.0f;
		break;
	case WP_BFG:
		if( cg_explosion.integer & 1 << ( WP_BFG - 2 ) ){
			mod = cgs.media.dishFlashModel;
			shader = cgs.media.bfgExplosionShader;
		}
		sfx = cgs.media.sfx_rockexp;
		mark = cgs.media.burnMarkShader;
		radius = 32;
		isSprite = qtrue;
		break;
	case WP_SHOTGUN:
		if( cg_explosion.integer & 1 << ( WP_SHOTGUN - 2 ) ){
			mod = cgs.media.bulletFlashModel;
			shader = cgs.media.bulletExplosionShader;
		}
		mark = cgs.media.bulletMarkShader;
		sfx = 0;
		radius = 4;
		light = 30;
		duration = 100;
		lightColor[0] = 1;
		lightColor[1] = 0.75;
		lightColor[2] = 0.0;
		break;

#ifdef MISSIONPACK
	case WP_CHAINGUN:
		mod = cgs.media.bulletFlashModel;
		if( soundType == IMPACTSOUND_FLESH ) {
			sfx = cgs.media.sfx_chghitflesh;
		} else if( soundType == IMPACTSOUND_METAL ) {
			sfx = cgs.media.sfx_chghitmetal;
		} else {
			sfx = cgs.media.sfx_chghit;
		}
		mark = cgs.media.bulletMarkShader;

		r = rand() & 3;
		if ( r < 2 ) {
			sfx = cgs.media.sfx_ric1;
		} else if ( r == 2 ) {
			sfx = cgs.media.sfx_ric2;
		} else {
			sfx = cgs.media.sfx_ric3;
		}

		radius = 8;
		light = 30;
		duration = 100;
		lightColor[0] = 1;
		lightColor[1] = 0.75;
		lightColor[2] = 0.0;
		break;
#endif

	case WP_MACHINEGUN:
		if( cg_explosion.integer & 1 << ( WP_MACHINEGUN - 2 ) ){
			mod = cgs.media.bulletFlashModel;
			shader = cgs.media.bulletExplosionShader;
		}
		mark = cgs.media.bulletMarkShader;

		r = rand() & 3;
		if ( r == 0 ) {
			sfx = cgs.media.sfx_ric1;
		} else if ( r == 1 ) {
			sfx = cgs.media.sfx_ric2;
		} else {
			sfx = cgs.media.sfx_ric3;
		}

		radius = 6;
		light = 30;
		duration = 100;
		lightColor[0] = 1;
		lightColor[1] = 0.75;
		lightColor[2] = 0.0;
		break;
	}

	if ( sfx ) {
		trap_S_StartSound( origin, ENTITYNUM_WORLD, CHAN_AUTO, sfx );
	}

	//
	// create the explosion
	//
	if ( mod ) {
		le = CG_MakeExplosion( origin, dir, 
							   mod,	shader,
							   duration, isSprite );
		le->light = light;
		VectorCopy( lightColor, le->lightColor );
		if ( weapon == WP_RAILGUN ) {
			// colorize with client color
			VectorCopy( cgs.clientinfo[clientNum].color1, le->color );
		}
	}

	//
	// impact mark
	//
	alphaFade = (mark == cgs.media.energyMarkShader);	// plasma fades alpha, all others fade color
	if ( weapon == WP_RAILGUN ) {
		float	*color;
		vec3_t	impactColor;

		// colorize with client color
		color = cgs.clientinfo[clientNum].color1;
		CG_ImpactMark( mark, origin, dir, random()*360, color[0],color[1], color[2],1, alphaFade, radius, qfalse );
		impactColor[0] = color[0];
		impactColor[1] = color[1];
		impactColor[2] = color[2];
		CG_ExplosionParticles(weapon, origin, dir, impactColor);
	} else {
		CG_ImpactMark( mark, origin, dir, random()*360, 1,1,1,1, alphaFade, radius, qfalse );
		CG_ExplosionParticles(weapon, origin, dir, NULL);
	}
}


/*
=================
CG_MissileHitPlayer
=================
*/
void CG_MissileHitPlayer( int weapon, vec3_t origin, vec3_t dir, int entityNum ) {
	CG_Bleed( origin, entityNum );

	// some weapons will make an explosion with the blood, while
	// others will just make the blood
	switch ( weapon ) {
	case WP_GRENADE_LAUNCHER:
	case WP_ROCKET_LAUNCHER:
#ifdef MISSIONPACK
	case WP_NAILGUN:
	case WP_CHAINGUN:
	case WP_PROX_LAUNCHER:
#endif
		CG_MissileHitWall( weapon, 0, origin, dir, IMPACTSOUND_FLESH );
		break;
	default:
		break;
	}
}



/*
============================================================================

SHOTGUN TRACING

============================================================================
*/

/*
================
CG_ShotgunPellet
================
*/
static void CG_ShotgunPellet( vec3_t start, vec3_t end, int skipNum ) {
	trace_t		tr;
	int sourceContentType, destContentType;

	CG_Trace( &tr, start, NULL, NULL, end, skipNum, MASK_SHOT );

	sourceContentType = trap_CM_PointContents( start, 0 );
	destContentType = trap_CM_PointContents( tr.endpos, 0 );

	// FIXME: should probably move this cruft into CG_BubbleTrail
	if ( sourceContentType == destContentType ) {
		if ( sourceContentType & CONTENTS_WATER ) {
			CG_BubbleTrail( start, tr.endpos, 32 );
		}
	} else if ( sourceContentType & CONTENTS_WATER ) {
		trace_t trace;

		trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
		CG_BubbleTrail( start, trace.endpos, 32 );
	} else if ( destContentType & CONTENTS_WATER ) {
		trace_t trace;

		trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
		CG_BubbleTrail( tr.endpos, trace.endpos, 32 );
	}

	if (  tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	if ( cg_entities[tr.entityNum].currentState.eType == ET_PLAYER ) {
		CG_MissileHitPlayer( WP_SHOTGUN, tr.endpos, tr.plane.normal, tr.entityNum );
	} else {
		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			// SURF_NOIMPACT will not make a flame puff or a mark
			return;
		}
		if ( tr.surfaceFlags & SURF_METALSTEPS ) {
			CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_METAL );
		} else {
			CG_MissileHitWall( WP_SHOTGUN, 0, tr.endpos, tr.plane.normal, IMPACTSOUND_DEFAULT );
		}
	}
}

/*
================
CG_ShotgunPattern

Perform the same traces the server did to locate the
hit splashes
================
*/
//unlagged - attack prediction
// made this non-static for access from cg_unlagged.c
void CG_ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, int otherEntNum ) {
	int			i;
	float		r, u;
	vec3_t		end;
	vec3_t		forward, right, up;
	int		outery, outerx;
	
	outery = sin(60 * (2*M_PI/360 ) ) * OUTERRADIUS;
	outerx = cos(60 * (2*M_PI/360 ) ) * OUTERRADIUS;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	// generate the "random" spread pattern
	for ( i = 0 ; i < DEFAULT_SHOTGUN_COUNT ; i++ ) {
		/*r = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		u = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;*/
		
		//NEW sg-pattern
		switch(i){
			case 0: 
				r = OUTERRADIUS;
				u = 0;
				break;
			case 1: 
				r = outerx;
				u = outery;
				break;
			case 2: 
				r = -outerx;
				u = outery;
				break;
			case 3: 
				r = -OUTERRADIUS;
				u = 0;
				break;
			case 4: 
				r = -outerx;
				u = -outery;
				break;
			case 5: 
				r = outerx;
				u = -outery;
				break;
			case 6: 
				r = INNERRADIUS;
				u = INNERRADIUS;
				break;
			case 7: 
				r = -INNERRADIUS;
				u = INNERRADIUS;
				break;
			case 8: 
				r = -INNERRADIUS;
				u = -INNERRADIUS;
				break;
			case 9: 
				r = INNERRADIUS;
				u = -INNERRADIUS;
				break;
			case 10: 
				r = 0;
				u = 0;
				break;
			default:
				r = 0;
				u = 0;
		}
		
		r += Q_crandom( &seed ) * ( DEFAULT_SHOTGUN_SPREAD * 16 - OUTERRADIUS );
		u += Q_crandom( &seed ) * ( DEFAULT_SHOTGUN_SPREAD * 16 - OUTERRADIUS );
		//NEW sg-pattern
		
		VectorMA( origin, 8192 * 16, forward, end);
		VectorMA (end, r, right, end);
		VectorMA (end, u, up, end);

		CG_ShotgunPellet( origin, end, otherEntNum );
	}
}

/*
==============
CG_ShotgunFire
==============
*/
void CG_ShotgunFire( entityState_t *es ) {
	vec3_t	v;
	int		contents;

	VectorSubtract( es->origin2, es->pos.trBase, v );
	VectorNormalize( v );
	VectorScale( v, 32, v );
	VectorAdd( es->pos.trBase, v, v );
	if ( cg_smoke_SG.integer && cgs.glconfig.hardwareType != GLHW_RAGEPRO ) {
		// ragepro can't alpha fade, so don't even bother with smoke
		vec3_t			up;

		contents = trap_CM_PointContents( es->pos.trBase, 0 );
		if ( !( contents & CONTENTS_WATER ) ) {
			VectorSet( up, 0, 0, 8 );
			CG_SmokePuff( v, up, 32, 1, 1, 1, 0.33f, 900, cg.time, 0, LEF_PUFF_DONT_SCALE, cgs.media.shotgunSmokePuffShader );
		}
	}
	CG_ShotgunPattern( es->pos.trBase, es->origin2, es->eventParm, es->otherEntityNum );
}

/*
============================================================================

BULLETS

============================================================================
*/


/*
===============
CG_Tracer
===============
*/
void CG_Tracer( vec3_t source, vec3_t dest ) {
	vec3_t		forward, right;
	polyVert_t	verts[4];
	vec3_t		line;
	float		len, begin, end;
	vec3_t		start, finish;
	vec3_t		midpoint;

	// tracer
	VectorSubtract( dest, source, forward );
	len = VectorNormalize( forward );

	// start at least a little ways from the muzzle
	if ( len < 100 ) {
		return;
	}
	begin = 50 + random() * (len - 60);
	end = begin + cg_tracerLength.value;
	if ( end > len ) {
		end = len;
	}
	VectorMA( source, begin, forward, start );
	VectorMA( source, end, forward, finish );

	line[0] = DotProduct( forward, cg.refdef.viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef.viewaxis[2] );

	VectorScale( cg.refdef.viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef.viewaxis[2], right );
	VectorNormalize( right );

	VectorMA( finish, cg_tracerWidth.value, right, verts[0].xyz );
	verts[0].st[0] = 0;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	VectorMA( finish, -cg_tracerWidth.value, right, verts[1].xyz );
	verts[1].st[0] = 1;
	verts[1].st[1] = 0;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	VectorMA( start, -cg_tracerWidth.value, right, verts[2].xyz );
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	VectorMA( start, cg_tracerWidth.value, right, verts[3].xyz );
	verts[3].st[0] = 0;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;

	trap_R_AddPolyToScene( cgs.media.tracerShader, 4, verts );

	midpoint[0] = ( start[0] + finish[0] ) * 0.5;
	midpoint[1] = ( start[1] + finish[1] ) * 0.5;
	midpoint[2] = ( start[2] + finish[2] ) * 0.5;

	// add the tracer sound
	trap_S_StartSound( midpoint, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.tracerSound );

}


/*
======================
CG_CalcMuzzlePoint
======================
*/
static qboolean	CG_CalcMuzzlePoint( int entityNum, vec3_t muzzle ) {
	vec3_t		forward;
	centity_t	*cent;
	int			anim;

	if ( entityNum == cg.snap->ps.clientNum ) {
		VectorCopy( cg.snap->ps.origin, muzzle );
		muzzle[2] += cg.snap->ps.viewheight;
		AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
		VectorMA( muzzle, 14, forward, muzzle );
		return qtrue;
	}

	cent = &cg_entities[entityNum];
	if ( !cent->currentValid ) {
		return qfalse;
	}

	VectorCopy( cent->currentState.pos.trBase, muzzle );

	AngleVectors( cent->currentState.apos.trBase, forward, NULL, NULL );
	anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;
	if ( anim == LEGS_WALKCR || anim == LEGS_IDLECR ) {
		muzzle[2] += CROUCH_VIEWHEIGHT;
	} else {
		muzzle[2] += DEFAULT_VIEWHEIGHT;
	}

	VectorMA( muzzle, 14, forward, muzzle );

	return qtrue;

}

/*
======================
CG_Bullet

Renders bullet effects.
======================
*/
void CG_Bullet( vec3_t end, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum ) {
	trace_t trace;
	int sourceContentType, destContentType;
	vec3_t		start;

	// if the shooter is currently valid, calc a source point and possibly
	// do trail effects
	if ( sourceEntityNum >= 0 && cg_tracerChance.value > 0 ) {
		if ( CG_CalcMuzzlePoint( sourceEntityNum, start ) ) {
			sourceContentType = trap_CM_PointContents( start, 0 );
			destContentType = trap_CM_PointContents( end, 0 );

			// do a complete bubble trail if necessary
			if ( ( sourceContentType == destContentType ) && ( sourceContentType & CONTENTS_WATER ) ) {
				CG_BubbleTrail( start, end, 32 );
			}
			// bubble trail from water into air
			else if ( ( sourceContentType & CONTENTS_WATER ) ) {
				trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
				CG_BubbleTrail( start, trace.endpos, 32 );
			}
			// bubble trail from air into water
			else if ( ( destContentType & CONTENTS_WATER ) ) {
				trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
				CG_BubbleTrail( trace.endpos, end, 32 );
			}

			// draw a tracer
			if ( random() < cg_tracerChance.value ) {
				CG_Tracer( start, end );
			}
		}
	}

	// impact splash and mark
	if ( flesh ) {
		CG_Bleed( end, fleshEntityNum );
	} else {
		CG_MissileHitWall( WP_MACHINEGUN, 0, end, normal, IMPACTSOUND_DEFAULT );
	}

}
