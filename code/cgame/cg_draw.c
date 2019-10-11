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
// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"

#ifdef MISSIONPACK
#include "../ui/ui_shared.h"

// used for scoreboard
extern displayContextDef_t cgDC;
menuDef_t *menuScoreboard = NULL;
#else
int drawTeamOverlayModificationCount = -1;
#endif

int sortedTeamPlayers[TEAM_MAXOVERLAY];
int	numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

#ifdef MISSIONPACK

int CG_Text_Width ( const char *text, float scale, int limit ) {
    int count,len;
    float out;
    glyphInfo_t *glyph;
    float useScale;
    const char *s = text;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if ( scale <= cg_smallFont.value ) {
        font = &cgDC.Assets.smallFont;
    } else if ( scale > cg_bigFont.value ) {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    out = 0;
    if ( text ) {
        len = strlen ( text );
        if ( limit > 0 && len > limit ) {
            len = limit;
        }
        count = 0;
        while ( s && *s && count < len ) {
            if ( Q_IsColorString ( s ) ) {
                s += 2;
                continue;
            } else {
                glyph = &font->glyphs[*s & 255];
                out += glyph->xSkip;
                s++;
                count++;
            }
        }
    }
    return out * useScale;
}

int CG_Text_Height ( const char *text, float scale, int limit ) {
    int len, count;
    float max;
    glyphInfo_t *glyph;
    float useScale;
    const char *s = text;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if ( scale <= cg_smallFont.value ) {
        font = &cgDC.Assets.smallFont;
    } else if ( scale > cg_bigFont.value ) {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    max = 0;
    if ( text ) {
        len = strlen ( text );
        if ( limit > 0 && len > limit ) {
            len = limit;
        }
        count = 0;
        while ( s && *s && count < len ) {
            if ( Q_IsColorString ( s ) ) {
                s += 2;
                continue;
            } else {
                glyph = &font->glyphs[*s & 255];
                if ( max < glyph->height ) {
                    max = glyph->height;
                }
                s++;
                count++;
            }
        }
    }
    return max * useScale;
}

void CG_Text_PaintChar ( float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader ) {
    float w, h;
    w = width * scale;
    h = height * scale;
    CG_AdjustFrom640 ( &x, &y, &w, &h );
    trap_R_DrawStretchPic ( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint ( float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style ) {
    int len, count;
    vec4_t newColor;
    glyphInfo_t *glyph;
    float useScale;
    fontInfo_t *font = &cgDC.Assets.textFont;
    if ( scale <= cg_smallFont.value ) {
        font = &cgDC.Assets.smallFont;
    } else if ( scale > cg_bigFont.value ) {
        font = &cgDC.Assets.bigFont;
    }
    useScale = scale * font->glyphScale;
    if ( text ) {
        const char *s = text;
        trap_R_SetColor ( color );
        memcpy ( &newColor[0], &color[0], sizeof ( vec4_t ) );
        len = strlen ( text );
        if ( limit > 0 && len > limit ) {
            len = limit;
        }
        count = 0;
        while ( s && *s && count < len ) {
            glyph = &font->glyphs[*s & 255];
            //int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
            //float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
            if ( Q_IsColorString ( s ) ) {
                memcpy ( newColor, g_color_table[ColorIndex ( * ( s+1 ) ) ], sizeof ( newColor ) );
                newColor[3] = color[3];
                trap_R_SetColor ( newColor );
                s += 2;
                continue;
            } else {
                float yadj = useScale * glyph->top;
                if ( style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
                    int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
                    colorBlack[3] = newColor[3];
                    trap_R_SetColor ( colorBlack );
                    CG_Text_PaintChar ( x + ofs, y - yadj + ofs,
                                        glyph->imageWidth,
                                        glyph->imageHeight,
                                        useScale,
                                        glyph->s,
                                        glyph->t,
                                        glyph->s2,
                                        glyph->t2,
                                        glyph->glyph );
                    colorBlack[3] = 1.0;
                    trap_R_SetColor ( newColor );
                }
                CG_Text_PaintChar ( x, y - yadj,
                                    glyph->imageWidth,
                                    glyph->imageHeight,
                                    useScale,
                                    glyph->s,
                                    glyph->t,
                                    glyph->s2,
                                    glyph->t2,
                                    glyph->glyph );
                // CG_DrawPic(x, y - yadj, scale * cgDC.Assets.textFont.glyphs[text[i]].imageWidth, scale * cgDC.Assets.textFont.glyphs[text[i]].imageHeight, cgDC.Assets.textFont.glyphs[text[i]].glyph);
                x += ( glyph->xSkip * useScale ) + adjust;
                s++;
                count++;
            }
        }
        trap_R_SetColor ( NULL );
    }
}


#endif

/*
==============
CG_DrawField

Draws large numbers for status bar and powerups
TODO: Different Fonts
==============
*/
#ifndef MISSIONPACK
/*static void CG_DrawField ( int x, int y, int width, int value, float size ) {
    char	num[8], *ptr;
    int		l;
    int		frame;

    if ( width < 1 ) {
        return;
    }

    // draw number string
    if ( width > 5 ) {
        width = 5;
    }

    switch ( width ) {
    case 1:
        value = value > 9 ? 9 : value;
        value = value < 0 ? 0 : value;
        break;
    case 2:
        value = value > 99 ? 99 : value;
        value = value < -9 ? -9 : value;
        break;
    case 3:
        value = value > 999 ? 999 : value;
        value = value < -99 ? -99 : value;
        break;
    case 4:
        value = value > 9999 ? 9999 : value;
        value = value < -999 ? -999 : value;
        break;
    }

    Com_sprintf ( num, sizeof ( num ), "%i", value );
    l = strlen ( num );
    if ( l > width )
        l = width;
    x += 2 + CHAR_WIDTH*size* ( width - l );

    ptr = num;
    while ( *ptr && l ) {
        if ( *ptr == '-' )
            frame = STAT_MINUS;
        else
            frame = *ptr -'0';

        CG_DrawPic ( x,y, CHAR_WIDTH*size, CHAR_HEIGHT*size, cgs.media.numberShaders[frame] );
        x += CHAR_WIDTH*size;
        ptr++;
        l--;
    }
}*/
#endif // MISSIONPACK

static void CG_DrawFieldFontsize ( int x, int y, int width, int value, int fontWidth, int fontHeight ) {
    char	num[8], *ptr;
    int		l;
    int		frame;

    if ( width < 1 ) {
        return;
    }

    // draw number string
    if ( width > 5 ) {
        width = 5;
    }

    switch ( width ) {
    case 1:
        value = value > 9 ? 9 : value;
        value = value < 0 ? 0 : value;
        break;
    case 2:
        value = value > 99 ? 99 : value;
        value = value < -9 ? -9 : value;
        break;
    case 3:
        value = value > 999 ? 999 : value;
        value = value < -99 ? -99 : value;
        break;
    case 4:
        value = value > 9999 ? 9999 : value;
        value = value < -999 ? -999 : value;
        break;
    }

    Com_sprintf ( num, sizeof ( num ), "%i", value );
    l = strlen ( num );
    if ( l > width )
        l = width;
    x +=  fontWidth* ( width - l );

    ptr = num;
    while ( *ptr && l ) {
        if ( *ptr == '-' )
            frame = STAT_MINUS;
        else
            frame = *ptr -'0';

        CG_DrawPic ( x,y, fontWidth, fontHeight, cgs.media.numberShaders[frame] );
        x += fontWidth;
        ptr++;
        l--;
    }
}
/*
==============
CG_DrawFieldHud

Draws large numbers for status bar and powerups, position defined in hud
TODO: Colorization
==============
*/
static void CG_DrawFieldHud ( int value, int hudnumber ) {
    hudElements_t hudelement = cgs.hud[hudnumber];

    if ( !hudelement.inuse )
        return;

    if ( hudelement.color[0] != 1 || hudelement.color[1] != 1 || hudelement.color[2] != 1 )
        trap_R_SetColor ( hudelement.color );

    if ( hudelement.textAlign == 0 ) {
        if ( value >= 100 )
            CG_DrawFieldFontsize ( hudelement.xpos, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
        else if ( value >= 10 )
            CG_DrawFieldFontsize ( hudelement.xpos-hudelement.fontWidth, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
        else
            CG_DrawFieldFontsize ( hudelement.xpos-2*hudelement.fontWidth, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
    } else if ( hudelement.textAlign == 2 ) {
        CG_DrawFieldFontsize ( hudelement.xpos+hudelement.width - 3*hudelement.fontWidth, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
    } else {
        if ( value >= 100 )
            CG_DrawFieldFontsize ( hudelement.xpos+hudelement.width/2 - 3*hudelement.fontWidth/2, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
        else if ( value >= 10 )
            CG_DrawFieldFontsize ( hudelement.xpos+hudelement.width/2 - 4*hudelement.fontWidth/2, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
        else
            CG_DrawFieldFontsize ( hudelement.xpos+hudelement.width/2 - 5*hudelement.fontWidth/2, hudelement.ypos, 3, value, hudelement.fontWidth, hudelement.fontHeight );
    }
}

/*
==============
CG_DrawBarHud

Draw bars for ammo, health and armor
==============
*/

static void CG_DrawBarHud ( int hudnumber, int value, int maxvalue ) {
    hudElements_t hudelement = cgs.hud[hudnumber];
    qboolean horizontal = hudelement.width > hudelement.height;
    float x, y, width, height;
    float xx, yy, ww, hh;
    float scale = ( float ) value/ ( float ) maxvalue;

    if ( !hudelement.inuse )
        return;

    if ( hudelement.color[0] != 1 || hudelement.color[1] != 1 || hudelement.color[2] != 1 )
        trap_R_SetColor ( hudelement.color );

    if ( scale > 1.0f )
        scale = 1.0f;

    if ( horizontal ) {
        height = hudelement.height;
        width = hudelement.width * scale;
        if ( hudelement.textAlign == 0 ) {
            x = hudelement.xpos;
            y = hudelement.ypos;

        } else if ( hudelement.textAlign == 2 ) {
            x = hudelement.xpos + hudelement.width - width;
            y = hudelement.ypos;

        } else {
            x = hudelement.xpos + hudelement.width/2 - width/2;
            y = hudelement.ypos;

        }
    } else {
        height = hudelement.height * scale;
        width = hudelement.width;
        x = hudelement.xpos;
        y = hudelement.ypos + hudelement.height - height;
    }

    if ( hudelement.imageHandle ) {

        if ( horizontal ) {
            if ( hudelement.textAlign == 0 ) {
                xx = hudelement.xpos;
                yy = hudelement.ypos;
                hh = height;
                ww = width;
                CG_AdjustFrom640 ( &xx, &yy, &ww, &hh );
                trap_R_DrawStretchPic ( xx, yy, ww, hh, 0, 0, ( float ) width/ ( float ) hudelement.width, 1, hudelement.imageHandle );
            } else if ( hudelement.textAlign == 2 ) {
                xx = hudelement.xpos + hudelement.width - width;
                yy = hudelement.ypos;
                hh = height;
                ww = width;
                CG_AdjustFrom640 ( &xx, &yy, &ww, &hh );
                trap_R_DrawStretchPic ( xx, yy, ww, hh, 1 - ( float ) width/ ( float ) hudelement.width, 0, 1, 1, hudelement.imageHandle );
            } else {
                xx = hudelement.xpos + hudelement.width/2 - width/2;
                yy = hudelement.ypos;
                hh = height;
                ww = width;
                CG_AdjustFrom640 ( &xx, &yy, &ww, &hh );
                trap_R_DrawStretchPic ( xx, yy, ww, hh, 1.0f/2.0f - ( float ) width/ ( float ) ( 2*hudelement.width ), 0, 1.0f/2.0f + ( float ) width/ ( float ) ( 2*hudelement.width ), 1, hudelement.imageHandle );
            }
        } else {
            xx = hudelement.xpos;
            yy = hudelement.ypos+ hudelement.height - height;
            hh = height;
            ww = width;
            CG_AdjustFrom640 ( &xx, &yy, &ww, &hh );
            trap_R_DrawStretchPic ( xx, yy, ww, hh, 0, 1 - ( float ) height/ ( float ) hudelement.height, 1, 1, hudelement.imageHandle );
        }
    } else {
        CG_DrawPic ( x, y, width, height, cgs.media.whiteShader );
    }

    trap_R_SetColor ( NULL ); //FIXME: really?
}


/*
================
CG_Draw3DModel

================
*/
void CG_Draw3DModel ( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles ) {
    refdef_t		refdef;
    refEntity_t		ent;

    if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer ) {
        return;
    }

    CG_AdjustFrom640 ( &x, &y, &w, &h );

    memset ( &refdef, 0, sizeof ( refdef ) );

    memset ( &ent, 0, sizeof ( ent ) );
    AnglesToAxis ( angles, ent.axis );
    VectorCopy ( origin, ent.origin );
    ent.hModel = model;
    ent.customSkin = skin;
    ent.renderfx = RF_NOSHADOW;		// no stencil shadows

    refdef.rdflags = RDF_NOWORLDMODEL;

    AxisClear ( refdef.viewaxis );

    refdef.fov_x = 30;
    refdef.fov_y = 30;

    refdef.x = x;
    refdef.y = y;
    refdef.width = w;
    refdef.height = h;

    refdef.time = cg.time;

    trap_R_ClearScene();
    trap_R_AddRefEntityToScene ( &ent );
    trap_R_RenderScene ( &refdef );
}

/*
================
CG_DrawHead

Used for both the status bar and the scoreboard
================
*/
void CG_DrawHead ( float x, float y, float w, float h, int clientNum, vec3_t headAngles ) {
    clientInfo_t	*ci;

    ci = &cgs.clientinfo[ clientNum ];

    if ( cg_drawIcons.integer ) {
        CG_DrawPic ( x, y, w, h, ci->modelIcon );
    }

    // if they are deferred, draw a cross out
    if ( ci->deferred ) {
        CG_DrawPic ( x, y, w, h, cgs.media.deferShader );
    }
}

/*
================
CG_DrawFlagModel

Used for both the status bar and the scoreboard
================
*/
void CG_DrawFlagModel ( float x, float y, float w, float h, int team, qboolean force2D ) {
    qhandle_t		cm;
    float			len;
    vec3_t			origin, angles;
    vec3_t			mins, maxs;
    qhandle_t		handle;

    if ( !force2D && cg_draw3dIcons.integer ) {

        VectorClear ( angles );

        cm = cgs.media.redFlagModel;

        // offset the origin y and z to center the flag
        trap_R_ModelBounds ( cm, mins, maxs );

        origin[2] = -0.5 * ( mins[2] + maxs[2] );
        origin[1] = 0.5 * ( mins[1] + maxs[1] );

        // calculate distance so the flag nearly fills the box
        // assume heads are taller than wide
        len = 0.5 * ( maxs[2] - mins[2] );
        origin[0] = len / 0.268;	// len / tan( fov/2 )

        angles[YAW] = 60 * sin ( cg.time / 2000.0 );;

        if ( team == TEAM_RED ) {
            handle = cgs.media.redFlagModel;
            if ( cgs.gametype == GT_DOUBLE_D ) {
                if ( cgs.redflag == TEAM_BLUE )
                    handle = cgs.media.blueFlagModel;
                if ( cgs.redflag == TEAM_FREE )
                    handle = cgs.media.neutralFlagModel;
                if ( cgs.redflag == TEAM_NONE )
                    handle = cgs.media.neutralFlagModel;
            }
        } else if ( team == TEAM_BLUE ) {
            handle = cgs.media.blueFlagModel;
            if ( cgs.gametype == GT_DOUBLE_D ) {
                if ( cgs.redflag == TEAM_BLUE )
                    handle = cgs.media.blueFlagModel;
                if ( cgs.redflag == TEAM_FREE )
                    handle = cgs.media.neutralFlagModel;
                if ( cgs.redflag == TEAM_NONE )
                    handle = cgs.media.neutralFlagModel;
            }
        } else if ( team == TEAM_FREE ) {
            handle = cgs.media.neutralFlagModel;
        } else {
            return;
        }
        CG_Draw3DModel ( x, y, w, h, handle, 0, origin, angles );
    } else if ( cg_drawIcons.integer ) {
        gitem_t *item;

        if ( team == TEAM_RED ) {
            item = BG_FindItemForPowerup ( PW_REDFLAG );
        } else if ( team == TEAM_BLUE ) {
            item = BG_FindItemForPowerup ( PW_BLUEFLAG );
        } else if ( team == TEAM_FREE ) {
            item = BG_FindItemForPowerup ( PW_NEUTRALFLAG );
        } else {
            return;
        }
        if ( item ) {
            CG_DrawPic ( x, y, w, h, cg_items[ ITEM_INDEX ( item ) ].icon );
        }
    }
}

/*
================
CG_DrawStatusBarFlag

================
*/
#ifndef MISSIONPACK
/*static void CG_DrawStatusBarFlag ( float x, int team ) {
    CG_DrawFlagModel ( x, 480 - ICON_SIZE, ICON_SIZE, ICON_SIZE, team, qfalse );
}*/
#endif // MISSIONPACK

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground ( int x, int y, int w, int h, float alpha, int team ) {
    vec4_t		hcolor;

    hcolor[3] = alpha;
    if ( team == TEAM_RED ) {
        hcolor[0] = 1;
        hcolor[1] = 0;
        hcolor[2] = 0;
    } else if ( team == TEAM_BLUE ) {
        hcolor[0] = 0;
        hcolor[1] = 0;
        hcolor[2] = 1;
    } else {
        return;
    }
    trap_R_SetColor ( hcolor );
    CG_DrawPic ( x, y, w, h, cgs.media.teamStatusBar );
    trap_R_SetColor ( NULL );
}

/*
================
CG_DrawStatusBar

================
*/
static void CG_DrawStatusBar ( void ) {
    int			color;
    centity_t	*cent;
    playerState_t	*ps;
    int			value;
    vec4_t		hcolor;
    vec3_t		angles;
    hudElements_t 	hudelement;
    int ammoPack=200;

    static float colors[4][4] = {
//		{ 0.2, 1.0, 0.2, 1.0 } , { 1.0, 0.2, 0.2, 1.0 }, {0.5, 0.5, 0.5, 1} };
        { 1.0f, 0.69f, 0.0f, 1.0f },    // normal
        { 1.0f, 0.2f, 0.2f, 1.0f },     // low health
        { 0.5f, 0.5f, 0.5f, 1.0f },     // weapon firing
        { 1.0f, 1.0f, 1.0f, 1.0f }
    };   // health > 100

    cent = &cg_entities[cg.snap->ps.clientNum];
    ps = &cg.snap->ps;

    VectorClear ( angles );

    //
    // ammo
    //
    if ( cent->currentState.weapon ) {
        hudelement = cgs.hud[HUD_AMMOCOUNT];


        value = ps->ammo[cent->currentState.weapon];


        if ( value > -1 && ( hudelement.inuse || cgs.hud[HUD_AMMOBAR].inuse ) ) {

            if ( cg.predictedPlayerState.weaponstate == WEAPON_FIRING
                    && cg.predictedPlayerState.weaponTime > 100 ) {
                // draw as dark grey when reloading
                //color = 2;	// dark grey
                VectorCopy ( colors[2], hcolor );
            } else {
                if ( value >= 0 ) {
                    //color = 0;	// green
                    VectorCopy ( colors[0], hcolor );
                } else {
                    //color = 1;	// red
                    VectorCopy ( colors[1], hcolor );
                }
            }

            //TODO: inline function? It is used in Weaponbar too, but the function will be long

            if ( cgs.hud[HUD_AMMOBAR].inuse )
                switch ( cent->currentState.weapon ) {
                case WP_MACHINEGUN:
                    ammoPack = 100;
                    break;
                case WP_SHOTGUN:
                    ammoPack = 10;
                    break;
                case WP_GRENADE_LAUNCHER:
                    ammoPack = 10;
                    break;
                case WP_ROCKET_LAUNCHER:
                    ammoPack = 10;
                    break;
                case WP_LIGHTNING:
                    ammoPack = 100;
                    break;
                case WP_RAILGUN:
                    ammoPack = 10;
                    break;
                case WP_PLASMAGUN:
                    ammoPack = 60;
                    break;
                case WP_BFG:
                    ammoPack = 15;
                    break;
                /*default:
                    ammoPack = 200;*/
                }
            hcolor[3] = cgs.hud[HUD_AMMOCOUNT].color[3];
            trap_R_SetColor ( hcolor );

            CG_DrawFieldHud ( value, HUD_AMMOCOUNT );

            hcolor[3] = cgs.hud[HUD_AMMOBAR].color[3];
            trap_R_SetColor ( hcolor );

            CG_DrawBarHud ( HUD_AMMOBAR, value, ammoPack );
            trap_R_SetColor ( NULL );

        }
        if ( cent->currentState.weapon && cg_weapons[ /*cg.predictedPlayerState.weapon*/ cent->currentState.weapon ].ammoIcon ) { /* for multiview */
            CG_DrawHudIcon ( HUD_AMMOICON, qfalse, cg_weapons[ /*cg.predictedPlayerState.weapon*/ cent->currentState.weapon ].ammoIcon );
        }
    }
    /* TODO: color the flagshader used in the flagstatus, maybe this is faster and it will save memory */
    if ( cg.predictedPlayerState.powerups[PW_REDFLAG] ) {
        CG_DrawHudIcon ( HUD_STATUSBARFLAG, qfalse, cgs.media.redFlagShader[0] );
    } else if ( cg.predictedPlayerState.powerups[PW_BLUEFLAG] ) {
        CG_DrawHudIcon ( HUD_STATUSBARFLAG, qfalse, cgs.media.blueFlagShader[0] );
    } else if ( cg.predictedPlayerState.powerups[PW_NEUTRALFLAG] ) {
        CG_DrawHudIcon ( HUD_STATUSBARFLAG, qfalse, cgs.media.neutralFlagShader[0] );
    } else if ( cgs.gametype == GT_CTF_ELIMINATION ) {
        if ( cgs.csStatus == 1 )
            CG_DrawHudIcon ( HUD_STATUSBARFLAG, qfalse, cgs.media.medalDefend );
        else if ( cgs.csStatus == 2 )
            CG_DrawHudIcon ( HUD_STATUSBARFLAG, qfalse, cgs.media.medalCapture );
    }

    //
    // health
    //
    //TODO: we could try the same as above, but we will only save 2*ICONSIZE memory
    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
        CG_DrawHudIcon ( HUD_HEALTHICON, qtrue,  cgs.media.healthBlue );
    else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED )
        CG_DrawHudIcon ( HUD_HEALTHICON, qtrue,  cgs.media.healthRed );
    else
        CG_DrawHudIcon ( HUD_HEALTHICON, qtrue,  cgs.media.healthYellow );


    value = ps->stats[STAT_HEALTH];


    if ( value > 100 ) {
        VectorCopy ( colors[3], hcolor );
    } else if ( value > cg_lowHealthPercentile.value * 100 ) {
        VectorCopy ( colors[0], hcolor );
    } else if ( value > 0 ) {
        color = ( cg.time >> 8 ) & 1;	// flash
        VectorCopy ( colors[color], hcolor );
    } else {
        VectorCopy ( colors[1], hcolor );
    }

    hcolor[3] = cgs.hud[HUD_HEALTHCOUNT].color[3];
    trap_R_SetColor ( hcolor );
    CG_DrawFieldHud ( value, HUD_HEALTHCOUNT );  //FIXME: NULL color at the end of DrawFieldHud

    hcolor[3] = cgs.hud[HUD_HEALTHBAR].color[3];
    trap_R_SetColor ( hcolor );

    CG_DrawBarHud ( HUD_HEALTHBAR, value, 100 );

    //
    // armor
    //

    value = ps->stats[STAT_ARMOR];


    if ( value > 100 )
        VectorCopy ( colors[3], hcolor );
    else
        VectorCopy ( colors[0], hcolor );

    hcolor[3] = cgs.hud[HUD_ARMORCOUNT].color[3];
    trap_R_SetColor ( hcolor );

    CG_DrawFieldHud ( value, HUD_ARMORCOUNT );

    hcolor[3] = cgs.hud[HUD_ARMORBAR].color[3];
    trap_R_SetColor ( hcolor );

    CG_DrawBarHud ( HUD_ARMORBAR, ps->stats[STAT_ARMOR], 100 );
    trap_R_SetColor ( NULL );

    // if we didn't draw a 3D icon, draw a 2D icon for armor
    //TODO: same as above, or just keep one icon(yellow)?
    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
        CG_DrawHudIcon ( HUD_ARMORICON, qtrue, cgs.media.armorBlue );
    else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED )
        CG_DrawHudIcon ( HUD_ARMORICON, qtrue, cgs.media.armorRed );
    else
        CG_DrawHudIcon ( HUD_ARMORICON, qtrue, cgs.media.armorYellow );
}

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

/*
================
CG_DrawAttacker

================
*/
static void CG_DrawAttacker ( void ) {
    int			t;
    const char	*info;
    char	*name;
    int			clientNum;
    hudElements_t	attacker = cgs.hud[HUD_ATTACKERICON];


    if ( !attacker.inuse )
        return;

    if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 ) {
        return;
    }

    if ( !cg.attackerTime ) {
        return;
    }

    clientNum = cg.predictedPlayerState.persistant[PERS_ATTACKER];
    if ( clientNum < 0 || clientNum >= MAX_CLIENTS || clientNum == cg.snap->ps.clientNum ) {
        return;
    }

    t = cg.time - cg.attackerTime;
    if ( t > attacker.time ) {
        return;
    }

    CG_DrawHudIcon ( HUD_ATTACKERICON, qfalse, cgs.clientinfo[ clientNum ].modelIcon );

    attacker = cgs.hud[HUD_ATTACKERNAME];

    if ( t > attacker.time ) {
        return;
    }
    if ( attacker.inuse ) {
        info = CG_ConfigString ( CS_PLAYERS + clientNum );
        name = Info_ValueForKey ( info, "n" );

        CG_DrawStringHud ( HUD_ATTACKERNAME, qtrue, name );

    }
    return;
}

/*
================
CG_DrawSpeedMeter

================
*/
static void CG_DrawSpeedMeter ( void ) {
    //const char        *s;
    /*vec_t       *vel;
    int         speed;
    */
    /* speed meter can get in the way of the scoreboard */
    if ( cg.scoreBoardShowing ) {
        return;
    }

    //vel = cg.snap->ps.velocity;
    /* ignore vertical component of velocity */
    //TODO: test to calculate it once when receiving a snapshot and use this value.
    //This will reduce calculation ~ 1/3 with snaps 40 and fps 125
    //speed = sqrt ( vel[0] * vel[0] + vel[1] * vel[1] );

    //s = va ( "%iu/s", speed );

    CG_DrawStringHud ( HUD_SPEED, qtrue, va ( "%iu/s", cg.speed ) );

    return;
}
/*
================
CG_DrawAccelMeter
TODO: Speedup
================
*/
static void CG_DrawAccelMeter ( void ) {
    vec_t       *accel;
    vec_t	    *lastaccel;
    vec3_t	    acc;
    vec_t	    *vel;
    int         accelValue;
    int	    vel_norm;
    int 	    width;
    vec4_t	    color;
    hudElements_t hudelement = cgs.hud[HUD_ACCEL];

    if ( cg.scoreBoardShowing ) {
        return;
    }
    if ( !hudelement.inuse )
        return;

    accel = cg.accel;
    lastaccel = cg.lastaccel;

    acc[0] = ( ( cg.time-cg.AccelTime ) * accel[0] + ( ( cg.AccelTime - cg.lastAccelTime ) - cg.time + cg.AccelTime ) * lastaccel[0] ) / ( cg.AccelTime - cg.lastAccelTime );
    acc[1] = ( ( cg.time-cg.AccelTime ) * accel[1] + ( ( cg.AccelTime - cg.lastAccelTime ) - cg.time + cg.AccelTime ) * lastaccel[1] ) / ( cg.AccelTime - cg.lastAccelTime );

    vel = cg.snap->ps.velocity;
    vel_norm = sqrt ( vel[0]*vel[0] + vel[1] * vel[1] );

    if ( vel_norm >= 320 )
        accelValue = ( acc[0]*vel[0] + acc[1]*vel[1] ) /vel_norm;
    else
        accelValue = 0;

    if ( accelValue > 150 )
        accelValue = 150;
    else if ( accelValue < -150 )
        accelValue = -150;

    width = ( hudelement.width/2 ) * accelValue / 150;

    if ( accelValue > 0 ) {

        color[0] = 0.0f;
        color[1] = 1.0f;
        color[2] = 0.0f;
        color[3] = 0.33f;

        CG_FillRect ( hudelement.xpos + hudelement.width/2, hudelement.ypos, width, hudelement.height, color );
    } else if ( accelValue <= 0 ) {
        width *= -1;

        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
        color[3] = 0.33f;

        CG_FillRect ( hudelement.xpos + hudelement.width/2 - width, hudelement.ypos, width, hudelement.height, color );
    }
    return;

}

/*
==================
CG_DrawSnapshot
==================
*/
/*static float CG_DrawSnapshot ( float y ) {
    char		*s;
    int			w;

    s = va ( "time:%i snap:%i cmd:%i", cg.snap->serverTime,
             cg.latestSnapshotNum, cgs.serverCommandSequence );
    w = CG_DrawStrlen ( s ) * BIGCHAR_WIDTH;

    CG_DrawBigString ( 635 - w, y + 2, s, 1.0F );

    return y + BIGCHAR_HEIGHT + 4;
}*/

/*
==================
CG_DrawFPS
==================
*/
#define	FPS_FRAMES	4
static void CG_DrawFPS ( void ) {
    const char		*s;
    static int	previousTimes[FPS_FRAMES];
    static int	index;
    int		i, total;
    int		fps;
    static	int	previous;
    int		t, frameTime;

    if ( !cgs.hud[HUD_FPS].inuse )
        return;

    // don't use serverTime, because that will be drifting to
    // correct for internet lag changes, timescales, timedemos, etc
    t = trap_Milliseconds();
    frameTime = t - previous;
    previous = t;

    previousTimes[index % FPS_FRAMES] = frameTime;
    index++;
    if ( index > FPS_FRAMES ) {
        // average multiple frames together to smooth changes out a bit
        total = 0;
        for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
            total += previousTimes[i];
        }
        if ( !total ) {
            total = 1;
        }
        fps = 1000 * FPS_FRAMES / total;
        s = va ( "%ifps", fps );

        CG_DrawStringHud ( HUD_FPS, qtrue, s );

    }

    return;
}

/*
=================
CG_DrawTimer
=================
*/
void CG_DrawTimer ( void ) {
    const char		*s;
    int			mins, seconds, tens;
    int			msec;

    if ( !cgs.hud[HUD_GAMETIME].inuse )
        return;

    if ( !cgs.timeout )
        msec = cg.time - cgs.timeoutDelay - cgs.levelStartTime;
    else
        msec = cgs.timeoutTime - cgs.timeoutDelay + cgs.timeoutAdd - cgs.levelStartTime;

    seconds = msec / 1000;
    if ( cg_inverseTimer.integer && cgs.timelimit ) {
        seconds = cgs.timelimit * 60 - seconds;
    }
    mins = seconds / 60;
    seconds -= mins * 60;
    tens = seconds / 10;
    seconds -= tens * 10;

    //CG_DrawBigString( 320 - w/2, 10, s, 1.0F);

    s = va ( "%i:%i%i", mins, tens, seconds );  //FIXME: why not use %02i, is it slower?
    CG_DrawStringHud ( HUD_GAMETIME, qtrue, s );

    return;
}

/*
=================
CG_DrawLMSmode
TODO: add to hud, but is it used in the game?
can we remove the lms gametype?
=================
*/

/*static float CG_DrawLMSmode ( float y ) {
    char		*s;
    int		w;

    if ( cgs.lms_mode == 0 ) {
        s = va ( "LMS: Point/round + OT" );
    } else
        if ( cgs.lms_mode == 1 ) {
            s = va ( "LMS: Point/round - OT" );
        } else
            if ( cgs.lms_mode == 2 ) {
                s = va ( "LMS: Point/kill + OT" );
            } else
                if ( cgs.lms_mode == 3 ) {
                    s = va ( "LMS: Point/kill - OT" );
                } else
                    s = va ( "LMS: Unknown mode" );

    w = CG_DrawStrlen ( s ) * SMALLCHAR_WIDTH;
    CG_DrawSmallString ( 635 - w, y + 2, s, 1.0F );

    return y + SMALLCHAR_HEIGHT+4;
}*/

/*
=================
CG_DrawCTFoneway
TODO: is it used?
=================
*/

/*static float CG_DrawCTFoneway ( float y ) {
    char		*s;
    int		w;
    vec4_t		color;

    if ( cgs.gametype != GT_CTF_ELIMINATION )
        return y;

    memcpy ( color,g_color_table[ColorIndex ( COLOR_WHITE ) ],sizeof ( color ) );

    if ( ( cgs.elimflags&EF_ONEWAY ) ==0 ) {
        return y; //nothing to draw
    } else
        if ( cgs.attackingTeam == TEAM_BLUE ) {
            memcpy ( color,g_color_table[ColorIndex ( COLOR_BLUE ) ],sizeof ( color ) );
            s = va ( "Blue team on offence" );
        } else
            if ( cgs.attackingTeam == TEAM_RED ) {
                memcpy ( color,g_color_table[ColorIndex ( COLOR_RED ) ],sizeof ( color ) );
                s = va ( "Red team on offence" );
            } else
                s = va ( "Unknown team on offence" );

    w = CG_DrawStrlen ( s ) * SMALLCHAR_WIDTH;
    CG_DrawSmallStringColor ( 635 - w, y + 2, s, color );

    return y + SMALLCHAR_HEIGHT+4;
}

*/

/*
=================
CG_DrawDomStatus
=================
*/

/*static float CG_DrawDomStatus ( float y ) {
    int 		i,w;
    char		*s;
    vec4_t		color;

    for ( i = 0;i < cgs.domination_points_count;i++ ) {
        switch ( cgs.domination_points_status[i] ) {
        case TEAM_RED:
            memcpy ( color,g_color_table[ColorIndex ( COLOR_RED ) ],sizeof ( color ) );
            break;
        case TEAM_BLUE:
            memcpy ( color,g_color_table[ColorIndex ( COLOR_BLUE ) ],sizeof ( color ) );
            break;
        default:
            memcpy ( color,g_color_table[ColorIndex ( COLOR_WHITE ) ],sizeof ( color ) );
            break;
        }

        s = va ( "%s",cgs.domination_points_names[i] );
        w = CG_DrawStrlen ( s ) * SMALLCHAR_WIDTH;
        CG_DrawSmallStringColor ( 635 - w, y + 2, s, color );
        y += SMALLCHAR_HEIGHT+4;

    }

    return y;
}
*/

/*
=================
CG_DrawEliminationTimer
=================
*/
static void CG_DrawEliminationTimer ( void ) {
    const char		*s;
    int			mins, seconds, tens, sec;
    int			msec;
    vec4_t			color;
    char	*st;
    int rst;

    rst = cgs.roundStartTime;

    //default color is white
    memcpy ( color,g_color_table[ColorIndex ( COLOR_WHITE ) ],sizeof ( color ) );

    //msec = cg.time - cgs.levelStartTime;
    if ( cg.time>rst ) { //We are started
        msec = cgs.roundtime*1000 - ( cg.time -rst );
        if ( msec<=30*1000-1 ) //<= 30 seconds
            memcpy ( color,g_color_table[ColorIndex ( COLOR_YELLOW ) ],sizeof ( color ) );
        if ( msec<=10*1000-1 ) //<= 10 seconds
            memcpy ( color,g_color_table[ColorIndex ( COLOR_RED ) ],sizeof ( color ) );
        msec += 1000; //120-1 instead of 119-0
    } else {
        //Warmup
        msec = -cg.time +rst;
        memcpy ( color,g_color_table[ColorIndex ( COLOR_GREEN ) ],sizeof ( color ) );
        sec = msec/1000;
        msec += 1000; //5-1 instead of 4-0
        /***
        Lots of stuff
        ****/
        if ( cg.warmup == 0 ) {
            st = va ( "Round in: %i", sec + 1 );
            if ( sec != cg.warmupCount ) {
                cg.warmupCount = sec;
                switch ( sec ) {
                case 0:
                    trap_S_StartLocalSound ( cgs.media.count1Sound, CHAN_ANNOUNCER );
                    break;
                case 1:
                    trap_S_StartLocalSound ( cgs.media.count2Sound, CHAN_ANNOUNCER );
                    break;
                case 2:
                    trap_S_StartLocalSound ( cgs.media.count3Sound, CHAN_ANNOUNCER );
                    break;
                default:
                    break;
                }
            }

            CG_DrawStringHud ( HUD_COUNTDOWN, qtrue, st );
        }
        /*
        Lots of stuff
        */
    }

    seconds = msec / 1000;
    mins = seconds / 60;
    seconds -= mins * 60;
    tens = seconds / 10;
    seconds -= tens * 10;

    if ( msec>=0 )
        s = va ( "%i:%i%i", mins, tens, seconds ); //TODO: %02i, removes  one division
    else
        s = va ( "^1Overtime" );

    CG_DrawStringHud ( HUD_CATIME, qtrue, s );

    return;
}

/*
=================
CG_DrawEliminationTimer
=================
*/
static void CG_DrawTimeout ( void ) {
    int 	msec;
    int	sec;
    const char	*st;

    if ( !cgs.timeout )
        return;

    msec = cgs.timeoutTime + cgs.timeoutAdd - cg.time;
    sec = msec/1000;

    st = va ( "Timeout ends in: %i", sec + 1 );

    if ( sec != cgs.timeoutCount ) {
        cgs.timeoutCount = sec;
        switch ( sec ) {
        case 0:
            trap_S_StartLocalSound ( cgs.media.count1Sound, CHAN_ANNOUNCER );
            break;
        case 1:
            trap_S_StartLocalSound ( cgs.media.count2Sound, CHAN_ANNOUNCER );
            break;
        case 2:
            trap_S_StartLocalSound ( cgs.media.count3Sound, CHAN_ANNOUNCER );
            break;
        default:
            break;
        }
    }
    CG_DrawStringHud ( HUD_COUNTDOWN, qtrue, st );
}

/*
=================
CG_DrawTeamOverlay
=================
*/

static void CG_DrawTeamOverlay ( qboolean right, qboolean upper ) {
    int x, y;//, w, h, xx;
    int i, j, len;
    const char *p;
    vec4_t		color;
    int pwidth, lwidth;
    int plyrs;
    char st[16];
    clientInfo_t *ci;
    gitem_t	*item;
    int count;
    hudElements_t hudelement;
    int 	hudoffset = 0;

    /*if ( !cg_drawTeamOverlay.integer ) {
    	return y;
    }*/

    if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE ) {
        return; // Not on any team
    }

    plyrs = 0;

    // max player name width
    pwidth = 0;
    count = ( numSortedTeamPlayers > 8 ) ? 8 : numSortedTeamPlayers;
    for ( i = 0; i < count; i++ ) {
        ci = cgs.clientinfo + sortedTeamPlayers[i];
        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM] ) {
            plyrs++;
            len = CG_DrawStrlen ( ci->name );
            if ( len > pwidth )
                pwidth = len;
        }
    }

    if ( !plyrs )
        return;

    if ( pwidth > TEAM_OVERLAY_MAXNAME_WIDTH )
        pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;

    // max location name width
    lwidth = 0;
    for ( i = 1; i < MAX_LOCATIONS; i++ ) {
        p = CG_ConfigString ( CS_LOCATIONS + i );
        if ( p && *p ) {
            len = CG_DrawStrlen ( p );
            if ( len > lwidth )
                lwidth = len;
        }
    }

    if ( lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH )
        lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;

    for ( i = 0; i < count; i++ ) {
        hudelement = cgs.hud[HUD_TEAMOVERLAY1+i-hudoffset];

        if ( !hudelement.inuse )
            continue;

        ci = cgs.clientinfo + sortedTeamPlayers[i];

        if ( !cg_selfOnTeamOverlay.integer && sortedTeamPlayers[i] == 0 ) {
            hudoffset = 1;
            continue;
        }

        if ( cgs.gametype >= GT_TEAM && hudelement.teamBgColor == 1 ) {
            if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
                color[0] = 1;
                color[1] = 0;
                color[2] = 0;
            } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
                color[0] = 0;
                color[1] = 0;
                color[2] = 1;
            }
        } else if ( cgs.gametype >= GT_TEAM && hudelement.teamBgColor == 2 ) {
            if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
                color[0] = 1;
                color[1] = 0;
                color[2] = 0;
            } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
                color[0] = 0;
                color[1] = 0;
                color[2] = 1;
            }
        } else {
            color[0] = hudelement.bgcolor[0];
            color[1] = hudelement.bgcolor[1];
            color[2] = hudelement.bgcolor[2];
        }
        color[3] = hudelement.bgcolor[3];

        CG_FillRect ( hudelement.xpos, hudelement.ypos, hudelement.width, hudelement.height, color );

        if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM] ) {

            color[0] = color[1] = color[2] = color[3] = 1.0;

            x = hudelement.xpos;
            y = hudelement.ypos + hudelement.height/2 - hudelement.fontHeight/2;

            for ( j = 0; j <= PW_NUM_POWERUPS; j++ ) {
                if ( ci->powerups & ( 1 << j ) ) {

                    item = BG_FindItemForPowerup ( j );

                    if ( item ) {
                        CG_DrawPic ( x, y, hudelement.fontWidth, hudelement.fontWidth,
                                     trap_R_RegisterShader ( item->icon ) );
                        x += hudelement.fontWidth;
                    }
                }
            }

            CG_DrawStringExt ( x, y,
                               ci->name, color, qfalse, qfalse,
                               hudelement.fontWidth, hudelement.fontHeight, TEAM_OVERLAY_MAXNAME_WIDTH );

            x = hudelement.xpos + hudelement.width/2 - hudelement.fontWidth* lwidth/2 + 2*hudelement.fontWidth;

            if ( lwidth ) {
                p = CG_ConfigString ( CS_LOCATIONS + ci->location );
                if ( !p || !*p )
                    p = "unknown";
                len = CG_DrawStrlen ( p );
                if ( len > lwidth )
                    len = lwidth;


                CG_DrawStringExt ( x, y,
                                   p, color, qfalse, qfalse, hudelement.fontWidth, hudelement.fontHeight,
                                   TEAM_OVERLAY_MAXLOCATION_WIDTH );
            }

            x = hudelement.xpos + hudelement.width - 7 * hudelement.fontWidth;

            CG_GetColorForHealth ( ci->health, ci->armor, color );

            Com_sprintf ( st, sizeof ( st ), "%3i %3i", ci->health, ci->armor );

            CG_DrawStringExt ( x, y,
                               st, color, qfalse, qfalse,
                               hudelement.fontWidth, hudelement.fontHeight, 0 );

            // draw weapon icon
            x += hudelement.fontWidth * 3;

            if ( cg_weapons[ci->curWeapon].weaponIcon ) {
                CG_DrawPic ( x, y, hudelement.fontWidth, hudelement.fontWidth,
                             cg_weapons[ci->curWeapon].weaponIcon );
            } else {
                CG_DrawPic ( x, y, hudelement.fontWidth, hudelement.fontWidth,
                             cgs.media.deferShader );
            }


            /*xx = x + TINYCHAR_WIDTH;

            CG_DrawStringExt( xx, y,
            	ci->name, hcolor, qfalse, qfalse,
            	TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH);

            if (lwidth) {
            	p = CG_ConfigString(CS_LOCATIONS + ci->location);
            	if (!p || !*p)
            		p = "unknown";
            //	len = CG_DrawStrlen(p);
            //	if (len > lwidth)
            //		len = lwidth;

            // 			xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth +
            // 				((lwidth/2 - len/2) * TINYCHAR_WIDTH);
            	xx = x + TINYCHAR_WIDTH * 2 + TINYCHAR_WIDTH * pwidth;
            	CG_DrawStringExt( xx, y,
            		p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
            		TEAM_OVERLAY_MAXLOCATION_WIDTH);
            }

            CG_GetColorForHealth( ci->health, ci->armor, hcolor );

            Com_sprintf (st, sizeof(st), "%3i %3i", ci->health,	ci->armor);

            xx = x + TINYCHAR_WIDTH * 3 +
            	TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;

            CG_DrawStringExt( xx, y,
            	st, hcolor, qfalse, qfalse,
            	TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );

            // draw weapon icon
            xx += TINYCHAR_WIDTH * 3;

            if ( cg_weapons[ci->curWeapon].weaponIcon ) {
            	CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
            		cg_weapons[ci->curWeapon].weaponIcon );
            } else {
            	CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
            		cgs.media.deferShader );
            }

            // Draw powerup icons
            if (right) {
            	xx = x;
            } else {
            	xx = x + w - TINYCHAR_WIDTH;
            }

            for (j = 0; j <= PW_NUM_POWERUPS; j++) {
            	if (ci->powerups & (1 << j)) {

            		item = BG_FindItemForPowerup( j );

            		if (item) {
            			CG_DrawPic( xx, y, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
            			trap_R_RegisterShader( item->icon ) );
            			if (right) {
            				xx -= TINYCHAR_WIDTH;
            			} else {
            				xx += TINYCHAR_WIDTH;
            			}
            		}
            	}
            }

            y += TINYCHAR_HEIGHT;*/
        }
    }

    return;
