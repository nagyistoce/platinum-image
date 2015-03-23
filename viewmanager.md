Viewmanager lists the available viewports and allows access through viewport [IDs](IDs.md) and various ways of requesting updates.

# Plans #

```
Example of a SINGLE virtual view 

              3                 = noofhorizontaltiles

 +-----+------+-------------+
 |     |      |             |
 +-----+      |             |
 |     |      |             |
 |     |      +-------------+
 |     |      |             |
 +-----+------+-------------+
   2      1       2             = noofverticaltiles[0,1,2]


 ID (unikt id i en viewport, inte index i viewports-vektorn) 
 talar om vilken viewport som ska visas på
 en plats i griden. Det är ID som  manipulate_views ändrar på (nya viewports 
 kan tillkomma här, och gamla tas bort).
 -1 = viewport saknas f.n. på denna plats i griden

 viewportmapping[v][h][vv]

 En 3D-loop sköter vyerna. Den YTTERSTA loopen är vilken av N st
 virtuella (samtida) viewports som ska visas.
 De två innersta är antal byer i x- respektive y-led (för varje x-pos, ska vi lagra en y-storlek).
```