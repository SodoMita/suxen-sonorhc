# Ren — description levels

`id: ren` · appeal track `shadow` · approved: `false`

Minor-coded. The prologue places him at about sixteen, in a school uniform. Appeal is the shadow and the eyes. Do not age him up, and do not redesign the uniform.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 36 words | roster, thumbnail, silhouette test | never |
| L1 Card | 140 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 287 words | design review, first composition without construction | never |
| L3 Production | 506 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1431 chars | you are about to generate the default sprite | yes, saved to `characters/ren/prompts/` first |

## Lore

- `docs/vn_specific/ren_lore.md`
- `docs/vn_specific/characters_starting/ren_origin.md`

## L0 Glance

Ren reads as a narrow unlit column with no face. Main colour is void black, not the cyan of the eyes. Memory point: a face that is only two cyan eyes in a black school-uniform shadow.

## L1 Card

Ren is the exchange student whose body settled into a stable dark after the Nexus drift. He reads as a narrow unlit column with no face: closed black uniform, one hand pocketed, smoke at the calves, no readable mouth. The thing a player names is a face that is only two cyan eyes in a black school-uniform shadow. Main colour is void black. Hair and cloth are the same dark; only the eyes are cyan. The default costume is the shadow gakuran, a closed high-collar uniform, not a coat and not armor. At rest the eyes are steady and alert, not angry. A second look finds the button line and the holes in the smoke. He is the darkest figure in the cast. Nobody else has an absent face. Aurora's cyan is a pane; his cyan is only the eyes.

## L2 Design

Ren is the cast's dark anchor. Silhouette class: narrow unlit column with no face. Memory point, stated once and kept in every asset: a face that is only two cyan eyes in a black school-uniform shadow. Big mass is the unlit column of the gakuran from collar to shoes. Mid mass is the short hair and the smoke at the calves. Small mass is the two eyes and the five-button line. Negative space is the gap between the free arm and the torso, plus the holes in the smoke. Do not close either hole with a cape, a scarf, or a painted floor shadow. Eyes are two narrow cyan lights with no iris drawing and no mouth under them. The brow is only a level shadow. Hair is short and disappears into the same black as the collar. The body is a narrow teenage build inside a square uniform. Main colour is void black #1A1C24, and it means a person who learned to be unseen. The eye cyan #3DFFE8 is reserved: it appears in the eyes and nowhere else. Hair, skin, and cloth stay inside the black family. Smoke is a grey #6A7278, softer than the suit. Default outfit is the shadow gakuran. A black gakuran with a standing collar, five buttons, straight trousers, and plain black shoes, the cloth reading as one dark mass. The alternate, the pre-drift coat, is the same cut in ordinary wool and is not drawn until a flashback is approved. Nearest neighbour is Aurora, who also uses cyan. The difference is placement: her cyan is a tall pane and trim on a pale coat; his is two eyes in a void. In greyscale he is the darkest sprite on the roster.

## L3 Production

