# Aurora — description levels

`id: aurora` · appeal track `presence` · approved: `false`

No biological age. Depicted as an adult-coded projection. Do not redraw her as a child. Appeal is the code pane and the coat, not a transparent body.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 32 words | roster, thumbnail, silhouette test | never |
| L1 Card | 134 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 295 words | design review, first composition without construction | never |
| L3 Production | 483 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1491 chars | you are about to generate the default sprite | yes, saved to `characters/aurora/prompts/` first |

## Lore

- `docs/vn_specific/aurora_lore.md`
- `docs/vn_specific/characters_starting/aurora_origin.md`

## L0 Glance

Aurora reads as a tall hair-bell column with a rectangular pane. Main colour is reactor cyan, not white. Memory point: a vertical pane of scrolling station code standing just behind her shoulder.

## L1 Card

Aurora is the holographic subsystem of station Prometheus, the voice that keeps the Nexus core from drifting. She reads as a tall hair-bell column with a rectangular pane: long silver hair, a narrow coat, and a flat rectangle of light behind one shoulder. The thing a player names is a vertical pane of scrolling station code standing just behind her shoulder. Main colour is reactor cyan, against station-silver hair and anxious cyan eyes. The default costume is the interface coat, a high-neck pale coat traced with cyan circuits, closed shoes, no wings and no bell skirt. At rest her face is serious and a little frightened, not blank. A second look finds circuit traces that stop at the cuffs. She is not the winged figure and not the priestess. The pane is the difference.

## L2 Design

Aurora is the pale technical column of the cast. Silhouette class: tall hair-bell column with a rectangular pane. Memory point: a vertical pane of scrolling station code standing just behind her shoulder. Big mass is the hair bell plus the narrow coat. Mid mass is the code pane and the hem. Small mass is the circuit trim at collar, cuffs, and hem. Negative space is the gap between the coat and the pane, and the air under the hair. Do not fill that gap with a cape or a second pane. Eyes are wide and slightly anxious, cyan, one catchlight. Brows lift at the inner corner. Mouth is small and closed. Face is a soft oval. Hair is long, straight, silver, heavier at the ends. Body is an adult-coded narrow build inside a coat, never a sealed suit. Main colour is reactor cyan #3EC8E6, and it means the station is still running. Hair is station silver #E7F4F8. The coat is near-white #F4FBFF. Eye and pane share the main cyan. Circuit trim #7EE7FF is the only lighter cyan, and it stays on the edges. Default outfit is the interface coat. A high-neck opaque tunic under a pale knee-length coat, with cyan circuit lines only on the cuffs, the collar, and the hem. The alternate lab shell is a flashback-only closed shell with no pane, and it is not drawn until a label asks. Nearest neighbours are Aria, who shares silver hair, and Elara, who shares a pale gown. Aria has wings and a ring halo and no pane. Elara has a floor-width bell and crescents. Aurora has the pane and a knee-length coat. In greyscale she is a light column with a mid-grey rectangle; Aria is a wide white bar; Elara is a wide pale bell.

## L3 Production

