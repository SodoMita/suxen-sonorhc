# Character description levels

This directory is the description stage of the sprite pipeline. Each speaking character has the same facts written at five densities. A later level adds detail. It does not change an earlier one.

Source of truth: `src/_shared.json` and `src/<id>.json`.
Rendered pages: `<id>.md`, plus `CAST_AT_A_GLANCE.md` for the whole roster at L0.
Regenerate with `python3 tools/render_character_levels.py`. Do not hand-edit the rendered files.

## Why more than one text

Image models weight the start of a prompt and drop the end when they hit a character limit. The end is where exclusions live. A production bible is the right thing to judge a picture against, and the wrong thing to paste into a generator.

So the pipeline refuses to treat "the character description" as one blob.

| Level | Size | What it is for | What it must not do |
|---|---|---|---|
| L0 Glance | 22–55 words | Roster table, 64px thumbnail test, "could I name them from a black fill?" | No hex, no garment construction |
| L1 Card | 70–160 words | A reference image is already attached and the prompt must stay short | No hex, no construction paragraph |
| L2 Design | 180–420 words | Design review, first composition | No fastening-level construction |
| L3 Production | 320–900 words | QA, cloth, face geometry, legacy drift | Never sent to a generator |
| L4 Prompt | 700–1500 characters | The default standing-sprite prompt | Does not paste L3. States the memory point twice. Exclusions appear at the start and the end |

Word counts must strictly increase from L0 to L3. L4 is a compression, assembled by `tools/assemble_prompt.py`, not a sixth prose draft.

## The same fact, climbing

Aurora's memory point is "a vertical pane of scrolling station code standing just behind her shoulder".

- **L0** names the pane and the silhouette class, and stops.
- **L1** adds who she is not (not the winged figure, not the priestess) and the coat's name.
- **L2** adds the negative space between coat and pane, the hex, and the nearest neighbours.
- **L3** says the pane is a separate rectangle, not a window in the body, and names the shipped suit as a non-reference.
- **L4** states the pane in IDENTITY and again in STAGING, and spends the remaining budget on exclusions.

If a generation misses a construction fact that exists only in L3, add that one fact to the next numbered prompt. Do not paste L3 in to "be safe".

## Open questions

Heights, scene-by-scene costume plots, and owner approval are unanswered. `approved` stays false. Do not invent those answers from genre habit or from a picture the generator just made.

Narrative lore stays in `docs/vn_specific/`. These pages are the visual spec.
