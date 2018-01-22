//================================================================
// Smarine
//================================================================

models/players/smarine/pm_u_torsob-detail
{
	nopicmip
	{
		map models/players/smarine/pm_u_torsob-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/smarine/pm_u_torsob-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/smarine/pm_l_legsb-detail
{
	nopicmip
	{
		map models/players/smarine/pm_l_legsb-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/smarine/pm_l_legsb-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/smarine/pm_h_headb-detail
{
	nopicmip
	{
		map models/players/smarine/pm_h_headb-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/smarine/pm_h_headb-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/smarine/pm_u_torsob
{
	nopicmip
	{
		map models/players/smarine/pm_u_torsob.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}

models/players/smarine/pm_l_legsb
{
	nopicmip
	{
		map models/players/smarine/pm_l_legsb.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}

models/players/smarine/pm_h_headb
{
	nopicmip
	{
		map models/players/smarine/pm_h_headb.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}

//================================================================
// Major
//================================================================

models/players/major/head_pm-detail
{
	nopicmip
	{
		map models/players/major/head_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{	
		map models/players/major/head_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/major/lower_pm-detail
{
	nopicmip
	{
		map models/players/major/lower_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/major/lower_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/major/torso_pm-detail
{
	nopicmip
	{
		map models/players/major/torso_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/major/torso_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
}

models/players/major/head_pm
{
	nopicmip
	{
		map models/players/major/head_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
// 	{	
// 		map models/players/major/head_pm.tga
// 		blendFunc GL_ONE GL_ZERO
// 		alphaFunc LT128
// 		rgbGen identity
// 	}
}

models/players/major/lower_pm
{
	nopicmip
	{
		map models/players/major/lower_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
// 	{
// 		map models/players/major/lower_pm.tga
// 		blendFunc GL_ONE GL_ZERO
// 		alphaFunc LT128
// 		rgbGen identity
// 	}
}

models/players/major/torso_pm
{
	nopicmip
	{
		map models/players/major/torso_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
// 	{
// 		map models/players/major/torso_pm.tga
// 		blendFunc GL_ONE GL_ZERO
// 		alphaFunc LT128
// 		rgbGen identity
// 	}
}

//================================================================
// Sarge
//================================================================

models/players/sarge/skin_pm
{
	nopicmip
	{
		map models/players/sarge/skin_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
		alphaGen entity
	}
// 	{
// 		map models/players/sarge/skin_pm.tga
// 		blendFunc GL_ONE GL_ZERO
// 		alphaFunc LT128
// 		rgbGen identity
// 		alphaGen identity
// 	}
}

models/players/sarge/skin_pm-detail
{
	nopicmip
	{
		map models/players/sarge/skin_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
		alphaGen entity
	}
	{
		map models/players/sarge/skin_pm-detail.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
		alphaGen identity
	}
}

//================================================================
// Skelebot
//================================================================

models/players/skelebot/skin1_pm-detail
{
	nopicmip
	{
		map models/players/skelebot/skin1_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/skelebot/skin1_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
	{
		map textures/effects/tinfx3.jpg
		blendfunc add
		tcGen environment
		rgbGen lightingDiffuse
	}
}

models/players/skelebot/skin2_pm-detail
{
	nopicmip
	{
		map models/players/skelebot/skin2_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
	{
		map models/players/skelebot/skin2_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc LT128
		rgbGen identity
	}
	{

		map textures/effects/tinfx3.jpg
		blendfunc add
		tcGen environment
		rgbGen lightingDiffuse
	}
}

models/players/skelebot/skin1_pm
{
	nopicmip
	{
		map models/players/skelebot/skin1_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}

models/players/skelebot/skin2_pm
{
	nopicmip
	{
		map models/players/skelebot/skin2_pm.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}

//================================================================
// ALL
//================================================================

models/players/flat
{
	nopicmip
	{
		map models/players/flat.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		rgbGen entity
	}
}