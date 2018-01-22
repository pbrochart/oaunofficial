textures/base_floor/proto_rustygrate
//last edited by **HD :surfaceparm trans
{
	surfaceparm metalsteps
	surfaceparm nodamage
	surfaceparm trans
	cull none
	nopicmip
	{
		map textures/base_floor/proto_rustygrate.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/proto_rustygrate2
//last edited by **HD : surfaceparm trans
{
	surfaceparm metalsteps
	surfaceparm nodamage
	surfaceparm trans
	cull none
	nopicmip
	{
		map textures/base_floor/proto_rustygrate2.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/rusty_pentragrate
//last edited by **HD : surfaceparm trans
{
	surfaceparm metalsteps
	surfaceparm nodamage
	surfaceparm trans
	cull none
	nopicmip
	{
		map textures/base_floor/rusty_pentragrate.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/proto_grate
//last edited by **HD : surfaceparm trans
{
	surfaceparm metalsteps
	surfaceparm nodamage
	surfaceparm trans
	cull none
	nopicmip
	{
		map textures/base_floor/proto_grate.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GE128
		depthWrite
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/proto_grate3
//last edited by **HD : surfaceparm trans
{
	surfaceparm metalsteps
	surfaceparm nodamage
	surfaceparm trans
	cull none
	nopicmip
	{
		map textures/base_floor/proto_grate3.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GE128
		depthWrite
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
		depthFunc equal
	}
}
