# Elara — description levels

`id: elara` · appeal track `presence` · approved: `false`

Numeric age not locked. Depicted as an adult priestess. Appeal is calm, cloth, and the crescents. Do not turn the vestments into a costume accident.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 37 words | roster, thumbnail, silhouette test | never |
| L1 Card | 140 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 264 words | design review, first composition without construction | never |
| L3 Production | 414 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1470 chars | you are about to generate the default sprite | yes, saved to `characters/elara/prompts/` first |

## Lore

- `docs/vn_specific/elara_lore.md`
- `docs/vn_specific/characters_starting/elara_origin.md`

## L0 Glance

Elara reads as a wide floor-length bell with a side braid. Main colour is moon green, not gold. Memory point: a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green.

## L1 Card

Elara is the healer of the moonlit groves, depicted as an adult priestess. She reads as a wide floor-length bell with a side braid: a pale gown to the floor, long silver hair, elf ears. The thing a player names is a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green. Main colour is moon green, reserved for the eyes and the two clasps, against a near-white gown. The default costume is the moon vestments, an opaque pale gown with a high collar and a floor-width skirt. At rest she is calm, lids lowered, a small smile. A second look finds the side braid. She is not Aria: no wings, no gold ring. She is not Aurora: no code pane, and the skirt is a bell, not a coat. The silk stays cloth.

## L2 Design

Elara is the wide pale bell. Silhouette class: wide floor-length bell with a side braid. Memory point: a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green. Big mass is the floor-width skirt. Mid mass is the sleeves and the long hair. Small mass is the two crescents, the braid ribbon, and the elf ears. Negative space is the air between sleeve and skirt. Do not fill it with a cloak in the default sprite. Eyes are calm green, lids lowered, one catchlight. Brow soft and level. Mouth a small closed smile. Face is long. Elf ears are visible and not hidden by hair. Hair is silver with one side braid. The body is adult-coded and mostly hidden by the gown. Main colour is moon green #7EAE8A, and it means grove light, not station cyan. It is reserved for eyes and crescents. Gown is near-white #F7FBFF. Sash is a cool #C5D6EA. Ribbon is #A8C8C0. Do not use Aria's gold. Default outfit is the moon vestments. Layered moon-priestess vestments: an opaque ice-pale gown, a high collar, long sleeves, and a floor-width skirt, closed with two matching crescent clasps. The alternate grove cloak is the same gown plus a pale cloak, and only if a scene asks. The crescents must remain visible. Nearest neighbour is Aria, another pale gown and silver hair. Aria's outline is a wing bar and a ring. Elara's is a bell and a braid. Aurora is a narrow coat plus a pane, not a bell. In greyscale Elara is the widest light shape without wings.

## L3 Production

Production notes for Elara. The vestments are the character. They do not malfunction into a reveal. Both crescents are always visible: a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green. If one crescent is missing, or they are different colours, reject. Elf ears must read at sprite size. Hands are open and relaxed, five digits, no staff, no book. Sandals show at the hem, closed at the toe, on the bottom edge. Default costume, the moon vestments: Two weights of silk: a heavier opaque gown under a lighter overskirt that still reads as cloth, not gauze. Throat clasp and skirt crescent are the same silver-green metal, sewn on, not floating. A side braid is bound with a narrow ribbon. Sandals close at the toe. Nothing lifts or turns transparent. Lace at the hem is a pattern in the cloth, not a hole. Sleeves are long. Collar is high. The overskirt has weight and folds; it does not cling and it does not lift. The grove-cloak alternate stays off the default sprite. If drawn later, it is hoodless and does not cover the throat clasp. Expressions are face-only. Neutral is calm, not blank. Happy widens the closed smile slightly. Sad raises the inner brow and lowers the gaze. Surprised widens the eyes and opens the mouth a little, no teeth. Blush is a cheek wash, not a new pose and not a costume change. Hair and braid stay put between differentials. Legacy keys already exist for all five expressions. New masters must not overwrite those files; they get new names until approved and installed beside them. Light is cool moonlight from above and slightly in front, with soft shadow in the skirt folds. The face is the cleanest value. Face box locks on the first approved master. The wide skirt means the head is a smaller fraction of the canvas than on Ren; do not enlarge the head to compensate. Height is unconfirmed. Legacy: Shipped elara sprites are close to this gown and can guide proportion, but they are opaque-on-black and were not matted. Do not img2img them toward a thinner dress. QA rejects: a thinner dress, a lifting hem, transparent silk, gold halo, wings, a code pane, hidden crescents, cropped sandals, a staff. Dialogue that describes cloth misbehaving is not a sprite instruction. Layered moon-priestess vestments: an opaque ice-pale gown, a high collar, long sleeves, and a floor-width skirt, closed with two matching crescent clasps. #7EAE8A

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Elara. Moon-grove healer in layered vestments. Class: wide floor-length bell with a side braid. Wide pale gown to the floor, long silver hair, one braid at the side, elf ears. Eyes: calm green eyes, lowered lids, one catchlight. Hair: long silver hair with one side braid. Defining feature: a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green.
WARDROBE: Moon vestments. An opaque pale gown with two crescent clasps. Props: crescent clasps.
COLOR: Main moon green #7EAE8A. Hair #E4E8F0. Eyes #7EAE8A. Outer #F7FBFF. Accent #7EAE8A.
STAGING: Standing, hands lightly open at her sides, skirt spreading, feet together under the hem. Neutral, alert, eyes to camera. Cool moonlight from above and slightly in front, soft shadow in the skirt folds. Focal point: a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: thinner dress; lifting hem; transparent silk; hidden crescents.
```

## Canon card

- **Memory point:** a crescent clasp at the throat and a second crescent on the skirt, both the same silver-green
- **Secondary hook:** a side braid bound with a narrow ribbon
- **Silhouette class:** wide floor-length bell with a side braid
- **Masses:** big — the floor-width bell of the skirt; mid — the sleeves and the long hair; small — two crescents, the braid ribbon, the elf ears
- **Negative space:** the air between the wide sleeves and the skirt, and under the hem before it meets the floor
- **Main colour:** moon green `#7EAE8A` — grove light, not station cyan
- **Default outfit:** moon vestments
- **Alternate outfit:** grove cloak
- **Shape majority:** circle (0.62 circle / 0.18 square / 0.2 triangle)
- **Legacy:** Shipped elara sprites are close to this gown and can guide proportion, but they are opaque-on-black and were not matted. Do not img2img them toward a thinner dress.

### Colour zones

- **hair**: moon silver `#E4E8F0`
- **skin**: fair `#F4D5C8`
- **eye**: grove green `#7EAE8A`
- **tops1**: gown white `#F7FBFF`
- **tops2**: sleeve `#E7F0F6`
- **waist**: sash `#C5D6EA`
- **bottom1**: skirt `#F4F8FC`
- **bottom2**: lace hem `#D5E0EA`
- **shoes**: sandal silver `#C0C8D0`
- **decoration1**: crescent `#7EAE8A`
- **decoration2**: braid ribbon `#A8C8C0`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | elara_neutral | calm and present |
| happy | elara_happy | warm, not giddy |
| sad | elara_sad | worried for someone else |
| surprised | elara_surprised | a quiet startle |
| blush | elara_blush | the current face plus a cheek wash |

### Banned in every prompt

- thinner dress
- lifting hem
- transparent silk
- hidden crescents

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether the grove cloak is needed in the current labels.
