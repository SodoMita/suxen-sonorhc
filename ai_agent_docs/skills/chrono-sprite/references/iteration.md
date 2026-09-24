# Iteration

One pass is the target, not the assumption. This file keeps a second pass from starting over.

## The rule

Every prompt you send is saved, with its result, before the session ends.

```
characters/<id>/prompts/
  00_level_sprite.txt          # rendered L4, verdict UNSENT, do not hand-edit
  01_sprite.txt                # exact text sent
  01_sprite.result.md
  02_plate_black.txt
  02_plate_black.result.md
  KEPT.md                      # which file produced the master
```

Numbering is one sequence per character, across stages, so the order is recoverable.

`00_level_sprite.txt` is overwritten by a re-render. It is not the record of a sent prompt. Copy into the next number when you send.

Rejected images go in `characters/<id>/_wip/` and are not committed. Prompts are committed.

## Result note

```markdown
# 01_sprite — aurora

Verdict: REJECT
Model: <name the model>
Kept: coat colour and the pane placement.
Broke: the pane became a window in the torso.
Change for next: one sentence, that the pane is a separate rectangle behind the shoulder.
```

Verdicts: `KEEP`, `KEEP WITH EDIT`, `REJECT`, `PARTIAL`, `UNSENT`.

If the rejection is a depiction failure, say so and do not describe the image.

## Loop

1. Send the saved prompt.
2. Write the result note before the next attempt.
3. Change exactly one thing.
4. Three attempts at a stage with no improvement: stop editing the prompt. Re-read L3. The design or the reference is wrong.
5. Five attempts total: stop and ask.
6. The same defect twice is not randomness. Rephrase the missing fact positively. Do not repeat the negative harder.

## Partial acceptance

A turnaround may have one usable view. Crop that view into `_wip/`, say so in the note, and do not pretend the other views were kept. Do not matte a partial.

## Promotion

When a prompt works, add its number to `KEPT.md` and copy any new fact that was not in the level source back into `src/<id>.json`, then re-render. A fact that lives only in a chat transcript will be lost, and the next session will spend generations relearning it.