//#endif
}

/*static float CG_DrawFollowMessage( float y ) {
	char		*s;
	int			w;

	if ( !(cg.snap->ps.pm_flags & PMF_FOLLOW) || ( ( cgs.elimflags & EF_NO_FREESPEC ) && (cgs.gametype == GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION ) ) ) {
		return y;
	}

	s = va("USE_ITEM to stop following");
	w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
	CG_DrawSmallString( 635 - w, y + 2, s, 1.0F);

	return y + SMALLCHAR_HEIGHT+4;
}*/

/*
=================
CG_DrawLivingCount

Draw the small two score display
=================
*/
static void CG_DrawLivingCount ( void ) {
    /*const char	*s, *t;
    int			s1, s2;*/

    if ( cgs.gametype != GT_ELIMINATION && cgs.gametype != GT_CTF_ELIMINATION )
        return;

    /*s1 = cgs.redLivingCount;
    s2 = cgs.blueLivingCount;
    s = va ( "%i", s1 );
    t = va ( "%i", s2 );*/

    CG_DrawHudIcon ( HUD_TI_OWN, qtrue, ( qhandle_t ) NULL ); //TODO: looks nasty
    CG_DrawHudIcon ( HUD_TI_NME, qtrue, ( qhandle_t ) NULL ); //

    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
        CG_DrawStringHud ( HUD_TC_NME, qtrue, va ( "%i", cgs.blueLivingCount ) );
        CG_DrawStringHud ( HUD_TC_OWN, qtrue, va ( "%i", cgs.redLivingCount ) );
    } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
        CG_DrawStringHud ( HUD_TC_NME, qtrue, va ( "%i", cgs.redLivingCount ) );
        CG_DrawStringHud ( HUD_TC_OWN, qtrue, va ( "%i", cgs.blueLivingCount ) );
    }

    return;
}