Production notes for Ren. Identity is locked before polish. If a beautiful frame gives him a mouth, a lit cheek, or a brighter suit, it is off-model and it is rejected. The memory point stays verbatim in every asset: a face that is only two cyan eyes in a black school-uniform shadow. The free arm must not touch the torso. The smoke must not become a puddle or a ground shadow. Feet sit on the bottom edge of the canvas, shoes fully in frame. Hands: one is a simple pocket shape, the other a closed hand with a thumb, five digits, no extra fingers. Do not hide both hands; the pocket is allowed, the second hand is not. Default costume, the shadow gakuran, in construction: The collar stands about two fingers high and closes with a hidden hook; five dark buttons run down the centre; shoulders are square and unpadded; trousers break once over plain black shoes; no shirt cuff shows. Smoke is a soft edge at the calves, not a floor shadow under the shoes. Cloth weight is wool, matte, almost no highlight. Buttons are the same value as the cloth, readable by shape, not by shine. The alternate pre-drift coat is out of scope until the open question is answered. Do not invent a civilian outfit, a scarf, or a weapon. Expressions change only the eye region and, for blush, a faint warm rim on the cheek shadow. Neutral: eyes level, medium cyan, gaze to camera. Happy: eyes slightly narrowed, cyan unchanged. Sad: gaze lowered a little, cyan dimmer. Surprised: eyes wider, cyan brighter. Blush is an overlay, not a new pose, and it must not create a readable mouth. Hair silhouette stays byte-stable across the set. There is no happy-open-mouth version. There is no crying version with tears drawn as white streaks down a lit face. Staging: standing, weight even, pocket hand on the viewer's right or left but consistent once the first sprite is approved. Light is almost none. A thin cool rim may separate the shoulder from a dark scene; it must not reveal a nose, lips, or skin tone. Face box: lock it on the first approved sprite, around the eyes only, and never move it. Do not invent pixel numbers before that sprite exists. Relative height is unconfirmed. Do not scale him against the cast until the owner states centimetres. Legacy files: Shipped ren.webp and ren_shadow.webp are the same opaque-on-black picture. They already match this identity. Do not img2img them into a readable face, a different uniform, or a brighter body. Black-key matting will delete him. Alpha comes only from a white plate and a black plate. QA rejects: a mouth; a lit face; a cape; a coloured streak in the hair; cropped shoes; a ground shadow; any costume that reads as glamour rather than a school uniform. He is minor-coded. The uniform stays a uniform. A black gakuran with a standing collar, five buttons, straight trousers, and plain black shoes, the cloth reading as one dark mass. #1A1C24

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Ren. Shadow protagonist in a school uniform. Class: narrow unlit column with no face. Narrow black school-uniform figure, face unreadable except two cyan eyes, smoke at the calves. Eyes: two narrow cyan eyes, no mouth. Hair: short black hair lost in the shadow. Defining feature: a face that is only two cyan eyes in a black school-uniform shadow.
WARDROBE: Shadow gakuran. A closed black school uniform with a high collar. Props: none beyond the uniform and the eye-light.
COLOR: Main void black #1A1C24. Hair #1A1C24. Eyes #3DFFE8. Outer #141820. Accent #3DFFE8.
STAGING: Standing, one hand in a pocket, the other clear of the torso, feet on the bottom edge. Neutral, alert, eyes to camera. Almost no key light. The eyes are the only bright source. Focal point: a face that is only two cyan eyes in a black school-uniform shadow, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: readable mouth; lit face; glamour redesign of the uniform; cape filling the arm gap.
```

## Canon card

- **Memory point:** a face that is only two cyan eyes in a black school-uniform shadow
- **Secondary hook:** smoke leaving the calves, with holes in it
- **Silhouette class:** narrow unlit column with no face
- **Masses:** big — the unlit column of the gakuran from collar to shoes; mid — short hair mass and the smoke at the calves; small — two cyan eyes and a five-button line
- **Negative space:** the gap between the free arm and the torso, plus the holes in the smoke
- **Main colour:** void black `#1A1C24` — a person who learned to be unseen
- **Default outfit:** shadow gakuran
- **Alternate outfit:** pre-drift coat
- **Shape majority:** square (0.05 circle / 0.8 square / 0.15 triangle)
- **Legacy:** Shipped ren.webp and ren_shadow.webp are the same opaque-on-black picture. They already match this identity. Do not img2img them into a readable face, a different uniform, or a brighter body.

### Colour zones

- **hair**: shadow black `#1A1C24`
- **skin**: unlit `#14161C`
- **eye**: eye cyan `#3DFFE8`
- **tops1**: gakuran black `#141820`
- **tops2**: hidden shirt `#101218`
- **waist**: button line `#2A3038`
- **bottom1**: trouser black `#12141A`
- **bottom2**: crease grey `#2C3138`
- **shoes**: shoe black `#0C0E12`
- **decoration1**: eye cyan `#3DFFE8`
- **decoration2**: smoke grey `#6A7278`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | ren, ren_shadow | steady and present, not angry |
| happy | — | a small ease |
| sad | — | withdrawn |
| surprised | — | caught |
| blush | — | a faint warm rim on the cheek shadow |

### Banned in every prompt

- readable mouth
- lit face
- glamour redesign of the uniform
- cape filling the arm gap

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether the calf smoke is a second sprite layer.
- Whether a pre-drift flashback sprite is in scope.
