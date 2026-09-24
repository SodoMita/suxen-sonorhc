# Kira — description levels

`id: kira` · appeal track `cool` · approved: `false`

Age not locked. Lore recruits her as a teenager and never states a current adult age. Appeal is ears, tail, and competence. The costume stays fully covering. Do not use the shipped beachwear armor as a reference.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 39 words | roster, thumbnail, silhouette test | never |
| L1 Card | 139 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 292 words | design review, first composition without construction | never |
| L3 Production | 461 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1478 chars | you are about to generate the default sprite | yes, saved to `characters/kira/prompts/` first |

## Lore

- `docs/vn_specific/kira_lore.md`
- `docs/vn_specific/characters_starting/kira_origin.md`

## L0 Glance

Kira reads as a compact athletic wedge with ears breaking the outline. Main colour is sun amber, not rust armor. Memory point: cat ears scorched pale at the tips, and a tail that ends in a hard amber light.

## L1 Card

Kira is the scout who feels rift weather through her ears and tail. She reads as a compact athletic wedge with ears breaking the outline: short gold hair, a closed jacket, boots, and a tail. The thing a player names is cat ears scorched pale at the tips, and a tail that ends in a hard amber light. Main colour is sun amber, in the eyes and the tail tip, against a rust jacket and dark trousers. The default costume is the scout jacket, a closed high-collar jacket, trousers, and boots. At rest she looks alert and pleased, not fierce. A second look finds the scratched sleeve where a seal used to be. She is not the staff-bearer and not the hooded analyst. The ears and the amber tail tip are the difference. No sword. The jacket stays closed.

## L2 Design

Kira is the warm wedge of the roster. Silhouette class: compact athletic wedge with ears breaking the outline. Memory point: cat ears scorched pale at the tips, and a tail that ends in a hard amber light. Big mass is the torso wedge and the short hair. Mid mass is the ears, the jacket hem, and the tail. Small mass is the amber tip, the knee pads, and the scratched sleeve. Negative space is the gap under each ear and the air between arm and jacket. Do not fill it with a cape or a pauldron. Eyes are bright amber, lively, one catchlight. Brow slightly up. Mouth a small closed smile at rest. Cheeks are round, not sharp. Hair is short and sun-gold. Ears sit above it, tips scorched pale. Body is compact and athletic, fully covered, not a chibi. Main colour is sun amber #E8942A, and it means heat she no longer has to hide. It is reserved for eyes and the tail tip. Jacket is scout rust #C4652A. Shirt is dark #3E2A24. Trousers #4A3428. Ear scorch is a pale #F6E2B8, a small mark, not a second focal point. Default outfit is the scout jacket. A sun-faded scout jacket with a standing collar, worn closed over a dark shirt, cargo trousers, knee pads, and boots; the academy seal is scratched off the sleeve. The alternate academy suit is a sealed pressure suit for flashbacks only, tail strapped, no weapon. Nearest neighbour in warmth is nobody; in silhouette Selene is also a triangle, but hers is a tall staff and coat tails. Kira is short-haired, eared, and amber. Selene is violet, long-haired, and staffed. In greyscale Kira is a mid-value wedge with two ear notches. That notch is the thumbnail test. #E86A1A

## L3 Production

