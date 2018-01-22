textures/sfx/largerblock3b3_pent
//last edited by **HD
{
	qer_editorimage textures/sfx/largerblock3b3_pent.tga
	{
		map textures/sfx/largerblock3b3_pent.tga
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendfunc filter
	}
	{
		map textures/sfx/pentagramfloor_red_glow.tga
		blendfunc add
	}
}

popshaft
{
	cull disable
	{
		map models/powerups/telezap.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.8 
		tcMod scroll 0.6 0
	}
}

popflare
{
	deformVertexes autosprite
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		tcMod rotate 766
		tcMod stretch inversesawtooth 0 1 0 1.8 
	}
}