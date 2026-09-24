# Nova — description levels

`id: nova` · appeal track `cool` · approved: `false`

Age not locked. Lore has her as a junior researcher in youth. Appeal is the hood, the pane, and the competence of the kit. The jacket is a shield, not a costume slip.

Visual production spec for the sprite pipeline. Narrative lore stays in the files below; this page is what an image stage is allowed to know. Higher levels add detail. They must not contradict a lower level. Do not send L3 to a generator — send L4, which is assembled from the canon fields and checked against a character budget.

| Level | Words / chars | Read it when | Send it when |
|---|---|---|---|
| L0 Glance | 41 words | roster, thumbnail, silhouette test | never |
| L1 Card | 147 words | a reference image is already attached | never as a full prompt; quote one sentence |
| L2 Design | 281 words | design review, first composition without construction | never |
| L3 Production | 459 words | QA, garment construction, expression geometry | never |
| L4 Prompt | 1484 chars | you are about to generate the default sprite | yes, saved to `characters/nova/prompts/` first |

## Lore

- `docs/vn_specific/nova_lore.md`
- `docs/vn_specific/characters_starting/nova_origin.md`

## L0 Glance

Nova reads as a boxy oversized jacket with a floating pane offset to one side. Main colour is signal magenta, not cyan. Memory point: an oversized hood she does not put up, and a diagnostic pane held up like a shield.

## L1 Card

Nova is the quiet analyst, the one who hides in a jacket that is too big and holds a scanner like a shield. She reads as a boxy oversized jacket with a floating pane offset to one side: dark kit, hood down, a small pane in one hand. The thing a player names is an oversized hood she does not put up, and a diagnostic pane held up like a shield. Main colour is signal magenta, a streak in dark hair, against a grey jacket and a small cyan pane. The default costume is the analyst jacket, closed to the collar, hood down, trousers and boots. At rest she is reserved, not blank and not fierce. A second look finds the streak. She is not Aurora. Aurora's pane is a tall rectangle behind the body. Nova's pane is small and in the hand. The jacket stays closed.

## L2 Design

Nova is the box in the roster. Silhouette class: boxy oversized jacket with a floating pane offset to one side. Memory point: an oversized hood she does not put up, and a diagnostic pane held up like a shield. Big mass is the oversized jacket. Mid mass is the hood on the back and the held pane. Small mass is the magenta streak, the knee pads, and the boot lights. Negative space is the offset between body and pane. The pane must not touch the jacket, or it becomes Aurora's halo-pane. Eyes are cool and a little wary, one catchlight. Brow slightly drawn, not angry. Mouth closed and small. Hair is short and dark with one magenta streak. The body is a narrow build lost in the jacket. Main colour is signal magenta #E23E8C, and it means a person inside the equipment. It is reserved for the streak. Jacket is grey #3A4150. The pane is cyan #7EC8E0, smaller than a torso, and it is not her main colour. Default outfit is the analyst jacket. An oversized dark analyst jacket, hood down, worn closed over a high-neck layer, with cargo trousers, knee pads, boots, and a small diagnostic pane in one hand. The station-coat alternate is a fitted flashback coat with no pane, not the default. Nearest tech figure is Aurora. Separate them by mass and by where the light sits: Aurora is a pale column with a pane behind her; Nova is a dark box with a pane in her hand. In greyscale Nova is a mid-dark rectangle with a small light rectangle offset to one side. The hood bump on the back keeps her from reading as a plain coat.

## L3 Production

Production notes for Nova. The jacket is a psychological shield from the lore, drawn as volume, not as a slip off the shoulder. Both halves of the memory point must be visible: an oversized hood she does not put up, and a diagnostic pane held up like a shield. If the hood is up, the pane touches the body, or the streak is missing, reject. The pane hand is a gloved hand with five digits around the edge of the rectangle. The belt hand is also gloved, five digits, no weapon. Boots fully in frame, on the bottom edge. Knee pads are small and do not become armor plates. Default costume, the analyst jacket: At least one size too big: dropped shoulders, sleeves past the wrist, hem at the thigh, zip pulled to the collar. The hood sits on the back and is never up in the default sprite. Under it is a closed high-neck shirt, no gap at the waist. The diagnostic pane is a flat cyan rectangle held out at chest height, smaller than Aurora's pane and clearly in the hand. Zip to the collar. No waist gap. No crop layer. The hood's lining is a darker grey and is seen only because the hood is down. The pane may show abstract bars. It must not show readable words. It is smaller than her torso. The station-coat alternate is out of scope for the default set. Expressions are face-only. Neutral is reserved and watching. Happy is a tiny closed smile, the legacy nova_happy key. Sad and surprised have no legacy files yet. Sad raises the inner brow. Surprised widens the eyes and parts the lips, no teeth. Blush is a faint cheek wash. The hood and the pane do not move between expressions. Do not overwrite nova_neutral or nova_happy. Light is a cool lab key from the front-left. Cyan spill from the pane hits the glove only, not the whole face. Face box locks on the first approved master. The hood sits behind the head and stays outside the box. Age is not locked. Do not draw a child, and do not draw the fitted open suit from the shipped files. Height is unconfirmed. Legacy: Shipped nova sprites are a fitted tactical suit with the jacket open at the waist. They are not the production reference. Do not img2img from them. QA rejects: jacket open at the waist, a fitted tactical suit, hood up, a pane as tall as the body, readable UI text, a sword, cropped boots. She is the shield and the reading, not a costume malfunction. An oversized dark analyst jacket, hood down, worn closed over a high-neck layer, with cargo trousers, knee pads, boots, and a small diagnostic pane in one hand. #E23E8C

