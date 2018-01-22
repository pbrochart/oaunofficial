textures/sfx/border11c
{
// last edited by **HD
	surfaceparm nomarks
	q3map_surfacelight 450
	q3map_lightimage textures/base_trim/border11c_pulse1.tga
	qer_editorimage textures/base_trim/border11c.tga
	{
		map textures/base_trim/border11c.tga
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendfunc gl_dst_color gl_zero
	}
	{
		map textures/base_trim/border11c_pulse1b.tga
		blendfunc add
		tcmod scale 0.02 1
		rgbGen wave sin 1 0.5 0 3
		tcmod scroll 0.5 0
	}
	{
		map textures/base_trim/border11c_light.tga
		blendfunc add
		rgbgen wave sin 0.5 .1 0.25 6
	}
}

textures/base_trim/border11light 
{
//last edited by **HD
	q3map_surfacelight 450
	q3map_lightimage textures/base_trim/border11light.glow.tga
	qer_editorimage textures/base_trim/border11light.tga
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border11light
		rgbGen identity
		blendFunc filter
	}
	{
		map textures/base_trim/border11light.glow.tga
		blendFunc add
		rgbGen wave sin .5 .1 0.25 6
	}
}