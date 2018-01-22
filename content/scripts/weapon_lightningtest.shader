models/weapons2/lightning/w_lightning
{
	{
		map models/weapons2/lightning/w_lightning.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

lightningflash
{
	cull none
	{
		clampmap models/weapons2/lightning/f_lightning.jpg
		blendFunc GL_ONE GL_ONE
                rgbgen wave triangle .8 2 0 9
                tcMod rotate 360
	}	
        {
		clampmap models/weapons2/lightning/f_lightning.jpg
		blendFunc GL_ONE GL_ONE
                rgbgen wave triangle 1 1.4 0 9.5
                tcMod rotate -202
	}	
	
}