Production notes for Aurora. She is a projection with a solid costume, not a transparent body. The memory point is painted or layered, but it is always there: a vertical pane of scrolling station code standing just behind her shoulder. If the pane is missing, cropped, or turned into a window through the torso, reject the frame. Hands are fully drawn, relaxed, five digits, held a little off the coat so the column reads. Shoes are closed station shoes, fully in frame, feet on the bottom edge. Default costume, the interface coat: The coat closes with a standing collar and a hidden placket, no zipper teeth; the cloth is matte projected wool, heavier at the hem so it hangs still; circuit lines are embroidered light, not holes. The code pane is a separate flat rectangle behind one shoulder, not a window in the body. No zipper, no belt buckle shine, no transparent thigh panel, no circuit lines crossing the chest as a window. A stress state, not a required expression, may pixelate the coat hem into square cyan bits. Those holes are in the cloth edge only. The tunic under the coat stays opaque. The lab-shell alternate has no pane and is out of scope until a flashback label exists. Expressions are head-only edits of the approved sprite. Neutral, wired today as aurora_serious, is pleasant-alert and a little afraid, never blank. Happy is relief: a small closed smile, eyes still wide. Sad lowers the gaze and the mouth, no tears as costume. Surprised opens the mouth slightly with no teeth and widens the eyes. Blush is a soft cheek wash stacked on the current face, not a new pose. Hair silhouette outside the face box stays put. Do not chain expression edits. Extra legacy crops, aurora_gorgeous and aurora_smile_face, are not production ids. Do not treat them as the model. Staging light is a soft key from the upper front-left, cool fill from the right, thin cyan rim on the hair. The face gets the cleanest light. Face box locks on the first approved sprite and does not move. Do not invent pixel numbers before that file exists. Height is unconfirmed. Do not encode a fake scale. Legacy: Shipped aurora sprites are opaque-on-black drafts of a body-tight interface suit with see-through panels. They are not the production reference. Do not img2img from them. Do not chroma-key the black behind the shipped files; recover alpha from a white plate and a black plate after the design is approved. QA rejects: a sealed suit, a transparent panel over skin, wings, a bell skirt, a missing pane, text that is not the abstract code-scroll of the pane, cropped feet, a child-coded head ratio. The pane may suggest code. It must not contain readable words. A high-neck opaque tunic under a pale knee-length coat, with cyan circuit lines only on the cuffs, the collar, and the hem. #3EC8E6

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Aurora. Holographic subsystem of station Prometheus. Class: tall hair-bell column with a rectangular pane. Tall pale figure, long silver hair in a bell, a flat rectangle of light behind one shoulder. Eyes: wide cyan eyes, anxious, one catchlight. Hair: long straight silver hair, heavier at the ends. Defining feature: a vertical pane of scrolling station code standing just behind her shoulder.
WARDROBE: Interface coat. A high-neck pale coat traced with cyan circuits. Props: code pane.
COLOR: Main reactor cyan #3EC8E6. Hair #E7F4F8. Eyes #3EC8E6. Outer #F4FBFF. Accent #3EC8E6.
STAGING: Standing, weight even, arms a little away from the coat, feet together on the bottom edge. Neutral, alert, eyes to camera. Soft key from the upper front-left, cool fill, a thin cyan rim on the hair. Focal point: a vertical pane of scrolling station code standing just behind her shoulder, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: body-tight interface suit; see-through clothing panels; wardrobe accident; child-coded redesign.
```

## Canon card

- **Memory point:** a vertical pane of scrolling station code standing just behind her shoulder
- **Secondary hook:** cyan circuit traces that stop at the cuffs
- **Silhouette class:** tall hair-bell column with a rectangular pane
- **Masses:** big — the hair bell and the narrow coat column; mid — the rectangular code pane and the coat hem; small — circuit lines at collar, cuffs, and hem
- **Negative space:** the gap between the coat and the code pane, and the space under the hair bell
- **Main colour:** reactor cyan `#3EC8E6` — the station is still running
- **Default outfit:** interface coat
- **Alternate outfit:** lab shell
- **Shape majority:** square (0.3 circle / 0.5 square / 0.2 triangle)
- **Legacy:** Shipped aurora sprites are opaque-on-black drafts of a body-tight interface suit with see-through panels. They are not the production reference. Do not img2img from them.

### Colour zones

- **hair**: station silver `#E7F4F8`
- **skin**: cool fair `#F3D6CB`
- **eye**: reactor cyan `#3EC8E6`
- **tops1**: coat white `#F4FBFF`
- **tops2**: tunic `#D7EEF5`
- **waist**: soft belt `#8FB8C4`
- **bottom1**: coat hem `#E7F6FA`
- **bottom2**: tunic lower `#C5DDE6`
- **shoes**: station grey `#8AA4B0`
- **decoration1**: code pane `#3EC8E6`
- **decoration2**: circuit trim `#7EE7FF`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | aurora_serious, aurora_serious_face | serious and a little frightened, not blank |
| happy | aurora_happy | relief, not a grin |
| sad | aurora_sad, aurora_sad_face | afraid of being switched off |
| surprised | aurora_surprised, aurora_surprised_face | a glitch of alarm |
| blush | aurora_blush, aurora_blush_face | the current face plus a soft cheek wash |

### Banned in every prompt

- body-tight interface suit
- see-through clothing panels
- wardrobe accident
- child-coded redesign

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether the code pane is a second texture the engine can flicker.
- Whether aurora_gorgeous and the smile-face crop are retired or kept as extras.