/*
=====================
CG_DrawUpperRight
TODO: This functions are not used anymore, or should be
=====================
*/
/*static void CG_DrawUpperRight( void ) {
    float	y;

    y = 0;
    */
    /*if ( cgs.gametype == GT_DOUBLE_D ) {
    	y = CG_DrawDoubleDominationThings(y);
    }
    else*/
    /*if ( cgs.gametype == GT_LMS && cg.showScores ) {
        y = CG_DrawLMSmode ( y );
    } else
        if ( cgs.gametype == GT_CTF_ELIMINATION ) {
            y = CG_DrawCTFoneway ( y );
        } else
            if ( cgs.gametype == GT_DOMINATION ) {
                y = CG_DrawDomStatus ( y );
            }

    if ( cg_drawSnapshot.integer ) {
        y = CG_DrawSnapshot ( y );
    }


    //y = CG_DrawFollowMessage( y );




}*/

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

/*
=================
CG_DrawScores

Draw the small two score display
=================
*/
#ifndef MISSIONPACK
static void CG_DrawScores ( void ) {
    const char	*s;
    int			s1, s2, score;
    int			x, w;
    int			v;
    gitem_t		*item;
    //int statusA,statusB;
    //vec4_t color;

    //statusA = cgs.redflag;
    //statusB = cgs.blueflag;

    s1 = cgs.scores1;
    s2 = cgs.scores2;

    // draw from the right side to left
    if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
        s = va ( "%i", s2 );

        if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
            CG_DrawScoresHud ( HUD_SCOREOWN, s, qfalse );
        else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED )
            CG_DrawScoresHud ( HUD_SCORENME, s, qfalse );
        else
            CG_DrawScoresHud ( HUD_SCOREOWN, s, qfalse );


        if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION ) {
            // Display flag status
            item = BG_FindItemForPowerup ( PW_BLUEFLAG );
            if ( item ) {
                if ( cgs.blueflag >= 0 && cgs.blueflag <= 2 ) {
                    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
                        trap_R_SetColor ( cgs.hud[HUD_FS_OWN].color );
                        CG_DrawHudIcon ( HUD_FS_OWN, qfalse, cgs.media.neutralFlagShader[cgs.blueflag] );
                    } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
                        trap_R_SetColor ( cgs.hud[HUD_FS_NME].color );
                        CG_DrawHudIcon ( HUD_FS_NME, qfalse, cgs.media.neutralFlagShader[cgs.blueflag] );
                    }
                    trap_R_SetColor ( NULL );
                }
            }
        }
        s = va ( "%i", s1 );

        if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED )
            CG_DrawScoresHud ( HUD_SCOREOWN, s, qfalse );
        else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE )
            CG_DrawScoresHud ( HUD_SCORENME, s, qfalse );
        else
            CG_DrawScoresHud ( HUD_SCORENME, s, qtrue );

        if ( cgs.gametype == GT_CTF || cgs.gametype == GT_CTF_ELIMINATION ) {
            // Display flag status
            item = BG_FindItemForPowerup ( PW_REDFLAG );
            if ( item ) {
                if ( cgs.redflag >= 0 && cgs.redflag <= 2 ) {
                    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
                        trap_R_SetColor ( cgs.hud[HUD_FS_OWN].color );
                        CG_DrawHudIcon ( HUD_FS_OWN, qtrue, cgs.media.neutralFlagShader[cgs.redflag] );
                    } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
                        trap_R_SetColor ( cgs.hud[HUD_FS_NME].color );
                        CG_DrawHudIcon ( HUD_FS_NME, qtrue, cgs.media.neutralFlagShader[cgs.redflag] );
                    }
                    trap_R_SetColor ( NULL );
                }
            }
        }

        if ( cgs.gametype >= GT_CTF && cgs.ffa_gt==0 ) {
            v = cgs.capturelimit;
        } else {
            v = cgs.fraglimit;
        }
        if ( v ) {
            s = va ( "%i", v );
            CG_DrawScoresHud ( HUD_SCORELIMIT, s, qfalse );
        }

    } else {
        qboolean	spectator;

        x = 640;
        score = cg.snap->ps.persistant[PERS_SCORE];
        spectator = ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR );

        // always show your score in the second box if not in first place
        if ( s1 != score ) {
            s2 = score;
        }
        if ( s2 != SCORE_NOT_PRESENT ) {
            s = va ( "%i", s2 );
            w = CG_DrawStrlen ( s ) * BIGCHAR_WIDTH + 8;
            x -= w;
            if ( spectator ) {
                CG_DrawScoresHud ( HUD_SCORENME, s, qtrue );
            } else if ( score == s2 && score != s1 ) {
                CG_DrawScoresHud ( HUD_SCOREOWN, s, qfalse );
            } else {
                CG_DrawScoresHud ( HUD_SCORENME, s, qfalse );
            }
        }

        // first place
        if ( s1 != SCORE_NOT_PRESENT ) {
            s = va ( "%i", s1 );
            w = CG_DrawStrlen ( s ) * BIGCHAR_WIDTH + 8;
            x -= w;
            if ( spectator ) {
                CG_DrawScoresHud ( HUD_SCOREOWN, s, qtrue );
            } else if ( score == s1 ) {
                CG_DrawScoresHud ( HUD_SCOREOWN, s, qfalse );
            } else {
                CG_DrawScoresHud ( HUD_SCORENME, s, qfalse );
            }
        }

        if ( cgs.fraglimit ) {
            s = va ( "%i", cgs.fraglimit );
            CG_DrawScoresHud ( HUD_SCORELIMIT, s, qfalse );
        }

    }

    return;
}
#endif // MISSIONPACK

