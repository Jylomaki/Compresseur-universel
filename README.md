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
  Espace col(done in tp) -> couper bit poid faible   
    CbCr -> Reechantillonage(done in tp2, need better algorithme though, PSNR is ~30) ->  Palette -> Dico    
    Y -> codage par plage par block par couche   
    huffman   
    
    A ajouter: palette, dico, (1 dim taille img?), suite de pix  


First thing:
  PSNR calculation.
  and Dumper, so we can dump to file.
