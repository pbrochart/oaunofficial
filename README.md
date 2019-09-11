This is my contribution to Aftershock Mod for Openarena

Revision 305: Can't use plasmaSpark because Openarena
has a shader with the same name, so renamed it to plasmaSparkAs.
Add g_crosshairNamesFog (disabled by default): Display names even
in fog (necessary for maps like asylum, quarantine...).
Revision 306: Fix minor bugs and improve performances.
Revision 307: Add cg_damagePlums (enabled by default): Display damage
indicator (works only if server has g_damagePlums enabled).
Revision 308: With cg_damagePlums, sum the damages for the shotgun.
Revision 309: Fix all compiler warnings, mapcycle and getmapcycle.
Use damage without armour for cg_damagePlums and some minor bug fixes.
Revision 310: Display damage indicator thru the walls.
Fix duplicate damages with shotgun when they are multiple targets.
Add cg_damagePlum: Controls which weapons will draw damage plumes.
Default is all: "/g/mg/sg/gl/rl/lg/rg/pg/bfg/cg/ng/pl/".
Revision 311: Added new features for cg_drawGun (4,5,6).
Same as 1,2,3 but the weapons are transparent.
Revision 312: Some bug fixes from ioquake3.
Revision 313: Add double jump, air control and extra
damage knockback from CPM physics to g_aftershockPhysic.
Revision 314: Add g_promode and remove g_aftershockPhysic.
Revision 315: Some bug fixes from ioquake3.
Revision 316: Add cg_waterWarp (enabled by default): Control the
wavy undulating visual effect when the player is under water.
A little improvement of the CTF scoreboard.
Backport some bug fixes from ioquake3.
Revision 317: Optimization of bot's parsing from ioquake3.
Revision 318: Some bug fixes from ioquake3.
Revision 319: Add missing commit and some bug fixes from ioquake3.
Revision 320: Fix hit accuracy stats for lg/sg and predicting entity
origin on rotating mover from ioquake3.
Revision 321: Add pmove_accurate (disabled by default):
It's an alternative to pmove_fixed which had drawbacks
(choppy movement). Setting this to 125 will make all players jump
as high and far as if they had 125 fps even when their
actual fps is lower or unstable (but require more bandwidth).
It's frame-rate independent physics and be fair for everyone.
BoxOnPlaneSide rewrite from ioquake3 (by Diego de Estrada).
Revision 322: Minor enhancements on CTF and small bug fixes from ioquake3.
Revision 323: Fix broken AI by moving green armour at the end of list
and remove unused-but-set gcc warnings.
