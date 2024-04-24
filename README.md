# fst_m1_moteur3d
## Compilation
```bash
mkdir build
cd build
cmake ..
make
```

## Exécution
Donner en paramètre un fichier de scène (comme [african_head.scene](./obj/african_diablo.scene)) ou directement un fichier .obj (comme [african_head.obj](./obj/african_head.obj)).

```bash
fst_m1_moteur3d ../obj/african_diablo.scene
```

Des paramètres sont disponibles pour activer ou désactiver certaines fonctionnalités du programme.

Pour plus d'informations, exécutez la commande suivante :
```bash
fst_m1_moteur3d --help
```

## Eléments problématiques à ce jour
> En prenant pour exemple le fichier [african_head.scene](./obj/african_diablo.scene).
- Les yeux ne s'affichent pas correctement (outer est devant inner). J'ai commenté *outer* dans le fichier de scène pour que *inner* soit visible.
- [linear interpolation with perspective deformations](https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties:-linear-interpolation-with-perspective-deformations),
on peut voir que la texture au sol n'est pas correcte. Je ne parviens pas à résoudre ce problème.
- Probablement un souci avec mes normales : si on désactive la normalMap, le sol est tout noir.
- L'ombre sur le front d'*african_head* n'est pas lisse.
