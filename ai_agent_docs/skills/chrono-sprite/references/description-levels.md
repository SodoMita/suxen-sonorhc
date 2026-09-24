# Description levels

The ladder is the first stage of the pipeline, not a preface. `ai_agent_docs/character_levels/README.md` is the contract. This file is how a sprite stage uses it.

## Source and render

Edit `ai_agent_docs/character_levels/src/<id>.json`. Then:

```bash
python3 tools/render_character_levels.py
python3 tools/render_character_levels.py --check
```

`--check` fails if a rendered page drifted, if a memory point is missing from a level, if L0 or L1 grew a hex code, if the production paragraph leaked downward, if two main colours sit too close, or if L4 exceeds 1500 characters.

## What each level is allowed to know

L0 can name the silhouette class, the main colour by words, and the memory point. It cannot specify a fastening.

L1 can name the outfit and say who this character is not. It still cannot carry a hex. Use it when a reference image is attached and the model will ignore a long prompt.

L2 can assign zones, hexes, both outfits at design-sentence length, and the nearest neighbour. Use it to review a design before spending a generation.

L3 can specify cloth, closures, expression geometry, the face-box rule, and what the shipped file actually shows. Use it as the acceptance criteria. Do not paste it into a generator. Truncation cuts the end, and the end is the exclusion block.

L4 is assembled. It states the memory point in IDENTITY and again in STAGING, puts a short exclusion line in FORMAT, and repeats the full exclusion at the end. If a picture misses a fact that lives only in L3, add that one fact to the next numbered prompt. Record why in the result note.

## Consistency rules the checker enforces

- The memory point string is identical in L0, L1, L2, L3, and twice in L4.
- The silhouette class string is in L0.
- The default outfit name is in L1, L2, and L3.
- The design sentence is in L2 and L3, and not in L0 or L1.
- The production paragraph is in L3 only.
- The legacy note is in L3, verbatim.
- Word counts strictly increase from L0 to L3.
- Main-colour hex distance between any two characters is at least 80.

If you need a fact the page does not have, ask. Do not fill it from the shipped PNG.
