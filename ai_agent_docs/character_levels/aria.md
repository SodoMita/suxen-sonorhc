# Aria — description levels

`id: aria` · appeal track `cool` · approved: `false`

Depicted age not locked. A trial at sixteen is backstory; the sprite is not a child, and it is not a glamour figure. Appeal is the wings, the ring, and the held feather.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 34 words | roster, thumbnail, silhouette test | never |
| L1 Card | 144 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 262 words | design review, first composition without construction | never |
| L3 Production | 414 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1444 chars | you are about to generate the default sprite | yes, saved to `characters/aria/prompts/` first |

## Lore

- `docs/vn_specific/aria_lore.md`
- `docs/vn_specific/characters_starting/aria_origin.md`

## L0 Glance

Aria reads as a wide wing bar under a thin ring halo. Main colour is halo gold, not silver. Memory point: a thin gold ring halo, and one loose feather held at the chest.

## L1 Card

Aria is the keeper of the sky sanctum, on screen as a winged figure in a closed gown. She reads as a wide wing bar under a thin ring halo: wings wider than the skirt, long white hair, no elf ears. The thing a player names is a thin gold ring halo, and one loose feather held at the chest. Main colour is halo gold, a thin ring and a waist band, against a white gown and soft blue eyes. The default costume is the sanctum gown, closed, long sleeves, full skirt, bare feet. At rest she is gentle, not blank. A second look finds grey tips on the wings. She is not Aurora, who has a code pane and no wings. She is not Elara, who has crescents, a braid, and elf ears. The halo is a ring, not a disc of light.

## L2 Design

Aria is the wide white bar of the roster. Silhouette class: wide wing bar under a thin ring halo. Memory point: a thin gold ring halo, and one loose feather held at the chest. Big mass is the wing bar. Mid mass is the gown and the long hair. Small mass is the ring, the held feather, and the gold waist band. Negative space is the holes between wing feathers and the gap between ring and hair. Do not fill the ring into a solid disc. Eyes are soft blue, gentle, one catchlight. Brow relaxed. Mouth a small closed smile. No elf ears. Face is a soft oval. Hair is long and white, with a small gold pin, not a side braid. Body is adult-coded under the gown. Main colour is halo gold #E4C36A, and it means a duty worn as a ring. It is reserved for the ring, the waist band, and the stitch. Gown is white #FFFEFB. Eyes are soft blue #8EB4E8, not Aurora's cyan. Feather is #FFF8E8. Default outfit is the sanctum gown. A closed white sanctum gown with gold embroidery, long sleeves, a full skirt, feathered wings, and bare feet; the halo is a thin ring, not a glow. The fallen-pin alternate removes the ring and is not drawn until a scene says the halo is gone. Nearest pale figures are Elara and Aurora. Elara is a bell with green crescents and elf ears. Aurora is a coat with a pane. Aria is the only wing bar. In greyscale the wings must still separate her from Elara's skirt.

## L3 Production

