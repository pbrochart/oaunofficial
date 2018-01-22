models/ammo/grenadeSkin
{
	{
		map models/ammo/grenadeskin.tga
		rgbGen lightingDiffuse

	}
	{
		map models/weapons2/grenadel/chromey.tga
		rgbGen lightingDiffuse
		tcGen environment 
		blendfunc add
	}
	{
		map models/ammo/grenadeskin.tga
		rgbGen lightingDiffuse
		blendfunc blend
	}
}

models/ammo/grenadeColor
{
	nopicmip
	{
		map models/ammo/grenadeskin.tga
		rgbGen lightingDiffuse

	}
	{
		map models/weapons2/grenadel/chromey.tga
		rgbGen lightingDiffuse
		tcGen environment 
		blendfunc add
	}
	{
		map models/ammo/grenadeskin.tga
		rgbGen lightingDiffuse
		blendfunc blend
	}
	{
		map models/ammo/grenadeskinColor.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/ammo/grenadeskinColor.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}