/*
================
CG_DrawPowerups
================
*/
#ifndef MISSIONPACK
static void CG_DrawPowerups ( void ) {
    int		sorted[MAX_POWERUPS];
    int		sortedTime[MAX_POWERUPS];
    int		i, j, k;
    int		active;
    playerState_t	*ps;
    int		t;
    gitem_t	*item;
    int		color;
    static float colors[2][4] = {
        { 0.2f, 1.0f, 0.2f, 1.0f } ,
        { 1.0f, 0.2f, 0.2f, 1.0f }
    };

    ps = &cg.snap->ps;

    if ( ps->stats[STAT_HEALTH] <= 0 ) {
        return;
    }

    // sort the list by time remaining
    active = 0;
    for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
        if ( !ps->powerups[ i ] ) {
            continue;
        }
        // ZOID--don't draw if the power up has unlimited time
        // This is true of the CTF flags
        if ( ps->powerups[ i ] == INT_MAX ) {
            continue;
        }

        t = ps->powerups[ i ] - cg.time;
        if ( t <= 0 ) {
            continue;
        }

        item = BG_FindItemForPowerup ( i );
        if ( item && item->giType == IT_PERSISTANT_POWERUP )
            continue; //Don't draw persistant powerups here!

        // insert into the list
        for ( j = 0 ; j < active ; j++ ) {
            if ( sortedTime[j] >= t ) {
                for ( k = active - 1 ; k >= j ; k-- ) {
                    sorted[k+1] = sorted[k];
                    sortedTime[k+1] = sortedTime[k];
                }
                break;
            }
        }
        sorted[j] = i;
        sortedTime[j] = t;
        active++;
    }

    // draw the icons and timers
    for ( i = 0 ; i < active ; i++ ) {
        item = BG_FindItemForPowerup ( sorted[i] );

        if ( item ) {

            color = 1;
            trap_R_SetColor ( colors[color] );

            if ( i < 4 ) {
                CG_DrawFieldHud ( sortedTime[ i ] / 1000, HUD_PU1+i );
                CG_DrawHudIcon ( HUD_PU1ICON + i, qfalse, trap_R_RegisterShader ( item->icon ) );
                if ( sorted[i] == PW_QUAD ) {
                    CG_DrawFieldFontsize(cgs.hud[HUD_PU1ICON + i].xpos, cgs.hud[HUD_PU1ICON + i].ypos, 2, cg.quadKills, 10, 10);
                }
            }

        }
    }
    trap_R_SetColor ( NULL );

    return;
}
#endif // MISSIONPACK

/*
=====================
CG_DrawLowerRight
TODO: not used anymore
=====================
*/
#ifndef MISSIONPACK
/*static void CG_DrawLowerRight ( void ) {
    float	y;

    y = 480 - ICON_SIZE;

    if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 && cg_drawTeamOverlay.integer == 2 ) {
        CG_DrawTeamOverlay ( qtrue, qfalse );
    }

    CG_DrawScores();
    CG_DrawPowerups();
}*/
#endif // MISSIONPACK

/*
=====================
CG_DrawLowerLeft
TODO: not used anymore
=====================
*/
#ifndef MISSIONPACK
/*static void CG_DrawLowerLeft ( void ) {
    float	y;

    y = 480 - ICON_SIZE;

    if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 && cg_drawTeamOverlay.integer == 3 ) {
        CG_DrawTeamOverlay ( qfalse, qfalse );
    }
}*/
#endif // MISSIONPACK


//===========================================================================================

/*
=================
CG_DrawTeamInfo
TODO: why isnt it called CG_DrawTeamChat?
TODO: add time hud-propertie
=================
*/
#ifndef MISSIONPACK
static void CG_DrawTeamInfo ( void ) {
    int w;
    int i;
    vec4_t		hcolor;
    int		chatHeight = TEAMCHAT_HEIGHT;
    hudElements_t	hudelement;
    int 		x;

    hcolor[0] = hcolor[1] = hcolor[2] = 1.0f;
    hcolor[3] = 1.0f;

    if ( !cg.forceChat ) {
        if ( cgs.teamLastChatPos != cgs.teamChatPos ) {
            if ( cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer ) {
                cgs.teamLastChatPos++;
            }

            for ( i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i-- ) {
                hudelement = cgs.hud[HUD_TEAMCHAT1 + i - cgs.teamLastChatPos ];

                if ( !hudelement.inuse )
                    return;

                w = CG_DrawStrlen ( cgs.teamChatMsgs[i % chatHeight ] ) * hudelement.fontWidth;

                if ( hudelement.textAlign == 0 )
                    x = hudelement.xpos;
                else if ( hudelement.textAlign == 2 )
                    x = hudelement.xpos + hudelement.width - w;
                else
                    x = hudelement.xpos + hudelement.width/2 - w/2;

                CG_DrawStringExt ( x,
                                   hudelement.ypos,
                                   cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse,
                                   qfalse, hudelement.fontWidth, hudelement.fontHeight, 0 );
            }
        }
    } else {
        for ( i = cgs.teamChatPos - 1; i >= cgs.teamChatPos - chatHeight; i-- ) {
            hudelement = cgs.hud[HUD_TEAMCHAT1 + i - cgs.teamChatPos + chatHeight ];

            if ( !hudelement.inuse )
                return;

            w = CG_DrawStrlen ( cgs.teamChatMsgs[i % chatHeight ] ) * hudelement.fontWidth;

            if ( hudelement.textAlign == 0 )
                x = hudelement.xpos;
            else if ( hudelement.textAlign == 2 )
                x = hudelement.xpos + hudelement.width - w;
            else
                x = hudelement.xpos + hudelement.width/2 - w/2;

            CG_DrawStringExt ( x,
                               hudelement.ypos,
                               cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse,
                               qfalse, hudelement.fontWidth, hudelement.fontHeight, 0 );
        }
    }
}
#endif // MISSIONPACK

/*
=================
CG_DrawConsole
TODO:hud time
=================
*/
static void CG_DrawConsole ( void ) {
    int i;
    int xpos, len;
    vec4_t hcolor;
    int consoleHeight = TEAMCHAT_HEIGHT;

    if ( !cgs.hud[HUD_CONSOLE].inuse )
        return;

    if ( cgs.lastConsolePos != cgs.consolePos ) {
        if ( cg.time - cgs.consoleMsgTimes[cgs.lastConsolePos % consoleHeight] >
                cg_consoleTime.integer ) {
            cgs.lastConsolePos++;
        }

        hcolor[0] = hcolor[1] = hcolor[2] = 1.0f;
        hcolor[3] = 1.0f;

        for ( i = cgs.lastConsolePos; i < cgs.consolePos; i++ ) {
            len = CG_DrawStrlen( cgs.consoleMsgs[i % consoleHeight] );

            if ( cgs.hud[HUD_CONSOLE].textAlign == 0 ) {
                xpos = cgs.hud[HUD_CONSOLE].xpos;
            }
            else if ( cgs.hud[HUD_CONSOLE].textAlign == 2 ) {
                xpos = cgs.hud[HUD_CONSOLE].xpos + cgs.hud[HUD_CONSOLE].width - cgs.hud[HUD_CONSOLE].fontWidth * len;
            }
            else {
                xpos = cgs.hud[HUD_CONSOLE].xpos + cgs.hud[HUD_CONSOLE].width/2 - cgs.hud[HUD_CONSOLE].fontWidth * len/2;
            }

            CG_DrawStringExt ( xpos,
                               cgs.hud[HUD_CONSOLE].ypos + ( i - cgs.lastConsolePos ) * cgs.hud[HUD_CONSOLE].fontHeight,
                               cgs.consoleMsgs[i % consoleHeight], hcolor, qfalse,
                               qfalse, cgs.hud[HUD_CONSOLE].fontWidth, cgs.hud[HUD_CONSOLE].fontHeight, 0 );
        }
    }
}

