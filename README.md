# Compresseur-universel

On sait faire:
Compression par palette
Changement espace couleur
Reechantillonage
Codage dictionnaire
Carte des différences
Huffman (huffman - Zillow)
System de moyenne (genre ondelette)
  
Compression frequentielle

Compression pattern (ex: jpg)

On va faire: 
  Espace col(done in tp) -> Reechantillonage(done in tp2, need better algorithme though, PSNR is ~30) ->  2 Palette(Y, CbCr) -> Dico (?2 dico: lum/chro) -> Huffman
    A ajouter: palette, dico, (1 dim taille img?), suite de pix 


First thing:
  PSNR calculation.
  and Dumper, so we can dump to file.
