models/mapobjects/spotlamp/beam
//last edited by **HD: nopicmip
{
    surfaceparm trans	
    surfaceparm nomarks	
    surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	nopicmip
	
	{
		map models/mapobjects/spotlamp/beam.jpg
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
    }
}

models/mapobjects/flag/banner_strgg
// last edited by **HD
{
    qer_editorimage models/mapobjects/flag/banner_strgg.tga
    surfaceparm nomarks
    cull none
	tessSize 64
	deformVertexes wave 128 sin 0 2 0 .6
	surfaceparm alphashadow
	{
		map models/mapobjects/flag/banner_strgg.tga
		blendFunc GL_ONE GL_ZERO
        alphaFunc GE128
		rgbGen identity
	}
	{
		map models/mapobjects/flag/banner_strgg.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