/*
=================
CG_DrawChat
TODO: hud time
=================
*/
static void CG_DrawChat ( qboolean endOfGame ) {
    int i;
    vec4_t hcolor;
    int chatHeight = TEAMCHAT_HEIGHT;

    hcolor[0] = hcolor[1] = hcolor[2] = 1.0f;
    hcolor[3] = 1.0f;

    if ( !cg.forceChat ) {
        if ( cgs.lastChatPos != cgs.chatPos ) {
            if ( cg.time - cgs.chatMsgTimes[cgs.lastChatPos % chatHeight] >
                    cg_chatTime.integer ) {
                cgs.lastChatPos++;
            }

            for ( i = cgs.lastChatPos; i < cgs.chatPos; i++ ) {
                if ( !endOfGame ) {
                    CG_DrawStringHud ( HUD_CHAT1 + i - cgs.lastChatPos, qfalse, ( const char* ) cgs.chatMsgs[i % chatHeight] );

                } else
                    CG_DrawStringExt ( 0,
                                       0 + ( i - cgs.lastChatPos ) * TINYCHAR_HEIGHT,
                                       cgs.chatMsgs[i % chatHeight], hcolor, qfalse,
                                       qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
            }
        }
    } else {
        for ( i = cgs.chatPos - chatHeight; i < cgs.chatPos; i++ ) {
            if ( !endOfGame ) {
                CG_DrawStringHud ( HUD_CHAT1 + i - cgs.chatPos + chatHeight, qfalse, ( const char* ) cgs.chatMsgs[i % chatHeight] );

            } else
                CG_DrawStringExt ( 0,
                                   0 + ( i - cgs.chatPos + chatHeight ) * TINYCHAR_HEIGHT,
                                   cgs.chatMsgs[i % chatHeight], hcolor, qfalse,
                                   qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
        }
    }
}

/*
=================
CG_DrawDeathNotice
TODO: hud time
=================
*/

static void CG_DrawDeathNotice ( void ) {
    int i;
    int x;
    int y;
    int width;
    vec4_t color1,color2;
    qboolean setcolor1, setcolor2;
    hudElements_t hudelement;

    for ( i = 0; i < 5 ; i++ ) {
        hudelement = cgs.hud[HUD_DEATHNOTICE1 + i ];
        y = hudelement.ypos;
        if ( ( cgs.deathNoticeTime[ i ] != 0 ) && ( cg.time - cgs.deathNoticeTime[ i ] < cg_deathNoticeTime.integer ) && hudelement.inuse ) {

            if ( cgs.deathNoticeTeam1[ i ] == TEAM_RED ) {
                color1[0]=color1[3]=1.0F;
                color1[1]=color1[2]=0.2f;
                setcolor1 = qtrue;
            } else if ( cgs.deathNoticeTeam1[ i ] == TEAM_BLUE ) {
                color1[2]=color1[3]=1.0F;
                color1[1]=color1[0]=0.2f;
                setcolor1 = qtrue;
            } else {
                color1[0]=color1[1]=color1[2]=color1[3]=1.0F;
                setcolor1 = qfalse;
            }

            if ( cgs.deathNoticeTeam2[ i ] == TEAM_RED ) {
                color2[0]=color2[3]=1.0F;
                color2[1]=color2[2]=0.2f;
                setcolor2 = qtrue;
            } else if ( cgs.deathNoticeTeam2[ i ] == TEAM_BLUE ) {
                color2[2]=color2[3]=1.0F;
                color2[1]=color2[0]=0.2f;
                setcolor2 = qtrue;
            } else {
                color2[0]=color2[1]=color2[2]=color2[3]=1.0F;
                setcolor2 = qfalse;
            }

            if ( cgs.deathNoticeTwoIcons[ i ] )
                width = hudelement.fontWidth * ( CG_DrawStrlen ( cgs.deathNoticeName1[ i ] ) + CG_DrawStrlen ( cgs.deathNoticeName2[ i ] ) ) + 2* ( hudelement.fontHeight + 2 );
            else
                width = hudelement.fontWidth * ( CG_DrawStrlen ( cgs.deathNoticeName1[ i ] ) + CG_DrawStrlen ( cgs.deathNoticeName2[ i ] ) ) + hudelement.fontHeight + 2;

            if ( hudelement.textAlign == 0 ) {
                x = hudelement.xpos;
            } else if ( hudelement.textAlign == 2 ) {
                x = hudelement.xpos + hudelement.width - width;
            } else {
                x = hudelement.xpos + hudelement.width/2 - width/2;
            }

            CG_DrawStringExt ( x,y, cgs.deathNoticeName1[ i ], color1, setcolor1, qfalse, hudelement.fontWidth, hudelement.fontHeight, 0 );

            x += hudelement.fontWidth* ( CG_DrawStrlen ( cgs.deathNoticeName1[ i ] ) );

            if ( cgs.deathNoticeTwoIcons[ i ] ) {
                CG_DrawPic ( x + 1, y, hudelement.fontHeight, hudelement.fontHeight, cgs.deathNoticeIcon2[ i ] );
                x += hudelement.fontHeight + 2;
            }

            CG_DrawPic ( x + 1, y, hudelement.fontHeight, hudelement.fontHeight, cgs.deathNoticeIcon1[ i ] );

            x += hudelement.fontHeight + 2;

            CG_DrawStringExt ( x, y, cgs.deathNoticeName2[ i ], color2, setcolor2, qfalse, hudelement.fontWidth, hudelement.fontHeight, 0 );
        }
    }
}

/*
===================
CG_DrawHoldableItem
===================
*/
#ifndef MISSIONPACK
static void CG_DrawHoldableItem ( void ) {
    int		value;

    value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];
    if ( value ) {
        CG_RegisterItemVisuals ( value );
        CG_DrawPic ( 640-ICON_SIZE, ( SCREEN_HEIGHT-ICON_SIZE ) /2, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
    }

}
#endif // MISSIONPACK

#ifndef MISSIONPACK
/*
===================
CG_DrawPersistantPowerup
===================
*/
#if 1 // sos001208 - DEAD // sago - ALIVE
static void CG_DrawPersistantPowerup ( void ) {
    int		value;

    value = cg.snap->ps.stats[STAT_PERSISTANT_POWERUP];
    if ( value ) {
        CG_RegisterItemVisuals ( value );
        CG_DrawPic ( 640-ICON_SIZE, ( SCREEN_HEIGHT-ICON_SIZE ) /2 - ICON_SIZE, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
    }
}
#endif
#endif // MISSIONPACK


/*
===================
CG_DrawReward
===================
*/
static void CG_DrawReward ( void ) {
    float	*color;
    int		i;
    //float	x, y;
    //int maxRewards;
    //char	buf[32];  //TODO:aha, 32 digits are useful?

    if ( !cg_drawRewards.integer ) {
        return;
    }

    color = CG_FadeColor ( cg.rewardTime, REWARD_TIME );
    if ( !color ) {
        if ( cg.rewardStack > 0 ) {
            for ( i = 0; i < cg.rewardStack; i++ ) {
                cg.rewardSound[i] = cg.rewardSound[i+1];

                cg.rewardShader[i] = cg.rewardShader[i+1];
                cg.rewardCount[i] = cg.rewardCount[i+1];
            }
            cg.rewardTime = cg.time;
            cg.rewardStack--;
            color = CG_FadeColor ( cg.rewardTime, REWARD_TIME );

            trap_S_StartLocalSound ( cg.rewardSound[0], CHAN_ANNOUNCER );

        } else {
            return;
        }
    }

    trap_R_SetColor ( color );

    //maxRewards = (cgs.hud[HUD_REWARD].width - cgs.hud[HUD_REWARD].xpos)/cgs.hud[HUD_REWARD].height;
    //for( i=0; i < maxRewards && i <= cg.rewardStack; i++ ){
    //	CG_DrawPic( cgs.hud[HUD_REWARD].xpos + i*cgs.hud[HUD_REWARD].height - (cg.rewardStack + 1) * cgs.hud[HUD_REWARD].height/2 + cgs.hud[HUD_REWARD].width/2, cgs.hud[HUD_REWARD].ypos, cgs.hud[HUD_REWARD].height, cgs.hud[HUD_REWARD].height, cg.rewardShader[i]);
    //}

    CG_DrawHudIcon ( HUD_REWARD, qfalse, cg.rewardShader[0] );

    //Com_sprintf ( buf, sizeof ( buf ), "%i", cg.rewardCount[0] );

    CG_DrawStringHud ( HUD_REWARDCOUNT, qfalse, va ( "%i", cg.rewardCount[0] ) );
    trap_R_SetColor ( NULL );
}

/*
===================
CG_DrawRespawnTimers
===================
*/
void CG_DrawRespawnTimers ( void ) {
    int i;
    int time;
    int x,y;
    int xpos, ypos, width, height, boxWidth, boxHeight, textalign;
    qboolean horizontal;
    int count;
    int iconsize, icon_xrel, icon_yrel, text_xrel, text_yrel, text_step;
    char *s;
    int w;
    vec4_t color;
    hudElements_t hudelement = cgs.hud[HUD_RESPAWNTIMER];

    if ( !hudelement.inuse )
        return;

    height = hudelement.height;
    width = hudelement.width;
    xpos = hudelement.xpos;
    ypos = hudelement.ypos;
    textalign = hudelement.textAlign;

    horizontal = height < width;

    count = cgs.respawnTimerNumber;


    if ( horizontal ) {
        boxHeight = height;
        boxWidth = width/8;
        x = xpos + width/2 - ( boxWidth*count ) /2;
        y = ypos;
    } else {
        boxHeight = height/8;
        boxWidth = width;
        x = xpos;
        y = ypos + height/2 - ( boxHeight*count ) /2;
    }

    if ( textalign == 0 ) {
        if ( boxHeight < ( boxWidth - 2*hudelement.fontWidth - 6 ) )
            iconsize = boxHeight;
        else
            iconsize = ( boxWidth - 2*hudelement.fontWidth - 6 );

        icon_yrel = boxHeight/2 - iconsize/2;
        icon_xrel = boxWidth - iconsize - 2;

        text_yrel = boxHeight/2 - hudelement.fontHeight/2;
        text_xrel = 2;
        text_step = 0;
    } else if ( textalign == 2 ) {
        if ( boxHeight < ( boxWidth - 2*hudelement.fontWidth - 6 ) )
            iconsize = boxHeight;
        else
            iconsize = ( boxWidth - 2*hudelement.fontWidth - 6 );

        icon_yrel = boxHeight/2 - iconsize/2;
        icon_xrel = 2;

        text_yrel = boxHeight/2 - hudelement.fontHeight/2;
        text_xrel = boxWidth - 2*hudelement.fontWidth - 2;
        text_step = hudelement.fontWidth;
    } else {
        if ( boxWidth < ( boxHeight - hudelement.fontHeight - 6 ) )
            iconsize = boxWidth;
        else
            iconsize = ( boxHeight - hudelement.fontHeight - 6 );

        icon_xrel = boxWidth/2 - iconsize/2;
        icon_yrel = 2;

        text_xrel = boxWidth/2 - 2*hudelement.fontWidth/2;
        text_yrel = boxHeight - hudelement.fontHeight - 2;
        text_step = hudelement.fontWidth/2;
    }

    if ( iconsize < 0 )
        iconsize = 0;

    for ( i = 0; i < count; i++ ) {
        time = ( cgs.respawnTimerTime[i] - cg.time );
        if ( cg_items[cg_entities[cgs.respawnTimerEntitynum[i]].currentState.modelindex].icon ) {
            CG_DrawPic ( x + icon_xrel, y + icon_yrel , iconsize, iconsize, cg_items[cg_entities[cgs.respawnTimerEntitynum[i]].currentState.modelindex].icon );
            if ( cgs.respawnTimerNextItem[i] != -1 &&  cg_items[cg_entities[cgs.respawnTimerNextItem[i]].currentState.modelindex].icon ) {
                CG_DrawPic ( x + icon_xrel + iconsize - iconsize/4, y + icon_yrel + iconsize - iconsize/4, iconsize/4, iconsize/4, cg_items[cg_entities[cgs.respawnTimerNextItem[i]].currentState.modelindex].icon );
            }
            
            color[3] = 1.0f;
            if( cgs.gametype >= GT_TEAM ){
		    if( cgs.respawnTimerClientNum[i] == TEAM_RED ){
			    color[0] = 1.0f;
			    color[1] = 0.2f;;
			    color[2] = 0.2f;
		    } else {
			    color[0] = 0.2f;
			    color[1] = 0.2f;
			    color[2] = 1.0f;
		    }
	    } else {
		    if( cgs.respawnTimerClientNum[i] == cg.snap->ps.clientNum ){
			    VectorCopy(colorWhite,color);
		    } else {
			    VectorCopy(colorGreen,color);
		    }
	      
	    }
            
            if ( time > 0 ) {
                s = va ( "%i", time/1000+1 );
                w = CG_DrawStrlen ( s );
                CG_DrawStringExt ( x + text_xrel + ( 2 - w ) *text_step, y + text_yrel, s, color, qtrue, hudelement.textstyle & 1, hudelement.fontWidth, hudelement.fontHeight, 0 );
            } else {
		s = va ( "*" );
                w = CG_DrawStrlen ( s );
                CG_DrawStringExt ( x + text_xrel + ( 2 - w ) *text_step, y + text_yrel, s, color, qtrue, hudelement.textstyle & 1, hudelement.fontWidth, hudelement.fontHeight, 0 );
            }

            if ( cgs.gametype == GT_CTF ) {
                if ( cgs.respawnTimerTeam[ i ] == TEAM_RED ) {
                    CG_DrawPic ( x + icon_xrel, y + icon_yrel + iconsize - iconsize/4, iconsize/4, iconsize/4, cgs.media.redMarker );
                } else if ( cgs.respawnTimerTeam[ i ] == TEAM_BLUE ) {
                    CG_DrawPic ( x + icon_xrel, y + icon_yrel + iconsize - iconsize/4, iconsize/4, iconsize/4, cgs.media.blueMarker );
                }
            }

            if ( horizontal )
                x += boxWidth;
            else
                y += boxHeight;
        }

    }
}

//
// these are the key numbers that should be passed to KeyEvent
//

// normal keys should be passed as lowercased ascii

typedef enum {
    K_TAB = 9,
    K_ENTER = 13,
    K_ESCAPE = 27,
    K_SPACE = 32,

    K_BACKSPACE = 127,

    K_COMMAND = 128,
    K_CAPSLOCK,
    K_POWER,
    K_PAUSE,

    K_UPARROW,
    K_DOWNARROW,
    K_LEFTARROW,
    K_RIGHTARROW,

    K_ALT,
    K_CTRL,
    K_SHIFT,
    K_INS,
    K_DEL,
    K_PGDN,
    K_PGUP,
    K_HOME,
    K_END,

    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_F13,
    K_F14,
    K_F15,

    K_KP_HOME,
    K_KP_UPARROW,
    K_KP_PGUP,
    K_KP_LEFTARROW,
    K_KP_5,
    K_KP_RIGHTARROW,
    K_KP_END,
    K_KP_DOWNARROW,
    K_KP_PGDN,
    K_KP_ENTER,
    K_KP_INS,
    K_KP_DEL,
    K_KP_SLASH,
    K_KP_MINUS,
    K_KP_PLUS,
    K_KP_NUMLOCK,
    K_KP_STAR,
    K_KP_EQUALS,

    K_MOUSE1,
    K_MOUSE2,
    K_MOUSE3,
    K_MOUSE4,
    K_MOUSE5,

    K_MWHEELDOWN,
    K_MWHEELUP,

    K_JOY1,
    K_JOY2,
    K_JOY3,
    K_JOY4,
    K_JOY5,
    K_JOY6,
    K_JOY7,
    K_JOY8,
    K_JOY9,
    K_JOY10,
    K_JOY11,
    K_JOY12,
    K_JOY13,
    K_JOY14,
    K_JOY15,
    K_JOY16,
    K_JOY17,
    K_JOY18,
    K_JOY19,
    K_JOY20,
    K_JOY21,
    K_JOY22,
    K_JOY23,
    K_JOY24,
    K_JOY25,
    K_JOY26,
    K_JOY27,
    K_JOY28,
    K_JOY29,
    K_JOY30,
    K_JOY31,
    K_JOY32,

    K_AUX1,
    K_AUX2,
    K_AUX3,
    K_AUX4,
    K_AUX5,
    K_AUX6,
    K_AUX7,
    K_AUX8,
    K_AUX9,
    K_AUX10,
    K_AUX11,
    K_AUX12,
    K_AUX13,
    K_AUX14,
    K_AUX15,
    K_AUX16,

    K_WORLD_0,
    K_WORLD_1,
    K_WORLD_2,
    K_WORLD_3,
    K_WORLD_4,
    K_WORLD_5,
    K_WORLD_6,
    K_WORLD_7,
    K_WORLD_8,
    K_WORLD_9,
    K_WORLD_10,
    K_WORLD_11,
    K_WORLD_12,
    K_WORLD_13,
    K_WORLD_14,
    K_WORLD_15,
    K_WORLD_16,
    K_WORLD_17,
    K_WORLD_18,
    K_WORLD_19,
    K_WORLD_20,
    K_WORLD_21,
    K_WORLD_22,
    K_WORLD_23,
    K_WORLD_24,
    K_WORLD_25,
    K_WORLD_26,
    K_WORLD_27,
    K_WORLD_28,
    K_WORLD_29,
    K_WORLD_30,
    K_WORLD_31,
    K_WORLD_32,
    K_WORLD_33,
    K_WORLD_34,
    K_WORLD_35,
    K_WORLD_36,
    K_WORLD_37,
    K_WORLD_38,
    K_WORLD_39,
    K_WORLD_40,
    K_WORLD_41,
    K_WORLD_42,
    K_WORLD_43,
    K_WORLD_44,
    K_WORLD_45,
    K_WORLD_46,
    K_WORLD_47,
    K_WORLD_48,
    K_WORLD_49,
    K_WORLD_50,
    K_WORLD_51,
    K_WORLD_52,
    K_WORLD_53,
    K_WORLD_54,
    K_WORLD_55,
    K_WORLD_56,
    K_WORLD_57,
    K_WORLD_58,
    K_WORLD_59,
    K_WORLD_60,
    K_WORLD_61,
    K_WORLD_62,
    K_WORLD_63,
    K_WORLD_64,
    K_WORLD_65,
    K_WORLD_66,
    K_WORLD_67,
    K_WORLD_68,
    K_WORLD_69,
    K_WORLD_70,
    K_WORLD_71,
    K_WORLD_72,
    K_WORLD_73,
    K_WORLD_74,
    K_WORLD_75,
    K_WORLD_76,
    K_WORLD_77,
    K_WORLD_78,
    K_WORLD_79,
    K_WORLD_80,
    K_WORLD_81,
    K_WORLD_82,
    K_WORLD_83,
    K_WORLD_84,
    K_WORLD_85,
    K_WORLD_86,
    K_WORLD_87,
    K_WORLD_88,
    K_WORLD_89,
    K_WORLD_90,
    K_WORLD_91,
    K_WORLD_92,
    K_WORLD_93,
    K_WORLD_94,
    K_WORLD_95,

    K_SUPER,
    K_COMPOSE,
    K_MODE,
    K_HELP,
    K_PRINT,
    K_SYSREQ,
    K_SCROLLOCK,
    K_BREAK,
    K_MENU,
    K_EURO,
    K_UNDO,

    // Pseudo-key that brings the console down
    K_CONSOLE,

    MAX_KEYS
} keyNum_t;

typedef struct {
    char	*name;
    int		keynum;
} keyname_t;


// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] =
{
    {"TAB", K_TAB},
    {"ENTER", K_ENTER},
    {"ESCAPE", K_ESCAPE},
    {"SPACE", K_SPACE},
    {"BACKSPACE", K_BACKSPACE},
    {"UPARROW", K_UPARROW},
    {"DOWNARROW", K_DOWNARROW},
    {"LEFTARROW", K_LEFTARROW},
    {"RIGHTARROW", K_RIGHTARROW},

    {"ALT", K_ALT},
    {"CTRL", K_CTRL},
    {"SHIFT", K_SHIFT},

    {"COMMAND", K_COMMAND},

    {"CAPSLOCK", K_CAPSLOCK},


    {"F1", K_F1},
    {"F2", K_F2},
    {"F3", K_F3},
    {"F4", K_F4},
    {"F5", K_F5},
    {"F6", K_F6},
    {"F7", K_F7},
    {"F8", K_F8},
    {"F9", K_F9},
    {"F10", K_F10},
    {"F11", K_F11},
    {"F12", K_F12},
    {"F13", K_F13},
    {"F14", K_F14},
    {"F15", K_F15},

    {"INS", K_INS},
    {"DEL", K_DEL},
    {"PGDN", K_PGDN},
    {"PGUP", K_PGUP},
    {"HOME", K_HOME},
    {"END", K_END},

    {"MOUSE1", K_MOUSE1},
    {"MOUSE2", K_MOUSE2},
    {"MOUSE3", K_MOUSE3},
    {"MOUSE4", K_MOUSE4},
    {"MOUSE5", K_MOUSE5},

    {"MWHEELUP",	K_MWHEELUP },
    {"MWHEELDOWN",	K_MWHEELDOWN },

    {"JOY1", K_JOY1},
    {"JOY2", K_JOY2},
    {"JOY3", K_JOY3},
    {"JOY4", K_JOY4},
    {"JOY5", K_JOY5},
    {"JOY6", K_JOY6},
    {"JOY7", K_JOY7},
    {"JOY8", K_JOY8},
    {"JOY9", K_JOY9},
    {"JOY10", K_JOY10},
    {"JOY11", K_JOY11},
    {"JOY12", K_JOY12},
    {"JOY13", K_JOY13},
    {"JOY14", K_JOY14},
    {"JOY15", K_JOY15},
    {"JOY16", K_JOY16},
    {"JOY17", K_JOY17},
    {"JOY18", K_JOY18},
    {"JOY19", K_JOY19},
    {"JOY20", K_JOY20},
    {"JOY21", K_JOY21},
    {"JOY22", K_JOY22},
    {"JOY23", K_JOY23},
    {"JOY24", K_JOY24},
    {"JOY25", K_JOY25},
    {"JOY26", K_JOY26},
    {"JOY27", K_JOY27},
    {"JOY28", K_JOY28},
    {"JOY29", K_JOY29},
    {"JOY30", K_JOY30},
    {"JOY31", K_JOY31},
    {"JOY32", K_JOY32},

    {"AUX1", K_AUX1},
    {"AUX2", K_AUX2},
    {"AUX3", K_AUX3},
    {"AUX4", K_AUX4},
    {"AUX5", K_AUX5},
    {"AUX6", K_AUX6},
    {"AUX7", K_AUX7},
    {"AUX8", K_AUX8},
    {"AUX9", K_AUX9},
    {"AUX10", K_AUX10},
    {"AUX11", K_AUX11},
    {"AUX12", K_AUX12},
    {"AUX13", K_AUX13},
    {"AUX14", K_AUX14},
    {"AUX15", K_AUX15},
    {"AUX16", K_AUX16},

    {"KP_HOME",			K_KP_HOME },
    {"KP_UPARROW",		K_KP_UPARROW },
    {"KP_PGUP",			K_KP_PGUP },
    {"KP_LEFTARROW",	K_KP_LEFTARROW },
    {"KP_5",			K_KP_5 },
    {"KP_RIGHTARROW",	K_KP_RIGHTARROW },
    {"KP_END",			K_KP_END },
    {"KP_DOWNARROW",	K_KP_DOWNARROW },
    {"KP_PGDN",			K_KP_PGDN },
    {"KP_ENTER",		K_KP_ENTER },
    {"KP_INS",			K_KP_INS },
    {"KP_DEL",			K_KP_DEL },
    {"KP_SLASH",		K_KP_SLASH },
    {"KP_MINUS",		K_KP_MINUS },
    {"KP_PLUS",			K_KP_PLUS },
    {"KP_NUMLOCK",		K_KP_NUMLOCK },
    {"KP_STAR",			K_KP_STAR },
    {"KP_EQUALS",		K_KP_EQUALS },

    {"PAUSE", K_PAUSE},

    {"SEMICOLON", ';'},	// because a raw semicolon seperates commands

    {"WORLD_0", K_WORLD_0},
    {"WORLD_1", K_WORLD_1},
    {"WORLD_2", K_WORLD_2},
    {"WORLD_3", K_WORLD_3},
    {"WORLD_4", K_WORLD_4},
    {"WORLD_5", K_WORLD_5},
    {"WORLD_6", K_WORLD_6},
    {"WORLD_7", K_WORLD_7},
    {"WORLD_8", K_WORLD_8},
    {"WORLD_9", K_WORLD_9},
    {"WORLD_10", K_WORLD_10},
    {"WORLD_11", K_WORLD_11},
    {"WORLD_12", K_WORLD_12},
    {"WORLD_13", K_WORLD_13},
    {"WORLD_14", K_WORLD_14},
    {"WORLD_15", K_WORLD_15},
    {"WORLD_16", K_WORLD_16},
    {"WORLD_17", K_WORLD_17},
    {"WORLD_18", K_WORLD_18},
    {"WORLD_19", K_WORLD_19},
    {"WORLD_20", K_WORLD_20},
    {"WORLD_21", K_WORLD_21},
    {"WORLD_22", K_WORLD_22},
    {"WORLD_23", K_WORLD_23},
    {"WORLD_24", K_WORLD_24},
    {"WORLD_25", K_WORLD_25},
    {"WORLD_26", K_WORLD_26},
    {"WORLD_27", K_WORLD_27},
    {"WORLD_28", K_WORLD_28},
    {"WORLD_29", K_WORLD_29},
    {"WORLD_30", K_WORLD_30},
    {"WORLD_31", K_WORLD_31},
    {"WORLD_32", K_WORLD_32},
    {"WORLD_33", K_WORLD_33},
    {"WORLD_34", K_WORLD_34},
    {"WORLD_35", K_WORLD_35},
    {"WORLD_36", K_WORLD_36},
    {"WORLD_37", K_WORLD_37},
    {"WORLD_38", K_WORLD_38},
    {"WORLD_39", K_WORLD_39},
    {"WORLD_40", K_WORLD_40},
    {"WORLD_41", K_WORLD_41},
    {"WORLD_42", K_WORLD_42},
    {"WORLD_43", K_WORLD_43},
    {"WORLD_44", K_WORLD_44},
    {"WORLD_45", K_WORLD_45},
    {"WORLD_46", K_WORLD_46},
    {"WORLD_47", K_WORLD_47},
    {"WORLD_48", K_WORLD_48},
    {"WORLD_49", K_WORLD_49},
    {"WORLD_50", K_WORLD_50},
    {"WORLD_51", K_WORLD_51},
    {"WORLD_52", K_WORLD_52},
    {"WORLD_53", K_WORLD_53},
    {"WORLD_54", K_WORLD_54},
    {"WORLD_55", K_WORLD_55},
    {"WORLD_56", K_WORLD_56},
    {"WORLD_57", K_WORLD_57},
    {"WORLD_58", K_WORLD_58},
    {"WORLD_59", K_WORLD_59},
    {"WORLD_60", K_WORLD_60},
    {"WORLD_61", K_WORLD_61},
    {"WORLD_62", K_WORLD_62},
    {"WORLD_63", K_WORLD_63},
    {"WORLD_64", K_WORLD_64},
    {"WORLD_65", K_WORLD_65},
    {"WORLD_66", K_WORLD_66},
    {"WORLD_67", K_WORLD_67},
    {"WORLD_68", K_WORLD_68},
    {"WORLD_69", K_WORLD_69},
    {"WORLD_70", K_WORLD_70},
    {"WORLD_71", K_WORLD_71},
    {"WORLD_72", K_WORLD_72},
    {"WORLD_73", K_WORLD_73},
    {"WORLD_74", K_WORLD_74},
    {"WORLD_75", K_WORLD_75},
    {"WORLD_76", K_WORLD_76},
    {"WORLD_77", K_WORLD_77},
    {"WORLD_78", K_WORLD_78},
    {"WORLD_79", K_WORLD_79},
    {"WORLD_80", K_WORLD_80},
    {"WORLD_81", K_WORLD_81},
    {"WORLD_82", K_WORLD_82},
    {"WORLD_83", K_WORLD_83},
    {"WORLD_84", K_WORLD_84},
    {"WORLD_85", K_WORLD_85},
    {"WORLD_86", K_WORLD_86},
    {"WORLD_87", K_WORLD_87},
    {"WORLD_88", K_WORLD_88},
    {"WORLD_89", K_WORLD_89},
    {"WORLD_90", K_WORLD_90},
    {"WORLD_91", K_WORLD_91},
    {"WORLD_92", K_WORLD_92},
    {"WORLD_93", K_WORLD_93},
    {"WORLD_94", K_WORLD_94},
    {"WORLD_95", K_WORLD_95},

    {"WINDOWS", K_SUPER},
    {"COMPOSE", K_COMPOSE},
    {"MODE", K_MODE},
    {"HELP", K_HELP},
    {"PRINT", K_PRINT},
    {"SYSREQ", K_SYSREQ},
    {"SCROLLOCK", K_SCROLLOCK },
    {"BREAK", K_BREAK},
    {"MENU", K_MENU},
    {"POWER", K_POWER},
    {"EURO", K_EURO},
    {"UNDO", K_UNDO},

    {NULL,0}
};

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
char *Key_KeynumToString( int keynum ) {
    keyname_t	*kn;
    static	char	tinystr[5];
    int			i, j;

    if ( keynum == -1 ) {
        return "<KEY NOT FOUND>";
    }

    if ( keynum < 0 || keynum >= MAX_KEYS ) {
        return "<OUT OF RANGE>";
    }

    // check for printable ascii (don't use quote)
    if ( keynum > 32 && keynum < 127 && keynum != '"' && keynum != ';' ) {
        tinystr[0] = keynum;
        tinystr[1] = 0;
        return tinystr;
    }

    // check for a key string
    for ( kn=keynames ; kn->name ; kn++ ) {
        if (keynum == kn->keynum) {
            return kn->name;
        }
    }

    // make a hex string
    i = keynum >> 4;
    j = keynum & 15;

    tinystr[0] = '0';
    tinystr[1] = 'x';
    tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
    tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
    tinystr[4] = 0;

    return tinystr;
}

/*
===================
CG_DrawReady
===================
*/
static void CG_DrawReady ( void ) {
    int key;
    if ( !cgs.startWhenReady )
        return;
    if ( cg.warmup >= 0 )
        return;

    key = trap_Key_GetKey("ready");
    if ( key > 0 && key < 256 ) {
        if ( cg.readyMask & ( 1 << cg.snap->ps.clientNum ) ) {
            CG_DrawStringHud ( HUD_READYSTATUS, qfalse, va( "^2You are READY, press \"%s\" to get not ready", Key_KeynumToString(key) ) );
        } else {
            CG_DrawStringHud ( HUD_READYSTATUS, qfalse, va( "^1You are not READY, press \"%s\" to get ready", Key_KeynumToString(key) ) );
        }
    }
    else {
        if ( cg.readyMask & ( 1 << cg.snap->ps.clientNum ) ) {
            CG_DrawStringHud ( HUD_READYSTATUS, qfalse, va( "^2You are READY" ) );
        } else {
            CG_DrawStringHud ( HUD_READYSTATUS, qfalse, va( "^1You are not READY, type \"\\ready\" to get ready" ) );
        }
    }
}

/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define	LAG_SAMPLES		128


typedef struct {
    int		frameSamples[LAG_SAMPLES];
    int		frameCount;
    int		snapshotFlags[LAG_SAMPLES];
    int		snapshotSamples[LAG_SAMPLES];
    int		snapshotCount;
} lagometer_t;

lagometer_t		lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo ( void ) {
    int			offset;

    offset = cg.time - cg.latestSnapshotTime;
    lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1 ) ] = offset;
    lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo ( snapshot_t *snap ) {
    // dropped packet
    if ( !snap ) {
        lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = -1;
        lagometer.snapshotCount++;
        return;
    }

    // add this snapshot's info
    lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->ping;
    lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->snapFlags;
    lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect ( void ) {
    int			cmdNum;
    usercmd_t	cmd;
    const char		*s;
    int			w;  // bk010215 - FIXME char message[1024];

    // draw the phone jack if we are completely past our buffers
    cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
    trap_GetUserCmd ( cmdNum, &cmd );
    if ( cmd.serverTime <= cg.snap->ps.commandTime
            || cmd.serverTime > cg.time ) {	// special check for map_restart // bk 0102165 - FIXME
        return;
    }

    // also add text in center of screen
    s = "Connection Interrupted"; // bk 010215 - FIXME
    w = CG_DrawStrlen ( s ) * BIGCHAR_WIDTH;
    CG_DrawBigString ( 320 - w/2, 100, s, 1.0F );

    // blink the icon
    if ( ( cg.time >> 9 ) & 1 ) {
        return;
    }

    CG_DrawHudIcon ( HUD_NETGRAPH, qfalse, trap_R_RegisterShader ( "gfx/2d/net.tga" ) );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer ( void ) {
    int		a, i;
    float	v;
    float	ax, ay, aw, ah, mid, range;
    int		color;
    float	vscale;

    if ( cg.demoPlayback )
        return;

    if ( cgs.timeout )
        return;

    if ( cgs.localServer ) {
        CG_DrawDisconnect();
        return;
    }

    if ( cgs.hud[HUD_NETGRAPH].inuse ) {

	    trap_R_SetColor ( NULL );

	    CG_DrawHudIcon ( HUD_NETGRAPH, qtrue, ( qhandle_t ) NULL );
	    //CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	    ax = cgs.hud[HUD_NETGRAPH].xpos;
	    ay = cgs.hud[HUD_NETGRAPH].ypos;
	    aw = cgs.hud[HUD_NETGRAPH].width;
	    ah = cgs.hud[HUD_NETGRAPH].height;
	    CG_AdjustFrom640 ( &ax, &ay, &aw, &ah );

	    color = -1;
	    range = ah / 3;
	    mid = ay + range;

	    vscale = range / MAX_LAGOMETER_RANGE;

	    // draw the frame interpoalte / extrapolate graph
	    for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & ( LAG_SAMPLES - 1 );
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
		    if ( color != 1 ) {
			color = 1;
			trap_R_SetColor ( g_color_table[ColorIndex ( COLOR_YELLOW ) ] );
		    }
		    if ( v > range ) {
			v = range;
		    }
		    trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
		    if ( color != 2 ) {
			color = 2;
			trap_R_SetColor ( g_color_table[ColorIndex ( COLOR_BLUE ) ] );
		    }
		    v = -v;
		    if ( v > range ) {
			v = range;
		    }
		    trap_R_DrawStretchPic ( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	    }

	    // draw the snapshot latency / drop graph
	    range = ah / 2;
	    vscale = range / MAX_LAGOMETER_PING;

	    for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & ( LAG_SAMPLES - 1 );
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
		    if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
			if ( color != 5 ) {
			    color = 5;	// YELLOW for rate delay
			    trap_R_SetColor ( g_color_table[ColorIndex ( COLOR_YELLOW ) ] );
			}
		    } else {
			if ( color != 3 ) {
			    color = 3;
			    trap_R_SetColor ( g_color_table[ColorIndex ( COLOR_GREEN ) ] );
			}
		    }
		    v = v * vscale;
		    if ( v > range ) {
			v = range;
		    }
		    trap_R_DrawStretchPic ( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
		    if ( color != 4 ) {
			color = 4;		// RED for dropped snapshots
			trap_R_SetColor ( g_color_table[ColorIndex ( COLOR_RED ) ] );
		    }
		    trap_R_DrawStretchPic ( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	    }

	    trap_R_SetColor ( NULL );
    
    }

    CG_DrawStringHud ( HUD_NETGRAPHPING, qtrue, ( const char* ) va ( "%ims", cg.snap->ping ) );

    if ( cg_nopredict.integer || cg_synchronousClients.integer ) {
        CG_DrawBigString ( ax, ay, "snc", 1.0 );
    }

    CG_DrawDisconnect();
}



/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
TODO: maybe add hudelement
==============
*/
void CG_CenterPrint ( const char *str, int y, int charWidth ) {
    char	*s;

    if ( !cg_drawCenterprint.integer )
        return;

    Q_strncpyz ( cg.centerPrint, str, sizeof ( cg.centerPrint ) );

    cg.centerPrintTime = cg.time;
    cg.centerPrintY = y;
    cg.centerPrintCharWidth = charWidth;

    // count the number of lines for centering
    cg.centerPrintLines = 1;
    s = cg.centerPrint;
    while ( *s ) {
        if ( *s == '\n' )
            cg.centerPrintLines++;
        s++;
    }
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString ( void ) {
    char	*start;
    int		l;
    int		x, y, w;
#ifdef MISSIONPACK // bk010221 - unused else
    int h;
#endif
    float	*color;

    if ( !cg.centerPrintTime ) {
        return;
    }

    color = CG_FadeColor ( cg.centerPrintTime, 1000 * cg_centertime.value );
    if ( !color ) {
        return;
    }

    trap_R_SetColor ( color );

    start = cg.centerPrint;

    y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

    while ( 1 ) {
        char linebuffer[1024];

        for ( l = 0; l < 50; l++ ) {
            if ( !start[l] || start[l] == '\n' ) {
                break;
            }
            linebuffer[l] = start[l];
        }
        linebuffer[l] = 0;

#ifdef MISSIONPACK
        w = CG_Text_Width ( linebuffer, 0.5, 0 );
        h = CG_Text_Height ( linebuffer, 0.5, 0 );
        x = ( SCREEN_WIDTH - w ) / 2;
        CG_Text_Paint ( x, y + h, 0.5, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
        y += h + 6;
#else
        w = cg.centerPrintCharWidth * CG_DrawStrlen ( linebuffer );

        x = ( SCREEN_WIDTH - w ) / 2;

        CG_DrawStringExt ( x, y, linebuffer, color, qfalse, qtrue,
                           cg.centerPrintCharWidth, ( int ) ( cg.centerPrintCharWidth * 1.5 ), 0 );

        y += cg.centerPrintCharWidth * 1.5;
#endif
        while ( *start && ( *start != '\n' ) ) {
            start++;
        }
        if ( !*start ) {
            break;
        }
        start++;
    }

    trap_R_SetColor ( NULL );
}
/*
===================
CG_DrawFragMessage
===================
*/
static void CG_DrawFragMessage ( void ) {

    if ( !cgs.hud[HUD_FRAGMSG].inuse )  //In CG_DrawStringHud is a check too, and most ppl will activate fragMessages
        return;
    if ( !cg.fragMessageTime )
        return;
    if ( cg.time - cg.fragMessageTime > cgs.hud[HUD_FRAGMSG].time ) {
        cg.fragMessageTime = 0;
        return;
    }

    CG_DrawStringHud ( HUD_FRAGMSG, qtrue, ( const char* ) cg.fragMessage );
}
/*
===================
CG_DrawRankMessage
===================
*/
static void CG_DrawRankMessage ( void ) {
    hudElements_t hudelement = cgs.hud[HUD_RANKMSG];

    if ( !cg.rankMessageTime )
        return;
    if ( !hudelement.inuse )
        return;
    if ( cg.time - cg.rankMessageTime > hudelement.time ) {
        cg.rankMessageTime = 0;
        return;
    }

    CG_DrawStringHud ( HUD_RANKMSG, qtrue, cg.rankMessage );
}


/*
=====================
CG_DrawCenter1FctfString
=====================
*/
static void CG_DrawCenter1FctfString ( void ) {
#ifndef MISSIONPACK
    int		x, y, w;
    float       *color;
    char        *line;
    int status;

    if ( cgs.gametype != GT_1FCTF )
        return;

    status = cgs.flagStatus;

    //Sago: TODO: Find the proper defines instead of hardcoded values.
    switch ( status ) {
    case 2:
        line = va ( "Red has the flag!" );
        color = colorRed;
        break;
    case 3:
        line = va ( "Blue has the flag!" );
        color = colorBlue;
        break;
    case 4:
        line = va ( "Flag dropped!" );
        color = colorWhite;
        break;
    default:
        return;

    };
    y = 100;


    w = cg.centerPrintCharWidth * CG_DrawStrlen ( line );

    x = ( SCREEN_WIDTH - w ) / 2;

    CG_DrawStringExt ( x, y, line, color, qfalse, qtrue,
                       cg.centerPrintCharWidth, ( int ) ( cg.centerPrintCharWidth * 1.5 ), 0 );


#endif
}
#ifndef MISSIONPACK
static int lastDDSec = -100;
#endif
/*
=====================
CG_DrawCenterDDString
=====================
*/
static void CG_DrawCenterDDString ( void ) {
#ifndef MISSIONPACK
    //float       *color;
    char        *line;
    int 		statusA, statusB;
    int sec;


    if ( cgs.gametype != GT_DOUBLE_D )
        return;

    statusA = cgs.redflag;
    statusB = cgs.blueflag;

    if ( ( ( statusB == statusA ) && ( statusA == TEAM_RED ) ) ||
            ( ( statusB == statusA ) && ( statusA == TEAM_BLUE ) ) ) {
    } else
        return; //No team is dominating

    if ( statusA == TEAM_BLUE ) {
        line = va ( "Blue scores in %i", ( cgs.timetaken+10*1000-cg.time ) /1000+1 );
        //color = colorBlue;
    } else if ( statusA == TEAM_RED ) {
        line = va ( "Red scores in %i", ( cgs.timetaken+10*1000-cg.time ) /1000+1 );
        //color = colorRed;
    } else {
        lastDDSec = -100;
        return;
    }

    sec = ( cgs.timetaken+10*1000-cg.time ) /1000+1;
    if ( sec!=lastDDSec ) {
        //A new number is being displayed... play the sound!
        switch ( sec ) {
        case 1:
            trap_S_StartLocalSound ( cgs.media.count1Sound, CHAN_ANNOUNCER );
            break;
        case 2:
            trap_S_StartLocalSound ( cgs.media.count2Sound, CHAN_ANNOUNCER );
            break;
        case 3:
            trap_S_StartLocalSound ( cgs.media.count3Sound, CHAN_ANNOUNCER );
            break;
        default:
            break;
        }
    }
    lastDDSec = sec;

    CG_DrawStringHud ( HUD_COUNTDOWN, qtrue, line );

#endif
}


/*
================================================================================

CROSSHAIR

================================================================================
*/

static void CG_GetHitColor( float *color ) {
    float hitcolor[3];
    float factor = ((float)(cg.lastHitDamage[0])/200.0f)+0.5f;

    if( cg_crosshairHitColorStyle.integer == 1 || cg_crosshairHitColorStyle.integer == 3 ) {
	    factor = 1.0f;
    }
    
    if ( factor > 1 )
        factor = 1.0f;

    if ( strlen(cg_crosshairHitColor.string) == 1 && ColorIndex(*(cg_crosshairHitColor.string)) >= 0 && ColorIndex(*(cg_crosshairHitColor.string)) < MAX_CCODES ) {
        hitcolor[0] = (g_color_table[ColorIndex(*(cg_crosshairHitColor.string))][0]);
        hitcolor[1] = (g_color_table[ColorIndex(*(cg_crosshairHitColor.string))][1]);
        hitcolor[2] = (g_color_table[ColorIndex(*(cg_crosshairHitColor.string))][2]);
    }
    else {
        hitcolor[0] = 1.0f;
        hitcolor[1] = 0.0f;
        hitcolor[2] = 0.0f;
    }

    color[0] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][0]) + factor*(hitcolor[0]-(g_color_table[ColorIndex(*(cg_crosshairColor.string))][0]));
    color[1] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][1]) + factor*(hitcolor[1]-(g_color_table[ColorIndex(*(cg_crosshairColor.string))][1]));
    color[2] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][2]) + factor*(hitcolor[2]-(g_color_table[ColorIndex(*(cg_crosshairColor.string))][2]));
    color[3] = 1.0f;
}


