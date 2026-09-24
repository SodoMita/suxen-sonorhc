# Design canon

Five levers, in the order you apply them. They are how you judge a picture. They are not a licence to invent an answer the level doc does not give.

## 1. Silhouette

Fill the figure 100% black. If you cannot name the character, the design has failed. Fix that before spending a generation on rendering.

Mass sits at roughly 6:3:1:

| Tier | Job |
|---|---|
| Big | the class of shape, readable across a room |
| Mid | rhythm: hair, sleeves, props |
| Small | buttons, trim, the second look |

Negative space is part of the design. The gap between Ren's arm and his torso, the holes in his smoke, the air between Aurora's coat and her pane, the notches of Kira's ears — closing those holes makes the character less readable, not more.

Each silhouette class in `CAST_AT_A_GLANCE.md` is unique. Do not give a second character a wing bar, a code pane, or an absent face.

## 2. Memory point

Exactly one. It is the sentence in the level doc, copied, not improved. It gets the accent colour and it appears in every asset, including the back view of a turnaround.

A second competing focal point halves both. The secondary hook is allowed to be found on a second look: Ren's smoke holes, Selene's chained book, Nova's magenta streak.

## 3. Colour

Anime casts do not follow a 60-30-10 area split. The head carries hair, skin, and eyes. The body carries a small set of garment zones. The main colour is a decision about what the character means.

Ren's main colour is void black. His eyes are cyan, and that cyan is reserved. Aurora's main colour is reactor cyan, and it means the station is still running. Those two cyans must not be confused: his is two points, hers is a pane.

Judge the roster together, in colour and in greyscale. The checker rejects main hexes closer than distance 80. It cannot see a bad greyscale. You still have to.

## 4. Shape

Circle reads warm or young, square stable, triangle sharp. The ratios in the source JSON are there so the roster does not converge. Do not nudge them toward a third each.

## 5. Symbol set

Eye, brow, mouth, contour, hair, body. The eye carries most of the read. Copy the fields into prompts. A "friendlier" paraphrase is an off-model sprite.

Ren has no mouth. Do not add one because a neutral face "should" have one.

## Roster

Any two characters differ in silhouette class and in main colour. That is the minimum. If a thumbnail of the set could swap two names, the designs have not separated, whatever the checker said about hex distance.