Production notes for Aria. The wings and the ring do the work. The gown stays a gown. Memory point, both parts, in every asset: a thin gold ring halo, and one loose feather held at the chest. If the halo becomes a glowing disc, or the feather is missing, reject. Wing holes must survive a thumbnail. The near hand holds the feather at the chest, five digits visible enough to count. The far hand is relaxed, also five digits. Bare feet are fully drawn, both of them, on the bottom edge. This is a sanctum fact, not a costume slip. Default costume, the sanctum gown: High neckline, a gold band at the waist, feather embroidery that is stitching, not holes. Wings attach at the shoulder blades and extend sideways past the skirt, white with soft grey tips. The halo is a thin unbroken metal ring a hand's width above the hair. One loose feather is held, not falling. Feet are bare and fully drawn on the bottom edge. Neckline is high. Sleeves are long. Embroidery is stitching. The skirt has volume and does not cling. The fallen-pin variant is out of scope for the default set. Expressions are face-only. Wings do not reshuffle between differentials. Neutral is gentle and present. Happy is a warmer closed smile. Sad lowers the gaze. Surprised widens the eyes and parts the lips, no teeth. Blush is a cheek wash. Legacy keys exist for all five; do not overwrite them. Light is a soft warm key on the face. Gold glints only on the ring and the waist band, not across the whole gown. Face box locks on the first approved master. The ring is just outside the hair and must not jump between expressions. Depicted age is not locked. Do not draw a child, and do not draw a glamour redesign of the gown. Height is unconfirmed. Legacy: Shipped aria sprites match this gown, wings, and halo closely enough to guide proportion. They are opaque-on-black. Do not img2img them toward a thinner dress or a glowing disc halo. QA rejects: a glowing disc, a thinner gown, elf ears, a code pane, a staff, cropped feet, wings that do not clear the skirt, readable text on the embroidery. Feathers shed as a story beat are a separate pose, not an expression. A closed white sanctum gown with gold embroidery, long sleeves, a full skirt, feathered wings, and bare feet; the halo is a thin ring, not a glow. #E4C36A

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Aria. Sky sanctum keeper with wings and a ring halo. Class: wide wing bar under a thin ring halo. White gown, wings spread wider than the skirt, a thin ring above the head, a feather in one hand. Eyes: soft blue eyes, gentle, one catchlight. Hair: long white hair, a small gold pin, not a braid. Defining feature: a thin gold ring halo, and one loose feather held at the chest.
WARDROBE: Sanctum gown. A closed white gown, wings, and a thin ring. Props: held feather, ring halo.
COLOR: Main halo gold #E4C36A. Hair #F4F7FB. Eyes #8EB4E8. Outer #FFFEFB. Accent #E4C36A.
STAGING: Standing, one hand holding a feather at the chest, the other relaxed, wings spread, bare feet on the bottom edge. Neutral, alert, eyes to camera. Soft warm key on the face, gold glint only on the ring and the waist band. Focal point: a thin gold ring halo, and one loose feather held at the chest, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: glowing disc halo; thinner gown; elf ears; code pane.
```

## Canon card

- **Memory point:** a thin gold ring halo, and one loose feather held at the chest
- **Secondary hook:** white wings with soft grey tips
- **Silhouette class:** wide wing bar under a thin ring halo
- **Masses:** big — the horizontal wing bar; mid — the gown bell and the long hair; small — the ring halo, the held feather, the gold waist band
- **Negative space:** the holes between wing feathers, and the gap between the ring and the hair
- **Main colour:** halo gold `#E4C36A` — a duty worn as a ring, not as shine
- **Default outfit:** sanctum gown
- **Alternate outfit:** fallen pin
- **Shape majority:** circle (0.55 circle / 0.15 square / 0.3 triangle)
- **Legacy:** Shipped aria sprites match this gown, wings, and halo closely enough to guide proportion. They are opaque-on-black. Do not img2img them toward a thinner dress or a glowing disc halo.

### Colour zones

- **hair**: cloud white `#F4F7FB`
- **skin**: fair `#F6D7CC`
- **eye**: soft blue `#8EB4E8`
- **tops1**: gown white `#FFFEFB`
- **tops2**: sleeve `#F4EFE4`
- **waist**: gold band `#E4C36A`
- **bottom1**: skirt `#FFFEFB`
- **bottom2**: gold stitch `#E4C36A`
- **shoes**: bare foot `#F6D7CC`
- **decoration1**: halo gold `#E4C36A`
- **decoration2**: held feather `#FFF8E8`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | aria_neutral | gentle and present |
| happy | aria_happy | quiet joy |
| sad | aria_sad | the duty weighing |
| surprised | aria_surprised | a soft startle |
| blush | aria_blush | the current face plus a cheek wash |

### Banned in every prompt

- glowing disc halo
- thinner gown
- elf ears
- code pane

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether a later scene removes the halo.
- Whether bare feet stay once height and costume plot are confirmed.