/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair ( void ) {
    float		w, h;
    qhandle_t	hShader;
    float		f;
    float		x, y;
    int			ca = 0; //only to get rid of the warning(not useful)
    int 		currentWeapon;
    vec4_t		color;
    float		xscale = ((float)cg.refdef.width)/640.0f;
    float		yscale = ((float)cg.refdef.height)/480.0f;

    currentWeapon = cg.predictedPlayerState.weapon;

    if ( !cg_drawCrosshair.integer ) {
        return;
    }

    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
        return;
    }

    if ( cg.renderingThirdPerson ) {
        return;
    }

    if ( cg_crosshairHitColorTime.integer > 0 && cg_crosshairHitColorStyle.integer ) {
	if ( cg_crosshairHitColorStyle.integer == 3 || cg_crosshairHitColorStyle.integer == 4 ) {
		if ( cg.time - cg.lastHitTime[0] < (int)( cg_crosshairHitColorTime.integer * ( cg.lastHitDamage[0]/100.0f ) ) ) {
		    CG_GetHitColor(color);
		} else {
		    color[0] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][0]);
		    color[1] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][1]);
		    color[2] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][2]);
		    color[3] = 1.0f;
		}
	  
	} else {
		if ( cg.time - cg.lastHitTime[0] < cg_crosshairHitColorTime.integer ) {
		    CG_GetHitColor(color);
		} else {
		    color[0] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][0]);
		    color[1] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][1]);
		    color[2] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][2]);
		    color[3] = 1.0f;
		}
	}
    }
    // set color based on health
    else if ( cg_crosshairHealth.integer ) {
        CG_ColorForHealth ( color );
    } else {
        color[0] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][0]);
	color[1] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][1]);
	color[2] = (g_color_table[ColorIndex(*(cg_crosshairColor.string))][2]);
	color[3] = 1.0f;
    }

    ca = cgs.crosshair[currentWeapon];
    w = h =cgs.crosshairSize[currentWeapon];

    if ( cgs.screenXScale > cgs.screenYScale ) {
        w = w * cgs.screenYScale / cgs.screenXScale;
    }

    if ( cg_crosshairPulse.integer == 1 ) {
        // pulse the size of the crosshair when picking up items
        f = cg.time - cg.itemPickupBlendTime;
        if ( f > 0 && f < ITEM_BLOB_TIME ) {
            f /= ITEM_BLOB_TIME;
            w *= ( 1 + f );
            h *= ( 1 + f );
        }
    }

    if ( cg_crosshairPulse.integer == 2 ) {
        f = cg.time - cg.lastHitTime[0];
        if ( f > 0 && f < ITEM_BLOB_TIME ) {
            f = f/ ( ITEM_BLOB_TIME );
            f = cg.lastHitDamage[0]* ( 1 - f ) /50;
            w *= ( 1 + f );
            h *= ( 1 + f );
        }
    }

    x = cg_crosshairX.integer;
    y = cg_crosshairY.integer;
    //CG_AdjustFrom640 ( &x, &y, &w, &h );
    x = cg.refdef.x + x*xscale;
    y = cg.refdef.y + y*yscale;
    w *= xscale;
    h *= yscale;

    if ( ca < 0 ) {
        ca = 0;
    }
    hShader = cgs.media.crosshairShader[ ( ca-1 ) % NUM_CROSSHAIRS ];

    if ( !hShader )
        hShader = cgs.media.crosshairShader[ ( ca-1 ) % 10 ];

    trap_R_SetColor ( color );

    trap_R_DrawStretchPic ( x + 0.5 * ( cg.refdef.width - w ),
                            y + 0.5 * ( cg.refdef.height - h ),
                            w, h, 0, 0, 1, 1, hShader );

    trap_R_SetColor ( NULL );
}

