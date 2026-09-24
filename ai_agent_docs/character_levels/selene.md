# Selene — description levels

`id: selene` · appeal track `presence` · approved: `false`

Numeric age not locked. Depicted as an adult. Appeal is the staff, the book, and the coat. Do not turn the coat into a slipping robe.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 32 words | roster, thumbnail, silhouette test | never |
| L1 Card | 127 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 253 words | design review, first composition without construction | never |
| L3 Production | 402 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1495 chars | you are about to generate the default sprite | yes, saved to `characters/selene/prompts/` first |

## Lore

- `docs/vn_specific/selene_lore.md`
- `docs/vn_specific/characters_starting/selene_origin.md`

## L0 Glance

Selene reads as a tall inverted triangle of staff and coat tails. Main colour is twilight violet, not black. Memory point: a crystal-headed staff held at her side, taller than her shoulder.

## L1 Card

Selene is the twilight scholar, depicted as an adult, keeper of scripts and a dry sense of humour. She reads as a tall inverted triangle of staff and coat tails: dark coat, long violet hair, elf ears, a staff. The thing a player names is a crystal-headed staff held at her side, taller than her shoulder. Main colour is twilight violet, with a pale-violet eye and a gold edge. The default costume is the twilight coat, closed, with boots and the staff. At rest she looks knowing, almost smiling, not fierce. A second look finds a small book on a chain. She is not Kira, though both can read as triangles: Kira is short, eared, and amber. Selene is tall, staffed, and violet. The coat stays closed.

## L2 Design

Selene is the tall dark triangle. Silhouette class: tall inverted triangle of staff and coat tails. Memory point: a crystal-headed staff held at her side, taller than her shoulder. Big mass is the coat tails plus the staff. Mid mass is the long hair and the sleeve. Small mass is the crystal, the chained book, and the gold edge. Negative space is the opening between staff and coat. If the staff overlaps the body completely, the triangle dies. Eyes are pale violet, level, one catchlight. Brow has a slight arch, not a scowl. Mouth is closed, almost a smile. Elf ears show. Hair is deep violet and long, not a silver bell. Body is a tall adult-coded build under the coat. Main colour is twilight violet #5B2E86, and it means dusk, not Ren's void and not Elara's green. Coat is a darker #2A2148. Crystal is #B388E0. Gold cord is #C6A15A, used only as edge, not as a second main colour. Default outfit is the twilight coat. A dark violet twilight coat over a closed inner robe, gold filigree at the edges, boots, a crystal staff, and a spellbook chained at the belt. The archive wrap, coat without staff, is not the standing sprite. Nearest triangle is Kira. Separate them by height of the prop, hair length, and hue: amber wedge versus violet staff. In greyscale Selene is a dark triangle with a lighter crystal at the top of the staff. That crystal is the thumbnail test along with the tail of the coat.

## L3 Production

Production notes for Selene. Authority is the coat and the staff, not an open neckline. The staff is the memory point and must clear the shoulder: a crystal-headed staff held at her side, taller than her shoulder. The book is the secondary hook. If the book becomes as detailed as the crystal, it is competing. Keep it small and closed. Staff hand shows five digits around the shaft. The other hand rests at the belt near the chain, also five digits. Boots fully in frame, on the bottom edge. Default costume, the twilight coat: Heavy wool, front opening overlapped and staying closed, gold cord at the edges, tails to the floor. The staff is wood with a faceted crystal head, held in one hand, its foot near the bottom edge. The spellbook is a small closed volume on a short chain: a second look, not a second focal point. The front overlap does not fall open. Gold is cord and a little filigree, not jewelry across the chest. No high slit. The archive wrap is not drawn for the runtime standing set. Expressions are face-only. Neutral is knowing. Happy is a real small smile, still not sweet. Sad drops the irony: inner brow up, mouth flat. Surprised widens the eyes and parts the lips, no teeth. Blush is a faint cheek wash. The staff and the hair do not move between expressions. Legacy keys exist for all five. Do not overwrite them. Light: cool rim on the hair, a small warm glint in the crystal only, face evenly lit. Do not turn the crystal into a lens flare that hides the head. Face box locks on the first approved master. The staff is outside the face box and must stay identical across expressions. Height is unconfirmed. Tall is a silhouette claim, not a centimetre. Legacy: Shipped selene sprites already carry the staff, the coat, and the book. They are opaque-on-black. Use them for proportion only. Do not img2img them toward a slipping robe. QA rejects: a slipping robe, an open coat, a sword in place of the staff, a missing crystal, the book drawn larger than the hand, child-coded proportions, cropped boots. Readable fake letters on the coat are text. Use abstract filigree, not words. A dark violet twilight coat over a closed inner robe, gold filigree at the edges, boots, a crystal staff, and a spellbook chained at the belt. #5B2E86

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Selene. Twilight scholar with a staff and a chained book. Class: tall inverted triangle of staff and coat tails. Tall dark figure, coat tails to the floor, a staff taller than the shoulder, long violet hair. Eyes: pale violet eyes, level, knowing, one catchlight. Hair: long deep-violet hair, one braid optional, not a bell. Defining feature: a crystal-headed staff held at her side, taller than her shoulder.
WARDROBE: Twilight coat. A closed dark coat, boots, and a tall staff. Props: crystal staff, chained spellbook.
COLOR: Main twilight violet #5B2E86. Hair #3A2158. Eyes #C9A6E8. Outer #2A2148. Accent #B388E0.
STAGING: Standing, staff in one hand, book hand at the belt, coat tails spread, feet together. Neutral, alert, eyes to camera. A cool rim on the hair and a small warm glint in the crystal, face evenly lit. Focal point: a crystal-headed staff held at her side, taller than her shoulder, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: slipping robe; staff replaced by a sword; open coat; child-coded proportions.
```

## Canon card

- **Memory point:** a crystal-headed staff held at her side, taller than her shoulder
- **Secondary hook:** a small spellbook on a short chain at the belt
- **Silhouette class:** tall inverted triangle of staff and coat tails
- **Masses:** big — the coat tails and the staff making a triangle; mid — the long hair and the sleeve; small — the crystal head, the chained book, the gold edge
- **Negative space:** the opening between the staff and the coat, which must stay readable
- **Main colour:** twilight violet `#5B2E86` — dusk, not night-black and not moon-green
- **Default outfit:** twilight coat
- **Alternate outfit:** archive wrap
- **Shape majority:** triangle (0.15 circle / 0.25 square / 0.6 triangle)
- **Legacy:** Shipped selene sprites already carry the staff, the coat, and the book. They are opaque-on-black. Use them for proportion only. Do not img2img them toward a slipping robe.

### Colour zones

- **hair**: deep violet `#3A2158`
- **skin**: deep warm `#6B4638`
- **eye**: pale violet `#C9A6E8`
- **tops1**: coat violet `#2A2148`
- **tops2**: inner robe `#1A1630`
- **waist**: gold cord `#C6A15A`
- **bottom1**: coat tail `#241C40`
- **bottom2**: boot `#1A1428`
- **shoes**: boot black `#14101C`
- **decoration1**: crystal `#B388E0`
- **decoration2**: filigree `#C6A15A`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | selene_neutral | knowing and calm |
| happy | selene_happy | pleased, not sweet |
| sad | selene_sad | the irony dropped |
| surprised | selene_surprised | genuinely caught |
| blush | selene_blush | the current face plus a faint cheek wash |

### Banned in every prompt

- slipping robe
- staff replaced by a sword
- open coat
- child-coded proportions

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether the archive wrap is ever needed as a standing sprite.
