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