/*
=================
CG_DrawCrosshair3D
=================
*/
static void CG_DrawCrosshair3D ( void ) {
    float		w;
    qhandle_t	hShader;
    float		f;
    int			ca;

    trace_t trace;
    vec3_t endpos;
    float stereoSep, zProj, maxdist, xmax;
    char rendererinfos[128];
    refEntity_t ent;

    if ( !cg_drawCrosshair.integer ) {
        return;
    }

    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
        return;
    }

    if ( cg.renderingThirdPerson ) {
        return;
    }

    w = cg_crosshairSize.value;

    // pulse the size of the crosshair when picking up items
    f = cg.time - cg.itemPickupBlendTime;
    if ( f > 0 && f < ITEM_BLOB_TIME ) {
        f /= ITEM_BLOB_TIME;
        w *= ( 1 + f );
    }

    ca = cg_drawCrosshair.integer;
    if ( ca < 0 ) {
        ca = 0;
    }
    hShader = cgs.media.crosshairShader[ ca % NUM_CROSSHAIRS ];

    if ( !hShader )
        hShader = cgs.media.crosshairShader[ ca % 10 ];

    // Use a different method rendering the crosshair so players don't see two of them when
    // focusing their eyes at distant objects with high stereo separation
    // We are going to trace to the next shootable object and place the crosshair in front of it.

    // first get all the important renderer information
    trap_Cvar_VariableStringBuffer ( "r_zProj", rendererinfos, sizeof ( rendererinfos ) );
    zProj = atof ( rendererinfos );
    trap_Cvar_VariableStringBuffer ( "r_stereoSeparation", rendererinfos, sizeof ( rendererinfos ) );
    stereoSep = zProj / atof ( rendererinfos );

    xmax = zProj * tan ( cg.refdef.fov_x * M_PI / 360.0f );

    // let the trace run through until a change in stereo separation of the crosshair becomes less than one pixel.
    maxdist = cgs.glconfig.vidWidth * stereoSep * zProj / ( 2 * xmax );
    VectorMA ( cg.refdef.vieworg, maxdist, cg.refdef.viewaxis[0], endpos );
    CG_Trace ( &trace, cg.refdef.vieworg, NULL, NULL, endpos, 0, MASK_SHOT );

    memset ( &ent, 0, sizeof ( ent ) );
    ent.reType = RT_SPRITE;
    ent.renderfx = RF_DEPTHHACK | RF_CROSSHAIR;

    VectorCopy ( trace.endpos, ent.origin );

    // scale the crosshair so it appears the same size for all distances
    ent.radius = w / 640 * xmax * trace.fraction * maxdist / zProj;
    ent.customShader = hShader;

    trap_R_AddRefEntityToScene ( &ent );
}



/*
=================
CG_ScanForCrosshairEntity
=================
*/
static void CG_ScanForCrosshairEntity ( void ) {
    trace_t		trace;
    vec3_t		start, end;
    int			content;

    VectorCopy ( cg.refdef.vieworg, start );
    VectorMA ( start, 131072, cg.refdef.viewaxis[0], end );

    CG_Trace ( &trace, start, vec3_origin, vec3_origin, end,
               cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY );
    if ( trace.entityNum >= MAX_CLIENTS ) {
        return;
    }

    if ( !g_crosshairNamesFog.integer ) {
        // if the player is in fog, don't show it
        content = CG_PointContents( trace.endpos, 0 );
        if ( content & CONTENTS_FOG ) {
            return;
        }
    }

    // if the player is invisible, don't show it
    if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) ) {
        return;
    }

    // update the fade timer
    cg.crosshairClientNum = trace.entityNum;
    cg.crosshairClientTime = cg.time;
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void CG_DrawCrosshairNames ( void ) {
    float		*color;
    char		*name;
    int		armorcolor, healthcolor;

    if ( !cgs.hud[HUD_TARGETNAME].inuse )
        return;

    if ( !cg_drawCrosshair.integer ) {
        return;
    }

    if ( cg.renderingThirdPerson ) {
        return;
    }


    // scan the known entities to see if the crosshair is sighted on one
    CG_ScanForCrosshairEntity();

    // draw the name of the player being looked at
    color = CG_FadeColor ( cg.crosshairClientTime, cgs.hud[HUD_TARGETNAME].time );
    if ( !color ) {
        trap_R_SetColor ( NULL );
        return;
    }

    name = cgs.clientinfo[ cg.crosshairClientNum ].name;

    CG_DrawStringHud ( HUD_TARGETNAME, qtrue, name );

    trap_R_SetColor ( NULL );

    if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_RED && cg.snap->ps.persistant[PERS_TEAM] != TEAM_BLUE )
        return;

    if ( cg.snap->ps.persistant[PERS_TEAM] != cgs.clientinfo[ cg.crosshairClientNum ].team )
        return;

    if ( ( cgs.hud[HUD_TARGETSTATUS].inuse ) && ( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR ) ) {
        if ( cgs.clientinfo[ cg.crosshairClientNum ].health >= 100 )
            healthcolor = 2;
        else if ( cgs.clientinfo[ cg.crosshairClientNum ].health >= 50 )
            healthcolor = 3;
        else
            healthcolor = 1;

        if ( cgs.clientinfo[ cg.crosshairClientNum ].armor >= 100 )
            armorcolor = 2;
        else if ( cgs.clientinfo[ cg.crosshairClientNum ].armor >= 50 )
            armorcolor = 3;
        else
            armorcolor = 1;



        CG_DrawStringHud ( HUD_TARGETSTATUS, qfalse, va ( "(^%i%i^7|^%i%i^7)", healthcolor, cgs.clientinfo[ cg.crosshairClientNum ].health, armorcolor, cgs.clientinfo[ cg.crosshairClientNum ].armor ) );
    }
}


//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator ( void ) {
    CG_DrawBigString ( 320 - 9 * 8, 440, "SPECTATOR", 1.0F );
    if ( cgs.gametype == GT_TOURNAMENT ) {
        CG_DrawBigString ( 320 - 15 * 8, 460, "waiting to play", 1.0F );
    } else if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
        CG_DrawBigString ( 320 - 39 * 8, 460, "press ESC and use the JOIN menu to play", 1.0F );
    }
}

#define VOTEPOS_X 0
#define VOTEPOS_Y 58

/*
=================
CG_DrawVote
=================
*/
static void CG_DrawVote ( void ) {
    char	*s;
    int		sec;

    if ( !cgs.voteTime ) {
        return;
    }

    // play a talk beep whenever it is modified
    if ( cgs.voteModified ) {
        cgs.voteModified = qfalse;
        trap_S_StartLocalSound ( cgs.media.talkSound, CHAN_LOCAL_SOUND );
    }

    sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
    if ( sec < 0 ) {			//We could add an inline function for that
        sec = 0;
    }

    s = va ( "VOTE(%i):%s yes:%i no:%i", sec, cgs.voteString, cgs.voteYes, cgs.voteNo );
    CG_DrawStringHud ( HUD_VOTEMSG, qtrue, s );
}

/*
=================
CG_DrawTeamVote
=================
*/
static void CG_DrawTeamVote ( void ) {
    char	*s;
    int		sec, cs_offset;

    if ( cgs.clientinfo[cg.clientNum].team == TEAM_RED )
        cs_offset = 0;
    else if ( cgs.clientinfo[cg.clientNum].team == TEAM_BLUE )
        cs_offset = 1;
    else
        return;

    if ( !cgs.teamVoteTime[cs_offset] ) {
        return;
    }

    // play a talk beep whenever it is modified
    if ( cgs.teamVoteModified[cs_offset] ) {
        cgs.teamVoteModified[cs_offset] = qfalse;
        trap_S_StartLocalSound ( cgs.media.talkSound, CHAN_LOCAL_SOUND );
    }

    sec = ( VOTE_TIME - ( cg.time - cgs.teamVoteTime[cs_offset] ) ) / 1000;
    if ( sec < 0 ) {
        sec = 0;
    }
    s = va ( "TEAMVOTE(%i):%s yes:%i no:%i", sec, cgs.teamVoteString[cs_offset],
             cgs.teamVoteYes[cs_offset], cgs.teamVoteNo[cs_offset] );
    CG_DrawSmallString ( 0, 90, s, 1.0F );
}


static qboolean CG_DrawScoreboard ( void ) {
#ifdef MISSIONPACK
    static qboolean firstTime = qtrue;

    if ( menuScoreboard ) {
        menuScoreboard->window.flags &= ~WINDOW_FORCED;
    }
    if ( cg_paused.integer ) {
        cg.deferredPlayerLoading = 0;
        firstTime = qtrue;
        return qfalse;
    }

    // should never happen in Team Arena
    if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
        cg.deferredPlayerLoading = 0;
        firstTime = qtrue;
        return qfalse;
    }

    // don't draw scoreboard during death while warmup up
    if ( cg.warmup && !cg.showScores ) {
        return qfalse;
    }

    if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD || cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
    } else {
	if ( !CG_FadeColor( cg.scoreFadeTime, FADE_TIME ) ) {
            // next time scoreboard comes up, don't print killer
            cg.deferredPlayerLoading = 0;
            cg.killerName[0] = 0;
            firstTime = qtrue;
            return qfalse;
        }
    }


    if ( menuScoreboard == NULL ) {
        if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
            menuScoreboard = Menus_FindByName ( "teamscore_menu" );
        } else {
            menuScoreboard = Menus_FindByName ( "score_menu" );
        }
    }

    if ( menuScoreboard ) {
        if ( firstTime ) {
            CG_SetScoreSelection ( menuScoreboard );
            firstTime = qfalse;
        }
        Menu_Paint ( menuScoreboard, qtrue );
    }

    // load any models that have been deferred
    if ( ++cg.deferredPlayerLoading > 10 ) {
        CG_LoadDeferredPlayers();
    }

    return qtrue;
#else
    if ( cgs.gametype == GT_TOURNAMENT ) {
        if ( cg_oldScoreboard.integer )
            return CG_DrawOldScoreboardOld();
        return CG_DrawOldTourneyScoreboard();
    } else {
        if ( cg_oldScoreboard.integer )
            return CG_DrawOldScoreboardOld();
        return CG_DrawOldScoreboard();
    }
#endif
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission ( void ) {

    if ( cgs.gametype == GT_SINGLE_PLAYER ) {
        CG_DrawCenterString();
        return;
    }
    cg.scoreFadeTime = cg.time;
    cg.scoreBoardShowing = CG_DrawScoreboard();
}

/*
=================
CG_DrawFollow
TODO: Add multiview
=================
*/
static qboolean CG_DrawFollow ( void ) {
    const char	*name;

    if ( ! ( cg.snap->ps.pm_flags & PMF_FOLLOW ) ) {
        return qfalse;
    }

    name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

    CG_DrawStringHud ( HUD_FOLLOW, qtrue, va ( "following %s", name ) );

    return qtrue;
}



/*
=================
CG_DrawAmmoWarning
=================
*/
static void CG_DrawAmmoWarning ( void ) {
    //const char	*s;

    //Don't report in instant gib same with RA
    if ( cgs.nopickup )
        return;

    if ( !cg.lowAmmoWarning ) {
        return;
    }

    if ( cg.lowAmmoWarning == 2 ) {
        //	s = "OUT OF AMMO";
        CG_DrawStringHud ( HUD_AMMOWARNING, qtrue, "OUT OF AMMO" );
    } else {
        //	s = "LOW AMMO WARNING";
        CG_DrawStringHud ( HUD_AMMOWARNING, qtrue, "LOW AMMO WARNING" );
    }

    //CG_DrawStringHud ( HUD_AMMOWARNING, qtrue, s );

}