## L4 Prompt

Assembled by `tools/assemble_prompt.py`. Do not hand-edit. If a generation misses a fact that exists only in L3, add that one fact to the next numbered prompt and record why.

```text
FORMAT: Full-body VN sprite, one figure, tall portrait, flat mid-grey background, head to soles, margin. Early exclude: no text, no watermark, no extra limbs, no nudity, no transparent clothing, no sexual pose, fully clothed.
STYLE: Anime VN tachie, closed ink line, flat cel, two shadow steps.
IDENTITY: Nova. Quiet analyst who hides in an oversized jacket. Class: boxy oversized jacket with a floating pane offset to one side. Boxy dark jacket, hood down, small pane held out to one side. Eyes: cool eyes, a little wary, one catchlight. Hair: short dark hair with one magenta streak. Defining feature: an oversized hood she does not put up, and a diagnostic pane held up like a shield.
WARDROBE: Analyst jacket. An oversized closed jacket, hood down, pane in hand. Props: diagnostic pane.
COLOR: Main signal magenta #E23E8C. Hair #E23E8C. Eyes #7EC8E0. Outer #3A4150. Accent #7EC8E0.
STAGING: Standing, one hand holding the pane out, the other at the belt, feet together. Neutral, alert, eyes to camera. Cool lab key from the front-left, a little cyan spill from the pane onto the glove only. Focal point: an oversized hood she does not put up, and a diagnostic pane held up like a shield, fully visible.
EXCLUDE: No text, no watermark, no extra limbs, no cropped feet, no scenery, no nudity, no transparent clothing, no sexual pose, no child proportions, fully clothed, no living-artist name. Also: jacket open at the waist; fitted tactical suit; hood up; pane as tall as the body.
```

## Canon card

- **Memory point:** an oversized hood she does not put up, and a diagnostic pane held up like a shield
- **Secondary hook:** a magenta streak in otherwise dark hair
- **Silhouette class:** boxy oversized jacket with a floating pane offset to one side
- **Masses:** big — the oversized jacket box; mid — the hood mass on the back and the held pane; small — the magenta streak, knee pads, boot lights
- **Negative space:** the offset between the body and the pane, which must not touch the jacket
- **Main colour:** signal magenta `#E23E8C` — a person inside the equipment
- **Default outfit:** analyst jacket
- **Alternate outfit:** station coat
- **Shape majority:** square (0.1 circle / 0.7 square / 0.2 triangle)
- **Legacy:** Shipped nova sprites are a fitted tactical suit with the jacket open at the waist. They are not the production reference. Do not img2img from them.

### Colour zones

- **hair**: magenta streak `#E23E8C`
- **skin**: fair cool `#E8C2B4`
- **eye**: pane cyan `#7EC8E0`
- **tops1**: jacket grey `#3A4150`
- **tops2**: hood lining `#2A3038`
- **waist**: belt `#4A5560`
- **bottom1**: trouser grey `#2E3540`
- **bottom2**: knee pad `#3E4652`
- **shoes**: boot `#232830`
- **decoration1**: diagnostic pane `#7EC8E0`
- **decoration2**: hair streak `#E23E8C`

### Expression aliases

Production ids are the pipeline names. Legacy keys are what `scenes/vn_balloon.tscn` already wires. Do not overwrite a legacy file to "fix" the name.

| Production | Legacy keys | Read |
|---|---|---|
| neutral | nova_neutral | reserved and watching |
| happy | nova_happy | a private relief |
| sad | — | the shield failing |
| surprised | — | the reading jumped |
| blush | — | the current face plus a faint cheek wash |

### Banned in every prompt

- jacket open at the waist
- fitted tactical suit
- hood up
- pane as tall as the body

## Open questions

Unanswered. Do not invent answers for these. An unanswered question is not a gap to fill from genre habit.

- Exact height in centimetres, and whether the cast shares one world scale.
- Owner approval of this level doc. approved stays false until then.
- Which outfit is worn in which dialogue label.
- Whether the memory-point prop is a separate overlay or painted into the sprite.
- Whether sad, surprised, and blush masters are in the first production batch.