Production notes for Kira. Competence and the animal signs carry the appeal. The costume is work clothes. Memory point in every asset: cat ears scorched pale at the tips, and a tail that ends in a hard amber light. If the ears are missing, the same colour as the hair all the way to the tip, or the tail tip is a flame sprite, reject. Hands: one on the hip, one relaxed, five digits, no weapon. The signal wand is a folded stick on the belt, not drawn as a blade. Feet in boots, both fully in frame, on the bottom edge. Tail visible from the side or behind the leg, not wrapped as a belt. Default costume, the scout jacket: Issued canvas, two chest pockets with button flaps, a cloth belt, sleeves rolled once. Lighter than the sealed academy suit, and it still covers torso, arms to the wrist, and legs to the boot. The tail leaves under the hem and ends in a hard amber tip, not a flame. Canvas shows a little wear at the cuffs. Button flaps, not a bare chest harness. Collar stands. Zip or buttons run to the neck. No stomach gap. No thigh cutout. The scratched sleeve is a pale scrape where a patch was, small enough to miss at thumbnail size. The academy-suit alternate is fully sealed and out of scope until a flashback label exists. Expressions are face-only. The ears do not flop between differentials; an ear-move is a separate pose, not an expression. Neutral is alert and pleased. Happy, the legacy kira_happy key, is an open smile without a new pose. Sad drops the joke: inner brow up, mouth flat. Surprised widens the eyes and opens the mouth slightly, no teeth. Blush is a cheek wash on the current face. There is no legacy neutral, sad, or surprised file yet; do not pretend the happy file is neutral. Staging light is a warm key from the upper left, as if from a low sun, with a cool fill in the jacket shadow. Face box locks after the first approved sprite. Head does not translate between expressions. Height is unconfirmed. Her compactness is silhouette, not a licence to draw a child. Legacy: Shipped kira sprites are armor cut like beachwear, with an exposed stomach and a sword. They are not the production reference. Do not img2img from them. QA rejects: beachwear cut, an exposed stomach, a sword, child-short proportions, a missing tail tip, ears that do not break the outline, a cape. Age is not locked. Draw the scout, not a glamour costume. A sun-faded scout jacket with a standing collar, worn closed over a dark shirt, cargo trousers, knee pads, and boots; the academy seal is scratched off the sleeve. #E86A1A

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Kira. Sunfire scout who reads rift weather through ears and tail. Class: compact athletic wedge with ears breaking the outline. Compact closed jacket, ears above the hair, amber tail tip. Eyes: bright amber eyes, lively, one catchlight. Hair: short sun-gold hair, ears above it, tips scorched pale. Defining feature: cat ears scorched pale at the tips, and a tail that ends in a hard amber light.
WARDROBE: Scout jacket. A closed high-collar jacket, trousers, and boots. Props: signal wand on the belt.
COLOR: Main sun amber #E86A1A. Hair #F0C14A. Eyes #E86A1A. Outer #C4652A. Accent #E86A1A.
STAGING: Standing, one hand on the hip, the other relaxed, both feet in frame. Neutral, alert, eyes to camera. Warm key from the upper left, as if from a low sun, cool fill on the jacket shadow. Focal point: cat ears scorched pale at the tips, and a tail that ends in a hard amber light, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: beachwear armor; exposed stomach; sword on the hip; child-coded chibi proportions.
```

## Canon card

- **Memory point:** cat ears scorched pale at the tips, and a tail that ends in a hard amber light
- **Secondary hook:** a scratched-off academy seal on the sleeve
- **Silhouette class:** compact athletic wedge with ears breaking the outline
- **Masses:** big — the wedge of the torso and the short hair; mid — the ears, the jacket hem, and the tail; small — the amber tail tip, knee pads, and the scratched sleeve
- **Negative space:** the gap under the raised ear and the space between arm and jacket
- **Main colour:** sun amber `#E86A1A` — heat she no longer has to hide
- **Default outfit:** scout jacket
- **Alternate outfit:** academy suit
- **Shape majority:** triangle (0.2 circle / 0.25 square / 0.55 triangle)
- **Legacy:** Shipped kira sprites are armor cut like beachwear, with an exposed stomach and a sword. They are not the production reference. Do not img2img from them.

### Colour zones

- **hair**: sun gold `#F0C14A`
- **skin**: warm `#E6B08A`
- **eye**: amber `#E86A1A`
- **tops1**: scout rust `#C4652A`
- **tops2**: dark shirt `#3E2A24`
- **waist**: cloth belt `#6B4A32`
- **bottom1**: cargo brown `#4A3428`
- **bottom2**: knee pad `#8A5A32`
- **shoes**: boot brown `#3A2418`
- **decoration1**: tail tip `#E86A1A`
- **decoration2**: ear scorch `#F6E2B8`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | — | alert and pleased, not blank |
| happy | kira_happy | open delight |
| sad | — | the joke dropped |
| surprised | — | ears would move, but the head does not |
| blush | kira_blush | the current face plus a cheek wash |

### Banned in every prompt

- beachwear armor
- exposed stomach
- sword on the hip
- child-coded chibi proportions

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether the sealed academy suit is needed for a flashback in the current dialogue.