#ifdef MISSIONPACK
/*
=================
CG_DrawProxWarning
TODO: not really used, we need to remove all the crap weapons
=================
*/
static void CG_DrawProxWarning ( void ) {
    char s [32];
    int			w;
    static int proxTime;
    int proxTick;

    if ( ! ( cg.snap->ps.eFlags & EF_TICKING ) ) {
        proxTime = 0;
        return;
    }

    if ( proxTime == 0 ) {
        proxTime = cg.time;
    }

    proxTick = 10 - ((cg.time - proxTime) / 1000);

    if ( proxTick > 0 && proxTick <= 5 ) {
        Com_sprintf ( s, sizeof ( s ), "INTERNAL COMBUSTION IN: %i", proxTick );
    } else {
        Com_sprintf ( s, sizeof ( s ), "YOU HAVE BEEN MINED" );
    }

    w = CG_DrawStrlen ( s ) * BIGCHAR_WIDTH;
    CG_DrawBigStringColor ( 320 - w / 2, 64 + BIGCHAR_HEIGHT, s, g_color_table[ColorIndex ( COLOR_RED ) ] );
}
#endif


/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup ( void ) {
    int			sec;
    int			i;
    clientInfo_t	*ci1, *ci2;
    const char	*s;

    sec = cg.warmup;
    if ( !sec ) {
        return;
    }

    if ( sec < 0 ) {

        s = "Warmup";
        CG_DrawStringHud ( HUD_WARMUP, qtrue, s );
        cg.warmupCount = 0;
        return;
    }

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

        if ( ci1 && ci2 ) {
            s = va ( "%s ^7vs %s", ci1->name, ci2->name );
            CG_DrawStringHud ( HUD_GAMETYPE, qtrue, s );
        }
    } else {
        if ( cgs.gametype == GT_FFA ) {
            s = "Free For All";
        } else if ( cgs.gametype == GT_TEAM ) {
            s = "Team Deathmatch";
        } else if ( cgs.gametype == GT_CTF ) {
            s = "Capture the Flag";
        } else if ( cgs.gametype == GT_ELIMINATION ) {
            s = "Elimination";
        } else if ( cgs.gametype == GT_CTF_ELIMINATION ) {
            s = "CTF Elimination";
        } else if ( cgs.gametype == GT_LMS ) {
            s = "Last Man Standing";
        } else if ( cgs.gametype == GT_DOUBLE_D ) {
            s = "Double Domination";
        } else if ( cgs.gametype == GT_1FCTF ) {
            s = "One Flag CTF";
        } else if ( cgs.gametype == GT_OBELISK ) {
            s = "Overload";
        } else if ( cgs.gametype == GT_HARVESTER ) {
            s = "Harvester";
        } else if ( cgs.gametype == GT_DOMINATION ) {
            s = "Domination";
        } else {
            s = "";
        }
        CG_DrawStringHud ( HUD_GAMETYPE, qtrue, s );
    }

    sec = ( sec - cg.time ) / 1000;
    if ( sec < 0 ) {
        cg.warmup = 0;
        sec = 0;
    }
    s = va ( "Starts in: %i", sec + 1 );
    if ( sec != cg.warmupCount ) {
        cg.warmupCount = sec;
        switch ( sec ) {
        case 0:
            trap_S_StartLocalSound ( cgs.media.count1Sound, CHAN_ANNOUNCER );
            break;
        case 1:
            trap_S_StartLocalSound ( cgs.media.count2Sound, CHAN_ANNOUNCER );
            break;
        case 2:
            trap_S_StartLocalSound ( cgs.media.count3Sound, CHAN_ANNOUNCER );
            break;
        default:
            break;
        }
    }

    CG_DrawStringHud ( HUD_COUNTDOWN, qtrue, s );
//#endif
}

//==================================================================================
#ifdef MISSIONPACK
/*
=================
CG_DrawTimedMenus
=================
*/
void CG_DrawTimedMenus ( void ) {
    if ( cg.voiceTime ) {
        int t = cg.time - cg.voiceTime;
        if ( t > 2500 ) {
            Menus_CloseByName ( "voiceMenu" );
            trap_Cvar_Set ( "cl_conXOffset", "0" );
            cg.voiceTime = 0;
        }
    }
}
#endif

#define PICKUPITEM_SIZE ICON_SIZE/4
#define PICKUPITEM_SPACE 4
#define PICKUPITEM_Y 420

/*
===================
CG_DrawPickupItem
===================
*/
//#ifndef MISSIONPACK
static void CG_DrawPickupItem ( void ) {
    int		value;
    //char		*s;
    int		min, ten, second, msecs;
    //int x;

    //x = 0;

    if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) {
        return;
    }

    value = cg.itemPickup;
    if ( value ) {
        if ( ( cg.time - cg.itemPickupBlendTime ) < 3000 ) {
            CG_RegisterItemVisuals ( value );
            if ( cg_drawItemPickups.integer & 1 ) {
                CG_DrawHudIcon ( HUD_ITEMPICKUPICON, qfalse, cg_items[ value ].icon );
            }
            if ( cg_drawItemPickups.integer & 2 ) {
                CG_DrawStringHud ( HUD_ITEMPICKUPNAME, qtrue, bg_itemlist[ value ].pickup_name );
            }
            if ( cg_drawItemPickups.integer & 4 ) {

                msecs = cg.itemPickupBlendTime - cgs.levelStartTime;
                second = msecs/1000;
                min = second / 60;
                second -= min * 60;
                ten = second / 10;
                second -= ten * 10;

                //s = va ( "%i:%i%i", min, ten, second );

                CG_DrawStringHud ( HUD_ITEMPICKUPTIME, qtrue, va ( "%i:%i%i", min, ten, second ) );
            }
        }
    }
    return;
}
/*
===================
CG_Predecorate
===================
*/
static void CG_Predecorate ( void ) {
    int i;
    //hudElements_t hudelement;
    for ( i = 0; i < 8; i++ ) {
        //hudelement = cgs.hud[HUD_PREDECORATE1 + i];
        if ( !cgs.hud[HUD_PREDECORATE1+i].inuse )
            continue;
        CG_DrawHudIcon ( HUD_PREDECORATE1+i, qtrue, ( qhandle_t ) NULL );
        CG_DrawStringHud ( HUD_PREDECORATE1+i, qtrue, "" );
    }
}
/*
===================
CG_Postdecorate
===================
*/
static void CG_Postdecorate ( void ) {
    int i;
    //hudElements_t hudelement;
    for ( i = 0; i < 8; i++ ) {
        //hudelement = cgs.hud[HUD_POSTDECORATE1 + i];
        if ( !cgs.hud[HUD_POSTDECORATE1 + i].inuse )
            continue;
        CG_DrawHudIcon ( HUD_POSTDECORATE1+i, qtrue, ( qhandle_t ) NULL );
        CG_DrawStringHud ( HUD_POSTDECORATE1+i, qtrue, "" );
    }
}

/*
=================
CG_Draw2D
=================
*/
void CG_DrawMVDhud ( stereoFrame_t stereoFrame ) {

    if ( cg_draw2D.integer == 0 ) {
        return;
    }


    if ( stereoFrame == STEREO_CENTER )
        CG_DrawCrosshair();		//TODO: thirdPersonView removes the CH from mv

    if ( !cg.showScores ) {
        CG_Predecorate();
        CG_DrawStatusBar();
        //CG_DrawPowerups(); //TODO
        CG_Postdecorate();
    }
}

/*
=================
CG_Draw2D
=================
*/
void CG_Draw2D ( stereoFrame_t stereoFrame ) {
#ifdef MISSIONPACK
    if ( cgs.orderPending && cg.time > cgs.orderTime ) {
        CG_CheckOrderPending();
    }
#endif
    // if we are taking a levelshot for the menu, don't draw anything
    if ( cg.levelShot ) {
        return;
    }

    if ( cg_draw2D.integer == 0 ) {
        return;
    }

    if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
        CG_DrawIntermission();
        CG_DrawChat ( qtrue );
	
	if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
            CG_DrawTeamInfo();
        }
        
        return;
    }

    /*
    	if (cg.cameraMode) {
    		return;
    	}
    */
    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR /*|| cg.snap->ps.pm_type == PM_SPECTATOR*/ ) {
        CG_DrawSpectator();

        if ( stereoFrame == STEREO_CENTER )
            CG_DrawCrosshair();

        CG_DrawCrosshairNames();

        if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
#ifndef MISSIONPACK
            CG_DrawTeamInfo();
#endif
        }

    } else {
        // don't draw any status if dead or the scoreboard is being explicitly shown
        if ( !cg.showScores && cg.snap->ps.stats[STAT_HEALTH] > 0 ) {


            CG_Predecorate();
            CG_DrawStatusBar();

            CG_DrawAmmoWarning();

            if ( stereoFrame == STEREO_CENTER )
                CG_DrawCrosshair();
            CG_DrawCrosshairNames();
            CG_DrawWeaponSelect();


            CG_DrawHoldableItem();
            CG_DrawPersistantPowerup();

            CG_DrawReward();
            CG_DrawPickupItem();

        }

        if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 ) {
            CG_DrawTeamInfo();
        }
    }

    CG_DrawDeathNotice();
    CG_DrawChat ( qfalse );
    CG_DrawConsole();

    CG_DrawVote();
    CG_DrawTeamVote();

    //CG_DrawUpperRight();

    if ( stereoFrame == STEREO_CENTER || stereoFrame == STEREO_RIGHT ) {
        CG_DrawFPS();
    }

    CG_DrawLivingCount();
    CG_DrawTimer();
    
    if ( cgs.gametype==GT_ELIMINATION || cgs.gametype == GT_CTF_ELIMINATION || cgs.gametype==GT_LMS ) {
            CG_DrawEliminationTimer();
        }
    CG_DrawWarmup();
    CG_DrawTimeout();
    
    // don't draw center string if scoreboard is up
    cg.scoreBoardShowing = CG_DrawScoreboard();
    if ( !cg.scoreBoardShowing ) {
        

        CG_DrawLagometer();
        
        CG_DrawCenterDDString();
        CG_DrawCenter1FctfString();
        CG_DrawCenterString();
        CG_DrawFragMessage();
        CG_DrawRankMessage();
        CG_DrawScores();
        CG_DrawPowerups();

        CG_DrawAttacker();

        if ( cg_drawSpeed.integer ) {
            CG_DrawSpeedMeter();
        }
        if ( cg_drawAccel.integer ) {
            CG_DrawAccelMeter();
        }

        //if ( !CG_DrawFollow() ) {
	    CG_DrawFollow();
            
        //}

        if ( cgs.gametype >= GT_TEAM && cgs.ffa_gt!=1 && cg_drawTeamOverlay.integer ) {
            CG_DrawTeamOverlay ( qtrue, qfalse );
        }

        if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR )
            CG_DrawRespawnTimers();

        if ( cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR )
            CG_DrawReady();
    }

    cg.accBoardShowing = CG_DrawAccboard();
    CG_Postdecorate();
}


/*static void CG_DrawTourneyScoreboard ( void ) {
#ifdef MISSIONPACK
#else
    if ( cg_oldScoreboard.integer )
        CG_DrawOldTourneyScoreboardOld();
    else
        CG_DrawOldTourneyScoreboard();
#endif
}*/

float CG_OriginToScreenpos0( vec3_t origin ) {
	float X1org=-160.0f, Y1org=16.0f, X2org=672.0f, Y2org=-1368.0f, X3org=-96.0f, Y3org=192.0f, X1=396.0f, X2=228.0f, X3=383.0f;
	float b;
	float a;
	float c;
	
	b = ( (X1-X3)/(X1org-X3org)-(X2-X3)/(X2org-X3org) )/( (Y1org-Y3org)/(X1org-X3org)-(Y2org-Y3org)/(X2org-X3org));
	a = ((X1-X3)/(X1org-X3org))-b*((Y1org-Y3org)/(X1org-X3org));
	c = X3 - b*Y3org - a*X3org;
 	
	return a*origin[0]+b*origin[1]+c;
}

float CG_OriginToScreenpos1( vec3_t origin ) {
	float X1org=-160.0f, Y1org=16.0f, X2org=672.0f, Y2org=-1368.0f, X3org=-96.0f, Y3org=192.0f, X1=292.0f, X2=16.0f, X3=328.0f;
	float b;
	float a;
	float c;
	
	b = ( (X1-X3)/(X1org-X3org)-(X2-X3)/(X2org-X3org) )/( (Y1org-Y3org)/(X1org-X3org)-(Y2org-Y3org)/(X2org-X3org));
	a = ((X1-X3)/(X1org-X3org))-b*((Y1org-Y3org)/(X1org-X3org));
	c = X3 - b*Y3org - a*X3org;
 	
	return a*origin[0]+b*origin[1]+c;
}

/*
=====================
CG_DrawOverviewEntities

Perform all drawing needed to completely fill the screen
=====================
*/
static void CG_DrawOverviewEntities ( void ) {
	int num,clientnum;
	centity_t *cent;
	for ( num = 0 ; num < cg.nextSnap->numEntities ; num++ ) {
		cent = &cg_entities[ cg.nextSnap->entities[ num ].number ];
		if( cent->currentState.eType == ET_ITEM ){
			if ( cent->currentState.modelindex >= bg_numItems ) {
				continue;
			}
			// if set to invisible, skip
			if ( !cent->currentState.modelindex ||   cent->currentState.eFlags & EF_NODRAW  ) {
				continue;
			}
			CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-5, CG_OriginToScreenpos1(cent->lerpOrigin)-5, 10, 10, cg_items[cent->currentState.modelindex].icon);
		} else if( cent->currentState.eType == ET_PLAYER ) {
			trap_R_SetColor(colorGreen);
			CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-5, CG_OriginToScreenpos1(cent->lerpOrigin)-5, 10, 10, cgs.media.playericon);
			trap_R_SetColor(NULL);
		} else if( cent->currentState.eType == ET_MISSILE ) {
			if( cent->currentState.weapon == WP_GRENADE_LAUNCHER ) {
				CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-2.5f, CG_OriginToScreenpos1(cent->lerpOrigin)-2.5f, 5, 5, cgs.media.grenadeMapoverview);
			} else if( cent->currentState.weapon == WP_ROCKET_LAUNCHER ) {
				CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-2.5f, CG_OriginToScreenpos1(cent->lerpOrigin)-2.5f, 5, 5, cgs.media.rocketMapoverview);
			} else if( cent->currentState.weapon == WP_PLASMAGUN ) {
				CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-2.5f, CG_OriginToScreenpos1(cent->lerpOrigin)-2.5f, 5, 5, cgs.media.plasmaMapoverview);
			} else if( cent->currentState.weapon == WP_BFG ) {
				CG_DrawPic(CG_OriginToScreenpos0(cent->lerpOrigin)-2.5f, CG_OriginToScreenpos1(cent->lerpOrigin)-2.5f, 5, 5, cgs.media.bfgMapoverview);
			}
		} else if( cent->currentState.eType > ET_EVENTS ) {
			switch( cent->currentState.event & ~EV_EVENT_BITS ){
				case EV_RAILTRAIL:
					CG_DrawLine( CG_OriginToScreenpos0(cent->currentState.origin2), CG_OriginToScreenpos1(cent->currentState.origin2), 
						     CG_OriginToScreenpos0(cent->currentState.pos.trBase), CG_OriginToScreenpos1(cent->currentState.pos.trBase), colorGreen );
					break;
				case EV_BULLET_HIT_FLESH:
				case EV_BULLET_HIT_WALL:
					clientnum = cent->currentState.clientNum;
					CG_DrawLine( CG_OriginToScreenpos0(cg_entities[clientnum].lerpOrigin), CG_OriginToScreenpos1(cg_entities[clientnum].lerpOrigin), 
						     CG_OriginToScreenpos0(cent->currentState.pos.trBase), CG_OriginToScreenpos1(cent->currentState.pos.trBase), colorYellow );
					break;
					
			}
		}
		
	}
	if( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ){
		CG_DrawPic(CG_OriginToScreenpos0(cg.snap->ps.origin)-5, CG_OriginToScreenpos1(cg.snap->ps.origin)-5, 10, 10, cgs.media.playericon);
	}
}
/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive ( stereoFrame_t stereoView, qboolean draw2d ) {
    //int i;
    qhandle_t picture;
    if ( !cg.snap ) {
        CG_DrawInformation();
        return;
    }

    // optionally draw the tournement scoreboard instead
    if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR &&
            ( cg.snap->ps.pm_flags & PMF_SCOREBOARD ) ) {
        if ( cg.scoresRequestTime + 5000 < cg.time ) {
            cg.scoresRequestTime = cg.time;
            trap_SendClientCommand( "score" );
        }

        CG_FillRect(0, 0, 640, 480, colorBlack);
        cg.showScores = qtrue;
        CG_DrawScoreboard();
        CG_DrawChat ( qtrue );
        CG_DrawTeamInfo();
        return;
    }
    
    if( cg_mapoverview.integer /*&& cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR*/ ){
	    picture = trap_R_RegisterShader("mapoverview/ztn3tourney1.tga");
	    CG_DrawPic(0,0,640,480,picture);
	    //CG_FillRect(0, 0, 640, 480, colorBlack);
	    CG_DrawOverviewEntities();
	    return;
    }

    if ( stereoView != STEREO_CENTER )
        CG_DrawCrosshair3D();

    // draw 3D view
    trap_R_RenderScene ( &cg.refdef );

    /*if( draw2d )
          CG_Draw2D ( stereoView );*/
    // draw status bar and other floating elements

}



