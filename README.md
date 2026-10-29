# PatchMSIFileVersion



Patch le nom d'un fichier .MSI en remplaçant \[VERSION] par la version réelle du fichier

(par exemple Setup\_\[VERSION].msi deviendra Setup\_1.0.0.msi)



Utilisation dans le post-build d'un projet VDPROJ lors de la génération d'un Setup :

Il suffit de configurer le projet pour générer un fichier de nom  xxxxx\_\[VERSION].msi

et dans le post-build d'ajouter la syntaxe :

"PatchMSIFileVersion.exe" "$(BuiltOuputPath)"



L'exécutable peut également être lancé en ligne de commande "DOS"



